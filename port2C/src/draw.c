#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "tileloader.h"
#include "draw.h"

void drawSpriteSimple(GameState* game, int objNum) {
    IceObject* obj = &game->objs[objNum];
    if(obj->type == 0) return;
    
    SDL_Rect dest = {obj->x, obj->y - spriteH[obj->look], 24, 32};
    SDL_Rect clip = {obj->x, obj->y - spriteH[obj->look], 24, 16};
    SDL_RenderSetClipRect(game->renderer, &clip);
    SDL_RenderCopy(game->renderer, game->sprites[obj->look], NULL, &dest);
    SDL_RenderSetClipRect(game->renderer, NULL);
}

void drawSprite(GameState* game, int objNum) {
    IceObject* obj = &game->objs[objNum];
    if(obj->type == 0) return;

    SDL_Rect dest;
    SDL_Rect clip;

    int xClip = 24 - obj->x % 24;
    int yClipOffset = obj->y - spriteEffH[obj->look] + 48;
    int mapPos = obj->x / 24 + 12 * (obj->y >> 4);
    int mapPosRight = mapPos + 1;

    int yclip;
    for (yclip = (obj->y & 240) + 16; mapPos < 168 && (game->map[mapPos] & 3) == 0 && yclip < yClipOffset + 32; mapPos += 12) {
        yclip += 16;
    }

    if (yclip > yClipOffset) {
        yclip -= yClipOffset;
        if (yclip > spriteH[obj->look]) {
            yclip = spriteH[obj->look];
        }

        clip.x = obj->x;
        clip.y =obj->y - spriteEffH[obj->look] + 48;
        clip.w = xClip;
        clip.h = yclip;
        SDL_RenderSetClipRect(game->renderer, &clip);

        dest.x = obj->x;
        dest.y = obj->y - spriteEffH[obj->look] + 48;
        dest.w = 24;
        dest.h = spriteH[obj->look];
        SDL_RenderCopy(game->renderer, game->sprites[obj->look], NULL, &dest);
    }

    if (xClip != 24) {
        yClipOffset = obj->y - spriteEffH[obj->look] + 48;

        for (yclip = (obj->y & 240) + 16; mapPosRight < 168 && (game->map[mapPosRight] & 3) == 0 && yclip < yClipOffset + 32; mapPosRight += 12) {
        yclip += 16;
        }

        if (yclip > yClipOffset) {
        yclip -= yClipOffset;
        if (yclip > spriteH[obj->look]) {
            yclip = spriteH[obj->look];
        }

        clip.x = obj->x + xClip;
        clip.y = obj->y - spriteEffH[obj->look] + 48;
        clip.w = 24 - xClip;
        clip.h = yclip;
        SDL_RenderSetClipRect(game->renderer, &clip);

        dest.x = obj->x;
        dest.y = obj->y - spriteEffH[obj->look] + 48;
        dest.w = 24;
        dest.h = spriteH[obj->look];
        SDL_RenderCopy(game->renderer, game->sprites[obj->look], NULL, &dest);
        }
    }

    SDL_RenderSetClipRect(game->renderer, NULL);
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