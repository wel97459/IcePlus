#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "tileloader.h"
#include "draw.h"

void drawSpriteSimple(GameState* game, int objNum) {
    IceObject* obj = &game->objs[objNum];
    if(obj->type == 0) return;
    
    SDL_Rect dest = {obj->x, obj->y - spriteH[obj->look], 24, 32};
    SDL_RenderCopy(game->renderer, game->sprites[obj->look], NULL, &dest);
}

void drawBlockSimple(GameState* game, int look, int x, int y) {
    SDL_Rect dest = {x, y, 24, 32};
    // or get the dimensions in a rectangle
    SDL_QueryTexture(game->blocks[look], NULL, NULL, &dest.w, &dest.h);
    SDL_RenderCopy(game->renderer, game->blocks[look], NULL, &dest);
}

SDL_Texture* drawNewTexture(GameState* game){
    SDL_Texture* tex;
    // Create a new texture with the same properties as the one we are duplicating
    tex = SDL_CreateTexture(game->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
    return tex;
}

void drawSetTarget(GameState* game, SDL_Texture* target){
    // Save the current rendering target (will be NULL if it is the current window)
    game->renderTarget = SDL_GetRenderTarget(game->renderer);
    SDL_SetRenderTarget(game->renderer, target);
}

void drawResetTarget(GameState* game){
    SDL_SetRenderTarget(game->renderer, game->renderTarget);
}