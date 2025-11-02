#pragma once

#include "SequencerEngine.h"
#include "TransportControl.h"
#include "WaveformVisualizer.h"
#include <memory>

// DAW Application class - Main coordinator for the audio workstation
class DAWApplication
{
public:
    DAWApplication();
    ~DAWApplication();

    // Initialize the DAW
    bool Initialize();

    // Get core components
    std::shared_ptr<SequencerEngine> GetSequencer();
    std::shared_ptr<TransportControl> GetTransport();
    std::shared_ptr<WaveformVisualizer> GetVisualizer();

    // Convenience methods for common DAW operations
    std::shared_ptr<SequencerChannel> AddTrack(const std::string& trackName);
    bool LoadAudioFile(uint32_t channelId, const std::string& filePath);
    
    // Play/Stop operations affecting all channels
    void PlayAll();
    void StopAll();
    void PauseAll();

    // Get channel count
    uint32_t GetTrackCount() const;

private:
    std::shared_ptr<SequencerEngine> sequencer_;
    std::shared_ptr<TransportControl> transport_;
    std::shared_ptr<WaveformVisualizer> visualizer_;
};
