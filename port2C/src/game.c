#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "tileloader.h"
#include "draw.h"

const SDL_Rect ScreenSpace = {0, 0, SCREEN_WIDTH*SCREEN_SIZE, SCREEN_HEIGHT*SCREEN_SIZE};

const unsigned char levIce[] = {12, 12, 13, 13, 14, 14, 16, 16, 12, 12, 12, 17, 17, 12, 12, 18, 18, 12, 12, 12};
const unsigned char levRock[] = {7, 7, 7, 8, 8, 8, 8, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6};
const int offsets[] = {0, -12, 12, -1, 1};
const int motionKeys[] = {0, SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT};
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

void addObject(GameState* game, int type, int pos, int look, int dir) 
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
    game->backgoundTexture = drawNewTexture(game);
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
    //updateScore(0);
    resetGameClip(game);
    //buildShadows((byte)0);
}

void prepareEnemies(GameState* game) {
    IceObject* player = &game->objs[0];
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
    // this.gg.setColor(Color.white);
    // this.LEVELTEXT[6] = (char)(48 + (this.level + 1) / 10 % 10);
    // this.LEVELTEXT[7] = (char)(48 + (this.level + 1) % 10);
    // this.gg.drawChars(this.LEVELTEXT, 0, 8, 88 - this.fm.charsWidth(this.LEVELTEXT, 0, 8) / 2, 50 + this.textDy);
    game->counter = 0;
    prepareEnemies(game);
    int EnemyX = 31;
    int e = 3;

    for (game->objs[0].y = 105; game->enemies[e] == 0; e--) {
        EnemyX += 15;
    }
    drawSetTarget(game, game->foregoundTexture);
    for (int var21 = 0; var21 < 4; var21++) {
        game->objs[0].x = EnemyX;
        EnemyX += 30;
        if (game->enemies[var21] > 0) {
        if (game->enemies[var21] == 6) {
            game->objs[0].look = 15;
        } else {
            game->objs[0].look = 38;
        }

        drawSpriteSimple(game, 0);
        }
    }
    
}


void preUpdate(GameState* game){
    IceObject* player = &game->objs[0];

    int eCounter = game->counter % 3;
    if (game->enemies[eCounter] > 0) {
        int newEnemiePos;
        int dir;
        if (player->x < 144) {
            newEnemiePos = 23;
            dir = 3;
        } else {
            newEnemiePos = 12;
            dir = 4;
        }

        newEnemiePos += 12 * (rand() % 7);

        while (newEnemiePos < 146 && game->map[newEnemiePos + offsets[dir]] > 0) {
            newEnemiePos += 12;
        }

        if (newEnemiePos < 146) {
            addObject(game, game->enemies[eCounter], newEnemiePos, 0, dir);
            game->enemies[eCounter] = 0;
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
                        // if (game->soundOn != 0) {
                        //     game->playSound(1);
                        // }
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
        //       this.playSound(0);
        //    }

            if (obj1->type == 6) {
                obj1->look = 42;
                //this.updateScore(50);
            } else {
                obj1->look = 44;
                //this.updateScore(100);
            }

            int var30 = 0;

            while (game->enemies[var30] > 0) {
                var30++;
            }

            game->enemies[var30] = obj1->type;
            obj1->type = 8;
            obj1->dir = 0;
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
    //       this.playSound(2);
    //    }

        game->coins++;
        //this.updateScore(25);
        if (game->coins == 5) {
            player->look = 1;
            game->gameMode = 7;
            game->counter = 0;
        }
    } else {
        //this.updateScore(5);
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
        // this.playSound(3);
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
        } else {
            // if (this.soundOn != 0) {
            //     this.playSound(5);
            // }

            game->gameMode = 8;
        }
    }

}