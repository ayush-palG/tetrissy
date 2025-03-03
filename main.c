#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL2/SDL.h>

#define TILES_CAPACITY 100

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

typedef size_t Color;

const Color GREEN  = 0x2C6E49FF;
const Color ORANGE = 0xFF8811FF;
const Color YELLOW = 0xFCFC62FF;
const Color ERIN   = 0x53FF45FF;
const Color PURPLE = 0x785589FF;
const Color GRAY   = 0xAAABBCFF;

typedef struct {
  int x, y;
} Rect;

typedef struct {
  SDL_Rect *blocks;
  size_t count;
  Color color;
} Tile;

typedef struct  {
  Tile tiles[TILES_CAPACITY];
  size_t tiles_count;
} Tile_Group;

Tile_Group tg = {0};

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

void render_tile(SDL_Renderer *renderer, Tile *tile)
{
  secc(SDL_SetRenderDrawColor(renderer, HEX_COLOR(tile->color)));
  secc(SDL_RenderFillRects(renderer, tile->blocks, tile->count));

  secc(SDL_SetRenderDrawColor(renderer, HEX_COLOR(GRID_COLOR)));
  secc(SDL_RenderDrawRects(renderer, tile->blocks, tile->count));
}

int main(void)
{
  srand(69);  
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
      }
    }

    secc(SDL_SetRenderDrawColor(renderer, HEX_COLOR(BACKGROUND_COLOR)));
    secc(SDL_RenderClear(renderer));
    
    SDL_RenderPresent(renderer);
  }

  SDL_Quit();
  
  return 0;
}
