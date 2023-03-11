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

    while (1) {
        SDL_Event winEvent;

        if (SDL_PollEvent(&winEvent)) {
            if (SDL_QUIT == winEvent.type) {
                break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        SDL_RenderClear(renderer);

        struct vector sphereColour = {230, 40, 150};

        double radius = 75.0;

        double ambientStrength = 0.2;

        struct vector direction = {0, 0, -1};

        struct vector centerSphere = {0, 0, 0};

        struct vector light = {100, 100, 1000};

        for (int j = 0; j < winHeight; j++) {
            for (int i = 0; i < winWidth; i++) {
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

              //printf("%f %f %f\n", points.x, points.y, points.z);

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

                //printf("%f\n", intersection);
              } else if (discriminant < 0) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
              }

              SDL_RenderDrawPoint(renderer, i, j);
            }
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyWindow(win);
    SDL_Quit();

    return EXIT_SUCCESS;
}