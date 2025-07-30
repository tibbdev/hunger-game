/**
 * @file    hunger.c
 * @author  tibbdev
 * @brief   Hunger management functions for the Hunger Game simulation
 * @version 0.1
 * @date    2025-07-30
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "hunger.h"

#include <stddef.h>

#define DEFAULT_MAX_HUNGER 1000.0f // Default maximum hunger level if not specified

void hunger_init(Hunger *hunger, float max_hunger)
{
   if(NULL == hunger)
   {
      return; // Handle null pointer
   }

   if(max_hunger <= 0)
   {
      max_hunger = DEFAULT_MAX_HUNGER; // Default maximum hunger level if invalid
   }

   hunger->hunger_level = max_hunger * 0.5f; // Initialize hunger level to half of max hunger
   hunger->max_hunger   = max_hunger;        // Set maximum hunger level
}

void hunger_eat(Hunger *hunger, float food_amount)
{
   if(NULL == hunger || food_amount < 0)
   {
      return; // Handle null pointer or invalid food amount
   }
   hunger->hunger_level -= food_amount; // Decrease hunger level by food amount
   if(hunger->hunger_level < 0)
   {
      hunger->hunger_level = 0; // Ensure hunger level does not go below zero
   }
}

bool hunger_below_threshold(const Hunger *hunger, float threshold)
{
   if(NULL == hunger || threshold < 0)
   {
      return false; // Handle null pointer or invalid threshold
   }
   if(threshold > hunger->max_hunger)
   {
      return false; // If threshold is greater than max hunger, return false
   }
   return hunger->hunger_level < threshold; // Check if hunger level is below threshold
}

float hunger_get_level(const Hunger *hunger)
{
   if(NULL == hunger)
   {
      return -1; // Handle null pointer, return -1 as an error code
   }
   return hunger->hunger_level; // Return current hunger level
}

void hunger_reset(Hunger *hunger)
{
   if(NULL == hunger)
   {
      return; // Handle null pointer
   }
   hunger->hunger_level = hunger->max_hunger * 0.5f; // Reset hunger level to maximum
}

void hunger_increase(Hunger *hunger, float amount)
{
   if(NULL == hunger || amount < 0)
   {
      return; // Handle null pointer or invalid amount
   }
   hunger->hunger_level += amount; // Increase hunger level by specified amount
   if(hunger->hunger_level > hunger->max_hunger)
   {
      hunger->hunger_level = hunger->max_hunger; // Ensure hunger level does not exceed max
   }
}

bool hunger_is_full(const Hunger *hunger)
{
   if(NULL == hunger)
   {
      return false; // Handle null pointer
   }
   return hunger->hunger_level >= hunger->max_hunger; // Check if hunger level is at maximum
}

void hunger_simulate(Hunger *hunger, float time_passed)
{
   if(NULL == hunger || time_passed < 0)
   {
      return; // Handle null pointer or invalid time passed
   }
   // Simulate hunger increase over time, e.g., increase by 1 for each time unit
   hunger->hunger_level += time_passed;
   if(hunger->hunger_level > hunger->max_hunger)
   {
      hunger->hunger_level = hunger->max_hunger; // Ensure hunger level does not exceed max
   }
}
