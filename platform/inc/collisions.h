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
} collision_rect_t;

bool collision_aabb_centered(collision_rect_t *const A, collision_rect_t *const B);
bool collision_aabb(collision_rect_t *const A, collision_rect_t *const B);
