#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <memory>
#include <fstream>

// WAV file header structures
#pragma pack(push, 1)
struct WAVHeader
{
    char riff[4]; // "RIFF"
    uint32_t fileSize;      // File size - 8
    char wave[4];           // "WAVE"
};

struct WAVFormat
{
    char fmt[4];     // "fmt "
    uint32_t chunkSize; // 16 for PCM
    uint16_t audioFormat;   // 1 for PCM
    uint16_t numChannels;   // 1 = mono, 2 = stereo
    uint32_t sampleRate;    // 44100, 48000, etc.
    uint32_t byteRate;      // sampleRate * numChannels * bitsPerSample/8
    uint16_t blockAlign;    // numChannels * bitsPerSample/8
    uint16_t bitsPerSample; // 8, 16, 24, 32
};

struct WAVData
{
    char data[4];           // "data"
    uint32_t dataSize;      // Size of data section
};
#pragma pack(pop)

// Audio track class - represents a single audio track with WAV data
class AudioTrack
{
public:
    AudioTrack();
    ~AudioTrack();

    // Load WAV file
    bool LoadFromFile(const std::string& filePath);
  bool LoadFromMemory(const float* samples, uint32_t numSamples, uint32_t sampleRate, uint16_t channels);
    
    // Audio properties
    uint32_t GetSampleRate() const { return sampleRate_; }
    uint16_t GetNumChannels() const { return numChannels_; }
    uint16_t GetBitsPerSample() const { return bitsPerSample_; }
    uint32_t GetNumSamples() const { return static_cast<uint32_t>(samples_.size() / numChannels_); }
    double GetDurationSeconds() const;
    
    // Sample access
  const std::vector<float>& GetSamples() const { return samples_; }
    float GetSample(uint32_t index, uint16_t channel) const;
    
    // Track properties
    const std::string& GetName() const { return name_; }
    void SetName(const std::string& name) { name_ = name; }
    
    const std::string& GetFilePath() const { return filePath_; }
    
    float GetVolume() const { return volume_; }
    void SetVolume(float volume);
    
    float GetPan() const { return pan_; }
 void SetPan(float pan); // -1.0 (left) to 1.0 (right)
    
    bool IsMuted() const { return muted_; }
    void SetMuted(bool muted) { muted_ = muted; }
    
    bool IsSoloed() const { return soloed_; }
    void SetSoloed(bool soloed) { soloed_ = soloed; }
  
    // Waveform analysis
    void GetPeakAmplitudes(std::vector<float>& minValues, std::vector<float>& maxValues, uint32_t numPoints) const;
    float GetPeakAmplitude() const;
    float GetRMSAmplitude() const;
    
    // Clear audio data
    void Clear();
    bool IsEmpty() const { return samples_.empty(); }

private:
    std::string name_;
    std::string filePath_;
    
    // Audio data
    std::vector<float> samples_; // Normalized float samples [-1.0, 1.0]
    uint32_t sampleRate_;
    uint16_t numChannels_;
    uint16_t bitsPerSample_;
    
    // Track properties
    float volume_;  // 0.0 to 1.0
    float pan_;     // -1.0 to 1.0
    bool muted_;
    bool soloed_;
    
// Helper methods
    bool LoadWAV(const std::string& filePath);
    bool ParseWAVHeader(std::ifstream& file, WAVHeader& header, WAVFormat& format, WAVData& dataChunk);
    void ConvertToFloat(const std::vector<uint8_t>& rawData, uint16_t bitsPerSample);
};
