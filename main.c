#include<stdio.h>
#include<math.h>
#include<SDL2/SDL.h>

#define WIN_WIDTH 600
#define WIN_HEIGHT 600
#define CAMERA_DISTANCE_SCALAR 1
#define FOCAL_LENGTH (600 * CAMERA_DISTANCE_SCALAR)
#define CONTROLLER_DEADZONE (SDL_MAX_SINT16 / 3)

struct vector {
  double x;
  double y;
  double z;
};

struct inputs {
  int up;
  int down;
  int right;
  int left;
};

double magnitude(struct vector v1) {
  return sqrt(v1.x * v1.x + v1.y * v1.y + v1.z * v1.z);
}

struct vector normalise(struct vector v1) {
  double mag = magnitude(v1);

  if (mag != 0) {
    v1.x /= mag;
    v1.y /= mag;
    v1.z /= mag;
  }

  struct vector v = {v1.x, v1.y, v1.z};

  return v;
}

struct vector cross(struct vector v1, struct vector v2) {
  struct vector v = {
    (v1.y * v2.z) - (v1.z * v2.y),
    (v1.z * v2.x) - (v1.x * v2.z),
    (v1.x * v2.y) - (v1.y * v2.x)
  };

  return v;
}

double dot(struct vector v1, struct vector v2) {
  return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

struct vector sub(struct vector v1, struct vector v2) {
  struct vector v = {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};

  return v;
}

struct vector add(struct vector v1, struct vector v2) {
  struct vector v = {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};

  return v;
}

struct vector mul(struct vector v1, double n) {
  struct vector v = {n * v1.x, n * v1.y, n * v1.z};

  return v;
}

double clampAsDouble(double n) {
  if (n < 0) {
    n = 0;
  } else if (n > 1) {
    n = 1;
  }

  return n;
}

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);   

    SDL_GameController* controller = SDL_GameControllerOpen(0);

    printf("%s Connected!\n", SDL_GameControllerName(controller));

    SDL_Window* win;

    SDL_Renderer* renderer;

    SDL_CreateWindowAndRenderer(WIN_WIDTH, WIN_HEIGHT, 0, &win, &renderer);
    SDL_SetWindowTitle(win, "SDL PROJECT");

    int renderLoopActive = 1;

    struct inputs moveInputs = {0, 0, 0, 0};

    struct inputs lookInputs = {0, 0, 0, 0};

    struct vector sphereColour = {230, 40, 150};

    double radius = 75.0;

    double ambientStrength = 0.2;

    struct vector centerSphere = {0, 0, 0};

    struct vector light = {100, 100, 1000};

    struct vector origin = {0, 0, 0};

    struct vector VRP = {0, 0, FOCAL_LENGTH};

    struct vector VPN = {0, 0, -1};

    float yaw = -90;
    float pitch = 0;

    int time = 0;

    struct vector upApproximation = {0, 1, 0};

    while (renderLoopActive) {
        Uint64 start = SDL_GetPerformanceCounter();

        SDL_Event event;

        while (SDL_PollEvent(&event)) {
          switch(event.type) {
            case SDL_QUIT: {
              renderLoopActive = 0;
              break;
            }

            case SDL_CONTROLLERAXISMOTION: {
              Sint16 axisValue = event.caxis.value;

              if (event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX) {
                if (axisValue > CONTROLLER_DEADZONE) {
                  moveInputs.right = 1;
                } else {
                  moveInputs.right = 0;
                }
                
                if (axisValue < -CONTROLLER_DEADZONE) {
                  moveInputs.left = 1;
                } else {
                  moveInputs.left = 0;
                }
              }

              if (event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY) {
                if (axisValue > CONTROLLER_DEADZONE) {
                  moveInputs.down = 1;
                } else {
                  moveInputs.down = 0;
                }
                
                if (axisValue < -CONTROLLER_DEADZONE) {
                  moveInputs.up = 1;
                } else {
                  moveInputs.up = 0;
                }
              }

              if (event.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTX) {
                if (axisValue > CONTROLLER_DEADZONE) {
                  lookInputs.right = 1;
                } else {
                  lookInputs.right = 0;
                }
                
                if (axisValue < -CONTROLLER_DEADZONE) {
                  lookInputs.left = 1;
                } else {
                  lookInputs.left = 0;
                }
              }
            }

            case SDL_KEYDOWN: {
              switch (event.key.keysym.scancode) {
                case SDL_SCANCODE_W: {
                  moveInputs.up = 1;
                  break;
                }

                case SDL_SCANCODE_D: {
                  moveInputs.right = 1;
                  break;
                }

                case SDL_SCANCODE_S: {
                  moveInputs.down = 1;
                  break;
                }
                
                case SDL_SCANCODE_A: {
                  moveInputs.left = 1;
                  break;
                }

                break;
              }

              break;
            }

            case SDL_KEYUP: {
              switch (event.key.keysym.scancode) {
                case SDL_SCANCODE_W: {
                  moveInputs.up = 0;
                  break;
                }

                case SDL_SCANCODE_D: {
                  moveInputs.right = 0;
                  break;
                }

                case SDL_SCANCODE_S: {
                  moveInputs.down = 0;
                  break;
                }
                
                case SDL_SCANCODE_A: {
                  moveInputs.left = 0;
                  break;
                }
                
                break;
              }

              break;
            }

            break;
          }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        SDL_RenderClear(renderer);

        float moveSpeed = 6.0;

        int renderLoopAmount = 3;

        yaw += (lookInputs.right - lookInputs.left) * 3;

        VPN.x = cos(yaw * (M_PI / 180)) * cos(pitch * (M_PI / 180));
        VPN.y = sin(pitch * (M_PI / 180));
        VPN.z = sin(yaw * (M_PI / 180)) * cos(pitch * (M_PI / 180));
        VPN = normalise(VPN);

        struct vector VRV = cross(VPN, upApproximation);
        VRV = normalise(VRV);

        struct vector VUV = cross(VRV, VPN);
        VUV = normalise(VUV);

        struct vector move = {
          (moveInputs.right - moveInputs.left) * moveSpeed,
          0,
          (moveInputs.up - moveInputs.down) * moveSpeed
        };

        VRP.x += (move.x * VRV.x) + (move.z * VPN.x);
        VRP.z += (move.x * VRV.z) + (move.z * VPN.z);

        printf("Yaw: %f\n", yaw);
        printf("VRP: %f %f %f\n", VRP.x, VRP.y, VRP.z);
        printf("VRV: %f %f %f\n", VRV.x, VRV.y, VRV.z);
        printf("VPN: %f %f %f\n", VPN.x, VPN.y, VPN.z);

        for (int j = 0; j < WIN_HEIGHT; j += renderLoopAmount) {
            for (int i = 0; i < WIN_WIDTH; i += renderLoopAmount) {
              double v = -WIN_HEIGHT / 2 + j;
              double u = -WIN_WIDTH / 2 + i;

              struct vector direction = add(mul(VPN, FOCAL_LENGTH), add(mul(VUV, v), mul(VRV, u)));
              direction = normalise(direction);
       
              struct vector centerOfSphereToOrigin = sub(VRP, centerSphere);

              double a = dot(direction, direction);
              double b = 2 * dot(centerOfSphereToOrigin, direction);
              double c = dot(centerOfSphereToOrigin, centerOfSphereToOrigin) - (radius * radius);

              double discriminant = b * b - (4 * a * c);

              double intersection = (- b - sqrt(discriminant)) / 2 * a;
              struct vector points = add(VRP, (mul(direction, intersection)));

/*
              if ((v == 0) && (u ==0)) {
                printf("a: %lf \n", a);
                printf("direction: %f %f %f\n", direction.x, direction.y, direction.z);
                printf("centerOfSphereToOrigin: %f %f %f\n", centerOfSphereToOrigin.x, centerOfSphereToOrigin.y, centerOfSphereToOrigin.z);
                printf("points: %f %f %f\n", points.x, points.y, points.z);
              }
            */

              struct vector lightSource = sub(light, points);
              lightSource = normalise(lightSource);

              struct vector surfaceNormal = sub(points, centerSphere);
              surfaceNormal = normalise(surfaceNormal);

              // Calculating the angle that the lights hits the surface of the sphere
              double cosTheta = dot(lightSource, surfaceNormal);

              SDL_SetRenderDrawColor(
                renderer,
                (sphereColour.x * clampAsDouble(cosTheta + ambientStrength)),
                (sphereColour.y * clampAsDouble(cosTheta + ambientStrength)),
                (sphereColour.z * clampAsDouble(cosTheta + ambientStrength)), 
                255
              ); 

              if ((cosTheta < 0) && (discriminant >= 0)) {
                SDL_SetRenderDrawColor(
                  renderer, 
                  sphereColour.x * ambientStrength,
                  sphereColour.y * ambientStrength, 
                  sphereColour.z * ambientStrength, 
                  255
                ); 

              } else if (discriminant < 0) {
                SDL_SetRenderDrawColor(renderer, 135, 207, 235, 255);
              }

              SDL_RenderDrawPoint(renderer, i, j);

              if ((i < WIN_WIDTH - 1) && (j < WIN_HEIGHT - 1)) {
                SDL_RenderDrawPoint(renderer, i + 1, j);
                SDL_RenderDrawPoint(renderer, i, j + 1);
                SDL_RenderDrawPoint(renderer, i + 1, j + 1);

                SDL_RenderDrawPoint(renderer, i + 2, j);
                SDL_RenderDrawPoint(renderer, i, j + 2);
                SDL_RenderDrawPoint(renderer, i + 2, j + 2);

                SDL_RenderDrawPoint(renderer, i + 2, j + 1);
                SDL_RenderDrawPoint(renderer, i + 1, j + 2);  
              }     
            }
        }

        Uint64 end = SDL_GetPerformanceCounter();
        float elapsed = (end - start) / (float)SDL_GetPerformanceFrequency();

        if (time > 30) {
          printf("fps: %d\n", (int) round(1 / elapsed));
          time = 0;
        } else {
          time++;
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        int crosshairSize = 10;

        SDL_RenderDrawLine(renderer, WIN_WIDTH / 2 - crosshairSize, WIN_HEIGHT / 2,
          WIN_WIDTH / 2 + crosshairSize, WIN_HEIGHT / 2);

        SDL_RenderDrawLine(renderer, WIN_WIDTH / 2, WIN_HEIGHT / 2 - crosshairSize,
          WIN_WIDTH / 2, WIN_HEIGHT / 2 + crosshairSize);

        SDL_RenderDrawLine(renderer, WIN_WIDTH / 2 - crosshairSize, WIN_HEIGHT / 2 - 1,
          WIN_WIDTH / 2 + crosshairSize, WIN_HEIGHT / 2 - 1);

        SDL_RenderDrawLine(renderer, WIN_WIDTH / 2 - 1, WIN_HEIGHT / 2 - crosshairSize,
          WIN_WIDTH / 2 - 1, WIN_HEIGHT / 2 + crosshairSize); 

        SDL_RenderDrawLine(renderer, WIN_WIDTH / 2 - crosshairSize, WIN_HEIGHT / 2 + 1,
          WIN_WIDTH / 2 + crosshairSize, WIN_HEIGHT / 2 + 1);

        SDL_RenderDrawLine(renderer, WIN_WIDTH / 2 + 1, WIN_HEIGHT / 2 - crosshairSize,
          WIN_WIDTH / 2 + 1, WIN_HEIGHT / 2 + crosshairSize); 


        SDL_RenderPresent(renderer); 
    }

    if (controller != NULL) {
      SDL_GameControllerClose(controller);
    }

    SDL_DestroyWindow(win);
    SDL_Quit();

    return EXIT_SUCCESS;
}