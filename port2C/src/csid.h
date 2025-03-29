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
#define PAL_FRAMERATE 50.06 //50.0443427 //50.1245419 //(C64_PAL_CPUCLK/63/312.5), selected carefully otherwise some ADSR-sensitive tunes may suffer more:
#define DEFAULT_SAMPLERATE 44100.0         //(Soldier of Fortune, 2nd Reality, Alliance, X-tra energy, Jackal, Sanxion, Ultravox, Hard Track, Swing, Myth, LN3, etc.)
#define CLOCK_RATIO_DEFAULT C64_PAL_CPUCLK/DEFAULT_SAMPLERATE  //(50.0567520: lowest framerate where Sanxion is fine, and highest where Myth is almost fine)
#define VCR_SHUNT_6581 1500 //kOhm //cca 1.5 MOhm Rshunt across VCR FET drain and source (causing 220Hz bottom cutoff with 470pF integrator capacitors in old C64)
#define VCR_FET_TRESHOLD 192 //Vth (on cutoff numeric range 0..2048) for the VCR cutoff-frequency control FET below which it doesn't conduct
#define CAP_6581 0.470 //nF //filter capacitor value for 6581
#define FILTER_DARKNESS_6581 22.0 //the bigger the value, the darker the filter control is (that is, cutoff frequency increases less with the same cutoff-value)
#define FILTER_DISTORTION_6581 0.0016 //the bigger the value the more of resistance-modulation (filter distortion) is applied for 6581 cutoff-control

struct cSID_SID_s{
    unsigned int TriSaw_8580[4096];
    unsigned int PulseSaw_8580[4096];
    unsigned int PulseTriSaw_8580[4096];
    int8_t ADSRstate[9];
    int8_t expcnt[9];
    int8_t prevSR[9];
    int8_t sourceMSBrise[9];  
    short int envcnt[9];
    unsigned int prevwfout[9];
    unsigned int prevwavdata[9];
    unsigned int sourceMSB[3];
    unsigned int noise_LFSR[9];
    int phaseaccu[9];
    int prevaccu[9];
    int prevlowpass[3];
    int prevbandpass[3];
    float ratecnt[9];
    float cutoff_ratio_8580;
    float cutoff_steepness_6581;
    float cap_6581_reciprocal;
};

struct cSID_CPU_s {
    Uint16 PC;
    Uint16 addr;
    Uint16 storadd;
    short int A;
    short int T;
    short int SP; 
    Uint8 X;
    Uint8 Y;
    Uint8 IR;
    Uint8 ST;  //STATUS-flags: N V - B D I Z C
    float CPUtime;
    Uint8 cycles;
    Uint8 dynCIA;
};

typedef struct {
    int OUTPUT_SCALEDOWN;
    float clock_ratio;
    int dynCIA;
    int SIDamount;
    int SID_model[3];
    int requested_SID_model;
    int sampleratio;
    int freamCounter;
    int playtime;
    uint8_t *filedata;
    uint8_t memory[MAX_DATA_LEN];
    float ADSRperiods[16];
    uint8_t ADSRstep[16];
    char timermode[0x20];
    char SIDtitle[0x20];
    char SIDauthor[0x20];
    char SIDinfo[0x20];
    int subtune;
    Uint16 initaddr;
    Uint16 playaddr;
    Uint16 playaddf;
    Uint16 SID_address[3]; 
    Uint8 finished;
    int samplerate; 
    float framecnt;
    float frame_sampleperiod;
    float addf;
    struct cSID_SID_s sid;
    struct cSID_CPU_s cpu;
} cSID;

//function prototypes
cSID cSID_LoadSID(const char* filename);
void cSID_init(cSID* csid, int samplerate);
void cSID_initSID(cSID* csid);
void cSID_initCPU (cSID* csid, uint16_t mempos);
void cSID_initSubtune(cSID* csid, uint8_t subt, int playtime);
int cSID_SID(cSID* csid, char num, unsigned int baseaddr);
Uint8 cSID_CPU(cSID* csid); 
void cSID_play(void* userdata, Uint8 *stream, int len );
unsigned int cSID_combinedWF(cSID* csid, char num, char channel, unsigned int* wfarray, int index, char differ6581, Uint8 freqh);
void cSID_createCombinedWF(unsigned int* wfarray, float bitmul, float bitstrength, float treshold);

#endif