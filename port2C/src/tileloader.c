#include <stdio.h>
#include <stdlib.h>
#include "tileloader.h"

const unsigned char rgb332_3b_lut[] = {0x00, 0x24, 0x49, 0x6d, 0x92, 0xb6, 0xdb, 0xff };
const unsigned char rgb332_2b_lut[] = {0x00, 0x55, 0xaa, 0xff };


const int spriteH[] = { 29, 32, 31, 29, 31, 31, 29, 31, 32, 32, 30, 32, 32, 30, 24, 27, 30, 30, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 30, 29, 27, 25, 23, 22, 25, 27, 29, 27, 15, 15, 15, 15, 16, 12};
const int spriteEffH[] ={ 29, 32, 31, 29, 31, 31, 29, 31, 32, 32, 30, 32, 32, 30, 24, 27, 30, 30, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 30, 29, 27, 25, 23, 22, 25, 27, 29, 27, 22, 22, 22, 22, 16, 12};


const int propbackW[] = {24, 24, 24, 24, 24, 24, 72, 24, 24, 24, 24, 12, 12, 12, 12, 12, 12};
const int propbackH[] = {32, 32, 32, 32, 32, 32, 16, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8};

#define BACKBYTE_SIZE 6720
#define SPRITE_SIZE 32208
#define FILE_LOC "../raw/"
unsigned char* backByte_;

size_t LoadData(char *filename, char *data)
{
    size_t file_size = 0;

    // Read the raw pixel data
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Failed to open %s\n", filename);
        return 0;
    }

    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    rewind(file); 

    if (fread(data, 1, file_size, file) != file_size) {
        printf("Failed to read logo.raw\n");
        fclose(file);
        return 0;
    }
    fclose(file);

    return file_size;
}

size_t GetFileSize(char *filename)
{
    size_t file_size = 0;

    // Read the raw pixel data
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Failed to open %s\n", filename);
        return 0;
    }

    fseek(file, 0, SEEK_END);
    file_size = ftell(file);

    fclose(file);
    return file_size;
}

SDL_Surface* convert_rgb332_to_rgba8888(SDL_Surface* surface, const int noAlpha) {
    // Create a new surface with the desired pixel format
    SDL_Surface* dest = SDL_CreateRGBSurface(0, surface->w, surface->h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    
    if (!dest) {
        return NULL;
    }

    // Lock both surfaces for pixel access
    if (SDL_LockSurface(surface) < 0 || SDL_LockSurface(dest) < 0) {
        SDL_FreeSurface(dest);
        return NULL;
    }

    uint32_t* pixels = (uint32_t*)dest->pixels;
    uint8_t* src_pixels = (uint8_t*)surface->pixels;
    //111 111 11
    // Iterate over each pixel
    for (int y = 0; y < surface->h; y++) {
        for (int x = 0; x < surface->w; x++) {
            // Get the current pixel value (assuming 1 byte per pixel for RGB332)
            uint8_t pixel = *(src_pixels + y * surface->pitch + x);

            // Extract RGB components from the 3-bit format
            uint8_t red = (pixel & 0xE0) >> 5;      // 3 bits for red
            uint8_t green = (pixel & 0x1c) >> 2;    // 3 bit for green
            uint8_t blue = (pixel & 0x03);            // 2 bits for blue

            // Convert to 8-bit components and scale appropriately
            red = rgb332_3b_lut[red];           // Scale 3 bits to 8 bits
            green = rgb332_3b_lut[green];       // Scale 3 bit to 8 bits
            blue = rgb332_2b_lut[blue];        // Scale 2 bits to 8 bits

            // Create the RGBA pixel (assuming alpha is 255 for no transparency)
            uint32_t rgba_pixel = (red << 16) | (green << 8) | blue | ((red+green+blue+noAlpha) ? 0xFF000000 : 0x00000000);

            // Assign the pixel to the destination surface
            *(pixels + y * dest->w + x) = rgba_pixel;
        }
    }

    // Unlock the surfaces
    SDL_UnlockSurface(surface);
    SDL_UnlockSurface(dest);

    return dest;
}

int loadSpriteData(unsigned char* spriteData, unsigned char* backByte)
{
    if(!LoadData(FILE_LOC "sprites1.raw", &spriteData[0]) ||
    !LoadData(FILE_LOC "sprites2.raw", &spriteData[13008]) ||
    !LoadData(FILE_LOC "sprites3.raw", &spriteData[23760])) {
        printf("Failed to load sprites.\n");
        return 0;
    }

    for (int var1 = 0; var1 < 768; var1++) {
        backByte[var1] = spriteData[var1 + 13008];
        backByte[var1 + 768] = spriteData[var1 + 13008];
        backByte[var1 + 1536] = spriteData[var1 + 18384];
        backByte[var1 + 2304] = spriteData[var1 + 18384];
    }

    unsigned char* bigiceshadow = malloc(256);
    if(!LoadData(FILE_LOC "bigiceshadow.raw", bigiceshadow)) {
        printf("Failed to load bigiceshadow.\n");
        free(bigiceshadow);
        return 0;
    }

    int var3 = 1152;
    int var4 = 0;

    for (int var6 = 0; var6 < 16; var6++) {
        for (int var2 = 0; var2 < 8; var2++) {
        backByte[var3++] = bigiceshadow[var4++];
        }

        var3 += 16;
    }

    var3 += 1152;

    for (int var7 = 0; var7 < 16; var7++) {
        for (int var8 = 0; var8 < 8; var8++) {
        backByte[var3++] = bigiceshadow[var4++];
        }

        var3 += 16;
    }

    free(bigiceshadow);
    return 1;
}

int loadBlockData(const char *bigfile, unsigned char* backByte)
{
    char* filename = malloc(256);
    sprintf(filename ,FILE_LOC "%s", bigfile);

    unsigned char* bigdirt = malloc(GetFileSize(filename));
    if(!LoadData(filename, bigdirt)){
        free(bigdirt);
        free(filename);
        return 0; 
    }
    free(filename);

    for (int var2 = 0; var2 < 768; var2++) {
       backByte[var2 + 3072] = bigdirt[var2];
       backByte[var2 + 3840] = bigdirt[var2];
    }

    for (int var9 = 0; var9 < 384; var9++) {
       backByte[var9 + 4224] = bigdirt[768 + var9];
    }

    int var5 = 4608;

    for (int var4 = 0; var4 < 16; var4++) {
       for (int var10 = 0; var10 < 3; var10++) {
          for (int var3 = 0; var3 < 24; var3++) {
            backByte[var5++] = bigdirt[1152 + var4 * 24 + var10 * 384 + var3];
          }
       }
    }

    int levGround = (3072 - 2304) / 192;
    int var13 = levGround * 192;
    int var14 = 5760;
    var5 = 2304;

    for (int var11 = 0; var11 < var13; var11++) {
        backByte[var14++] = bigdirt[var5++];
    }
    free(bigdirt);
    return 1;
}


int loadSprites(GameState* game) {
    unsigned char* spriteData = (unsigned char*) malloc(SPRITE_SIZE);
    backByte_ = (unsigned char*) malloc(BACKBYTE_SIZE);

    if(!loadSpriteData(spriteData, backByte_)){
        printf("Failed to load sprite data.");
        return 0;
    }

    int propO = 0;
    for (int i = 0; i < SPRITE_COUNT; i++) {
        // Create the individual sprite surface
        SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, 24, 32, 8, SDL_PIXELFORMAT_RGB332);
        if (!surface) {
            printf("Failed to create surface: %s\n", SDL_GetError());
            return 0;
            // Cleanup code
        }
        printf("Memcpy %i -", i);
        fflush(stdout);
        // Copy the pixel data into the surface
        memcpy(surface->pixels, &spriteData[propO], surface->pitch * surface->h);
        printf("done\n");
        fflush(stdout);

        SDL_Surface* surfaceRGBA = convert_rgb332_to_rgba8888(surface, 0);
        game->sprites[i] = SDL_CreateTextureFromSurface(game->renderer, surfaceRGBA);

        // Cleanup the individual surface
        SDL_FreeSurface(surface);
        SDL_FreeSurface(surfaceRGBA);
        propO += 24 * spriteH[i];
    }
    return 1;
}

int loadBlocks(GameState* game, const char *filename){
    if(!loadBlockData(filename, backByte_)){
        printf("Failed to load sprite data.");
        return 0;
    }

    int propO = 0;
    for (int i = 0; i < BLOCK_COUNT; i++) {
        // Create the individual sprite surface
        SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, propbackW[i], propbackH[i], 8, SDL_PIXELFORMAT_RGB332);
        if (!surface) {
            printf("Failed to create surface: %s\n", SDL_GetError());
            return 0;
            // Cleanup code
        }
    
        // Copy the pixel data into the surface
        memcpy(surface->pixels, &backByte_[propO], surface->pitch * surface->h);
    
        SDL_Surface* surfaceRGBA = convert_rgb332_to_rgba8888(surface, 1);
        game->blocks[i] = SDL_CreateTextureFromSurface(game->renderer, surfaceRGBA);

        // Cleanup the individual surface
        SDL_FreeSurface(surface);
        SDL_FreeSurface(surfaceRGBA);
        propO += propbackW[i] * propbackH[i];
    }
    return 1;
}