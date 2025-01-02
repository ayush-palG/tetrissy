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

#define BACKGROUND_COLOR     0x181818FF
#define RECT_COLOR           0xFFFFFFFF
#define RECT_BOUNDARY_COLOR  0x666666FF
#define GRID_COLOR           0x444444FF
#define BLOCK_COLOR          0x46F46FFF
#define BLOCK_BOUNDARY_COLOR 0x30B13DFF

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

typedef struct {
  int x, y;
} Coord;

SDL_Rect rects[4];

Coord dirs[4] = {
  {1,  0},
  {0, -1},
  {-1, 0},
  {0,  1},
};

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

int random_int_range(int low, int high)
{
  return (rand() % (high-low)) + low;
}

Dir random_dir(void)
{
  return (Dir) random_int_range(0, 4);
}

int coord_equals(Coord a, Coord b)
{
  return a.x == b.x && a.y == b.y;
}

int is_cell_empty(SDL_Rect *rects, Coord pos)
{
  for (size_t i = 0; i < 4; ++i) {
    Coord rect_coord = {rects[i].x, rects[i].y};
    if (coord_equals(rect_coord, pos)) return 0;
  }
  
  return 1;
}

void reset_block_rects(SDL_Rect *rects)
{
  for (size_t i = 0; i < 4; ++i) {
    rects[i] = (SDL_Rect) {0,0,0,0};
  }
}

void generate_random_block_set(SDL_Rect *rects)
{
  Coord pos = {120, 120};
  for (size_t i = 0; i < 4; ++i) {
    if (is_cell_empty(rects, pos)) {
      rects[i] = (SDL_Rect) {pos.x, pos.y, CELL_WIDTH, CELL_HEIGHT};
    } else {
      --i;
    }
    if (random_int_range(0, 2)) {
      pos.x += dirs[random_dir()].x * CELL_WIDTH;
    } else {
      pos.y += dirs[random_dir()].y * CELL_HEIGHT;
    }
  }

  SDL_Delay(1000);
}

void generate_blocks(SDL_Renderer *renderer)
{
  secc(SDL_SetRenderDrawColor(renderer, HEX_COLOR(BLOCK_COLOR)));

  reset_block_rects(rects);
  generate_random_block_set(rects);
  secc(SDL_RenderFillRects(renderer, rects, 4));

  secc(SDL_SetRenderDrawColor(renderer, HEX_COLOR(BLOCK_BOUNDARY_COLOR)));
  secc(SDL_RenderDrawRects(renderer, rects, 4));
}

int main(int argc, char *argv[])
{
  srand(time(0));
  
  secc(SDL_Init(SDL_INIT_VIDEO));

  SDL_Window *window = secp(SDL_CreateWindow("Tetrissy",
	     SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
	     SCREEN_WIDTH, SCREEN_HEIGHT,
	     SDL_WINDOW_RESIZABLE));

  SDL_Renderer *renderer = secp(SDL_CreateRenderer(
		   window, -1, SDL_RENDERER_ACCELERATED));

  int quit = 0;
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
	  
	} break;
	}
      } break;
      }
    }

    secc(SDL_SetRenderDrawColor(renderer, HEX_COLOR(BACKGROUND_COLOR)));
    secc(SDL_RenderClear(renderer));
    
    render_grids(renderer);
    generate_blocks(renderer);
    
    SDL_RenderPresent(renderer);
  }

  SDL_Quit();
  
  return 0;
}
