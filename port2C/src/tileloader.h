#ifndef TILELOADER_H
#define TILELOADER_H
#include "game.h"

extern const int spriteH[];
extern const int spriteEffH[];
extern const int propbackW[];
extern const int propbackH[];
extern const unsigned char shadowX[];

size_t LoadData(char *filename, char *data);
size_t GetFileSize(char *filename);
SDL_Surface* convert_rgb332_to_rgba8888(SDL_Surface* surface, const int noAlpha);
int loadSpriteData(unsigned char* spriteData, unsigned char* backByte);
int loadBlockData(GameState* game, const char *bigfile, unsigned char* backByte);
int loadSprites(GameState* game);
int loadBlocks(GameState* game, const char *filename);
void buildShadows(GameState* game, int var1);
void loadLogo(GameState* game);
void buildGreenBG(GameState* game);
#endif