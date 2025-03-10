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

    game->renderTarget = SDL_GetRenderTarget(game->renderer);

    TTF_Init();

    game->font = TTF_OpenFont("font.ttf", 14);
    if(game->font == NULL){
        printf("Failed to load Font.\n");
    }

    loadSprites(game);
    loadLogo(game);
    loadSounds(game);

    game->running = true;
    game->foregoundTexture = drawNewTexture(game);
    game->backgoundTexture = drawNewTexture(game);

    prepareIntro(game);
}

void handleInput(GameState* game) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) game->running = false;
        
        if (e.type == SDL_KEYDOWN) {
                game->lastKey = e.key.keysym.sym; 
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
    for (int i = 0; i < game->sortN; i++){
        drawShadow(game, game->sortIX[i]);
        drawSprite(game, game->sortIX[i]);
    }
    

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
    for (int i = 0; i < BLOCK_COUNT; i++) {
        SDL_DestroyTexture(game->blocks[i]);
    }

    for (int i = 0; i < 6; i++) {
        Mix_FreeChunk(game->sounds[i]);
    }

    SDL_DestroyTexture(game->logo);
    SDL_DestroyTexture(game->foregoundTexture);
    SDL_DestroyTexture(game->backgoundTexture);

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
                //printf("Waiting %i\n", game.counter);
                if (game.counter > 50) {
                    game.gameMode = game.nextMode;
                    game.counter = 0;
                }
                SDL_RenderCopy(game.renderer, game.backgoundTexture, NULL, &ScreenSpace);
                SDL_RenderCopy(game.renderer, game.foregoundTexture, NULL, &ScreenSpace);
            break;
            case SetupIntroScreen:
                setUpIntroScreen(&game);
                SDL_RenderCopy(game.renderer, game.backgoundTexture, NULL, &ScreenSpace);
                SDL_RenderCopy(game.renderer, game.foregoundTexture, NULL, &ScreenSpace);
            break;
            case AnimateIntro:
                animateIntro(&game);
                SDL_RenderCopy(game.renderer, game.backgoundTexture, NULL, &ScreenSpace);
                SDL_RenderCopy(game.renderer, game.foregoundTexture, NULL, &ScreenSpace);
            break;
            case PrepareGameLevel:
                //printf("PrepareGameLevel: %i\n", game.counter);
                gameStart(&game);
                SDL_RenderCopy(game.renderer, game.foregoundTexture, NULL, &ScreenSpace);
            break;
            case ToBlack:
                //printf("ToBlack: %i\n", game.counter);
                drawToPlayField(&game);
                //game.gameMode = 6;
            break;
            case ResetLevel:
                //printf("ResetLevel: %i\n", game.counter);
                if (game.counter > 12) {
                    prepareLevel(&game);
                    game.gameMode = 4;
                    resetGameClip(&game);
                }
            break;
            case MainGameLoop:
                //printf("MainGameLoop: %i\n", game.counter);
                updateMainGame(&game);
                renderMainGame(&game);
            break;
            case FinshedLevel:
                game.objs[0].look = (game.counter & 4) >> 2;
                renderMainGame(&game);
                if (game.counter == 10) {
                    playSound(&game, 4);
                }
                if (game.counter > 50) {
                    game.level++;
                    resetGameClip(&game);
                    game.gameMode = PrepareGameLevel;
                }
            break;
            case GameOver:
                gameOver(&game);
                SDL_RenderCopy(game.renderer, game.backgoundTexture, NULL, &ScreenSpace);
                SDL_RenderCopy(game.renderer, game.foregoundTexture, NULL, &ScreenSpace);
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