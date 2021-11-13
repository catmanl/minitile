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

#ifdef __cplusplus
extern "C" {
#endif
void mt_init(int grid_width, int grid_height, int tile_size, const char *sheet_path);
bool mt_run(void);
void mt_update(void);
void mt_close(void);
#ifdef __cplusplus
}
#endif

#endif // minitile_h
