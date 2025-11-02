#include "AudioTrack.h"
#include <algorithm>
#include <cmath>
#include <cstring>

AudioTrack::AudioTrack()
    : sampleRate_(44100)
    , numChannels_(2)
    , bitsPerSample_(16)
    , volume_(1.0f)
    , pan_(0.0f)
    , muted_(false)
, soloed_(false)
{
}

AudioTrack::~AudioTrack()
{
}

bool AudioTrack::LoadFromFile(const std::string& filePath)
{
    filePath_ = filePath;
    
    // Extract name from file path
    size_t lastSlash = filePath.find_last_of("/\\");
  size_t lastDot = filePath.find_last_of('.');
    if (lastSlash != std::string::npos && lastDot != std::string::npos)
    {
        name_ = filePath.substr(lastSlash + 1, lastDot - lastSlash - 1);
    }
    else
    {
        name_ = filePath;
 }
    
    // Try to load as WAV
    if (LoadWAV(filePath))
    {
        return true;
    }
    
    // If WAV loading failed, generate test tone
    printf("WAV loading failed, generating test tone for: %s\n", filePath.c_str());
    
    // Generate 5 second test tone at 440Hz
    sampleRate_ = 44100;
    numChannels_ = 2;
    bitsPerSample_ = 16;
    
    uint32_t numSamples = sampleRate_ * 5; // 5 seconds
    samples_.resize(numSamples * numChannels_);
    
  const float frequency = 440.0f; // A4
    const float amplitude = 0.5f;
    
    for (uint32_t i = 0; i < numSamples; ++i)
    {
  float t = static_cast<float>(i) / sampleRate_;
      float value = amplitude * std::sin(2.0f * 3.14159265359f * frequency * t);
        
   // Apply envelope (fade in/out)
        float envelope = 1.0f;
        if (i < sampleRate_ / 10) // Fade in 0.1s
        {
      envelope = static_cast<float>(i) / (sampleRate_ / 10);
      }
        else if (i > numSamples - sampleRate_ / 10) // Fade out 0.1s
        {
            envelope = static_cast<float>(numSamples - i) / (sampleRate_ / 10);
        }
    
        float sample = value * envelope;
        samples_[i * numChannels_] = sample;
        samples_[i * numChannels_ + 1] = sample;
    }
    
    return true;
}

bool AudioTrack::LoadFromMemory(const float* samples, uint32_t numSamples, uint32_t sampleRate, uint16_t channels)
{
    sampleRate_ = sampleRate;
    numChannels_ = channels;
    bitsPerSample_ = 32; // Float
    
    samples_.assign(samples, samples + numSamples * channels);
    return true;
}

bool AudioTrack::LoadWAV(const std::string& filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open())
    {
     return false;
    }
    
    WAVHeader header;
    WAVFormat format;
    WAVData dataChunk;
  
    if (!ParseWAVHeader(file, header, format, dataChunk))
    {
        return false;
    }
    
    // Store format info
    sampleRate_ = format.sampleRate;
    numChannels_ = format.numChannels;
    bitsPerSample_ = format.bitsPerSample;
    
    // Read raw audio data
    std::vector<uint8_t> rawData(dataChunk.dataSize);
    file.read(reinterpret_cast<char*>(rawData.data()), dataChunk.dataSize);
    
    if (!file)
    {
        return false;
    }
    
    // Convert to float
    ConvertToFloat(rawData, bitsPerSample_);
 
    return true;
}

bool AudioTrack::ParseWAVHeader(std::ifstream& file, WAVHeader& header, WAVFormat& format, WAVData& dataChunk)
{
  // Read RIFF header
    file.read(reinterpret_cast<char*>(&header), sizeof(WAVHeader));
    if (std::strncmp(header.riff, "RIFF", 4) != 0 || std::strncmp(header.wave, "WAVE", 4) != 0)
    {
    return false;
    }
    
    // Read fmt chunk
 file.read(reinterpret_cast<char*>(&format), sizeof(WAVFormat));
    if (std::strncmp(format.fmt, "fmt ", 4) != 0)
    {
        return false;
    }
    
    // Skip any extra format bytes
    if (format.chunkSize > 16)
    {
        file.seekg(format.chunkSize - 16, std::ios::cur);
    }
    
    // Find data chunk
    char chunkID[4];
    uint32_t chunkSize;
    
    while (file.read(chunkID, 4))
    {
        file.read(reinterpret_cast<char*>(&chunkSize), sizeof(uint32_t));
        
        if (std::strncmp(chunkID, "data", 4) == 0)
        {
            std::memcpy(dataChunk.data, chunkID, 4);
      dataChunk.dataSize = chunkSize;
      return true;
        }
      
        // Skip this chunk
        file.seekg(chunkSize, std::ios::cur);
    }
    
    return false;
}

void AudioTrack::ConvertToFloat(const std::vector<uint8_t>& rawData, uint16_t bitsPerSample)
{
    uint32_t numSamples = static_cast<uint32_t>(rawData.size()) / (bitsPerSample / 8);
    samples_.resize(numSamples);
    
    if (bitsPerSample == 8)
    {
        // 8-bit: unsigned, 128 = center
 for (uint32_t i = 0; i < numSamples; ++i)
   {
            samples_[i] = (static_cast<float>(rawData[i]) - 128.0f) / 128.0f;
        }
    }
    else if (bitsPerSample == 16)
    {
    // 16-bit: signed
        const int16_t* data = reinterpret_cast<const int16_t*>(rawData.data());
        for (uint32_t i = 0; i < numSamples; ++i)
        {
            samples_[i] = static_cast<float>(data[i]) / 32768.0f;
      }
    }
    else if (bitsPerSample == 24)
    {
      // 24-bit: signed, stored in 3 bytes
      for (uint32_t i = 0; i < numSamples; ++i)
 {
          int32_t value = (rawData[i * 3 + 2] << 16) | (rawData[i * 3 + 1] << 8) | rawData[i * 3];
            
            // Sign extend
            if (value & 0x800000)
         {
        value |= 0xFF000000;
            }
        
      samples_[i] = static_cast<float>(value) / 8388608.0f;
   }
 }
    else if (bitsPerSample == 32)
    {
        // 32-bit: could be int or float
        const int32_t* dataInt = reinterpret_cast<const int32_t*>(rawData.data());
      const float* dataFloat = reinterpret_cast<const float*>(rawData.data());
        
        // Heuristic: if first sample is between -1 and 1, assume float
        if (dataFloat[0] >= -1.0f && dataFloat[0] <= 1.0f)
    {
  samples_.assign(dataFloat, dataFloat + numSamples);
}
else
  {
        for (uint32_t i = 0; i < numSamples; ++i)
          {
       samples_[i] = static_cast<float>(dataInt[i]) / 2147483648.0f;
      }
        }
    }
}

double AudioTrack::GetDurationSeconds() const
{
    if (samples_.empty() || sampleRate_ == 0)
    {
        return 0.0;
  }
    return static_cast<double>(GetNumSamples()) / sampleRate_;
}

float AudioTrack::GetSample(uint32_t index, uint16_t channel) const
{
    if (channel >= numChannels_ || samples_.empty())
    {
        return 0.0f;
    }
    
    uint32_t sampleIndex = index * numChannels_ + channel;
    if (sampleIndex >= samples_.size())
    {
  return 0.0f;
    }
    
    return samples_[sampleIndex];
}

void AudioTrack::SetVolume(float volume)
{
    volume_ = std::max(0.0f, std::min(1.0f, volume));
}

void AudioTrack::SetPan(float pan)
{
    pan_ = std::max(-1.0f, std::min(1.0f, pan));
}

void AudioTrack::GetPeakAmplitudes(std::vector<float>& minValues, std::vector<float>& maxValues, uint32_t numPoints) const
{
    minValues.clear();
    maxValues.clear();
    
    if (samples_.empty() || numPoints == 0)
    {
        return;
    }
    
    minValues.resize(numPoints, 0.0f);
    maxValues.resize(numPoints, 0.0f);
    
    uint32_t numSamples = GetNumSamples();
    uint32_t samplesPerPoint = std::max(1u, numSamples / numPoints);
    
    for (uint32_t i = 0; i < numPoints; ++i)
    {
        uint32_t startSample = i * samplesPerPoint;
        uint32_t endSample = std::min(startSample + samplesPerPoint, numSamples);
    
        float minVal = 0.0f;
float maxVal = 0.0f;
    
        for (uint32_t j = startSample; j < endSample; ++j)
        {
      // Average all channels for simplicity
            float avgSample = 0.0f;
       for (uint16_t ch = 0; ch < numChannels_; ++ch)
            {
   avgSample += GetSample(j, ch);
            }
            avgSample /= numChannels_;
  
            minVal = std::min(minVal, avgSample);
       maxVal = std::max(maxVal, avgSample);
        }
        
   minValues[i] = minVal;
     maxValues[i] = maxVal;
    }
}

float AudioTrack::GetPeakAmplitude() const
{
    float peak = 0.0f;
    for (float sample : samples_)
{
        peak = std::max(peak, std::abs(sample));
    }
    return peak;
}

float AudioTrack::GetRMSAmplitude() const
{
    if (samples_.empty())
    {
        return 0.0f;
    }
    
    float sumSquares = 0.0f;
    for (float sample : samples_)
    {
        sumSquares += sample * sample;
    }
    
    return std::sqrt(sumSquares / samples_.size());
}

void AudioTrack::Clear()
{
    samples_.clear();
    filePath_.clear();
}
