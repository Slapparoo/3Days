#include "3d.h"
#include <math.h>
static SDL_AudioSpec audio_spec;
static int64_t freq=0;
static int64_t sample;
static float *buffer;
static void AudioCB(void *ud,void *d,int len) {
    if(buffer)
    	memcpy(d,buffer,len);
    else
        memset(d,0,len);
}
static void AudioGen(void *ud,float *data,int len) {
    sample=0;
    len/=sizeof(float);
    int olen=len;
    double tmp;
    for(;len>0;) {
        double t=(double)++sample/(double)audio_spec.freq;
        if(!freq) {
            data[--len]=0;
            if(audio_spec.channels==2)
				data[--len]=0;
        } else {
			// What we do here is we Get the remainder of 2*freq*t and 1(we never go above 1)
			// We round to get a 0 or 1 value for a square wave
			//          ______       ____
			//  1       |     |     |
			//          |     |     |
			// -1   ____|     |_____|
            data[--len]=tmp=-1.0+2.0*round(fmod(2.0*t*freq,1.0));
            if(audio_spec.channels==2)
				data[--len]=tmp;
		}
		sample%=audio_spec.samples;
    }
} 
void InitSound() {
    SDL_AudioSpec want;
    SDL_memset(&want, 0, sizeof(want));
    want.freq=24000;
    want.format=AUDIO_F32;
    want.channels=2;
    want.samples=8192;
    want.callback=AudioCB;
    SDL_PauseAudioDevice(SDL_OpenAudioDevice(NULL,0,&want, &audio_spec,
		SDL_AUDIO_ALLOW_FREQUENCY_CHANGE|
		SDL_AUDIO_ALLOW_CHANNELS_CHANGE|
		SDL_AUDIO_ALLOW_SAMPLES_CHANGE
	),0);
    buffer=NULL;
}
void SndFreq(int64_t f) {
    freq=f;
    PoopFree(buffer);
    buffer=PoopMAlloc(sizeof(float)*audio_spec.channels*audio_spec.samples);
    AudioGen(NULL,buffer,MSize(buffer));
}
