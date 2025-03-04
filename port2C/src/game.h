#ifndef GAME_H
#define GAME_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#define SCREEN_WIDTH 287
#define SCREEN_HEIGHT 256
#define SCREEN_SIZE 2

#define SPRITE_COUNT 48
#define BLOCK_COUNT 17

typedef enum GAMEMODES {
    NextMode,
    SetupIntroScreen,
    AnimateIntro,
    PrepareGameLevel,
    ToBlack,
    ResetLevel,
    MainGameLoop,
    notsure,
    GameOver,
    HighScores,
    Settings
} GameModes;

typedef enum OBJTYPE {
    NoObject,
    Player,
    IceBlock,
    IceBlockCoin,
    IceBlockBreak,
    IceBlockBreakCoin,
    EnemieFire,
    EnemieSpiningFire,
    EnemieKillScore,
    PlayerDied,
} ObjectTypes;

typedef struct {
    int x, y;
    int look;
    ObjectTypes type;
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
    int sortN;
    int sortY[10];
    int sortIX[10];
    int map[168];
    int levGround;
    unsigned char ground[672];
    int enemies[4];
    GameModes gameMode;
    int counter;
    int lastKey;
    int running;
    int level;
    int coins;
    int lives;
    int score;
    int clipX;
    int clipY;
    int clipW;
    int clipH;
} GameState;

void addObject(GameState* game, int type, int pos, int look, int dir);
void clearObjs(GameState* game);
void prepareLevel(GameState* game);
void buildMap(GameState* game, int* levelValues);
void buildTiles(GameState* game);
void sortSprites(GameState* game);
void prepareEnemies(GameState* game);
void gameStart(GameState* game);
void preUpdate(GameState* game);
void updatePlayer(GameState* game, int objNum);
void updateBlocks(GameState* game, int objNum);
void updateBreakBlock(GameState* game, int objNum);
void updateEnemies(GameState* game, int objNum);
void updateKillScore(GameState* game, int objNum);
void updatePlayerDied(GameState* game, int objNum);
#endif