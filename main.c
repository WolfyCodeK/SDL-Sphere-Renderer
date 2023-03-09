#include<stdio.h>
#include<SDL2/SDL.h>

int main(int argc, char *argv[]) {

    SDL_Init(SDL_INIT_VIDEO);   

    SDL_Window* win;

    SDL_Renderer* renderer;

    int pixel;

    SDL_CreateWindowAndRenderer(600, 600, 0, &win, &renderer);

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

        pixel = SDL_RenderDrawPoint(renderer, 300, 300);

        if (pixel != 0) {
            printf("%s", SDL_GetError());
            return 1;
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyWindow(win);
    SDL_Quit();

    if (pixel == 0) {
        printf("\nCOLD");
    }

    return EXIT_SUCCESS;
}