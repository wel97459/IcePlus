#include <stdio.h>
#include <stdlib.h>
#include "tileloader.h"

const int spriteH[] = {
    29, 32, 31, 29, 31, 31, 29, 31, 32, 32, 30, 
    32, 32, 30, 24, 27, 30, 30, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 30,
    29, 27, 25, 23, 22, 25, 27, 29, 27, 15, 15, 
    15, 15, 16, 12
};

const int spriteEffH[] = { 
    29, 32, 31, 29, 31, 31, 29, 31, 32, 32, 30, 
    32, 32, 30, 24, 27, 30, 30, 32, 32, 32, 32, 
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 30, 
    29, 27, 25, 23, 22, 25, 27, 29, 27, 22, 22, 
    22, 22, 16, 12
};

const int propbackW[] = {
    24, 24, 24, 24, 24, 24, 72, 12, 12, 12, 12, 
    12, 12, 12, 12, 12, 12
};

const int propbackH[] = {
    32, 32, 32, 32, 32, 32, 16, 8,  8,  8,  8, 
    8,  8,  8,  8,  8,  8
};

const unsigned char shadowX[] = {
    5,  7,  6,  6,  6,  6,  6,  6,  6,  6,  6,
    6,  6,  6,  0,  0,  0,  0,  8,  8,  8,  8,
    0,  0,  0,  8,  8,  8,  8,  6,  6,  6,  6, 
    5,  4,  3,  2,  1,  0,  0,  0,  0,  0,  0, 
    0,  0,  0,  0,  0,  0,  0,  0
};

const unsigned char shadBytes[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,
    1,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,
    0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  0,  0,  0,  0,  0,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  0,  0,  0,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  0,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,
    0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,
    1,  1,  0,  0,  0,  0,  0,  0,  0,  0
};



const unsigned char rgb332_3b_lut[] = {0x00, 0x24, 0x49, 0x6d, 0x92, 0xb6, 0xdb, 0xff };
const unsigned char rgb332_2b_lut[] = {0x00, 0x55, 0xaa, 0xff };

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

SDL_Texture* createImage(GameState* game, unsigned char* data, int w, int h, int a){
    SDL_Texture* tex;
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 8, SDL_PIXELFORMAT_RGB332);
    if (!surface) {
        printf("Failed to create surface: %s\n", SDL_GetError());
        return NULL;
        // Cleanup code
    }

    // Copy the pixel data into the surface
    memcpy(surface->pixels, data, surface->pitch * surface->h);

    SDL_Surface* surfaceRGBA = convert_rgb332_to_rgba8888(surface, a);
    tex = SDL_CreateTextureFromSurface(game->renderer, surfaceRGBA);

    // Cleanup the individual surface
    SDL_FreeSurface(surface);
    SDL_FreeSurface(surfaceRGBA);
    return tex;
}

int loadSpriteData(unsigned char* spriteData, unsigned char* backByte)
{
    if(!LoadData(FILE_LOC "sprites1.raw", &spriteData[0]) ||
    !LoadData(FILE_LOC "sprites2.raw", &spriteData[13008]) ||
    !LoadData(FILE_LOC "sprites3.raw", &spriteData[23760])) {
        printf("Failed to load sprites.\n");
        return 0;
    }

    for (int i = 0; i < 768; i++) {
        backByte[i] = spriteData[i + 13008];
        backByte[i + 768] = spriteData[i + 13008];
        backByte[i + 1536] = spriteData[i + 18384];
        backByte[i + 2304] = spriteData[i + 18384];
    }
    
    unsigned char* bigiceshadow = malloc(257);
    if(!LoadData(FILE_LOC "bigiceshadow.raw", bigiceshadow)) {
        printf("Failed to load bigiceshadow.\n");
        free(bigiceshadow);
        return 0;
    }

    int startOffset = 1152;
    int shadowIndex = 0;

    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 8; x++) {
        backByte[startOffset++] = bigiceshadow[shadowIndex++];
        }
        startOffset += 16;
    }

    startOffset += 1152;

    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 8; x++) {
            backByte[startOffset++] = bigiceshadow[shadowIndex++];
        }
        startOffset += 16;
    }

    free(bigiceshadow);
    return 1;
}

int loadBlockData(GameState* game, const char *bigfile, unsigned char* backByte)
{
    char* filename = malloc(256);
    sprintf(filename ,FILE_LOC "%s", bigfile);
    size_t fSize = GetFileSize(filename);
    unsigned char* bigdirt = malloc(fSize);
    if(!LoadData(filename, bigdirt)){
        free(bigdirt);
        free(filename);
        return 0; 
    }
    free(filename);

    //Copy soild block tiles
    for (int var2 = 0; var2 < 768; var2++) {
       backByte[var2 + 3072] = bigdirt[var2];
       backByte[var2 + 3840] = bigdirt[var2];
    }

    //Copy solid black face with shadow
    for (int var9 = 0; var9 < 384; var9++) {
       backByte[var9 + 4224] = bigdirt[768 + var9];
    }

    //Load top boarder tiles
    int indexBig = 4608;

    for (int var4 = 0; var4 < 16; var4++) {
       for (int var10 = 0; var10 < 3; var10++) {
          for (int var3 = 0; var3 < 24; var3++) {
            backByte[indexBig++] = bigdirt[1152 + var4 * 24 + var10 * 384 + var3];
          }
       }
    }

    game->levGround = (fSize - 2304) / 192;
    int index = 5760;
    int index1 = 5760+96;

    indexBig = 2304;
    for(int i = 0; i< game->levGround; i++){
        for(int y = 0; y < 16; y++){
            for (int x = 0; x < 12; x++) {
                if((y & 0x01)) backByte[index1++] = bigdirt[indexBig];
                if(!(y & 0x01)) backByte[index++] = bigdirt[indexBig];
                indexBig++;
            }
        }
        index1+=96;
        index+=96;
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
        game->sprites[i] = createImage(game, &spriteData[propO], 24, spriteH[i], 0);
        propO += 24 * spriteH[i];
    }
    return 1;
}

int loadBlocks(GameState* game, const char *filename){
    if(!loadBlockData(game, filename, backByte_)){
        printf("Failed to load sprite data.");
        return 0;
    }

    int propO = 0;
    for (int i = 0; i < BLOCK_COUNT; i++) {
        if (game->blocks[i] != NULL)
            SDL_DestroyTexture(game->blocks[i]);
        
        game->blocks[i] = createImage(game, &backByte_[propO], propbackW[i], propbackH[i], 1);
        propO += propbackW[i] * propbackH[i];
    }
    return 1;
}

void buildShadows(GameState* game, int var1) {
    unsigned char* shadByte = (unsigned char*)malloc(1536);
    int index = 0;
    int var7 = 768;
    if (var1 == 0) {
       for (int i = 0; i < 2; i++) {
          for (int y = 0; y < 16; y++) {
             int locIndex = index;

             for (int x = 0; x < 24; x++) {
                shadByte[var7++] = locIndex;
                locIndex = 32 - locIndex;
             }

             index = 32 - index;
          }

          index = 32 - index;
       }
    } else {
       for (int i = 0; i < 768; i++) {
          shadByte[768 + i] = var1;
       }
    }

    for (int i = 0; i < 384; i++) {
       if (shadBytes[i] != 0) {
          shadByte[i] = shadByte[i + 768];
          shadByte[i + 384] = shadByte[i + 1152];
       }
    }

    for (int i = 0; i < 4; i++) {
        if (game->shadows[i] != NULL)
            SDL_DestroyTexture(game->shadows[i]);
    
        game->shadows[i] = createImage(game, &shadByte[384 * i], 24, 16, 0);
    }
 }
