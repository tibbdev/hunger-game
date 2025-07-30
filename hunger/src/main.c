#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "hunger.h"

#include "SDL3/SDL.h"

#define MAX_HUNGER  2000
#define FOOD_AMOUNT 42

#define WINDOW_WIDTH  640
#define WINDOW_HEIGHT 480

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

int main(void) 
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer("Hunger Game", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer);

    if (window == NULL || renderer == NULL) {
        fprintf(stderr, "Failed to create window or renderer: %s\n", SDL_GetError());
        return 1;
    }

    SDL_SetRenderVSync(renderer, true);

    bool running = true;
    SDL_Event event;

    time_t current_time = time(NULL);
    time_t prev_time = current_time;

    srand(current_time % 3600 );

    // Initialize hunger structure
    Hunger hunger;

    hunger_init(&hunger, MAX_HUNGER);
    hunger_display(&hunger);

    SDL_FRect hunger_rect = {10, 10 + ((float)hunger.hunger_level / hunger.max_hunger)*(WINDOW_HEIGHT - 20), 20, WINDOW_HEIGHT - 10};
    SDL_Color hunger_color = {255, 255, 0, 255}; // YELLOW color for hunger bar
    SDL_Color full_color = {255, 0, 0, 255}; // Red color for full hunger

    uint32_t iter = 0;

    while (running)
    {
        // Handle events
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
            else if (event.type == SDL_EVENT_KEY_DOWN)
            {
                switch (event.key.key)
                {
                    case SDLK_ESCAPE:
                        running = false;
                        break;
                    case SDLK_SPACE:
                        // Eat food when space is pressed
                        hunger_eat(&hunger, (rand() % FOOD_AMOUNT) + 1);
                        break;
                    default:
                        break;
                }
            }
        }

        if (hunger_is_full(&hunger))
        {
            SDL_SetRenderDrawColor(renderer, full_color.r, full_color.g, full_color.b, full_color.a);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, hunger_color.r, hunger_color.g, hunger_color.b, hunger_color.a);
        }

        hunger_rect.x = 10;
        hunger_rect.w = 10;
        hunger_rect.y = (WINDOW_HEIGHT - 10) - ((float)hunger.hunger_level / hunger.max_hunger)*(WINDOW_HEIGHT - 20);
        hunger_rect.h = ((float)hunger.hunger_level / hunger.max_hunger)*(WINDOW_HEIGHT - 20);

        // Simulate hunger increase over time
        hunger_simulate(&hunger, 1);

        // Render the window
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Clear with black
        SDL_RenderClear(renderer);

        if (hunger_is_full(&hunger))
        {
            SDL_SetRenderDrawColor(renderer, full_color.r, full_color.g, full_color.b, full_color.a);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, hunger_color.r, hunger_color.g, hunger_color.b, hunger_color.a);
        }

        // Draw the hunger bar
        SDL_RenderFillRect(renderer, &hunger_rect);

        // Draw the hunger bar outline
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200);
        SDL_RenderRect(renderer, &hunger_rect);

        SDL_RenderPresent(renderer);
    }
    return 0;
}
