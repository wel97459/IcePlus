#ifndef CSID_H
#define CSID_H
#include <SDL_config.h>
#include <SDL.h>
#include <SDL_audio.h>

//global constants and variables
#define C64_PAL_CPUCLK 985248.0
#define SID_CHANNEL_AMOUNT 3 
#define MAX_FILENAME_LEN 512
#define MAX_DATA_LEN 65536
#define MAX_PLAYLIST_LEN 1000000
#define MAX_PLAYLIST_ROWLEN 600
#define PAL_FRAMERATE 50.06 //50.0443427 //50.1245419 //(C64_PAL_CPUCLK/63/312.5), selected carefully otherwise some ADSR-sensitive tunes may suffer more:
#define DEFAULT_SAMPLERATE 44100.0         //(Soldier of Fortune, 2nd Reality, Alliance, X-tra energy, Jackal, Sanxion, Ultravox, Hard Track, Swing, Myth, LN3, etc.)
#define CLOCK_RATIO_DEFAULT C64_PAL_CPUCLK/DEFAULT_SAMPLERATE  //(50.0567520: lowest framerate where Sanxion is fine, and highest where Myth is almost fine)
#define VCR_SHUNT_6581 1500 //kOhm //cca 1.5 MOhm Rshunt across VCR FET drain and source (causing 220Hz bottom cutoff with 470pF integrator capacitors in old C64)
#define VCR_FET_TRESHOLD 192 //Vth (on cutoff numeric range 0..2048) for the VCR cutoff-frequency control FET below which it doesn't conduct
#define CAP_6581 0.470 //nF //filter capacitor value for 6581
#define FILTER_DARKNESS_6581 22.0 //the bigger the value, the darker the filter control is (that is, cutoff frequency increases less with the same cutoff-value)
#define FILTER_DISTORTION_6581 0.0016 //the bigger the value the more of resistance-modulation (filter distortion) is applied for 6581 cutoff-control


//function prototypes
int cSID_LoadSID(const char* filename);
void cSID_init(int samplerate);
void cSID_initSID();
void cSID_initCPU (unsigned int mempos);
void cSID_initSubtune (Uint8 subtune); 
int cSID_SID(char num, unsigned int baseaddr);
Uint8 cSID_CPU(); 
void cSID_play(void* userdata, Uint8 *stream, int len );
unsigned int cSID_combinedWF(char num, char channel, unsigned int* wfarray, int index, char differ6581, Uint8 freq);
void cSID_createCombinedWF(unsigned int* wfarray, float bitmul, float bitstrength, float treshold);

#endif