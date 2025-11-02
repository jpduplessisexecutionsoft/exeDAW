#pragma once

#include <windows.h>
#include <gl/gl.h>
#include <memory>
#include <vector>
#include <functional>
#include <string>

struct Vec2 { float x, y; };

struct RectF { float x, y, w, h; };

inline bool PointInRectF(const RectF& r, float px, float py)
{
 return px >= r.x && px <= r.x + r.w && py >= r.y && py <= r.y + r.h;
}

// Simple OpenGL color helper
struct Color4
{
 float r, g, b, a;
 Color4() : r(0), g(0), b(0), a(1) {}
 Color4(float rr, float gg, float bb, float aa=1.0f) : r(rr), g(gg), b(bb), a(aa) {}
};

inline Color4 FromCOLORREF(COLORREF c, float alpha =1.0f)
{
 return Color4(GetRValue(c) /255.0f, GetGValue(c) /255.0f, GetBValue(c) /255.0f, alpha);
}

class GLTextRenderer
{
public:
 GLTextRenderer() : baseList_(0), hFont_(nullptr) {}
 ~GLTextRenderer();

 bool Initialize(HDC hdc, int pixelHeight =16, const wchar_t* face = L"Segoe UI");
 void Print(float x, float y, const std::wstring& text, const Color4& color);

private:
 GLuint baseList_;
 HFONT hFont_;
};

class UIElement
{
public:
 explicit UIElement(const RectF& r) : rect_(r), visible_(true) {}
 virtual ~UIElement() {}

 virtual void Render(int width, int height, GLTextRenderer* text) =0;
 virtual bool OnMouseDown(float x, float y) { (void)x; (void)y; return false; }
 virtual bool OnMouseUp(float x, float y) { (void)x; (void)y; return false; }
 virtual bool OnMouseMove(float x, float y) { (void)x; (void)y; return false; }

 void SetRect(const RectF& r) { rect_ = r; }
 const RectF& GetRect() const { return rect_; }

 void SetVisible(bool v) { visible_ = v; }
 bool IsVisible() const { return visible_; }

protected:
 RectF rect_;
 bool visible_;
};

class UIButton : public UIElement
{
public:
 UIButton(const RectF& r, std::wstring label, std::function<void()> onClick)
 : UIElement(r), label_(std::move(label)), onClick_(std::move(onClick)), hovered_(false), pressed_(false) {}

 void Render(int width, int height, GLTextRenderer* text) override;
 bool OnMouseDown(float x, float y) override;
 bool OnMouseUp(float x, float y) override;
 bool OnMouseMove(float x, float y) override;

private:
 std::wstring label_;
 std::function<void()> onClick_;
 bool hovered_;
 bool pressed_;
};

class UIMenuBar : public UIElement
{
public:
 struct Item { std::wstring title; std::function<void()> onClick; };

 UIMenuBar(const RectF& r) : UIElement(r), hoveredIndex_(-1), pressedIndex_(-1) {}

 void AddItem(const std::wstring& title, std::function<void()> onClick) { items_.push_back({ title, onClick }); }

 void Render(int width, int height, GLTextRenderer* text) override;
 bool OnMouseDown(float x, float y) override;
 bool OnMouseUp(float x, float y) override;
 bool OnMouseMove(float x, float y) override;

private:
 std::vector<Item> items_;
 int hoveredIndex_;
 int pressedIndex_;
};

class UIDraggableWindow : public UIElement
{
public:
 UIDraggableWindow(const RectF& r, std::wstring title)
 : UIElement(r), title_(std::move(title)), dragging_(false), dragOffset_{0,0} {}

 void Render(int width, int height, GLTextRenderer* text) override;
 bool OnMouseDown(float x, float y) override;
 bool OnMouseUp(float x, float y) override;
 bool OnMouseMove(float x, float y) override;

 // Content region inside the window (below title bar)
 RectF ContentRect() const;

private:
 std::wstring title_;
 bool dragging_;
 Vec2 dragOffset_;
};

class UIManager
{
public:
 UIManager() : text_(new GLTextRenderer()), width_(0), height_(0), mouseDown_(false), capture_(nullptr) {}
 ~UIManager() { delete text_; }

 bool Initialize(HDC hdc);
 void Resize(int w, int h) { width_ = w; height_ = h; }

 void Add(std::shared_ptr<UIElement> el) { elements_.push_back(std::move(el)); }

 void Render();

 void OnMouseDown(float x, float y);
 void OnMouseUp(float x, float y);
 void OnMouseMove(float x, float y);

 GLTextRenderer* GetText() const { return text_; }

private:
 GLTextRenderer* text_;
 int width_, height_;
 bool mouseDown_;
 UIElement* capture_;
 std::vector<std::shared_ptr<UIElement>> elements_;

 UIElement* HitTest(float x, float y);
};
