#pragma once

#include <windows.h>
#include <gl/gl.h>
#include <vector>
#include <memory>
#include <cstdint>
#include <string>

struct SequencerNote
{
    uint8_t pitch;
  uint8_t velocity;
    uint32_t startBeat;
uint32_t durationBeats;
    bool selected = false;

    SequencerNote(uint8_t p = 0, uint8_t v = 100, uint32_t start = 0, uint32_t dur = 480)
        : pitch(p), velocity(v), startBeat(start), durationBeats(dur) {}
};

struct SequencerTrack
{
    std::string name;
    std::vector<SequencerNote> notes;
    uint8_t volume = 100;
    bool muted = false;
    bool soloed = false;
uint32_t color = 0xFF5588FF;
};

enum class QuantizeValue
{
    Off,
    Beat,
    Half,
  Quarter,
    Eighth,
    Triplet
};

class SequencerModel
{
public:
    SequencerModel();
    ~SequencerModel();

    void AddTrack(const std::string& name);
    void RemoveTrack(size_t index);
    void ClearTracks();

    const std::vector<SequencerTrack>& GetTracks() const { return tracks_; }
    SequencerTrack* GetTrackMutable(size_t index);
    size_t GetTrackCount() const { return tracks_.size(); }

    void AddNote(size_t trackIdx, const SequencerNote& note);
  void RemoveNote(size_t trackIdx, size_t noteIdx);
    void ClearNotes(size_t trackIdx);

    double GetTempoAPM() const { return tempoAPM_; }
    void SetTempoAPM(double tempo) { tempoAPM_ = tempo; }

    int GetTimeSignature() const { return timeSignature_; }
    void SetTimeSignature(int sig) { timeSignature_ = sig; }

    uint32_t GetPlayheadBeat() const { return playheadBeat_; }
    void SetPlayheadBeat(uint32_t beat) { playheadBeat_ = beat; }

    uint32_t GetLoopStartBeat() const { return loopStartBeat_; }
    uint32_t GetLoopEndBeat() const { return loopEndBeat_; }
    void SetLoopRange(uint32_t start, uint32_t end);

    bool IsLoopEnabled() const { return loopEnabled_; }
    void SetLoopEnabled(bool enabled) { loopEnabled_ = enabled; }

    QuantizeValue GetQuantize() const { return quantize_; }
    void SetQuantize(QuantizeValue q) { quantize_ = q; }
    uint32_t QuantizeNote(uint32_t beat) const;

    void SelectAll(size_t trackIdx);
    void DeselectAll();
    void DeleteSelected();
    void QuantizeSelected();

    static constexpr uint32_t PPQ = 480;

private:
 std::vector<SequencerTrack> tracks_;
    uint32_t playheadBeat_ = 0;
    uint32_t loopStartBeat_ = 0;
    uint32_t loopEndBeat_ = 1920;
    bool loopEnabled_ = false;
    double tempoAPM_ = 120.0;
    int timeSignature_ = 4;
    QuantizeValue quantize_ = QuantizeValue::Off;
};
