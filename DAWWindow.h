#pragma once

#include <windows.h>
#include <gl/gl.h>
#include <memory>
#include <vector>
#include <map>
#include "DAWApplication.h"
#include "DAWTheme.h"
#include "ModernUILayout.h"
#include "ModernUI.h"

// MDI child window types
enum class MDIChildType
{
    MenuBar,      // Top menu bar
    Transport,    // Play/Pause/Stop/Record controls
    Sequencer,    // Main piano roll editor
    Inspector,    // Properties/settings panel
    Mixer,        // Mixer/channels panel
    Browser,      // Sample/file browser
    Properties    // Track properties
};

// Child window context data
struct MDIChildContext
{
    HWND hwnd = nullptr;
    MDIChildType type;
    HDC hdc = nullptr;
    HGLRC hglrc = nullptr;
    std::unique_ptr<UIManager> ui;
    bool visible = true;

    MDIChildContext(MDIChildType t = MDIChildType::Sequencer) : type(t) {}
};

// Modern MDI DAW Window
class DAWWindow
{
public:
    DAWWindow();
    ~DAWWindow();

    // Window creation and management
    bool Create(HINSTANCE hInstance, int nCmdShow);
    void Destroy();

    // Message handling
    LRESULT HandleMessage(UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMenuCommand(UINT commandId);

    // Get window handles
    HWND GetHandle() const { return hWnd_; }
    HWND GetMDIClientHandle() const { return hMDIClient_; }
    HWND GetChildWindow(MDIChildType type) const;

    // Window management
    void ShowChild(MDIChildType type, bool show = true);
    void SetChildVisible(MDIChildType type, bool visible);

    // Redraw the GUI
    void Redraw();
    void RedrawAll();  // Force all windows to repaint

private:
    // Render timer
    static constexpr UINT_PTR RENDER_TIMER_ID = 1;
    static constexpr UINT RENDER_TIMER_MS = 16;  // ~60 FPS

    // DAW Application instance
    std::shared_ptr<DAWApplication> daw_;

    HWND hWnd_;    // Main MDI parent window
    HWND hMDIClient_;  // MDI client window
    HINSTANCE hInstance_;
    HMENU hMainMenu_;     // Main menu bar

    // Legacy single-GL members (unused now but kept for compatibility)
  HDC hDC_;
    HGLRC hGLRC_;

    // Child window management
    std::map<MDIChildType, MDIChildContext> children_;
    std::map<HWND, MDIChildType> hwndToType_;

    // State tracking
    bool isPlaybackActive_ = false;
    bool isRecordingActive_ = false;
    float currentZoom_ = 1.0f;

    // Helpers
    void CreateMainMenuBar();
    void CreateMDIChildren();
    HWND CreateMDIChildWindow(const wchar_t* title, MDIChildType type, int x, int y, int w, int h);

    bool SetupPixelFormat(HDC hdc);

    // Rendering helpers
    void RenderChild(HWND child);
    void BuildChildUI(HWND child, MDIChildType type);

    // Menu item handlers
    void OnFileNew();
    void OnFileOpen();
 void OnFileSave();
    void OnFileSaveAs();
    void OnFileExit();
    void OnEditUndo();
  void OnEditRedo();
    void OnEditCut();
    void OnEditCopy();
    void OnEditPaste();
 void OnEditDelete();
    void OnEditSelectAll();
    void OnViewMenuBar();
    void OnViewTransport();
    void OnViewSequencer();
    void OnViewInspector();
    void OnViewMixer();
    void OnViewBrowser();
    void OnViewProperties();
    void OnHelpAbout();

    // Transport controls
    void OnPlayClick();
    void OnPauseClick();
    void OnStopClick();
    void OnRecordClick();
    void OnMetronomeClick();
    void OnSettingsClick();
    void OnAddTrackClick();
    void OnLoadAudioClick();
    void OnDeleteTrackClick();
    void OnLoopCheckClick();
    void OnSnapToggleClick();
    void OnZoomChange();
    void UpdateTempoDisplay();

    // Static callbacks
    static LRESULT CALLBACK ParentWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK ChildWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    static DAWWindow* GetInstance(HWND hWnd);
};

// Menu IDs
enum MenuCommandIds
{
    // File menu
    ID_FILE_NEW = 40001,
    ID_FILE_OPEN = 40002,
    ID_FILE_SAVE = 40003,
    ID_FILE_SAVEAS = 40004,
    ID_FILE_EXIT = 40005,

    // Edit menu
    ID_EDIT_UNDO = 40010,
    ID_EDIT_REDO = 40011,
    ID_EDIT_CUT = 40012,
    ID_EDIT_COPY = 40013,
    ID_EDIT_PASTE = 40014,
    ID_EDIT_DELETE = 40015,
 ID_EDIT_SELECTALL = 40016,

    // View menu
    ID_VIEW_MENUBAR = 40020,
    ID_VIEW_TRANSPORT = 40021,
    ID_VIEW_SEQUENCER = 40022,
    ID_VIEW_INSPECTOR = 40023,
    ID_VIEW_MIXER = 40024,
    ID_VIEW_BROWSER = 40025,
    ID_VIEW_PROPERTIES = 40026,

    // Help menu
    ID_HELP_ABOUT = 40030
};
