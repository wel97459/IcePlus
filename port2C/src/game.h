#ifndef GAME_H
#define GAME_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#define SCREEN_WIDTH 287
#define SCREEN_HEIGHT 256
#define SCREEN_SIZE 2

#define SPRITE_COUNT 48
#define BLOCK_COUNT 17

typedef struct {
    int x, y;
    int look;
    int type;
    int dir;
    int step;
    int pos;
} IceObject;

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* renderTarget;
    SDL_Texture* backgoundTexture;
    SDL_Texture* foregoundTexture;
    SDL_Texture* sprites[SPRITE_COUNT];
    SDL_Texture* blocks[BLOCK_COUNT];
    Mix_Chunk* sounds[6];
    IceObject objs[10];
    int map[168];
    int levGround;
    unsigned char ground[672];
    int gameMode;
    int lastKey;
    int running;
    int level;
    int coins;
} GameState;

void addObject(GameState* game, int type, int pos, int look, int dir);
void clearObjs(GameState* game);
void prepareLevel(GameState* game);
void buildMap(GameState* game, int* levelValues);
void buildTiles(GameState* game);
#endif