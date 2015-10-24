#include "lib/universal_include.h"
#include "lib/sound/sound_wave.h"

#include "lib/binary_file_reader.h"


SoundWave::SoundWave()
{
    m_samples = NULL;
    m_numSamples = 0;
}


bool SoundWave::Load(char const *filename)
{
    BinaryFileReader f;
    if (!f.Open(filename))
        return false;

    // 
    // Read header

    unsigned char buf[4];
    if (f.ReadBytes(4, buf) != 4 || memcmp(buf, "RIFF", 4) != 0)
        return false;

    unsigned chunkSize = f.ReadS32();

    if (f.ReadBytes(4, buf) != 4 || memcmp(buf, "WAVE", 4) != 0)
        return false;

    //
    // Read fmt chunk

    if (f.ReadBytes(4, buf) != 4 || memcmp(buf, "fmt ", 4) != 0)
        return false;
    unsigned fmtChunkSize = f.ReadU32();
    unsigned audioFormat = f.ReadU16();
    unsigned numChannels = f.ReadU16();
    unsigned sampleRate = f.ReadU32();
    unsigned byteRate = f.ReadU32();
    unsigned blockAlign = f.ReadU16();
    unsigned bitsPerSample = f.ReadU16();

    ReleaseAssert(audioFormat == 1, "Sample '%s' unsupported format", filename);
    ReleaseAssert(numChannels == 1, "Sample '%s' is not mono", filename);
    ReleaseAssert(blockAlign == 1, "Sample '%s' unsupported block alignment", filename);
    ReleaseAssert(bitsPerSample == 8 || bitsPerSample == 16, "Sample '%s' is not 8 or 16 bits sample depth", filename);


    //
    // Read data chunk

    if (f.ReadBytes(4, buf) != 4 || memcmp(buf, "data", 4) != 0)
        return false;
    unsigned dataChunkSize = f.ReadU32();

    m_numSamples = dataChunkSize;
    m_samples = new short [m_numSamples];

    for (int i = 0; i < dataChunkSize; i++)
        m_samples[i] = (short)(f.ReadU8() << 8) - 32768;

    return true;
}
