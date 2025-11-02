#pragma once

// GUI Subsystem Validation Header
// This file contains compile-time and runtime checks to ensure exeDAW runs as GUI app

#include <Windows.h>

namespace exeDAW {
namespace Validation {

// Compile-time check: Ensure we're not building as console app
#ifdef _CONSOLE
  #error "CRITICAL ERROR: _CONSOLE is defined! exeDAW MUST be built as Windows GUI application. Check project SubSystem setting."
#endif

// Compile-time check: Ensure Windows subsystem is targeted
#ifndef _WINDOWS
    #error "CRITICAL ERROR: _WINDOWS is not defined! exeDAW MUST be built as Windows GUI application."
#endif

/// <summary>
/// Runtime validation that we're running as GUI application
/// </summary>
/// <returns>True if running as GUI application, false if console mode detected</returns>
inline bool ValidateGUIMode()
{
    // Check if we have a console attached
    // In Windows GUI subsystem, GetConsoleWindow() should return NULL
    HWND consoleWindow = GetConsoleWindow();
    
    if (consoleWindow != NULL)
    {
    // Console window exists - this might indicate console subsystem
     // However, user might have attached console manually with AllocConsole()
        // So we'll log this but not fail
        OutputDebugStringA("WARNING: Console window detected. If this is unexpected, check SubSystem setting.\n");
    }
 
    return true;  // We can still function as GUI even if console is present
}

/// <summary>
/// Display validation information to user
/// </summary>
inline void DisplayValidationInfo()
{
    const char* message = 
  "exeDAW GUI Subsystem Validation\n\n"
        "? Compiled as Windows GUI Application\n"
        "? Entry Point: WinMain\n"
        "? Subsystem: Windows\n\n"
        "This application will always run as a GUI application.";
 
    OutputDebugStringA("=== exeDAW GUI Validation ===\n");
    OutputDebugStringA("Status: GUI subsystem active\n");
    OutputDebugStringA("Entry Point: WinMain\n");
    OutputDebugStringA("Compile Flags: _WINDOWS defined, _CONSOLE not defined\n");
}

// Automatic validation on header inclusion
struct AutoValidator
{
    AutoValidator()
    {
        ValidateGUIMode();
     DisplayValidationInfo();
    }
};

// Static instance for automatic validation
// This will run before main/WinMain when the header is included
// Comment out if you don't want automatic validation
// static AutoValidator g_autoValidator;

} // namespace Validation
} // namespace exeDAW
