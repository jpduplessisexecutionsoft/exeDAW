// exeDAW.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "exeDAW.h"
#include "DAWImGuiWindow.h"
#include "GUIValidation.h"
#include <memory>
#include <stdexcept>
#include <string>
#include <Windows.h>

// Global DAW Window
std::unique_ptr<DAWImGuiWindow> g_pDAWWindow;

void ReportError(const std::string& message)
{
    // Use MessageBox for GUI apps - fprintf won't show in Windows GUI subsystem
    MessageBoxA(NULL, message.c_str(), "exeDAW - Error", MB_OK | MB_ICONERROR | MB_TOPMOST);
    
    // Also log to debug output for developers
    OutputDebugStringA("exeDAW Error: ");
    OutputDebugStringA(message.c_str());
    OutputDebugStringA("\n");
}

int APIENTRY WinMain(HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR     lpCmdLine,
                    int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    // Validate GUI subsystem at runtime
    exeDAW::Validation::ValidateGUIMode();
    exeDAW::Validation::DisplayValidationInfo();

    try
    {
        // Create and initialize the DAW Window (ImGui)
        g_pDAWWindow = std::make_unique<DAWImGuiWindow>();

        if (!g_pDAWWindow->Create("exeDAW - Professional Digital Audio Workstation", 1600, 1000))
        {
            ReportError("Failed to create DAW window.\n\n"
                "Possible causes:\n"
                "- OpenGL 3.3+ not supported by graphics driver\n"
                "- GLFW initialization failed\n"
                "- Insufficient graphics memory");
            g_pDAWWindow.reset(); // Clean up before returning
            return 1;
        }

        // Main application loop
        while (g_pDAWWindow->IsRunning())
        {
            if (!g_pDAWWindow->Update())
            {
                break;
            }

            g_pDAWWindow->Render();
        }

        // Explicit cleanup before exit
        g_pDAWWindow.reset();
        return 0;
    }
    catch (const std::exception& e)
    {
        ReportError(std::string("An exception occurred: ") + e.what());
        g_pDAWWindow.reset(); // Clean up on exception
        return 1;
    }
    catch (...)
    {
        ReportError("An unknown exception occurred");
        g_pDAWWindow.reset(); // Clean up on unknown exception
        return 1;
    }
}
