#include <SDL.h>
#include <SDL_mixer.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "tileloader.h"
#include "draw.h"
#include "sound.h"
#include "csid.h"

void initGame(GameState* game) {
    
    #ifdef __SWITCH__
        // Initialize the default gamepad (which reads handheld mode inputs as well as the first connected controller)
        padInitializeDefault(&game->pad);
        // Configure our supported input layout: a single player with standard controller styles
        padConfigureInput(1, HidNpadStyleSet_NpadStandard);
        romfsInit();
    #endif

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    Mix_OpenAudio(DEFAULT_SAMPLERATE, MIX_DEFAULT_FORMAT, 1, 2048);

    game->window = SDL_CreateWindow(
        "IcePlus", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT
        , SDL_WINDOW_SHOWN
    );

    game->renderer = SDL_CreateRenderer(
        game->window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    game->renderTarget = SDL_GetRenderTarget(game->renderer);
    
    TTF_Init();
    
    game->font = TTF_OpenFont(FILE_LOC "raw/font.ttf", 14);
    if(game->font == NULL){
        printf("Failed to load Font.\n");
    }


    loadSprites(game);
    loadLogo(game);
    loadSounds(game);

    game->csid = cSID_LoadSID(FILE_LOC "raw/Iceblox_Plus.sid");
    Mix_HookMusic(cSID_play, &game->csid);

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
    
    #ifdef __SWITCH__
    padUpdate(&game->pad);
    uint64_t kHeld = padGetButtons(&game->pad);
        if(kHeld & HidNpadButton_Plus)
            game->running = false;
        else if(kHeld & HidNpadButton_A)
            game->lastKey = SDLK_RETURN;
        else if(kHeld & HidNpadButton_AnyLeft)
            game->lastKey = SDLK_LEFT;
        else if(kHeld & HidNpadButton_AnyRight)
            game->lastKey = SDLK_RIGHT;
        else if(kHeld & HidNpadButton_AnyUp)
            game->lastKey = SDLK_UP;
        else if(kHeld & HidNpadButton_AnyDown)
            game->lastKey = SDLK_DOWN;
        else
            game->lastKey = 0;

    #endif

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
    Uint32 start_time = SDL_GetTicks();
    Uint32 end_time;
    Uint32 delay_time;
    while (game.running) {
        #ifdef __SWITCH__
        appletMainLoop();
        #endif
        start_time = SDL_GetTicks();
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
                    cSID_initSubtune(&game.csid, 1);
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
                if(game.counter == 1)
                    cSID_initSubtune(&game.csid, 2);
                // if (game.counter == 10) {
                //     playSound(&game, 4);
                // }
                if (game.counter > 80) {
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
        end_time = SDL_GetTicks();
        delay_time = 1000 / 15 - (end_time - start_time);
        if(delay_time > 100) delay_time = 100;
        if (delay_time > 0) {
            SDL_Delay(delay_time);
        }
    }
    
    cleanup(&game);
    return 0;
}