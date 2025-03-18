#ifndef TETRISSY_H_
#define TETRISSY_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TILES_CAPACITY 100

#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 600
#define CELL_WIDTH  40
#define CELL_HEIGHT 40
#define BOARD_WIDTH  ((float) SCREEN_WIDTH  / CELL_WIDTH)
#define BOARD_HEIGHT ((float) SCREEN_HEIGHT / CELL_HEIGHT)

#define MIN_RECTS_IN_TILE 3
#define MAX_RECTS_IN_TILE 4

#define HEX_COLOR(hex)                 \
  ((hex) >> (3*8)) & 0xFF,	       \
  ((hex) >> (2*8)) & 0xFF,	       \
  ((hex) >> (1*8)) & 0xFF,             \
  ((hex) >> (0*8)) & 0xFF

typedef size_t Color;

#define GREEN  0x2C6E49FF
#define ORANGE 0xFF8811FF
#define YELLOW 0xFCFC62FF
#define ERIN   0x53FF45FF
#define PURPLE 0x785589FF
#define GRAY   0xAAABBCFF

#define BACKGROUND_COLOR     0x181818FF
#define GRID_COLOR           0x444444FF

typedef enum {
  false = 0,
  true,
} Bool;

typedef struct {
  int x, y;
} Coord;

const Coord dirs[4] = {
  {1,  0},
  {0, -1},
  {-1, 0},
  {0,  1},
};

typedef struct {
  SDL_Rect *blocks;
  size_t count;
  Color color;
} Tile;

typedef struct  {
  Tile tiles[TILES_CAPACITY];
  size_t tiles_count;
} Tile_Group;

void secc(int code);
void *secp(void *ptr);
size_t random_st_range(const size_t low, const size_t high);
Bool is_cell_empty(const Tile *tile, const Coord pos);

void tile_move_left(Tile_Group *tg);
void tile_move_right(Tile_Group *tg);
void tile_move_down(Tile_Group *tg);
void add_tile(Tile_Group *tg);
Bool is_in_contact_with_tile(Tile *tile1, Tile *tile2);
Bool is_tile_below(Tile_Group *tg);
Bool is_tile_left_or_right(Tile_Group *tg);

void render_grids(SDL_Renderer *renderer);
void render_tiles(SDL_Renderer *renderer, const Tile_Group *tg);

#endif // TETRISSY_H_

#ifdef TETRISSY_IMPLEMENTATION

void secc(int code)
{
  if (code < 0) {
    fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
    exit(1);
  }
}

void *secp(void *ptr)
{
  if (ptr == NULL) {
    fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
    exit(1);
  }
  return ptr;
}

size_t random_st_range(const size_t low, const size_t high)
{
  assert(high >= low);
  return (rand() % (high-low)) + low;
}

Bool is_cell_empty(const Tile *tile, const Coord pos)
{
  for (size_t i = 0; i < tile->count; ++i) {
    if (tile->blocks[i].x == pos.x && tile->blocks[i].y == pos.y) return false;
  }
  return true;
}

void tile_move_left(Tile_Group *tg)
{
  Tile *tile = &(tg->tiles[tg->tiles_count-1]);
  for (size_t i = 0; i < tile->count; ++i) {
    if (tile->blocks[i].x <= 0) return;
  }

  for (size_t i = 0; i < tile->count; ++i) {
    tile->blocks[i].x -= CELL_WIDTH;
  }
}

void tile_move_right(Tile_Group *tg)
{
  Tile *tile = &(tg->tiles[tg->tiles_count-1]);
  for (size_t i = 0; i < tile->count; ++i) {
    if (tile->blocks[i].x >= (SCREEN_WIDTH - CELL_WIDTH)) return;
  }

  for (size_t i = 0; i < tile->count; ++i) {
    tile->blocks[i].x += CELL_WIDTH;
  }
}

void tile_move_down(Tile_Group *tg)
{
  Tile *tile = &(tg->tiles[tg->tiles_count-1]);
  for (size_t i = 0; i < tile->count; ++i) {
    if (tile->blocks[i].y >= (SCREEN_HEIGHT - CELL_HEIGHT)) {
      add_tile(tg);
      return;
    }
  }
    
  for (size_t i = 0; i < tile->count; ++i) {
    tile->blocks[i].y += CELL_HEIGHT;
  }
}

void add_tile(Tile_Group *tg)
{
  Color colors[] = {GREEN, ORANGE, YELLOW, ERIN, PURPLE, GRAY};
  Coord pos = {.x = 40*random_st_range(3, 7), .y = 80};
  size_t rand_num = random_st_range(MIN_RECTS_IN_TILE, MAX_RECTS_IN_TILE);

  Tile tile = {0};
  tile.blocks = (SDL_Rect *) malloc(sizeof(SDL_Rect) * rand_num);
  tile.color = colors[random_st_range(0, 6)];
  /*
    This creates a random tile of blocks, I could also hard-code
    the blocks of tiles into memory and then fetch a random tile,
    but I like this more this way.
  */
  while (tile.count != rand_num) {
    if (is_cell_empty(&tile, pos)) {
      tile.blocks[tile.count++] = (SDL_Rect) {pos.x, pos.y, CELL_WIDTH, CELL_HEIGHT};
    }

    if (random_st_range(0, 2)) {
      pos.x += dirs[random_st_range(0, 4)].x * CELL_WIDTH;
    } else {
      pos.y += dirs[random_st_range(0, 4)].y * CELL_HEIGHT;
    }
  }

  assert(tg->tiles_count < TILES_CAPACITY);
  tg->tiles[tg->tiles_count++] = tile;
}

Bool is_in_contact_with_tile(Tile *tile1, Tile *tile2)
{
  for (size_t i = 0; i < tile1->count; ++i) {
    for (size_t j = 0; j < tile2->count; ++j) {
      if (tile2->blocks[j].x - tile1->blocks[i].x <= 40) return 1;
      if (tile2->blocks[j].y - tile1->blocks[i].y <= 40) return 1;
    }
  }
  return 0;
}

Bool is_tile_below(Tile_Group *tg)
{
  Tile *tile = &(tg->tiles[tg->tiles_count-1]);
  for (size_t i = 0; i < tg->tiles_count-1; ++i) {
    for (size_t j = 0; j < tg->tiles[i].count; ++j) {
      for (size_t k = 0; k < tile->count; ++k) {
	if (tile->blocks[k].x == tg->tiles[i].blocks[j].x) {
	  if (abs(tile->blocks[k].y - tg->tiles[i].blocks[j].y) <= CELL_HEIGHT) return 1;
	}
      }
    }
  }
  return 0;
}

Bool is_tile_left_or_right(Tile_Group *tg)
{
  Tile *tile = &(tg->tiles[tg->tiles_count-1]);
  for (size_t i = 0; i < tg->tiles_count-1; ++i) {
    for (size_t j = 0; j < tg->tiles[i].count; ++j) {
      for (size_t k = 0; k < tile->count; ++k) {
	if (tile->blocks[k].y == tg->tiles[i].blocks[j].y) {
	  if (abs(tile->blocks[k].x - tg->tiles[i].blocks[j].x) <= CELL_WIDTH) return 1;
	}
      }
    }
  }
  return 0;
}

void render_grids(SDL_Renderer *renderer)
{
  secc(SDL_SetRenderDrawColor(renderer, HEX_COLOR(GRID_COLOR)));

  for (size_t x = 0; x < BOARD_WIDTH; ++x) {
    secc(SDL_RenderDrawLine(renderer, x*CELL_WIDTH, 0, x*CELL_WIDTH, SCREEN_HEIGHT));
  }

  for (size_t y = 0; y < BOARD_HEIGHT; ++y) {
    secc(SDL_RenderDrawLine(renderer, 0, y*CELL_HEIGHT, SCREEN_WIDTH, y*CELL_HEIGHT));
  }
}

void render_tiles(SDL_Renderer *renderer, const Tile_Group *tg)
{
  for (size_t i = 0; i < tg->tiles_count; ++i) {
    secc(SDL_SetRenderDrawColor(renderer, HEX_COLOR(tg->tiles[i].color)));
    secc(SDL_RenderFillRects(renderer, tg->tiles[i].blocks, tg->tiles[i].count));

    secc(SDL_SetRenderDrawColor(renderer, HEX_COLOR(GRID_COLOR)));
    secc(SDL_RenderDrawRects(renderer, tg->tiles[i].blocks, tg->tiles[i].count));
  }
}

#endif // TETRISSY_IMPLEMENTATION
