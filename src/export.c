#include "minitile.h"
#include <stdio.h>

void mt_export_map(mt_map_t map, const char *filename)
{
  FILE *f = fopen(filename, "w");
  int total = map.width*map.height;

  fprintf(f, "int mt_map[%d] = {\n", total);
  fprintf(f, "  ");

  for (int i = 0; i < total; i++) {
    fprintf(f, "%d, ", map.handle[i]);
    if ((i + 1) % map.width == 0) {
      fprintf(f, "\n  ");
    }
  }
  fprintf(f, "\n};\n");

  fclose(f);
}

