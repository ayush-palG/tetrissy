#include <SDL2/SDL.h>

#define TETRISSY_IMPLEMENTATION
#include "tetrissy.h"

Tile_Group tg = {0};

int main(void)
{
  srand(time(0));
  secc(SDL_Init(SDL_INIT_VIDEO));

  SDL_Window *window = secp(SDL_CreateWindow("Tetrissy",
	     SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
	     SCREEN_WIDTH, SCREEN_HEIGHT,
	     SDL_WINDOW_RESIZABLE));

  SDL_Renderer *renderer = secp(SDL_CreateRenderer(
		   window, -1, SDL_RENDERER_ACCELERATED));

  Bool quit = false;
  Bool is_running = true;
  uint32_t dt = SDL_GetTicks();
  add_tile(&tg);
  
  while (!quit) {
    SDL_Event event;
    
    while(SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT: {
	quit = true;
      } break;
      case SDL_KEYDOWN: {
	switch (event.key.keysym.sym) {
	case SDLK_q:
	  quit = true;
	  break;
	case SDLK_r:
	  for (size_t i = 0; i < tg.tiles_count; ++i) {
	      free(tg.tiles[i].blocks);
	  }
	  tg.tiles_count = 0;
	  add_tile(&tg);
	  break;
	case SDLK_a:
	  if (is_running) {
	    if (!is_tile_left_or_right(&tg)) tile_move_left(&tg);
	  }
	  break;
	case SDLK_d:
	  if (is_running) {
	    if (!is_tile_left_or_right(&tg)) tile_move_right(&tg);
	  }
	  break;
	case SDLK_SPACE:
	  is_running = (is_running == true) ? false : true;
	  break;
	}	    
      } break;
      }
    }

    if (is_running) {
      secc(SDL_SetRenderDrawColor(renderer, HEX_COLOR(BACKGROUND_COLOR)));
      secc(SDL_RenderClear(renderer));

      render_grids(renderer);
      render_tiles(renderer, &tg);
    
      SDL_RenderPresent(renderer);

    if (SDL_GetTicks() - dt > 500) {
      tile_move_down(&tg);
      if (is_tile_below(&tg)) add_tile(&tg);
      dt = SDL_GetTicks();
      }
    }
  }

  SDL_Quit();
  
  return 0;
}
