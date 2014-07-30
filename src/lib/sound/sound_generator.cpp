#include "lib/universal_include.h"
#include "lib/sound/sound_generator.h"

#include <stdlib.h>
#include "lib/sound/sound_filter.h"
#include "lib/sound/sound_script.h"
#include "lib/sound/sound_system.h"


#define MAX_ADSR_VOL 0x7ffff


void SignalGenerator::Init(int waveType, unsigned freq, int freqVel)
{
	m_waveType = waveType;
	m_freq = freq;
	m_freqVel = freqVel;

	m_phase = 0;
}


void SignalGenerator::GenerateSamples(short *samples, unsigned numSamples)
{
	m_freq += m_freqVel;

	for (int i = 0; i < SOUND_SYNTH_NUM_OUTPUT_SAMPLES; i++)
	{
		m_phase += m_freq;
		m_phase &= 0x3fffff;
		unsigned index = m_phase >> 14;

		int sample;
		switch (m_waveType)
		{
		case Saw:
			sample = g_soundSystem->m_sawLut[index];
			break;
		case Triangle:
			sample = g_soundSystem->m_triangleLut[index];
			break;
		case Square:
			if (index < 128)
				sample = -32767;
			else
				sample = 32767;
			break;
		case Noise:
			sample = (rand() << 1) - 32767;
			break;
		}

		samples[i] = sample;
	}
}


int SignalGenerator::ParseWaveType(char const *s)
{
	if (stricmp(s, "saw") == 0)
		return Saw;
	else if (stricmp(s, "triangle") == 0)
		return Triangle;
	else if (stricmp(s, "square") == 0)
		return Square;
	else if (stricmp(s, "Noise") == 0)
		return Noise;
	return Invalid;
}



// ****************************************************************************

int SoundSynth::ParseModulationType(char const *s)
{
	if (stricmp(s, "Add") == 0)
		return ModulationAdd;
	else if (stricmp(s, "Multiply") == 0)
		return ModulationMultiply;
	return ModulationInvalid;
}


void SoundSynth::Init(SoundScript const *script)
{
	m_sigGen1.Init(script->m_osc1_type, script->m_osc1_freq * 90, script->m_osc1_freq_vel);
	m_sigGen2.Init(script->m_osc2_type, script->m_osc2_freq * 90, script->m_osc2_freq_vel);

	m_sigGenModulation = script->m_modulation;

	m_attackIncrement = script->m_attack_slope;
	m_decayDecrement = script->m_decay_slope;
	m_sustainVolume = script->m_sustain_vol * (0x7ffff/100);
	m_sustainDuration = script->m_sustain_time;
	m_releaseDecrement = script->m_release_slope;
	
	m_adsrCycle = Attack;
	m_volume = 0;

	m_filterResLowPass = NULL;
	if (script->m_lowpass_freq != 0)
	{
		m_filterLowPassFreq = script->m_lowpass_freq;
		m_filterLowPassFreqVel = script->m_lowpass_freq_vel;
		m_filterLowPassResonance = script->m_lowpass_res;

		m_filterResLowPass = new FilterResLowPass;
		m_filterResLowPass->SetParameters(script->m_lowpass_freq, script->m_lowpass_res);
	}

	m_filterDistortion = new FilterDistortion;
	m_filterDistortion->SetParameters(50.0f);
}


bool SoundSynth::GenerateSamples()
{
	m_sigGen1.GenerateSamples(m_outputSamples, SOUND_SYNTH_NUM_OUTPUT_SAMPLES);

	if (m_sigGen2.m_freq != 0)
	{
		m_sigGen2.GenerateSamples(m_outputSamples2, SOUND_SYNTH_NUM_OUTPUT_SAMPLES);

		if (m_sigGenModulation == ModulationMultiply)
		{
			for (int i = 0; i < SOUND_SYNTH_NUM_OUTPUT_SAMPLES; i++)
			{
				int sample = m_outputSamples[i] * abs(m_outputSamples2[i]);
				sample /= 32768;
				m_outputSamples[i] = sample;
			}
		}
		else 
		{
			for (int i = 0; i < SOUND_SYNTH_NUM_OUTPUT_SAMPLES; i++)
			{
				int sample = m_outputSamples[i] + m_outputSamples2[i];
				sample /= 2;
				m_outputSamples[i] = sample;
			}
		}
	}

	for (int i = 0; i < SOUND_SYNTH_NUM_OUTPUT_SAMPLES; i++)
	{
		switch (m_adsrCycle)
		{
		case Attack:
			m_volume += m_attackIncrement;
			if (m_volume > MAX_ADSR_VOL)
			{
				m_adsrCycle = Decay;
				m_volume = MAX_ADSR_VOL;
			}
			break;
		case Decay:
			m_volume -= m_decayDecrement;
			if (m_volume < m_sustainVolume)
			{
				m_adsrCycle = Sustain;
				m_volume = m_sustainVolume;
				m_sustainSamplesRemaining = m_sustainDuration;
			}
			break;
		case Sustain:
			if (m_sustainSamplesRemaining == 0)
				m_adsrCycle = Release;
			else
				m_sustainSamplesRemaining--;
			break;
		case Release:
			if (m_volume < m_releaseDecrement)
				m_adsrCycle = Complete;
			else
				m_volume -= m_releaseDecrement;
			break;
		}

		unsigned sample = m_outputSamples[i];
 		sample *= (m_volume >> 3);
 		sample >>= 16;
		m_outputSamples[i] = sample;
	}

	if (m_filterResLowPass)
	{
		if (m_filterLowPassFreqVel)
		{
			m_filterLowPassFreq += m_filterLowPassFreqVel;
			m_filterResLowPass->SetParameters(m_filterLowPassFreq, m_filterLowPassResonance);
		}

		m_filterResLowPass->ProcessSamples(m_outputSamples, SOUND_SYNTH_NUM_OUTPUT_SAMPLES);
	}

	if (m_filterDistortion)
	{
		m_filterDistortion->ProcessSamples(m_outputSamples, SOUND_SYNTH_NUM_OUTPUT_SAMPLES);
	}

	return m_adsrCycle != Complete;
}
