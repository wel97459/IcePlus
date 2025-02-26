#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define MAX(a, b)  ((a > b) ? a : b)
#define IMG_FILE "logo.raw"
#define IMG_SIZE 32208
#define IMG_W 48
#define IMG_H 48

SDL_Renderer* renderer;
SDL_Window* window;

#define NUM_SPRITES 48
int spriteH[] = { 29, 32, 31, 29, 31, 31, 29, 31, 32, 32, 30, 32, 32, 30, 24, 27, 30, 30, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 30, 29, 27, 25, 23, 22, 25, 27, 29, 27, 15, 15, 15, 15, 16, 12};
int spriteEffH[] ={ 29, 32, 31, 29, 31, 31, 29, 31, 32, 32, 30, 32, 32, 30, 24, 27, 30, 30, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 30, 29, 27, 25, 23, 22, 25, 27, 29, 27, 22, 22, 22, 22, 16, 12};
int propSpritesH[] = {29, 32, 31, 29, 31, 31, 29, 31, 32, 31,  31, 32, 32, 30, 24, 28, 29};

#define NUM_BACK 17
int propbackW[] = {24, 24, 24, 24, 24, 24, 72, 24, 24, 24, 24, 12, 12, 12, 12, 12, 12};
int propbackH[] = {32, 32, 32, 32, 32, 32, 16, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8};

void SetAlpha(void *pixels, int w, int h){
    union p
    {
        void *p;
        char *c;
    };
    union p pix;
    pix.p = pixels;
    for (size_t i = 3; i < (w*h)*4; i+=4)
    {
        pix.c[i] = pix.c[i-3];
        pix.c[i-3] = pix.c[i-1];
        pix.c[i-1] = pix.c[i];
        pix.c[i] = 0xff;
    }
}

void screenshot(const char filename[], int w, int h)
{
	// Create an empty RGB surface that will be used to create the screenshot bmp file
	SDL_Surface* pScreenShot = SDL_CreateRGBSurface(0, w, h, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0x00000000);
	
	// Read the pixels from the current render target and save them onto the surface
	SDL_RenderReadPixels(renderer, NULL, SDL_GetWindowPixelFormat(window), pScreenShot->pixels, pScreenShot->pitch);
    
	// Create the bmp screenshot file
	//SDL_SaveBMP(pScreenShot, filename);
    SetAlpha(pScreenShot->pixels, w, h);
    
    stbi_write_png(filename, w, h, 4, pScreenShot->pixels, w*4);
	// Destroy the screenshot surface
	SDL_FreeSurface(pScreenShot);
}

size_t LoadData(char *filename, char *data){
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

size_t GetFileSize(char *filename){
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

int main(int argc, char* argv[]) {
    int s = 0;
    char *bigfile = "bigdirt.raw";
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            s = atoi(argv[i + 1]);
        } else if (strcmp(argv[i], "-b") == 0 && i + 1 < argc) {
            bigfile = argv[i + 1];
        //} else if (strcmp(argv[i], "-chip8") == 0 && i + 1 < argc) {
        //    chip8 = argv[i + 1];
        }
    }
    

    size_t file_size = 0;
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }


    unsigned char* spriteData = malloc(IMG_SIZE);
    unsigned char* backByte = malloc(6720);

    LoadData("sprites1.raw", &spriteData[0]);
    LoadData("sprites2.raw", &spriteData[13008]);
    LoadData("sprites3.raw", &spriteData[23760]);

    for (int var1 = 0; var1 < 768; var1++) {
        backByte[var1] = spriteData[var1 + 13008];
        backByte[var1 + 768] = spriteData[var1 + 13008];
        backByte[var1 + 1536] = spriteData[var1 + 18384];
        backByte[var1 + 2304] = spriteData[var1 + 18384];
    }

    unsigned char* bigiceshadow = malloc(256);
    LoadData("bigiceshadow.raw", bigiceshadow);

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

    unsigned char* bigdirt = malloc(GetFileSize(bigfile));
    LoadData(bigfile, bigdirt);

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

    // Calculate the total width and height needed for all sprites
    int totalWidth = 0;
    int totalHeight = 0;
    for (int s = 0; s < NUM_BACK; s++) {
        totalWidth += 24;//propbackW[s];
        totalHeight = MAX(totalHeight, propbackH[s]);
    }
    totalWidth += NUM_BACK;
    // Create a window and renderer
    window = SDL_CreateWindow("Logo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, totalWidth*6, totalHeight*6, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Failed to create window: %s\n", SDL_GetError());
        goto freeUp;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Failed to create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        goto freeUp;
    }

    // Create a surface for the combined texture
    SDL_Surface* combinedSurface = SDL_CreateRGBSurfaceWithFormat(0, totalWidth, totalHeight, 8, SDL_PIXELFORMAT_RGB332);
    if (!combinedSurface) {
        printf("Failed to create combined surface: %s\n", SDL_GetError());
        // Cleanup code
        goto freeUp;
    }
    int propO = 0;
    int xOffset = 0;
    for (int s = 0; s < NUM_BACK; s++) {
        // Create the individual sprite surface
        SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, propbackW[s], propbackH[s], 8, SDL_PIXELFORMAT_RGB332);
        if (!surface) {
            printf("Failed to create surface: %s\n", SDL_GetError());
            // Cleanup code
            goto freeUp;
        }

        // Copy the pixel data into the surface
        memcpy(surface->pixels, &backByte[propO], surface->pitch * surface->h);

        // Blit the individual surface onto the combined surface
        SDL_Rect dstRect = { xOffset, 0, propbackW[s], propbackH[s] };
        SDL_BlitSurface(surface, NULL, combinedSurface, &dstRect);

        // Cleanup the individual surface
        SDL_FreeSurface(surface);

        xOffset += propbackW[s]+1;
        propO += propbackW[s] * propbackH[s];
    }

    // Clear the renderer and draw the surface
    SDL_RenderClear(renderer);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, combinedSurface);
    if (!texture) {
        printf("Failed to create texture: %s\n", SDL_GetError());
        SDL_FreeSurface(combinedSurface);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        goto freeUp;
    }

    strcpy(&bigfile[strlen(bigfile)-3], "bmp");
    SDL_SaveBMP(combinedSurface, bigfile);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    
    SDL_RenderPresent(renderer);
    screenshot("out.png", totalWidth*6, totalHeight*6);

    // Main loop to keep the window open
    int running = 1;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
                break;
            }
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
        }
    }

    // Cleanup
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(combinedSurface);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
freeUp:
    free(spriteData);
    free(backByte);
    free(bigiceshadow);
    SDL_Quit();

    return 0;
}