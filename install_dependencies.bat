@echo off
REM Installation script for exeDAW ImGui dependencies

echo.
echo ========================================
echo    exeDAW - ImGui Dependencies Setup
echo ========================================
echo.

REM Check if vcpkg is installed
if not exist "C:\vcpkg\vcpkg.exe" (
    echo ERROR: vcpkg not found at C:\vcpkg\vcpkg.exe
  echo Please install vcpkg first:
 echo   git clone https://github.com/Microsoft/vcpkg.git
    echo   cd vcpkg
    echo   .\bootstrap-vcpkg.bat
    pause
    exit /b 1
)

echo Installing dependencies with vcpkg...
echo.

echo [1/4] Installing GLFW3...
call C:\vcpkg\vcpkg.exe install glfw3:x64-windows
if %errorlevel% neq 0 goto ERROR

echo.
echo [2/4] Installing ImGui with GLFW binding...
call C:\vcpkg\vcpkg.exe install imgui:x64-windows
if %errorlevel% neq 0 goto ERROR

echo.
echo [3/4] Installing ImGui OpenGL3 binding...
call C:\vcpkg\vcpkg.exe install imgui[glfw-binding]:x64-windows imgui[opengl3-binding]:x64-windows
if %errorlevel% neq 0 goto ERROR

echo.
echo [4/4] Installing GLEW (OpenGL extension loader)...
call C:\vcpkg\vcpkg.exe install glew:x64-windows
if %errorlevel% neq 0 goto ERROR

echo.
echo ========================================
echo    All dependencies installed!
echo ========================================
echo.
echo Next steps:
echo 1. Open exeDAW.vcxproj in Visual Studio
echo 2. Right-click project and select "Manage NuGet Packages"
echo 3. Add vcpkg NuGet package (if using vcpkg integration)
echo 4. Build the solution (Ctrl+Shift+B)
echo 5. Run the application (F5)
echo.
pause
goto END

:ERROR
echo.
echo ERROR: Installation failed!
echo Please check the vcpkg output above.
pause
exit /b 1

:END
