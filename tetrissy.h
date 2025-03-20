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
Bool is_tile_left_or_right(Tile_Group *tg);
Bool is_tile_on_bottom(const Tile *tile);
Bool is_tile_below(Tile_Group *tg, Tile *tile);

void tile_move_left(Tile_Group *tg);
void tile_move_right(Tile_Group *tg);
void tiles_move_down(Tile_Group *tg);

void add_tile(Tile_Group *tg);
void add_new_tile_on_screen(Tile_Group *tg);

void remove_block_from_tile(Tile *tile, size_t block_index)
void remove_tile_blocks_from_column(Tile_Group *tg, size_t column);
void remove_tiles(Tile_Group *tg);

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

Bool is_tile_on_bottom(const Tile *tile)
{
  for (size_t i = 0; i < tile->count; ++i) {
    if (tile->blocks[i].y >= (SCREEN_HEIGHT - CELL_HEIGHT)) {
      return true;
    }
  }
  return false;
}

Bool is_tile_below(Tile_Group *tg, Tile *tile)
{
  for (size_t i = 0; i < tg->tiles_count-1; ++i) {
    if (&tg->tiles[i] != tile) {
      for (size_t j = 0; j < tg->tiles[i].count; ++j) {
	for (size_t k = 0; k < tile->count; ++k) {
	  if (tile->blocks[k].x == tg->tiles[i].blocks[j].x) {
	    if (tg->tiles[i].blocks[j].y - tile->blocks[k].y == CELL_HEIGHT) return 1;
	  }
	}
      }
    }
  }
  return 0;
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

void tiles_move_down(Tile_Group *tg)
{
  for (size_t i = 0; i < tg->tiles_count; ++i) {
    Tile *tile = &(tg->tiles[i]);
    if (!is_tile_below(tg, tile) && is_tile_on_bottom(tile) == false) {
      for (size_t i = 0; i < tile->count; ++i) {
	tile->blocks[i].y += CELL_HEIGHT;
      }
    }
  }
}

void add_tile(Tile_Group *tg)
{
  Color colors[] = {GREEN, ORANGE, YELLOW, ERIN, PURPLE, GRAY};
  Coord pos = {.x = 40*random_st_range(3, 7), .y = 80};
  size_t rand_num = random_st_range(MIN_RECTS_IN_TILE, MAX_RECTS_IN_TILE+1);

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

void add_new_tile_on_screen(Tile_Group *tg)
{
  Tile *tile = &(tg->tiles[tg->tiles_count-1]);
  for (size_t i = 0; i < tile->count; ++i) {
    if ((tile->blocks[i].y >= (SCREEN_HEIGHT - CELL_HEIGHT)) || is_tile_below(tg, tile)) {
      add_tile(tg);
      return;
    }
  }
}

void remove_block_from_tile(Tile *tile, size_t block_index)
{
  SDL_Rect *new_blocks= (SDL_Rect *) malloc(sizeof(SDL_Rect) * (tile->count-1));

  for (size_t i = 0, j = 0; i < tile->count; ++i) {
    if (i != block_index) new_blocks[j++] = tile->blocks[i];
  }
  free(tile->blocks);
  tile->blocks = new_blocks;
  tile->count -= 1;
}

void remove_tile_blocks_from_column(Tile_Group *tg, size_t column)
{
  for (size_t i = 0; i < tg->tiles_count-1; ++i) {
    for (size_t j = 0; j < tg->tiles[i].count; ++j) {
      int y = tg->tiles[i].blocks[j].y / CELL_HEIGHT;
      if (y == (int) column) {
	// Remove the tile_block from the tile of the tile_group
	remove_block_from_tile(&(tg->tiles[i]), j--);
      }
    }
  }
}

void remove_tiles(Tile_Group *tg)
{
  int *tiled_cells = (int *) malloc(sizeof(int) * BOARD_WIDTH * BOARD_HEIGHT);
  for (size_t i = 0; i < (size_t) (BOARD_WIDTH * BOARD_HEIGHT); ++i) tiled_cells[i] = 0;

  for (size_t i = 0; i < tg->tiles_count-1; ++i) {
    for (size_t j = 0; j < tg->tiles[i].count; ++j) {
      int x = tg->tiles[i].blocks[j].x / CELL_WIDTH;
      int y = tg->tiles[i].blocks[j].y / CELL_HEIGHT;
      tiled_cells[y*(int)BOARD_WIDTH + x] = 1;
    }
  }

  for (size_t y = 0; y < (size_t) BOARD_HEIGHT; ++y) {
    size_t counter = 0;
    for (size_t x = 0; x < (size_t) BOARD_WIDTH; ++x) {
      if (tiled_cells[y*(int)BOARD_WIDTH + x] == 1) counter += 1;
    }
    if (counter >= 10) remove_tile_blocks_from_column(tg, y);
  }

  free(tiled_cells);
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
