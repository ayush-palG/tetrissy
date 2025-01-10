#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL2/SDL.h>

#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 600

#define CELL_WIDTH  40
#define CELL_HEIGHT 40

#define BOARD_WIDTH  ((float) SCREEN_WIDTH  / CELL_WIDTH)
#define BOARD_HEIGHT ((float) SCREEN_HEIGHT / CELL_HEIGHT)

#define MIN_RECTS_IN_TILE 3
#define MAX_RECTS_IN_TILE 4

#define BACKGROUND_COLOR     0x181818FF
#define GRID_COLOR           0x444444FF

#define HEX_COLOR(hex)                 \
  ((hex) >> (3*8)) & 0xFF,	       \
  ((hex) >> (2*8)) & 0xFF,	       \
  ((hex) >> (1*8)) & 0xFF,             \
  ((hex) >> (0*8)) & 0xFF

typedef enum {
  DIR_RIGHT = 0,
  DIR_UP,
  DIR_LEFT,
  DIR_BOTTOM,
} Dir;

typedef enum {
  GREEN  = 0x2C6E49FF,
  ORANGE = 0xFF8811FF,
  YELLOW = 0xFCFC62FF,
  ERIN   = 0x53FF45FF,
  PURPLE = 0x785589FF,
  GRAY   = 0xAAABBCFF
} Color;

typedef struct {
  int x, y;
} Coord;

typedef struct {
  int count;
  Color color;
  SDL_Rect *blocks;
} Tile;

typedef struct tile_group {
  Tile *tile;
  struct tile_group *next_tile;
} Tile_Group;

Coord dirs[4] = {
  {1,  0},
  {0, -1},
  {-1, 0},
  {0,  1},
};

Tile_Group *last_tile = NULL;

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

int random_int_range(int low, int high)
{
  return (rand() % (high-low)) + low;
}

Dir random_dir(void)
{
  return (Dir) random_int_range(0, 4);
}

Color random_color(void)
{
  Color colors[] = {GREEN, ORANGE, YELLOW, ERIN, PURPLE, GRAY};
  return colors[random_int_range(0, 6)];
}

int coord_equals(Coord a, Coord b)
{
  return a.x == b.x && a.y == b.y;
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

int is_cell_empty(Tile *tile, Coord pos)
{
  for (size_t i = 0; i < tile->count; ++i) {
    Coord rect_coord = {tile->blocks[i].x, tile->blocks[i].y};
    if (coord_equals(rect_coord, pos)) return 0;
  }
  
  return 1;
}

void reset_tile_rects(Tile *tile)
{
  for (size_t i = 0; i < tile->count; ++i) {
    tile->blocks[i] = (SDL_Rect) {0,0,0,0};
  }
}

void generate_random_tile_set(Tile *tile)
{
  Coord pos = {40*random_int_range(3, 7), 120};
  for (size_t i = 0; i < tile->count; ++i) {
    if (is_cell_empty(tile, pos)) {
      tile->blocks[i] = (SDL_Rect) {pos.x, pos.y, CELL_WIDTH, CELL_HEIGHT};
    } else {
      --i;
    }
    
    if (random_int_range(0, 2)) {
      pos.x += dirs[random_dir()].x * CELL_WIDTH;
    } else {
      pos.y += dirs[random_dir()].y * CELL_HEIGHT;
    }
  }
}

Tile generate_boundaries(void)
{
  Tile boundary_tile;
  boundary_tile.count = BOARD_WIDTH + (2 * BOARD_HEIGHT);
  boundary_tile.blocks = malloc(sizeof(*(boundary_tile.blocks)) * boundary_tile.count);

  int index = 0;
  for (size_t i = 0; i < BOARD_WIDTH; ++i, ++index) {
    boundary_tile.blocks[index] = (SDL_Rect) {i*CELL_WIDTH, SCREEN_HEIGHT, CELL_WIDTH, CELL_HEIGHT};
  }
  
  for (size_t i = 0; i < BOARD_HEIGHT; ++i, ++index) {
    boundary_tile.blocks[index] = (SDL_Rect) {-40, i*CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT};
  }
  
  for (size_t i = 0; i < BOARD_HEIGHT; ++i, ++index) {
    boundary_tile.blocks[index] = (SDL_Rect) {SCREEN_WIDTH, i*CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT};
  }
  
  return boundary_tile;
}

void render_tile(SDL_Renderer *renderer, Tile *tile)
{
  secc(SDL_SetRenderDrawColor(renderer, HEX_COLOR(tile->color)));
  secc(SDL_RenderFillRects(renderer, tile->blocks, tile->count));

  secc(SDL_SetRenderDrawColor(renderer, HEX_COLOR(GRID_COLOR)));
  secc(SDL_RenderDrawRects(renderer, tile->blocks, tile->count));
}

Tile *init_tile(void)
{
  Tile *tile = (Tile *) malloc(sizeof(Tile));
  tile->count = random_int_range(MIN_RECTS_IN_TILE, MAX_RECTS_IN_TILE+1);
  tile->color = random_color();
  tile->blocks = malloc(sizeof(*(tile->blocks))*tile->count);
  reset_tile_rects(tile); // TODO: Check if it's required
  return tile;
}

void push_block(Block **block)
void push_tile_group(Tile_Group **tile_group)
{
  Tile_Group *new_tile_group = (Tile_Group *) malloc(sizeof(Tile_Group));
  new_tile_group->tile = init_tile();
  new_tile_group->next_tile = NULL;

  if (*tile_group == NULL) {
    *tile_group = new_tile_group;
  } else {
    last_tile = *tile_group;
    while (last_tile->next_tile) {
      last_tile = last_tile->next_tile;
    }
    last_tile->next_tile = new_tile_group;
  }
}

Tile_Group *traverse_tile_group(Tile_Group *tile_group)
{
  assert(tile_group);
  while (tile_group->next_tile) {
    tile_group = tile_group->next_tile;
  }
  return tile_group;
}

void free_block(Block *block)
{
  assert(block);
  block = block->next_tile;
  
  Block *next_block = NULL;
  while(block) {
    next_block = block->next_tile;
    free(block);
    block = next_block;
  }
}

void render_tile_group(SDL_Renderer *renderer, Tile_Group *tile_group)
{
  assert(tile_group);
  
  while (tile_group) {
    render_tile(renderer, tile_group->tile);
    tile_group = tile_group->next_tile;
  }
}

void tile_step(Tile_Group *tile_group, Tile *tile)
{
  for (size_t i = 0; i < tile->count; ++i) {
    if (tile->blocks[i].y >= (SCREEN_HEIGHT - CELL_HEIGHT)) {
      push_tile_group(&tile_group);
      last_tile = traverse_tile_group(tile_group);
      generate_random_tile_set(last_tile->tile);
      return;
    }
  }

  for (size_t i = 0; i < tile->count; ++i) {
     tile->blocks[i].y += CELL_HEIGHT;
  }
}

void down_to_earth(Tile *tile)
{
  int distance = SCREEN_HEIGHT;
  for (size_t i = 0; i < tile->count; ++i) {
    if ((SCREEN_HEIGHT - tile->blocks[i].y) < distance) {
      distance = SCREEN_HEIGHT - tile->blocks[i].y;
    }
  }

  for (size_t i = 0; i < tile->count; ++i) {
    tile->blocks[i].y += distance - CELL_HEIGHT;
  }
}

void tile_move_left(Tile *tile)
{
  for (size_t i = 0; i < tile->count; ++i) {
    if (tile->blocks[i].x <= 0) return;
  }

  for (size_t i = 0; i < tile->count; ++i) {
    tile->blocks[i].x -= CELL_WIDTH;
  }
}

void tile_move_right(Tile *tile)
{
  for (size_t i = 0; i < tile->count; ++i) {
    if (tile->blocks[i].x >= (SCREEN_WIDTH - CELL_WIDTH)) return;
  }

  for (size_t i = 0; i < tile->count; ++i) {
    tile->blocks[i].x += CELL_WIDTH;
  }
}

Tile_Group *tile_group = NULL;

int main(int argc, char *argv[])
{
  srand(time(0));
  push_tile_group(&tile_group);
  last_tile = traverse_tile_group(tile_group);
  generate_random_tile_set(last_tile->tile);
  
  secc(SDL_Init(SDL_INIT_VIDEO));

  SDL_Window *window = secp(SDL_CreateWindow("Tetrissy",
	     SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
	     SCREEN_WIDTH, SCREEN_HEIGHT,
	     SDL_WINDOW_RESIZABLE));

  SDL_Renderer *renderer = secp(SDL_CreateRenderer(
		   window, -1, SDL_RENDERER_ACCELERATED));

  Tile boundary_tile = generate_boundaries();
  Uint32 dt = SDL_GetTicks();

  int quit = 0;
  int is_stopped = 0;
  while (!quit) {
    SDL_Event event;
    
    while(SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT: {
	quit = 1;
      } break;

      case SDL_KEYDOWN: {
	switch(event.key.keysym.sym) {
	case SDLK_r: {
	  //free_tile_group(tile_group);
	  //push_tile_group(&tile_group);
	  //last_tile_group = traverse_tile_group(tile_group);
	  //generate_random_tile_set(last_tile_group->tile);
	} break;

	case SDLK_q: {
	  quit = 1;
	} break;

	case SDLK_a: {
	  last_tile = traverse_tile_group(tile_group);
	  tile_move_left(last_tile->tile);
	} break;
	case SDLK_d: {
	  last_tile = traverse_tile_group(tile_group);
	  tile_move_right(last_tile->tile);
	} break;
	case SDLK_w: {
	  last_tile = traverse_tile_group(tile_group);
	  rotate_tile(last_tile->tile);
	} break;
	case SDLK_s: {
	  last_tile = traverse_tile_group(tile_group);
	  down_to_earth(last_tile->tile);
	} break;

	case SDLK_SPACE: {
	  is_stopped = !is_stopped;
	} break;
	}
      } break;
      }
    }

    if (!is_stopped) {
      secc(SDL_SetRenderDrawColor(renderer, HEX_COLOR(BACKGROUND_COLOR)));
      secc(SDL_RenderClear(renderer));

      render_grids(renderer);
      render_tile(renderer, &boundary_tile);
    
      render_tile_group(renderer, tile_group);
    
      SDL_RenderPresent(renderer);

      if (SDL_GetTicks() - dt > 300) {
	last_tile = traverse_tile_group(tile_group);
	tile_step(tile_group, last_tile->tile);
	dt = SDL_GetTicks();
    }
    }
  }

  SDL_Quit();
  
  return 0;
}
