#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <string>

// Forward declarations
struct ImGuiContext;

class DAWApplication;
class SequencerEngine;

// Main application window using ImGui
class DAWImGuiWindow
{
public:
    DAWImGuiWindow();
  ~DAWImGuiWindow();

    // Window lifecycle
    bool Create(const char* title, int width, int height);
    void Destroy();
    bool IsRunning() const;

    // Main loop - call this each frame
    bool Update();
    void Render();

    // Application access
    DAWApplication* GetDAWApplication() { return daw_.get(); }

private:
    // Core application
    std::shared_ptr<DAWApplication> daw_;
    std::shared_ptr<SequencerEngine> sequencer_;

    // Window state
    bool isRunning_ = true;
    int windowWidth_ = 1600;
    int windowHeight_ = 1000;

    // ImGui context
    ImGuiContext* imguiContext_ = nullptr;

// UI state
    struct UIState
    {
   bool showMenuBar = true;
bool showTransport = true;
        bool showSequencer = true;
        bool showInspector = true;
        bool showMixer = false;
        bool showBrowser = false;
 bool showProperties = false;
        bool showAbout = false;
    } uiState_;

    // UI rendering methods
    void DrawMenuBar();
    void DrawTransport();
    void DrawSequencer();
    void DrawInspector();
    void DrawMixer();
    void DrawBrowser();
    void DrawProperties();
    void DrawAbout();

    // Menu handlers
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
    void OnHelpAbout();

    // Transport handlers
    void OnPlayClick();
    void OnPauseClick();
    void OnStopClick();
    void OnRecordClick();
    void OnMetronomeClick();

    // Helper methods
    void SetupImGui();
    void CleanupImGui();
    void SetupStyle();
};
