#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include "tileloader.h"
#include "draw.h"

void IncrmentGameClip(GameState* game){
    game->clipX -= 10;
    game->clipY -= 9;
    game->clipW += 20;
    game->clipH += 17;
    if (game->clipX < 0)
    {
        game->clipX = -1;
        game->clipY = -1;
        game->clipW = SCREEN_WIDTH+1;
        game->clipH = SCREEN_HEIGHT+1;
    }
    
}

void resetGameClip(GameState* game)
{
    game->clipW = 50 * (SCREEN_WIDTH/SCREEN_HEIGHT);
    game->clipX = (SCREEN_WIDTH>>1) - (game->clipW>>1);
    game->clipH = 50;
    game->clipY = (SCREEN_HEIGHT>>1) - (game->clipH>>1);
}

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

void drawImageXY(GameState* game, SDL_Texture* tex, int x, int y){
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;
    SDL_QueryTexture(tex, NULL, NULL, &dest.w, &dest.h);
    SDL_RenderCopy(game->renderer, tex, NULL, &dest);
}

void drawSpriteSimple(GameState* game, int objNum) {
    IceObject* obj = &game->objs[objNum];
    drawImageXY(game, game->sprites[obj->look], obj->x, obj->y - spriteH[obj->look]);
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
    if (game->clipX < 0){
        resetGameClip(game);
        return 0;
    }
    
    IncrmentGameClip(game);
    drawSetTarget(game, game->foregoundTexture);
    SDL_Rect rect = {game->clipX, game->clipY, game->clipW, game->clipH};
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(game->renderer, &rect);
    drawResetTarget(game);

    return 1;
}


void drawToPlayField(GameState* game){
    drawSetTarget(game, game->foregoundTexture);
    // SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
    // SDL_RenderClear(game->renderer);
    setClip(game, game->clipX, game->clipY, game->clipW, game->clipH);

    drawImage(game, game->backgoundTexture, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_RenderSetClipRect(game->renderer, NULL);
    
    drawResetTarget(game);
    SDL_RenderCopy(game->renderer, game->foregoundTexture, NULL, &ScreenSpace);

    IncrmentGameClip(game);

    if (game->clipX < 0) {
        game->gameMode = 6;
        resetGameClip(game);
    }
}

void drawShadowSimple(GameState* game, int objNum) {
    IceObject* obj = &game->objs[objNum];
    int var2 = 0;
    if (shadowX[obj->look] == 8) {
       var2 = 2;
    }

    int var9 = obj->x + shadowX[obj->look];
    if ((var9 + obj->y + game->counter & 1) > 0)
        var2++;

    if (shadowX[obj->look] > 0) {
       drawImageXY(game, game->shadows[var2], obj->x + shadowX[obj->look], obj->y - 16);
    }
 }

void drawShadow(GameState* game, int objNum) {
    IceObject* obj = &game->objs[objNum];
    int var3 = 0;
    if (shadowX[obj->look] == 0)
        return; 

    if (shadowX[obj->look] == 8) {
        var3 = 2;
    }

    int var4 = obj->y + 32;
    int var9 = obj->x + shadowX[obj->look];
    if ((var9 + obj->y + game->counter & 1) > 0)
        var3++;

    int var6 = 24 - var9 % 24;
    int var7 = var9 / 24 + 12 * (obj->y >> 4);
    int var8 = var7 + 1;

    int var2;
    for (var2 = (obj->y & 240) + 16; var7 < 168 && (game->map[var7] & 3) == 0 && var2 < var4 + 16; var7 += 12) {
        var2 += 16;
    }

    if (var2 > var4) {
        var2 -= var4;

        if (var2 > 16)
            var2 = 16;

        setClip(game, var9, var4, var6, var2);
        drawImage(game, game->shadows[var3], var9, var4, 24, 16);
    }

    if (var6 != 24) {
        int var5;
        if ((game->map[var8] & 3) != 0) {
            var5 = 16 - (obj->y & 15);
        } else {
            var5 = 0;
        }

        var2 = (obj->y & 240) + 32;

        for (int var14 = var8 + 12; var14 < 168 && (game->map[var14] & 3) == 0 && var2 < var4 + 16; var14 += 12) {
            var2 += 16;
        }

        if (var2 > var4 + var5) {
            var2 -= var4;

            if (var2 > 16) 
                var2 = 16;
            

            var2 -= var5;
            if (var2 > 0) {
                setClip(game, var9 + var6, var4 + var5, 24 - var6, var2);
                drawImage(game, game->shadows[var3], var9, var4, 24, 16);
            }
        }
    }

    SDL_RenderSetClipRect(game->renderer, NULL);
}

SDL_Rect vPrint(GameState* game, int x, int y, SDL_Color color, const char* text){
    SDL_Rect pos;
    pos.x = x;
    pos.y = y;
    SDL_Color Black = {0, 0, 0};
    SDL_Surface* surfaceTextBlack = TTF_RenderText_Solid(game->font, text, Black);
    SDL_Texture* TextBlack = SDL_CreateTextureFromSurface(game->renderer, surfaceTextBlack);

    SDL_Surface* surfaceTextWhite = TTF_RenderText_Solid(game->font, text, color); 
    SDL_Texture* TextWhite = SDL_CreateTextureFromSurface(game->renderer, surfaceTextWhite);
    pos.w = surfaceTextWhite->w;
    pos.h = surfaceTextWhite->h;

    drawImageXY(game, TextBlack, x+2, y+2);
    drawImageXY(game, TextWhite, x, y);

    SDL_FreeSurface(surfaceTextBlack);
    SDL_FreeSurface(surfaceTextWhite);
    SDL_DestroyTexture(TextBlack);
    SDL_DestroyTexture(TextWhite);
    return pos;
 }

SDL_Rect vPrintCenter(GameState* game, int x, int y, SDL_Color color, const char* text){
    SDL_Rect pos = GetPrintSize(game, text);
    pos.x = x;
    pos.y = y;

    pos = vPrint(game, (x) - (pos.w/2), y, color, text);
    
    pos.x = x;
    pos.y = y;
    return pos;
}

SDL_Rect GetPrintSize(GameState* game, const char* text){
    SDL_Rect pos;
    pos.x = 0;
    pos.y = 0;
    SDL_Color Black = {0, 0, 0};
    SDL_Surface* surfaceTextWhite = TTF_RenderText_Solid(game->font, text, Black); 
    pos.w = surfaceTextWhite->w;
    pos.h = surfaceTextWhite->h;
    SDL_FreeSurface(surfaceTextWhite);
    return pos;
}

void drawGreenBackgound(GameState* game)
{
    drawSetTarget(game, game->backgoundTexture);

    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        SDL_SetRenderDrawColor(game->renderer, 0, i>>1, 0,255);
        SDL_RenderDrawLine(game->renderer, 0,i,SCREEN_WIDTH,i);
    }

    drawImageXY(game, game->logo, (SCREEN_WIDTH>>1) - (176>>1) , 10);

    drawResetTarget(game);
}