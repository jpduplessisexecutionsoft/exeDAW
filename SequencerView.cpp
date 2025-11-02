#include "SequencerView.h"
#include "DAWTheme.h"
#include <cmath>
#include <sstream>
#include <iomanip>

SequencerView::SequencerView(std::shared_ptr<SequencerModel> model)
 : model_(model), pixelsPerBeat_(DEFAULT_PIXELS_PER_BEAT),
 pixelsPerNote_(DEFAULT_PIXELS_PER_NOTE), scrollX_(0.0f), scrollY_(0.0f),
 dragStartX_(0), dragStartY_(0), dragging_(false), draggedNoteTrack_(-1),
 draggedNoteIndex_(-1), dragMode_(DragMode::None)
{
}

SequencerView::~SequencerView()
{
}

int SequencerView::TimeToPixelX(uint32_t beat) const
{
 return (int)(beat * pixelsPerBeat_ / SequencerModel::PPQ) - (int)scrollX_;
}

int SequencerView::PitchToPixelY(uint8_t pitch) const
{
 return HEADER_HEIGHT + PIANO_ROLL_HEIGHT - (pitch + 1) * (int)pixelsPerNote_ - (int)scrollY_;
}

uint32_t SequencerView::PixelXToTime(int pixelX) const
{
 int adjustedX = pixelX + (int)scrollX_;
 return (uint32_t)((adjustedX * SequencerModel::PPQ) / pixelsPerBeat_);
}

uint8_t SequencerView::PixelYToPitch(int pixelY) const
{
 int adjustedY = HEADER_HEIGHT + PIANO_ROLL_HEIGHT - pixelY + (int)scrollY_;
 uint8_t pitch = (uint8_t)(adjustedY / pixelsPerNote_ - 1);
 if (pitch > 127) pitch = 127;
 return pitch;
}

void SequencerView::Render(int width, int height, GLTextRenderer* textRenderer)
{
 glMatrixMode(GL_PROJECTION);
 glLoadIdentity();
 glOrtho(0, width, height, 0, -1, 1);
 glMatrixMode(GL_MODELVIEW);
 glLoadIdentity();

 glClearColor(0.06f, 0.06f, 0.08f, 1.0f);
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 // Render in order
 RenderGrid(width, height);
 RenderPlayhead(width, height);
 RenderNotes(width, height, textRenderer);
 RenderSelection(width, height);
 RenderMeasureRuler(width, textRenderer);
 RenderTimelineRuler(width, textRenderer);
 RenderHeader(width, height, textRenderer);
 RenderTrackHeaders(height);
}

void SequencerView::RenderHeader(int width, int height, GLTextRenderer* text)
{
 // Header background
 DrawFilledRect(0, 0, width, HEADER_HEIGHT, DAWTheme::COLOR_SURFACE_ELEVATED);
 DrawOutlineRect(0, 0, width, HEADER_HEIGHT, DAWTheme::COLOR_SURFACE_OVERLAY);
}

void SequencerView::RenderTrackHeaders(int height)
{
 const auto& tracks = model_->GetTracks();
 int y = HEADER_HEIGHT;

 for (size_t i = 0; i < tracks.size(); ++i)
 {
 int trackHeight = (int)(128 * pixelsPerNote_);
 
 // Track background
 uint32_t bgColor = (i % 2 == 0) ? DAWTheme::COLOR_SURFACE : DAWTheme::COLOR_SURFACE_OVERLAY;
 DrawFilledRect(0, y, TRACK_LABEL_WIDTH, trackHeight, bgColor);
 DrawOutlineRect(0, y, TRACK_LABEL_WIDTH, trackHeight, DAWTheme::COLOR_SURFACE_OVERLAY);

 // Track name
 // Note: In a real implementation, would render text using GLTextRenderer
 y += trackHeight;
 }
}

void SequencerView::RenderGrid(int width, int height)
{
 glColor4ub(50, 50, 65, 255);
 glBegin(GL_LINES);

 // Vertical grid lines (beats)
 uint32_t startBeat = (uint32_t)(scrollX_ * SequencerModel::PPQ / pixelsPerBeat_);
 uint32_t endBeat = (uint32_t)((scrollX_ + width) * SequencerModel::PPQ / pixelsPerBeat_) + SequencerModel::PPQ;

 for (uint32_t beat = startBeat; beat <= endBeat; beat += SequencerModel::PPQ / 4)
 {
 int x = TimeToPixelX(beat);
 if (x >= 0 && x < width)
 {
 glVertex2i(x, HEADER_HEIGHT);
 glVertex2i(x, height);
 }
 }

 // Horizontal grid lines (notes)
 for (int pitch = 0; pitch < 128; ++pitch)
 {
 int y = PitchToPixelY(pitch);
 if (y >= HEADER_HEIGHT && y < height)
 {
 glVertex2i(TRACK_LABEL_WIDTH, y);
 glVertex2i(width, y);
 }
 }

 glEnd();

 // Measure lines (thicker)
 glColor4ub(70, 70, 90, 255);
 glBegin(GL_LINES);
 for (uint32_t beat = startBeat; beat <= endBeat; beat += SequencerModel::PPQ * 4)
 {
 int x = TimeToPixelX(beat);
 if (x >= 0 && x < width)
 {
 glVertex2i(x, HEADER_HEIGHT);
 glVertex2i(x, height);
 }
 }
 glEnd();
}

void SequencerView::RenderPlayhead(int width, int height)
{
 int playx = TimeToPixelX(model_->GetPlayheadBeat());
 if (playx >= TRACK_LABEL_WIDTH && playx < width)
 {
 DrawLine(playx, HEADER_HEIGHT, playx, height, DAWTheme::COLOR_PLAYHEAD);
 }
}

void SequencerView::RenderNotes(int width, int height, GLTextRenderer* text)
{
 const auto& tracks = model_->GetTracks();

 for (size_t trackIdx = 0; trackIdx < tracks.size(); ++trackIdx)
 {
 const auto& track = tracks[trackIdx];
 for (size_t noteIdx = 0; noteIdx < track.notes.size(); ++noteIdx)
 {
 const auto& note = track.notes[noteIdx];

 int x = TRACK_LABEL_WIDTH + TimeToPixelX(note.startBeat);
 int y = PitchToPixelY(note.pitch);
 int w = (int)(note.durationBeats * pixelsPerBeat_ / SequencerModel::PPQ);
 int h = (int)pixelsPerNote_;

 // Adjust for small width
 if (w < 2) w = 2;

 // Note color based on velocity
 uint32_t color = track.color;
 if (note.selected)
 {
 color = DAWTheme::COLOR_ACCENT_PRIMARY;
 }

 // Render note
 DrawFilledRect(x, y, w, h, color);
 DrawOutlineRect(x, y, w, h, DAWTheme::COLOR_TEXT_SECONDARY);

 // Velocity display (height or opacity)
 float velocityAlpha = note.velocity / 127.0f;
 (void)velocityAlpha;  // Would use this for opacity blending
 }
 }
}

void SequencerView::RenderSelection(int width, int height)
{
 // Highlight selected notes with semi-transparent overlay
 glEnable(GL_BLEND);
 glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

 const auto& tracks = model_->GetTracks();
 for (size_t trackIdx = 0; trackIdx < tracks.size(); ++trackIdx)
 {
 const auto& track = tracks[trackIdx];
 for (const auto& note : track.notes)
 {
 if (note.selected)
 {
 int x = TRACK_LABEL_WIDTH + TimeToPixelX(note.startBeat);
 int y = PitchToPixelY(note.pitch);
 int w = (int)(note.durationBeats * pixelsPerBeat_ / SequencerModel::PPQ);
 int h = (int)pixelsPerNote_;

 glColor4f(0.35f, 0.7f, 1.0f, 0.3f);
 glBegin(GL_QUADS);
 glVertex2i(x, y);
 glVertex2i(x + w, y);
 glVertex2i(x + w, y + h);
 glVertex2i(x, y + h);
 glEnd();
 }
 }
 }

 glDisable(GL_BLEND);
}

void SequencerView::RenderMeasureRuler(int width, GLTextRenderer* text)
{
 // Draw measure numbers at top
 uint32_t startBeat = (uint32_t)(scrollX_ * SequencerModel::PPQ / pixelsPerBeat_);
 uint32_t endBeat = (uint32_t)((scrollX_ + width) * SequencerModel::PPQ / pixelsPerBeat_) + SequencerModel::PPQ * 4;

 for (uint32_t beat = startBeat; beat <= endBeat; beat += SequencerModel::PPQ * 4)
 {
 int measure = beat / (SequencerModel::PPQ * 4) + 1;
 int x = TRACK_LABEL_WIDTH + TimeToPixelX(beat);

 if (x >= TRACK_LABEL_WIDTH && x < width)
 {
 std::ostringstream oss;
 oss << measure;
 // DrawText(x + 4, 8, oss.str(), text);
 }
 }
}

void SequencerView::RenderTimelineRuler(int width, GLTextRenderer* text)
{
 // Draw beat markers
 uint32_t startBeat = (uint32_t)(scrollX_ * SequencerModel::PPQ / pixelsPerBeat_);
 uint32_t endBeat = (uint32_t)((scrollX_ + width) * SequencerModel::PPQ / pixelsPerBeat_) + SequencerModel::PPQ;

 glColor4ub(180, 180, 190, 255);
 glBegin(GL_LINES);
 for (uint32_t beat = startBeat; beat <= endBeat; beat += SequencerModel::PPQ)
 {
 int x = TRACK_LABEL_WIDTH + TimeToPixelX(beat);
 glVertex2i(x, HEADER_HEIGHT - 4);
 glVertex2i(x, HEADER_HEIGHT);
 }
 glEnd();
}

void SequencerView::OnMouseDown(float x, float y)
{
 dragStartX_ = (int)x;
 dragStartY_ = (int)y;
 dragging_ = true;

 // Hit test
 if (!HitTestNote((int)x, (int)y, draggedNoteTrack_, draggedNoteIndex_))
 {
 draggedNoteTrack_ = -1;
 draggedNoteIndex_ = -1;
 model_->DeselectAll();
 }
 else if (draggedNoteTrack_ >= 0)
 {
 auto* track = model_->GetTrackMutable(draggedNoteTrack_);
 if (track && draggedNoteIndex_ < (int)track->notes.size())
 {
 if (!track->notes[draggedNoteIndex_].selected)
 {
 model_->DeselectAll();
 track->notes[draggedNoteIndex_].selected = true;
 }
 dragMode_ = DragMode::Move;
 }
 }
}

void SequencerView::OnMouseUp(float x, float y)
{
 dragging_ = false;
 dragMode_ = DragMode::None;
 draggedNoteTrack_ = -1;
 draggedNoteIndex_ = -1;
}

void SequencerView::OnMouseMove(float x, float y)
{
 if (!dragging_ || dragMode_ == DragMode::None) return;

 int dx = (int)x - dragStartX_;
 int dy = (int)y - dragStartY_;

 if (dragMode_ == DragMode::Move && draggedNoteTrack_ >= 0 && draggedNoteIndex_ >= 0)
 {
 auto* track = model_->GetTrackMutable(draggedNoteTrack_);
 if (track && draggedNoteIndex_ < (int)track->notes.size())
 {
 auto& note = track->notes[draggedNoteIndex_];

 // Move all selected notes
 uint32_t deltaTime = PixelXToTime(dx) - PixelXToTime(0);
 int deltaPitch = (dy > 0) ? -1 : (dy < 0) ? 1 : 0;

 for (auto& n : track->notes)
 {
 if (n.selected)
 {
 if ((int)n.startBeat + (int)deltaTime >= 0)
 {
 n.startBeat += deltaTime;
 }
 if ((int)n.pitch + deltaPitch >= 0 && (int)n.pitch + deltaPitch <= 127)
 {
 n.pitch += deltaPitch;
 }
 }
 }
 }
 }
}

void SequencerView::OnMouseWheel(float delta)
{
 // Horizontal zoom
 float newZoom = pixelsPerBeat_ * (1.0f + delta * 0.1f);
 if (newZoom >= MIN_ZOOM_H && newZoom <= MAX_ZOOM_H)
 {
 pixelsPerBeat_ = newZoom;
 }
}

void SequencerView::OnKeyDown(int keyCode)
{
 switch (keyCode)
 {
 case 'A':  // Select all
 {
 auto* track = model_->GetTrackMutable(0);
 if (track) model_->SelectAll(0);
 break;
 }
 case VK_DELETE:  // Delete selected
 model_->DeleteSelected();
 break;
 case 'Q':  // Quantize
 model_->QuantizeSelected();
 break;
 default:
 break;
 }
}

bool SequencerView::HitTestNote(int mouseX, int mouseY, int& outTrack, int& outNote)
{
 outTrack = -1;
 outNote = -1;

 const auto& tracks = model_->GetTracks();
 for (size_t trackIdx = 0; trackIdx < tracks.size(); ++trackIdx)
 {
 const auto& track = tracks[trackIdx];
 for (size_t noteIdx = 0; noteIdx < track.notes.size(); ++noteIdx)
 {
 const auto& note = track.notes[noteIdx];

 int x = TRACK_LABEL_WIDTH + TimeToPixelX(note.startBeat);
 int y = PitchToPixelY(note.pitch);
 int w = (int)(note.durationBeats * pixelsPerBeat_ / SequencerModel::PPQ);
 int h = (int)pixelsPerNote_;

 if (IsPointInRect(mouseX, mouseY, x, y, w, h))
 {
 outTrack = trackIdx;
 outNote = noteIdx;
 return true;
 }
 }
 }

 return false;
}

bool SequencerView::IsPointInRect(int x, int y, int rx, int ry, int rw, int rh) const
{
 return x >= rx && x < rx + rw && y >= ry && y < ry + rh;
}

void SequencerView::DrawFilledRect(int x, int y, int w, int h, uint32_t color)
{
 uint8_t r = (color >> 16) & 0xFF;
 uint8_t g = (color >> 8) & 0xFF;
 uint8_t b = color & 0xFF;

 glColor3ub(r, g, b);
 glBegin(GL_QUADS);
 glVertex2i(x, y);
 glVertex2i(x + w, y);
 glVertex2i(x + w, y + h);
 glVertex2i(x, y + h);
 glEnd();
}

void SequencerView::DrawOutlineRect(int x, int y, int w, int h, uint32_t color)
{
 uint8_t r = (color >> 16) & 0xFF;
 uint8_t g = (color >> 8) & 0xFF;
 uint8_t b = color & 0xFF;

 glColor3ub(r, g, b);
 glBegin(GL_LINE_LOOP);
 glVertex2i(x, y);
 glVertex2i(x + w, y);
 glVertex2i(x + w, y + h);
 glVertex2i(x, y + h);
 glEnd();
}

void SequencerView::DrawText(int x, int y, const std::string& text, GLTextRenderer* renderer)
{
 (void)x; (void)y; (void)text; (void)renderer;
 // TODO: Implement text rendering
}

void SequencerView::DrawLine(int x1, int y1, int x2, int y2, uint32_t color)
{
 uint8_t r = (color >> 16) & 0xFF;
 uint8_t g = (color >> 8) & 0xFF;
 uint8_t b = color & 0xFF;

 glColor3ub(r, g, b);
 glBegin(GL_LINES);
 glVertex2i(x1, y1);
 glVertex2i(x2, y2);
 glEnd();
}
