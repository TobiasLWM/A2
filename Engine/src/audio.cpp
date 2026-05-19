#include <audio.h>

#include <vector>
#include <algorithm>

// Mixer
MIX_Mixer *mixer;

// Track temporary tracks (for cleanup)
std::vector<MIX_Track*> playingOnce;

// Initialise Audio System
bool initAudioSystem() {
    // Initialise SDL_Mixer
    if (!MIX_Init()) {
        SDL_Log("Couldn't initialize SDL_mixer: %s", SDL_GetError());
        return false;
    }

    // Create a Mixer
    mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);

    // Return Success
    return true;
}

// Update Audio System
void updateAudioSystem() {
    // Remove any Temporary tracks that have completed (and clean them up).
    playingOnce.erase(std::remove_if(playingOnce.begin(), playingOnce.end(), [](MIX_Track *track) {
        if(MIX_TrackPlaying(track) == false) {
            MIX_DestroyTrack(track);
            return true;
        }
        return false;
    }), playingOnce.end());
}

// Close AudioSystem
void closeAudioSystem() {
    // Close Audio System
    for(MIX_Track *track : playingOnce) {
        // Destroy Track
        MIX_DestroyTrack(track);
    }

    // Destroy Mixer
    MIX_DestroyMixer(mixer);
    mixer = nullptr;

    // Quit SDL3_MIX
    MIX_Quit();
}

// Set Gain
void setGain(float gain) {
    // Set gain for mixer
    MIX_SetMixerGain(mixer, gain);
}

// Load Audio clip from file
AudioClip loadAudioClip(const char *path) {
    // Load audio data from file.
    MIX_Audio *audio = MIX_LoadAudio(mixer, path, true);

    // Check result
    if (!audio) {
        SDL_Log("Couldn't load audio from %s: %s", path, SDL_GetError());
    }

    // Create track
    MIX_Track *track = MIX_CreateTrack(mixer);
    if (!track) {
        SDL_Log("Couldn't create track: %s", SDL_GetError());
    }

    // Set audio data
    MIX_SetTrackAudio(track, audio);

    // Create audio clip
    AudioClip clip = {track, audio};

    // Return result
    return clip;
}

// Play Audio clip
void playAudio(AudioClip clip, float gain, bool repeat) {
    // Set gain
    MIX_SetTrackGain(clip.track, gain);

    // Create properties group
    SDL_PropertiesID properties = SDL_CreateProperties();
    if(repeat) {
        // Set -1 (infinite) loops
        SDL_SetNumberProperty(properties, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
    }

    // Play track (restart if already playing)
    MIX_PlayTrack(clip.track, properties);
}

// Play Audio clip once (creates a copy of the track and plays once to completion)
void playOnce(AudioClip clip, float gain) {
    // Create a new track
    MIX_Track *track = MIX_CreateTrack(mixer);

    // Check success
    if (!track) {
        SDL_Log("Couldn't create track: %s", SDL_GetError());
        return;
    }

    // Set Audio data for track
    MIX_SetTrackAudio(track, clip.audio);
    
    // Set gain
    MIX_SetTrackGain(track, gain);

    // Play track
    MIX_PlayTrack(track, 0);

    // Add to list of play once track
    playingOnce.push_back(track);
}

// Stop Audio clip playing
void stopAudio(AudioClip clip) {
    // Stop track
    MIX_StopTrack(clip.track, 0);
}

// Pause Audio Clip playing
void pauseAudio(AudioClip clip) {
    // Pause track
    MIX_PauseTrack(clip.track);
}

// Resume Audio Clip playing
void resumeAudio(AudioClip clip, float gain) {
    // Set Gain
    MIX_SetTrackGain(clip.track, gain);

    // Resume track
    MIX_ResumeTrack(clip.track);
}