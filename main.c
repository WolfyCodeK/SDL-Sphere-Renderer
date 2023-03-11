#include<stdio.h>
#include<math.h>
#include<SDL2/SDL.h>

#define winWidth 600
#define winHeight 600

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
    SDL_Init(SDL_INIT_VIDEO);   

    SDL_Window* win;

    SDL_Renderer* renderer;

    SDL_CreateWindowAndRenderer(winWidth, winHeight, 0, &win, &renderer);
    SDL_SetWindowTitle(win, "SDL PROJECT");

    int renderLoopActive = 1;

    struct inputs userInputs = {0, 0, 0, 0};

    struct vector sphereColour = {230, 40, 150};

    double radius = 75.0;

    double ambientStrength = 0.2;

    struct vector direction = {0, 0, -1};

    struct vector centerSphere = {0, 0, 0};

    struct vector light = {100, 100, 1000};

    int time = 0;

    while (renderLoopActive) {
        Uint64 start = SDL_GetPerformanceCounter();

        SDL_Event winEvent;

        while (SDL_PollEvent(&winEvent)) {
          switch(winEvent.type) {
            case SDL_QUIT: {
              renderLoopActive = 0;
              break;
            }

            case SDL_KEYDOWN: {
              switch (winEvent.key.keysym.scancode) {
                case SDL_SCANCODE_W: {
                  userInputs.up = 1;
                  break;
                }

                case SDL_SCANCODE_D: {
                  userInputs.right = 1;
                  break;
                }

                case SDL_SCANCODE_S: {
                  userInputs.down = 1;
                  break;
                }
                
                case SDL_SCANCODE_A: {
                  userInputs.left = 1;
                  break;
                }

                break;
              }

              break;
            }

            case SDL_KEYUP: {
              switch (winEvent.key.keysym.scancode) {
                case SDL_SCANCODE_W: {
                  userInputs.up = 0;
                  break;
                }

                case SDL_SCANCODE_D: {
                  userInputs.right = 0;
                  break;
                }

                case SDL_SCANCODE_S: {
                  userInputs.down = 0;
                  break;
                }
                
                case SDL_SCANCODE_A: {
                  userInputs.left = 0;
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

        int moveSpeed = 6;

        centerSphere.x += (userInputs.right - userInputs.left) * moveSpeed;
        centerSphere.y += (userInputs.down - userInputs.up) * moveSpeed;

        int renderLoopAmount = 3;

        for (int j = 0; j < winHeight; j += renderLoopAmount) {
            for (int i = 0; i < winWidth; i += renderLoopAmount) {
              double v = -winHeight / 2 + j;
              double u = -winWidth / 2 + i;

              struct vector origin = {u, v, 400};

              struct vector centerOfSphereToOrigin = sub(origin, centerSphere);

              double a = dot(direction, direction);
              double b = 2 * dot(centerOfSphereToOrigin, direction);
              double c = dot(centerOfSphereToOrigin, centerOfSphereToOrigin) - (radius * radius);

              double discriminant = b * b - (4 * a * c);

              double intersection = (- b - sqrt(discriminant)) / 2 * a;
              struct vector points = add(origin, (mul(direction, intersection)));

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
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
              }

              SDL_RenderDrawPoint(renderer, i, j);

              if ((i < winWidth - 1) && (j < winHeight - 1)) {
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

        if (time > 10) {
          printf("fps: %d\n", (int) round(1 / elapsed));
          time = 0;
        } else {
          time++;
        }

        SDL_RenderPresent(renderer); 
    }

    SDL_DestroyWindow(win);
    SDL_Quit();

    return EXIT_SUCCESS;
}