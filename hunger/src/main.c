/**
 * @file 	main.c
 * @author 	tibbdev
 * @brief 	Hunger Game - A simple SDL3 application to simulate hunger levels.
 * @version 0.1
 * @date 	2025-07-30
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "SDL3/SDL.h"

#include "hunger.h"
#include "player.h"

#define MAX_HUNGER        1000.0f
#define FOOD_AMOUNT       42
#define HUNGER_BAR_HEIGHT 200

#define WINDOW_WIDTH  640
#define WINDOW_HEIGHT 480

SDL_Window   *window       = NULL;
SDL_Renderer *renderer     = NULL;
const char   *window_title = "Hunger Game";

int main(void)
{
   SDL_Init(SDL_INIT_VIDEO);
   SDL_CreateWindowAndRenderer(window_title, WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer);

   if(window == NULL || renderer == NULL)
   {
      fprintf(stderr, "Failed to create window or renderer: %s\n", SDL_GetError());
      return 1;
   }

   SDL_SetRenderVSync(renderer, true);

   bool      running = true;
   SDL_Event event;

   SDL_Time current_time;
   SDL_GetCurrentTime(&current_time);

   srand(current_time % 3600);

   Player player;
   player_init(&player, MAX_HUNGER);
   player_move_to(&player, 200, 200);

   SDL_FRect hunger_rect  = { 10, 10 + ((float)player.hunger.hunger_level / player.hunger.max_hunger) * HUNGER_BAR_HEIGHT, 10, HUNGER_BAR_HEIGHT };
   SDL_Color hunger_color = { 255, 255, 0, 255 }; // YELLOW color for hunger bar
   SDL_Color full_color   = { 255, 0, 0, 255 };   // Red color for full hunger

   uint32_t iter      = 0;
   uint32_t frame_cnt = 0;
   uint32_t fps       = 60;

   bool mv_lft = false;
   bool mv_rgt = false;
   bool mv_up  = false;
   bool mv_dn  = false;

   SDL_GetCurrentTime(&current_time);
   SDL_Time prev_time = current_time;

   while(running)
   {
      static float dt = 0.0f;
      static float dx = 0.0f;
      static float dy = 0.0f;

      SDL_GetCurrentTime(&current_time);

      dt = current_time - prev_time;
      dt *= 0.000000001;

      frame_cnt++;

      // Handle events
      while(SDL_PollEvent(&event))
      {
         if(event.type == SDL_EVENT_QUIT)
         {
            running = false;
         }
         else if(event.type == SDL_EVENT_KEY_DOWN)
         {
            switch(event.key.key)
            {
               case SDLK_ESCAPE:
                  running = false;
                  break;
               case SDLK_SPACE:
                  // Eat food when space is pressed
                  hunger_eat(&player.hunger, (rand() % FOOD_AMOUNT) + 1);
                  break;

               case SDLK_W:
               case SDLK_UP:
                  mv_up = true;
                  break;
               case SDLK_S:
               case SDLK_DOWN:
                  mv_dn = true;
                  break;

               case SDLK_A:
               case SDLK_LEFT:
                  mv_lft = true;
                  break;
               case SDLK_D:
               case SDLK_RIGHT:
                  mv_rgt = true;
                  break;
               default:
                  break;
            }
         }
         else if(event.type == SDL_EVENT_KEY_UP)
         {
            switch(event.key.key)
            {
               case SDLK_W:
               case SDLK_UP:
                  mv_up = false;
                  break;
               case SDLK_S:
               case SDLK_DOWN:
                  mv_dn = false;
                  break;

               case SDLK_A:
               case SDLK_LEFT:
                  mv_lft = false;
                  break;
               case SDLK_D:
               case SDLK_RIGHT:
                  mv_rgt = false;
                  break;
               default:
                  break;
            }
         }
      }

      if(mv_lft && !mv_rgt)
      {
         dx = -1;
      }
      else if(!mv_lft && mv_rgt)
      {
         dx = 1;
      }
      else
      {
         dx = 0;
      }

      if(mv_up && !mv_dn)
      {
         dy = -1;
      }
      else if(!mv_up && mv_dn)
      {
         dy = 1;
      }
      else
      {
         dy = 0;
      }

      if((dx < 0) && (player.x <= 0.5f * player.size))
      {
         dx       = 0;
         player.x = 0.5f * player.size;
      }
      else if((dx > 0) && (player.x >= 400 - (0.5f * player.size)))
      {
         dx       = 0;
         player.x = 400 - (0.5f * player.size);
      }

      if((dy < 0) && (player.y <= 0.5f * player.size))
      {
         dy       = 0;
         player.y = 0.5f * player.size;
      }
      else if((dy > 0) && (player.y >= 400 - (0.5f * player.size)))
      {
         dy       = 0;
         player.y = 400 - (0.5f * player.size);
      }

      // Update player state
      player_move(&player, dt, dx, dy);
      player_update(&player, dt); // Update player with a fixed delta time

      // Update hunger bar position and color
      hunger_rect.y = (WINDOW_HEIGHT - 10) - ((float)player.hunger.hunger_level / player.hunger.max_hunger) * HUNGER_BAR_HEIGHT;
      hunger_rect.h = ((float)player.hunger.hunger_level / player.hunger.max_hunger) * HUNGER_BAR_HEIGHT;

      if(player.state == PLAYER_DEAD)
      {
         // If the player is dead, set the hunger bar to BLACK
         hunger_color = (SDL_Color) { 0, 0, 0, 255 }; // BLACK for dead state
      }
      else if(player.hunger_state == PLAYER_HUNGER_STARVED)
      {
         // If the player is starved, set the hunger bar to dark red
         hunger_color = (SDL_Color) { 165, 0, 0, 255 }; // Dark color for starved state
      }
      else if(player.hunger_state == PLAYER_HUNGER_STARVING)
      {
         // If the player is starving, set the hunger bar to red
         hunger_color = (SDL_Color) { 255, 0, 0, 255 }; // Red color for hungry state
      }
      else if(player.hunger_state == PLAYER_HUNGER_HUNGRY)
      {
         // If the player is hungry, set the hunger bar to orange
         hunger_color = (SDL_Color) { 255, 165, 0, 255 }; // Orange color for hungry state
      }
      else
      {
         // If the player is okay, set the hunger bar to green
         hunger_color = (SDL_Color) { 0, 255, 0, 255 }; // Green color for okay state
      }

      player_display(&player); // Display player state in console

      // Render the window
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Clear with black
      SDL_RenderClear(renderer);

      SDL_FRect wrld_rect = { 120, 40, 400, 400 };

      SDL_SetRenderDrawColor(renderer, 0, 55, 55, 255);
      SDL_RenderFillRect(renderer, &wrld_rect);

      SDL_FRect plyr_rect;
      plyr_rect.h = player.size;
      plyr_rect.w = player.size;
      plyr_rect.x = wrld_rect.x + player.x - (player.size * 0.5f);
      plyr_rect.y = wrld_rect.y + player.y - (player.size * 0.5f);

      // Draw the hunger bar
      SDL_SetRenderDrawColor(renderer, hunger_color.r, hunger_color.g, hunger_color.b, hunger_color.a);
      SDL_RenderFillRect(renderer, &hunger_rect);
      SDL_RenderFillRect(renderer, &plyr_rect);

      // Draw the hunger bar outline
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200);
      SDL_RenderRect(renderer, &hunger_rect);
      SDL_RenderRect(renderer, &wrld_rect);
      SDL_RenderPresent(renderer);

      if(current_time / 1000000000 != prev_time / 1000000000)
      {
         char window_fps_title[64];
         snprintf(window_fps_title, sizeof(window_fps_title), "%s - FPS := %u", window_title, frame_cnt);

         // Update the window title with the current frame count
         SDL_SetWindowTitle(window, window_fps_title);
         fps       = frame_cnt;
         frame_cnt = 0;
      }

      prev_time = current_time;
   }
   return 0;
}
