#pragma once

#include <string>
#include <vector>
#include <memory>
#include <cstdint>

// Forward declarations
class SequencerChannel;

// Audio data structure for waveform representation
struct AudioData
{
    std::vector<float> samples;
    uint32_t sampleRate;
  uint16_t channels;
  uint16_t bitDepth;
    std::string filePath;

    AudioData() : sampleRate(44100), channels(2), bitDepth(16) {}
};

// Audio Player - Can be used on multiple channels
class AudioPlayer
{
public:
    enum PlaybackState
    {
        STOPPED,
PLAYING,
 PAUSED,
    RECORDING
};

    AudioPlayer();
    ~AudioPlayer();

    // Load audio file
    bool LoadAudioFile(const std::string& filePath);

  // Playback control
    void Play();
    void Stop();
    void Pause();
    void Resume();

    // Playback position
    void SetPosition(uint32_t sampleIndex);
    uint32_t GetPosition() const;
    uint32_t GetDuration() const;

    // Audio data access
    const AudioData& GetAudioData() const;
    PlaybackState GetState() const;

    // Volume control
    void SetVolume(float volume);
    float GetVolume() const;

private:
    AudioData audioData_;
    PlaybackState state_;
    uint32_t currentPosition_;
    float volume_;

    // Helper function to parse audio file (simplified)
    bool ParseAudioFile(const std::string& filePath);
};
