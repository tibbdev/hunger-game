/**
 * @file    player.c
 * @author  tibbdev
 * @brief   Player management functions for the Hunger Game simulation
 * @version 0.1
 * @date    2025-07-30
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "player.h"

#include <math.h>

void player_init(Player *player, float max_hunger)
{
   if(player == NULL)
   {
      return; // Handle null pointer case
   }

   hunger_init(&player->hunger, max_hunger); // Initialize hunger structure

   player->x                     = 0.0f;                              // Initialize x-coordinate
   player->y                     = 0.0f;                              // Initialize y-coordinate
   player->speed_x               = 0.0f;                              // Set default movement speed
   player->speed_y               = 0.0f;                              // Set default movement speed
   player->accel                 = 120.0f;                            // Set default acceleration
   player->decel                 = 10 * player->accel;                // Set default deceleration
   player->max_speed             = 120.0f;                            // Set default maximum speed
   player->size                  = 8.0f;                              // Set default size (width and height)
   player->consumption_rate      = 32.0f;                             // Set default consumption rate
   player->starved_time          = 0.0f;                              // Initialize starved time
   player->state                 = PLAYER_IDLE;                       // Set initial state to idle
   player->hunger_state          = PLAYER_HUNGER_OK;                  // Set initial hunger state to okay
   player->hungry_threshold      = player->hunger.max_hunger * 0.55f; // Set threshold for hunger state
   player->starving_threshold    = player->hunger.max_hunger * 0.9f;  // Set threshold for starving state
   player->starve_time_threshold = 10.0f;                             // Set threshold for starvation time
   player->starved_time          = 0.0f;                              // Initialize starved time
}

void player_update(Player *player, float delta_time)
{
   if(player == NULL)
   {
      return; // Handle null pointer case
   }

   if(player->state != PLAYER_DEAD)
   {
      // Update player's hunger level based on consumption rate and delta time
      hunger_increase(&player->hunger, player->consumption_rate * delta_time);

      // Check hunger state and update player state accordingly
      if(hunger_below_threshold(&player->hunger, player->hungry_threshold))
      {
         player->hunger_state = PLAYER_HUNGER_OK; // Player is Satisfied
      }
      else if(hunger_below_threshold(&player->hunger, player->starving_threshold))
      {
         player->hunger_state = PLAYER_HUNGER_HUNGRY; // Player is Hungry
      }
      else if(!hunger_is_full(&player->hunger))
      {
         player->hunger_state = PLAYER_HUNGER_STARVING; // Player's hunger is okay
      }
      else
      {
         player->hunger_state = PLAYER_HUNGER_STARVED; // Player is Starved
      }

      if((player->hunger_state == PLAYER_HUNGER_STARVED) || (player->hunger_state == PLAYER_HUNGER_STARVING))
      {
         player->starved_time += delta_time; // Record the time when the player started starving

         if(player->starved_time >= player->starve_time_threshold)
         {
            player->state = PLAYER_DEAD; // Player is dead due to starvation
         }
      }
      else if(player->starved_time > 0.0f)
      {
         player->starved_time -= delta_time;
         if(player->starved_time < 0.0f)
         {
            player->starved_time = 0.0f;
         }
      }
   }
}

void player_move(Player *player, float dt, float dx, float dy)
{
   if(player == NULL)
   {
      return; // Handle null pointer case
   }

   if(player->state == PLAYER_DEAD)
   {
      return; // Do not move if the player is dead
   }

   if(player->hunger_state == PLAYER_HUNGER_STARVED)
   {
      return; // Do not move if the player is starved
   }

   float length = sqrtf(dx * dx + dy * dy);

   if(length > 0.0f)
   {
      if(player->speed_x < player->max_speed)
      {
         player->speed_x += player->accel * dt; // Accelerate the player
      }
      else if(player->speed_x > player->max_speed)
      {
         player->speed_x = player->max_speed; // Cap the speed at maximum
      }

      if(player->speed_y < player->max_speed)
      {
         player->speed_y += player->accel * dt; // Accelerate the player
      }
      else if(player->speed_y > player->max_speed)
      {
         player->speed_y = player->max_speed; // Cap the speed at maximum
      }

      player->dx = dx / length;
      player->dy = dy / length;

      player->x += player->dx * player->speed_x * dt;
      player->y += player->dy * player->speed_y * dt;

      player->state = PLAYER_MOVING;
   }
   else
   {
      if(player->speed_x > 0.0f)
      {
         player->speed_x -= player->decel * dt; // Decelerate the player
         if(player->speed_x < 0.0f)
         {
            player->speed_x = 0.0f; // Ensure speed does not go below zero
         }
         else
         {
            player->x += player->dx * player->speed_x * dt;
         }
      }

      if(player->speed_y > 0.0f)
      {
         player->speed_y -= player->decel * dt; // Decelerate the player
         if(player->speed_y < 0.0f)
         {
            player->speed_y = 0.0f; // Ensure speed does not go below zero
         }
         else
         {
            player->y += player->dy * player->speed_y * dt;
         }
      }
      player->state = PLAYER_IDLE;
   }
}

void player_move_to(Player *player, float x, float y)
{
   if(player == NULL)
   {
      return; // Handle null pointer case
   }

   player->x = x;
   player->y = y;
}

#include <stdio.h>
void player_display(const Player *player)
{
   if(player == NULL)
   {
      return; // Handle null pointer case
   }

   // Display player's current state
   printf("Player Position: (%.2f, %.2f)\n", player->x, player->y);
   printf("Player State: %d\n", player->state);
   printf("Player Hunger State: %d\n", player->hunger_state);
   printf("Current Hunger Level: %.2f / %.2f\n", player->hunger.hunger_level, player->hunger.max_hunger);
   printf("Starved Time: %.2f seconds\n", player->starved_time);
   printf("Hunger Thresholds: Hungry = %.2f, Starving = %.2f\n", player->hungry_threshold, player->starving_threshold);
}
