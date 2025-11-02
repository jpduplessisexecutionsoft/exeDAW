#include "SequencerEngine.h"
#include <algorithm>

SequencerEngine::SequencerEngine() : nextChannelId_(1)
{
}

SequencerEngine::~SequencerEngine()
{
    DeleteAllChannels();
}

std::shared_ptr<SequencerChannel> SequencerEngine::CreateChannel(const std::string& name, 
    SequencerChannel::ChannelType type)
{
    auto channel = std::make_shared<SequencerChannel>(nextChannelId_, name, type);
    channels_.push_back(channel);
    channelMap_[nextChannelId_] = channel;
    nextChannelId_++;
  return channel;
}

std::shared_ptr<SequencerChannel> SequencerEngine::GetChannel(uint32_t channelId) const
{
    auto it = channelMap_.find(channelId);
    if (it != channelMap_.end())
    {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<SequencerChannel> SequencerEngine::GetChannelByName(const std::string& name) const
{
    for (const auto& channel : channels_)
    {
 if (channel->GetChannelName() == name)
        {
        return channel;
}
    }
    return nullptr;
}

const std::vector<std::shared_ptr<SequencerChannel>>& SequencerEngine::GetAllChannels() const
{
    return channels_;
}

uint32_t SequencerEngine::GetChannelCount() const
{
    return static_cast<uint32_t>(channels_.size());
}

bool SequencerEngine::UpdateChannel(uint32_t channelId, const std::string& newName)
{
    auto channel = GetChannel(channelId);
    if (channel)
    {
        channel->SetChannelName(newName);
        return true;
    }
    return false;
}

bool SequencerEngine::DeleteChannel(uint32_t channelId)
{
    auto it = std::find_if(channels_.begin(), channels_.end(),
        [channelId](const std::shared_ptr<SequencerChannel>& ch) {
      return ch->GetChannelId() == channelId;
        });

    if (it != channels_.end())
    {
 channels_.erase(it);
        channelMap_.erase(channelId);
    return true;
    }
    return false;
}

void SequencerEngine::DeleteAllChannels()
{
    channels_.clear();
 channelMap_.clear();
}

bool SequencerEngine::LoadAudioToChannel(uint32_t channelId, const std::string& filePath)
{
    auto channel = GetChannel(channelId);
    if (channel)
    {
        auto player = std::make_shared<AudioPlayer>();
        if (player->LoadAudioFile(filePath))
        {
            channel->AssignAudioPlayer(player);
       return true;
        }
    }
    return false;
}

bool SequencerEngine::AssignPlayerToChannel(uint32_t channelId, std::shared_ptr<AudioPlayer> player)
{
    auto channel = GetChannel(channelId);
    if (channel && player)
    {
        return channel->AssignAudioPlayer(player);
    }
    return false;
}

uint32_t SequencerEngine::GetChannelIdCounter() const
{
    return nextChannelId_;
}
