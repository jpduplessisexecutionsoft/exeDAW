#include "TransportControl.h"

TransportControl::TransportControl()
    : currentState_(STOPPED), playheadPosition_(0), tempo_(120.0f),
   timeSignatureNumerator_(4), timeSignatureDenominator_(4),
    loopEnabled_(false), loopStart_(0), loopEnd_(0), sampleRate_(44100),
    playbackCallback_(nullptr)
{
}

TransportControl::~TransportControl()
{
}

void TransportControl::Play()
{
    currentState_ = PLAYING;
    if (playbackCallback_)
    {
        playbackCallback_(PLAYING);
    }
}

void TransportControl::Stop()
{
    currentState_ = STOPPED;
    playheadPosition_ = 0;
    if (playbackCallback_)
 {
        playbackCallback_(STOPPED);
    }
}

void TransportControl::Pause()
{
    if (currentState_ == PLAYING)
    {
        currentState_ = PAUSED;
        if (playbackCallback_)
        {
            playbackCallback_(PAUSED);
      }
    }
}

void TransportControl::Resume()
{
    if (currentState_ == PAUSED)
    {
        currentState_ = PLAYING;
  if (playbackCallback_)
 {
          playbackCallback_(PLAYING);
        }
    }
}

void TransportControl::Record()
{
    currentState_ = RECORDING;
    if (playbackCallback_)
    {
        playbackCallback_(RECORDING);
    }
}

TransportControl::TransportState TransportControl::GetState() const
{
    return currentState_;
}

bool TransportControl::IsPlaying() const
{
    return currentState_ == PLAYING;
}

bool TransportControl::IsPaused() const
{
    return currentState_ == PAUSED;
}

bool TransportControl::IsRecording() const
{
    return currentState_ == RECORDING;
}

bool TransportControl::IsStopped() const
{
    return currentState_ == STOPPED;
}

void TransportControl::SetPlayheadPosition(uint32_t sampleIndex)
{
    playheadPosition_ = sampleIndex;
}

uint32_t TransportControl::GetPlayheadPosition() const
{
    return playheadPosition_;
}

void TransportControl::SetTempo(float bpm)
{
    if (bpm > 0.0f)
    {
     tempo_ = bpm;
    }
}

float TransportControl::GetTempo() const
{
    return tempo_;
}

void TransportControl::SetTimeSignature(uint32_t numerator, uint32_t denominator)
{
 if (numerator > 0 && denominator > 0)
    {
        timeSignatureNumerator_ = numerator;
        timeSignatureDenominator_ = denominator;
    }
}

void TransportControl::GetTimeSignature(uint32_t& numerator, uint32_t& denominator) const
{
    numerator = timeSignatureNumerator_;
    denominator = timeSignatureDenominator_;
}

void TransportControl::SetLoopEnabled(bool enabled)
{
    loopEnabled_ = enabled;
}

bool TransportControl::IsLoopEnabled() const
{
    return loopEnabled_;
}

void TransportControl::SetLoopRange(uint32_t startSample, uint32_t endSample)
{
    if (startSample < endSample)
    {
        loopStart_ = startSample;
        loopEnd_ = endSample;
    }
}

void TransportControl::GetLoopRange(uint32_t& startSample, uint32_t& endSample) const
{
startSample = loopStart_;
  endSample = loopEnd_;
}

void TransportControl::SetSampleRate(uint32_t sampleRate)
{
  if (sampleRate > 0)
    {
        sampleRate_ = sampleRate;
}
}

uint32_t TransportControl::GetSampleRate() const
{
    return sampleRate_;
}

void TransportControl::SetPlaybackCallback(void (*callback)(TransportState state))
{
    playbackCallback_ = callback;
}
