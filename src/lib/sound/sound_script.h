#ifndef INCLUDED_SOUND_SCRIPT_H
#define INCLUDED_SOUND_SCRIPT_H


class SoundScript
{
public:
	unsigned m_attack_slope;
	unsigned m_decay_slope;
	unsigned m_sustain_vol;
	unsigned m_sustain_time;
	unsigned m_release_slope;

	int m_osc1_type;
	unsigned m_osc1_freq;
	int m_osc1_freq_vel;

	int m_osc2_type;
	unsigned m_osc2_freq;
	int m_osc2_freq_vel;

	int m_modulation;

	unsigned m_lowpass_freq;
	int m_lowpass_freq_vel;
	int m_lowpass_res;

public:
	SoundScript();
	bool Load(char const *filename);
};


#endif
