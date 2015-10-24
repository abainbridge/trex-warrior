#ifndef INCLUDED_SOUND_SYSTEM_H
#define INCLUDED_SOUND_SYSTEM_H


#include "lib/sound/sound_generator.h"
#include "lib/vector3.h"


class StereoSample;
class SoundWave;


class SoundChannelBase
{
public:
    enum
    {
        TypeSynth,
        TypeWave
    };

    unsigned m_type;
    bool m_active;
    Vector3 m_pos;
    bool m_usePos;

    SoundChannelBase();

    unsigned short GetVolume(Vector3 const *listenerPos);
};


class SoundChannelSynth: public SoundChannelBase
{
public:
	SoundSynth m_synth;
	int m_nextSampleIndex;	// Index into SoundSynth::m_outputSamples

    SoundChannelSynth();
};


class SoundChannelWave: public SoundChannelBase
{
public:
    SoundWave *m_wave;
    float m_index;  // Index into m_wave->m_samples

    SoundChannelWave();
};


class SoundSystem
{
private:
    Vector3 m_listenerPos;

public:
    SoundChannelSynth *m_synthChannels;
    int m_numSynthChannels;

    SoundChannelWave *m_waveChannels;
    int m_numWaveChannels;

	int *m_mixerBuf;

	short m_sawLut[256];
	short m_triangleLut[256];

	SoundSystem();

	void Advance(Vector3 const &listenerPos);
	void DeviceCallback(StereoSample *buf, unsigned int numSamples);

    void PlaySynth(char const *filename, Vector3 const *pos);
    void PlayWave(char const *filename, Vector3 const *pos);
};


extern SoundSystem *g_soundSystem;


#endif
