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
   int hunger_level; // Current hunger level
   int max_hunger;   // Maximum hunger level
} Hunger;

// Function to initialize hunger
void hunger_init(Hunger *hunger, int max_hunger);
// Function to eat food and decrease hunger level
void hunger_eat(Hunger *hunger, int food_amount);
// Function to check if the hunger level is below a threshold
bool hunger_is_low(const Hunger *hunger, int threshold);
// Function to get the current hunger level
int hunger_get_level(const Hunger *hunger);
// Function to reset hunger level
void hunger_reset(Hunger *hunger);
// Function to increase hunger level over time
void hunger_increase(Hunger *hunger, int amount);
// Function to check if hunger is at maximum
bool hunger_is_full(const Hunger *hunger);
// Function to simulate hunger over time
void hunger_simulate(Hunger *hunger, int time_passed);
// Function to display current hunger status
void hunger_display(const Hunger *hunger);
