//
// Created by marcus on 18/03/2026.
//

#pragma once

#include <stdint.h>

#include "physfs.h"
#include "game_types.h"

void init_resources(char *const argv);
int16_t resources_load_textures(hunger_game_t * const game, const char * const texture_paths[]);
