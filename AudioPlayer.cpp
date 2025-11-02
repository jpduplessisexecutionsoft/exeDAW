#include "AudioPlayer.h"
#include <cstring>
#include <algorithm>
#include <fstream>

AudioPlayer::AudioPlayer() 
    : state_(STOPPED), currentPosition_(0), volume_(1.0f)
{
}

AudioPlayer::~AudioPlayer()
{
}

bool AudioPlayer::LoadAudioFile(const std::string& filePath)
{
    return ParseAudioFile(filePath);
}

void AudioPlayer::Play()
{
    state_ = PLAYING;
    if (currentPosition_ >= GetDuration())
    {
        currentPosition_ = 0; // Restart from beginning if at the end
    }
}

void AudioPlayer::Stop()
{
    state_ = STOPPED;
    currentPosition_ = 0;
}

void AudioPlayer::Pause()
{
    if (state_ == PLAYING)
    {
        state_ = PAUSED;
    }
}

void AudioPlayer::Resume()
{
    if (state_ == PAUSED)
    {
        state_ = PLAYING;
    }
}

void AudioPlayer::SetPosition(uint32_t sampleIndex)
{
    if (sampleIndex <= GetDuration())
    {
        currentPosition_ = sampleIndex;
    }
}

uint32_t AudioPlayer::GetPosition() const
{
    return currentPosition_;
}

uint32_t AudioPlayer::GetDuration() const
{
    return static_cast<uint32_t>(audioData_.samples.size() / audioData_.channels);
}

const AudioData& AudioPlayer::GetAudioData() const
{
    return audioData_;
}

AudioPlayer::PlaybackState AudioPlayer::GetState() const
{
    return state_;
}

void AudioPlayer::SetVolume(float volume)
{
    volume_ = std::max(0.0f, std::min(1.0f, volume));
}

float AudioPlayer::GetVolume() const
{
    return volume_;
}

bool AudioPlayer::ParseAudioFile(const std::string& filePath)
{
    // This is a simplified implementation
    // In a real application, you would use libraries like JUCE, PortAudio, or FMOD
    
    audioData_.filePath = filePath;
    audioData_.sampleRate = 44100;
    audioData_.channels = 2;
  audioData_.bitDepth = 16;

    // Simulate loading audio data (create dummy waveform)
    // In production, parse actual audio files (WAV, MP3, FLAC, etc.)
    uint32_t durationSamples = audioData_.sampleRate * 10; // 10 seconds
    audioData_.samples.resize(durationSamples * audioData_.channels);

    // Generate a simple sine wave for demonstration
    for (uint32_t i = 0; i < durationSamples; ++i)
    {
        float frequency = 440.0f; // A4 note
        float amplitude = 0.8f;
      float sample = amplitude * std::sin(2.0f * 3.14159f * frequency * i / audioData_.sampleRate);
        
        // Fill both channels with the same signal
        audioData_.samples[i * 2] = sample;
 audioData_.samples[i * 2 + 1] = sample;
 }

    return true;
}
