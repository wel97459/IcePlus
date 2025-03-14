
#include "game.h"
#include "sound.h"
#include "csid.h"

#ifdef __SWITCH__
    #undef Mix_LoadWAV
    #define Mix_LoadWAV Mix_LoadWAV_MINE
#endif

void playSound(GameState* game, int sound)
{
    //if (game->soundOn != 0) {
    Mix_PlayChannel(-1, game->sounds[sound], 0);
    //}
}

void loadSounds(GameState* game) {
    // Load sound effects (replace with actual paths)
    char path[128];
    for (int i = 0; i < 6; i++) {
        snprintf(path, 128, "%ssounds/sound%d.wav", FILE_LOC , i);
        game->sounds[i] = Mix_LoadWAV(path);
    }
}


#ifdef __SWITCH__
Mix_Chunk *Mix_LoadWAV_MINE(const char * path)
{
    return LoadWAV(SDL_RWFromFile(path, "rb"), 1);
}

/* Load a wave file - this is hack around linking problmes with SDL_mixer not finding SDL2*/
Mix_Chunk *LoadWAV(SDL_RWops *src, int freesrc)
{
    Uint8 magic[4];
    Mix_Chunk *chunk;
    SDL_AudioSpec wavespec, *loaded;
    SDL_AudioCVT wavecvt;
    int samplesize;
    int wavfree;        /* to decide how to free chunk->abuf. */
    Uint8 *resized_buf;

    /* rcg06012001 Make sure src is valid */
    if (!src) {
        Mix_SetError("Mix_LoadWAV_RW with NULL src");
        return NULL;
    }

    /* Allocate the chunk memory */
    chunk = (Mix_Chunk *)SDL_malloc(sizeof(Mix_Chunk));
    if (chunk == NULL) {
        //Mix_OutOfMemory();
        if (freesrc) {
            SDL_RWclose(src);
        }
        return NULL;
    }

    /* Find out what kind of audio file this is */
    if (SDL_RWread(src, magic, 1, 4) != 4) {
        SDL_free(chunk);
        if (freesrc) {
            SDL_RWclose(src);
        }
        Mix_SetError("Couldn't read first 4 bytes of audio data");
        return NULL;
    }
    /* Seek backwards for compatibility with older loaders */
    SDL_RWseek(src, -4, RW_SEEK_CUR);

    wavfree = 0;
    if (SDL_memcmp(magic, "WAVE", 4) == 0 || SDL_memcmp(magic, "RIFF", 4) == 0) {
        wavfree = 1;
        loaded = SDL_LoadWAV_RW(src, freesrc, &wavespec, (Uint8 **)&chunk->abuf, &chunk->alen);
    }
    if (!loaded) {
        /* The individual loaders have closed src if needed */
        SDL_free(chunk);
        return NULL;
    }

#if 0
    PrintFormat("Audio device", &mixer);
    PrintFormat("-- Wave file", &wavespec);
#endif

    /* Build the audio converter and create conversion buffers */
    if (wavespec.format != MIX_DEFAULT_FORMAT ||
         wavespec.channels != 1 ||
         wavespec.freq != DEFAULT_SAMPLERATE) {
        if (SDL_BuildAudioCVT(&wavecvt,
                wavespec.format, wavespec.channels, wavespec.freq,
                MIX_DEFAULT_FORMAT, 1, DEFAULT_SAMPLERATE) < 0) {
            if (wavfree) {
                SDL_FreeWAV(chunk->abuf);
            } else {
                SDL_free(chunk->abuf);
            }
            SDL_free(chunk);
            return NULL;
        }
        samplesize = ((wavespec.format & 0xFF)/8)*wavespec.channels;
        wavecvt.len = chunk->alen & ~(samplesize - 1);
        wavecvt.buf = (Uint8 *)SDL_calloc(1, wavecvt.len*wavecvt.len_mult);
        if (wavecvt.buf == NULL) {
            //Mix_OutOfMemory();
            if (wavfree) {
                SDL_FreeWAV(chunk->abuf);
            } else {
                SDL_free(chunk->abuf);
            }
            SDL_free(chunk);
            return NULL;
        }
        SDL_memcpy(wavecvt.buf, chunk->abuf, wavecvt.len);
        if (wavfree) {
            SDL_FreeWAV(chunk->abuf);
        } else {
            SDL_free(chunk->abuf);
        }

        /* Run the audio converter */
        if (SDL_ConvertAudio(&wavecvt) < 0) {
            SDL_free(wavecvt.buf);
            SDL_free(chunk);
            return NULL;
        }

        resized_buf = SDL_realloc(wavecvt.buf, wavecvt.len_cvt);
        if (resized_buf == NULL) {
            chunk->abuf = wavecvt.buf;
        } else {
            chunk->abuf = resized_buf;
        }
        chunk->alen = wavecvt.len_cvt;
        wavfree = 0;
    }

    chunk->allocated = (wavfree == 0) ? 1 : 2; /* see Mix_FreeChunk() */
    chunk->volume = MIX_MAX_VOLUME;

    return chunk;
}
#endif