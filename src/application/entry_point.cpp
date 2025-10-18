// SPDX-License-Identifier: GPL-2.0-only
/*
#include <SDL3/SDL.h>
#include <imgui.h>

#include "App.hpp"
#include "Settings.hpp"

static const std::string s_SettingsFilePath = "./settings.toml";

namespace GeimBoi {
int main();
}

int GeimBoi::main()
{
	GeimBoi::Settings::load(s_SettingsFilePath);
	GeimBoi::App app;
	app.run();
	GeimBoi::Settings::save(s_SettingsFilePath);
	return 0;
}

*/
/*
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL3/SDL.h>
#undef main



#define PI 3.14159
#define PI2 (PI * 2.0)

float approxsin(float dt)
{
	float j = dt * 0.15915;
	j = j - (int)j;
	return 20.785 * j * (j - 0.5) * (j - 1.0f);
}

float harmoni_square_wave(
	uint32_t freq, float dutycycle, uint32_t harmonics, float time
)
{
	float a = .0;
	float b = .0;
	float p = dutycycle * PI2;

	for (uint32_t n = 1; n < harmonics; n++) {
		float c = n * freq * PI2 * time;
		a += -approxsin(c) / n;
		b += -approxsin(c - p * n) / n;
	}
	return (2.0 / PI) * (a - b);
}

int geimboi_main(int argc, char* argv[])
{
	if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_EVENTS)) {
		SDL_Log("SDL_Init failed: %s", SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_AudioSpec want;
	SDL_zero(want);

	want.format = SDL_AUDIO_F32; // 32-bit float samples
	want.channels = 1;			 // mono
	want.freq = 48000;			 // sample rate 48kHz

	auto generator = [](float dt) {
		return harmoni_square_wave(5, 0.5, 1, dt);
	};

	SDL_AudioStream* stream = SDL_OpenAudioDeviceStream(
		SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
		&want,
		NULL, // no capture
		NULL  // no extra device spec
	);
	if (!stream) {
		SDL_Log("SDL_OpenAudioDeviceStream failed: %s", SDL_GetError());
		SDL_Quit();
		return EXIT_FAILURE;
	}

	// Start playback
	if (SDL_ResumeAudioStreamDevice(stream) < 0) {
		SDL_Log("SDL_ResumeAudioStreamDevice failed: %s", SDL_GetError());
		SDL_DestroyAudioStream(stream);
		SDL_Quit();
		return EXIT_FAILURE;
	}

	const double two_pi = 2.0 * M_PI;
	const double frequency = 440.0; // A4-tone 440 Hz
	const double sample_rate = (double)want.freq;
	double phase = 0.0;
	const double phase_increment = two_pi * frequency / sample_rate;

	bool running = true;
	SDL_Event event;

	while (running) {
		// Handle events (user may close window or press a key, etc.)
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				running = false;
			}
		}

		// Check how many bytes are queued in the stream
		int avail = SDL_GetAudioStreamAvailable(stream);
		// If queued data is less than about half a second, generate more
		if (avail < (int)(sample_rate * sizeof(float) * 0.5)) {
			const int samples_to_generate = 1024;
			float buffer[samples_to_generate];
			for (int i = 0; i < samples_to_generate; ++i) {
				buffer[i] = (float)generator(phase);
				phase += phase_increment;
				if (phase >= two_pi) {
					phase -= two_pi;
				}
			}
			// push data (in bytes)
			SDL_PutAudioStreamData(
				stream, buffer, samples_to_generate * sizeof(float)
			);
		}

		// small delay so we don't hog CPU
		SDL_Delay(10);
	}

	SDL_DestroyAudioStream(stream);
	SDL_Quit();
	return EXIT_SUCCESS;
}
*/

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL3/SDL.h>
#define M_PI 3.1415f
static const double TWO_PI = 2.0 * M_PI;

/* Structure to hold our sine‐wave generator state */
typedef struct {
	/*
	double phase;
	double phaseIncrement;
	int sampleRate;
	double frequency;
	*/
	float dt;
} SineState;

float approxsin(float dt)
{
	float j = dt * 0.15915;
	j = j - (int)j;
	return 20.785 * j * (j - 0.5) * (j - 1.0f);
}

/* This callback will be called by SDL when the audio device/stream needs more
 * data */
static void SDLCALL AudioCallback(
	void* userdata,
	SDL_AudioStream* astream,
	int additional_bytes,
	int total_bytes
)
{
	SineState* state = (SineState*)userdata;

	int bytes_needed = additional_bytes;
	if (bytes_needed <= 0) {
		return; /* nothing needed right now */
	}

	/* We're going to use float samples (32-bit float) as our input format */
	int bytes_per_sample = sizeof(float) * 1; /* mono */
	int count_samples = bytes_needed / bytes_per_sample;
	if (count_samples < 1) {
		return;
	}

	float* buffer = (float*)SDL_malloc(count_samples * sizeof(float));
	if (!buffer) {
		SDL_Log(
			"Failed to allocate audio buffer of %d samples\n", count_samples
		);
		return;
	}

	for (int i = 0; i < count_samples; ++i) {
		buffer[i] = (float)approxsin(state->dt);
		state->dt += (1.0 / 44100) * additional_bytes;
	}

	/* Push generated audio data into the stream */
	SDL_PutAudioStreamData(astream, buffer, count_samples * sizeof(float));

	SDL_free(buffer);
}

int geimboi_main(int argc, char* argv[])
{
	if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_EVENTS)) {
		SDL_Log("SDL_Init failed: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	/* Setup our desired audio spec */
	SDL_AudioSpec want;
	SDL_zero(want);
	want.format = SDL_AUDIO_F32; /* 32-bit float samples */
	want.channels = 1;			 /* mono */
	want.freq = 44100;			 /* sample rate 48 kHz */

	SineState state;
	state.dt = 0;

	/* Open the audio device + stream with callback */
	SDL_AudioStream* astream = SDL_OpenAudioDeviceStream(
		SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &want, AudioCallback, &state
	);
	if (!astream) {
		SDL_Log("SDL_OpenAudioDeviceStream failed: %s\n", SDL_GetError());
		SDL_Quit();
		return EXIT_FAILURE;
	}

	/* Start playback */
	if (SDL_ResumeAudioStreamDevice(astream) < 0) {
		SDL_Log("SDL_ResumeAudioStreamDevice failed: %s\n", SDL_GetError());
		SDL_DestroyAudioStream(astream);
		SDL_Quit();
		return EXIT_FAILURE;
	}

	/* Our manual main loop: poll events until quit */
	bool running = true;
	SDL_Event event;
	while (running) {
		while (SDL_WaitEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				running = false;
			}
		}

		// SDL_Delay(10); /* reduce CPU usage */
	}

	SDL_DestroyAudioStream(astream);
	SDL_Quit();
	return EXIT_SUCCESS;
}
