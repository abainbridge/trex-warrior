#include "lib/universal_include.h"
#include "lib/sound/sound_filter.h"

#include "lib/math_utils.h"


//*****************************************************************************
// Class FilterResLowPass
//*****************************************************************************

// Res from 0 to 100 (approx)
void FilterResLowPass::SetParameters(float freq, int res)
{
	float f = freq; // Cutoff frequency, from approx 0 Hz to SampleRate/2
	float r = (float)res * 0.014f;
	r = 1.4f - r;

	m_in1 = 0.0f;
	m_in2 = 0.0f;
	m_out1 = 0.0f;
	m_out2 = 0.0f;

	float c = 1.0 / tan(M_PI * f / 48000.0);
	m_a1 = 1.0 / (1.0 + r * c + c * c);
	m_a2 = 2.0 * m_a1;
	m_b1 = 2.0 * (1.0 - c * c) * m_a1;
	m_b2 = (1.0 - r * c + c * c) * m_a1;
}


void FilterResLowPass::ProcessSamples(signed short *data, unsigned int numSamples)
{
	for (unsigned int n = 0; n < numSamples; n++)
	{
		float in0 = data[n];
		float result = m_a1 * in0 + 
				   m_a2 * m_in1 + 
				   m_a1 * m_in2 - 
				   m_b1 * m_out1 - 
				   m_b2 * m_out2;

		if (result > 32765.0f) 
			result = 32765.0f;
		else if (result < -32765.0f) 
			result = -32765.0f;
		
		data[n] = result;

		m_in2 = m_in1;
		m_in1 = in0;
		m_out2 = m_out1;
		m_out1 = data[n];
	}
}


//*****************************************************************************
// Class FilterDistortion
//*****************************************************************************

void FilterDistortion::SetParameters(float gain)
{
	m_gain = gain;
}


void FilterDistortion::ProcessSamples(short *data, unsigned numSamples)
{
	for (int i = 0; i < numSamples; i++)
	{
		int s = data[i] * m_gain;
		if (s > 32767)
			s = 32767;
		else if (s < -32767)
			s = -32767;
		data[i] = s;
	}
}
