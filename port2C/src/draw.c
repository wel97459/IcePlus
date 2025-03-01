#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "tileloader.h"
#include "draw.h"

void drawSpriteSimple(GameState* game, int var1) {
    IceObject* obj = &game->objs[var1];
    SDL_Rect dest = {obj->x, obj->y - spriteH[obj->look], 24, 32};
    SDL_RenderCopy(game->renderer, game->sprites[obj->look], NULL, &dest);
 }