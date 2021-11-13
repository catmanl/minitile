#include "minitile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void show_help(void);

int main(int argc, char **argv)
{
  if (argc < 5) {
    show_help();
    return -1;
  }

  int width = atoi(argv[1]);
  int height = atoi(argv[2]);
  int tile_size = atoi(argv[3]);
  const char *sheet = argv[4];

  mt_init(width, height, tile_size, sheet);

  while (mt_run()) {
    mt_update();
  }

  mt_close();
  return 0;
}

void show_help(void)
{
  printf("Usage: minitile [grid_width] [grid_height] [tile_size] [tileset_path] (file_saved)\n");
  printf("Where [] are mandatory and () are optional.\n");
  printf("Arguments help:\n");
  printf("  [grid_width] - Width of the tilemap. Must be greater than 5 and lower than 20\n");
  printf("  [grid_height] - Height of the tilemap. Must be greater than 5 and lower than 15\n");
  printf("  [tile_size] - Size of each tile in the tileset. Must be greater than 8 and lower than 32.\n");
  printf("                WARNING: the program will take this value both for x and y\n");
  printf("  [tileset_path] - Path to tileset spritesheet\n");
  printf("  (file_saved) - Path to a .mtile file\n");
}

