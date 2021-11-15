#include "minitile.h"
#include "third_party/raylib.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define NEZ_TILEMAP_IMPLEMENTATION
#include "third_party/tilemap.h"

#define NEZ_TILESET_IMPLEMENTATION
#include "third_party/tileset.h"

#define RAYGUI_IMPLEMENTATION
#include "third_party/raygui.h"

#define ADDITIONAL_SIZE   140
#define MINITILE_VERSION  1.1

// Defines to fit the program
#define mt_sheet_t          Texture2D
#define mt_load_sheet(p)    LoadTexture(p)
#define mt_unload_sheet(s)  UnloadTexture(s)

typedef enum {
  LAYER_EDITOR = 0,
  LAYER_WINDOW = 1,
} Layers;

typedef struct mt_data {
  int grid_width;
  int grid_height;
  int tile_size;
  int screen_width;
  int screen_height;
  int scale;
  int layer;
  const char *info_message;
  bool display_exported;
  int frames_counter;

  // raylib-specific stuff
  mt_sheet_t sheet_loaded;
  Camera2D camera;

  // Tilemap-specific stuff
  TileMap *tilemap;
  TileSet *tileset;
  int tileset_index;
  int mouse_tile_x;
  int mouse_tile_y;
  bool grid_on;
} mt_data;

static void mt__update(void);
static void check_input(void);
static void mt_draw(void);
static void draw_tile_preview(void);
static void draw_gui_controls(void);
static void show_tileset(void);

mt_data DATA = { 0 };
mt_map_t MAP = { 0 };

void mt_init(int grid_width, int grid_height, int tile_size, const char *sheet_path)
{
  // Set some limitations first
  assert(grid_width >= 5 && grid_width <= 20);
  assert(grid_height >= 5 && grid_height <= 15);
  assert(tile_size >= 8 && tile_size <= 32);
  assert(sheet_path != NULL);
  const int scale = 2;

  const int screen_width = (scale*grid_width*tile_size) + ADDITIONAL_SIZE;
  const int screen_height = (scale*grid_height*tile_size) + ADDITIONAL_SIZE;
#ifndef MINITILE_DEBUG
  SetTraceLogLevel(LOG_NONE);
#endif
  SetConfigFlags(FLAG_VSYNC_HINT);
  InitWindow(screen_width, screen_height, "Minitile - Mini tilemap editor");
  SetExitKey(0);

  DATA.grid_width = grid_width;
  DATA.grid_height = grid_height;
  DATA.tile_size = tile_size;
  DATA.screen_width = screen_width;
  DATA.screen_height = screen_height;
  DATA.scale = scale;
  DATA.display_exported = false;
  DATA.frames_counter = 0;
  DATA.sheet_loaded = mt_load_sheet(sheet_path);
  DATA.camera = (Camera2D){
    .offset = (Vector2){ 0.0f, 0.0f },
    .target = (Vector2){ 0.0f, 0.0f },
    .rotation = 0.0f,
    .zoom = (float)scale,
  };
  DATA.layer = LAYER_EDITOR;

  if (FileExists(sheet_path)) {
    DATA.tileset = TileSetNewInitFromFile(sheet_path, tile_size, tile_size, NULL, 0);
    DATA.tileset_index = 0;
    DATA.tilemap = TileMapNew();
    DATA.tilemap->tileSet = DATA.tileset;
    DATA.tilemap->x = 0;
    DATA.tilemap->y = 0;
    DATA.mouse_tile_x = 0;
    DATA.mouse_tile_y = 0;
    DATA.grid_on = true;
    TileMapInitSize(DATA.tilemap, DATA.grid_width, DATA.grid_height);
    TileMapClearGrid(DATA.tilemap);
    MAP.handle = malloc(sizeof(int)*DATA.grid_width*DATA.grid_height);
    MAP.width = DATA.grid_width;
    MAP.height = DATA.grid_height;
    for (int i = 0; i < MAP.width*MAP.height; i++) {
      MAP.handle[i] = DATA.tilemap->grid[i];
    }
  } else {
    printf("minitile: tilesheet must exist.\nminitile: input value: \"%s\"\n", sheet_path);
    exit(1);
  }
}

bool mt_run(void)
{
  return !WindowShouldClose();
}

void mt_update(void)
{
  mt__update();
  mt_draw();
}

void mt_close(void)
{
  TileMapDestroy(DATA.tilemap);
  free(MAP.handle);
  mt_unload_sheet(DATA.sheet_loaded);
  CloseWindow();
}

mt_map_t mt_get_map(void)
{
  MAP.width = DATA.grid_width;
  MAP.height = DATA.grid_height;
  for (int i = 0; i < MAP.width*MAP.height; i++) {
    MAP.handle[i] = DATA.tilemap->grid[i];
  }
  return MAP;
}

static void mt__update(void)
{
  check_input();
  
  DATA.info_message = TextFormat("Minitile v%.1f\t\t\t\t\t\tTiles count: %d\nMap width: %d\t\t\t\tIndex: %d (use A and D)\nMap height: %d\t\t\t Cursor: %d,%d\n", MINITILE_VERSION, DATA.tileset->tileCount, DATA.grid_width, DATA.tileset_index, DATA.grid_height, DATA.mouse_tile_x, DATA.mouse_tile_y);

  if (DATA.display_exported) {
    DATA.frames_counter++;
    if (DATA.frames_counter >= 180) {
      DATA.display_exported = false;
      DATA.frames_counter = 0;
    }
  }
}

static void check_input(void)
{
  if (IsKeyPressed(KEY_A) && DATA.tileset_index > 0) {
    DATA.tileset_index--;
  } else if (IsKeyPressed(KEY_D) && DATA.tileset_index < DATA.tileset->tileCount - 1) {
    DATA.tileset_index++;
  }

  // Check whether the mouse tile position goes out of bounds
  // If so, don't register
  DATA.mouse_tile_x = (GetMouseX()/(DATA.tile_size*DATA.scale)) >= (DATA.grid_width)? (DATA.grid_width) : (GetMouseX()/(DATA.tile_size*DATA.scale));
  DATA.mouse_tile_y = (GetMouseY()/(DATA.tile_size*DATA.scale)) >= (DATA.grid_height)? (DATA.grid_height) : (GetMouseY()/(DATA.tile_size*DATA.scale));

  // Set the tile requested to the ID chosen
  if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
    TileMapSetTile(DATA.tilemap, DATA.mouse_tile_x, DATA.mouse_tile_y, DATA.tileset_index);
  }

  // Erase a tile placed
  if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
    TileMapSetTile(DATA.tilemap, DATA.mouse_tile_x, DATA.mouse_tile_y, -1);
  }
}

static void mt_draw(void)
{
  BeginDrawing();
    ClearBackground(DARKGRAY);
    BeginMode2D(DATA.camera);   // Draw everything inside this camera block
      TileMapDraw(DATA.tilemap);
      TileMapDrawGrid(DATA.tilemap, DATA.grid_on? LIGHTGRAY : BLANK);
      draw_tile_preview();
    EndMode2D();
    DrawText(DATA.info_message, 10, GetScreenHeight() - (ADDITIONAL_SIZE - 10), 20, WHITE);
    draw_gui_controls();
  EndDrawing();
}

static void draw_tile_preview(void)
{
  int col = DATA.tileset_index % DATA.tilemap->tileSet->collumns;
  int row = DATA.tileset_index / DATA.tilemap->tileSet->collumns;
  Rectangle tile_shown = { (float)col*DATA.tilemap->tileSet->tileX, (float)row*DATA.tilemap->tileSet->tileY, (float)DATA.tile_size, (float)DATA.tile_size };
  Vector2 position = { (float)(DATA.mouse_tile_x*DATA.tile_size), (float)(DATA.mouse_tile_y*DATA.tile_size) };
  if (DATA.mouse_tile_x < DATA.grid_width && DATA.mouse_tile_y < DATA.grid_height) {
    DrawTextureRec(DATA.tilemap->tileSet->texture, tile_shown, position, Fade(WHITE, 0.4f));
  }
}

static void draw_gui_controls(void)
{
  // Display grid
  Rectangle grid_check = { (float)(GetScreenWidth() - ADDITIONAL_SIZE + 10), 10.0f, 32.0f, 32.0f };
  DATA.grid_on = GuiCheckBox(grid_check, "", DATA.grid_on);
  DrawText("DISPLAY GRID", grid_check.x + grid_check.width + 5, grid_check.y + grid_check.height/2 - grid_check.y/2, 10, WHITE);

  // Clear
  Rectangle clear_button = { (float)(GetScreenWidth() - ADDITIONAL_SIZE + 10), 50.0f, 110.0f, 32.0f };
  if (GuiButton(clear_button, "Clear")) {
    TileMapClearGrid(DATA.tilemap);
  }

  // Show the tileset
  Rectangle tileset_button = { (float)(GetScreenWidth() - ADDITIONAL_SIZE + 10), 92.0f, 110.0f, 32.0f };
  if (GuiButton(tileset_button, "Show tileset")) {
    DATA.layer = LAYER_WINDOW;
  }

  // Export the map
  Rectangle export_button = { (float)(GetScreenWidth() - ADDITIONAL_SIZE + 10), 134.0f, 110.0f, 32.0f };
  if (GuiButton(export_button, "Export to .h file")) {
    mt_export_map(mt_get_map(), "mt_map.h");
    DATA.display_exported = true;
  }

  if (DATA.display_exported) {
    DrawText("Exported map to\n    mt_map.h!", export_button.x + 15, export_button.y + 40, 10, WHITE);
  }

  show_tileset();
}

static void show_tileset(void)
{
  if (DATA.layer == LAYER_WINDOW) {
    // Display a fake window
    GuiLock();
    Rectangle window = { 4.0f, 4.0f, (float)(GetScreenWidth() - 8.0f), (float)(GetScreenHeight() - 8.0f) };
    DrawRectangleRec(window, DARKGRAY);
    DrawRectangleLinesEx(window, 2, LIGHTGRAY);

    // Draw the tilesheet with an outline
    DrawTexture(DATA.tilemap->tileSet->texture, window.x + 4.0f, window.y + 4.0f, WHITE);
    Rectangle outline = { window.x + 4.0f, window.y + 4.0f, DATA.tilemap->tileSet->texture.width, DATA.tilemap->tileSet->texture.height };
    DrawRectangleLinesEx(outline, 1, BLACK);

    // Exit the window
    Rectangle exit_button = { window.width/2 - 110.0f/2, window.height - 110.0f/2, 110.0f, 32.0f };
    GuiUnlock();
    if (GuiButton(exit_button, "Go back")) {
      DATA.layer = LAYER_EDITOR;
    }
  }
}

