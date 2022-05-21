#include "3d.h"
static SDL_AudioSpec audio_spec;
static int64_t freq=0;
static void AudioCB(void *ud,float *data,int len) {
    double start,time=0.0;
    if(!freq) {
        memset(data,0,len);
        return;
    }
    len/=sizeof(float);
    int olen=len;
    for(start=0;start<olen/2;start++) {
        time=start*((double)audio_spec.freq/olen);
        if(1&(int64_t)((2*freq*time)/audio_spec.freq)) {
            data[--len]=1.0;
        } else {
            data[--len]=0.;
        }
    }
} 
void InitSound() {
    SDL_AudioSpec want;
    SDL_memset(&want, 0, sizeof(want));
    want.freq=48000;
    want.format=AUDIO_F32;
    want.channels=1;
    want.samples=4096;
    want.callback=AudioCB;
    SDL_PauseAudioDevice(SDL_OpenAudioDevice(NULL,0,&want, &audio_spec,SDL_AUDIO_ALLOW_ANY_CHANGE),0);
}
void SndFreq(int64_t f) {
    freq=f;
}
