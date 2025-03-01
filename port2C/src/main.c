#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "tileloader.h"

#define SCREEN_WIDTH 176
#define SCREEN_HEIGHT 208

void loadSounds(GameState* game) {
    // Load sound effects (replace with actual paths)
    for (int i = 0; i < 6; i++) {
        char path[50];
        snprintf(path, sizeof(path), "../sounds/sound%d.wav", i);
        game->sounds[i] = Mix_LoadWAV(path);
    }
}

void initGame(GameState* game) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    
    game->window = SDL_CreateWindow("IcePlus", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                   SCREEN_WIDTH * 3, SCREEN_HEIGHT * 3, SDL_WINDOW_SHOWN);
    game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderSetLogicalSize(game->renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    printf("Loading Sprites - ");
    fflush(stdout);
    loadSprites(game);
    printf("Done\n");
    fflush(stdout);
    //loadSounds(game);
    
    // Initialize game objects
    for (int i = 0; i < 10; i++) {
        game->objs[i] = (IceObject){0};
    }
    game->gameMode = 0;
    game->running = true;
}

void handleInput(GameState* game) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) game->running = false;
        
        if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_ESCAPE: game->running = false; break;
                case SDLK_LEFT:  game->lastKey = SDLK_LEFT; break;
                case SDLK_RIGHT: game->lastKey = SDLK_RIGHT; break;
                case SDLK_UP:    game->lastKey = SDLK_UP; break;
                case SDLK_DOWN:  game->lastKey = SDLK_DOWN; break;
                case SDLK_SPACE: game->lastKey = SDLK_SPACE; break;
            }
        }else if (e.type == SDL_KEYUP) {
            if (e.key.keysym.sym == game->lastKey) {
                game->lastKey = 0;
            }
        }
    }
}

void updateGame(GameState* game) {
    // Add game logic here based on gameMode
    // Example: Update player position based on lastKey
    IceObject* player = &game->objs[0];
    if (game->lastKey == SDLK_LEFT) player->x--;
    if (game->lastKey == SDLK_RIGHT) player->x++;
    if (game->lastKey == SDLK_UP) player->y--;
    if (game->lastKey == SDLK_DOWN) player->y++;
}

void renderGame(GameState* game) {
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
    SDL_RenderClear(game->renderer);
    
    // Render objects

            SDL_Rect dest = {40, 40, 24, 32};
            SDL_RenderCopy(game->renderer, game->sprites[18], NULL, &dest);
            dest.x = game->objs[0].x;
            dest.y = game->objs[0].y;
            SDL_RenderCopy(game->renderer, game->sprites[1], NULL, &dest);

    
    SDL_RenderPresent(game->renderer);
}

void cleanup(GameState* game) {
    for (int i = 0; i < SPRITE_COUNT; i++) {
        SDL_DestroyTexture(game->sprites[i]);
    }
    for (int i = 0; i < 6; i++) {
        Mix_FreeChunk(game->sounds[i]);
    }
    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);
    Mix_CloseAudio();
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    GameState game = {0};
    initGame(&game);
    game.objs[0].x = 100;
    game.objs[0].y = 100;
    while (game.running) {
        handleInput(&game);
        updateGame(&game);
        renderGame(&game);
        SDL_Delay(16);  // ~60 FPS
    }
    
    cleanup(&game);
    return 0;
}