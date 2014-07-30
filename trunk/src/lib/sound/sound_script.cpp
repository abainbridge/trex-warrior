#include "lib/universal_include.h"
#include "lib/sound/sound_script.h"

#include <stdlib.h>
#include <string.h>
#include "lib/sound/sound_generator.h"
#include "lib/text_file_reader.h"


SoundScript::SoundScript()
{
	m_attack_slope = 1000;
	m_decay_slope = 1000;
	m_sustain_vol = 90;
	m_sustain_time = 16000;
	m_release_slope = 100;

	m_osc1_type = SignalGenerator::Triangle;
	m_osc1_freq = 10000;
	m_osc1_freq_vel = 0;

	m_osc2_freq = 0;
	m_osc2_freq_vel = 0;
	
	m_modulation = SoundSynth::ModulationMultiply;

	m_lowpass_freq = 0;
	m_lowpass_freq_vel = 0;
	m_lowpass_res = 0;
}


bool SoundScript::Load(char const *filename)
{
	TextFileReader f(filename);
	if (!f.IsOpen())
		return false;

	f.SetSeperatorChars(" \t=");
	while (f.ReadLine())
	{
		if (!f.TokenAvailable()) 
			continue;

		char const *name = f.GetNextToken();
		if (name[0] == '\n')
			continue;
		char const *val = f.GetNextToken();
		if (!name || !val)
			goto handle_error;
			
		if (stricmp(name, "attack_slope") == 0)
		{
			m_attack_slope = atoi(val);
		}
		else if (stricmp(name, "decay_slope") == 0)
		{
			m_decay_slope = atoi(val);
		}
		else if (stricmp(name, "sustain_vol") == 0)
		{
			m_sustain_vol = atoi(val);
		}
		else if (stricmp(name, "sustain_time") == 0)
		{
			m_sustain_time = atoi(val);
		}
		else if (stricmp(name, "release_slope") == 0)
		{
			m_release_slope = atoi(val);
		}
		else if (stricmp(name, "osc1_type") == 0)
		{
			m_osc1_type = SignalGenerator::ParseWaveType(val);
			if (m_osc1_type == SignalGenerator::Invalid)
				goto handle_error;
		}
		else if (stricmp(name, "osc1_freq") == 0)
		{
			m_osc1_freq = atoi(val);
		}
		else if (stricmp(name, "osc1_freq_vel") == 0)
		{
			m_osc1_freq_vel = atoi(val);
		}
		else if (stricmp(name, "osc2_type") == 0)
		{
			m_osc2_type = SignalGenerator::ParseWaveType(val);
			if (m_osc2_type == SignalGenerator::Invalid)
				goto handle_error;
		}
		else if (stricmp(name, "osc2_freq") == 0)
		{
			m_osc2_freq = atoi(val);
		}
		else if (stricmp(name, "osc2_freq_vel") == 0)
		{
			m_osc2_freq_vel = atoi(val);
		}
		else if (stricmp(name, "modulation") == 0)
		{
			m_modulation = SoundSynth::ParseModulationType(val);
			if (m_modulation == SoundSynth::ModulationInvalid)
				goto handle_error;
		}
 		else if (stricmp(name, "lowpass_freq") == 0)
 		{
			m_lowpass_freq = atoi(val);
 		}
		else if (stricmp(name, "lowpass_freq_vel") == 0)
		{
			m_lowpass_freq_vel = atoi(val);
		}
		else if (stricmp(name, "lowpass_res") == 0)
		{
			m_lowpass_res = atoi(val);
		}
		else
		{
			ReleaseAssert(0, "Parse error in '%s', line %d", filename, f.m_lineNum);
		}
	}

	return true;

handle_error:
	ReleaseAssert(0, "Parse error in '%s', line %d", filename, f.m_lineNum);
	return false;
}