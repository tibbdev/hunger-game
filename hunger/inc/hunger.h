/**
 * @file    hunger.h
 * @author  tibbdev
 * @brief   Header file for hunger management system
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
   float hunger_level; // Current hunger level
   float max_hunger;   // Maximum hunger level
} Hunger;

// Function to initialize hunger
void hunger_init(Hunger *hunger, float max_hunger);
// Function to eat food and decrease hunger level
void hunger_eat(Hunger *hunger, float food_amount);
// Function to check if the hunger level is below a threshold
bool hunger_below_threshold(const Hunger *hunger, float threshold);
// Function to get the current hunger level
float hunger_get_level(const Hunger *hunger);
// Function to reset hunger level
void hunger_reset(Hunger *hunger);
// Function to increase hunger level over time
void hunger_increase(Hunger *hunger, float amount);
// Function to check if hunger is at maximum
bool hunger_is_full(const Hunger *hunger);
// Function to simulate hunger over time
void hunger_simulate(Hunger *hunger, float time_passed);
// Function to display current hunger status
void hunger_display(const Hunger *hunger);
