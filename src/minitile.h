#ifndef minitile_h
#define minitile_h

#ifndef __cplusplus
  #if !defined(bool)
    #include <stdbool.h>
  #endif
#endif

typedef struct mt_sheet_t {
  unsigned int id;
  int width;
  int height;
  int mipmaps;
} mt_sheet_t;

typedef struct mt_map_t {
  int *handle;
  int width;
  int height;
} mt_map_t;

#ifdef __cplusplus
extern "C" {
#endif
void mt_init(int grid_width, int grid_height, int tile_size, const char *sheet_path);
bool mt_run(void);
void mt_update(void);
void mt_close(void);
mt_map_t mt_get_map(void);

void mt_export_map(mt_map_t map, const char *filename);
void mt_take_screenshot(float x, float y, float width, float height, const char *filename);
#ifdef __cplusplus
}
#endif

#endif // minitile_h
