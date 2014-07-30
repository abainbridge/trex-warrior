#ifndef INCLUDED_SOUND_GENERATOR_H
#define INCLUDED_SOUND_GENERATOR_H


#define SOUND_SYNTH_NUM_OUTPUT_SAMPLES 256


class FilterResLowPass;
class FilterDistortion;
class SoundScript;


class SignalGenerator
{
public:
	enum 
	{
		Invalid,
		Saw,
		Triangle,
		Square,
		Noise,		
		// Remember to update ParseWaveType if updating this enum
	};

	int m_waveType;
	unsigned m_freq;
	int m_freqVel;

private:
	unsigned m_phase;

public:
	void Init(int waveType, unsigned freq, int freqVel);
	void GenerateSamples(short *samples, unsigned numSamples);

	static int ParseWaveType(char const *s);
};


// ****************************************************************************

class SoundSynth
{
public:
	enum
	{
		ModulationInvalid,
		ModulationAdd,
		ModulationMultiply,
		// Remember to update ParseModulationType if updating this enum
	};

	int m_sigGenModulation;
	SignalGenerator m_sigGen1;
	SignalGenerator m_sigGen2;

	// Volume units below are simply that 2^19-1 is full volume, 0 is, er, zero volume
	unsigned m_attackIncrement;	// Amount to increment the volume by for each sample generated in the attack cycle
	unsigned m_decayDecrement;	// Amount to decrement the volume by for each sample generated in the attack cycle
	unsigned m_sustainVolume;	// Volume for the sustain cycle
	unsigned m_sustainDuration;	// In samples
	unsigned m_releaseDecrement;// Amount to decrement the volume by for each sample generated in the attack cycle

	FilterResLowPass *m_filterResLowPass;
	unsigned m_filterLowPassFreq;
	int m_filterLowPassFreqVel;
	int m_filterLowPassResonance;

	FilterDistortion *m_filterDistortion;

	short m_outputSamples[SOUND_SYNTH_NUM_OUTPUT_SAMPLES];
	short m_outputSamples2[SOUND_SYNTH_NUM_OUTPUT_SAMPLES];

private:
	enum
	{
		Attack,
		Decay,
		Sustain,
		Release,
		Complete
	};
	
	int m_adsrCycle;
	
	unsigned m_volume;
	unsigned m_sustainSamplesRemaining;
	
public:
	static int ParseModulationType(char const *s);

	void Init(SoundScript const *script);

	bool GenerateSamples();	// Fills m_outputSamples
};


#endif
