//
// Created by marcus on 17/03/2026.
//

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <SDL3/SDL.h>
#include "food.h"
#include "player.h"

typedef enum
{
   HUNGER_GAME_GASSET_GameBg,
   HUNGER_GAME_GASSET_MenuBg,
   HUNGER_GAME_GASSET_Player,
   HUNGER_GAME_GASSET_Arena,
   HUNGER_GAME_GASSET_TextThings,
   HUNGER_GAME_GASSET_MenuItems,

   HUNGER_GAME_GASSET_CNT
} hunger_assets_enum;

typedef enum
{
   HUNGER_GAME_STATE_MainMenu,
   HUNGER_GAME_STATE_Paused,
   HUNGER_GAME_STATE_Playing,
   HUNGER_GAME_STATE_GameOver,
   HUNGER_GAME_STATE_Help,
   HUNGER_GAME_STATE_OverlaySettings,
   HUNGER_GAME_STATE_SettingsMenu,
   HUNGER_GAME_STATE_Leaderboard,

   HUNGER_GAME_STATE_CNT
} hunger_game_states_enum;

typedef struct
{
   SDL_Texture*            asset_textures[HUNGER_GAME_GASSET_CNT];
   player_t                player;
   hunger_game_states_enum game_state;
} hunger_game_t;


