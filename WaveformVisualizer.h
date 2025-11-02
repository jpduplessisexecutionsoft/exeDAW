#pragma once

#include "AudioPlayer.h"
#include <vector>

// Waveform Visualizer - Displays audio waveforms
class WaveformVisualizer
{
public:
    struct WaveformPoint
    {
float x;
     float minAmplitude;
  float maxAmplitude;
    };

    WaveformVisualizer();
~WaveformVisualizer();

    // Set audio data to visualize
    void SetAudioData(const AudioData& audioData);
    void ClearAudioData();

    // Generate waveform points for rendering
    std::vector<WaveformPoint> GenerateWaveformPoints(uint32_t pixelWidth, uint32_t pixelHeight);

    // Waveform statistics
    float GetPeakAmplitude() const;
  float GetRMSAmplitude() const;
   
    // Display parameters
    void SetZoomLevel(float zoomFactor);
    float GetZoomLevel() const;

    void SetDisplayMode(bool stereo); // true for stereo view, false for mono
    bool GetDisplayMode() const;

private:
    AudioData audioData_;
    std::vector<WaveformPoint> waveformCache_;
    float zoomLevel_;
    bool stereoDisplay_;

    // Helper functions
    void CalculatePeakAmplitudes();
    float CalculateRMS(const std::vector<float>& samples) const;
};
