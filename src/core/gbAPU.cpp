#include <SDL2/SDL.h>

#include "gbAPU.hpp"
#include "gbGameBoy.hpp"

using namespace GeimBoi;

static const uint16_t samples = 256;
static SDL_AudioSpec spec;

gbAPU::gbAPU(gbGameBoy* emu)
    : channel1(emu), channel2(emu),  mGameBoy(emu)
{
    SDL_InitSubSystem(SDL_INIT_AUDIO);

    // Init format
    spec.freq = 44100;
    spec.format = AUDIO_F32SYS;
    spec.samples = samples;
    spec.callback = sdl2_callback;
    spec.userdata = this;
    spec.channels = 1;

    // Open device for output with our format
    if ( SDL_OpenAudio(&spec, NULL) < 0 ) {
        printf("Couldn't open audio");
    }

    // Enable audio
    SDL_PauseAudio(0);

    Reset();
}

gbAPU::~gbAPU()
{
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void gbAPU::Reset() 
{
    channel1.Restart();
    channel2.Restart();
}

void gbAPU::UpdateTimers(uint16_t cycles) 
{
    // frame sequencer
    const uint32_t end_time = mSequenceCounter + cycles;
    while (mSequenceCounter != end_time)
    {
        mSequenceCounter++;
        // 256hz
        if ((mSequenceCounter % 16458) == 0)
        {
            channel1.ClockLength();
            channel2.ClockLength();
        }
        // 128hz
        if ((mSequenceCounter % 32916) == 0)
        {
            channel1.ClockSweep();
        }
        // 64hz
        if ((mSequenceCounter % 65832) == 0)
        {
            channel1.ClockEnvelope();
            channel2.ClockEnvelope();
        }
    }
}

void gbAPU::sdl2_callback(void* userdata, uint8_t *stream, int len)
{
    // User data is our gbApu class
    gbAPU* audio = reinterpret_cast<gbAPU*>(userdata);
    memset(stream, 0, len); // from heap so can be random data 
    const float Volume = audio->masterVolume * 0.50f;
    
    // buffer used for mixing channels.
    float_t buffer[samples];

    // Channel 1
    memset(&buffer, 0, sizeof(buffer));
    for(int i = 0; i < samples; i++)
    {
        const double dt = audio->timeElapsed + ((1.0f / 44100.0)*i);
        buffer[i] = (float)audio->channel1.GetAmplitude(dt) * Volume;
    }
    SDL_MixAudio(stream, reinterpret_cast<uint8_t*>(buffer), len, SDL_MIX_MAXVOLUME);

    // Channel 2
    memset(&buffer, 0, sizeof(buffer));
    for(int i = 0; i < samples; i++)
    {
        const double dt = audio->timeElapsed + ((1.0f / 44100.0)*i);
        buffer[i] = (float)audio->channel2.GetAmplitude(dt) * Volume;
    }
    SDL_MixAudio(stream, reinterpret_cast<uint8_t*>(buffer), len, SDL_MIX_MAXVOLUME);

    audio->timeElapsed += (1.0f / 44100.0) * samples;
}

void gbAPU::WriteByte(uint16_t addr, uint8_t data)
{
    if (addr == 0xFF11 || addr == 0xFF12 || addr == 0xFF13 || addr == 0xFF14)
    {
        channel1.WriteByte(addr, data);
    }
    else if (addr == 0xFF16 || addr == 0xFF17 || addr == 0xFF18 || addr == 0xFF19)
    {
        channel2.WriteByte(addr, data);
    }
}

