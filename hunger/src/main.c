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
#include "SDL3/SDL_joystick.h"

#include "hunger.h"
#include "player.h"
#include "food.h"
#include "collisions.h"

#define FOOD_QUANTITY_MAX               32
#define FOOD_QUANTITY_MIN               1
#define FOOD_QUANTITY_REDUCER           1
#define FOOD_QUANTITY_REDUCE_AFTER_secs 4.0f

#define MAX_HUNGER        1000.0f
#define HUNGER_BAR_HEIGHT 200

#define WINDOW_WIDTH  960
#define WINDOW_HEIGHT 540

#define WORLD_WIDTH  480
#define WORLD_HEIGHT 480

SDL_Window   *window       = NULL;
SDL_Renderer *renderer     = NULL;
const char   *window_title = "Hunger Game";

SDL_FRect wrld_rect = { (WINDOW_WIDTH >> 1) - (WORLD_WIDTH >> 1), (WINDOW_HEIGHT >> 1) - (WORLD_HEIGHT >> 1), WORLD_WIDTH, WORLD_HEIGHT };

void draw_hunger_bar(SDL_Renderer *renderer, SDL_FRect *hunger_rect, SDL_Color hunger_color)
{
   SDL_SetRenderDrawColor(renderer, hunger_color.r, hunger_color.g, hunger_color.b, hunger_color.a);
   SDL_RenderFillRect(renderer, hunger_rect);
   SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200); // White color for outline
   SDL_RenderRect(renderer, hunger_rect);
}

void draw_vertical_bar64(SDL_Renderer *renderer, SDL_FRect *reducer_bar_rect, SDL_Color reducer_bar_color, float progress)
{
   progress = SDL_roundf(progress * 64) / 64;

   if(progress > 1.0)
   {
      progress = 1.0f;
   }

   // draw black background
   SDL_SetRenderDrawColor(renderer, 0, 0, 0, UINT8_MAX);
   SDL_RenderFillRect(renderer, reducer_bar_rect);

   // fill the bar
   SDL_FRect bar_rect = { .h = reducer_bar_rect->h * progress, .w = reducer_bar_rect->w, .x = reducer_bar_rect->x, .y = reducer_bar_rect->y + (reducer_bar_rect->h * (1 - progress)) };
   SDL_SetRenderDrawColor(renderer, reducer_bar_color.r, reducer_bar_color.g, reducer_bar_color.b, reducer_bar_color.a);
   SDL_RenderFillRect(renderer, &bar_rect);

   // draw black gradation lines
   bar_rect.h   = 2;
   uint16_t cnt = 0;
   do
   {
      bar_rect.y = ((cnt + 1) * 12.8) - (bar_rect.h * 0.5f) + reducer_bar_rect->y;
      SDL_SetRenderDrawColor(renderer, UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX >> 1);
      SDL_RenderFillRect(renderer, &bar_rect);
      cnt++;
   } while(((cnt + 1) * 12.8) < (uint16_t)(reducer_bar_rect->h));

   // draw the outline
   SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200); // White color for outline
   SDL_RenderRect(renderer, reducer_bar_rect);
}

void draw_player(SDL_Renderer *renderer, Player *player, SDL_FRect *wrld_rect)
{
   SDL_Color plyr_clr = { 255, 255, 0, 255 }; // Default color for hunger bar (yellow)
   if(player->state == PLAYER_DEAD)
   {
      // If the player is dead, set the hunger bar to Super Dark Red
      plyr_clr = (SDL_Color) { 69, 7, 9, 255 }; // Super Dark Red for dead state
   }
   else if(player->hunger_state == PLAYER_HUNGER_STARVED)
   {
      // If the player is starved, set the hunger bar to dark red
      plyr_clr = (SDL_Color) { 159, 7, 18, 255 }; // Dark color for starved state
   }
   else if(player->hunger_state == PLAYER_HUNGER_STARVING)
   {
      // If the player is starving, set the hunger bar to red
      plyr_clr = (SDL_Color) { 251, 33, 21, 255 }; // Red color for hungry state
   }
   else if(player->hunger_state == PLAYER_HUNGER_HUNGRY)
   {
      // If the player is hungry, set the hunger bar to orange
      plyr_clr = (SDL_Color) { 254, 154, 55, 255 }; // Orange color for hungry state
   }
   else
   {
      // If the player is okay, set the hunger bar to green
      plyr_clr = (SDL_Color) { 187, 244, 81, 255 }; // Green color for okay state
   }

   SDL_FRect plyr_rect;
   plyr_rect.h = player->size;
   plyr_rect.w = player->size;
   plyr_rect.x = wrld_rect->x + player->x - (player->size * 0.5f);
   plyr_rect.y = wrld_rect->y + player->y - (player->size * 0.5f);

   float player_hunger_bar_y   = wrld_rect->y + player->y - (player->size * 0.5f) - 3.0f;                                                 // Position the hunger bar above the player
   float player_hunger_bar_x0  = wrld_rect->x + player->x - (player->size * 0.5f) - 2.0f;                                                 // Center the hunger bar above the player
   float player_hunger_bar_x1  = wrld_rect->x + (player->x + (player->size * 0.5f) + 1.0f);                                               // Center the hunger bar above the player
   float player_hunger_bar_len = (player_hunger_bar_x1 - player_hunger_bar_x0) * player->hunger.hunger_level / player->hunger.max_hunger; // Calculate the length of the hunger bar based on the player's hunger level

   float player_hunger_bar_x2 = player_hunger_bar_x0 + player_hunger_bar_len; // Center the hunger bar above the player

   SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black color for hunger bar background
   SDL_RenderLine(renderer, player_hunger_bar_x0, player_hunger_bar_y, player_hunger_bar_x1, player_hunger_bar_y);

   SDL_SetRenderDrawColor(renderer, plyr_clr.r, plyr_clr.g, plyr_clr.b, plyr_clr.a);
   SDL_RenderLine(renderer, player_hunger_bar_x0, player_hunger_bar_y, player_hunger_bar_x2, player_hunger_bar_y);

   SDL_RenderFillRect(renderer, &plyr_rect);

   SDL_SetRenderDrawColor(renderer, plyr_clr.r * 0.75f, plyr_clr.g * 0.75f, plyr_clr.b * 0.75f, plyr_clr.a);
   SDL_RenderRect(renderer, &plyr_rect);
}

void draw_food(SDL_Renderer *renderer, Food *food, uint8_t food_count, SDL_FRect *wrld_rect)
{
   if(food == NULL || food_count == 0)
   {
      return; // No food to draw
   }

   if(wrld_rect == NULL)
   {
      return; // No world rectangle to draw food in
   }

   for(uint8_t i = 0; i < food_count; i++)
   {
      SDL_SetRenderDrawColor(renderer, 240, 128, 128, 255); // Salmon color for food
      SDL_FRect food_rect;
      food_rect.h = food[i].size;
      food_rect.w = food[i].size;
      food_rect.x = wrld_rect->x + food[i].x - (food[i].size * 0.5f);
      food_rect.y = wrld_rect->y + food[i].y - (food[i].size * 0.5f);
      SDL_RenderFillRect(renderer, &food_rect);
   }
}

void draw_food_count(SDL_Renderer *renderer, uint8_t food_count, uint32_t x, uint16_t y, uint16_t qty_per_row)
{
   SDL_FRect food_rect;
   food_rect.w = 6.0f;
   food_rect.h = food_rect.w;

   for(uint16_t idx = 0; idx < FOOD_QUANTITY_MAX; idx++)
   {
      if(idx < food_count)
      {
         SDL_SetRenderDrawColor(renderer, 240, 128, 128, 255); // Salmon colour
      }
      else
      {
         SDL_SetRenderDrawColor(renderer, 60, 32, 32, 255); // Super-Dark Salmon colour
      }

      food_rect.x = x + (idx % qty_per_row) * food_rect.w * 1.5f;
      food_rect.y = y + (idx / qty_per_row) * food_rect.w * 1.5f;

      SDL_RenderFillRect(renderer, &food_rect);
   }
}

void draw_world(SDL_Renderer *renderer, SDL_FRect *wrld_rect)
{
   SDL_SetRenderDrawColor(renderer, 3, 79, 59, 255); // Dark color for world
   SDL_RenderFillRect(renderer, wrld_rect);
   SDL_SetRenderDrawColor(renderer, 240, 253, 244, 255); // Dark color for world
   SDL_RenderRect(renderer, wrld_rect);
}

int main(void)
{
   SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD);

   SDL_CreateWindowAndRenderer(window_title, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_FULLSCREEN, &window, &renderer);

   if(window == NULL || renderer == NULL)
   {
      printf("Failed to create window or renderer: %s\n", SDL_GetError());
      return 1;
   }

   SDL_SetRenderVSync(renderer, true);
   SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

   int window_width  = WINDOW_WIDTH;
   int window_height = WINDOW_HEIGHT;
   SDL_GetWindowSize(window, &window_width, &window_height);

   float scale_x = (float)window_width / (float)WINDOW_WIDTH;
   float scale_y = (float)window_height / (float)WINDOW_HEIGHT;

   SDL_SetRenderScale(renderer, scale_x, scale_y);

   bool      running = true;
   SDL_Event event;
   SDL_Time  current_time;

   SDL_GetCurrentTime(&current_time);
   srand(current_time % 3600);

   Player player;
   player_init(&player, MAX_HUNGER);
   player_move_to(&player, 200, 200);

   Food    food[FOOD_QUANTITY_MAX];
   uint8_t food_count = 0;
   uint8_t food_max   = FOOD_QUANTITY_MAX;
   while(food_count < FOOD_QUANTITY_MAX)
   {
      float nutrition = (rand() % (FOOD_AMOUNT - FOOD_MIN)) + FOOD_MIN;
      float size      = (FOOD_SIZE >> 1) + (FOOD_SIZE * nutrition / FOOD_AMOUNT);
      food_spawn(food + food_count, WORLD_WIDTH, WORLD_HEIGHT, size, nutrition);
      food_count++;
   }

   SDL_FRect reducer_bar_rect  = { .x = 32, .y = 16, .w = 16, .h = 128 };
   SDL_Color reducer_bar_color = { 32, 255, 16, 255 };

   SDL_FRect hunger_rect  = { 10, 10 + ((float)player.hunger.hunger_level / player.hunger.max_hunger) * HUNGER_BAR_HEIGHT, 10, HUNGER_BAR_HEIGHT };
   SDL_Color hunger_color = { 255, 255, 0, 255 }; // YELLOW color for hunger bar
   SDL_Color full_color   = { 255, 0, 0, 255 };   // Red color for full hunger

   uint32_t iter      = 0;
   uint32_t frame_cnt = 0;

   bool mv_lft = false;
   bool mv_rgt = false;
   bool mv_up  = false;
   bool mv_dn  = false;

   SDL_GetCurrentTime(&current_time);
   SDL_Time prev_time    = current_time;
   float    elapsed_time = 0.0;

   bool paused = true;

   SDL_Color paused_clear_colour  = { .r = 8, .g = 10, .b = 44, .a = UINT8_MAX };
   SDL_Color running_clear_colour = { .r = 44, .g = 10, .b = 8, .a = UINT8_MAX };

   float dt = 0.0f;
   float dx = 0.0f;
   float dy = 0.0f;

   while(running)
   {
      SDL_GetCurrentTime(&current_time);

      if(!paused)
      {
         dt = current_time - prev_time;
         dt *= 0.000000001;

         if(player.state != PLAYER_DEAD)
         {
            elapsed_time += dt;
            if(elapsed_time >= FOOD_QUANTITY_REDUCE_AFTER_secs)
            {
               elapsed_time = 0.0f;
               if(food_count > FOOD_QUANTITY_MIN)
               {
                  food_count -= FOOD_QUANTITY_REDUCER;
                  if(food_count < FOOD_QUANTITY_MIN)
                  {
                     food_count = FOOD_QUANTITY_MIN;
                  }
               }
            }
         }
      }
      else
      {
         dt = 0;
      }
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

               case SDLK_P:
                  paused = !paused;
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
         dx             = 0;
         player.speed_x = 0;
         player.x       = 1 + 0.5f * player.size;
      }
      else if((dx > 0) && (player.x >= WORLD_WIDTH - (0.5f * player.size)))
      {
         dx             = 0;
         player.speed_x = 0;
         player.x       = WORLD_WIDTH - (0.5f * player.size) - 1;
      }

      if((dy < 0) && (player.y <= 0.5f * player.size))
      {
         dy             = 0;
         player.speed_y = 0;
         player.y       = 1 + 0.5f * player.size;
      }
      else if((dy > 0) && (player.y >= WORLD_HEIGHT - (0.5f * player.size)))
      {
         dy             = 0;
         player.speed_y = 0;
         player.y       = WORLD_HEIGHT - (0.5f * player.size) - 1;
      }

      // Update player state
      player_move(&player, dt, dx, dy);
      player_update(&player, dt); // Update player with a fixed delta time

      // Update hunger bar position and color
      hunger_rect.y = (WINDOW_HEIGHT - 10) - ((float)player.hunger.hunger_level / player.hunger.max_hunger) * HUNGER_BAR_HEIGHT;
      hunger_rect.h = ((float)player.hunger.hunger_level / player.hunger.max_hunger) * HUNGER_BAR_HEIGHT;

      // Render the window
      if(paused)
      {
         SDL_SetRenderDrawColor(renderer, paused_clear_colour.r, paused_clear_colour.g, paused_clear_colour.b, paused_clear_colour.a); // Clear paused colour
      }
      else
      {
         SDL_SetRenderDrawColor(renderer, running_clear_colour.r, running_clear_colour.g, running_clear_colour.b, running_clear_colour.a); // Clear running colour
      }
      SDL_RenderClear(renderer);

      // Check for collision between player and food
      collision_rect_t player_collision_box = { player.x - 0.5f * player.size, player.y - 0.5f * player.size, player.size, player.size };

      // Iterate through all food items and check for collisions
      for(uint8_t i = 0; i < food_count; i++)
      {
         collision_rect_t food_collision_box = { food[i].x - 0.5f * food[i].size, food[i].y - 0.5f * food[i].size, food[i].size, food[i].size };
         if(collision_aabb_centered(&player_collision_box, &food_collision_box))
         {
            // Handle player-food collision
            player_eat(&player, food[i].nutrient);

            // Respawn food at a new location
            float nutrition = (rand() % (FOOD_AMOUNT - FOOD_MIN)) + FOOD_MIN;
            float size      = (FOOD_SIZE >> 1) + (FOOD_SIZE * nutrition / FOOD_AMOUNT);
            food_spawn(&food[i], WORLD_WIDTH - (FOOD_SIZE << 1), WORLD_HEIGHT - (FOOD_SIZE << 1), size, nutrition);
         }
      }

      float reduction_time_ratio = elapsed_time / FOOD_QUANTITY_REDUCE_AFTER_secs;
      if(0.9f < reduction_time_ratio) // red
      {
         reducer_bar_color.r = 248;
         reducer_bar_color.g = 8;
         reducer_bar_color.b = 0;
      }
      else if(0.7f < reduction_time_ratio) // orangey
      {
         reducer_bar_color.r = 248;
         reducer_bar_color.g = 200;
         reducer_bar_color.b = 0;
      }
      else // green
      {
         reducer_bar_color.r = 32;
         reducer_bar_color.g = 240;
         reducer_bar_color.b = 0;
      }

      SDL_SetRenderDrawColor(renderer, 0, 55, 55, 255);
      SDL_RenderFillRect(renderer, &wrld_rect);

      // draw_hunger_bar(renderer, &hunger_rect, hunger_color);
      draw_world(renderer, &wrld_rect);
      draw_food(renderer, food, food_count, &wrld_rect);
      draw_player(renderer, &player, &wrld_rect);
      draw_vertical_bar64(renderer, &reducer_bar_rect, reducer_bar_color, reduction_time_ratio);
      draw_food_count(renderer, food_count, reducer_bar_rect.x + reducer_bar_rect.w + 8, reducer_bar_rect.y, 4);

      SDL_RenderPresent(renderer);

      if((current_time / 1000000000) != (prev_time / 1000000000))
      {
         char window_fps_title[64];
         snprintf(window_fps_title, sizeof(window_fps_title), "%s - FPS := %u", window_title, frame_cnt);

         // Update the window title with the current frame count
         SDL_SetWindowTitle(window, window_fps_title);
         frame_cnt = 0;
      }

      prev_time = current_time;
   }
   return 0;
}
