#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "tileloader.h"
#include "draw.h"
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
    
    game->window = SDL_CreateWindow(
        "IcePlus", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH * SCREEN_SIZE, SCREEN_HEIGHT * SCREEN_SIZE
        , SDL_WINDOW_SHOWN
    );

    game->renderer = SDL_CreateRenderer(
        game->window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    loadSprites(game);

    //loadSounds(game);


    game->level = 0;
    game->gameMode = 0;
    game->running = true;
    game->foregoundTexture = drawNewTexture(game);

    prepareLevel(game);
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
    game->counter++;
    updatePlayer(game,0);
}

void renderGame(GameState* game) {
    drawSetTarget(game, game->foregoundTexture);
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 0);
    SDL_RenderClear(game->renderer);

    drawSprite(game, 0);
    // for(int i = 0; i < BLOCK_COUNT; i++)
    //     drawBlockSimple(game, i, 10+((i & 0x07)*24)+(i==7 ? 54 : 0), 100+((i>>3)*32));
    drawResetTarget(game);
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 0);
    SDL_RenderClear(game->renderer);
    // Render objects
        SDL_Rect dest = {0, 0, SCREEN_WIDTH*SCREEN_SIZE, SCREEN_HEIGHT*SCREEN_SIZE};
        SDL_RenderCopy(game->renderer, game->backgoundTexture, NULL, &dest);
        SDL_RenderCopy(game->renderer, game->foregoundTexture, NULL, &dest);
    
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
    int c=0;
    while (game.running) {
        if(c++>60){
            //test++;
            c=0;
        }
        handleInput(&game);
        updateGame(&game);
        renderGame(&game);
        SDL_Delay(32);  // ~60 FPS
    }
    
    cleanup(&game);
    return 0;
}