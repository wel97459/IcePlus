#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "tileloader.h"
#include "draw.h"

const SDL_Rect ScreenSpace = {0, 0, SCREEN_WIDTH*SCREEN_SIZE, SCREEN_HEIGHT*SCREEN_SIZE};

const unsigned char levIce[] = {12, 12, 13, 13, 14, 14, 16, 16, 12, 12, 12, 17, 17, 12, 12, 18, 18, 12, 12, 12};
const unsigned char levRock[] = {7, 7, 7, 8, 8, 8, 8, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6};
const int offsets[] = {0, -12, 12, -1, 1};
const int motionKeys[] = {0, SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT, SDLK_SPACE, SDLK_RETURN};
const int cyclic[] = {0, 1, 2, 1};
const int cyclic2[] = {0, 1, 2, 3, 2, 1};
const int pStepX[] = {3, 4, 3, 4, 3, 4, 3};
const int pStepY[] = {2, 3, 2, 2, 3, 2, 2};
const int iStepX[] = {6, 6, 6, 6};
const int iStepY[] = {4, 4, 4, 4};
const int fStepX[] = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
const int fStepY[] = {1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2};
const int rStepX[] = {3, 3, 3, 3, 3, 3, 3, 3};
const int rStepY[] = {2, 2, 2, 2, 2, 2, 2, 2};

const int levFl1[] = {1, 2, 2, 3, 3, 1, 1, 2, 2, 4, 4, 1, 1, 2, 2, 0, 0, 1, 1, 0};
const int levFl2[] = {0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 4};

const char* Intro_text[] = {"Written and designed", "By Karl Hornell", "Ported to C/SDL2 By Winston Lowe"};

const char* Help_text[] = {
    "You control Pixel",
    "Pete, the penguin.",
    "Move ice blocks by",
    "walking against them",
    "or crush them",
    "against other blocks.",
    "Watch out for",
    "the flames.",
    "You can snuff",
    "them with ice.",
    "Collect all five",
    "golden coins ...",
    "... to get to",
    "the next level."
 };

 const char* Menu_text[] = {"Play", "Settings", "Exit", "Start on level", "Sound", "Back", "OFF", "ON"};
 const char* Level_text = "Level %01i";
 const char* Various_text[] = {"Highscores", "GAME OVER", "DEMO"};

const char levNames[10][24] = {
    "bigdirt.raw",
    "biglego.raw",
    "bigbeach.raw",
    "biggrass.raw",
    "bigweird.raw",
    "bigdice.raw",
    "bigwood.raw",
    "bigapples.raw",
    "bigboxes.raw",
    "bigbooks.raw"
};


void playSound(GameState* game, int sound)
{
    //if (game->soundOn != 0) {
    //Mix_PlayChannel(-1, game->sounds[sound], 0);
    //}
}

int addObject(GameState* game, int type, int pos, int look, int dir) 
{
    int i = 0;

    while (i < 9 && game->objs[i].type > 0) {
        i++;
    }
    IceObject *obj = &game->objs[i];

    obj->type = type;
    obj->pos = pos;
    obj->look = look;
    obj->dir = dir;
    obj->x = pos % 12 * 24;
    obj->y = pos / 12 << 4;
    obj->step = 0;

    return i;
}

void clearObjs(GameState* game){
    for (int i = 0; i < 10; i++) {
        game->objs[i].type = 0;
    }
}

void seedRandom() 
{
    srand(time(NULL));
}

int generateRandomTile() {
    return 12 + (rand() % 144);
}

int currentFloorRandom()
{
    return (rand() % 576);
}

void updateScore(GameState* game, int incAmount)
{
    game->score += incAmount;
    drawSetTarget(game, game->backgoundTexture);
    //Draw Top
    for (int i = 0; i < 4; i++) {
        drawBlockSimple(game, 6, i * 72, 0);
    }
    SDL_Color White = {255, 255, 255};
    char str[64];
    sprintf(str,"%05i", game->score);
    vPrint(game, 1, 1, White, str);

    int lives = game->lives;

    for (short i = SCREEN_WIDTH-19; lives > 0; i -= 19) {
        drawImageXY(game, game->sprites[46], i, 0);
        lives--;
    }

    drawResetTarget(game);

}

void startSession(GameState* game) {
    game->gameMode = PrepareGameLevel;
    game->level = 0;
    game->lives = 2;
    game->score = 0;
    resetGameClip(game);
    buildShadows(game, 0);
}

void prepareLevel(GameState* game) {
    int levelValues[] = {0, 9, 5, 7};
    if (game->level < 20) {
        levelValues[1] = levIce[game->level];
        levelValues[3] = levRock[game->level];
    }

    seedRandom();
    buildMap(game, levelValues);
    const char * filen = levNames[game->level / 2 % 10];
    loadBlocks(game, filen);
    buildTiles(game);

    clearObjs(game);

    addObject(game, 1, 13, 11, 0);
    game->coins = 0;
    updateScore(game, 0);
}

void buildMap(GameState* game, int* levelValues) {
    int terrain[180];
    int iteration = 0;

    while (iteration < 5) {
        for (int x = 0; x < 14; x++) {
            for (int y = 0; y < 12; y++) {
                int xy = x * 12 + y;
                if (x > 0 && x < 13 && y > 0 && y < 11) {
                    game->map[xy] = 0;
                } else {
                    game->map[xy] = 3;
                }
            }
        }

        game->map[13] = 4;
        game->map[167] = 0;

        for (int layerIndex = 1; layerIndex < 4; layerIndex++) {
            for (int instanceCount = 0; instanceCount < levelValues[layerIndex]; instanceCount++) {
                int currentTile = 0;

                while (game->map[currentTile] != 0) {
                    currentTile = generateRandomTile();
                    if (game->map[currentTile] != 0) {
                        currentTile = 0;
                    } else {
                        game->map[currentTile] = layerIndex;
                        currentTile = 167;
                    }
                }
            }
        }

        game->map[13] = 4;
        terrain[0] = 13;
        int touchedCount = 0;
        int numLayerTouched = 1;
        iteration = 0;

        while (touchedCount < numLayerTouched) {
            int startIndex = terrain[touchedCount++];

            for (int dir = 1; dir < 5; dir++) {
                if (game->map[startIndex + offsets[dir]] < 3) {
                    terrain[numLayerTouched++] = (startIndex + offsets[dir]);
                    if (game->map[startIndex + offsets[dir]] == 2) {
                        iteration++;
                    }

                    game->map[startIndex + offsets[dir]] = game->map[startIndex + offsets[dir]] | 4;
                }
            }
        }
    }

    game->map[167] = 3;

    for (int i = 0; i < 168; i++) {
        game->map[i] = game->map[i] & 3;
    }
}

void buildTiles(GameState* game) {
    drawSetTarget(game, game->backgoundTexture);

    for (int i = 0; i < 672; i++) {
        game->ground[i] = 0;
    }

    for (int level = 1; level < game->levGround; level++) {
        for (int i = 0; i < 44; i++) {
            game->ground[48 + currentFloorRandom()] = (unsigned char)level;
        }
    }
    int tile;
    
    for (int y = 0; y < 14; y++) {
        for (int x = 0; x < 12; x++) {
            int xy = 12 * y + x;
            if (game->map[xy] != 0) {
                tile = 2 * game->map[xy] - 2;
                if (x > 0 && y < 13 && game->map[xy + 11] != 0) {
                    tile++;
                }

                drawBlockSimple(game, tile, x * 24, 16 + y * 16);
            } else {
                tile = 7 + 2 * game->ground[xy * 4];
                if (x > 0 && game->map[xy - 1] != 0) {
                    tile++;
                }

                drawBlockSimple(game, tile, x * 24, 32 + y * 16);
                tile = 7 + 2 * game->ground[xy * 4 + 1];
                drawBlockSimple(game, tile, x * 24 + 12, 32 + y * 16);
                tile = 7 + 2 * game->ground[xy * 4 + 2];
                if (x > 0 && game->map[xy - 1] != 0) {
                    tile++;
                }

                drawBlockSimple(game, tile, x * 24, 40 + y * 16);
                tile = 7 + 2 * game->ground[xy * 4 + 3];
                drawBlockSimple(game, tile, x * 24 + 12, 40 + y * 16);
            }
        }
    }

    //Draw Top
    for (int i = 0; i < 4; i++) {
        drawBlockSimple(game, 6, i * 72, 0);
    }

    drawResetTarget(game);
}

void sortSprites(GameState* game) {
    game->sortN = 0;

    for (int index = 0; index < 10; index++) {
        if (game->objs[index].type > 0) {
        game->sortY[game->sortN] = game->objs[index].y;
        if (game->objs[index].type == 8) {
            game->sortY[game->sortN] = game->sortY[game->sortN] + 100;
        }

        game->sortIX[game->sortN++] = index;
        }
    }

    for (int i = 1; i < game->sortN; i++) {
        for (int j = 0; j < i; j++) {
            if (game->sortY[j] > game->sortY[i]) {
                int sort = game->sortY[i];
                game->sortY[i] = game->sortY[j];
                game->sortY[j] = sort;
                sort = game->sortIX[i];
                game->sortIX[i] = game->sortIX[j];
                game->sortIX[j] = sort;
            }
        }
    }
}

void startGame(GameState* game) {
    game->gameMode = PrepareGameLevel;
    game->level = 0;
    game->lives = 2;
    game->score = 0;

    resetGameClip(game);
    buildShadows(game, 12);
}

void prepareEnemies(GameState* game) {
    int var1 = 0;
    int var2 = 0;
    int lvl;
    if (game->level > 19) {
       lvl = 19;
    } else {
       lvl = game->level;
    }

    while (var1 < levFl1[lvl]) {
       game->enemies[var2++] = 6;
       var1++;
    }

    for (int var4 = 0; var4 < levFl2[lvl]; var4++) {
        game->enemies[var2++] = 7;
    }

    while (var2 < 4) {
        game->enemies[var2++] = 0;
    }
 }

void gameStart(GameState* game){
    
    if(drawToBlack(game)){
        return;
    }

    for (int i = 1; i < 10; i++) {
        game->objs[i].type = NoObject;
    }

    game->gameMode = NextMode;
    game->nextMode = ResetLevel;

    game->counter = 0;
    prepareEnemies(game);
    int EnemyX = SCREEN_WIDTH/2;
    
    game->objs[0].y = 105;
    game->objs[0].type = 6;
    for (int e = 0; e < 4; e++) {
        if (game->enemies[e] != 0)
        {
            EnemyX -= 15;
        }        
    }

    drawSetTarget(game, game->foregoundTexture);

    for (int e = 0; e < 4; e++) {
        game->objs[0].x = EnemyX;
        EnemyX += 30;
        if (game->enemies[e] > 0) {
            if (game->enemies[e] == 6) {
                game->objs[0].look = 15;
            } else {
                game->objs[0].look = 38;
            }

            drawSpriteSimple(game, 0);
        }
    }

    char lvlText[64];
    sprintf(lvlText, Level_text, game->level+1); 
    SDL_Color White = {255, 255, 255};
    vPrintCenter(game, SCREEN_WIDTH/2, 50, White,(const char*) lvlText);

    drawResetTarget(game);
    
}


void preUpdate(GameState* game){
    IceObject* player = &game->objs[0];
    if(game->respawnWait > 0){
        game->respawnWait--;
        return;
    }
    
    int eCounter = game->counter % 3;

    if (game->enemies[eCounter] > 0) {
        int newEnemiePos;
        int dir;
        if (player->x < 144) {
            newEnemiePos = 22;
            dir = 3;
        } else {
            newEnemiePos = 13;
            dir = 4;
        }

        newEnemiePos += 12 * (rand() % 7);

        while (newEnemiePos < 146 && game->map[newEnemiePos + offsets[dir]] > 0) {
            newEnemiePos += 12;
        }

        if (newEnemiePos < 146) {
            addObject(game, game->enemies[eCounter], newEnemiePos, 0, dir);
            game->enemies[eCounter] = 0;
            game->respawnWait = (rand()%20)+10;
        }
    }
}

void updatePlayer(GameState* game, int objNum){
    IceObject* obj = &game->objs[objNum];
    int mapTile;

    if (obj->x % 24 == 0 && (obj->y & 15) == 0) {
        obj->step = 0;
        obj->dir = 0;

        for (int dir = 1; dir < 5; dir++) {
            if (game->lastKey == motionKeys[dir]) {
                int mapDir = obj->pos + offsets[dir];
                if (game->map[mapDir] == 0) {
                    obj->dir = dir;
                    obj->pos = obj->pos + offsets[dir];
                } else if (game->map[mapDir] == 1 || game->map[mapDir] == 2) {
                    if ((game->map[mapDir + offsets[dir]] & 3) == 0) {
                        mapTile = game->map[mapDir] + 1;
                        addObject(game, mapTile, mapDir, 11 + 7 * game->map[mapDir], dir);
                        game->map[mapDir] = 0;
                        removeBlock(game, mapDir);
                    } else {
                        playSound(game, 1);
                        mapTile = game->map[mapDir] + 3;
                        addObject(game, mapTile, mapDir, 11 + 7 * game->map[mapDir], 0);
                        game->map[mapDir] = 4;
                        removeBlock(game, mapDir);
                    }

                    obj->look = dir * 3 - 1 + cyclic[(game->counter & 6) >> 1];
                }
            }
        }
    }

    if (obj->dir > 0) {
        switch (obj->dir) {
            case 1:
            obj->y = obj->y - pStepY[obj->step];
            break;
            case 2:
            obj->y = obj->y + pStepY[obj->step];
            break;
            case 3:
            obj->x = obj->x - pStepX[obj->step];
            break;
            case 4:
            obj->x = obj->x + pStepX[obj->step];
        }

        obj->look = obj->dir * 3 - 1 + cyclic[(game->counter & 6) >> 1];
        obj->step++;
    }
}

void updateBlocks(GameState* game, int objNum){
    IceObject* obj = &game->objs[objNum];

    switch (obj->dir) {
        case 1:
           obj->y = obj->y - iStepY[obj->step];
           break;
        case 2:
           obj->y = obj->y + iStepY[obj->step];
           break;
        case 3:
           obj->x = obj->x - iStepX[obj->step];
           break;
        case 4:
           obj->x = obj->x + iStepX[obj->step];
     }

     obj->step++;
     if (obj->x % 24 == 0 && (obj->y & 15) == 0) {
        obj->step = 0;
        obj->pos = obj->pos + offsets[obj->dir];
        if (game->map[obj->pos + offsets[obj->dir]] > 0) {
           addBlock(game, obj->pos, obj->type - 1);
           obj->type = 0;
        }
     }

    for (int o = 1; o < 10; o++) {
        IceObject* obj1 = &game->objs[o];
        if ((obj1->type & 14) == 6
           && obj->x < obj1->x + 19
           && obj1->x < obj->x + 19
           && obj->y < obj1->y + 12
           && obj1->y < obj->y + 12) 
        {
        //    if (this.soundOn != 0) {
        playSound(game, 0);
        //    }

            if (obj1->type == 6) {
                obj1->look = 42;
                updateScore(game, 50);
            } else {
                obj1->look = 44;
                updateScore(game, 100);
            }

            int var30 = 0;

            while (game->enemies[var30] > 0) {
                var30++;
            }

            game->enemies[var30] = obj1->type;
            obj1->type = 8;
            obj1->dir = 0;
            game->respawnWait = (rand()%20)+10;
        }
    }
}

void updateBreakBlock(GameState* game, int objNum){
    IceObject* obj = &game->objs[objNum];
    IceObject* player = &game->objs[0];

    obj->look++;
    if (obj->look != 25 && obj->look != 32) {
       return;
    }

    game->map[obj->pos] = 0;
    if (obj->type == 5) {
    //    if (this.soundOn != 0) {
        playSound(game, 2);
    //    }

        game->coins++;
        updateScore(game, 25);
        if (game->coins == 5) {
            player->look = 1;
            game->gameMode = 7;
            game->counter = 0;
        }
    } else {
        updateScore(game, 5);
    }

    obj->type = 0;
    return;
}

void updateEnemies(GameState* game, int objNum){
    IceObject* obj = &game->objs[objNum];
    IceObject* player = &game->objs[0];

    if (obj->type == 6) {
        obj->look = 14 + cyclic2[game->counter % 6];
            switch (obj->dir) {
                case 1:
                    obj->y = obj->y - fStepY[obj->step];
                    break;
                case 2:
                    obj->y = obj->y + fStepY[obj->step];
                    break;
                case 3:
                    obj->x = obj->x - fStepX[obj->step];
                    break;
                case 4:
                    obj->x = obj->x + fStepX[obj->step];
            }
        } else {
        obj->look = 38 + (game->counter & 3);
        switch (obj->dir) {
            case 1:
                obj->y = obj->y - rStepY[obj->step];
                break;
            case 2:
                obj->y = obj->y + rStepY[obj->step];
                break;
            case 3:
                obj->x = obj->x - rStepX[obj->step];
                break;
            case 4:
                obj->x = obj->x + rStepX[obj->step];
        }
        }

        obj->step++;
        if (obj->x % 24 == 0 && (obj->y & 15) == 0) {
        obj->pos = obj->pos + offsets[obj->dir];
        obj->step = 0;
        if (obj->dir == 0 || (game->counter & 14) == 0) {
            obj->dir = 1 + (rand() % 3);
        }

        if (obj->x < player->x + 15 && obj->x > player->x - 15) {
            if (obj->y < player->y) {
                obj->dir = 2;
            } else {
                obj->dir = 1;
            }
        } else if (obj->y < player->y + 11 && obj->y > player->y - 11) {
            if (player->x > obj->x) {
                obj->dir = 4;
            } else {
                obj->dir = 3;
            }
        }

        if (game->map[obj->pos + offsets[obj->dir]] > 0) {
            obj->dir = 0;
        }
        }

        if (obj->x >= player->x + 14
        || obj->x <= player->x - 14
        || obj->y >= player->y + 9
        || obj->y <= player->y - 9)
        {
            return;
        }

        // if (this.soundOn != 0) {
        playSound(game, 3);
        // }

        player->type = 9;
        player->look = 32;

        for (int i = 1; i < 10; i++) {
            if ((game->objs[i].type & 14) == 6) {
                game->objs[i].type = 0;
            }
        }

        for (int i = 0; i < 4; i++) {
            game->enemies[i] = 0;
        }

        game->counter = 0;
}

void updateKillScore(GameState* game, int objNum){
    IceObject* obj = &game->objs[objNum];
    obj->look = (obj->look & 62) + (game->counter & 1);
    obj->dir++;
    if (obj->dir > 15) {
       obj->type = 0;
    }
    return;
}

void updatePlayerDied(GameState* game, int objNum){
    IceObject* obj = &game->objs[objNum];
    
    if ((game->counter & 1) != 0 && obj->look < 37) {
        obj->look++;
    }

    if (game->counter > 42) {
        game->lives--;
        if (game->lives >= 0) {
            obj->look = 6;
            obj->type = 1;
            prepareEnemies(game);
            updateScore(game, 0);
        } else {
            // if (this.soundOn != 0) {
            playSound(game, 5);
            // }

            game->gameMode = 8;
        }
    }

}

int gameOver(GameState* game){
    drawSetTarget(game, game->foregoundTexture); 
    SDL_Color White = {255, 255, 255};
    vPrintCenter(game, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, White, Various_text[1]);
    drawResetTarget(game);

    if (game->counter <= 100) {
       return 1;
    }

    if (game->level > game->maxLevel) {
        game->maxLevel = game->level;
    }

    if (game->score > game->highscores[7]) {
       //prepareHighscores();
       //return 0;
    }

    buildShadows(game, 0);
    prepareIntro(game);
    return 0;
}

void prepareIntro(GameState* game) {
    drawGreenBackgound(game);
    
    drawSetTarget(game, game->foregoundTexture);

    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 0);
    SDL_RenderClear(game->renderer);

    SDL_Color White = {255, 255, 255};
    SDL_Rect pos = vPrintCenter(game, SCREEN_WIDTH/2, 100, White,  Intro_text[0]);
    pos = vPrintCenter(game, SCREEN_WIDTH/2, pos.y+pos.h, White, Intro_text[1]);
    pos = vPrintCenter(game, SCREEN_WIDTH/2, pos.y+pos.h, White, Intro_text[2]);

    drawResetTarget(game);

    game->gameMode = 0;
    game->counter = 0;
    game->nextMode = 1;
 }

void setUpIntroScreen(GameState* game) {
    drawGreenBackgound(game);
    
    drawSetTarget(game, game->foregoundTexture);

    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 0);
    SDL_RenderClear(game->renderer);

    for (int i = 0; i < 3; i++) {
        game->objs[i].x = -36;
        game->objs[i].y = 178;
        game->objs[i].look = 0;
    }

    game->introCount = -1;
    prepareMenu(game, 0);
    buildShadows(game, 0);
    advanceIntro(game);
    game->gameMode = AnimateIntro;
    drawResetTarget(game);
}

void prepareMenu(GameState* game, int selected) {
    game->selected = selected;
    SDL_Color Grey = {64, 64, 64};
    SDL_Rect pos = {13,SCREEN_HEIGHT - 32,0,0};
    for (int i = 0; i < 3; i++) {
        game->menuX[i] = pos.x;
        pos = vPrint(game, pos.x , pos.y, Grey, Menu_text[i]);
        pos.x += pos.w+65;
    }
}


void advanceIntro(GameState* game) {
    game->introCount++;
    if (game->introCount > 8) {
       game->introCount = 0;
    }

    game->counter = 0;
    SDL_Color White = {255, 255, 255};
    SDL_Rect pos = {SCREEN_WIDTH/2,82,0,0};

    if (game->introCount < 7) {
        SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 0);
        SDL_Rect r = {0, 68, SCREEN_WIDTH, 50};
        SDL_RenderFillRect(game->renderer, &r);

        pos = vPrintCenter(game, pos.x, pos.y, White, Help_text[game->introCount << 1]);
        vPrintCenter(game, pos.x, pos.y+pos.h, White, Help_text[(game->introCount << 1) + 1]);
    } else {
        SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 0);
        SDL_Rect r = {0, 68, SCREEN_WIDTH, 120};
        SDL_RenderFillRect(game->renderer, &r);

        pos = vPrintCenter(game, pos.x, pos.y, White, Various_text[0]);
        int var5 = game->introCount - 7 << 2;
        char str[128];
        for (int i = var5; i < var5 + 4; i++) {
            sprintf(str, "%i. %05i ...",i+1, game->highscores[i]);
            pos.y += pos.h;
            pos = vPrintCenter(game, pos.x, pos.y, White, str);
        }
    }
 }

void animateIntro(GameState* game) {
    drawSetTarget(game, game->foregoundTexture);  
    if (game->introCount < 7) {
        SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 0);
        SDL_Rect r = {0, 118, SCREEN_WIDTH, 70};
        SDL_RenderFillRect(game->renderer, &r);
    }
    
    switch (game->introCount) {
       case 0:
          if (game->objs[0].x < SCREEN_WIDTH-64) {
             game->objs[0].look = 11 + cyclic[(game->counter & 7) >> 1];
             game->objs[0].x += 4;
          } else if (game->objs[1].x < 144) {
             game->objs[0].look = 6;
             game->objs[1].look = 18;
             game->objs[1].x += 4;
          } else if (game->objs[2].x < 72) {
             game->objs[2].look = 25;
             game->objs[2].x += 4;
          } else {
             advanceIntro(game);
          }
          break;
       case 1:
          if (game->objs[0].x > 168) {
             game->objs[0].x -= 4;
             game->objs[0].look = 8 + cyclic[(game->counter & 7) >> 1];
          } else if (game->objs[1].x > 96) {
             game->objs[1].x -= 6;
          } else if (game->counter > 27) {
            advanceIntro(game);
          }
        break;
       case 2:
          if (game->objs[0].x > 120) {
             game->objs[0].x -= 4;
             game->objs[0].look = 8 + cyclic[(game->counter & 7) >> 1];
          } else if (game->objs[1].look < 24) {
             game->objs[1].look = game->objs[1].look + (game->counter & 1);
          } else if (game->objs[0].x > 96) {
             game->objs[1].x = -36;
             game->objs[0].x -= 4;
             game->objs[0].look = 8 + cyclic[(game->counter & 7) >> 1];
          } else {
             game->objs[0].look = 6;
             game->objs[1].x = SCREEN_WIDTH;
             advanceIntro(game);
          }
          break;
       case 3:
          game->objs[1].look = 14 + cyclic2[game->counter % 6];
          if (game->counter < 25) {
             game->objs[1].x -= 4;
          } else if (game->counter >= 40) {
             if (game->counter < 65) {
                game->objs[1].x += 4;
             } else {
                game->objs[1].x = -24;
                advanceIntro(game);
             }
          }
          break;
       case 4:
          if (game->objs[1].x < game->objs[2].x) {
             game->objs[1].look = 14 + cyclic2[game->counter % 6];
             game->objs[1].x += 4;
          } else {
             game->objs[1].look = 42 + (game->counter & 1);
             game->objs[1].x++;
          }

          game->objs[2].x -= 6;
          if (game->counter < 32) {
             game->objs[0].look = 10;
          } else {
             game->objs[0].look = 6;
          }

          if (game->counter > 39) {
             game->objs[1].look = 18;
             game->objs[1].x = SCREEN_WIDTH;
             game->objs[2].x = SCREEN_WIDTH;
             advanceIntro(game);
          }
          break;
       case 5:
          if (game->objs[2].x > 168) {
             game->objs[2].x -= 4;
          } else if (game->objs[1].x > 192) {
             game->objs[1].x -= 4;
          } else if (game->objs[0].x < 144) {
             game->objs[0].x += 4;
             game->objs[0].look = 11 + cyclic[(game->counter & 7) >> 1];
          } else {
             advanceIntro(game);
          }
          break;
       case 6:
          if (game->objs[2].look < 31) {
             game->objs[2].look = game->objs[2].look + (game->counter & 1);
          } else if (game->counter < 45) {
             game->objs[2].x = -36;
             game->objs[0].look = (game->counter & 4) >> 2;
          } else {
             game->objs[0].x = -36;
             game->objs[1].x = -36;
             game->objs[0].look = 0;
             game->objs[1].look = 0;
             advanceIntro(game);
          }
          break;
       default:
          if (game->counter > 50) {
             advanceIntro(game);
          }
    }

    for (int i = 0; i < 3; i++){
        drawShadowSimple(game, i);
    }

    for (int i = 0; i < 4; i++){
        drawSpriteSimple(game, i);
    }

    SDL_Color Grey = {64, 64, 64};
    SDL_Color White = {255, 255, 255};

    if (game->lastKey != game->lastLastKey) {
        game->lastLastKey = game->lastKey;
        if (game->lastKey == motionKeys[3]) {
        if (game->selected > 0) {
            vPrint(game, game->menuX[game->selected], SCREEN_HEIGHT - 32, Grey, Menu_text[game->selected]);
            game->selected--;
        }
        } else if (game->lastKey == motionKeys[4] && game->selected < 2) {
            vPrint(game, game->menuX[game->selected], SCREEN_HEIGHT - 32, Grey, Menu_text[game->selected]);
            game->selected++;
        }
        if (game->lastKey == motionKeys[5] || game->lastKey == motionKeys[6]) {
            if (game->selected == 0) {
                startSession(game);
            } else if (game->selected == 1) {
                //prepareSettings();
            } else if (game->selected == 2) {
                game->running = 0;
            }
        }
    }
    vPrint(game, game->menuX[game->selected], SCREEN_HEIGHT - 32, (game->counter%20) < 10 ? White : Grey, Menu_text[game->selected]);

    drawResetTarget(game);
 }