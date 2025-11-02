#include "ModernUI.h"
#include "DAWTheme.h"
#include <windows.h>
#include <gl/glu.h>
#include <cwchar>

// ---------------- GLTextRenderer ----------------
GLTextRenderer::~GLTextRenderer()
{
 if (baseList_)
 {
 glDeleteLists(baseList_,96);
 baseList_ =0;
 }
 if (hFont_)
 {
 DeleteObject(hFont_);
 hFont_ = nullptr;
 }
}

bool GLTextRenderer::Initialize(HDC hdc, int pixelHeight, const wchar_t* face)
{
 if (baseList_)
 return true;

 HFONT hFont = CreateFontW(-pixelHeight,0,0,0, FW_NORMAL, FALSE, FALSE, FALSE,
 DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
 DEFAULT_PITCH | FF_DONTCARE, face);
 if (!hFont)
 return false;

 HFONT old = (HFONT)SelectObject(hdc, hFont);
 baseList_ = glGenLists(96);
 wglUseFontBitmapsW(hdc,32,96, baseList_);
 SelectObject(hdc, old);
 hFont_ = hFont;
 return true;
}

void GLTextRenderer::Print(float x, float y, const std::wstring& text, const Color4& color)
{
 glColor4f(color.r, color.g, color.b, color.a);
 glRasterPos2f(x, y);
 glPushAttrib(GL_LIST_BIT);
 glListBase(baseList_ -32);
 glCallLists((GLsizei)text.size(), GL_UNSIGNED_SHORT, text.c_str());
 glPopAttrib();
}

//-------------- Helpers--------------
static void FillRectGL(const RectF& r, const Color4& c)
{
 glColor4f(c.r, c.g, c.b, c.a);
 glBegin(GL_QUADS);
 glVertex2f(r.x, r.y);
 glVertex2f(r.x + r.w, r.y);
 glVertex2f(r.x + r.w, r.y + r.h);
 glVertex2f(r.x, r.y + r.h);
 glEnd();
}

static void StrokeRectGL(const RectF& r, const Color4& c, float w =1.0f)
{
 (void)w;
 glColor4f(c.r, c.g, c.b, c.a);
 glBegin(GL_LINE_LOOP);
 glVertex2f(r.x, r.y);
 glVertex2f(r.x + r.w, r.y);
 glVertex2f(r.x + r.w, r.y + r.h);
 glVertex2f(r.x, r.y + r.h);
 glEnd();
}

//-------------- UIButton--------------
void UIButton::Render(int width, int height, GLTextRenderer* text)
{
 (void)width; (void)height;
 Color4 bg = FromCOLORREF(DAWTheme::COLOR_SURFACE);
 if (hovered_) bg = FromCOLORREF(DAWTheme::COLOR_HOVER);
 if (pressed_) bg = FromCOLORREF(DAWTheme::COLOR_ACCENT_PRIMARY,0.9f);

 FillRectGL(rect_, bg);
 StrokeRectGL(rect_, FromCOLORREF(DAWTheme::COLOR_SURFACE_OVERLAY));

 // Center text baseline roughly
 float tx = rect_.x +8;
 float ty = rect_.y + rect_.h /2.0f +5;
 text->Print(tx, ty, label_, FromCOLORREF(DAWTheme::COLOR_TEXT_PRIMARY));
}

bool UIButton::OnMouseDown(float x, float y)
{
 if (!PointInRectF(rect_, x, y)) return false;
 pressed_ = true;
 return true;
}

bool UIButton::OnMouseUp(float x, float y)
{
 if (pressed_ && PointInRectF(rect_, x, y) && onClick_) onClick_();
 pressed_ = false;
 return PointInRectF(rect_, x, y);
}

bool UIButton::OnMouseMove(float x, float y)
{
 hovered_ = PointInRectF(rect_, x, y);
 return hovered_;
}

//-------------- UIMenuBar--------------
void UIMenuBar::Render(int width, int height, GLTextRenderer* text)
{
 (void)width; (void)height;
 FillRectGL(rect_, FromCOLORREF(DAWTheme::COLOR_SURFACE_ELEVATED));
 StrokeRectGL(rect_, FromCOLORREF(DAWTheme::COLOR_SURFACE_OVERLAY));

 float x = rect_.x +10;
 for (size_t i =0; i < items_.size(); ++i)
 {
 std::wstring t = items_[i].title;
 RectF itemRect{ x, rect_.y +2, (float)(t.size() *9 +20), rect_.h -4 };
 Color4 bg = (int)i == hoveredIndex_ ? FromCOLORREF(DAWTheme::COLOR_HOVER) : FromCOLORREF(DAWTheme::COLOR_SURFACE_ELEVATED);
 FillRectGL(itemRect, bg);
 StrokeRectGL(itemRect, FromCOLORREF(DAWTheme::COLOR_SURFACE_OVERLAY));
 text->Print(itemRect.x +10, itemRect.y + itemRect.h /2.0f +5, t, FromCOLORREF(DAWTheme::COLOR_TEXT_PRIMARY));
 x += itemRect.w +6;
 }
}

bool UIMenuBar::OnMouseDown(float x, float y)
{
 if (!PointInRectF(rect_, x, y)) return false;
 if (hoveredIndex_ >=0) { pressedIndex_ = hoveredIndex_; return true; }
 return false;
}

bool UIMenuBar::OnMouseUp(float x, float y)
{
 if (!PointInRectF(rect_, x, y)) { pressedIndex_ = -1; return false; }
 if (pressedIndex_ >=0 && pressedIndex_ == hoveredIndex_)
 {
 if (pressedIndex_ < (int)items_.size() && items_[pressedIndex_].onClick) items_[pressedIndex_].onClick();
 }
 pressedIndex_ = -1;
 return true;
}

bool UIMenuBar::OnMouseMove(float x, float y)
{
 if (!PointInRectF(rect_, x, y)) { hoveredIndex_ = -1; return false; }
 float curX = rect_.x +10;
 hoveredIndex_ = -1;
 for (size_t i =0; i < items_.size(); ++i)
 {
 std::wstring t = items_[i].title;
 RectF itemRect{ curX, rect_.y +2, (float)(t.size() *9 +20), rect_.h -4 };
 if (PointInRectF(itemRect, x, y)) { hoveredIndex_ = (int)i; break; }
 curX += itemRect.w +6;
 }
 return true;
}

//-------------- UIDraggableWindow--------------
RectF UIDraggableWindow::ContentRect() const
{
 return RectF{ rect_.x +2, rect_.y +28, rect_.w -4, rect_.h -30 };
}

void UIDraggableWindow::Render(int width, int height, GLTextRenderer* text)
{
 (void)width; (void)height;
 // Window background
 FillRectGL(rect_, FromCOLORREF(DAWTheme::COLOR_SURFACE));
 StrokeRectGL(rect_, FromCOLORREF(DAWTheme::COLOR_SURFACE_OVERLAY));
 // Title bar
 RectF titleBar{ rect_.x, rect_.y, rect_.w,24 };
 FillRectGL(titleBar, FromCOLORREF(DAWTheme::COLOR_SURFACE_ELEVATED));
 StrokeRectGL(titleBar, FromCOLORREF(DAWTheme::COLOR_SURFACE_OVERLAY));
 text->Print(titleBar.x +8, titleBar.y +16, title_, FromCOLORREF(DAWTheme::COLOR_TEXT_PRIMARY));
}

bool UIDraggableWindow::OnMouseDown(float x, float y)
{
 RectF titleBar{ rect_.x, rect_.y, rect_.w,24 };
 if (PointInRectF(titleBar, x, y))
 {
 dragging_ = true;
 dragOffset_ = { x - rect_.x, y - rect_.y };
 return true;
 }
 return false;
}

bool UIDraggableWindow::OnMouseUp(float x, float y)
{
 (void)x; (void)y;
 dragging_ = false;
 return false;
}

bool UIDraggableWindow::OnMouseMove(float x, float y)
{
 if (dragging_)
 {
 rect_.x = x - dragOffset_.x;
 rect_.y = y - dragOffset_.y;
 return true;
 }
 return PointInRectF(rect_, x, y);
}

//-------------- UIManager--------------
bool UIManager::Initialize(HDC hdc)
{
 return text_->Initialize(hdc,16, L"Segoe UI");
}

void UIManager::Render()
{
 // Setup2D ortho projection in pixel coordinates
 glMatrixMode(GL_PROJECTION);
 glLoadIdentity();
 glOrtho(0, width_, height_,0, -1,1);
 glMatrixMode(GL_MODELVIEW);
 glLoadIdentity();

 // No background fill here; caller is responsible for scene/background
 for (auto& el : elements_)
 {
 if (el->IsVisible()) el->Render(width_, height_, text_);
 }
}

UIElement* UIManager::HitTest(float x, float y)
{
 for (auto it = elements_.rbegin(); it != elements_.rend(); ++it)
 {
 auto& el = *it;
 if (el->IsVisible() && PointInRectF(el->GetRect(), x, y))
 return el.get();
 }
 return nullptr;
}

void UIManager::OnMouseDown(float x, float y)
{
 mouseDown_ = true;
 capture_ = HitTest(x, y);
 if (capture_) capture_->OnMouseDown(x, y);
}

void UIManager::OnMouseUp(float x, float y)
{
 mouseDown_ = false;
 if (capture_) { capture_->OnMouseUp(x, y); capture_ = nullptr; }
}

void UIManager::OnMouseMove(float x, float y)
{
 if (capture_) { capture_->OnMouseMove(x, y); return; }
 // Hover behavior
 if (auto* el = HitTest(x, y)) el->OnMouseMove(x, y);
}
