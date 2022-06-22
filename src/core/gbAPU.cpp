#include <SDL2/SDL.h>

#include "gbAPU.hpp"
#include "gbGameBoy.hpp"

using namespace GeimBoi;

static SDL_AudioSpec spec;

gbAPU::gbAPU(gbGameBoy* emu)
    : mGameBoy(emu), channel1(emu)
{
    SDL_InitSubSystem(SDL_INIT_AUDIO);

    // Init format
    spec.freq = 44100;
    spec.format = AUDIO_S16SYS;
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
}

void gbAPU::sdl2_callback(void* userdata, uint8_t *stream, int len)
{
    // User data is our gbApu class
    gbAPU* audio = reinterpret_cast<gbAPU*>(userdata);
    memset(stream, 0, len); // from heap so can be random data 
    
    // format is S16 instead of argument's U8
    int16_t* snd = reinterpret_cast<int16_t*>(stream);
    len /= sizeof(*snd);

    // Channel 1
    int16_t buffer[len];
    // prob not needed
    memset(&buffer, 0, sizeof(buffer));
    for(int i = 0; i < len; i++)
    {
        const double dt = audio->timeElapsed + ((1.0f / 44100.0)*i);
        buffer[i] = (5000 * audio->channel1.GetAmplitude(dt));
    }
    SDL_MixAudio(stream, reinterpret_cast<uint8_t*>(buffer), len*sizeof(*snd), SDL_MIX_MAXVOLUME);
    audio->timeElapsed += (1.0f / 44100.0) * len;
}
