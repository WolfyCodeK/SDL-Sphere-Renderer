#include<stdio.h>
#include<SDL2/SDL.h>

int main(int argc, char *argv[]) {

    const int winWidth = 600;
    const int winHeight = 600;

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

        SDL_RenderDrawLine(renderer, winWidth / 2, winHeight / 2, winWidth, winHeight);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyWindow(win);
    SDL_Quit();

    return EXIT_SUCCESS;
}