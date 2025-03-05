#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "tileloader.h"
#include "draw.h"

const SDL_Rect ScreenSpace = {0, 0, SCREEN_WIDTH*SCREEN_SIZE, SCREEN_HEIGHT*SCREEN_SIZE};

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
    game->gameMode = 3;
    game->running = true;
    game->foregoundTexture = drawNewTexture(game);

    prepareLevel(game);
    prepareEnemies(game);
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
            printf("key: %i\n", game->lastKey );
        }else if (e.type == SDL_KEYUP) {
            if (e.key.keysym.sym == game->lastKey) {
                game->lastKey = 0;
            }
        }
    }
}

void updateMainGame(GameState* game) {
    preUpdate(game);
    for (int i = 0; i < 10; i++)
    {
        switch(game->objs[i].type){
            case Player:
                updatePlayer(game,i);
            break;
            case IceBlock:
            case IceBlockCoin:
                updateBlocks(game, i);
            break;
            case IceBlockBreak:
            case IceBlockBreakCoin:
                updateBreakBlock(game, i);
            break;
            case EnemieFire:
            case EnemieSpiningFire:
                updateEnemies(game, i);
            break;
            case EnemieKillScore:
                updateKillScore(game, i);
            break;
            case PlayerDied:
                updatePlayerDied(game, i);
            break;
        }
    }
}

void renderMainGame(GameState* game) {
    drawSetTarget(game, game->foregoundTexture);
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 0);
    SDL_RenderClear(game->renderer);

    sortSprites(game);
    for (int i = 0; i < game->sortN; i++)
        drawSprite(game, game->sortIX[i]);
    

    drawResetTarget(game);
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 0);
    SDL_RenderClear(game->renderer);

    // Render objects
    SDL_RenderCopy(game->renderer, game->backgoundTexture, NULL, &ScreenSpace);
    SDL_RenderCopy(game->renderer, game->foregoundTexture, NULL, &ScreenSpace);

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
    while (game.running) {
        game.counter++;
        handleInput(&game);
        switch (game.gameMode)
        {
            case NextMode:
                if (SDL_GetTicks() > game.nextTime) {
                    game.gameMode = game.nextMode;
                }
            break;
            case SetupIntroScreen:
            case AnimateIntro:
            case PrepareGameLevel:
                printf("PrepareGameLevel: %i\n", game.counter);
                drawSetTarget(&game, game.foregoundTexture);
                gameStart(&game);
                drawResetTarget(&game);
                SDL_RenderCopy(game.renderer, game.foregoundTexture, NULL, &ScreenSpace);
            break;
            case ToBlack:
                printf("ToBlack: %i\n", game.counter);
                drawToPlayField(&game);
                drawResetTarget(&game);
                SDL_RenderCopy(game.renderer, game.foregoundTexture, NULL, &ScreenSpace);
            break;
            case ResetLevel:
                printf("ResetLevel: %i\n", game.counter);
                if (game.counter > 12) {
                    prepareLevel(&game);
                    game.gameMode = 4;
                }
            break;
            case MainGameLoop:
                updateMainGame(&game);
                renderMainGame(&game);
            break;
            
            default:
                break;
        }
        SDL_RenderPresent(game.renderer);
        SDL_Delay(60);  // ~60 FPS
    }
    
    cleanup(&game);
    return 0;
}