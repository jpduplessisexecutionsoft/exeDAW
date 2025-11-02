#include "DAWApplication.h"

DAWApplication::DAWApplication()
{
    sequencer_ = std::make_shared<SequencerEngine>();
    transport_ = std::make_shared<TransportControl>();
    visualizer_ = std::make_shared<WaveformVisualizer>();
}

DAWApplication::~DAWApplication()
{
}

bool DAWApplication::Initialize()
{
    // Initialize transport control
   transport_->SetSampleRate(44100);
    transport_->SetTempo(120.0f);
  transport_->SetTimeSignature(4, 4);
    transport_->SetLoopEnabled(false);
    
    return true;
}

std::shared_ptr<SequencerEngine> DAWApplication::GetSequencer()
{
    return sequencer_;
}

std::shared_ptr<TransportControl> DAWApplication::GetTransport()
{
    return transport_;
}

std::shared_ptr<WaveformVisualizer> DAWApplication::GetVisualizer()
{
    return visualizer_;
}

std::shared_ptr<SequencerChannel> DAWApplication::AddTrack(const std::string& trackName)
{
    return sequencer_->CreateChannel(trackName, SequencerChannel::AUDIO);
}

bool DAWApplication::LoadAudioFile(uint32_t channelId, const std::string& filePath)
{
    return sequencer_->LoadAudioToChannel(channelId, filePath);
}

void DAWApplication::PlayAll()
{
    transport_->Play();
    
    // Play all channels that have audio
    for (const auto& channel : sequencer_->GetAllChannels())
    {
if (channel->HasAudioPlayer())
  {
   channel->GetAudioPlayer()->Play();
        }
    }
}

void DAWApplication::StopAll()
{
    transport_->Stop();
    
    // Stop all channels
    for (const auto& channel : sequencer_->GetAllChannels())
    {
   if (channel->HasAudioPlayer())
        {
            channel->GetAudioPlayer()->Stop();
        }
    }
}

void DAWApplication::PauseAll()
{
    transport_->Pause();
    
    // Pause all channels
    for (const auto& channel : sequencer_->GetAllChannels())
    {
        if (channel->HasAudioPlayer())
   {
     channel->GetAudioPlayer()->Pause();
      }
    }
}

uint32_t DAWApplication::GetTrackCount() const
{
    return sequencer_->GetChannelCount();
}
