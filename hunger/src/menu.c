#include "menu.h"
#include <stddef.h>
#include <stdio.h>

TTF_Font *MenuFont = NULL;

void hunger_menu_init(SDL_Window *window, SDL_Renderer *renderer)
{
   MenuFont = TTF_OpenFont("./assets/fonts/BoldPixels.ttf", 10);
   if(NULL == MenuFont)
   {
      printf("ERROR : Failed to load font\r\n");
   }
}

void hunger_menu_tick(SDL_Window *window, SDL_Renderer *renderer)
{
}
