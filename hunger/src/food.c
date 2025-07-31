#include "food.h"

#include <stdlib.h>

void food_spawn(Food *food, uint32_t x_max, uint32_t y_max, float size, uint32_t nutrient)
{
   if(food == NULL)
   {
      return;
   }

   food->x        = size + (float)(rand() % x_max);
   food->y        = size + (float)(rand() % y_max);
   food->size     = size;
   food->nutrient = nutrient;
}
