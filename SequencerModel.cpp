#include "SequencerModel.h"
#include <algorithm>

SequencerModel::SequencerModel()
 : playheadBeat_(0), loopStartBeat_(0), loopEndBeat_(1920),
 loopEnabled_(false), tempoAPM_(120.0), timeSignature_(4), quantize_(QuantizeValue::Off)
{
}

SequencerModel::~SequencerModel()
{
 ClearTracks();
}

void SequencerModel::AddTrack(const std::string& name)
{
 SequencerTrack track;
 track.name = name;
 tracks_.push_back(std::move(track));
}

void SequencerModel::RemoveTrack(size_t index)
{
 if (index < tracks_.size())
 {
 tracks_.erase(tracks_.begin() + index);
 }
}

void SequencerModel::ClearTracks()
{
 tracks_.clear();
}

SequencerTrack* SequencerModel::GetTrackMutable(size_t index)
{
 if (index < tracks_.size()) return &tracks_[index];
 return nullptr;
}

void SequencerModel::AddNote(size_t trackIdx, const SequencerNote& note)
{
 if (trackIdx < tracks_.size())
 {
 tracks_[trackIdx].notes.push_back(note);
 // Keep notes sorted by start time
 std::sort(tracks_[trackIdx].notes.begin(), tracks_[trackIdx].notes.end(),
 [](const SequencerNote& a, const SequencerNote& b) { return a.startBeat < b.startBeat; });
 }
}

void SequencerModel::RemoveNote(size_t trackIdx, size_t noteIdx)
{
 if (trackIdx < tracks_.size() && noteIdx < tracks_[trackIdx].notes.size())
 {
 tracks_[trackIdx].notes.erase(tracks_[trackIdx].notes.begin() + noteIdx);
 }
}

void SequencerModel::ClearNotes(size_t trackIdx)
{
 if (trackIdx < tracks_.size())
 {
 tracks_[trackIdx].notes.clear();
 }
}

void SequencerModel::SetLoopRange(uint32_t start, uint32_t end)
{
 if (start < end)
 {
 loopStartBeat_ = start;
 loopEndBeat_ = end;
 }
}

uint32_t SequencerModel::QuantizeNote(uint32_t beat) const
{
 if (quantize_ == QuantizeValue::Off) return beat;

 uint32_t gridSize = PPQ;  // Default to beat
 switch (quantize_)
 {
 case QuantizeValue::Beat:   gridSize = PPQ; break;           // 1/4
 case QuantizeValue::Half:    gridSize = PPQ / 2; break;    // 1/8
 case QuantizeValue::Quarter:  gridSize = PPQ / 4; break;      // 1/16
 case QuantizeValue::Eighth:   gridSize = PPQ / 8; break;      // 1/32
 case QuantizeValue::Triplet:  gridSize = PPQ / 3; break;      // 1/3
 default: return beat;
 }

 return ((beat + gridSize / 2) / gridSize) * gridSize;
}

void SequencerModel::SelectAll(size_t trackIdx)
{
 if (trackIdx < tracks_.size())
 {
 for (auto& note : tracks_[trackIdx].notes)
 {
 note.selected = true;
 }
 }
}

void SequencerModel::DeselectAll()
{
 for (auto& track : tracks_)
 {
 for (auto& note : track.notes)
 {
 note.selected = false;
 }
 }
}

void SequencerModel::DeleteSelected()
{
 for (auto& track : tracks_)
 {
 auto it = track.notes.begin();
 while (it != track.notes.end())
 {
 if (it->selected)
 {
 it = track.notes.erase(it);
 }
 else
 {
 ++it;
 }
 }
 }
}

void SequencerModel::QuantizeSelected()
{
 for (auto& track : tracks_)
 {
 for (auto& note : track.notes)
 {
 if (note.selected)
 {
 note.startBeat = QuantizeNote(note.startBeat);
 }
 }
 }
}
