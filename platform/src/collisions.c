/**
 * @file    collisions.c
 * @author  tibbdev
 * @brief   Source file for collision system
 * @version 0.1
 * @date    2025-07-30
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "collisions.h"

bool collision_aabb(CollisionRect *const A, CollisionRect *const B)
{
   bool overlap_x = false;
   bool overlap_y = false;

   overlap_x = (A->x + 0.5f * A->w >= (B->x - 0.5f * B->w)) && (B->x + 0.5f * B->w >= (A->x - 0.5f * A->w));
   overlap_y = (A->y + 0.5f * A->h >= (B->y - 0.5f * B->h)) && (B->y + 0.5f * B->h >= (A->y - 0.5f * A->h));

   return overlap_x && overlap_y;
}
