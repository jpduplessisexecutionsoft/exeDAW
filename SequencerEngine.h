#pragma once

#include "SequencerChannel.h"
#include <memory>
#include <vector>
#include <map>

// Sequencer Engine - CRUD operations for channels and audio management
class SequencerEngine
{
public:
    SequencerEngine();
    ~SequencerEngine();

    // CREATE operations
    std::shared_ptr<SequencerChannel> CreateChannel(const std::string& name, 
       SequencerChannel::ChannelType type = SequencerChannel::AUDIO);
    
    // READ operations
    std::shared_ptr<SequencerChannel> GetChannel(uint32_t channelId) const;
    std::shared_ptr<SequencerChannel> GetChannelByName(const std::string& name) const;
    const std::vector<std::shared_ptr<SequencerChannel>>& GetAllChannels() const;
    uint32_t GetChannelCount() const;

    // UPDATE operations
    bool UpdateChannel(uint32_t channelId, const std::string& newName);
    
    // DELETE operations
    bool DeleteChannel(uint32_t channelId);
    void DeleteAllChannels();

    // Audio player management
    bool LoadAudioToChannel(uint32_t channelId, const std::string& filePath);
    bool AssignPlayerToChannel(uint32_t channelId, std::shared_ptr<AudioPlayer> player);

    // Sequencer state management
    uint32_t GetChannelIdCounter() const;

private:
    std::vector<std::shared_ptr<SequencerChannel>> channels_;
    std::map<uint32_t, std::shared_ptr<SequencerChannel>> channelMap_;
    uint32_t nextChannelId_;
};
