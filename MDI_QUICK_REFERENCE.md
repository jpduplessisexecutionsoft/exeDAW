# MDI Windows & Menu Bar - Quick Reference

## MDI Child Windows (7 Total)

| Window | Type | Default | Purpose |
|--------|------|---------|---------|
| Menu Bar | MenuBar | Visible | Standard application menu |
| Transport | Transport | Visible | Play/Record/Stop controls |
| Sequencer | Sequencer | Visible | Main piano roll editor |
| Inspector | Inspector | Visible | Properties/settings panel |
| Mixer | Mixer | Hidden | Channel mixing console |
| Browser | Browser | Hidden | Sample/file browser |
| Properties | Properties | Hidden | Advanced track properties |

---

## Menu Structure

### File (5 items)
```
New        Ctrl+N
Open       Ctrl+O
Save       Ctrl+S
Save As... Ctrl+Shift+S
?????????????????
Exit       Alt+F4
```

### Edit (7 items)
```
Undo Ctrl+Z
Redo       Ctrl+Y
?????????????????
Cut        Ctrl+X
Copy     Ctrl+C
Paste      Ctrl+V
Delete     Del
?????????????????
Select All Ctrl+A
```

### View (7 items)
```
Menu Bar
Transport
Sequencer
Inspector
Mixer
Browser
Properties
```

### Help (1 item)
```
About
```

**Total: 23 Menu Items**

---

## Accessing Windows Programmatically

### Show a Window
```cpp
dawWindow->ShowChild(MDIChildType::Mixer, true);
```

### Hide a Window
```cpp
dawWindow->ShowChild(MDIChildType::Browser, false);
```

### Get Window Handle
```cpp
HWND hSequencer = dawWindow->GetChildWindow(MDIChildType::Sequencer);
```

### Check Visibility
```cpp
auto& ctx = children_[MDIChildType::Mixer];
if (ctx.visible) { /* window is visible */ }
```

---

## Menu Command IDs

### File Commands (40001-40005)
- `ID_FILE_NEW` = 40001
- `ID_FILE_OPEN` = 40002
- `ID_FILE_SAVE` = 40003
- `ID_FILE_SAVEAS` = 40004
- `ID_FILE_EXIT` = 40005

### Edit Commands (40010-40016)
- `ID_EDIT_UNDO` = 40010
- `ID_EDIT_REDO` = 40011
- `ID_EDIT_CUT` = 40012
- `ID_EDIT_COPY` = 40013
- `ID_EDIT_PASTE` = 40014
- `ID_EDIT_DELETE` = 40015
- `ID_EDIT_SELECTALL` = 40016

### View Commands (40020-40026)
- `ID_VIEW_MENUBAR` = 40020
- `ID_VIEW_TRANSPORT` = 40021
- `ID_VIEW_SEQUENCER` = 40022
- `ID_VIEW_INSPECTOR` = 40023
- `ID_VIEW_MIXER` = 40024
- `ID_VIEW_BROWSER` = 40025
- `ID_VIEW_PROPERTIES` = 40026

### Help Commands (40030)
- `ID_HELP_ABOUT` = 40030

---

## Adding a New Menu Item

### 1. Add Command ID in DAWWindow.h
```cpp
enum MenuCommandIds
{
    // Add new ID
    ID_FILE_RECENT = 40006,
    // ...
};
```

### 2. Add Menu Item in CreateMainMenuBar()
```cpp
AppendMenu(hFileMenu, MFT_STRING, ID_FILE_RECENT, L"&Recent");
```

### 3. Add Handler Switch Case in HandleMenuCommand()
```cpp
case ID_FILE_RECENT:
    OnFileRecent();
    break;
```

### 4. Implement Handler
```cpp
void DAWWindow::OnFileRecent() { /* implementation */ }
```

---

## Child Window Implementation Pattern

### Per-Child Context
```cpp
struct MDIChildContext
{
 HWND hwnd;        // Window handle
    MDIChildType type;  // Window type
    HDC hdc;     // Device context
    HGLRC hglrc;        // OpenGL context
    std::unique_ptr<UIManager> ui;  // UI manager
    bool visible;       // Visibility flag
};
```

### Each Child Has
- ? Own OpenGL device context (HDC)
- ? Own OpenGL rendering context (HGLRC)
- ? Own UI manager instance
- ? Independent rendering pipeline
- ? Visibility toggle capability

---

## Layout Zones

### Screen Layout (Default)
```
???????????????????????????????????????????
? Menu Bar (1600x32)          ?
???????????????????????????????????????????
? Transport (1600x54)         ?
???????????????????????????????????????????
?     ?  ?
?   Sequencer          ?   Inspector      ?
?  (1200x600)       ?(390x600)      ?
?      ?  ?
???????????????????????????????????????????
? [Mixer - Hidden]        ?
???????????????????????????????????????????
```

### Z-Order (Front to Back)
1. Properties (hidden)
2. Browser (hidden)
3. Mixer (hidden)
4. Inspector (visible)
5. Sequencer (visible)
6. Transport (visible)
7. Menu Bar (visible)

---

## Event Flow

### Menu Selection
```
User clicks File > Open
   ?
WM_COMMAND message
    ?
ParentWindowProc()
  ?
HandleMessage(WM_COMMAND)
         ?
HandleMenuCommand(ID_FILE_OPEN)
         ?
OnFileOpen()
```

### Window Visibility Toggle
```
User clicks View > Mixer
         ?
WM_COMMAND with ID_VIEW_MIXER
     ?
OnViewMixer()
     ?
ShowChild(MDIChildType::Mixer, true)
         ?
ShowWindow(hwnd, SW_SHOW)
```

---

## Rendering Pipeline

### Per-Frame Render
```cpp
for (each child window)
{
    if (child.visible)
    {
        RenderChild(child.hwnd)
        {
            wglMakeCurrent(child.hdc, child.hglrc)
      // OpenGL rendering
     if (child.ui) child.ui->Render()
    SwapBuffers(child.hdc)
}
    }
}
```

---

## Window Messages Handled

| Message | Handler | Purpose |
|---------|---------|---------|
| `WM_COMMAND` | HandleMenuCommand | Menu selections |
| `WM_CREATE` | HandleMessage | Window creation |
| `WM_SIZE` | HandleMessage | Resize handling |
| `WM_DESTROY` | HandleMessage | Cleanup |
| `WM_PAINT` | ChildWindowProc | Painting |
| `WM_LBUTTONDOWN` | HandleMessage | Mouse click |
| `WM_LBUTTONUP` | HandleMessage | Mouse release |
| `WM_MOUSEMOVE` | HandleMessage | Mouse movement |

---

## Hidden/Show Toggle via View Menu

All View menu items now toggle window visibility:

```cpp
void DAWWindow::OnViewMixer()
{
    ShowChild(MDIChildType::Mixer, true);  // Always show
    // Future: Could toggle: !IsWindowVisible(GetChildWindow(...))
}
```

---

## Professional Features Added

? Standard application menu structure
? Keyboard shortcuts for all common operations
? Professional menu separators
? About dialog
? Multiple independent rendering contexts
? Per-window visibility control
? Clean command routing
? Extensible handler pattern

---

## Keyboard Shortcuts Available

- **Ctrl+N** ? File > New
- **Ctrl+O** ? File > Open
- **Ctrl+S** ? File > Save
- **Ctrl+Shift+S** ? File > Save As
- **Alt+F4** ? File > Exit
- **Ctrl+Z** ? Edit > Undo
- **Ctrl+Y** ? Edit > Redo
- **Ctrl+X** ? Edit > Cut
- **Ctrl+C** ? Edit > Copy
- **Ctrl+V** ? Edit > Paste
- **Del** ? Edit > Delete
- **Ctrl+A** ? Edit > Select All

---

## Architecture Benefits

? **Separation of Concerns**
   - Each window manages itself
   - Independent rendering
   - Isolated UI state

? **Extensibility**
   - Easy to add new windows
   - Easy to add new menu items
 - Clear handler pattern

? **Professional Quality**
   - Standard menu layout
   - Proper keyboard shortcuts
   - Clean command routing

? **Maintainability**
   - Well-organized code
   - Clear naming conventions
   - Documented patterns

---

## Future Enhancements Ready

- Add accelerator keys (keyboard input)
- Add toolbar buttons (linked to menu)
- Add status bar with menu tips
- Add context menus (right-click)
- Add window position saving/restoring
- Add dockable panels
- Add custom layouts

All frameworks are in place for these features!

