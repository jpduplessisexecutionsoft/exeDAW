#pragma once

#include <windows.h>
#include <array>

// Modern color palette inspired by premium DAWs and web design
struct DAWTheme
{
    // Primary Colors - Deep Space Theme
    static constexpr COLORREF COLOR_DARK_BG = RGB(15, 15, 20);           // Nearly black background
  static constexpr COLORREF COLOR_SURFACE = RGB(25, 25, 32); // Main surface
    static constexpr COLORREF COLOR_SURFACE_ELEVATED = RGB(35, 35, 45);   // Elevated surfaces
    static constexpr COLORREF COLOR_SURFACE_OVERLAY = RGB(45, 45, 55);    // Overlays and borders
 
    // Accent Colors - Vibrant Modern
    static constexpr COLORREF COLOR_ACCENT_PRIMARY = RGB(88, 166, 255);   // Bright blue
    static constexpr COLORREF COLOR_ACCENT_SECONDARY = RGB(255, 107, 107); // Vibrant red
    static constexpr COLORREF COLOR_ACCENT_TERTIARY = RGB(76, 175, 80);   // Fresh green
    static constexpr COLORREF COLOR_ACCENT_WARNING = RGB(255, 193, 7);    // Warning yellow
    
    // State Colors
    static constexpr COLORREF COLOR_HOVER = RGB(65, 65, 80);
    static constexpr COLORREF COLOR_ACTIVE = RGB(88, 166, 255);
    static constexpr COLORREF COLOR_DISABLED = RGB(40, 40, 50);
    
    // Text Colors
    static constexpr COLORREF COLOR_TEXT_PRIMARY = RGB(240, 240, 245);    // Near white
    static constexpr COLORREF COLOR_TEXT_SECONDARY = RGB(180, 180, 190);  // Medium gray
    static constexpr COLORREF COLOR_TEXT_TERTIARY = RGB(120, 120, 130);   // Dark gray
  
    // Special Colors for Audio
    static constexpr COLORREF COLOR_WAVEFORM = RGB(100, 200, 255);        // Cyan blue
    static constexpr COLORREF COLOR_WAVEFORM_PEAK = RGB(255, 80, 80);     // Red peak
    static constexpr COLORREF COLOR_GRID_MAJOR = RGB(55, 55, 70);
    static constexpr COLORREF COLOR_GRID_MINOR = RGB(40, 40, 55);
    static constexpr COLORREF COLOR_PLAYHEAD = RGB(88, 166, 255);
    
    // Gradients for modern look
    struct GradientColors
    {
        COLORREF start;
        COLORREF end;
    };
    
    static constexpr GradientColors GRADIENT_BUTTON_NORMAL = {RGB(50, 50, 65), RGB(35, 35, 50)};
    static constexpr GradientColors GRADIENT_BUTTON_HOVER = {RGB(70, 70, 90), RGB(55, 55, 75)};
    static constexpr GradientColors GRADIENT_BUTTON_ACTIVE = {RGB(88, 166, 255), RGB(65, 140, 230)};
    static constexpr GradientColors GRADIENT_BUTTON_RECORD = {RGB(255, 80, 80), RGB(220, 60, 60)};
};

// Utility class for modern drawing operations
class ModernDrawing
{
public:
    // Draw a rounded rectangle
    static void DrawRoundedRect(HDC hdc, const RECT& rect, int radius, COLORREF fillColor, COLORREF borderColor = RGB(0, 0, 0), int borderWidth = 1);
    
    // Draw a gradient fill
    static void DrawGradient(HDC hdc, const RECT& rect, const DAWTheme::GradientColors& gradient, bool vertical = true);
  
    // Draw a modern button
  static void DrawModernButton(HDC hdc, const RECT& rect, const wchar_t* text, bool isHovered, bool isPressed, bool isRecord = false);
    
    // Draw a modern slider/knob
    static void DrawModernSlider(HDC hdc, const RECT& rect, float value, const wchar_t* label = nullptr);
    
    // Draw text with modern styling
static void DrawModernText(HDC hdc, const RECT& rect, const wchar_t* text, COLORREF color, bool bold = false, bool centered = true);
    
    // Draw shadow effect
    static void DrawShadow(HDC hdc, const RECT& rect, int shadowSize = 5, int intensity = 80);
    
    // Create modern HBRUSH
    static HBRUSH CreateModernBrush(COLORREF color);
};
