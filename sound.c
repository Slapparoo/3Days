#include "3d.h"
#include <math.h>
#define MA_NO_DECODING
#define MA_NO_ENCODING
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
static ma_waveform waveform;
static ma_device_config audio_spec;
static ma_device audio_device;
static int64_t freq=0;
void AudioCB(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
	ma_waveform_read_pcm_frames(&waveform,pOutput,frameCount,NULL);
}
void InitSound() {
	audio_spec=ma_device_config_init(ma_device_type_playback);
	audio_spec.playback.format=ma_format_s16;
	audio_spec.playback.channels=2;
	audio_spec.sampleRate=24000;
	audio_spec.dataCallback=&AudioCB;
    assert(ma_device_init(NULL,&audio_spec,&audio_device)==MA_SUCCESS);
}
void SndFreq(int64_t f) {
	freq=f;
	ma_waveform_config config = ma_waveform_config_init(
		audio_spec.playback.format,
		audio_spec.playback.channels,
		audio_spec.sampleRate,
		ma_waveform_type_square,
		1.,
		f);
	ma_result result = ma_waveform_init(&config, &waveform);
	assert(ma_device_start(&audio_device)==MA_SUCCESS);
}
