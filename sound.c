#include "3d.h"
static SDL_AudioSpec audio_spec;
static int64_t freq=0;
static int64_t sample;
static void AudioCB(void *ud,float *data,int len) {
    len/=sizeof(float);
    int olen=len;
    for(;len>=0;) {
        double t=(double)++sample/(double)audio_spec.freq;
        if(!freq)
            data[--len]=0;
        else
            data[--len]=-1.0+2.0*round(fmod(2.0*t*freq,1.0));
    }
    sample%=audio_spec.freq;
} 
void InitSound() {
    SDL_AudioSpec want;
    SDL_memset(&want, 0, sizeof(want));
    want.freq=24000;
    want.format=AUDIO_F32;
    want.channels=1;
    want.samples=4096;
    want.callback=AudioCB;
    SDL_PauseAudioDevice(SDL_OpenAudioDevice(NULL,0,&want, &audio_spec,SDL_AUDIO_ALLOW_ANY_CHANGE),0);
}
void SndFreq(int64_t f) {
    freq=f;
}
