#ifndef INCLUDED_SOUND_SYSTEM_H
#define INCLUDED_SOUND_SYSTEM_H


#include "lib/sound/sound_generator.h"
#include "lib/vector3.h"


class StereoSample;


class SoundChannel
{
public:
	SoundSynth m_synth;
	bool m_active;
	int m_nextSampleIndex;	// Index into SoundSynth::m_outputSamples
    Vector3 m_pos;
    bool m_usePos;

	SoundChannel() { m_nextSampleIndex = SOUND_SYNTH_NUM_OUTPUT_SAMPLES; }
};



class SoundSystem
{
private:
    Vector3 m_listenerPos;

public:
	SoundChannel *m_channels;
	int m_numChannels;

	int *m_mixerBuf;

	short m_sawLut[256];
	short m_triangleLut[256];

	SoundSystem();

	void Advance(Vector3 const &listenerPos);
	void DeviceCallback(StereoSample *buf, unsigned int numSamples);

    void PlaySound(char const *filename, Vector3 const *pos);
};


extern SoundSystem *g_soundSystem;


#endif
