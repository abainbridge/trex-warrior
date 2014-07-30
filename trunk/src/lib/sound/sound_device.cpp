#include "lib/universal_include.h"

#include "lib/debug_utils.h"

#include "lib/sound/sound_device.h"


static HWAVEOUT	s_device;
SoundDevice *g_soundDevice = NULL;



//*****************************************************************************
// Class StereoSampleBuf
//*****************************************************************************

StereoSampleBuf::StereoSampleBuf()
{
	// Allocate the buffer
	int numSamples = 2000;
	m_buffer = new StereoSample[numSamples];

	// Clear the buffer
	memset(m_buffer, 0, numSamples * sizeof(StereoSample));

	// Register the buffer with Windows
	memset(&m_header, 0, sizeof(WAVEHDR));
	m_header.lpData = (char*)m_buffer;
	int blockAlign = 4;		// 2 channels * 2 bytes per sample
	m_header.dwBufferLength = numSamples * blockAlign;
	m_header.dwFlags = WHDR_DONE;
	int result = waveOutPrepareHeader(s_device, &m_header, sizeof(WAVEHDR));
	ReleaseAssert(result == MMSYSERR_NOERROR, "Couldn't init buffer");

	// Play the buffer
	result = waveOutWrite(s_device, &m_header, sizeof(WAVEHDR));
	ReleaseAssert(result == MMSYSERR_NOERROR, "Couldn't send sound data");
}


StereoSampleBuf::~StereoSampleBuf()
{
	waveOutUnprepareHeader(s_device, &m_header, sizeof(WAVEHDR));
	delete [] m_buffer;			
	m_buffer = NULL;
}



//*****************************************************************************
// Class SoundDevice
//*****************************************************************************

void CALLBACK WaveOutProc(HWAVEOUT _dev, UINT _msg, DWORD _userData, DWORD _param1, DWORD _param2)
{
	if (_msg != WOM_DONE) 
		return;
	if (!s_device)	
		return;
	if (!g_soundDevice || !g_soundDevice->m_callback) 
		return;

	g_soundDevice->m_fillsRequested++;
}


SoundDevice::SoundDevice()
:	m_callback(NULL),
	m_numBuffers(4),
	m_nextBuffer(0),
	m_fillsRequested(0)
{
	DebugAssert(!g_soundDevice);

	m_freq = 48000;
	m_samplesPerBuffer = 2000;


	//
	// Initialise the output device

	WAVEFORMATEX format;
	memset(&format, 0, sizeof(WAVEFORMATEX));
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nChannels = 2;
	format.nSamplesPerSec = m_freq;
	format.wBitsPerSample = 16;
	format.nBlockAlign = 4;		// 2 channels * 2 bytes per sample
	format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
	int result = waveOutOpen(&s_device, WAVE_MAPPER, &format, (DWORD)&WaveOutProc, 0, CALLBACK_FUNCTION);
	char const *errString = NULL;
	switch (result)
	{
		case MMSYSERR_ALLOCATED:	errString = "Specified resource is already allocated";	break;
		case MMSYSERR_BADDEVICEID:	errString = "Specified device ID is out of range";		break;
		case MMSYSERR_NODRIVER:		errString = "No device driver is present";				break;
		case MMSYSERR_NOMEM:		errString = "Unable to allocate or lock memory";		break;
		case WAVERR_BADFORMAT:		errString = "Attempted to open with an unsupported waveform-audio format";	break;
		case WAVERR_SYNC:			errString = "Device is synchronous but waveOutOpen called without WAVE_ALLOWSYNC flag";	break;
	}
	
	ReleaseAssert(result == MMSYSERR_NOERROR, "Failed to open audio output device: \"%s\"", errString);


	// 
	// Create the sound buffers

	m_buffers = new StereoSampleBuf[m_numBuffers];
}


void SoundDevice::SetCallback(void (*_callback)(StereoSample *, unsigned int))
{
	m_callback = _callback;
}


void SoundDevice::TopupBuffer()
{
	while (m_fillsRequested)
	{
		StereoSampleBuf *buf = &m_buffers[m_nextBuffer];

		g_soundDevice->m_callback(buf->m_buffer, g_soundDevice->m_samplesPerBuffer);

		int result = waveOutWrite(s_device, &buf->m_header, sizeof(WAVEHDR));
		if (result != MMSYSERR_NOERROR)
		{
			break;
		}
		else
		{
			m_nextBuffer++;
			m_nextBuffer %= m_numBuffers;
			m_fillsRequested--;
		}
	}
}
