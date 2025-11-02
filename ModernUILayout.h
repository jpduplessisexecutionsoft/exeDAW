#pragma once

#include <windows.h>
#include <vector>

// Modern UI Layout System - Premium DAW Style
class ModernUILayout
{
public:
    struct ControlLayout
    {
      HWND hwnd;
    RECT rect;
        const wchar_t* tooltip;
        int id;
    };
    
    // Layout regions
    struct LayoutRegions
    {
        RECT headerBar;         // Top toolbar
     RECT transportBar;      // Transport controls
        RECT workspaceTop;    // Waveform/timeline area
        RECT workspaceMain;  // Main editing area
        RECT tracksPanel;     // Tracks list on left
     RECT inspectorPanel;    // Properties on right
        RECT statusBar;    // Bottom status bar
   RECT sidebarLeft;       // Left sidebar with tools
    };
    
    static LayoutRegions ComputeLayout(const RECT& clientArea);
    
    // Calculate positions for modern grid-based layout
    static RECT CalculateGridPosition(
        const RECT& container,
    int column, int row,
        int totalColumns, int totalRows,
        int padding = 8
    );
    
    // Create modern spacing and alignment
    static void AlignHorizontal(
        std::vector<RECT>& rects,
      int totalWidth,
        int padding = 8
    );
    
    static void AlignVertical(
     std::vector<RECT>& rects,
     int totalHeight,
        int padding = 8
    );
};
