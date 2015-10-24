#ifndef INCLUDED_SOUND_WAVE_H
#define INCLUDED_SOUND_WAVE_H


class SoundWave
{
public:
    short *m_samples;
    int m_numSamples;

    SoundWave();
    bool Load(char const *filename);
};


#endif
