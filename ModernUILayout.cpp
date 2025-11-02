#include "ModernUILayout.h"
#include <algorithm>

ModernUILayout::LayoutRegions ModernUILayout::ComputeLayout(const RECT& clientArea)
{
    LayoutRegions layout = {};
    
 int width = clientArea.right - clientArea.left;
    int height = clientArea.bottom - clientArea.top;
    
    // Header bar - 60px with gradient and icons
    layout.headerBar = { clientArea.left, clientArea.top, clientArea.right, clientArea.top + 60 };
    
    // Transport bar - below header, 70px tall
    layout.transportBar = { clientArea.left, layout.headerBar.bottom, clientArea.right, layout.headerBar.bottom + 70 };
    
    // Left sidebar - 50px wide for tool icons
    int sidebarWidth = 50;
    layout.sidebarLeft = { clientArea.left, layout.transportBar.bottom, clientArea.left + sidebarWidth, clientArea.bottom - 30 };
    
    // Right inspector panel - 350px wide, collapsible
    int inspectorWidth = 350;
    layout.inspectorPanel = { clientArea.right - inspectorWidth, layout.transportBar.bottom, clientArea.right, clientArea.bottom - 30 };
    
    // Tracks panel - left of main workspace
    int tracksWidth = 200;
    layout.tracksPanel = { layout.sidebarLeft.right, layout.transportBar.bottom, layout.sidebarLeft.right + tracksWidth, clientArea.bottom - 30 };
 
    // Main workspace areas
    layout.workspaceTop = { layout.tracksPanel.right, layout.transportBar.bottom, layout.inspectorPanel.left, layout.tracksPanel.top + 300 };
    
    layout.workspaceMain = { layout.tracksPanel.right, layout.workspaceTop.bottom + 4, layout.inspectorPanel.left, layout.inspectorPanel.top };
 
    // Status bar - bottom, 30px tall
    layout.statusBar = { clientArea.left, clientArea.bottom - 30, clientArea.right, clientArea.bottom };
    
    return layout;
}

RECT ModernUILayout::CalculateGridPosition(
    const RECT& container,
    int column, int row,
    int totalColumns, int totalRows,
    int padding)
{
    int width = container.right - container.left;
    int height = container.bottom - container.top;
    
    int cellWidth = (width - padding * (totalColumns + 1)) / totalColumns;
    int cellHeight = (height - padding * (totalRows + 1)) / totalRows;
    
    RECT result = {
        container.left + padding + column * (cellWidth + padding),
      container.top + padding + row * (cellHeight + padding),
        container.left + padding + column * (cellWidth + padding) + cellWidth,
      container.top + padding + row * (cellHeight + padding) + cellHeight
    };
    
    return result;
}

void ModernUILayout::AlignHorizontal(
    std::vector<RECT>& rects,
    int totalWidth,
  int padding)
{
    if (rects.empty())
 return;
    
    int itemCount = static_cast<int>(rects.size());
    int availableWidth = totalWidth - padding * (itemCount + 1);
    int itemWidth = availableWidth / itemCount;
    
    for (int i = 0; i < itemCount; ++i)
    {
        int x = padding + i * (itemWidth + padding);
        int width = rects[i].right - rects[i].left;
        int height = rects[i].bottom - rects[i].top;
    
        rects[i] = { x, rects[i].top, x + itemWidth, rects[i].top + height };
    }
}

void ModernUILayout::AlignVertical(
    std::vector<RECT>& rects,
    int totalHeight,
    int padding)
{
    if (rects.empty())
        return;
    
    int itemCount = static_cast<int>(rects.size());
    int availableHeight = totalHeight - padding * (itemCount + 1);
    int itemHeight = availableHeight / itemCount;
    
    for (int i = 0; i < itemCount; ++i)
    {
        int y = padding + i * (itemHeight + padding);
     int width = rects[i].right - rects[i].left;
    int height = rects[i].bottom - rects[i].top;
        
      rects[i] = { rects[i].left, y, rects[i].left + width, y + itemHeight };
    }
}
