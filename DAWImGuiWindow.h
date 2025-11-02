#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <string>

// GLFW and OpenGL
#include <GLFW/glfw3.h>

// ImGui
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

// Forward declarations
class DAWApplication;
class SequencerEngine;
class AudioTrack;

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
    
    // Audio tracks
  std::vector<std::shared_ptr<AudioTrack>> audioTracks_;
    int selectedTrackIndex_;

    // Window state
    bool isRunning_ = true;
    int windowWidth_ = 1600;
    int windowHeight_ = 1000;
    std::string windowTitle_;

    // GLFW window
  GLFWwindow* glfwWindow_ = nullptr;
    bool glfwInitialized_ = false;

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
      
        // File dialog state
        bool showFileDialog = false;
 char filePathBuffer[512] = "";
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
    void DrawFileDialog();

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
    
    // Track management
    void OnAddTrack();
    void OnLoadAudio();
 void OnDeleteTrack();

    // Helper methods
    void SetupImGui();
    void CleanupImGui();
    void SetupStyle();
};
