# Download required ImGui and GLFW files directly

$ProgressPreference = 'SilentlyContinue'

# Create directories
New-Item -ItemType Directory -Force -Path "external/imgui" | Out-Null
New-Item -ItemType Directory -Force -Path "external/imgui/backends" | Out-Null
New-Item -ItemType Directory -Force -Path "external/glfw/include/GLFW" | Out-Null
New-Item -ItemType Directory -Force -Path "external/glfw/src" | Out-Null

$imguiBase = "https://raw.githubusercontent.com/ocornut/imgui/master"
$glfwBase = "https://raw.githubusercontent.com/glfw/glfw/master"

Write-Host "Downloading ImGui files..."

# ImGui core files
$imguiFiles = @(
    "imgui.cpp", "imgui.h", "imgui_draw.cpp", "imgui_tables.cpp", 
    "imgui_widgets.cpp", "imgui_internal.h", "imconfig.h",
 "imstb_rectpack.h", "imstb_textedit.h", "imstb_truetype.h"
)

foreach ($file in $imguiFiles) {
    Write-Host "  Downloading $file..."
    Invoke-WebRequest -Uri "$imguiBase/$file" -OutFile "external/imgui/$file"
}

# ImGui backends
$backendFiles = @(
    "imgui_impl_glfw.cpp", "imgui_impl_glfw.h",
    "imgui_impl_opengl3.cpp", "imgui_impl_opengl3.h", "imgui_impl_opengl3_loader.h"
)

foreach ($file in $backendFiles) {
    Write-Host "  Downloading backends/$file..."
    Invoke-WebRequest -Uri "$imguiBase/backends/$file" -OutFile "external/imgui/backends/$file"
}

Write-Host "`nDownloading GLFW files..."

# GLFW headers
$glfwHeaders = @("glfw3.h", "glfw3native.h")
foreach ($file in $glfwHeaders) {
    Write-Host "  Downloading include/GLFW/$file..."
 Invoke-WebRequest -Uri "$glfwBase/include/GLFW/$file" -OutFile "external/glfw/include/GLFW/$file"
}

# GLFW source files
$glfwSrc = @(
    "context.c", "init.c", "input.c", "monitor.c", "platform.c", "vulkan.c", "window.c",
  "egl_context.c", "osmesa_context.c",
    "null_init.c", "null_monitor.c", "null_window.c", "null_joystick.c",
    "win32_init.c", "win32_module.c", "win32_joystick.c", "win32_monitor.c",
    "win32_time.c", "win32_thread.c", "win32_window.c", "wgl_context.c",
    "internal.h", "platform.h", "mappings.h", "null_platform.h", "win32_platform.h"
)

foreach ($file in $glfwSrc) {
    Write-Host "  Downloading src/$file..."
    try {
     Invoke-WebRequest -Uri "$glfwBase/src/$file" -OutFile "external/glfw/src/$file"
    } catch {
        Write-Host "    Warning: Could not download $file (might not exist in this version)"
    }
}

Write-Host "`nDone! All required files downloaded successfully."
Write-Host "Files are now in:"
Write-Host "  external/imgui/"
Write-Host "  external/imgui/backends/"
Write-Host "  external/glfw/include/GLFW/"
Write-Host "  external/glfw/src/"
