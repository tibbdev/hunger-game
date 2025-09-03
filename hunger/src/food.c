#include "food.h"

#include <stdlib.h>

void food_spawn(Food *food, uint32_t x_max, uint32_t y_max, float size, uint32_t nutrient)
{
   if(food == NULL)
   {
      return;
   }

   float max_x = (float)x_max - size;
   float max_y = (float)y_max - size;

   float min_x = size;
   float min_y = size;

   if(max_x < min_x || max_y < min_y)
   {
      return; // Invalid world dimensions
   }

   uint16_t grid_size = 16;

   if(size > (x_max / grid_size))
   {
      size = 0.3f * x_max / grid_size; // Ensure food size isn't larger than the grid squares
   }
   if(size > (y_max / grid_size))
   {
      size = 0.3f * y_max / grid_size; // Ensure food size isn't larger than the grid squares
   }

   if(nutrient == 0)
   {
      nutrient = (rand() % FOOD_AMOUNT) + FOOD_MIN; // Random nutrient value if not specified
   }

   food->x        = (float)(rand() % grid_size);                     // Randomize initial x position within grid
   food->y        = (float)(rand() % grid_size);                     // Randomize initial y position within grid
   food->x        = min_x + (food->x / grid_size) * (max_x - min_x); // Ensure food spawns within the world bounds
   food->y        = min_y + (food->y / grid_size) * (max_y - min_y); // Ensure food spawns within the world bounds
   food->size     = size;
   food->nutrient = nutrient;
}
