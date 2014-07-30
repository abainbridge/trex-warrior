#ifndef INCLUDED_SOUND_FILTER_H
#define INCLUDED_SOUND_FILTER_H


//*****************************************************************************
// Class FilterResLowPass
//*****************************************************************************

class FilterResLowPass
{
private:
	float m_in1;	// in[n-1]
	float m_in2;	// in[n-2]
	float m_out1;	// out[n-1]
	float m_out2;	// out[n-2]

	float   m_a1;
	float   m_a2;
	float   m_b1;
	float   m_b2;

public:
    void SetParameters  (float freq, int res);
	void ProcessSamples (short *data, unsigned numSamples);
};


//*****************************************************************************
// Class FilterDistortion
//*****************************************************************************

class FilterDistortion
{
private:
	float m_gain;

public:
	void SetParameters(float gain);
	void ProcessSamples(short *data, unsigned numSamples);
};


 
// //*****************************************************************************
// // Class DspBitCrusher
// //*****************************************************************************
// 
// class DspBitCrusher : public DspEffect
// {
// protected:
//     float m_bitRate;
// 
// public:
//     DspBitCrusher(int _sampleRate);
// 
//     void SetParameters  ( float const *_params );
// 	void Process        ( signed short *_data, unsigned int _numSamples);
//    
// };
// 
// 
// //*****************************************************************************
// // Class DspGargle
// //*****************************************************************************
// 
// class DspGargle : public DspEffect
// {
// protected:
// 	enum
// 	{
// 		WaveTriangle,
// 		WaveSquare
// 	};
// 
// 	float				m_wetDryMix;		// Percentage figure
// 	float				m_freq;
// 	float				m_phase;
// 	int					m_waveType;
// 
// 	void ProcessTriangle(signed short *_data, unsigned int _numSamples);
// 	void ProcessSquare	(signed short *_data, unsigned int _numSamples);
// 
// public:
// 	DspGargle(int _sampleRate);
// 
// 	void SetParameters	(float const *_params);
// 	void Process		(signed short *_data, unsigned int _numSamples);
// };
// 
// 
// //*****************************************************************************
// // Class DspEcho
// //*****************************************************************************
// 
// class DspEcho : public DspEffect
// {
// protected:
// 	float				*m_buffer;
// 	float				m_wetDryMix;		// Percentage figure
// 	float				m_delay;			// In ms
// 	float				m_attenuation;		// Amount echo attenuates by with each repeat. Range 0 to 100. 100=echo repeats forever
// 
// 	int					m_currentBufferIndex;
// 
// public:
// 	DspEcho(int _sampleRate);
// 	~DspEcho();
// 
// 	void SetParameters	(float const *_params);
// 	void Process		(signed short *_data, unsigned int _numSamples);
// };
// 
// 
// //*****************************************************************************
// // Class DspReverb
// //*****************************************************************************
// 
// #define NUM_REVERB_DELAY_UNITS	6
// class DspReverb : public DspEffect
// {
// protected:
// 	signed short		*m_buffer;
// 	unsigned int		m_bufferSize;	// In samples
// 	int					m_currentBufferIndex;
// 	float				m_wetDryMix;
// 	int					m_delays[NUM_REVERB_DELAY_UNITS];
// 	float				m_decays[NUM_REVERB_DELAY_UNITS];
// 
// public:
// 	DspReverb(int _sampleRate);
// 	~DspReverb();
// 
// 	void SetParameters	(float const *_params);
// 	void Process		(signed short *_data, unsigned int _numSamples);
// };
// 

#endif
