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

#define FOOD_AMOUNT 37
#define FOOD_MIN    5
#define FOOD_SIZE   4

typedef struct
{
   float    x;        // X coordinate of the food
   float    y;        // Y coordinate of the food
   float    size;     // Size of the food item
   uint32_t nutrient; // Amount of nutrients
} Food;

/**
 * @brief Initializes a food item with given parameters.
 *
 * @param food Pointer to the Food structure to initialize.
 * @param x X coordinate of the food item.
 * @param y Y coordinate of the food item.
 * @param size Size of the food item.
 * @param nutrient Amount of nutrients in the food item.
 */
void food_spawn(Food *food, uint32_t x_max, uint32_t y_max, float size, uint32_t nutrient);