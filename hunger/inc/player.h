/**
 * @file    player.h
 * @author  tibbdev
 * @brief   Header file for player-related functions in the Hunger Game application.
 * @version 0.1
 * @date    2025-07-30
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

#include "hunger.h"

typedef enum
{
   PLAYER_IDLE,   // Player is idle
   PLAYER_MOVING, // Player is moving
   PLAYER_DEAD    // Player is dead
} PlayerState;

typedef enum
{
   PLAYER_HUNGER_OK,       // Player's hunger level is okay
   PLAYER_HUNGER_HUNGRY,   // Player's hunger level is rising
   PLAYER_HUNGER_STARVING, // Player's hunger level is critical
   PLAYER_HUNGER_STARVED   // Player is dead due to hunger
} PlayerHunger;

/**
 * @brief Structure to represent a player in the Hunger Game.
 */
typedef struct
{
   float        x;                     // Player's x-coordinate
   float        y;                     // Player's y-coordinate
   float        speed;                 // Player's movement speed
   float        size;                  // Player's size (width and height)
   float        consumption_rate;      // Rate at which the player consumes food
   float        starved_time;          // Time when the player started starving
   PlayerState  state;                 // Current state of the player
   PlayerHunger hunger_state;          // Current hunger state of the player
   float        hungry_threshold;      // Threshold for hunger state change
   float        starving_threshold;    // Threshold for starving state change
   float        starve_time_threshold; // Threshold for starvation time

   Hunger hunger; // Hunger structure to track hunger levels
} Player;

/**
 * @brief Initializes a player with default values.
 *
 * @param player Pointer to the Player structure to initialize.
 */
void player_init(Player *player, float max_hunger);

/**
 * @brief Updates the player's position based on input.
 *
 * @param player Pointer to the Player structure to update.
 * @param delta_time Time since the last update in seconds.
 */
void player_update(Player *player, float delta_time);

/**
 * @brief Moves the player by a specified amount in the x and y directions.
 *
 * @param player  Pointer to the Player structure to move.
 * @param dx
 * @param dy
 */
void player_move(Player *player, float dt, float dx, float dy);

/**
 * @brief Moves the player to a specified location.
 *
 * @param player  Pointer to the Player structure to move.
 * @param x
 * @param y
 */
void player_move_to(Player *player, float x, float y);

/**
 * @brief Displays the player's current state.
 *
 * @param player Pointer to the Player structure to display.
 */
void player_display(const Player *player);
