#ifndef GAME_H
#define GAME_H
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#define SCREEN_WIDTH 287
#define SCREEN_HEIGHT 256
#define SCREEN_SIZE 2

#define SPRITE_COUNT 48
#define BLOCK_COUNT 17
#define SHADOW_COUNT 5

extern const SDL_Rect ScreenSpace;

typedef enum GAMEMODES {
    NextMode,
    SetupIntroScreen,
    AnimateIntro,
    PrepareGameLevel,
    ToBlack,
    ResetLevel,
    MainGameLoop,
    FinshedLevel,
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
    TTF_Font* font;
    SDL_Texture* renderTarget;
    SDL_Texture* backgoundTexture;
    SDL_Texture* foregoundTexture;
    SDL_Texture* sprites[SPRITE_COUNT];
    SDL_Texture* blocks[BLOCK_COUNT];
    SDL_Texture* shadows[SHADOW_COUNT];
    SDL_Texture* logo;
    SDL_Texture* greenBG;
    Mix_Chunk* sounds[6];
    IceObject objs[10];
    int selected;
    int sortN;
    int sortY[10];
    int sortIX[10];
    int map[168];
    int levGround;
    unsigned char ground[672];
    int enemies[4];
    int respawnWait;
    GameModes gameMode;
    GameModes nextMode;
    uint32_t nextTime;
    int counter;
    int introCount;
    int lastKey;
    int lastLastKey;
    int menuX[4];
    int running;
    int level;
    int maxLevel;
    int coins;
    int lives;
    int score;
    int highscores[8];
    int clipX;
    int clipY;
    int clipW;
    int clipH;
} GameState;

void playSound(GameState* game, int sound);
int addObject(GameState* game, int type, int pos, int look, int dir);
void clearObjs(GameState* game);
void startSession(GameState* game);
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
int gameOver(GameState* game);
void prepareIntro(GameState* game);
void prepareMenu(GameState* game, int selected);
void setUpIntroScreen(GameState* game);
void advanceIntro(GameState* game);
void animateIntro(GameState* game);
#endif