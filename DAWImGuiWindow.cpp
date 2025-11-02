#include "DAWImGuiWindow.h"
#include "DAWApplication.h"
#include "SequencerEngine.h"
#include "SequencerModel.h"
#include "SequencerView.h"
#include "AudioTrack.h"
#include <cmath>
#include <cstdio>
#include <Windows.h> // For file dialog

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h> // For glfwGetWin32Window

DAWImGuiWindow::DAWImGuiWindow()
    : selectedTrackIndex_(-1)
{
    daw_ = std::make_shared<DAWApplication>();
    sequencer_ = std::make_shared<SequencerEngine>();
}

DAWImGuiWindow::~DAWImGuiWindow()
{
 // Explicitly call Destroy to ensure proper cleanup order
    if (glfwWindow_ || ImGui::GetCurrentContext())
    {
        Destroy();
    }
}

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

bool DAWImGuiWindow::Create(const char* title, int width, int height)
{
    // Validate input parameters
    if (!title || width <= 0 || height <= 0)
{
        fprintf(stderr, "Invalid window parameters\n");
        return false;
    }

    windowWidth_ = width;
    windowHeight_ = height;
 windowTitle_ = title;

    // Setup GLFW error callback
    glfwSetErrorCallback(glfw_error_callback);

    // Initialize GLFW
  if (!glfwInit())
 {
        fprintf(stderr, "Failed to initialize GLFW\n");
    return false;
    }
  glfwInitialized_ = true;

    // OpenGL 3.3 Core Profile
 glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create GLFW window
    glfwWindow_ = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!glfwWindow_)
    {
        fprintf(stderr, "Failed to create GLFW window\n");
     if (glfwInitialized_)
        {
   glfwTerminate();
            glfwInitialized_ = false;
        }
  return false;
    }

    glfwMakeContextCurrent(glfwWindow_);
    glfwSwapInterval(1); // Enable vsync

    // Setup ImGui
    SetupImGui();

    // Initialize DAW application
    if (!daw_)
    {
        fprintf(stderr, "Failed to create DAWApplication instance\n");
  return false;
    }

    try
    {
        daw_->Initialize();
    }
    catch (const std::exception& e)
    {
    fprintf(stderr, "DAWApplication initialization failed: %s\n", e.what());
  return false;
    }

    // Create default sequencer channels
    if (sequencer_)
    {
   try
        {
    sequencer_->CreateChannel("Kick");
     sequencer_->CreateChannel("Snare");
      sequencer_->CreateChannel("Hi-Hat");
         sequencer_->CreateChannel("Bass");
        }
  catch (const std::exception& e)
        {
    fprintf(stderr, "Warning: Failed to create sequencer channels: %s\n", e.what());
  }
    }

    isRunning_ = true;
    printf("exeDAW: ImGui/GLFW window created successfully\n");
    
    return true;
}

void DAWImGuiWindow::Destroy()
{
    isRunning_ = false;
  
    // Cleanup ImGui first (while GLFW context is still valid)
    if (glfwWindow_)
    {
        glfwMakeContextCurrent(glfwWindow_);
        CleanupImGui();
  }
    
    // Then destroy GLFW window
    if (glfwWindow_)
    {
     glfwDestroyWindow(glfwWindow_);
    glfwWindow_ = nullptr;
    }
    
    // Finally terminate GLFW (only if we initialized it)
    if (glfwInitialized_)
    {
  glfwTerminate();
        glfwInitialized_ = false;
    }
}

bool DAWImGuiWindow::IsRunning() const
{
    return isRunning_ && glfwWindow_ && !glfwWindowShouldClose(glfwWindow_);
}

void DAWImGuiWindow::SetupImGui()
{
  // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
 
    // Note: Docking and viewports may not be available in all ImGui builds
    // Uncomment if using docking branch:
// io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

  // Setup Platform/Renderer backends
ImGui_ImplGlfw_InitForOpenGL(glfwWindow_, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Setup DAW style
    SetupStyle();
}

void DAWImGuiWindow::CleanupImGui()
{
    // Shutdown ImGui backends in correct order
    if (ImGui::GetCurrentContext())
    {
        ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}

void DAWImGuiWindow::SetupStyle()
{
    // Dark theme optimized for audio production (like Ableton/Cubase)
    ImGui::StyleColorsDark();
    
    ImGuiStyle& style = ImGui::GetStyle();
    
    // Professional DAW colors
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.18f, 0.18f, 0.19f, 1.00f);
  colors[ImGuiCol_ChildBg] = ImVec4(0.14f, 0.14f, 0.15f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.12f, 0.12f, 0.13f, 1.00f);
    
    colors[ImGuiCol_Border] = ImVec4(0.28f, 0.28f, 0.30f, 1.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.24f, 0.24f, 0.25f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.30f, 0.30f, 0.32f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.35f, 0.35f, 0.37f, 1.00f);
    
    colors[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.12f, 0.13f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.16f, 0.17f, 1.00f);
  colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.10f, 0.11f, 1.00f);
  
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.15f, 1.00f);
    
    colors[ImGuiCol_Button] = ImVec4(0.28f, 0.28f, 0.30f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.35f, 0.35f, 0.37f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.40f, 0.40f, 0.42f, 1.00f);
    
    colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.50f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
 
    colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.18f, 0.19f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.30f, 0.30f, 0.32f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.24f, 0.24f, 0.25f, 1.00f);
    
    // Rounding for modern look
    style.WindowRounding = 0.0f;
  style.ChildRounding = 0.0f;
    style.FrameRounding = 3.0f;
    style.PopupRounding = 3.0f;
 style.ScrollbarRounding = 3.0f;
    style.GrabRounding = 3.0f;
    style.TabRounding = 3.0f;
    
    // Padding and spacing
    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.FramePadding = ImVec2(6.0f, 4.0f);
    style.ItemSpacing = ImVec2(8.0f, 4.0f);
    style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
    
    // Borders
    style.WindowBorderSize = 1.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
}

bool DAWImGuiWindow::Update()
{
    if (!IsRunning())
        return false;

    // Poll events
    glfwPollEvents();

// Start ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Draw all UI elements (no docking - simple window layout)
    if (uiState_.showMenuBar) DrawMenuBar();
    if (uiState_.showTransport) DrawTransport();
    if (uiState_.showSequencer) DrawSequencer();
    if (uiState_.showInspector) DrawInspector();
    if (uiState_.showMixer) DrawMixer();
    if (uiState_.showBrowser) DrawBrowser();
    if (uiState_.showProperties) DrawProperties();
    if (uiState_.showAbout) DrawAbout();

    return true;
}

void DAWImGuiWindow::Render()
{
  // Rendering
    ImGui::Render();
    
    int display_w, display_h;
 glfwGetFramebufferSize(glfwWindow_, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.18f, 0.18f, 0.19f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);
    
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(glfwWindow_);
}

void DAWImGuiWindow::DrawMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
     // File Menu
        if (ImGui::BeginMenu("File"))
        {
      if (ImGui::MenuItem("New", "Ctrl+N")) OnFileNew();
            if (ImGui::MenuItem("Open...", "Ctrl+O")) OnFileOpen();
  ImGui::Separator();
            if (ImGui::MenuItem("Save", "Ctrl+S")) OnFileSave();
          if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) OnFileSaveAs();
    ImGui::Separator();
       if (ImGui::MenuItem("Exit", "Alt+F4")) OnFileExit();
      ImGui::EndMenu();
        }

        // Edit Menu
        if (ImGui::BeginMenu("Edit"))
      {
     if (ImGui::MenuItem("Undo", "Ctrl+Z")) OnEditUndo();
        if (ImGui::MenuItem("Redo", "Ctrl+Y")) OnEditRedo();
      ImGui::Separator();
          if (ImGui::MenuItem("Cut", "Ctrl+X")) OnEditCut();
  if (ImGui::MenuItem("Copy", "Ctrl+C")) OnEditCopy();
      if (ImGui::MenuItem("Paste", "Ctrl+V")) OnEditPaste();
if (ImGui::MenuItem("Delete", "Del")) OnEditDelete();
    ImGui::Separator();
         if (ImGui::MenuItem("Select All", "Ctrl+A")) OnEditSelectAll();
      ImGui::EndMenu();
        }

        // View Menu
    if (ImGui::BeginMenu("View"))
   {
   ImGui::MenuItem("Transport", nullptr, &uiState_.showTransport);
     ImGui::MenuItem("Sequencer", nullptr, &uiState_.showSequencer);
         ImGui::MenuItem("Inspector", nullptr, &uiState_.showInspector);
      ImGui::MenuItem("Mixer", nullptr, &uiState_.showMixer);
       ImGui::MenuItem("Browser", nullptr, &uiState_.showBrowser);
  ImGui::MenuItem("Properties", nullptr, &uiState_.showProperties);
            ImGui::EndMenu();
   }

        // Help Menu
        if (ImGui::BeginMenu("Help"))
  {
      if (ImGui::MenuItem("About")) OnHelpAbout();
          ImGui::EndMenu();
    }

        ImGui::EndMainMenuBar();
    }
}

void DAWImGuiWindow::DrawTransport()
{
    ImGui::Begin("Transport", &uiState_.showTransport, ImGuiWindowFlags_NoCollapse);

    // Transport buttons in a row
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f);
    
    // Play button (green when playing)
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.7f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.8f, 0.4f, 1.0f));
    if (ImGui::Button("Play", ImVec2(80, 30))) OnPlayClick();
    ImGui::PopStyleColor(3);
    
    ImGui::SameLine();
    
    // Pause button
    if (ImGui::Button("Pause", ImVec2(80, 30))) OnPauseClick();
  
    ImGui::SameLine();
    
  // Stop button (red)
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.2f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.7f, 0.3f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.4f, 0.4f, 1.0f));
    if (ImGui::Button("Stop", ImVec2(80, 30))) OnStopClick();
    ImGui::PopStyleColor(3);
  
 ImGui::SameLine();
    
    // Record button (red circle)
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
    if (ImGui::Button("Record", ImVec2(80, 30))) OnRecordClick();
    ImGui::PopStyleColor(3);
    
    ImGui::SameLine();
    ImGui::Spacing();
  ImGui::SameLine();
    
    // Metronome toggle
    if (ImGui::Button("Metronome", ImVec2(100, 30))) OnMetronomeClick();
    
    ImGui::PopStyleVar();
    
    ImGui::Separator();
    
    // Tempo control
    static float tempo = 120.0f;
    ImGui::Text("Tempo:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(200);
    if (ImGui::SliderFloat("##tempo", &tempo, 30.0f, 300.0f, "%.1f BPM"))
 {
        // Update transport tempo
    }
    
    // Time signature
    ImGui::SameLine();
    ImGui::Spacing();
  ImGui::SameLine();
    ImGui::Text("4/4");
    
 // Position display
    ImGui::Separator();
    ImGui::Text("Position: 00:00:00.000");
    
ImGui::End();
}

void DAWImGuiWindow::DrawSequencer()
{
    ImGui::Begin("Sequencer", &uiState_.showSequencer, ImGuiWindowFlags_NoCollapse);

    // Toolbar
    if (ImGui::Button("Add Track"))
 {
        OnAddTrack();
    }
    
 ImGui::SameLine();
    
 if (ImGui::Button("Load Audio"))
    {
        OnLoadAudio();
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("Delete Track"))
    {
        OnDeleteTrack();
    }
    
    ImGui::SameLine();
    ImGui::Spacing();
    ImGui::SameLine();
    
 // Snap controls
  static int snapValue = 16;
    ImGui::Text("Snap:");
    ImGui::SameLine();
  ImGui::SetNextItemWidth(80);
    ImGui::Combo("##snap", &snapValue, "1/4\0001/8\0001/16\0001/32\0");
    
    ImGui::Separator();
    
    // Track list
    ImGui::BeginChild("TrackList", ImVec2(250, 0), true);
    ImGui::Text("Audio Tracks (%zu)", audioTracks_.size());
    ImGui::Separator();
    
    for (size_t i = 0; i < audioTracks_.size(); ++i)
    {
auto& track = audioTracks_[i];
     char label[128];
      snprintf(label, sizeof(label), "%s###track%zu", track->GetName().c_str(), i);
        
     bool isSelected = (static_cast<int>(i) == selectedTrackIndex_);
     if (ImGui::Selectable(label, isSelected))
    {
    selectedTrackIndex_ = static_cast<int>(i);
        }
 
      // Show track info
        if (!track->IsEmpty())
        {
       ImGui::SameLine();
      ImGui::TextColored(ImVec4(0.4f, 0.8f, 0.4f, 1.0f), "[%.1fs]", track->GetDurationSeconds());
  }
     
        if (track->IsMuted())
{
      ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "[M]");
   }
  
  if (track->IsSoloed())
  {
ImGui::SameLine();
      ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.4f, 1.0f), "[S]");
        }
    }
    
    ImGui::EndChild();
    
    ImGui::SameLine();
    
    // Waveform display area
    ImGui::BeginChild("WaveformView", ImVec2(0, 0), true);
    
    if (selectedTrackIndex_ >= 0 && selectedTrackIndex_ < static_cast<int>(audioTracks_.size()))
    {
 auto& track = audioTracks_[selectedTrackIndex_];
        
        if (!track->IsEmpty())
        {
   ImDrawList* draw_list = ImGui::GetWindowDrawList();
         ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
     ImVec2 canvas_size = ImGui::GetContentRegionAvail();
  
      // Draw background
            draw_list->AddRectFilled(canvas_pos,
       ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y),
     IM_COL32(20, 20, 22, 255));
            
  // Get waveform data
    std::vector<float> minVals, maxVals;
          uint32_t numPoints = static_cast<uint32_t>(canvas_size.x);
      track->GetPeakAmplitudes(minVals, maxVals, numPoints);
        
       // Draw waveform
          float centerY = canvas_pos.y + canvas_size.y * 0.5f;
     float scale = canvas_size.y * 0.4f;
         
      for (size_t i = 0; i < minVals.size(); ++i)
            {
  float x = canvas_pos.x + i;
      float y1 = centerY - maxVals[i] * scale;
       float y2 = centerY - minVals[i] * scale;
          
    draw_list->AddLine(
      ImVec2(x, y1),
       ImVec2(x, y2),
          IM_COL32(100, 180, 255, 255),
       1.0f);
        }
      
 // Draw center line
        draw_list->AddLine(
          ImVec2(canvas_pos.x, centerY),
         ImVec2(canvas_pos.x + canvas_size.x, centerY),
             IM_COL32(80, 80, 80, 255),
             1.0f);
            
     // Draw info text
      char info[256];
      snprintf(info, sizeof(info),
     "%s - %.2fs - %dHz %dch - Peak: %.2f RMS: %.2f",
    track->GetName().c_str(),
      track->GetDurationSeconds(),
     track->GetSampleRate(),
     track->GetNumChannels(),
     track->GetPeakAmplitude(),
 track->GetRMSAmplitude());
      
     ImVec2 text_pos = ImVec2(canvas_pos.x + 10, canvas_pos.y + 10);
     draw_list->AddText(text_pos, IM_COL32(220, 220, 220, 255), info);
  }
  else
     {
        ImVec2 text_pos = ImVec2(
           ImGui::GetWindowPos().x + ImGui::GetWindowSize().x * 0.5f - 80,
     ImGui::GetWindowPos().y + ImGui::GetWindowSize().y * 0.5f);
         ImGui::SetCursorScreenPos(text_pos);
       ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "No audio loaded");
     }
 }
    else
    {
   ImVec2 text_pos = ImVec2(
ImGui::GetWindowPos().x + ImGui::GetWindowSize().x * 0.5f - 100,
      ImGui::GetWindowPos().y + ImGui::GetWindowSize().y * 0.5f);
        ImGui::SetCursorScreenPos(text_pos);
  ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Select a track to view waveform");
    }
    
  ImGui::EndChild();
    
    // File dialog
    if (uiState_.showFileDialog)
    {
   DrawFileDialog();
    }
    
 ImGui::End();
}

void DAWImGuiWindow::DrawInspector()
{
  ImGui::Begin("Inspector", &uiState_.showInspector, ImGuiWindowFlags_NoCollapse);

    ImGui::Text("Track Properties");
    ImGui::Separator();
    
    if (selectedTrackIndex_ >= 0 && selectedTrackIndex_ < static_cast<int>(audioTracks_.size()))
    {
   auto& track = audioTracks_[selectedTrackIndex_];
 
        // Track name
  char trackName[128];
        strncpy_s(trackName, track->GetName().c_str(), sizeof(trackName) - 1);
        if (ImGui::InputText("Name", trackName, sizeof(trackName)))
        {
 track->SetName(trackName);
        }
  
      // Volume
   float volume = track->GetVolume();
     if (ImGui::SliderFloat("Volume", &volume, 0.0f, 1.0f, "%.2f"))
      {
      track->SetVolume(volume);
   }
        
  // Pan
  float pan = track->GetPan();
        if (ImGui::SliderFloat("Pan", &pan, -1.0f, 1.0f, "%.2f"))
 {
     track->SetPan(pan);
   }

    // Mute/Solo
        bool mute = track->IsMuted();
        if (ImGui::Checkbox("Mute", &mute))
   {
        track->SetMuted(mute);
}
        
        ImGui::SameLine();
        bool solo = track->IsSoloed();
        if (ImGui::Checkbox("Solo", &solo))
      {
      track->SetSoloed(solo);
   }
  
        ImGui::Separator();
  
// Audio info
     if (!track->IsEmpty())
        {
       ImGui::Text("Audio Information:");
     ImGui::BulletText("Sample Rate: %d Hz", track->GetSampleRate());
       ImGui::BulletText("Channels: %d", track->GetNumChannels());
     ImGui::BulletText("Bits Per Sample: %d", track->GetBitsPerSample());
     ImGui::BulletText("Duration: %.2f seconds", track->GetDurationSeconds());
       ImGui::BulletText("Samples: %u", track->GetNumSamples());
     ImGui::BulletText("Peak Amplitude: %.3f", track->GetPeakAmplitude());
      ImGui::BulletText("RMS Amplitude: %.3f", track->GetRMSAmplitude());
        }
        else
        {
   ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No audio loaded");
}
    }
    else
 {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No track selected");
    }
    
    ImGui::Separator();
    
 // Instrument section (disabled for audio tracks)
    ImGui::BeginDisabled();
    if (ImGui::CollapsingHeader("Instrument"))
    {
        ImGui::Text("Not available for audio tracks");
    }
    ImGui::EndDisabled();
    
    // Effects section (placeholder)
    if (ImGui::CollapsingHeader("Effects"))
    {
 ImGui::Text("No effects loaded");
        if (ImGui::Button("Add Effect..."))
   {
      // Open effects browser
        }
    }

    ImGui::End();
}

void DAWImGuiWindow::DrawMixer()
{
  ImGui::Begin("Mixer", &uiState_.showMixer, ImGuiWindowFlags_NoCollapse);

    ImGui::Text("Mixer Channels");
    ImGui::Separator();
    
    // Master channel
    ImGui::BeginGroup();
    ImGui::Text("Master");
    
    static float masterVol = 0.8f;
    ImGui::VSliderFloat("##master", ImVec2(40, 200), &masterVol, 0.0f, 1.0f, "");
    ImGui::Text("%.2f", masterVol);
    
    if (ImGui::Button("M##mastermute", ImVec2(40, 20))) {}
    if (ImGui::Button("S##mastersolo", ImVec2(40, 20))) {}
  
    ImGui::EndGroup();
    
    ImGui::SameLine();
    
    // Track channels
    if (sequencer_)
    {
  for (unsigned int i = 0; i < sequencer_->GetChannelCount() && i < 8; ++i)
        {
       ImGui::BeginGroup();
            
            char label[16];
         snprintf(label, sizeof(label), "Ch %u", i + 1);
            ImGui::Text("%s", label);

      float vol = 0.75f;
            snprintf(label, sizeof(label), "##vol%u", i);
    ImGui::VSliderFloat(label, ImVec2(40, 200), &vol, 0.0f, 1.0f, "");
            ImGui::Text("%.2f", vol);
         
snprintf(label, sizeof(label), "M##mute%u", i);
            if (ImGui::Button(label, ImVec2(40, 20))) {}
            
    snprintf(label, sizeof(label), "S##solo%u", i);
            if (ImGui::Button(label, ImVec2(40, 20))) {}
      
       ImGui::EndGroup();
            
       if (i < sequencer_->GetChannelCount() - 1 && i < 7)
                ImGui::SameLine();
        }
    }

    ImGui::End();
}

void DAWImGuiWindow::DrawBrowser()
{
    ImGui::Begin("Browser", &uiState_.showBrowser, ImGuiWindowFlags_NoCollapse);

    ImGui::Text("File Browser");
    ImGui::Separator();
    
    // Category tabs
    if (ImGui::BeginTabBar("BrowserTabs"))
    {
        if (ImGui::BeginTabItem("Instruments"))
        {
         ImGui::Text("Available Instruments:");
   ImGui::Separator();
    
     ImGui::Selectable("Piano");
       ImGui::Selectable("Synth");
            ImGui::Selectable("Drums");
            ImGui::Selectable("Bass");
     
         ImGui::EndTabItem();
   }
        
        if (ImGui::BeginTabItem("Samples"))
        {
          ImGui::Text("Available Samples:");
   ImGui::Separator();
   
  ImGui::Selectable("Kick.wav");
         ImGui::Selectable("Snare.wav");
 ImGui::Selectable("HiHat.wav");
          
     ImGui::EndTabItem();
        }
        
 if (ImGui::BeginTabItem("Effects"))
        {
       ImGui::Text("Available Effects:");
            ImGui::Separator();
            
            ImGui::Selectable("Reverb");
            ImGui::Selectable("Delay");
         ImGui::Selectable("Chorus");
            ImGui::Selectable("EQ");
            ImGui::Selectable("Compressor");
     
            ImGui::EndTabItem();
        }
        
        ImGui::EndTabBar();
    }

    ImGui::End();
}

void DAWImGuiWindow::DrawProperties()
{
    ImGui::Begin("Properties", &uiState_.showProperties, ImGuiWindowFlags_NoCollapse);

 ImGui::Text("Project Properties");
    ImGui::Separator();
    
    // Project settings
    if (ImGui::CollapsingHeader("Project", ImGuiTreeNodeFlags_DefaultOpen))
  {
        static char projectName[64] = "Untitled Project";
        ImGui::InputText("Name", projectName, sizeof(projectName));
      
        static int sampleRate = 44100;
      ImGui::Text("Sample Rate: %d Hz", sampleRate);
        
    static int bufferSize = 512;
        ImGui::SliderInt("Buffer Size", &bufferSize, 64, 2048);
    }
    
    // Audio settings
 if (ImGui::CollapsingHeader("Audio"))
    {
        static int device = 0;
        ImGui::Combo("Output Device", &device, "Default\0Device 1\0Device 2\0");
    
        static int inputDevice = 0;
        ImGui::Combo("Input Device", &inputDevice, "Default\0Device 1\0Device 2\0");
    }
    
    // MIDI settings
    if (ImGui::CollapsingHeader("MIDI"))
    {
        static bool midiThru = false;
        ImGui::Checkbox("MIDI Thru", &midiThru);
      
        ImGui::Text("MIDI Input Devices:");
        ImGui::Selectable("No MIDI devices found");
    }

    ImGui::End();
}

void DAWImGuiWindow::DrawAbout()
{
    ImGui::Begin("About exeDAW", &uiState_.showAbout, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

    ImGui::Text("exeDAW - Professional Digital Audio Workstation");
    ImGui::Separator();
    
    ImGui::Text("Version: 1.0.0");
    ImGui::Text("Built with: ImGui + GLFW + OpenGL");
    ImGui::Spacing();
    
    ImGui::Text("Features:");
    ImGui::BulletText("Multi-track sequencer");
    ImGui::BulletText("MIDI support");
    ImGui::BulletText("VST plugin support");
    ImGui::BulletText("Audio recording and playback");
    ImGui::BulletText("Professional mixer");
    ImGui::Spacing();
    
    if (ImGui::Button("Close", ImVec2(120, 0)))
        uiState_.showAbout = false;

    ImGui::End();
}

// Menu handlers
void DAWImGuiWindow::OnFileNew() 
{ 
    printf("File > New\n");
}

void DAWImGuiWindow::OnFileOpen() 
{ 
    printf("File > Open\n");
}

void DAWImGuiWindow::OnFileSave() 
{ 
    printf("File > Save\n");
}

void DAWImGuiWindow::OnFileSaveAs() 
{ 
    printf("File > Save As\n");
}

void DAWImGuiWindow::OnFileExit() 
{ 
    isRunning_ = false;
    if (glfwWindow_)
        glfwSetWindowShouldClose(glfwWindow_, GLFW_TRUE);
}

void DAWImGuiWindow::OnEditUndo() 
{ 
    printf("Edit > Undo\n");
}

void DAWImGuiWindow::OnEditRedo() 
{ 
    printf("Edit > Redo\n");
}

void DAWImGuiWindow::OnEditCut() 
{ 
    printf("Edit > Cut\n");
}

void DAWImGuiWindow::OnEditCopy() 
{ 
  printf("Edit > Copy\n");
}

void DAWImGuiWindow::OnEditPaste() 
{ 
    printf("Edit > Paste\n");
}

void DAWImGuiWindow::OnEditDelete() 
{ 
    printf("Edit > Delete\n");
}

void DAWImGuiWindow::OnEditSelectAll() 
{ 
    printf("Edit > Select All\n");
}

void DAWImGuiWindow::OnHelpAbout() 
{ 
    uiState_.showAbout = true;
}

// Transport handlers
void DAWImGuiWindow::OnPlayClick() 
{ 
    if (daw_) 
        daw_->PlayAll();
    printf("Transport > Play\n");
}

void DAWImGuiWindow::OnPauseClick() 
{ 
    if (daw_) 
        daw_->PauseAll();
    printf("Transport > Pause\n");
}

void DAWImGuiWindow::OnStopClick() 
{ 
    if (daw_) 
        daw_->StopAll();
    printf("Transport > Stop\n");
}

void DAWImGuiWindow::OnRecordClick() 
{ 
    if (daw_ && daw_->GetTransport()) 
        daw_->GetTransport()->Record();
    printf("Transport > Record\n");
}

void DAWImGuiWindow::OnMetronomeClick() 
{ 
    printf("Transport > Metronome\n");
}

void DAWImGuiWindow::OnAddTrack()
{
    auto track = std::make_shared<AudioTrack>();
    char name[32];
    snprintf(name, sizeof(name), "Track %zu", audioTracks_.size() + 1);
    track->SetName(name);
    audioTracks_.push_back(track);
    
    printf("Added audio track: %s\n", name);
}

void DAWImGuiWindow::OnLoadAudio()
{
    if (selectedTrackIndex_ >= 0 && selectedTrackIndex_ < static_cast<int>(audioTracks_.size()))
  {
        // Open Windows file dialog
  OPENFILENAMEA ofn = {};
        char szFile[512] = "";
        
        ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = glfwGetWin32Window(glfwWindow_);
 ofn.lpstrFile = szFile;
 ofn.nMaxFile = sizeof(szFile);
     ofn.lpstrFilter = "Audio Files\0*.WAV;*.MP3;*.FLAC;*.OGG\0WAV Files\0*.WAV\0All Files\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
 ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
        
        if (GetOpenFileNameA(&ofn))
        {
       auto& track = audioTracks_[selectedTrackIndex_];
if (track->LoadFromFile(szFile))
      {
       printf("Loaded audio file: %s\n", szFile);
          }
       else
            {
printf("Failed to load audio file: %s\n", szFile);
 }
     }
    }
    else
    {
        printf("No track selected. Please select a track first.\n");
  }
}

void DAWImGuiWindow::OnDeleteTrack()
{
    if (selectedTrackIndex_ >= 0 && selectedTrackIndex_ < static_cast<int>(audioTracks_.size()))
    {
        audioTracks_.erase(audioTracks_.begin() + selectedTrackIndex_);
        selectedTrackIndex_ = -1;
      printf("Track deleted\n");
    }
    else
    {
 printf("No track selected\n");
    }
}

void DAWImGuiWindow::DrawFileDialog()
{
    // Simple file path input dialog
 ImGui::OpenPopup("Load Audio File");
    
    if (ImGui::BeginPopupModal("Load Audio File", &uiState_.showFileDialog, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Enter audio file path:");
     ImGui::InputText("##filepath", uiState_.filePathBuffer, sizeof(uiState_.filePathBuffer));
        
 ImGui::Separator();
        
        if (ImGui::Button("Load", ImVec2(120, 0)))
        {
      if (selectedTrackIndex_ >= 0 && selectedTrackIndex_ < static_cast<int>(audioTracks_.size()))
 {
     auto& track = audioTracks_[selectedTrackIndex_];
    track->LoadFromFile(uiState_.filePathBuffer);
  }
      uiState_.showFileDialog = false;
      ImGui::CloseCurrentPopup();
        }
        
  ImGui::SameLine();
        
     if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
 uiState_.showFileDialog = false;
       ImGui::CloseCurrentPopup();
     }

        ImGui::EndPopup();
    }
}
