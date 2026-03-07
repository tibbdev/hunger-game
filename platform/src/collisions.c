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

bool collision_aabb_centered(collision_rect_t *const A, collision_rect_t *const B)
{
   bool overlap_x = false;
   bool overlap_y = false;

   overlap_x = (A->x + 0.5f * A->w >= (B->x - 0.5f * B->w)) && (B->x + 0.5f * B->w >= (A->x - 0.5f * A->w));
   overlap_y = (A->y + 0.5f * A->h >= (B->y - 0.5f * B->h)) && (B->y + 0.5f * B->h >= (A->y - 0.5f * A->h));

   return overlap_x && overlap_y;
}

bool collision_aabb(collision_rect_t *const A, collision_rect_t *const B)
{
   bool overlap_x = false;
   bool overlap_y = false;

   overlap_x = ((A->x + A->w) >= B->x) && ((B->x + B->w) >= A->x);
   overlap_y = (A->y + A->h >= B->y) && (B->y + B->h >= A->y);

   return overlap_x && overlap_y;
}
