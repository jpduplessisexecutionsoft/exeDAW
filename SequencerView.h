#pragma once

#include <windows.h>
#include <gl/gl.h>
#include <memory>
#include <string>
#include <sstream>
#include "SequencerModel.h"
#include "ModernUI.h"

class SequencerView
{
public:
    SequencerView(std::shared_ptr<SequencerModel> model);
    ~SequencerView();

    void Render(int width, int height, GLTextRenderer* textRenderer);
    void OnMouseDown(float x, float y);
    void OnMouseUp(float x, float y);
    void OnMouseMove(float x, float y);
  void OnMouseWheel(float delta);
    void OnKeyDown(int keyCode);

    void SetZoomHorizontal(float pixelsPerBeat) { pixelsPerBeat_ = pixelsPerBeat; }
    void SetZoomVertical(float pixelsPerNote) { pixelsPerNote_ = pixelsPerNote; }
    void SetScrollX(float offset) { scrollX_ = offset; }
    void SetScrollY(float offset) { scrollY_ = offset; }

float GetZoomHorizontal() const { return pixelsPerBeat_; }
    float GetZoomVertical() const { return pixelsPerNote_; }
    float GetScrollX() const { return scrollX_; }
    float GetScrollY() const { return scrollY_; }

    int TimeToPixelX(uint32_t beat) const;
    int PitchToPixelY(uint8_t pitch) const;
    uint32_t PixelXToTime(int pixelX) const;
    uint8_t PixelYToPitch(int pixelY) const;

    static constexpr float DEFAULT_PIXELS_PER_BEAT = 64.0f;
    static constexpr float DEFAULT_PIXELS_PER_NOTE = 12.0f;
    static constexpr float MIN_ZOOM_H = 8.0f;
    static constexpr float MAX_ZOOM_H = 256.0f;
    static constexpr float MIN_ZOOM_V = 4.0f;
  static constexpr float MAX_ZOOM_V = 32.0f;
static constexpr int HEADER_HEIGHT = 32;
    static constexpr int TRACK_LABEL_WIDTH = 120;
    static constexpr int PIANO_ROLL_HEIGHT = 12 * 128;

private:
    std::shared_ptr<SequencerModel> model_;
    float pixelsPerBeat_ = DEFAULT_PIXELS_PER_BEAT;
    float pixelsPerNote_ = DEFAULT_PIXELS_PER_NOTE;
    float scrollX_ = 0.0f;
    float scrollY_ = 0.0f;

    int dragStartX_ = 0;
    int dragStartY_ = 0;
    bool dragging_ = false;
 int draggedNoteTrack_ = -1;
    int draggedNoteIndex_ = -1;
    enum class DragMode { None, Move, ResizeStart, ResizeEnd } dragMode_ = DragMode::None;

    void RenderHeader(int width, int height, GLTextRenderer* text);
    void RenderTrackHeaders(int height);
    void RenderGrid(int width, int height);
    void RenderPlayhead(int width, int height);
    void RenderNotes(int width, int height, GLTextRenderer* text);
    void RenderSelection(int width, int height);
    void RenderMeasureRuler(int width, GLTextRenderer* text);
    void RenderTimelineRuler(int width, GLTextRenderer* text);

    bool HitTestNote(int mouseX, int mouseY, int& outTrack, int& outNote);
    bool IsPointInRect(int x, int y, int rx, int ry, int rw, int rh) const;

    void DrawFilledRect(int x, int y, int w, int h, uint32_t color);
void DrawOutlineRect(int x, int y, int w, int h, uint32_t color);
    void DrawText(int x, int y, const std::string& text, GLTextRenderer* renderer);
    void DrawLine(int x1, int y1, int x2, int y2, uint32_t color);
};
