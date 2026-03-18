//
// Created by marcus on 17/03/2026.
//
#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "SDL3/SDL.h"

typedef struct
{
   int16_t  mouse_x;
   int16_t  mouse_y;

   uint16_t key_up : 1;
   uint16_t key_down : 1;
   uint16_t select : 1;

   uint16_t mouse_lbtn : 1;
   uint16_t RESERVED : 12;
} main_menu_t;

int16_t main_menu_tick(SDL_Renderer* renderer, main_menu_t *menu);
