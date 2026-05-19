#ifndef Engine_audio_h_
#define Engine_audio_h_

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

// Audio Clip
class AudioClip {
    public:
    MIX_Track *track;
    MIX_Audio *audio;
};

// Initialise Audio System
bool initAudioSystem();

// Update Audio System
void updateAudioSystem();

// Close Audio System
void closeAudioSystem();

// Set Gain
void setGain(float gain);

// Load Audio clip
AudioClip loadAudioClip(const char *path);

// Play Audio clip
void playAudio(AudioClip clip, float gain=1.0f, bool repeat=false);

// Play Audio clip once (as a separate copy)
void playOnce(AudioClip clip, float gain=1.0f);

// Stop Audio clip
void stopAudio(AudioClip clip);

// Pause Audio clip
void pauseAudio(AudioClip clip);

// Resume Audio clip
void resumeAudio(AudioClip clip, float gain=1.0f);

#endif