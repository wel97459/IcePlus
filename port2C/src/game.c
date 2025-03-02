#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "tileloader.h"
#include "draw.h"

const unsigned char levIce[] = {12, 12, 13, 13, 14, 14, 16, 16, 12, 12, 12, 17, 17, 12, 12, 18, 18, 12, 12, 12};
const unsigned char levRock[] = {7, 7, 7, 8, 8, 8, 8, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6};
const int offsets[] = {0, -12, 12, -1, 1};

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

    game->map[167] = 6;

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
                tile = 7 + game->ground[xy * 4];
                if (x > 0 && game->map[xy - 1] != 0) {
                    tile++;
                }

                drawBlockSimple(game, tile, x * 24, 32 + y * 16);
                tile = 7 + game->ground[xy * 4 + 1];
                drawBlockSimple(game, tile, x * 24 + 12, 32 + y * 16);
                tile = 7 + game->ground[xy * 4 + 2];
                if (x > 0 && game->map[xy - 1] != 0) {
                    tile++;
                }

                drawBlockSimple(game, tile, x * 24, 40 + y * 16);
                tile = 7 + game->ground[xy * 4 + 3];
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