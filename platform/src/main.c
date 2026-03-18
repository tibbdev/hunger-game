/**
 * @file    main.c
 * @author  tibbdev
 * @brief   Hunger Game - A simple SDL3 application to simulate hunger levels.
 * @version 0.1
 * @date    2025-07-30
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "SDL3/SDL.h"
#include "SDL3/SDL_surface.h"
#include "SDL3/SDL_audio.h"
#include "SDL3/SDL_joystick.h"

#include "hunger.h"
#include "player.h"
#include "food.h"
#include "collisions.h"
#include "resources.h"
#include "physfs.h"

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

SDL_Texture *arena_tex  = NULL;
SDL_Texture *text_tex   = NULL;
SDL_Texture *bg_tex     = NULL;
SDL_Texture *food_tex   = NULL;
SDL_Texture *player_tex = NULL;

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

void draw_player(SDL_Renderer *renderer, player_t *player, SDL_FRect *wrld_rect, float elapsed_time)
{
   SDL_Color plyr_clr   = { 255, 255, 0, 255 }; // Default color for hunger bar (yellow)
   uint16_t  sprite_row = 0;
   uint64_t  times      = elapsed_time;
   elapsed_time -= (float)times;
   times = elapsed_time * 5;
   times %= 4;

   if(player->state == PLAYER_DEAD)
   {
      // If the player is dead, set the hunger bar to Super Dark Red
      plyr_clr         = (SDL_Color) { 69, 7, 9, 255 }; // Super Dark Red for dead state
      sprite_row       = 3;
      player->rotation = 360;

      times = elapsed_time;
      times %= 4;
   }
   else if(player->hunger_state == PLAYER_HUNGER_STARVED)
   {
      // If the player is starved, set the hunger bar to dark red
      plyr_clr   = (SDL_Color) { 159, 7, 18, 255 }; // Dark color for starved state
      sprite_row = 3;
      times      = elapsed_time * 2;
      times %= 4;
   }
   else if(player->hunger_state == PLAYER_HUNGER_STARVING)
   {
      // If the player is starving, set the hunger bar to red
      plyr_clr   = (SDL_Color) { 251, 33, 21, 255 }; // Red color for hungry state
      sprite_row = 2;
   }
   else if(player->hunger_state == PLAYER_HUNGER_HUNGRY)
   {
      // If the player is hungry, set the hunger bar to orange
      plyr_clr   = (SDL_Color) { 254, 154, 55, 255 }; // Orange color for hungry state
      sprite_row = 1;
   }
   else
   {
      // If the player is okay, set the hunger bar to green
      plyr_clr = (SDL_Color) { 187, 244, 81, 255 }; // Green color for okay state
   }

   if(NULL != player_tex)
   {
      SDL_FRect playr_src_rect;
      playr_src_rect.w = 12;
      playr_src_rect.h = 12;
      playr_src_rect.x = (times * 12) % player_tex->w;
      playr_src_rect.y = sprite_row * playr_src_rect.h * 2 + (playr_src_rect.h * player->moving);

      SDL_FRect playr_dest_rect;
      playr_dest_rect.w = 12;
      playr_dest_rect.h = 12;
      playr_dest_rect.x = wrld_rect->x + player->x - (playr_dest_rect.w * 0.5f);
      playr_dest_rect.y = wrld_rect->y + player->y - (playr_dest_rect.h * 0.5f);

      SDL_RenderTextureRotated(renderer, player_tex, &playr_src_rect, &playr_dest_rect, -1 * player->rotation, NULL, SDL_FLIP_NONE);
   }
   else
   {
      SDL_FRect plyr_rect;
      plyr_rect.h = player->size;
      plyr_rect.w = player->size;
      plyr_rect.x = wrld_rect->x + player->x - (player->size * 0.5f);
      plyr_rect.y = wrld_rect->y + player->y - (player->size * 0.5f);

      SDL_RenderFillRect(renderer, &plyr_rect);

      SDL_SetRenderDrawColor(renderer, plyr_clr.r * 0.75f, plyr_clr.g * 0.75f, plyr_clr.b * 0.75f, plyr_clr.a);
      SDL_RenderRect(renderer, &plyr_rect);
   }

   float player_hunger_bar_y   = wrld_rect->y + player->y - (player->size * 0.5f) - 3.0f;                                                 // Position the hunger bar above the player
   float player_hunger_bar_x0  = wrld_rect->x + player->x - (player->size * 0.5f) - 2.0f;                                                 // Center the hunger bar above the player
   float player_hunger_bar_x1  = wrld_rect->x + (player->x + (player->size * 0.5f) + 1.0f);                                               // Center the hunger bar above the player
   float player_hunger_bar_len = (player_hunger_bar_x1 - player_hunger_bar_x0) * player->hunger.hunger_level / player->hunger.max_hunger; // Calculate the length of the hunger bar based on the player's hunger level

   float player_hunger_bar_x2 = player_hunger_bar_x0 + player_hunger_bar_len; // Center the hunger bar above the player

   SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black color for hunger bar background
   SDL_RenderLine(renderer, player_hunger_bar_x0, player_hunger_bar_y, player_hunger_bar_x1, player_hunger_bar_y);

   SDL_SetRenderDrawColor(renderer, plyr_clr.r, plyr_clr.g, plyr_clr.b, plyr_clr.a);
   SDL_RenderLine(renderer, player_hunger_bar_x0, player_hunger_bar_y, player_hunger_bar_x2, player_hunger_bar_y);
}

void draw_food(SDL_Renderer *renderer, Food *food, uint8_t food_count, SDL_FRect *wrld_rect, float elapsed_time)
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
      if(NULL != food_tex)
      {
         SDL_FRect food_src_rect;
         food_src_rect.h  = 34;
         food_src_rect.w  = 34;
         uint8_t food_sel = (uint8_t)roundf(((float)food[i].nutrient / (FOOD_AMOUNT + FOOD_MIN)) * 6.0f);
         // printf("food_sel := %u\r\n", food_sel);

         switch(food_sel)
         {
            case 0:
               food_src_rect.x = 34;
               food_src_rect.y = 0;
               break;
            case 1:
               food_src_rect.x = 13 * 34;
               food_src_rect.y = 0;
               break;
            case 2:
               food_src_rect.x = 6 * 34;
               food_src_rect.y = 34;
               break;
            case 3:
               food_src_rect.x = 0;
               food_src_rect.y = 3 * 34;
               break;
            case 4:
               food_src_rect.x = 11 * 34;
               food_src_rect.y = 3 * 34;
               break;
            case 5:
               food_src_rect.x = 8 * 34;
               food_src_rect.y = 7 * 34;
               break;

            default:
               food_src_rect.x = 34;
               food_src_rect.y = 0;
               break;
         }

         SDL_FRect food_dest_rect;
         food_dest_rect.h = 0.42f * (26 + (2 * food_sel));
         food_dest_rect.w = 0.42f * (26 + (2 * food_sel));
         food_dest_rect.x = wrld_rect->x + food[i].x - (0.5f * food_dest_rect.w) + 0.2f * sinf((food[i].freq * elapsed_time) + food[i].freq);
         food_dest_rect.y = wrld_rect->y + food[i].y - (0.5f * food_dest_rect.h) + 0.2f * cosf((food[i].freq * elapsed_time) - food[i].freq);

         SDL_RenderTexture(renderer, food_tex, &food_src_rect, &food_dest_rect);
      }
      else
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

void draw_uint_scaled_3dig(SDL_Renderer *renderer, uint32_t x, uint32_t y, uint8_t colour, uint16_t number, float scale)
{
   if(NULL == renderer)
   {
      return;
   }

   if(NULL == text_tex)
   {
      return;
   }

   char str_num[11] = "";

   sprintf(str_num, "%03lu", number);

   uint16_t  idx = 0;
   SDL_FRect number_src_rect;
   SDL_FRect number_dest_rect;

   number_src_rect.x = 0;
   number_src_rect.y = 0;
   number_src_rect.w = 16;
   number_src_rect.h = 32;

   number_dest_rect.x = x;
   number_dest_rect.y = y;
   number_dest_rect.w = scale * number_src_rect.w;
   number_dest_rect.h = scale * number_src_rect.h;
   do
   {
      switch(str_num[idx])
      {
         case '0':
            number_src_rect.x = number_src_rect.w * (9 + (10 * colour));
            break;
         case '1':
         case '2':
         case '3':
         case '4':
         case '5':
         case '6':
         case '7':
         case '8':
         case '9':
            number_src_rect.x = number_src_rect.w * (((uint16_t)str_num[idx] - (uint16_t)'1') + (10 * colour));
            break;

         default:
            break;
      }

      number_dest_rect.x = x + idx * number_dest_rect.w;

      SDL_RenderTexture(renderer, text_tex, &number_src_rect, &number_dest_rect);
      idx++;
   } while(str_num[idx]);
}

void draw_uint_scaled(SDL_Renderer *renderer, uint32_t x, uint32_t y, uint8_t colour, uint32_t number, float scale)
{
   if(NULL == renderer)
   {
      return;
   }

   if(NULL == text_tex)
   {
      return;
   }

   char str_num[11] = "";

   sprintf(str_num, "%lu", number);

   uint16_t  idx = 0;
   SDL_FRect number_src_rect;
   SDL_FRect number_dest_rect;

   number_src_rect.x = 0;
   number_src_rect.y = 0;
   number_src_rect.w = 16;
   number_src_rect.h = 32;

   number_dest_rect.x = x;
   number_dest_rect.y = y;
   number_dest_rect.w = scale * number_src_rect.w;
   number_dest_rect.h = scale * number_src_rect.h;
   do
   {
      switch(str_num[idx])
      {
         case '0':
            number_src_rect.x = number_src_rect.w * (9 + (10 * colour));
            break;
         case '1':
         case '2':
         case '3':
         case '4':
         case '5':
         case '6':
         case '7':
         case '8':
         case '9':
            number_src_rect.x = number_src_rect.w * (((uint16_t)str_num[idx] - (uint16_t)'1') + (10 * colour));
            break;

         default:
            break;
      }

      number_dest_rect.x = x + idx * number_dest_rect.w;

      SDL_RenderTexture(renderer, text_tex, &number_src_rect, &number_dest_rect);
      idx++;
   } while(str_num[idx]);
}

void draw_uint_scaled_5dig(SDL_Renderer *renderer, uint32_t x, uint32_t y, uint8_t colour, uint32_t number, float scale)
{
   if(NULL == renderer)
   {
      return;
   }

   if(NULL == text_tex)
   {
      return;
   }

   char str_num[11] = "";

   sprintf(str_num, "%05lu", number);

   uint16_t  idx = 0;
   SDL_FRect number_src_rect;
   SDL_FRect number_dest_rect;

   number_src_rect.x = 0;
   number_src_rect.y = 0;
   number_src_rect.w = 16;
   number_src_rect.h = 32;

   number_dest_rect.x = x;
   number_dest_rect.y = y;
   number_dest_rect.w = scale * number_src_rect.w;
   number_dest_rect.h = scale * number_src_rect.h;
   do
   {
      switch(str_num[idx])
      {
         case '0':
            number_src_rect.x = number_src_rect.w * (9 + (10 * colour));
            break;
         case '1':
         case '2':
         case '3':
         case '4':
         case '5':
         case '6':
         case '7':
         case '8':
         case '9':
            number_src_rect.x = number_src_rect.w * (((uint16_t)str_num[idx] - (uint16_t)'1') + (10 * colour));
            break;

         default:
            break;
      }

      number_dest_rect.x = x + idx * number_dest_rect.w;

      SDL_RenderTexture(renderer, text_tex, &number_src_rect, &number_dest_rect);
      idx++;
   } while(str_num[idx]);
}

void draw_uint(SDL_Renderer *renderer, uint32_t x, uint32_t y, uint8_t colour, uint32_t number)
{
   draw_uint_scaled(renderer, x, y, colour, number, 1.0);
}

void draw_score(SDL_Renderer *renderer, uint8_t colour, player_t const *const player)
{
   draw_uint_scaled_5dig(renderer, 740, 462, 1, player->score, 1.0f);
   draw_uint_scaled_3dig(renderer, 767, 495, colour, player->eaten_count, 0.55f);
}

void draw_world(SDL_Renderer *renderer, SDL_FRect *wrld_rect)
{
   if(NULL != arena_tex)
   {
      float diff_h = arena_tex->h - wrld_rect->h;
      float diff_w = arena_tex->w - wrld_rect->w;

      static SDL_FRect arena_rect = { 0.0f, 0.0f, 0.0f, 0.0f };

      arena_rect.h = arena_tex->h;
      arena_rect.w = arena_tex->w;
      arena_rect.x = wrld_rect->x - (0.5f * diff_w);
      arena_rect.y = wrld_rect->y - (0.5f * diff_h);

      SDL_RenderTexture(renderer, arena_tex, NULL, &arena_rect);
   }
   else
   {
      SDL_SetRenderDrawColor(renderer, 3, 79, 59, 255); // Dark color for world
      SDL_RenderFillRect(renderer, wrld_rect);
      SDL_SetRenderDrawColor(renderer, 240, 253, 244, 255); // Dark color for world
      SDL_RenderRect(renderer, wrld_rect);
   }
}

int main(int argc, char ** argv)
{
   SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD);

   SDL_CreateWindowAndRenderer(window_title, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_FULLSCREEN | SDL_WINDOW_RESIZABLE, &window, &renderer);

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

   float scale = scale_x < scale_y ? scale_x : scale_y;

   SDL_SetRenderScale(renderer, scale, scale);

   init_resources(argv[0]);

   bg_tex     = SDL_CreateTextureFromSurface(renderer, SDL_LoadPNG("assets/img/background.png"));
   arena_tex  = SDL_CreateTextureFromSurface(renderer, SDL_LoadPNG("assets/img/arena.png"));
   text_tex   = SDL_CreateTextureFromSurface(renderer, SDL_LoadPNG("assets/img/text.png"));
   player_tex = SDL_CreateTextureFromSurface(renderer, SDL_LoadPNG("assets/img/player.png"));
   food_tex   = SDL_CreateTextureFromSurface(renderer, SDL_LoadPNG("assets/img/foodfromcts1a.png"));

   bool      running = true;
   SDL_Event event;
   SDL_Time  current_time;

   SDL_GetCurrentTime(&current_time);
   srand(current_time % 3600);

   player_t player;
   player_init(&player, MAX_HUNGER);
   player_move_to(&player, 0.5f * WORLD_WIDTH, 0.5f * WORLD_HEIGHT);

   uint32_t score_inc = 1;

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

   SDL_FRect reducer_bar_rect  = { .x = 740, .y = 14, .w = 16, .h = 128 };
   SDL_Color reducer_bar_color = { 32, 255, 16, 255 };

   SDL_FRect hunger_rect  = { 10, 10 + ((float)player.hunger.hunger_level / player.hunger.max_hunger) * HUNGER_BAR_HEIGHT, 10, HUNGER_BAR_HEIGHT };
   SDL_Color hunger_color = { 255, 255, 0, 255 }; // YELLOW color for hunger bar
   SDL_Color full_color   = { 255, 0, 0, 255 };   // Red color for full hunger

   uint32_t frame_cnt = 0;

   bool mv_lft = false;
   bool mv_rgt = false;
   bool mv_up  = false;
   bool mv_dn  = false;

   SDL_GetCurrentTime(&current_time);
   SDL_Time prev_time    = current_time;
   float    elapsed_time = 0.0;
   float    forever_time = 0.0;

   bool paused          = true;
   bool paused_released = true;

   SDL_Color paused_clear_colour  = { .r = 8, .g = 10, .b = 44, .a = UINT8_MAX };
   SDL_Color running_clear_colour = { .r = 44, .g = 10, .b = 8, .a = UINT8_MAX };

   float dt = 0.0f;
   float dx = 0.0f;
   float dy = 0.0f;

   uint32_t eat_counts[24];
   for(uint16_t idx = 0; 24 > idx; idx++)
   {
      eat_counts[idx] = 0;
   }
   uint16_t eat_count_idx = 0;

   uint8_t eat_count_colour = 1;

   player.rotation               = 0;
   bool            gamepad_found = SDL_HasGamepad();
   int             gp_count      = 0;
   SDL_JoystickID *ids           = SDL_GetGamepads(&gp_count);
   SDL_Gamepad    *gamepad       = NULL;

   // Iterate over the list of gamepads
   for(int i = 0; i < gp_count; i++)
   {
      SDL_Gamepad *gamepd = SDL_OpenGamepad(ids[i]);
      if(gamepad == NULL)
      {
         gamepad = gamepd;
      }

      printf("Gamepad connected: %s\r\n", SDL_GetGamepadName(gamepd));

      // Close the other gamepads
      if(i > 0)
      {
         SDL_CloseGamepad(gamepd);
      }
   }

   while(running)
   {
      SDL_GetCurrentTime(&current_time);
      eat_count_colour = 1;

      if(!paused)
      {
         eat_counts[eat_count_idx % 24] = player.eaten_count;
         eat_count_idx++;

         for(uint16_t idx = 0; (24 > idx) && (1 == eat_count_colour); idx++)
         {
            if(player.eaten_count != eat_counts[idx])
            {
               eat_count_colour = 2;
            }
         }

         dt = current_time - prev_time;
         dt *= 0.000000001;
         forever_time += dt;

         if((player.state != PLAYER_DEAD) && (player.hunger_state != PLAYER_HUNGER_STARVING))
         {
            elapsed_time += dt;
            if(elapsed_time >= FOOD_QUANTITY_REDUCE_AFTER_secs)
            {
               elapsed_time = 0.0f;
               score_inc++;
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

               case SDLK_SPACE:
                  if(paused_released)
                  {
                     paused = !paused;
                  }
                  paused_released = false;
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

               case SDLK_SPACE:
                  paused_released = true;
                  break;

               default:
                  break;
            }
         }

         if(event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN)
         {
            // printf("Button pressed: %s\r\n", SDL_GetGamepadStringForButton((SDL_GamepadButton)event.gbutton.button));
            switch((SDL_GamepadButton)event.gbutton.button)
            {
               case SDL_GAMEPAD_BUTTON_DPAD_UP:
                  mv_up = true;
                  break;
               case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
                  mv_dn = true;
                  break;
               case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
                  mv_lft = true;
                  break;
               case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
                  mv_rgt = true;
                  break;

               case SDL_GAMEPAD_BUTTON_START:
                  paused = !paused;
                  break;

               case SDL_GAMEPAD_BUTTON_BACK:
                  running = false;
                  break;

               default:
                  break;
            }
         }

         if(event.type == SDL_EVENT_GAMEPAD_BUTTON_UP)
         {
            // printf("Button released: %s\r\n", SDL_GetGamepadStringForButton((SDL_GamepadButton)event.gbutton.button));
            switch((SDL_GamepadButton)event.gbutton.button)
            {
               case SDL_GAMEPAD_BUTTON_DPAD_UP:
                  mv_up = false;
                  break;
               case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
                  mv_dn = false;
                  break;
               case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
                  mv_lft = false;
                  break;
               case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
                  mv_rgt = false;
                  break;

               default:
                  break;
            }
         }

         if(event.type == SDL_EVENT_GAMEPAD_AXIS_MOTION)
         {
            // printf("Axis moved: %s - Value := %ld\r\n", SDL_GetGamepadStringForAxis((SDL_GamepadAxis)event.gaxis.axis), event.gaxis.value);
         }
      }

      if(mv_lft && !mv_rgt)
      {
         dx              = -1;
         player.rotation = 270;
      }
      else if(!mv_lft && mv_rgt)
      {
         dx              = 1;
         player.rotation = 90;
      }
      else
      {
         dx = 0;
      }

      if(mv_up && !mv_dn)
      {
         dy = -1;
         if(0 == player.rotation)
         {
            player.rotation = 135;
         }
         else
         {
            player.rotation = 360;
         }
      }
      else if(!mv_up && mv_dn)
      {
         dy = 1;
         if(0 == player.rotation)
         {
            player.rotation += 45;
         }
         else
         {
            player.rotation = 180;
         }
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
      player_update(&player, dt);

      // Update hunger bar position and color
      hunger_rect.y = (WINDOW_HEIGHT - 10) - ((float)player.hunger.hunger_level / player.hunger.max_hunger) * HUNGER_BAR_HEIGHT;
      hunger_rect.h = ((float)player.hunger.hunger_level / player.hunger.max_hunger) * HUNGER_BAR_HEIGHT;

      SDL_FRect paused_src_rect  = { .x = 0, .y = 33, .w = 256, .h = 122 };
      SDL_FRect paused_dest_rect = { .x = ((0.5f * WINDOW_WIDTH) - (0.5f * paused_src_rect.w)), .y = 128, .w = paused_src_rect.w, .h = paused_src_rect.h };

      SDL_RenderClear(renderer);

      if(NULL != bg_tex)
      {
         SDL_RenderTexture(renderer, bg_tex, NULL, NULL);
      }

      // Check for collision between player and food
      collision_rect_t player_collision_box = { player.x - 0.5f * player.size, player.y - 0.5f * player.size, player.size, player.size };

      // Iterate through all food items and check for collisions
      for(uint8_t i = 0; i < food_count; i++)
      {
         collision_rect_t food_collision_box = { food[i].x - 0.5f * food[i].size, food[i].y - 0.5f * food[i].size, food[i].size, food[i].size };
         if(collision_aabb_centered(&player_collision_box, &food_collision_box))
         {
            // Handle player-food collision
            player_eat(&player, food[i].nutrient, score_inc);

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
      draw_food(renderer, food, food_count, &wrld_rect, forever_time);
      draw_player(renderer, &player, &wrld_rect, forever_time);
      draw_vertical_bar64(renderer, &reducer_bar_rect, reducer_bar_color, reduction_time_ratio);
      draw_food_count(renderer, food_count, reducer_bar_rect.x + reducer_bar_rect.w + 8, reducer_bar_rect.y, 4);
      draw_score(renderer, eat_count_colour, &player);

      // Render the window
      if(NULL == text_tex)
      {
         if(paused)
         {
            SDL_SetRenderDrawColor(renderer, paused_clear_colour.r, paused_clear_colour.g, paused_clear_colour.b, paused_clear_colour.a); // Clear paused colour
         }
         else
         {
            SDL_SetRenderDrawColor(renderer, running_clear_colour.r, running_clear_colour.g, running_clear_colour.b, running_clear_colour.a); // Clear running colour
         }
      }
      else if(paused)
      {
         SDL_RenderTexture(renderer, text_tex, &paused_src_rect, &paused_dest_rect);
      }

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

      if (PHYSFS_isInit())
      {
         PHYSFS_deinit();
      }
   }
   return 0;
}
