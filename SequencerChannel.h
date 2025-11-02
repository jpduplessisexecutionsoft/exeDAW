#pragma once

#include "AudioPlayer.h"
#include <memory>

// Sequencer Channel - Represents a track/channel in the DAW
class SequencerChannel
{
public:
    enum ChannelType
    {
        AUDIO,
 MIDI,
        BUS
    };

SequencerChannel(uint32_t channelId, const std::string& name, ChannelType type = AUDIO);
    ~SequencerChannel();

    // Channel information
    uint32_t GetChannelId() const;
    std::string GetChannelName() const;
    void SetChannelName(const std::string& name);
    ChannelType GetChannelType() const;

    // Audio player management
    bool AssignAudioPlayer(std::shared_ptr<AudioPlayer> player);
    std::shared_ptr<AudioPlayer> GetAudioPlayer() const;
    bool HasAudioPlayer() const;
    void RemoveAudioPlayer();

    // Channel properties
    void SetVolume(float volume);
    float GetVolume() const;

    void SetPan(float pan); // -1.0 (left) to 1.0 (right)
    float GetPan() const;

    void SetMuted(bool muted);
    bool IsMuted() const;

    void SetSolo(bool solo);
    bool IsSolo() const;

private:
    uint32_t channelId_;
    std::string channelName_;
    ChannelType channelType_;
    std::shared_ptr<AudioPlayer> audioPlayer_;
    float volume_;
    float pan_;
    bool muted_;
    bool solo_;
};
