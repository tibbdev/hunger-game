//
// Created by marcus on 18/03/2026.
//

#include "resources.h"

#include <stdio.h>

#define EXE_PATH_LEN_MAX 512

void init_resources(char *const argv)
{
   //
   PHYSFS_init(argv);

   const char * base_dir = PHYSFS_getBaseDir();
   char exe_path[1024];
   snprintf(exe_path, EXE_PATH_LEN_MAX - 1, "%s%s", base_dir, "hunger.exe");

   if (PHYSFS_exists(exe_path))
   {
      if (!PHYSFS_mount(exe_path, "/", 1))
      {
         printf("Failed to mount\r\n");
      }
   }
}

int16_t resources_load_textures(hunger_game_t *const game, const char *const texture_paths[])
{
   if ((NULL == game) || (NULL == texture_paths))
   {
      return -1;
   }

   if (!PHYSFS_isInit())
   {
      init_resources(NULL);
   }



   return -2; // not implemented
}