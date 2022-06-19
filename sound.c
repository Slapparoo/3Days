#include "3d.h"
#include <math.h>
#define MA_NO_DECODING
#define MA_NO_ENCODING
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
static ma_device_config audio_spec;
static ma_device audio_device;
static int64_t freq=0;
static int64_t sample;
static void AudioCB(ma_device *d,float *data,void *ud,ma_uint32 len) {
    int olen=len;
    for(;len>0;) {
        double t=(double)++sample/(double)audio_spec.sampleRate;
        if(!freq)
            data[--len]=0;
        else
			// What we do here is we Get the remainder of 2*freq*t and 1(we never go above 1)
			// We round to get a 0 or 1 value for a square wave
			//          ______       ____
			//  1       |     |     |
			//          |     |     |
			// -1   ____|     |_____|
            data[--len]=-1.0+2.0*round(fmod(2.0*t*freq,1.0));
    }
    sample%=audio_spec.sampleRate;
} 
void InitSound() {
	audio_spec=ma_device_config_init(ma_device_type_playback);
	audio_spec.playback.format=ma_format_f32;
	audio_spec.playback.channels=1;
	audio_spec.sampleRate=24000;
	audio_spec.dataCallback=&AudioCB;
    assert(ma_device_init(NULL,&audio_spec,&audio_device)==MA_SUCCESS);
}
void SndFreq(int64_t f) {
	if(!f)
		assert(ma_device_stop(&audio_device)==MA_SUCCESS);
	else
		assert(ma_device_start(&audio_device)==MA_SUCCESS);
    freq=f;
}
