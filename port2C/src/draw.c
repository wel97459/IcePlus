#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "tileloader.h"
#include "draw.h"

void setClip(GameState* game, int x, int y, int w, int h)
{
    SDL_Rect clip;
    clip.x = x;
    clip.y = y;
    clip.w = w;
    clip.h = h;
    SDL_RenderSetClipRect(game->renderer, &clip);
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

void drawImage(GameState* game, SDL_Texture* tex, int x, int y, int w, int h){
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;
    dest.w = w;
    dest.h = h;
    SDL_RenderCopy(game->renderer, tex, NULL, &dest);
}

void drawSpriteSimple(GameState* game, int objNum) {
    IceObject* obj = &game->objs[objNum];
    if(obj->type == 0) return;
    
    SDL_Rect dest = {obj->x, obj->y - spriteH[obj->look], 24, 32};
    SDL_RenderCopy(game->renderer, game->sprites[obj->look], NULL, &dest);
}

void drawSprite(GameState* game, int objNum) {
    IceObject* obj = &game->objs[objNum];
    if(obj->type == 0) return;

    int xClip = 24 - obj->x % 24;
    int yClipOffset = obj->y - spriteEffH[obj->look] + 48;
    int mapPos = obj->x / 24 + 12 * (obj->y >> 4);
    int mapPosRight = mapPos + 1;

    int yClip;
    for (yClip = (obj->y & 240) + 16; mapPos < 168 && (game->map[mapPos] & 3) == 0 && yClip < yClipOffset + 32; mapPos += 12) {
        yClip += 16;
    }

    if (yClip > yClipOffset) {
        yClip -= yClipOffset;
        if (yClip > spriteH[obj->look]) {
            yClip = spriteH[obj->look];
        }

        setClip(game, obj->x, obj->y - spriteEffH[obj->look] + 48, xClip, yClip);
        drawImage(game, game->sprites[obj->look], obj->x, obj->y - spriteEffH[obj->look] + 48, 24, spriteH[obj->look]);
    }

    if (xClip != 24) {
        yClipOffset = obj->y - spriteEffH[obj->look] + 48;

        for (yClip = (obj->y & 240) + 16; mapPosRight < 168 && (game->map[mapPosRight] & 3) == 0 && yClip < yClipOffset + 32; mapPosRight += 12) {
        yClip += 16;
        }

        if (yClip > yClipOffset) {
        yClip -= yClipOffset;
        if (yClip > spriteH[obj->look]) {
            yClip = spriteH[obj->look];
        }

        setClip(game, obj->x + xClip, obj->y - spriteEffH[obj->look] + 48, 24 - xClip, yClip);
        drawImage(game, game->sprites[obj->look], obj->x, obj->y - spriteEffH[obj->look] + 48, 24, spriteH[obj->look]);
        }
    }

    SDL_RenderSetClipRect(game->renderer, NULL);
}

void addBlock(GameState* game, int pos, int look) {
    drawSetTarget(game, game->backgoundTexture);

    int var4 = pos / 12;
    int var3 = pos % 12;
    game->map[pos] = look;
    int gnd = 2 * look - 2;
    if (game->map[pos + 11] != 0 && game->map[pos + 12] == 0) {
        gnd++;
    }

    int var6 = 16;
    if (game->map[pos + 12] == 0) {
        var6 = 32;
    }

    setClip(game, var3 * 24, 16 + var4 * 16, 24, var6);
    drawBlockSimple(game, gnd, var3 * 24, 16 + var4 * 16);
    if (game->map[pos + 1] == 0) {
        if (game->map[pos - 11] != 0) {
            gnd = 2 * game->map[pos - 11] - 1;
            var6 = 16;
            if (game->map[pos + 1] == 0) {
                var6 = 32;
            }

            setClip(game, 24 + var3 * 24, var4 * 16, 24, var6);
            drawBlockSimple(game, gnd, 24 + var3 * 24, var4 * 16);
        }

        if (game->map[pos + 13] == 0) {
            setClip(game, 24 + var3 * 24, 32 + var4 * 16, 12, 16);
            gnd = game->ground[pos * 4 + 4] * 2 + 8;
            drawBlockSimple(game, gnd, 24 + var3 * 24, 32 + var4 * 16);
            gnd = game->ground[pos * 4 + 6] * 2 + 8;
            drawBlockSimple(game, gnd, 24 + var3 * 24, 40 + var4 * 16);
        }
    }

    SDL_RenderSetClipRect(game->renderer, NULL);
    drawResetTarget(game);
}

void removeBlock(GameState* game, int mapPos) {
    drawSetTarget(game, game->backgoundTexture);

    int posY = mapPos / 12;
    int posX = mapPos % 12;
    if ((game->map[mapPos + 1] & 3) == 0) {
       if ((game->map[mapPos - 11] & 3) != 0) {
          int look = 2 * game->map[mapPos - 11] - 2;
          int clipH = 16;
          if (game->map[mapPos + 1] == 0) {
             clipH = 32;
          }

          setClip(game, 24 + posX * 24, posY * 16, 24, clipH);
          drawBlockSimple(game, look, 24 + posX * 24, posY * 16);
       }

       if ((game->map[mapPos + 13] & 3) == 0) {
          setClip(game, 24 + posX * 24, 32 + posY * 16, 12, 16);
          int gnd = game->ground[mapPos * 4 + 4] * 2 + 7;
          drawBlockSimple(game, gnd, 24 + posX * 24, 32 + posY * 16);
          gnd = game->ground[mapPos * 4 + 6] * 2 + 7;
          drawBlockSimple(game, gnd, 24 + posX * 24, 40 + posY * 16);
       }
    }

    setClip(game, posX * 24, 16 + posY * 16, 24, 32);
    if ((game->map[mapPos + 12] & 3) == 0) {
       int gnd = game->ground[mapPos * 4] * 2 + 7;
       if ((game->map[mapPos - 1] & 3) != 0) {
          gnd++;
       }

       drawBlockSimple(game, gnd, posX * 24, 32 + posY * 16);
       gnd = game->ground[mapPos * 4 + 2] * 2 + 7;
       if ((game->map[mapPos - 1] & 3) != 0) {
          gnd++;
       }

       drawBlockSimple(game, gnd, posX * 24, 40 + posY * 16);
       gnd = game->ground[mapPos * 4 + 1] * 2 + 7;
       drawBlockSimple(game, gnd, 12 + posX * 24, 32 + posY * 16);
       gnd = game->ground[mapPos * 4 + 3] * 2 + 7;
       drawBlockSimple(game, gnd, 12 + posX * 24, 40 + posY * 16);
    }

    if ((game->map[mapPos - 12] & 3) != 0) {
       int mapTile = 2 * (game->map[mapPos - 12] & 3) - 2;
       if ((game->map[mapPos - 1] & 3) != 0) {
          mapTile++;
       }

       drawBlockSimple(game, mapTile, posX * 24, posY * 16);
    } else {
       int gnd = game->ground[mapPos * 4 - 48] * 2 + 7;
       if ((game->map[mapPos - 13] & 3) != 0) {
          gnd++;
       }

       drawBlockSimple(game, gnd, posX * 24, 16 + posY * 16);
       gnd = game->ground[mapPos * 4 - 46] * 2 + 7;
       if ((game->map[mapPos - 13] & 3) != 0) {
          gnd++;
       }

       drawBlockSimple(game, gnd, posX * 24, 24 + posY * 16);
       gnd = game->ground[mapPos * 4 - 47] * 2 + 7;
       drawBlockSimple(game, gnd, 12 + posX * 24, 16 + posY * 16);
       gnd = game->ground[mapPos * 4 - 45] * 2 + 7;
       drawBlockSimple(game, gnd, 12 + posX * 24, 24 + posY * 16);
    }

    SDL_RenderSetClipRect(game->renderer, NULL);
    drawResetTarget(game);
}

void drawBlockSimple(GameState* game, int look, int x, int y) {
    SDL_Rect dest = {x, y, 24, 32};
    // or get the dimensions in a rectangle
    SDL_QueryTexture(game->blocks[look], NULL, NULL, &dest.w, &dest.h);
    SDL_RenderCopy(game->renderer, game->blocks[look], NULL, &dest);
}

int drawToBlack(GameState* game){
    SDL_Rect rect = {game->clipX, game->clipY, game->clipW, game->clipH};
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(game->renderer, &rect);

    game->clipX -= 8;
    game->clipY -= 10;
    game->clipW += 16;
    game->clipH += 20;
    if (game->clipX >= 0)
        return 0;

    game->clipX = 80;
    game->clipW = 16;
    game->clipY = 100;
    game->clipH = 8;

    return 1;
}


void drawToPlayField(GameState* game){
    drawSetTarget(game, game->foregoundTexture);
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 0);
    SDL_RenderClear(game->renderer);
    SDL_Rect rect = {game->clipX, game->clipY, game->clipW, game->clipH};

    drawImage(game, game->backgoundTexture, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    game->clipX -= 8;
    game->clipY -= 10;
    game->clipW += 16;
    game->clipH += 20;

    if (game->clipX < 0) {
        game->gameMode = 6;
    }
}