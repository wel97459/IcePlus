#ifndef TILELOADER_H
#define TILELOADER_H

#include <SDL2/SDL.h>
#include "game.h"

size_t LoadData(char *filename, char *data);
size_t GetFileSize(char *filename);
SDL_Surface* convert_rgb332_to_rgba8888(SDL_Surface* surface, const int noAlpha);
int loadSpriteData(unsigned char* spriteData, unsigned char* backByte);
int loadBlockData(const char *bigfile, unsigned char* backByte);
int loadSprites(GameState* game);
int loadBlocks(GameState* game, const char *filename);

#endif