#include "SequencerChannel.h"

SequencerChannel::SequencerChannel(uint32_t channelId, const std::string& name, ChannelType type)
    : channelId_(channelId), channelName_(name), channelType_(type),
   volume_(1.0f), pan_(0.0f), muted_(false), solo_(false)
{
}

SequencerChannel::~SequencerChannel()
{
    audioPlayer_.reset();
}

uint32_t SequencerChannel::GetChannelId() const
{
    return channelId_;
}

std::string SequencerChannel::GetChannelName() const
{
    return channelName_;
}

void SequencerChannel::SetChannelName(const std::string& name)
{
    channelName_ = name;
}

SequencerChannel::ChannelType SequencerChannel::GetChannelType() const
{
    return channelType_;
}

bool SequencerChannel::AssignAudioPlayer(std::shared_ptr<AudioPlayer> player)
{
    if (player)
  {
        audioPlayer_ = player;
 return true;
    }
    return false;
}

std::shared_ptr<AudioPlayer> SequencerChannel::GetAudioPlayer() const
{
    return audioPlayer_;
}

bool SequencerChannel::HasAudioPlayer() const
{
    return audioPlayer_ != nullptr;
}

void SequencerChannel::RemoveAudioPlayer()
{
    audioPlayer_.reset();
}

void SequencerChannel::SetVolume(float volume)
{
    volume_ = volume > 1.0f ? 1.0f : (volume < 0.0f ? 0.0f : volume);
}

float SequencerChannel::GetVolume() const
{
    return volume_;
}

void SequencerChannel::SetPan(float pan)
{
    pan_ = pan > 1.0f ? 1.0f : (pan < -1.0f ? -1.0f : pan);
}

float SequencerChannel::GetPan() const
{
    return pan_;
}

void SequencerChannel::SetMuted(bool muted)
{
    muted_ = muted;
}

bool SequencerChannel::IsMuted() const
{
    return muted_;
}

void SequencerChannel::SetSolo(bool solo)
{
    solo_ = solo;
}

bool SequencerChannel::IsSolo() const
{
    return solo_;
}
