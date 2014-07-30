#include "lib/universal_include.h"
#include "sound_system.h"

#include "lib/sound/sound_device.h"
#include "lib/sound/sound_generator.h"
#include "lib/resource.h"


SoundSystem *g_soundSystem = NULL;


static void SoundCallback(StereoSample *buf, unsigned int numSamples)
{
	g_soundSystem->DeviceCallback(buf, numSamples);
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

	m_numChannels = 4;
	m_channels = new SoundChannel[4];

	g_soundDevice = new SoundDevice();
	m_mixerBuf = new int [g_soundDevice->GetSamplesPerChunk()];
	g_soundDevice->SetCallback(SoundCallback);

	for (int i = 0; i < m_numChannels; i++)
		m_channels[i].m_active = false;
}


void SoundSystem::Advance(Vector3 const &listenerPos)
{
    m_listenerPos = listenerPos;
	g_soundDevice->TopupBuffer();
}


void SoundSystem::DeviceCallback(StereoSample *buf, unsigned int numSamples)
{
	memset(m_mixerBuf, 0, sizeof(int) * numSamples);

	for (int j = 0; j < m_numChannels; j++)
	{
		SoundChannel *chan = &m_channels[j];

        unsigned short channelVol = 1 << 15;
        if (chan->m_usePos)
        {
            float distFromListenerSqrd = (m_listenerPos - chan->m_pos).Len();
            float vol = 50.0f / distFromListenerSqrd;
            if (vol < 1.0f)
                channelVol *= vol;
        }

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

	// Mix the channels into the SoundDevice's buffer
	for (int i = 0; i < numSamples; i++)
	{
		buf[i].m_left = m_mixerBuf[i] / m_numChannels;
		buf[i].m_right = buf[i].m_left;
	}
}


#undef PlaySound
void SoundSystem::PlaySound(char const *filename, Vector3 const *pos)
{
	SoundScript *soundScript = g_resourceManager.GetSoundScript(filename);

	int freeChannelIndex = 0;
	for (int i = 0; i < m_numChannels; i++)
	{
		if (!m_channels[i].m_active)
		{
			freeChannelIndex = i;
			break;
		}
	}
	
    SoundChannel *channel = &m_channels[freeChannelIndex];
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
