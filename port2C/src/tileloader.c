#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include "tileloader.h"
#include "game.h"

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

int convert_rgb332_to_rgba8888(SDL_Surface* dest, SDL_Surface* surface, const int noAlpha) {
    // Create a new surface with the desired pixel format
    dest = SDL_CreateRGBSurface(0, surface->w, surface->h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    
    if (!dest) {
        return 0;
    }

    // Lock both surfaces for pixel access
    if (SDL_LockSurface(surface) < 0 || SDL_LockSurface(dest) < 0) {
        SDL_FreeSurface(dest);
        return 0;
    }

    uint32_t* pixels = (uint32_t*)dest->pixels;
    uint8_t* src_pixels = (uint8_t*)surface->pixels;

    // Iterate over each pixel
    for (int y = 0; y < surface->h; y++) {
        for (int x = 0; x < surface->w; x++) {
            // Get the current pixel value (assuming 1 byte per pixel for RGB332)
            uint8_t pixel = *(src_pixels + y * surface->pitch + x);

            // Extract RGB components from the 3-bit format
            uint8_t red = (pixel >> 1) & 0x03;  // 2 bits for red
            uint8_t green = pixel & 0x01;       // 1 bit for green
            uint8_t blue = (pixel >> 2) & 0x03; // 2 bits for blue

            // Convert to 8-bit components and scale appropriately
            red = (red << 6) * 0x7F;          // Scale 2 bits to 8 bits
            green = (green << 7) * 0x7F;       // Scale 1 bit to 8 bits
            blue = (blue << 6) * 0x7F;         // Scale 2 bits to 8 bits

            // Create the RGBA pixel (assuming alpha is 255 for no transparency)
            uint32_t rgba_pixel = (red << 16) | (green << 8) | blue | ((red+green+blue+noAlpha) ? 0xFF000000 : 0x00000000);

            // Assign the pixel to the destination surface
            *(pixels + y * dest->w + x) = rgba_pixel;
        }
    }

    // Unlock the surfaces
    SDL_UnlockSurface(surface);
    SDL_UnlockSurface(dest);

    return 1;
}

void loadSprites(GameState* game) {
    // Load the sprite sheet texture
    SDL_Surface* sheet = IMG_Load("../tileset/sprites.png");
    if (!sheet) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load sprite sheet: %s", IMG_GetError());
        return;
    }
    
    // Dimensions of individual sprites (modify these based on your sprite sheet)
    int sprite_width = 24;
    
    // Calculate position in the sheet for each sprite
    int x = 0;
    for (int i = 0; i < SPRITE_COUNT; i++) {
        SDL_Rect src_rect = { x, 0, sprite_width, spriteH[i] };
        SDL_Surface* surf = SDL_CreateRGBSurface(0, sprite_width, spriteH[i], sheet->format->BitsPerPixel, 0, 0, 0, 0);
        SDL_BlitSurface(sheet, &src_rect, surf, NULL);
        
        game->sprites[i] = SDL_CreateTextureFromSurface(game->renderer, surf);
        SDL_FreeSurface(surf);
        x += spriteH[i]+1;
    }

    SDL_FreeSurface(sheet);
}