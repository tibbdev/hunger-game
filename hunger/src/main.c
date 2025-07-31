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

#define MAX_HUNGER        1000.0f
#define FOOD_AMOUNT       42
#define FOOD_QUANTITY_MAX 10
#define FOOD_SIZE         4
#define HUNGER_BAR_HEIGHT 200

#define WINDOW_WIDTH  960
#define WINDOW_HEIGHT 540

#define WORLD_WIDTH  500
#define WORLD_HEIGHT 500

SDL_Window   *window       = NULL;
SDL_Renderer *renderer     = NULL;
const char   *window_title = "Hunger Game";

void draw_hunger_bar(SDL_Renderer *renderer, SDL_FRect *hunger_rect, SDL_Color hunger_color)
{
   SDL_SetRenderDrawColor(renderer, hunger_color.r, hunger_color.g, hunger_color.b, hunger_color.a);
   SDL_RenderFillRect(renderer, hunger_rect);
   SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200); // White color for outline
   SDL_RenderRect(renderer, hunger_rect);
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

   float player_hunger_bar_y  = wrld_rect->y + player->y - (player->size * 0.5f) - 4.0f; // Position the hunger bar above the player
   float player_hunger_bar_x0 = wrld_rect->x + player->x - (player->size * 0.5f) - 2.0f; // Center the hunger bar above the player

   float player_hunger_bar_x1 = wrld_rect->x + (player->x + (player->size * 0.5f) + 2.0f) * (float)player->hunger.hunger_level / player->hunger.max_hunger; // Center the hunger bar above the player

   SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black color for hunger bar outline
   SDL_RenderLine(renderer, player_hunger_bar_x0, player_hunger_bar_y, player_hunger_bar_x0 + player->size + 4, player_hunger_bar_y);
   SDL_RenderLine(renderer, player_hunger_bar_x0, player_hunger_bar_y + 1, player_hunger_bar_x0 + player->size + 4, player_hunger_bar_y + 1);

   SDL_SetRenderDrawColor(renderer, plyr_clr.r, plyr_clr.g, plyr_clr.b, plyr_clr.a);
   SDL_RenderLine(renderer, player_hunger_bar_x0, player_hunger_bar_y, player_hunger_bar_x1, player_hunger_bar_y);
   SDL_RenderLine(renderer, player_hunger_bar_x0, player_hunger_bar_y + 1, player_hunger_bar_x1, player_hunger_bar_y + 1);

   SDL_RenderFillRect(renderer, &plyr_rect);

   SDL_SetRenderDrawColor(renderer, 94, 233, 181, 255);
   SDL_RenderRect(renderer, &plyr_rect);
}
void draw_food(SDL_Renderer *renderer, Food *food, uint8_t food_count, SDL_FRect *wrld_rect)
{
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
void draw_world(SDL_Renderer *renderer, SDL_FRect *wrld_rect)
{
   SDL_SetRenderDrawColor(renderer, 3, 79, 59, 255); // Dark color for world
   SDL_RenderFillRect(renderer, wrld_rect);
   SDL_SetRenderDrawColor(renderer, 240, 253, 244, 255); // Dark color for world
   SDL_RenderRect(renderer, wrld_rect);
}

int main(void)
{
   SDL_Init(SDL_INIT_VIDEO);
   SDL_CreateWindowAndRenderer(window_title, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_FULLSCREEN, &window, &renderer);

   if(window == NULL || renderer == NULL)
   {
      fprintf(stderr, "Failed to create window or renderer: %s\n", SDL_GetError());
      return 1;
   }

   int window_width  = WINDOW_WIDTH;
   int window_height = WINDOW_HEIGHT;
   SDL_GetWindowSize(window, &window_width, &window_height);

   float scale_x = (float)window_width / (float)WINDOW_WIDTH;
   float scale_y = (float)window_height / (float)WINDOW_HEIGHT;

   SDL_SetRenderScale(renderer, scale_x, scale_y);

   SDL_SetRenderVSync(renderer, true);

   bool      running = true;
   SDL_Event event;

   SDL_Time current_time;
   SDL_GetCurrentTime(&current_time);

   srand(current_time % 3600);

   Player player;
   player_init(&player, MAX_HUNGER);
   player_move_to(&player, 200, 200);

   Food    food[FOOD_QUANTITY_MAX];
   uint8_t food_count = 0;
   for(; food_count < FOOD_QUANTITY_MAX; food_count++)
   {
      food_spawn(food + food_count, WORLD_WIDTH, WORLD_HEIGHT, FOOD_SIZE, (rand() % FOOD_AMOUNT) + 1);
   }

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

   while(running)
   {
      static float dt = 0.0f;
      static float dx = 0.0f;
      static float dy = 0.0f;

      SDL_GetCurrentTime(&current_time);

      dt = current_time - prev_time;
      dt *= 0.000000001;

      elapsed_time += dt;
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
      SDL_SetRenderDrawColor(renderer, 44, 10, 88, 255); // Clear dark color
      SDL_RenderClear(renderer);

      SDL_FRect wrld_rect = { (WINDOW_WIDTH >> 1) - (WORLD_WIDTH >> 1), (WINDOW_HEIGHT >> 1) - (WORLD_HEIGHT >> 1), WORLD_WIDTH, WORLD_HEIGHT };

      // Check for collision between player and food
      CollisionRect player_collision_box = { player.x - 0.5f * player.size, player.y - 0.5f * player.size, player.size, player.size };

      // Iterate through all food items and check for collisions
      for(uint8_t i = 0; i < food_count; i++)
      {
         CollisionRect food_collision_box = { food[i].x - 0.5f * food[i].size, food[i].y - 0.5f * food[i].size, food[i].size, food[i].size };
         if(collision_aabb(&player_collision_box, &food_collision_box))
         {
            // Handle player-food collision
            hunger_eat(&player.hunger, food[i].nutrient);
            // Respawn food at a new location
            food_spawn(&food[i], WORLD_WIDTH - (FOOD_SIZE << 1), WORLD_HEIGHT - (FOOD_SIZE << 1), FOOD_SIZE, (rand() % FOOD_AMOUNT) + 1);
         }
      }

      SDL_SetRenderDrawColor(renderer, 0, 55, 55, 255);
      SDL_RenderFillRect(renderer, &wrld_rect);

      // draw_hunger_bar(renderer, &hunger_rect, hunger_color);
      draw_world(renderer, &wrld_rect);
      draw_food(renderer, food, food_count, &wrld_rect);
      draw_player(renderer, &player, &wrld_rect);

      SDL_RenderPresent(renderer);

      if(current_time / 1000000000 != prev_time / 1000000000)
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
