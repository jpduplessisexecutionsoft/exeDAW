#include "WaveformVisualizer.h"
#include <algorithm>
#include <cmath>

WaveformVisualizer::WaveformVisualizer()
    : zoomLevel_(1.0f), stereoDisplay_(true)
{
}

WaveformVisualizer::~WaveformVisualizer()
{
}

void WaveformVisualizer::SetAudioData(const AudioData& audioData)
{
    audioData_ = audioData;
    CalculatePeakAmplitudes();
}

void WaveformVisualizer::ClearAudioData()
{
    audioData_.samples.clear();
    waveformCache_.clear();
}

std::vector<WaveformVisualizer::WaveformPoint> WaveformVisualizer::GenerateWaveformPoints(
    uint32_t pixelWidth, uint32_t pixelHeight)
{
    waveformCache_.clear();

    if (audioData_.samples.empty() || pixelWidth == 0)
    {
        return waveformCache_;
    }

    uint32_t samplesPerPixel = static_cast<uint32_t>(
        audioData_.samples.size() / (pixelWidth * zoomLevel_));
    
    if (samplesPerPixel == 0)
    {
        samplesPerPixel = 1;
    }

    for (uint32_t x = 0; x < pixelWidth; ++x)
    {
        uint32_t sampleStart = x * samplesPerPixel;
        uint32_t sampleEnd = std::min(sampleStart + samplesPerPixel,
      static_cast<uint32_t>(audioData_.samples.size()));

        float minAmplitude = 0.0f;
     float maxAmplitude = 0.0f;

        for (uint32_t i = sampleStart; i < sampleEnd; ++i)
        {
     float sample = audioData_.samples[i];
        minAmplitude = std::min(minAmplitude, sample);
    maxAmplitude = std::max(maxAmplitude, sample);
        }

     WaveformPoint point;
        point.x = static_cast<float>(x);
        point.minAmplitude = minAmplitude;
        point.maxAmplitude = maxAmplitude;
      waveformCache_.push_back(point);
    }

    return waveformCache_;
}

float WaveformVisualizer::GetPeakAmplitude() const
{
    float peak = 0.0f;
    for (float sample : audioData_.samples)
    {
        peak = std::max(peak, std::abs(sample));
}
    return peak;
}

float WaveformVisualizer::GetRMSAmplitude() const
{
    return CalculateRMS(audioData_.samples);
}

void WaveformVisualizer::SetZoomLevel(float zoomFactor)
{
    if (zoomFactor > 0.0f)
    {
        zoomLevel_ = zoomFactor;
    }
}

float WaveformVisualizer::GetZoomLevel() const
{
    return zoomLevel_;
}

void WaveformVisualizer::SetDisplayMode(bool stereo)
{
    stereoDisplay_ = stereo;
}

bool WaveformVisualizer::GetDisplayMode() const
{
    return stereoDisplay_;
}

void WaveformVisualizer::CalculatePeakAmplitudes()
{
    // This method could cache peak values per pixel range
    // for faster rendering in future calls
}

float WaveformVisualizer::CalculateRMS(const std::vector<float>& samples) const
{
    if (samples.empty())
    {
        return 0.0f;
    }

    float sumSquares = 0.0f;
    for (float sample : samples)
  {
        sumSquares += sample * sample;
    }

    return std::sqrt(sumSquares / samples.size());
}
