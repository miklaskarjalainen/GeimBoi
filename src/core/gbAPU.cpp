#include <SDL2/SDL.h>

#include "gbAPU.hpp"
#include "gbGameBoy.hpp"

using namespace GeimBoi;

static SDL_AudioSpec spec;

gbAPU::gbAPU(gbGameBoy* emu)
    : mGameBoy(emu), channel1(emu), channel2(emu)
{
    SDL_InitSubSystem(SDL_INIT_AUDIO);

    // Init format
    spec.freq = 44100;
    spec.format = AUDIO_F32SYS;
    spec.samples = 128;
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
}

void gbAPU::UpdateTimers(uint16_t cycles) 
{
    channel1.UpdateTimers(cycles);
    channel2.UpdateTimers(cycles);
}

void gbAPU::sdl2_callback(void* userdata, uint8_t *stream, int len)
{
    // User data is our gbApu class
    gbAPU* audio = reinterpret_cast<gbAPU*>(userdata);
    memset(stream, 0, len); // from heap so can be random data 
    const float Volume = audio->masterVolume * 0.15f;
    
    // format is F32 instead of argument's U8
    float_t* snd = reinterpret_cast<float_t*>(stream);
    len /= sizeof(*snd);

    // buffer used for mixing channels.
    float_t buffer[len];

    // Channel 1
    memset(&buffer, 0, sizeof(buffer));
    for(int i = 0; i < len; i++)
    {
        const double dt = audio->timeElapsed + ((1.0f / 44100.0)*i);
        buffer[i] = (float)audio->channel1.GetAmplitude(dt) * Volume;
    }
    SDL_MixAudio(stream, reinterpret_cast<uint8_t*>(buffer), len*sizeof(*snd), SDL_MIX_MAXVOLUME);

    // Channel 2
    memset(&buffer, 0, sizeof(buffer));
    for(int i = 0; i < len; i++)
    {
        const double dt = audio->timeElapsed + ((1.0f / 44100.0)*i);
        buffer[i] = (float)audio->channel2.GetAmplitude(dt) * Volume;
    }
    SDL_MixAudio(stream, reinterpret_cast<uint8_t*>(buffer), len*sizeof(*snd), SDL_MIX_MAXVOLUME);


    audio->timeElapsed += (1.0f / 44100.0) * len;
}

void gbAPU::WriteByte(uint16_t addr, uint8_t data)
{
    if (addr == 0xFF11 || addr == 0xFF13 || addr == 0xFF14)
    {
        channel1.WriteByte(addr, data);
    }
    else if (addr == 0xFF16 || addr == 0xFF18 || addr == 0xFF19)
    {
        channel2.WriteByte(addr, data);
    }
}
