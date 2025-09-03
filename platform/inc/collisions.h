/**
 * @file    collisions.h
 * @author  tibbdev
 * @brief   Header file for collision system
 * @version 0.1
 * @date    2025-07-30
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
   float x;
   float y;
   float w;
   float h;
} CollisionRect;

bool collision_aabb(CollisionRect *const A, CollisionRect *const B);
