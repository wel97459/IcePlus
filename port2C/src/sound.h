#ifndef SOUND_H
#define SOUND_H

void playSound(GameState* game, int sound);
void loadSounds(GameState* game);

#ifdef __SWITCH__
    Mix_Chunk *Mix_LoadWAV_MINE(const char * path);
    Mix_Chunk *LoadWAV(SDL_RWops *src, int freesrc);
#endif
#endif