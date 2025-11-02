#include "DAWTheme.h"
#include <cmath>
#include <algorithm>

void ModernDrawing::DrawRoundedRect(HDC hdc, const RECT& rect, int radius, COLORREF fillColor, COLORREF borderColor, int borderWidth)
{
    // Create rounded rectangle path
    HRGN hRgn = CreateRoundRectRgn(
  rect.left, rect.top,
      rect.right + 1, rect.bottom + 1,
   radius, radius
    );
    
    // Fill with color
    HBRUSH hBrush = CreateSolidBrush(fillColor);
FillRgn(hdc, hRgn, hBrush);
    
    // Draw border
    HPEN hPen = CreatePen(PS_SOLID, borderWidth, borderColor);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    FrameRgn(hdc, hRgn, hBrush, borderWidth, borderWidth);
    
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
    DeleteObject(hBrush);
    DeleteObject(hRgn);
}

void ModernDrawing::DrawGradient(HDC hdc, const RECT& rect, const DAWTheme::GradientColors& gradient, bool vertical)
{
 int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    
    if (vertical)
    {
      // Draw vertical gradient
        for (int y = 0; y < height; ++y)
   {
         float t = static_cast<float>(y) / height;
            
      int r = static_cast<int>(GetRValue(gradient.start) * (1 - t) + GetRValue(gradient.end) * t);
       int g = static_cast<int>(GetGValue(gradient.start) * (1 - t) + GetGValue(gradient.end) * t);
      int b = static_cast<int>(GetBValue(gradient.start) * (1 - t) + GetBValue(gradient.end) * t);
            
COLORREF color = RGB(r, g, b);
            HPEN hPen = CreatePen(PS_SOLID, 1, color);
          HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        
          MoveToEx(hdc, rect.left, rect.top + y, nullptr);
    LineTo(hdc, rect.right, rect.top + y);
            
     SelectObject(hdc, hOldPen);
 DeleteObject(hPen);
   }
    }
    else
    {
        // Draw horizontal gradient
     for (int x = 0; x < width; ++x)
        {
   float t = static_cast<float>(x) / width;
      
   int r = static_cast<int>(GetRValue(gradient.start) * (1 - t) + GetRValue(gradient.end) * t);
 int g = static_cast<int>(GetGValue(gradient.start) * (1 - t) + GetGValue(gradient.end) * t);
         int b = static_cast<int>(GetBValue(gradient.start) * (1 - t) + GetBValue(gradient.end) * t);
            
   COLORREF color = RGB(r, g, b);
    HPEN hPen = CreatePen(PS_SOLID, 1, color);
HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
      
          MoveToEx(hdc, rect.left + x, rect.top, nullptr);
  LineTo(hdc, rect.left + x, rect.bottom);
     
         SelectObject(hdc, hOldPen);
 DeleteObject(hPen);
      }
    }
}

void ModernDrawing::DrawModernButton(HDC hdc, const RECT& rect, const wchar_t* text, bool isHovered, bool isPressed, bool isRecord)
{
    // Determine gradient based on state
    DAWTheme::GradientColors gradient;
    if (isRecord)
    {
        gradient = DAWTheme::GRADIENT_BUTTON_RECORD;
    }
    else if (isPressed)
    {
 gradient = DAWTheme::GRADIENT_BUTTON_ACTIVE;
    }
    else if (isHovered)
    {
        gradient = DAWTheme::GRADIENT_BUTTON_HOVER;
    }
    else
    {
gradient = DAWTheme::GRADIENT_BUTTON_NORMAL;
    }
    
    // Draw gradient background
    DrawGradient(hdc, rect, gradient, true);
    
    // Draw rounded border
    HPEN hPen = CreatePen(PS_SOLID, 2, isPressed ? DAWTheme::COLOR_ACCENT_PRIMARY : DAWTheme::COLOR_SURFACE_OVERLAY);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    
    // Draw border
    HRGN hRgn = CreateRoundRectRgn(rect.left, rect.top, rect.right + 1, rect.bottom + 1, 4, 4);
    FrameRgn(hdc, hRgn, (HBRUSH)GetStockObject(HOLLOW_BRUSH), 2, 2);
    DeleteObject(hRgn);
    
    SelectObject(hdc, hOldPen);
  DeleteObject(hPen);
    
    // Draw text
    if (text)
 {
        DrawModernText(hdc, rect, text, DAWTheme::COLOR_TEXT_PRIMARY, false, true);
    }
}

void ModernDrawing::DrawModernSlider(HDC hdc, const RECT& rect, float value, const wchar_t* label)
{
    // Draw track background
    RECT trackRect = rect;
    trackRect.top = rect.top + (rect.bottom - rect.top) / 2 - 2;
    trackRect.bottom = rect.top + (rect.bottom - rect.top) / 2 + 2;
    
    DrawRoundedRect(hdc, trackRect, 2, DAWTheme::COLOR_SURFACE_OVERLAY, DAWTheme::COLOR_SURFACE_OVERLAY);
    
    // Draw filled portion
 RECT fillRect = trackRect;
    fillRect.right = trackRect.left + static_cast<int>((trackRect.right - trackRect.left) * value);
  
    if (fillRect.right > fillRect.left)
    {
        DrawRoundedRect(hdc, fillRect, 2, DAWTheme::COLOR_ACCENT_PRIMARY, DAWTheme::COLOR_ACCENT_PRIMARY);
    }
    
    // Draw thumb
    int thumbX = trackRect.left + static_cast<int>((trackRect.right - trackRect.left) * value);
    RECT thumbRect = { thumbX - 5, rect.top, thumbX + 5, rect.bottom };
    DrawRoundedRect(hdc, thumbRect, 3, DAWTheme::COLOR_ACCENT_PRIMARY, DAWTheme::COLOR_TEXT_PRIMARY, 1);
    
    // Draw label if provided
    if (label)
    {
        RECT labelRect = rect;
        labelRect.top = rect.bottom + 5;
        labelRect.bottom = labelRect.top + 20;
        DrawModernText(hdc, labelRect, label, DAWTheme::COLOR_TEXT_SECONDARY, false, true);
    }
}

void ModernDrawing::DrawModernText(HDC hdc, const RECT& rect, const wchar_t* text, COLORREF color, bool bold, bool centered)
{
    HFONT hFont;
    LOGFONTW lf = {};
    
    lf.lfHeight = -14; // 14 point font
    lf.lfWeight = bold ? FW_BOLD : FW_NORMAL;
    lf.lfQuality = ANTIALIASED_QUALITY;
    wcscpy_s(lf.lfFaceName, L"Segoe UI");
    
  hFont = CreateFontIndirectW(&lf);
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
    
    SetTextColor(hdc, color);
    SetBkMode(hdc, TRANSPARENT);
    
UINT format = DT_VCENTER | DT_SINGLELINE;
    if (centered)
    {
  format |= DT_CENTER;
    }
    else
    {
  format |= DT_LEFT;
    }
    
    DrawTextW(hdc, text, -1, (LPRECT)&rect, format);
    
    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
}

void ModernDrawing::DrawShadow(HDC hdc, const RECT& rect, int shadowSize, int intensity)
{
    for (int i = shadowSize; i > 0; --i)
    {
        int alpha = (intensity * i) / shadowSize;
        COLORREF shadowColor = RGB(0, 0, 0);
        
        // Create a subtle shadow by drawing multiple darkened rectangles
   HPEN hPen = CreatePen(PS_SOLID, 1, shadowColor);
      HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        
    // Draw shadow lines
        MoveToEx(hdc, rect.right + i, rect.top + i, nullptr);
        LineTo(hdc, rect.right + i, rect.bottom + i);
        
   MoveToEx(hdc, rect.right + i, rect.bottom + i, nullptr);
        LineTo(hdc, rect.left + i, rect.bottom + i);
        
    SelectObject(hdc, hOldPen);
      DeleteObject(hPen);
    }
}

HBRUSH ModernDrawing::CreateModernBrush(COLORREF color)
{
    return CreateSolidBrush(color);
}
