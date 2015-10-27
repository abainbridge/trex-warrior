#include "lib/universal_include.h"
#include "sound_system.h"

#include "lib/sound/sound_device.h"
#include "lib/sound/sound_generator.h"
#include "lib/sound/sound_wave.h"
#include "lib/hi_res_time.h"
#include "lib/resource.h"


SoundSystem *g_soundSystem = NULL;



// ***************************************************************************
// SoundChannelBase
// ***************************************************************************

SoundChannelBase::SoundChannelBase() 
{ 
    m_active = false; 
}


unsigned short SoundChannelBase::GetVolume(Vector3 const *listenerPos)
{
    unsigned short channelVol = 1 << 15;

    if (m_usePos)
    {
        float distFromListenerSqrd = (*listenerPos - m_pos).Len();
        float vol = 150.0f / distFromListenerSqrd;
        if (vol < 1.0f)
            channelVol *= vol;
    }

    return channelVol;
}


// ***************************************************************************
// SoundChannelSynth
// ***************************************************************************

SoundChannelSynth::SoundChannelSynth()
: SoundChannelBase()
{ 
    m_type = TypeSynth; 
    m_nextSampleIndex = SOUND_SYNTH_NUM_OUTPUT_SAMPLES; 
}


// ***************************************************************************
// SoundChannelWave
// ***************************************************************************

SoundChannelWave::SoundChannelWave() 
: SoundChannelBase()
{ 
    m_type = TypeWave;
    m_index = 0;
}


// ***************************************************************************
// SoundSystem
// ***************************************************************************

static void SoundCallback(StereoSample *buf, unsigned int numSamples)
{
	g_soundSystem->DeviceCallback(buf, numSamples);
}


int SoundSystem::GetFreeWaveChannel()
{
    int freeChannelIndex = -1;
    for (int i = 0; i < m_numWaveChannels; i++)
    {
        if (!m_waveChannels[i].m_active)
        {
            freeChannelIndex = i;
            break;
        }
    }

    if (freeChannelIndex < 0)
    {
        double oldestStartTime = g_gameTime;
        for (int i = 0; i < m_numWaveChannels; i++)
        {
            if (m_waveChannels[i].m_startTime < oldestStartTime)
            {
                oldestStartTime = m_waveChannels[i].m_startTime;
                freeChannelIndex = i;
            }
        }
    }

    return freeChannelIndex;
}


SoundSystem::SoundSystem()
{
	// Fill out the saw look up table
	for (int i = 0; i < 256; i++)
		m_sawLut[i] = -32767 + i * (65535 / 256);

	// Fill out the triangle wave look up table
	for (int i = 0; i < 128; i++)
		m_triangleLut[i] = (i-64) * (65535 / 128);
	for (int i = 128; i < 256; i++)
		m_triangleLut[i] = 32767 + (128 - i) * (65535 / 128);

	m_numSynthChannels = 1;
	m_synthChannels = new SoundChannelSynth[m_numSynthChannels];

    m_numWaveChannels = 4;
    m_waveChannels = new SoundChannelWave[m_numWaveChannels];

	g_soundDevice = new SoundDevice();
	m_mixerBuf = new int [g_soundDevice->GetSamplesPerChunk()];
	g_soundDevice->SetCallback(SoundCallback);
}


void SoundSystem::Advance(Vector3 const &listenerPos)
{
    m_listenerPos = listenerPos;
	g_soundDevice->TopupBuffer();
}


void SoundSystem::DeviceCallback(StereoSample *buf, unsigned int numSamples)
{
	memset(m_mixerBuf, 0, sizeof(int) * numSamples);

	for (int j = 0; j < m_numSynthChannels; j++)
	{
		SoundChannelSynth *chan = &m_synthChannels[j];
        unsigned short channelVol = chan->GetVolume(&m_listenerPos);
		int mixerBufOffset = 0;

		while (chan->m_active)
		{
			int numExistingSamples = SOUND_SYNTH_NUM_OUTPUT_SAMPLES - chan->m_nextSampleIndex;
			int numRemainingSamplesRequired = numSamples - mixerBufOffset;
			int numSamplesToTakeNow = min(numExistingSamples, numRemainingSamplesRequired);
			
			// Add samples from this synth to the mix buffer
			for (int i = 0; i < numSamplesToTakeNow; i++)
			{
                int sample = chan->m_synth.m_outputSamples[chan->m_nextSampleIndex];
				sample = (sample * channelVol) >> 16;
                m_mixerBuf[mixerBufOffset] += sample;
				mixerBufOffset++;
				chan->m_nextSampleIndex++;
			}

			numRemainingSamplesRequired -= numSamplesToTakeNow;

			// Get the synth to generate more samples if needed
			if (numRemainingSamplesRequired > 0)
			{
				chan->m_active = chan->m_synth.GenerateSamples();
				chan->m_nextSampleIndex = 0;
			}
			else
			{
				break;
			}
		}		
	}

    for (int j = 0; j < m_numWaveChannels; j++)
    {
        SoundChannelWave *chan = &m_waveChannels[j];

        if (chan->m_active)
        {
            unsigned short channelVol = chan->GetVolume(&m_listenerPos);
            int mixerBufOffset = 0;

            // Add samples from this wave to the mix buffer
            for (int i = 0; i < numSamples; i++)
            {
                int index = (int)chan->m_index;
                int sample = chan->m_wave->m_samples[index];
                sample = (sample * channelVol) >> 16;
                m_mixerBuf[mixerBufOffset] += sample;
                mixerBufOffset++;
                chan->m_index += 0.5f;

                if (chan->m_index >= chan->m_wave->m_numSamples)
                {
                    chan->m_active = false;
                    break;
                }
            }
        }
    }

	// Mix the channels into the SoundDevice's buffer
	int totalNumChannels = m_numSynthChannels + m_numWaveChannels;
    for (int i = 0; i < numSamples; i++)
	{
		buf[i].m_left = m_mixerBuf[i] / totalNumChannels;
		buf[i].m_right = buf[i].m_left;
	}
}


void SoundSystem::PlaySynth(char const *filename, Vector3 const *pos)
{
	SoundScript *soundScript = g_resourceManager.GetSoundScript(filename);
    if (!soundScript)
    {
        ReleaseAssert("Couldn't open sound script '%s'", filename);
        return;
    }

	int freeChannelIndex = 0;
	for (int i = 0; i < m_numSynthChannels; i++)
	{
		if (!m_synthChannels[i].m_active)
		{
			freeChannelIndex = i;
			break;
		}
	}
	
    SoundChannelSynth *channel = &m_synthChannels[freeChannelIndex];
	channel->m_active = true;
	channel->m_synth.Init(soundScript);
    if (pos)
    {
        channel->m_pos = *pos;
        channel->m_usePos = true;
    }
    else
    {
        channel->m_usePos = false;
    }
}


void SoundSystem::PlayWave(char const *filename, Vector3 const *pos)
{
    SoundWave *soundWave = g_resourceManager.GetSoundSample(filename);
    if (!soundWave)
    {
        ReleaseAssert("Couldn't open sound sample '%s'", filename);
        return;
    }

    int freeChannelIndex = GetFreeWaveChannel();

    SoundChannelWave *channel = &m_waveChannels[freeChannelIndex];
    channel->m_active = true;
    channel->m_wave = soundWave;
    channel->m_index = 0.0f;
    channel->m_startTime = g_gameTime;
    if (pos)
    {
        channel->m_pos = *pos;
        channel->m_usePos = true;
    }
    else
    {
        channel->m_usePos = false;
    }
}
