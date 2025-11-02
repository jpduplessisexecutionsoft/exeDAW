#pragma once

#include <cstdint>
#include <string>

// Transport Controls - Cubase-style transport control system
class TransportControl
{
public:
    enum TransportState
    {
        STOPPED = 0,
     PLAYING = 1,
        PAUSED = 2,
   RECORDING = 3
    };

    TransportControl();
    ~TransportControl();

    // Transport state control
    void Play();
    void Stop();
    void Pause();
    void Resume();
    void Record();

    // Transport state queries
    TransportState GetState() const;
    bool IsPlaying() const;
 bool IsPaused() const;
    bool IsRecording() const;
  bool IsStopped() const;

    // Playhead position (in samples)
 void SetPlayheadPosition(uint32_t sampleIndex);
    uint32_t GetPlayheadPosition() const;

    // Tempo control (BPM)
    void SetTempo(float bpm);
    float GetTempo() const;

    // Time signature
    void SetTimeSignature(uint32_t numerator, uint32_t denominator);
    void GetTimeSignature(uint32_t& numerator, uint32_t& denominator) const;

    // Loop control
    void SetLoopEnabled(bool enabled);
    bool IsLoopEnabled() const;

    void SetLoopRange(uint32_t startSample, uint32_t endSample);
    void GetLoopRange(uint32_t& startSample, uint32_t& endSample) const;

    // Sample rate
    void SetSampleRate(uint32_t sampleRate);
    uint32_t GetSampleRate() const;

    // Callbacks
    void SetPlaybackCallback(void (*callback)(TransportState state));

private:
    TransportState currentState_;
    uint32_t playheadPosition_;
    float tempo_;
    uint32_t timeSignatureNumerator_;
    uint32_t timeSignatureDenominator_;
    bool loopEnabled_;
    uint32_t loopStart_;
    uint32_t loopEnd_;
    uint32_t sampleRate_;
    void (*playbackCallback_)(TransportState state);
};
