#include<stdio.h>
#include<math.h>
#include<SDL2/SDL.h>

#define winWidth 600
#define winHeight 600

struct vector {
  float x;
  float y;
  float z;
};

float magnitude(double x, double y, double z) {
    return sqrt(x * x + y * y + z * z);
}

void normalise(double x, double y, double z) {
    double mag = magnitude(x, y, z);

    if (mag != 0) {
      x /= mag;
      y /= mag;
      z /= mag;
    }
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

struct vector mul(struct vector v1, float n) {
    struct vector v = {n * v1.x, n * v1.y, n * v1.z};

    return v;
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

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        /*  
        SDL_RenderDrawLine(renderer, winWidth / 2, winHeight / 2, 0, 0);
        SDL_RenderDrawLine(renderer, winWidth / 2, winHeight / 2, winWidth, 0);
        SDL_RenderDrawLine(renderer, winWidth / 2, winHeight / 2, 0, winHeight);
        SDL_RenderDrawLine(renderer, winWidth / 2, winHeight / 2, winWidth, winHeight);
        */

        

        struct vector centerSphere = {0, 0, 0};

        

        for (int j = 0; j < winHeight; j++) {
            for (int i = 0; i < winWidth; i++) {

            }
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyWindow(win);
    SDL_Quit();

    return EXIT_SUCCESS;
}