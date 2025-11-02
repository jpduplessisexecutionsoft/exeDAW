# ImGui Refactor - Quick Start Guide

## ?? Get Started in 5 Minutes

---

## Prerequisites

- Visual Studio 2022
- vcpkg installed at C:\vcpkg
- Git (already configured in your workspace)

---

## Step 1: Install Dependencies (2 minutes)

### Option A: Automatic (Recommended)
```bash
# Run from project root
install_dependencies.bat
```

### Option B: Manual
```bash
cd C:\vcpkg
.\vcpkg install glfw3:x64-windows
.\vcpkg install imgui:x64-windows imgui[glfw-binding]:x64-windows imgui[opengl3-binding]:x64-windows
.\vcpkg install glew:x64-windows
```

---

## Step 2: Configure Visual Studio (2 minutes)

### 1. Open Project Properties
- Right-click project ? Properties
- Ensure platform is **x64**

### 2. Set Include Directory
- Go to: Configuration Properties ? VC++ Directories
- Add to "Include Directories":
```
C:\vcpkg\installed\x64-windows\include
```

### 3. Set Library Directory
- Add to "Library Directories":
```
C:\vcpkg\installed\x64-windows\lib
C:\vcpkg\installed\x64-windows\debug\lib
```

### 4. Link Libraries
- Go to: Configuration Properties ? Linker ? Input
- Add to "Additional Dependencies":
```
imgui.lib
glfw3.lib
glew32.lib
opengl32.lib
user32.lib
gdi32.lib
shell32.lib
```

### 5. Apply & OK

---

## Step 3: Build & Run (1 minute)

```
Ctrl+Shift+B Build Solution
F5        Start Debugging
```

---

## ? You Should See

1. **Application window** - 1600x1000 pixels
2. **Menu bar** - File, Edit, View, Help
3. **Transport controls** - Play, Pause, Stop, Record buttons
4. **Sequencer window** - Track list
5. **Inspector panel** - Properties area
6. **All windows dockable** - Drag tabs to rearrange

---

## ?? Try It

- Click **File ? New** (prints to console for now)
- Click **Play** button (triggers audio playback)
- Drag window tabs to dock/undock
- Right-click window title to see options

---

## Troubleshooting

| Problem | Solution |
|---------|----------|
| "Cannot open include file" | Check Include Directories path |
| "Unresolved external symbol" | Verify Library Directories and libs |
| "Black window" | Check graphics driver supports OpenGL 3.3+ |
| Won't start | Check build output for error messages |

---

## Next Steps

- [ ] **Read** IMGUI_REFACTOR_GUIDE.md (detailed architecture)
- [ ] **Read** VISUAL_STUDIO_SETUP.md (detailed configuration)
- [ ] **Test** menu items and buttons
- [ ] **Modify** UI as needed
- [ ] **Add** audio functionality

---

## File Locations

| File | Purpose |
|------|---------|
| `DAWImGuiWindow.h` | Main ImGui window class |
| `DAWImGuiWindow.cpp` | Window implementation |
| `exeDAW.cpp` | Entry point (simplified main) |

---

## What's Implemented

? Menu bar (File, Edit, View, Help)
? Transport controls (Play, Pause, Stop, Record)
? Sequencer window (Track list)
? Inspector panel
? Mixer window
? Browser window
? Properties panel
? About dialog

---

## Architecture (Simple Overview)

```cpp
main()
  ?
DAWImGuiWindow::Create()
  ?? Initialize GLFW window
  ?? Setup OpenGL context
  ?? Initialize ImGui
  ?? Create channels
  ?
Main Loop:
  ?? DAWImGuiWindow::Update() - Process input, draw UI
  ?? DAWImGuiWindow::Render() - OpenGL rendering
```

---

## Commands Cheat Sheet

| Action | Command |
|--------|---------|
| Build | Ctrl+Shift+B |
| Debug | F5 |
| Stop | Shift+F5 |
| Clean | Build ? Clean Solution |

---

## Common Tasks

### Add a Menu Item
```cpp
// In DrawMenuBar()
if (ImGui::MenuItem("Item Name", "Ctrl+X"))
    OnMenuItemClick();
```

### Add a Window
```cpp
// In DrawMenuBar() - add to View menu
ImGui::MenuItem("My Panel", nullptr, &uiState_.showMyPanel);

// Add draw method
void DAWImGuiWindow::DrawMyPanel()
{
 if (ImGui::Begin("My Panel", &uiState_.showMyPanel))
    {
        ImGui::Text("Content here");
        ImGui::End();
 }
}

// Call in Update()
if (uiState_.showMyPanel) DrawMyPanel();
```

### Add a Button
```cpp
if (ImGui::Button("My Button", ImVec2(100, 30)))
    OnButtonClicked();
```

---

## Tips & Tricks

### Docking Windows
- Drag window tab to dock with another
- Drag window title bar to undock
- Right-click tab for more options

### Resizing Panels
- Grab edge between panels to resize
- Double-click edge to auto-fit

### Keyboard Shortcuts
- Ctrl+N ? New
- Ctrl+O ? Open
- Ctrl+S ? Save
- Ctrl+Z ? Undo

---

## What's Next

**Immediate:**
- ? Install dependencies
- ? Configure VS
- ? Build & run
- ? Test UI

**Short-term:**
- Add real audio playback
- Connect MIDI input
- Implement save/load
- Add keyboard shortcuts

**Medium-term:**
- Piano roll rendering
- Drag & drop MIDI
- Mixer channel rendering
- Waveform visualization

---

## Still Having Issues?

1. **Read** VISUAL_STUDIO_SETUP.md (detailed troubleshooting)
2. **Check** build output window (Ctrl+Alt+O)
3. **Verify** vcpkg packages installed: `vcpkg list | grep -E "glfw|imgui|glew"`
4. **Restart** Visual Studio

---

## Success Criteria

When you see **all of these**, you're good to go:

? Application window opens
? Menu bar visible with File/Edit/View/Help
? Transport panel with buttons
? Multiple dockable windows
? No error messages in output
? Buttons are clickable
? Windows can be dragged/docked

---

**Ready to go! Follow Step 1 above to begin.** ??

