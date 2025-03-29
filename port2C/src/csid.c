#include <stdio.h>
#include <fcntl.h>
#include <math.h>
#include <SDL_config.h>
#include <SDL.h>
#include <SDL_audio.h>
#include "csid.h"

// void switchPrintf(const char *fmt, ...);
// #undef printf
// #define printf switchPrintf


//raw output divided by this after multiplied by main volume, this also compensates for filter-resonance emphasis to avoid distotion

enum { GATE_BITMASK=0x01, SYNC_BITMASK=0x02, RING_BITMASK=0x04, TEST_BITMASK=0x08, 
       TRI_BITMASK=0x10, SAW_BITMASK=0x20, PULSE_BITMASK=0x40, NOISE_BITMASK=0x80, 
       HOLDZERO_BITMASK=0x10, DECAYSUSTAIN_BITMASK=0x40, ATTACK_BITMASK=0x80, 
       LOWPASS_BITMASK=0x10, BANDPASS_BITMASK=0x20, HIGHPASS_BITMASK=0x40, OFF3_BITMASK=0x80 };


// float csid->clock_ratio=csid->CLOCK_RATIO_DEFAULT;

// int SIDamount=1, SID_model[3]={8580,8580,8580}, requested_SID_model=-1, sampleratio;
// Uint8 filedata[MAX_DATA_LEN], csid->memory[MAX_DATA_LEN], timermode[0x20], SIDtitle[0x20], SIDauthor[0x20], SIDinfo[0x20];
// int subtune=0;
// unsigned int ini addr, playaddr, playaddf, SID_address[3]={0xD400,0,0}; 
// int csid->samplerate = DEFAULT_csid->SAMPLERATE; 
// float framecnt=0, frame_sampleperiod = DEFAULT_csid->SAMPLERATE/PAL_FRAMERATE; 

//SID-emulation variables:
const Uint8 FILTSW[9] = {1,2,4,1,2,4,1,2,4};
// Uint8 csid->sid.ADSRstate[9], excsid->cpu.pcnt[9], csid->sid.prevSR[9], sourceMSBrise[9];  
// short int envcnt[9];
// unsigned int csid->sid.prevwfout[9], prevwavdata[9], sourceMSB[3], noise_LFSR[9];
// int phaseaccu[9], prevaccu[9], csid->sid.prevlowpass[3], csid->sid.prevbandpass[3];;
// float ratecnt[9], csid->sid.cutoff_ratio_8580, csid->sid.cutoff_steepness_6581, csid->sid.cap_6581_reciprocal; //, cutoff_ratio_6581, cutoff_bottom_6581, cutoff_top_6581;
//CPU (and CIA/VIC-IRQ) emulation constants and variables - avoiding internal/automatic variables to retain speed
const Uint8 flagsw[]={0x01,0x21,0x04,0x24,0x00,0x40,0x08,0x28}, branchflag[]={0x80,0x40,0x01,0x02};
// unsigned int c->PC=0, pc->PC=0, addr=0, csid->cpu.storadd=0;
// short int A=0, T=0, SP=0xFF; 
// Uint8 X=0, Y=0, c->IR=0, ST=0x00;  //STATUS-flags: N V - B D I Z C
// float CPUtime=0.0;
// char cycles=0, finished=0, dynCIA=0;

cSID cSID_LoadSID(const char* filename)
{
    cSID csid;

    csid.SID_model[0]=8580;
    csid.SID_model[1]=8580;
    csid.SID_model[2]=8580;
    csid.samplerate = DEFAULT_SAMPLERATE;
    csid.SID_address[0]=0xD400; 
    csid.frame_sampleperiod = DEFAULT_SAMPLERATE/PAL_FRAMERATE;
    csid.OUTPUT_SCALEDOWN = SID_CHANNEL_AMOUNT * 16 + 26; 
    int strend, subtune_amount, preferred_SID_model[3]={8580.0,8580.0,8580.0}; 
    unsigned int i, offs, loadaddr;
    size_t file_size = 0;
    FILE *file=NULL;

    file = fopen(filename,"rb");
    if (file==NULL) { 
        return csid;
    } 

    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    rewind(file); 
    csid.filedata = malloc(file_size * sizeof(uint8_t));
    if (fread(csid.filedata, 1, file_size, file) != file_size) {
        printf("Failed to read logo.raw\n");
        fclose(file);
        return csid;
    }
    csid.subtune--; 
    if (csid.subtune<0 || csid.subtune>63)
        csid.subtune=0;
    // printf("%d Uint8s read (%s subtune %d)", file_size,filename,subtune+1);
    fclose(file);

    offs = csid.filedata[7];
    loadaddr = csid.filedata[8]+ csid.filedata[9]?  csid.filedata[8]*256+ csid.filedata[9] :  csid.filedata[offs]+ csid.filedata[offs+1]*256; 
    // printf("\nOffset: $%4.4X, Loadaddress: $%4.4X, Size: $%4.4X", offs, loadaddr file_size-offs);
    // printf("Timermodes: ");
    for (i=0; i<32; i++) {
        csid.timermode[31-i] = ( csid.filedata[0x12+(i>>3)] & (Uint8)pow(2,7-i%8)) ? 1 : 0;
        // printf(" %1d",timermode[31-i]);
    }

    for(i=0;i<MAX_DATA_LEN;i++){
        csid.memory[i]=0; 
    }

    for (i=offs+2;  i<file_size; i++) { 
        if (loadaddr+i-(offs+2)<MAX_DATA_LEN) 
            csid.memory[loadaddr+i-(offs+2)]= csid.filedata[i]; 
    }

    strend=1; 
    for(i=0; i<32; i++) { 
        if(strend!=0) {
            strend = csid.filedata[0x16+i];
            csid.SIDtitle[i]= csid.filedata[0x16+i]; 
        } else { 
            strend=0;
            csid.SIDtitle[i]=0; 
        }
    }
    // printf("\nTitle: %s    ",SIDtitle);

    strend=1; 
    for(i=0; i<32; i++) { 
        if(strend!=0) {
            strend=csid.SIDauthor[i]= csid.filedata[0x36+i]; 
        } else {
            strend=csid.SIDauthor[i]=0; 
        }
    }
    // printf("Author: %s    ",SIDauthor); 
    
    strend=1;
    for(i=0; i<32; i++) {
        if(strend!=0){ 
            strend=csid.SIDinfo[i]= csid.filedata[0x56+i];
        } else {
            strend=csid.SIDinfo[i]=0;
        }
    }
    // printf("Info: %s",SIDinfo);

    csid.initaddr =  csid.filedata[0xA] +  csid.filedata[0xB] ?  csid.filedata[0xA] * 256 +  csid.filedata[0xB] : loadaddr;
    csid.playaddr  =  csid.filedata[0xC] * 256 +  csid.filedata[0xD];
    csid.playaddf = csid.playaddr;
    // printf("\nInit:$%4.4X,Play:$%4.4X, ",initaddr, playaddr);

    subtune_amount =  csid.filedata[0xF];
    preferred_SID_model[0] = ( csid.filedata[0x77]&0x30) >= 0x20 ? 8580 : 6581; 
    // printf("Subtunes:%d , preferred SID-model:%d", subtune_amount, preferred_SID_model[0]);

    preferred_SID_model[1] = (csid.filedata[0x77]&0xC0) >= 0x80 ? 8580 : 6581;
    preferred_SID_model[2] = (csid.filedata[0x76]&0x03) >= 0x02 ? 8580 : 6581; 

    csid.SID_address[1] = csid.filedata[0x7A]>=0x42 && ( csid.filedata[0x7A]<0x80 ||  csid.filedata[0x7A]>=0xE0) ? 0xD000+ csid.filedata[0x7A]*16 : 0;
    csid.SID_address[2] = csid.filedata[0x7B]>=0x42 && ( csid.filedata[0x7B]<0x80 ||  csid.filedata[0x7B]>=0xE0) ? 0xD000+ csid.filedata[0x7B]*16 : 0;

    csid.SIDamount= 1 + (csid.SID_address[1]>0) + (csid.SID_address[2]>0); 
    if(csid.SIDamount>=2)
        // printf("(SID1), %d(SID2:%4.4X)",preferred_SID_model[1],SID_address[1]); 
    if(csid.SIDamount==3)
        // printf(", %d(SID3:%4.4X)",preferred_SID_model[2],SID_address[2]);
    if (csid.requested_SID_model!=-1)
        // // printf(" (requested:%d)",requested_SID_model); printf("\n");

    for (i=0;i<csid.SIDamount;i++) {
        if (csid.requested_SID_model==8580 || csid.requested_SID_model==6581) csid.SID_model[i] = csid.requested_SID_model;
        else csid.SID_model[i] = preferred_SID_model[i];
    }

    csid.OUTPUT_SCALEDOWN = SID_CHANNEL_AMOUNT * 16 + 26;
    if (csid.SIDamount == 2){
        csid.OUTPUT_SCALEDOWN /= 0.6;
    } else if (csid.SIDamount>=3) {
        csid.OUTPUT_SCALEDOWN /= 0.4;
    }

    cSID_init(&csid, csid.samplerate); 
    cSID_initSubtune(&csid, csid.subtune);  
    return csid;
}

void cSID_initSubtune(cSID* csid, Uint8 subt)
{
    static int timeout;
    csid->subtune = subt; 
    cSID_initCPU(csid, csid->initaddr); 
    cSID_initSID(csid);
    csid->cpu.A=csid->subtune; 
    csid->memory[1]=0x37;
    csid->memory[0xDC05]=0;
    for(timeout=100000;timeout>=0;timeout--) {
        if (cSID_CPU(csid)) break; 
    } 
    if (csid->timermode[csid->subtune] || csid->memory[0xDC05]) { //CIA timing
        if (!csid->memory[0xDC05]) {
            csid->memory[0xDC04]=0x24; csid->memory[0xDC05]=0x40;
        } //C64 startup-default
        csid->frame_sampleperiod = (csid->memory[0xDC04]+csid->memory[0xDC05]*256)/csid->clock_ratio;
    } else {
        csid->frame_sampleperiod = csid->samplerate/PAL_FRAMERATE;
    }  //Vsync timing
    // printf("Frame-sampleperiod: %.0f samples  (%.1fX speed)\n", round(frame_sampleperiod), csid->samplerate/PAL_FRAMERATE/frame_sampleperiod); 
    //frame_sampleperiod = (csid->memory[0xDC05]!=0 || (!timermode[subtune] && playaddf))? csid->samplerate/PAL_FRAMERATE : (csid->memory[0xDC04] + csid->memory[0xDC05]*256) / csid->clock_ratio; 
    if(csid->playaddf==0) {
        csid->playaddr = ((csid->memory[1] & 0x03) < 0x02) ? csid->memory[0xFFFE]+csid->memory[0xFFFF]*256 : csid->memory[0x314]+csid->memory[0x315]*256;
        // printf("IRQ-playaddress:%4.4X\n",playaddr); 
    } else { 
        csid->playaddr=csid->playaddf;
        if (csid->playaddr>=0xE000 && csid->memory[1]==0x37){ 
            csid->memory[1]=0x35;
        }
    } //player under KERNAL (Crystal Kingdom Dizzy)
    cSID_initCPU(csid, csid->playaddr); 
    csid->framecnt=1;
    csid->finished=0;
    csid->cpu.CPUtime=0;
}

void cSID_play(void* userdata, Uint8 *stream, int len ) //called by SDL at csid->samplerate pace
{   
    cSID *csid = (cSID *)userdata;
    static int i,j, output;
    for(i=0;i<len;i+=2) {
        csid->framecnt--;
        if (csid->framecnt<=0) {
            csid->framecnt=csid->frame_sampleperiod;
            csid->finished=0;
            csid->cpu.PC=csid->playaddr;
            csid->cpu.SP=0xFF;
            printf("freamcnt:%f  PC:%04X\n",csid->framecnt, csid->cpu.PC);
        }  
        if (csid->finished==0) { 
            while (csid->cpu.CPUtime<=csid->clock_ratio) { 
                 Uint16 pPC=csid->cpu.PC; 
                if (cSID_CPU(csid)>=0xFE || ((csid->memory[1]&3)>1 && pPC<0xE000 && (pPC==0xEA31 || pPC==0xEA81))) {
                    csid->finished=1;
                    break;
                } else { 
                    csid->cpu.CPUtime+=csid->cpu.cycles; //RTS,RTI and IRQ player ROM return handling
                }
                if ((csid->cpu.addr==0xDC05 || csid->cpu.addr==0xDC04) && (csid->memory[1]&3) && csid->timermode[csid->subtune])
                {
                    csid->frame_sampleperiod = (csid->memory[0xDC04] + csid->memory[0xDC05]*256) / csid->clock_ratio;  //dynamic CIA-setting (Galway/Rubicon workaround)
                    if (!csid->dynCIA) {csid->dynCIA=1; printf("( Dynamic CIA settings. New frame-sampleperiod: %.0f samples  (%.1fX speed) )\n", round(csid->frame_sampleperiod), csid->samplerate/PAL_FRAMERATE/csid->frame_sampleperiod);}
                }
                if(csid->cpu.storadd>=0xD420 && csid->cpu.storadd<0xD800 && (csid->memory[1]&3)) {  //CJ in the USA workaround (writing above $d420, except SID2/SID3)
                    if ( !(csid->SID_address[1]<=csid->cpu.storadd && csid->cpu.storadd<csid->SID_address[1]+0x1F) && !(csid->SID_address[2]<=csid->cpu.storadd && csid->cpu.storadd<csid->SID_address[2]+0x1F))
                        csid->memory[csid->cpu.storadd&0xD41F]=csid->memory[csid->cpu.storadd]; //write to $D400..D41F if not in SID2/SID3 address-space
                }
                if(csid->cpu.addr==0xD404 && !(csid->memory[0xD404]&GATE_BITMASK)) 
                    csid->sid.ADSRstate[0]&=0x3E; //Whittaker player workarounds (if GATE-bit triggered too fast, 0 for some cycles then 1)
                if(csid->cpu.addr==0xD40B && !(csid->memory[0xD40B]&GATE_BITMASK)) 
                    csid->sid.ADSRstate[1]&=0x3E;
                if(csid->cpu.addr==0xD412 && !(csid->memory[0xD412]&GATE_BITMASK))
                    csid->sid.ADSRstate[2]&=0x3E;
            }
            csid->cpu.CPUtime-=csid->clock_ratio;
        }

        output = cSID_SID(csid, 0, 0xD400);
        if (csid->SIDamount>=2) 
            output += cSID_SID(csid, 1, csid->SID_address[1]); 
        if (csid->SIDamount==3)
            output += cSID_SID(csid, 2, csid->SID_address[2]);
        //output = output >> 3;
        stream[i] = output & 0xFF;
        stream[i+1] = output >> 8;
    }
}


void cSID_initCPU (cSID* csid, uint16_t mempos) { 
    csid->cpu.PC=mempos; 
    csid->cpu.A=0;
    csid->cpu.X=0;
    csid->cpu.Y=0;
    csid->cpu.ST=0;
    csid->cpu.SP=0xFF;
} 

 //My CPU implementation is based on the instruction table by Graham at codebase64.
 //After some examination of the table it was clearly seen that columns of the table (instructions' 2nd nybbles)
 // mainly correspond to addressing modes, and double-rows usually have the same instructions.
 //The code below is laid out like this, with some exceptions present.
 //Thanks to the hardware being in my mind when coding this, the illegal instructions could be added fairly easily...
uint8_t cSID_CPU (cSID* csid) //the CPU emulation for SID/PRG playback (ToDo: CIA/VIC-IRQ/NMI/RESET vectors, BCD-mode)
{ //'IR' is the instruction-register, naming after the hardware-equivalent
  struct cSID_CPU_s *c = &csid->cpu;
  if(c->PC>0)printf("PC:%04X D:%02X\n", c->PC, csid->memory[c->PC]);
  uint8_t ret=0;
  c->IR=csid->memory[c->PC]; c->cycles=2; csid->cpu.storadd=0; //'cycle': ensure smallest 6510 runtime (for implied/register instructions)
  if(c->IR&1) {  //nybble2:  1/5/9/D:accu.instructions, 3/7/B/F:illegal ocsid->cpu.pcodes
   switch (c->IR&0x1F) { //addressing modes (begin with more complex cases), PC wraparound not handled inside to save codespace
    case 1: case 3: c->PC++; c->addr = csid->memory[csid->memory[c->PC]+c->X] + csid->memory[csid->memory[c->PC]+c->X+1]*256; c->cycles=6; break; //(zp,x)
    case 0x11: case 0x13: c->PC++; c->addr = csid->memory[csid->memory[c->PC]] + csid->memory[csid->memory[c->PC]+1]*256 + c->Y; c->cycles=6; break; //(zp),y (5..6 cycles, 8 for R-M-W)
    case 0x19: case 0x1B: c->PC++; c->addr=csid->memory[c->PC]; c->PC++; c->addr+=csid->memory[c->PC]*256 + c->Y; c->cycles=5; break; //abs,y //(4..5 cycles, 7 cycles for R-M-W)
    case 0x1D: c->PC++; c->addr=csid->memory[c->PC]; c->PC++; c->addr+=csid->memory[c->PC]*256 + c->X; c->cycles=5; break; //abs,c->x //(4..5 cycles, 7 cycles for R-M-W)
    case 0xD: case 0xF: c->PC++; c->addr=csid->memory[c->PC]; c->PC++; c->addr+=csid->memory[c->PC]*256; c->cycles=4; break; //abs
    case 0x15: c->PC++; c->addr = csid->memory[c->PC] + c->X; c->cycles=4; break; //zp,x
    case 5: case 7: c->PC++; c->addr = csid->memory[c->PC]; c->cycles=3; break; //zp
    case 0x17: c->PC++; if ((c->IR&0xC0)!=0x80) { c->addr = csid->memory[c->PC] + c->X; c->cycles=4; } //zp,c->x for illegal ocsid->cpu.pcodes
               else { c->addr = csid->memory[c->PC] + c->Y; c->cycles=4; }  break; //zp,y for LAX/SAX illegal ocsid->cpu.pcodes
    case 0x1F: c->PC++; if ((c->IR&0xC0)!=0x80) { c->addr = csid->memory[c->PC] + csid->memory[++c->PC]*256 + c->X; c->cycles=5; } //abs,c->x for illegal ocsid->cpu.pcodes
               else { c->addr = csid->memory[c->PC] + csid->memory[++c->PC]*256 + c->Y; c->cycles=5; }  break; //abs,y for LAX/SAX illegal ocsid->cpu.pcodes
    case 9: case 0xB: c->PC++; c->addr = c->PC; c->cycles=2;  //immediate
   }
 c->addr&=0xFFFF;
   switch (c->IR&0xE0) {
    case 0x60: if ((c->IR&0x1F)!=0xB) { if((c->IR&3)==3) {c->T=(csid->memory[c->addr]>>1)+(c->ST&1)*128; c->ST&=124; c->ST|=(c->T&1); csid->memory[c->addr]=c->T; c->cycles+=2;}   //ADC / RRA (ROR+ADC)
                c->T=c->A; c->A+=csid->memory[c->addr]+(c->ST&1); c->ST&=60; c->ST|=(c->A&128)|(c->A>255); c->A&=0xFF; c->ST |= (!c->A)<<1 | ( !((c->T^csid->memory[c->addr])&0x80) & ((c->T^c->A)&0x80) ) >> 1; }
               else { c->A&=csid->memory[c->addr]; c->T+=csid->memory[c->addr]+(c->ST&1); c->ST&=60; c->ST |= (c->T>255) | ( !((c->A^csid->memory[c->addr])&0x80) & ((c->T^c->A)&0x80) ) >> 1; //V-flag set by intermediate ADC mechanism: (c->A&mem)+mem
                c->T=c->A; c->A=(c->A>>1)+(c->ST&1)*128; c->ST|=(c->A&128)|(c->T>127); c->ST|=(!c->A)<<1; }  break; // ARR (AND+ROR, bit0 not going to C, but C and bit7 get exchanged.)
    case 0xE0: if((c->IR&3)==3 && (c->IR&0x1F)!=0xB) {csid->memory[c->addr]++;c->cycles+=2;}  c->T=c->A; c->A-=csid->memory[c->addr]+!(c->ST&1); //SBC / ISC(ISB)=INC+SBC
               c->ST&=60; c->ST|=(c->A&128)|(c->A>=0); c->A&=0xFF; c->ST |= (!c->A)<<1 | ( ((c->T^csid->memory[c->addr])&0x80) & ((c->T^c->A)&0x80) ) >> 1; break; 
    case 0xC0: if((c->IR&0x1F)!=0xB) { if ((c->IR&3)==3) {csid->memory[c->addr]--; c->cycles+=2;}  c->T=c->A-csid->memory[c->addr]; } // CMP / DCP(DEC+CMP)
               else {c->X=c->T=(c->A&c->X)-csid->memory[c->addr];} /*SBX(AXS)*/  c->ST&=124;c->ST|=(!(c->T&0xFF))<<1|(c->T&128)|(c->T>=0);  break;  //SBX (AXS) (CMP+DEX at the same time)
    case 0x00: if ((c->IR&0x1F)!=0xB) { if ((c->IR&3)==3) {c->ST&=124; c->ST|=(csid->memory[c->addr]>127); csid->memory[c->addr]<<=1; c->cycles+=2;}  
                c->A|=csid->memory[c->addr]; c->ST&=125;c->ST|=(!c->A)<<1|(c->A&128); } //ORA / SLO(ASO)=ASL+ORA
               else {c->A&=csid->memory[c->addr]; c->ST&=124;c->ST|=(!c->A)<<1|(c->A&128)|(c->A>127);}  break; //ANC (AND+Carry=bit7)
    case 0x20: if ((c->IR&0x1F)!=0xB) { if ((c->IR&3)==3) {c->T=(csid->memory[c->addr]<<1)+(c->ST&1); c->ST&=124; c->ST|=(c->T>255); c->T&=0xFF; csid->memory[c->addr]=c->T; c->cycles+=2;}  
                c->A&=csid->memory[c->addr]; c->ST&=125; c->ST|=(!c->A)<<1|(c->A&128); }  //AND / RLA (ROL+AND)
               else {c->A&=csid->memory[c->addr]; c->ST&=124;c->ST|=(!c->A)<<1|(c->A&128)|(c->A>127);}  break; //ANC (AND+Carry=bit7)
    case 0x40: if ((c->IR&0x1F)!=0xB) { if ((c->IR&3)==3) {c->ST&=124; c->ST|=(csid->memory[c->addr]&1); csid->memory[c->addr]>>=1; c->cycles+=2;}
                c->A^=csid->memory[c->addr]; c->ST&=125;c->ST|=(!c->A)<<1|(c->A&128); } //EOR / SRE(LSE)=LSR+EOR
                else {c->A&=csid->memory[c->addr]; c->ST&=124; c->ST|=(c->A&1); c->A>>=1; c->A&=0xFF; c->ST|=(c->A&128)|((!c->A)<<1); }  break; //ALR(ASR)=(AND+LSR)
    case 0xA0: if ((c->IR&0x1F)!=0x1B) { c->A=csid->memory[c->addr]; if((c->IR&3)==3) c->X=c->A; } //LDA / LAX (illegal, used by my 1 rasterline player) 
               else {c->A=c->X=c->SP=csid->memory[c->addr]&c->SP;} /*LAS(LAR)*/  c->ST&=125; c->ST|=((!c->A)<<1) | (c->A&128); break;  // LAS (LAR)
    case 0x80: if ((c->IR&0x1F)==0xB) { c->A = c->X & csid->memory[c->addr]; c->ST&=125; c->ST|=(c->A&128) | ((!c->A)<<1); } //XAA (TXA+AND), highly unstable on real 6502!
               else if ((c->IR&0x1F)==0x1B) { c->SP=c->A&c->X; csid->memory[c->addr]=c->SP&((c->addr>>8)+1); } //TAS(SHS) (SP=c->A&c->X, mem=S&H} - unstable on real 6502
               else {csid->memory[c->addr]=c->A & (((c->IR&3)==3)?c->X:0xFF); c->storadd=c->addr;}  break; //STA / SAX (at times same as AHX/SHX/SHY) (illegal) 
   }
  }
  
  else if(c->IR&2) {  //nybble2:  2:illegal/LDX, 6:c->A/c->X/INC/DEC, A:Accu-shift/reg.transfer/NOP, E:shift/c->X/INC/DEC
   switch (c->IR&0x1F) { //addressing modes
    case 0x1E: c->PC++; c->addr=csid->memory[c->PC]; c->PC++; c->addr+=csid->memory[c->PC]*256 + ( ((c->IR&0xC0)!=0x80) ? c->X:c->Y ); c->cycles=5; break; //abs,c->x / abs,y
    case 0xE: c->PC++; c->addr=csid->memory[c->PC]; c->PC++; c->addr+=csid->memory[c->PC]*256; c->cycles=4; break; //abs
    case 0x16: c->PC++; c->addr = csid->memory[c->PC] + ( ((c->IR&0xC0)!=0x80) ? c->X:c->Y ); c->cycles=4; break; //zp,c->x / zp,y
    case 6: c->PC++; c->addr = csid->memory[c->PC]; c->cycles=3; break; //zp
    case 2: c->PC++; c->addr = c->PC; c->cycles=2;  //imm.
   }  
   c->addr&=0xFFFF; 
   switch (c->IR&0xE0) {
    case 0x00: c->ST&=0xFE; case 0x20: if((c->IR&0xF)==0xA) { c->A=(c->A<<1)+(c->ST&1); c->ST&=124;c->ST|=(c->A&128)|(c->A>255); c->A&=0xFF; c->ST|=(!c->A)<<1; } //ASL/ROL (Accu)
      else { c->T=(csid->memory[c->addr]<<1)+(c->ST&1); c->ST&=124;c->ST|=(c->T&128)|(c->T>255); c->T&=0xFF; c->ST|=(!c->T)<<1; csid->memory[c->addr]=c->T; c->cycles+=2; }  break; //RMW (Read-Write-Modify)
    case 0x40: c->ST&=0xFE; case 0x60: if((c->IR&0xF)==0xA) { c->T=c->A; c->A=(c->A>>1)+(c->ST&1)*128; c->ST&=124;c->ST|=(c->A&128)|(c->T&1); c->A&=0xFF; c->ST|=(!c->A)<<1; } //LSR/ROR (Accu)
      else { c->T=(csid->memory[c->addr]>>1)+(c->ST&1)*128; c->ST&=124;c->ST|=(c->T&128)|(csid->memory[c->addr]&1); c->T&=0xFF; c->ST|=(!c->T)<<1; csid->memory[c->addr]=c->T; c->cycles+=2; }  break; //csid->memory (RMW)
    case 0xC0: if(c->IR&4) { csid->memory[c->addr]--; c->ST&=125;c->ST|=(!csid->memory[c->addr])<<1|(csid->memory[c->addr]&128); c->cycles+=2; } //DEC
      else {c->X--; c->X&=0xFF; c->ST&=125;c->ST|=(!c->X)<<1|(c->X&128);}  break; //DEX
    case 0xA0: if((c->IR&0xF)!=0xA) c->X=csid->memory[c->addr];  else if(c->IR&0x10) {c->X=c->SP;break;}  else c->X=c->A;  c->ST&=125;c->ST|=(!c->X)<<1|(c->X&128);  break; //LDX/TSX/TAX
    case 0x80: if(c->IR&4) {csid->memory[c->addr]=c->X;c->storadd=c->addr;}  else if(c->IR&0x10) c->SP=c->X;  else {c->A=c->X; c->ST&=125;c->ST|=(!c->A)<<1|(c->A&128);}  break; //STX/TXS/TXA
    case 0xE0: if(c->IR&4) { csid->memory[c->addr]++; c->ST&=125;c->T|=(!csid->memory[c->addr])<<1|(csid->memory[c->addr]&128); c->cycles+=2; } //INC/NOP
   }
  }
  
  else if((c->IR&0xC)==8) {  //nybble2:  8:register/status
   switch (c->IR&0xF0) {
    case 0x60: c->SP++; c->SP&=0xFF; c->A=csid->memory[0x100+c->SP]; c->ST&=125;c->ST|=(!c->A)<<1|(c->A&128); c->cycles=4; break; //PLA
    case 0xC0: c->Y++; c->Y&=0xFF; c->ST&=125;c->ST|=(!c->Y)<<1|(c->Y&128); break; //INY
    case 0xE0: c->X++; c->X&=0xFF; c->ST&=125;c->ST|=(!c->X)<<1|(c->X&128); break; //INX
    case 0x80: c->Y--; c->Y&=0xFF; c->ST&=125;c->ST|=(!c->Y)<<1|(c->Y&128); break; //DEY
    case 0x00: csid->memory[0x100+c->SP]=c->ST; c->SP--; c->SP&=0xFF; c->cycles=3; break; //PHP
    case 0x20: c->SP++; c->SP&=0xFF; c->ST=csid->memory[0x100+c->SP]; c->cycles=4; break; //PLP
    case 0x40: csid->memory[0x100+c->SP]=c->A; c->SP--; c->SP&=0xFF; c->cycles=3; break; //PHA
    case 0x90: c->A=c->Y; c->ST&=125;c->ST|=(!c->A)<<1|(c->A&128); break; //TYA
    case 0xA0: c->Y=c->A; c->ST&=125;c->ST|=(!c->Y)<<1|(c->Y&128); break; //TAY
    default: if(flagsw[c->IR>>5]&0x20) c->ST|=(flagsw[c->IR>>5]&0xDF); else c->ST&=255-(flagsw[c->IR>>5]&0xDF);  //CLC/SEC/CLI/SEI/CLV/CLD/SED
   }
  }
  
  else {  //nybble2:  0: control/branch/Y/compare  4: Y/compare  C:Y/compare/JMP
   if ((c->IR&0x1F)==0x10) { c->PC++; c->T=csid->memory[c->PC]; if(c->T&0x80) c->T-=0x100; //BPL/BMI/BVC/BVS/BCC/BCS/BNE/BEQ  relative branch 
    if(c->IR&0x20) {if (c->ST&branchflag[c->IR>>6]) {c->PC+=c->T;c->cycles=3;}} else {if (!(c->ST&branchflag[c->IR>>6])) {c->PC+=c->T;c->cycles=3;}}  } 
   else {  //nybble2:  0:Y/control/Y/compare  4:Y/compare  C:Y/compare/JMP
    switch (c->IR&0x1F) { //addressing modes
     case 0: c->PC++; c->addr = c->PC; c->cycles=2; break; //imm. (or abs.low for JSR/BRK)
     case 0x1C: c->PC++; c->addr=csid->memory[c->PC]; c->PC++; c->addr+=csid->memory[c->PC]*256 + c->X; c->cycles=5; break; //abs,x
     case 0xC: c->PC++; c->addr=csid->memory[c->PC]; c->PC++; c->addr+=csid->memory[c->PC]*256; c->cycles=4; break; //abs
     case 0x14: c->PC++; c->addr = csid->memory[c->PC] + c->X; c->cycles=4; break; //zp,x
     case 4: c->PC++; c->addr = csid->memory[c->PC]; c->cycles=3;  //zp
    }  
    c->addr&=0xFFFF;  
    switch (c->IR&0xE0) {
     case 0x00: csid->memory[0x100+c->SP]=c->PC%256; c->SP--;c->SP&=0xFF; csid->memory[0x100+c->SP]=c->PC/256;  c->SP--;c->SP&=0xFF; csid->memory[0x100+c->SP]=c->ST; c->SP--;c->SP&=0xFF; 
       c->PC = csid->memory[0xFFFE]+csid->memory[0xFFFF]*256-1; c->cycles=7; break; //BRK
     case 0x20: if(c->IR&0xF) { c->ST &= 0x3D; c->ST |= (csid->memory[c->addr]&0xC0) | ( !(c->A&csid->memory[c->addr]) )<<1; } //BIT
      else { csid->memory[0x100+c->SP]=(c->PC+2)%256; c->SP--;c->SP&=0xFF; csid->memory[0x100+c->SP]=(c->PC+2)/256;  c->SP--;c->SP&=0xFF; c->PC=csid->memory[c->addr]+csid->memory[c->addr+1]*256-1; c->cycles=6; }  break; //JSR
     case 0x40: if(c->IR&0xF) { c->PC = c->addr-1; c->cycles=3; } //JMP
      else { 
        if(c->SP>=0xFF){ 
            ret = 0xFE;
            goto returnCPU;
        } 
        c->SP++;c->SP&=0xFF; c->ST=csid->memory[0x100+c->SP]; c->SP++;c->SP&=0xFF; c->T=csid->memory[0x100+c->SP]; c->SP++;c->SP&=0xFF; c->PC=csid->memory[0x100+c->SP]+c->T*256-1; c->cycles=6; }  break; //RTI
     case 0x60: if(c->IR&0xF) { c->PC = csid->memory[c->addr]+csid->memory[c->addr+1]*256-1; c->cycles=5; } //JMP() (indirect)
      else { 
        if(c->SP>=0xFF) {
            ret = 0xFF;
            goto returnCPU;
        }
         c->SP++;c->SP&=0xFF; c->T=csid->memory[0x100+c->SP]; c->SP++;c->SP&=0xFF; c->PC=csid->memory[0x100+c->SP]+c->T*256-1; c->cycles=6; }  break; //RTS
     case 0xC0: c->T=c->Y-csid->memory[c->addr]; c->ST&=124;c->ST|=(!(c->T&0xFF))<<1|(c->T&128)|(c->T>=0); break; //CPY
     case 0xE0: c->T=c->X-csid->memory[c->addr]; c->ST&=124;c->ST|=(!(c->T&0xFF))<<1|(c->T&128)|(c->T>=0); break; //CPX
     case 0xA0: c->Y=csid->memory[c->addr]; c->ST&=125;c->ST|=(!c->Y)<<1|(c->Y&128); break; //LDY
     case 0x80: csid->memory[c->addr]=c->Y; c->storadd=c->addr;  //STY
    }
   }
  }

  c->PC++; //PC&=0xFFFF; 
  returnCPU:
  return ret; 
} 



//----------------------------- SID emulation -----------------------------------------

//Arrays to support the emulation:
#define PERIOD0 CLOCK_RATIO_DEFAULT //max(round(csid->clock_ratio),9)
#define STEP0 3 //ceil(PERIOD0/9.0)
const float ADSRperiods[16] = {PERIOD0, 32, 63, 95, 149, 220, 267, 313, 392, 977, 1954, 3126, 3907, 11720, 19532, 31251};
const Uint8 ADSRstep[16] =   {  STEP0, 1,  1,  1,  1,    1,   1,   1,   1,   1,    1,    1,    1,     1,     1,     1};
const Uint8 ADSR_exptable[256] = {1, 30, 30, 30, 30, 30, 30, 16, 16, 16, 16, 16, 16, 16, 16, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 4, 4, 4, 4, //pos0:1  pos6:30  pos14:16  pos26:8
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, //pos54:4 //pos93:2
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };


void cSID_init(cSID* csid, int samplerate)
{
    int i;
    csid->clock_ratio = C64_PAL_CPUCLK/samplerate;

    for(i=0;i<16;i++){
        csid->ADSRperiods[i] = ADSRperiods[i];
        csid->ADSRstep[i] = ADSRstep[i];
    }    

    if (csid->clock_ratio>9) {
        csid->ADSRperiods[0] = csid->clock_ratio;
        csid->ADSRstep[0] = ceil(csid->clock_ratio/9.0); 
    } else {
        csid->ADSRperiods[0] = 9.0;
        csid->ADSRstep[0] = 1;
    }

    csid->sid.cutoff_ratio_8580 = -2 * 3.14 * (12500 / 2048) / csid->samplerate; // -2 * 3.14 * ((82000/6.8) / 2048) / csid->samplerate; //approx. 30Hz..12kHz according to datasheet, but only for 6.8nF value, 22nF makes 9Hz...3.7kHz? wrong
    csid->sid.cap_6581_reciprocal = -1000000/CAP_6581; //lighten CPU-load in sample-callback
    csid->sid.cutoff_steepness_6581 = FILTER_DARKNESS_6581*(2048.0-VCR_FET_TRESHOLD); //pre-scale for 0...2048 cutoff-value range //lighten CPU-load in sample-callback
    //cutoff_bottom_6581 = 1 - exp( -1 / (0.000000000470*1500000) / csid->samplerate ); // 1 - exp( -2 * 3.14 * (26000/pow(2,9)/0.47) / csid->samplerate ); //around 140..220Hz cutoff is set by VCR-MOSFET limiter/shunt-resistor (1.5MOhm)
    //cutoff_top_6581 = 20000; //Hz // (26000/0.47);  // 1 - exp( -2 * 3.14 * (26000/0.47) / csid->samplerate);   //cutoff range is 9 octaves stated by datasheet, but process variation might eliminate any filter spec.
    //cutoff_ratio_6581 = -2 * 3.14 * (cutoff_top_6581 / 2048) / csid->samplerate; //(cutoff_top_6581-cutoff_bottom_6581)/(2048.0-192.0); //datasheet: 30Hz..12kHz with 2.2pF -> 140Hz..56kHz with 470pF?

    cSID_createCombinedWF(csid->sid.TriSaw_8580, 0.5, 2.2, 0.9);
    cSID_createCombinedWF(csid->sid.PulseSaw_8580, 0.23, 1.27, 0.55);
    cSID_createCombinedWF(csid->sid.PulseTriSaw_8580, 0.5, 1.6, 0.8);
    
    for(i = 0; i < 9; i++) {
        csid->sid.ADSRstate[i] = HOLDZERO_BITMASK; 
        csid->sid.envcnt[i] = 0;
        csid->sid.ratecnt[i] = 0; 
        csid->sid.phaseaccu[i] = 0;
        csid->sid.prevaccu[i] = 0;
        csid->sid.expcnt[i] = 0;
        csid->sid.prevSR[i]=0;
        csid->sid.noise_LFSR[i] = 0x7FFFFF; 
        csid->sid.prevwfout[i] = 0;
    }
    for(i = 0; i < 3; i++) {
        csid->sid.sourceMSBrise[i] = 0; csid->sid.sourceMSB[i] = 0;
        csid->sid.prevlowpass[i] = 0; csid->sid.prevbandpass[i] = 0;
    }
    cSID_initSID(csid);
}

//registers: 0:freql1  1:freqh1  2:pwml1  3:pwmh1  4:ctrl1  5:ad1   6:sr1  7:freql2  8:freqh2  9:pwml2 10:pwmh2 11:ctrl2 12:ad2  13:sr 14:freql3 15:freqh3 16:pwml3 17:pwmh3 18:ctrl3 19:ad3  20:sr3 
//           21:cutoffl 22:cutoffh 23:flsw_reso 24:vol_ftype 25:potX 26:potY 27:OSC3 28:ENV3
void cSID_initSID(cSID* csid) { 
    int i;

    for(i=0xD400; i<=0xD7FF; i++){
        csid->memory[i]=0; 
        for(i=0xDE00;i<=0xDFFF;i++) 
            csid->memory[i]=0;
    }

    for(i=0;i<9;i++) {
        csid->sid.ADSRstate[i] = HOLDZERO_BITMASK;
        csid->sid.ratecnt[i ]= csid->sid.envcnt[i]= csid->sid.expcnt[i] = 0;
    } 
}


//My SID implementation is similar to what I worked out in a SwinSID variant during 3..4 months of development. (So jsSID only took 2 weeks armed with this experience.)
//I learned the workings of ADSR/WAVE/filter operations mainly from the quite well documented resid and resid-fp codes.
//(The SID reverse-engineering sites were also good sources.)
//Note that I avoided many internal/automatic variables from the SID function, assuming better speed this way. (Not using stack as much, but I'm not sure and it may depend on platform...)
//(The same is true for CPU emulation and player-code.)
int cSID_SID(cSID* csid, char num, unsigned int baseaddr) //the SID emulation itself ('num' is the number of SID to iterate (0..2)
{
 //better keep these variables static so they won't slow down the routine like if they were internal automatic variables always recreated
 static Uint8 channel, ctrl, SR, prevgate, wf, test, *sReg, *vReg;
 static unsigned int accuadd, MSB, pw, wfout;
 static int tmp, step, lim, nonfilt, filtin, filtout, output;
 static float period, steep, rDS_VCR_FET, cutoff[3], resonance[3], ftmp;

 filtin=nonfilt=0; sReg = &csid->memory[baseaddr]; vReg = sReg;

 //treating 2SID and 3SID channels uniformly (0..5 / 0..8), this probably avoids some extra code
 for (channel = num * SID_CHANNEL_AMOUNT ; channel < (num + 1) * SID_CHANNEL_AMOUNT ; channel++, vReg += 7) {
  ctrl = vReg[4];

  //ADSR envelope-generator:
  SR = vReg[6]; tmp = 0;
  prevgate = (csid->sid.ADSRstate[channel] & GATE_BITMASK);
  if (prevgate != (ctrl & GATE_BITMASK)) { //gatebit-change?
   if (prevgate) csid->sid.ADSRstate[channel] &= 0xFF - (GATE_BITMASK | ATTACK_BITMASK | DECAYSUSTAIN_BITMASK);
   else { //falling edge
    csid->sid.ADSRstate[channel] = (GATE_BITMASK | ATTACK_BITMASK | DECAYSUSTAIN_BITMASK); //rising edge, also sets hold_zero_bit=0
    if ((SR & 0xF) > (csid->sid.prevSR[channel] & 0xF)) tmp = 1; //assume SR->GATE write order: workaround to have crisp soundstarts by triggering delay-bug
   }                                                      //(this is for the possible missed CTRL(GATE) vs SR register write order situations (1MHz CPU is cca 20 times faster than csid->samplerate)
  }
  csid->sid.prevSR[channel] = SR; //if(SR&0xF) ratecnt[channel]+=5;  //assume SR->GATE write order: workaround to have crisp soundstarts by triggering delay-bug
  csid->sid.ratecnt[channel] += csid->clock_ratio; if (csid->sid.ratecnt[channel] >= 0x8000) csid->sid.ratecnt[channel] -= 0x8000; //can wrap around (ADSR delay-bug: short 1st frame)
  //set ADSR period that should be checked against rate-counter (depending on ADSR state Attack/DecaySustain/Release)
  if (csid->sid.ADSRstate[channel] & ATTACK_BITMASK) step = vReg[5] >> 4;
  else if (csid->sid.ADSRstate[channel] & DECAYSUSTAIN_BITMASK) step = vReg[5] & 0xF;
  else step = SR & 0xF;
  period = ADSRperiods[step]; step = ADSRstep[step];
  if (csid->sid.ratecnt[channel] >= period && csid->sid.ratecnt[channel] < period + csid->clock_ratio && tmp == 0) { //ratecounter shot (matches rateperiod) (in genuine SID ratecounter is LFSR)
   csid->sid.ratecnt[channel] -= period; //compensation for timing instead of simply setting 0 on rate-counter overload
   if ((csid->sid.ADSRstate[channel] & ATTACK_BITMASK) || ++csid->sid.expcnt[channel] == ADSR_exptable[csid->sid.envcnt[channel]]) {
    if (!(csid->sid.ADSRstate[channel] & HOLDZERO_BITMASK)) {
     if (csid->sid.ADSRstate[channel] & ATTACK_BITMASK) 
     { csid->sid.envcnt[channel]+=step; if (csid->sid.envcnt[channel]>=0xFF) {csid->sid.envcnt[channel]=0xFF; csid->sid.ADSRstate[channel] &= 0xFF-ATTACK_BITMASK;} }
     else if ( !(csid->sid.ADSRstate[channel] & DECAYSUSTAIN_BITMASK) || csid->sid.envcnt[channel] > (SR&0xF0) + (SR>>4) )
     { csid->sid.envcnt[channel]-=step; if (csid->sid.envcnt[channel]<=0 && csid->sid.envcnt[channel]+step!=0) {csid->sid.envcnt[channel]=0; csid->sid.ADSRstate[channel]|=HOLDZERO_BITMASK;} }
    }
    csid->sid.expcnt[channel] = 0;
   }
  }
  csid->sid.envcnt[channel] &= 0xFF;
 
  //WAVE-generation code (phase accumulator and waveform-selector):
  test = ctrl & TEST_BITMASK;  wf = ctrl & 0xF0;  accuadd = (vReg[0] + vReg[1] * 256) * csid->clock_ratio;
  if (test || ((ctrl & SYNC_BITMASK) && csid->sid.sourceMSBrise[num])) csid->sid.phaseaccu[channel] = 0;
  else { csid->sid.phaseaccu[channel] += accuadd; if (csid->sid.phaseaccu[channel] > 0xFFFFFF) csid->sid.phaseaccu[channel] -= 0x1000000; }
  csid->sid.phaseaccu[channel] &= 0xFFFFFF; MSB = csid->sid.phaseaccu[channel] & 0x800000; csid->sid.sourceMSBrise[num] = (MSB > (csid->sid.prevaccu[channel] & 0x800000)) ? 1 : 0;
  if (wf & NOISE_BITMASK) { //noise waveform
   tmp = csid->sid.noise_LFSR[channel];
   if (((csid->sid.phaseaccu[channel] & 0x100000) != (csid->sid.prevaccu[channel] & 0x100000)) || accuadd >= 0x100000) //clock LFSR all time if clockrate exceeds observable at given csid->samplerate 
   { step = (tmp & 0x400000) ^ ((tmp & 0x20000) << 5); tmp = ((tmp << 1) + (step ? 1 : test)) & 0x7FFFFF; csid->sid.noise_LFSR[channel]=tmp; }
   //we simply zero output when other waveform is mixed with noise. On real SID LFSR continuously gets filled by zero and locks up. ($C1 waveform with pw<8 can keep it for a while...)
   wfout = (wf & 0x70) ? 0 : ((tmp & 0x100000) >> 5) + ((tmp & 0x40000) >> 4) + ((tmp & 0x4000) >> 1) + ((tmp & 0x800) << 1) + ((tmp & 0x200) << 2) + ((tmp & 0x20) << 5) + ((tmp & 0x04) << 7) + ((tmp & 0x01) << 8);
  } 
  else if (wf & PULSE_BITMASK) { //simple pulse
   pw = (vReg[2] + (vReg[3] & 0xF) * 256) * 16;  tmp = (int) accuadd >> 9;  
   if (0 < pw && pw < tmp) pw = tmp;  tmp ^= 0xFFFF;  if (pw > tmp) pw = tmp;  
   tmp = csid->sid.phaseaccu[channel] >> 8;
   if (wf == PULSE_BITMASK) { //simple pulse, most often used waveform, make it sound as clean as possible without oversampling
    //One of my biggest success with the SwinSID-variant was that I could clean the high-pitched and thin sounds.
    //(You might have faced with the unpleasant sound quality of high-pitched sounds without oversampling. We need so-called 'band-limited' synthesis instead.
    // There are a lot of articles about this issue on the internet. In a nutshell, the harsh edges produce harmonics that exceed the 
    // Nyquist frequency (csid->samplerate/2) and they are folded back into hearable range, producing unvanted ringmodulation-like effect.)
    //After so many trials with dithering/filtering/oversampling/etc. it turned out I can't eliminate the fukkin aliasing in time-domain, as suggested at pages.
    //Oversampling (running the wave-generation 8 times more) was not a way at 32MHz SwinSID. It might be an option on PC but I don't prefer it in JavaScript.)
    //The only solution that worked for me in the end, what I came up with eventually: The harsh rising and falling edges of the pulse are
    //elongated making it a bit trapezoid. But not in time-domain, but altering the transfer-characteristics. This had to be done
    //in a frequency-dependent way, proportionally to pitch, to keep the deep sounds crisp. The following code does this (my favourite testcase is Robocop3 intro):
    step = (accuadd>=255)? 65535/(accuadd/256.0) : 0xFFFF; //simple pulse, most often used waveform, make it sound as clean as possible without oversampling
    if (test) wfout=0xFFFF;
    else if (tmp<pw) { lim=(0xFFFF-pw)*step; if (lim>0xFFFF) lim=0xFFFF; tmp=lim-(pw-tmp)*step; wfout=(tmp<0)?0:tmp; } //rising edge
    else { lim=pw*step; if (lim>0xFFFF) lim=0xFFFF; tmp=(0xFFFF-tmp)*step-lim; wfout=(tmp>=0)?0xFFFF:tmp; } //falling edge
   }
   else { //combined pulse
    wfout = (tmp >= pw || test) ? 0xFFFF:0; //(this would be enough for a simple but aliased-at-high-pitches pulse)
    if (wf&TRI_BITMASK) { 
     if (wf&SAW_BITMASK) { wfout = wfout ? cSID_combinedWF(csid, num, channel, csid->sid.PulseTriSaw_8580,tmp>>4,1,vReg[1]) : 0; } //pulse+saw+triangle (waveform nearly identical to tri+saw)
     else { tmp=csid->sid.phaseaccu[channel]^(ctrl&RING_BITMASK?csid->sid.sourceMSB[num]:0); wfout = (wfout)? cSID_combinedWF(csid, num,channel,csid->sid.PulseSaw_8580,(tmp^(tmp&0x800000?0xFFFFFF:0))>>11,0,vReg[1]) : 0; } } //pulse+triangle
    else if (wf&SAW_BITMASK) wfout = wfout? cSID_combinedWF(csid, num, channel, csid->sid.PulseSaw_8580,tmp>>4,1,vReg[1]) : 0; //pulse+saw
   }
  }
  else if (wf&SAW_BITMASK) { //saw
   wfout=csid->sid.phaseaccu[channel]>>8; //saw (this row would be enough for simple but aliased-at-high-pitch saw)
   //The anti-aliasing (cleaning) of high-pitched sawtooth wave works by the same principle as mentioned above for the pulse,
   //but the sawtooth has even harsher edge/transition, and as the falling edge gets longer, tha rising edge should became shorter, 
   //and to keep the amplitude, it should be multiplied a little bit (with reciprocal of rising-edge steepness).
   //The waveform at the output essentially becomes an asymmetric triangle, more-and-more approaching symmetric shape towards high frequencies.
   //(If you check a recording from the real SID, you can see a similar shape, the high-pitch sawtooth waves are triangle-like...)
   //But for deep sounds the sawtooth is really close to a sawtooth, as there is no aliasing there, but deep sounds should be sharp...
   if (wf&TRI_BITMASK) wfout = cSID_combinedWF(csid, num,channel,csid->sid.TriSaw_8580,wfout>>4,1,vReg[1]); //saw+triangle
   else { //simple cleaned (bandlimited) saw
    steep=(accuadd/65536.0)/288.0;
    wfout += wfout*steep; if(wfout>0xFFFF) wfout=0xFFFF-(wfout-0x10000)/steep; 
   } 
  }
  else if (wf&TRI_BITMASK) { //triangle (this waveform has no harsh edges, so it doesn't suffer from strong aliasing at high pitches)
   tmp=csid->sid.phaseaccu[channel]^(ctrl&RING_BITMASK?csid->sid.sourceMSB[num]:0); wfout = (tmp^(tmp&0x800000?0xFFFFFF:0)) >> 7; 
  }
  wfout&=0xFFFF; if (wf) csid->sid.prevwfout[channel] = wfout; else { wfout = csid->sid.prevwfout[channel]; } //emulate waveform 00 floating wave-DAC (on real SID waveform00 decays after 15s..50s depending on temperature?)
  csid->sid.prevaccu[channel] = csid->sid.phaseaccu[channel]; csid->sid.sourceMSB[num] = MSB;            //(So the decay is not an exact value. Anyway, we just simply keep the value to avoid clicks and support SounDemon digi later...)

  //routing the channel signal to either the filter or the unfiltered master output depending on filter-switch SID-registers
  if (sReg[0x17] & FILTSW[channel]) filtin += ((int)wfout - 0x8000) * csid->sid.envcnt[channel] / 256;
  else if ((FILTSW[channel] != 4) || !(sReg[0x18] & OFF3_BITMASK)) 
   nonfilt += ((int)wfout - 0x8000) * csid->sid.envcnt[channel] / 256;
 }
 //update readable SID1-registers (some SID tunes might use 3rd channel ENV3/OSC3 value as control)
 if(num==0, csid->memory[1]&3) { sReg[0x1B]=wfout>>8; sReg[0x1C]=csid->sid.envcnt[3]; } //OSC3, ENV3 (some players rely on it)    

 //FILTER: two integrator loop bi-quadratic filter, workings learned from resid code, but I kindof simplified the equations
 //The phases of lowpass and highpass outputs are inverted compared to the input, but bandpass IS in phase with the input signal.
 //The 8580 cutoff frequency control-curve is ideal (binary-weighted resistor-ladder VCRs), while the 6581 has a treshold, and below that it 
 //outputs a constant ~200Hz cutoff frequency. (6581 uses MOSFETs as VCRs to control cutoff causing nonlinearity and some 'distortion' due to resistance-modulation.
 //There's a cca. 1.53Mohm resistor in parallel with the MOSFET in 6581 which doesn't let the frequency go below 200..220Hz
 //Even if the MOSFET doesn't conduct at all. 470pF capacitors are small, so 6581 can't go below this cutoff-frequency with 1.5MOhm.)
 cutoff[num] = sReg[0x16] * 8 + (sReg[0x15] & 7);
 if (csid->SID_model[num] == 8580) {
  cutoff[num] = ( 1 - exp((cutoff[num]+2) * csid->sid.cutoff_ratio_8580) ); //linear curve by resistor-ladder VCR
  resonance[num] = ( pow(2, ((4 - (sReg[0x17] >> 4)) / 8.0)) );
 } 
 else { //6581
  cutoff[num] += round(filtin*FILTER_DISTORTION_6581); //MOSFET-VCR control-voltage-modulation (resistance-modulation aka 6581 filter distortion) emulation
  rDS_VCR_FET = cutoff[num]<=VCR_FET_TRESHOLD ? 100000000.0 //below Vth treshold Vgs control-voltage FET presents an open circuit
   : csid->sid.cutoff_steepness_6581/(cutoff[num]-VCR_FET_TRESHOLD); // rDS ~ (-Vth*rDSon) / (Vgs-Vth)  //above Vth FET drain-source resistance is proportional to reciprocal of cutoff-control voltage
  cutoff[num] = ( 1 - exp( csid->sid.cap_6581_reciprocal / (VCR_SHUNT_6581*rDS_VCR_FET/(VCR_SHUNT_6581+rDS_VCR_FET)) / csid->samplerate ) ); //curve with 1.5MOhm VCR parallel Rshunt emulation
  resonance[num] = ( (sReg[0x17] > 0x5F) ? 8.0 / (sReg[0x17] >> 4) : 1.41 );
 }  
 filtout=0;
 ftmp = filtin + csid->sid.prevbandpass[num] * resonance[num] + csid->sid.prevlowpass[num];
 if (sReg[0x18] & HIGHPASS_BITMASK) filtout -= ftmp;
 ftmp = csid->sid.prevbandpass[num] - ftmp * cutoff[num];
 csid->sid.prevbandpass[num] = ftmp;
 if (sReg[0x18] & BANDPASS_BITMASK) filtout -= ftmp;
 ftmp = csid->sid.prevlowpass[num] + ftmp * cutoff[num];
 csid->sid.prevlowpass[num] = ftmp;
 if (sReg[0x18] & LOWPASS_BITMASK) filtout += ftmp;    

 //output stage for one SID
 //when it comes to $D418 volume-register digi playback, I made an AC / DC separation for $D418 value in the SwinSID at low (20Hz or so) cutoff-frequency,
 //and sent the AC (highpass) value to a 4th 'digi' channel mixed to the master output, and set ONLY the DC (lowpass) value to the volume-control.
 //This solved 2 issues: Thanks to the lowpass filtering of the volume-control, SID tunes where digi is played together with normal SID channels,
 //won't sound distorted anymore, and the volume-clicks disappear when setting SID-volume. (This is useful for fade-in/out tunes like Hades Nebula, where clicking ruins the intro.)
 output = (nonfilt+filtout) * (sReg[0x18]&0xF) / csid->OUTPUT_SCALEDOWN;
 if (output>=32767) output=32767; else if (output<=-32768) output=-32768; //saturation logic on overload (not needed if the callback handles it)
 return (int)output; // master output
}


//The anatomy of combined waveforms: The resid source simply uses 4kUint8 8bit samples from wavetable arrays, says these waveforms are mystic due to the analog behaviour.
//It's true, the analog things inside SID play a significant role in how the combined waveforms look like, but process variations are not so huge that cause much differences in SIDs.
//After checking these waveforms by eyes, it turned out for me that these waveform are fractal-like, recursively approachable waveforms.
//My 1st thought and trial was to store only a portion of the waveforms in table, and magnify them depending on phase-accumulator's state.
//But I wanted to understand how these waveforms are produced. I felt from the waveform-diagrams that the bits of the waveforms affect each other,
//hence the recursive look. A short C code proved by assumption, I could generate something like a pulse+saw combined waveform.
//Recursive calculations were not feasible for MCU of SwinSID, but for jsSID I could utilize what I found out and code below generates the combined waveforms into wavetables. 
//To approach the combined waveforms as much as possible, I checked out the SID schematic that can be found at some reverse-engineering sites...
//The SID's R-2R ladder WAVE DAC is driven by operation-amplifier like complementary FET output drivers, so that's not the place where I first thought the magic happens.
//These 'opamps' (for all 12 wave-bits) have single FETs as inputs, and they switch on above a certain level of input-voltage, causing 0 or 1 bit as R-2R DAC input.
//So the first keyword for the workings is TRESHOLD. These FET inputs are driven through serial switch FETs (wave-selector) that normally enables one waveform at a time.
//The phase-accumulator's output is brought to 3 kinds of circuitries for the 3 basic waveforms. The pulse simply drives
//all wave-selector inputs with a 0/1 depending on pulsewidth, the sawtooth has a XOR for triangle/ringmod generation, but what
//is common for all waveforms, they have an open-drain driver before the wave-selector, which has FETs towards GND and 'FET resistor' towards the power-supply rail.
//These outputs are clearly not designed to drive high loads, and normally they only have to drive the FETs input mentioned above.
//But when more of these output drivers are switched together by the switch-FETs in the wave-selector, they affect each other by loading each other.
//The pulse waveform, when selected, connects all of them together through a fairly strong connection, and its signal also affects the analog level (pulls below the treshold)...
//The farther a specific DAC bit driver is from the other, the less it affects its output. It turned out it's not powers of 2 but something else,
//that creates similar combined waveforms to that of real SID's... Note that combined waveforms never have values bigger than their sourcing sawtooth wave.
//The analog levels that get generated by the various bit drivers, that pull each other up/DOWN, depend on the resistances the components/wires inside the SID.
//And finally, what is output on the DAC depends on whether these analog levels are below or above the FET gate's treshold-level,
//That's how the combined waveform is generated. Maybe I couldn't explain well enough, but the code below is simple enough to understand the mechanism algoritmically.
//This simplified schematic exapmle might make it easier to understand sawtooth+pulse combination (must be observed with monospace fonts):
//                               _____            |-    .--------------.   /\/\--.
// Vsupply                /  .----| |---------*---|-    /    Vsupply   !    R    !      As can be seen on this schematic,
//  ------.       other   !  !   _____        !  TRES   \       \      !         /      the pulse wave-selector FETs 
//        !       saw bit *--!----| |---------'  HOLD   /       !     |-     2R  \      connect the neighbouring sawtooth
//        /       output  !  !                          !      |------|-         /      outputs with a fairly strong 
//     Rd \              |-  !WAVEFORM-SELECTOR         *--*---|-      !    R    !      connection to each other through
//        /              |-  !SWITCHING FETs            !  !    !      *---/\/\--*      their own wave-selector FETs.
//        ! saw-bit          !    _____                |-  !   ---     !         !      So the adjacent sawtooth outputs
//        *------------------!-----| |-----------*-----|-  !          |-         /      pull each other lower (or maybe a bit upper but not exceeding sawtooth line)
//        ! (weak drive,so   !  saw switch       ! TRES-!  `----------|-     2R  \      depending on their low/high state and
//       |- can be shifted   !                   ! HOLD !              !         /      distance from each other, causing
//  -----|- down (& up?)     !    _____          !      !              !     R   !      the resulting analog level that
//        ! by neighbours)   *-----| |-----------'     ---            ---   /\/\-*      will either turn the output on or not.
//   GND ---                 !  pulse switch                                     !      (Depending on their relation to treshold.)
//
//(As triangle waveform connects adjacent bits by default, the above explained effect becomes even stronger, that's why combined waveforms with thriangle are at 0 level most of the time.)

//in case you don't like these calculated combined waveforms it's easy to substitute the generated tables by pre-sampled 'exact' versions

unsigned int cSID_combinedWF(cSID* csid, char num, char channel, unsigned int* wfarray, int index, char differ6581, Uint8 freqh)
{
    csid->addf = 0.6+0.4/freqh;
    if(differ6581 && csid->SID_model[num]==6581) 
        index&=0x7FF; 
    csid->sid.prevwavdata[channel] = wfarray[index]*csid->addf + csid->sid.prevwavdata[channel]*(1.0-csid->addf);
    return csid->sid.prevwavdata[channel];
}

void cSID_createCombinedWF(unsigned int* wfarray, float bitmul, float bitstrength, float treshold) {
    int  i,j,k;
    for (i=0; i<4096; i++) {
        wfarray[i] = 0; 
        for (j=0; j<12;j++) {
            float bitlevel=( ( i >> j ) & 1); 
            for (k=0; k<12; k++) {
                if ( !((i >> k) & 1) ) bitlevel -= bitmul / pow(bitstrength, fabs(k-j));
            }
            wfarray[i] += (bitlevel>=treshold) ? pow(2,j) : 0; 
        } 
        wfarray[i] = 12;  
    }
}