#include "hunger.h"

#include <stddef.h>

#define DEFAULT_MAX_HUNGER 100 // Default maximum hunger level if not specified

void hunger_init(Hunger *hunger, int max_hunger)
{
    if (hunger == NULL)
    {
        return; // Handle null pointer
    }
    if (max_hunger <= 0)
    {
        max_hunger = DEFAULT_MAX_HUNGER; // Default maximum hunger level if invalid
    }
    hunger->hunger_level = max_hunger >> 1; // Initialize hunger level to half of max hunger
    hunger->max_hunger = max_hunger;   // Set maximum hunger level
}

void hunger_eat(Hunger *hunger, int food_amount)
{
    if (hunger == NULL || food_amount < 0)
    {
        return; // Handle null pointer or invalid food amount
    }
    hunger->hunger_level -= food_amount; // Decrease hunger level by food amount
    if (hunger->hunger_level < 0)
    {
        hunger->hunger_level = 0; // Ensure hunger level does not go below zero
    }
}

bool hunger_is_low(const Hunger *hunger, int threshold)
{
    if (hunger == NULL || threshold < 0)
    {
        return false; // Handle null pointer or invalid threshold
    }
    if (threshold > hunger->max_hunger)
    {
        return false; // If threshold is greater than max hunger, return false
    }
    return hunger->hunger_level < threshold; // Check if hunger level is below threshold
}

int hunger_get_level(const Hunger *hunger)
{
    if (hunger == NULL)
    {
        return -1; // Handle null pointer, return -1 as an error code
    }
    return hunger->hunger_level; // Return current hunger level
}

void hunger_reset(Hunger *hunger)
{
    if (hunger == NULL)
    {
        return; // Handle null pointer
    }
    hunger->hunger_level = hunger->max_hunger; // Reset hunger level to maximum
}

void hunger_increase(Hunger *hunger, int amount)
{
    if (hunger == NULL || amount < 0)
    {
        return; // Handle null pointer or invalid amount
    }
    hunger->hunger_level += amount; // Increase hunger level by specified amount
    if (hunger->hunger_level > hunger->max_hunger)
    {
        hunger->hunger_level = hunger->max_hunger; // Ensure hunger level does not exceed max
    }
}

bool hunger_is_full(const Hunger *hunger)
{
    if (hunger == NULL)
    {
        return false; // Handle null pointer
    }
    return hunger->hunger_level >= hunger->max_hunger; // Check if hunger level is at maximum
}

void hunger_simulate(Hunger *hunger, int time_passed)
{
    if (hunger == NULL || time_passed < 0)
    {
        return; // Handle null pointer or invalid time passed
    }
    // Simulate hunger increase over time, e.g., increase by 1 for each time unit
    hunger->hunger_level += time_passed;
    if (hunger->hunger_level > hunger->max_hunger)
    {
        hunger->hunger_level = hunger->max_hunger; // Ensure hunger level does not exceed max
    }
}

#include <stdio.h>
void hunger_display(const Hunger *hunger)
{
    if (hunger == NULL)
    {
        return; // Handle null pointer
    }
    // Display current hunger status
    printf("Current Hunger Level: %d/%d\n", hunger->hunger_level, hunger->max_hunger);
}
