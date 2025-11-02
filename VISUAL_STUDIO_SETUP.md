# Visual Studio Configuration for ImGui

## Step 1: Install Dependencies

Run the provided script:
```bash
install_dependencies.bat
```

Or manually with vcpkg:
```bash
cd C:\vcpkg
.\vcpkg install glfw3:x64-windows
.\vcpkg install imgui:x64-windows imgui[glfw-binding]:x64-windows imgui[opengl3-binding]:x64-windows
.\vcpkg install glew:x64-windows
```

---

## Step 2: Configure Visual Studio Project

### Option A: Using vcpkg Integration (Recommended)

1. Open Visual Studio
2. Go to **Tools ? Options ? NuGet Package Manager ? Package Sources**
3. Add vcpkg NuGet source (if not already there)
4. Install vcpkg NuGet package for the project

### Option B: Manual Configuration

#### Include Directories

1. Right-click project ? Properties
2. Configuration Properties ? VC++ Directories
3. Add to "Include Directories":
```
C:\vcpkg\installed\x64-windows\include
```

#### Library Directories

1. Configuration Properties ? VC++ Directories
2. Add to "Library Directories":
```
C:\vcpkg\installed\x64-windows\lib
C:\vcpkg\installed\x64-windows\debug\lib
```

#### Linker Input

1. Configuration Properties ? Linker ? Input
2. Add to "Additional Dependencies":
```
imgui.lib
glfw3.lib
glew32.lib
opengl32.lib
user32.lib
gdi32.lib
shell32.lib
```

---

## Step 3: Update Project Files

### framework.h

Remove Windows-specific includes and add:
```cpp
#pragma once

#include <windows.h>
#include <gl/gl.h>
#include <memory>
#include <vector>
#include <string>
```

---

## Step 4: Build

1. Clean solution: **Build ? Clean Solution**
2. Build: **Ctrl+Shift+B** (Build ? Build Solution)
3. Fix any remaining compilation errors

---

## Step 5: Run

1. **F5** (Debug ? Start Debugging)
2. Application window should appear with ImGui interface
3. All menus and panels should be visible

---

## Troubleshooting

### Compilation Error: "glfw3.h not found"

**Solution:**
- Check Include Directories are set correctly
- Run vcpkg install again
- Restart Visual Studio

### Linking Error: "unresolved external symbol"

**Solution:**
- Check Library Directories are set correctly
- Verify linker libraries are listed
- Check x64 platform (not x86)

### Runtime Error: "The specified module could not be found"

**Solution:**
- Add vcpkg\installed\x64-windows\bin to PATH
- Copy .dll files to output directory
- Run from x64 Debug/Release folder

### Black window on startup

**Solution:**
- Check OpenGL context creation (DAWImGuiWindow::Create)
- Verify graphics driver supports OpenGL 3.3+
- Check for OpenGL errors in debug output

---

## Verification Checklist

- [ ] GLFW installed via vcpkg
- [ ] ImGui installed via vcpkg
- [ ] GLEW installed via vcpkg
- [ ] Include paths added to project
- [ ] Library paths added to project
- [ ] Linker libraries added
- [ ] Solution builds successfully
- [ ] Application runs and displays UI
- [ ] Menus are clickable
- [ ] Windows can be docked

---

## Quick Reference: Required Libraries

| Library | vcpkg Package | .lib file | Purpose |
|---------|---------------|-----------|---------|
| GLFW | glfw3 | glfw3.lib | Window management |
| ImGui | imgui | imgui.lib | UI framework |
| GLEW | glew | glew32.lib | OpenGL loading |
| OpenGL | (Windows) | opengl32.lib | Graphics API |
| Windows | (Windows) | user32.lib, gdi32.lib | OS integration |

---

## Common Issues

### Issue: Can't find vcpkg
**Solution:** Install vcpkg in C:\vcpkg or adjust path in install_dependencies.bat

### Issue: Package installation fails
**Solution:** Update vcpkg: `git pull` in vcpkg directory

### Issue: DLL not found at runtime
**Solution:** Copy from vcpkg\installed\x64-windows\bin to project output directory

### Issue: ImGui windows not appearing
**Solution:** Check that ImGui_ImplGlfw_NewFrame() is called before drawing

---

## Next Steps

1. ? Install dependencies
2. ? Configure Visual Studio
3. ? Build solution
4. ? Run application
5. ?? Add audio playback
6. ?? Enhance sequencer UI
7. ?? Add save/load functionality

---

## Support

For issues with:
- **GLFW**: https://www.glfw.org/docs/latest/
- **ImGui**: https://github.com/ocornut/imgui
- **GLEW**: http://glew.sourceforge.net/
- **vcpkg**: https://github.com/Microsoft/vcpkg

