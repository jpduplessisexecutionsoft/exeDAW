# PowerShell script to thoroughly clean exeDAW.vcxproj

$vcxprojPath = "exeDAW.vcxproj"
$content = Get-Content $vcxprojPath -Raw

Write-Host "Starting comprehensive cleanup of exeDAW.vcxproj..."

# Remove ALL external/glfw ClCompile and ClInclude entries
Write-Host "Removing all external/glfw entries..."
$beforeCount = ([regex]::Matches($content, 'Include="external\\glfw\\')).Count
$content = $content -replace '<Cl(?:Compile|Include) Include="external\\glfw\\[^"]*"[^/>]*/>[\r\n\s]*', ''
$afterCount = ([regex]::Matches($content, 'Include="external\\glfw\\')).Count
Write-Host "  Removed $($beforeCount - $afterCount) GLFW entries"

# Remove ALL external/imgui ClCompile and ClInclude entries  
Write-Host "Removing all external/imgui entries..."
$beforeCount = ([regex]::Matches($content, 'Include="external\\imgui\\')).Count
$content = $content -replace '<Cl(?:Compile|Include) Include="external\\imgui\\[^"]*"[^/>]*/>[\r\n\s]*', ''
$afterCount = ([regex]::Matches($content, 'Include="external\\imgui\\')).Count
Write-Host "  Removed $($beforeCount - $afterCount) ImGui entries"

# Find where to insert our files - look for the ItemGroup with exeDAW.cpp
$pattern = '(<ItemGroup>[\s\S]*?<ClCompile Include="exeDAW\.cpp" />)'

$requiredFiles = @'
$1
    <ClCompile Include="external\imgui\imgui.cpp" />
<ClCompile Include="external\imgui\imgui_draw.cpp" />
    <ClCompile Include="external\imgui\imgui_tables.cpp" />
    <ClCompile Include="external\imgui\imgui_widgets.cpp" />
    <ClCompile Include="external\imgui\backends\imgui_impl_glfw.cpp" />
<ClCompile Include="external\imgui\backends\imgui_impl_opengl3.cpp" />
    <ClCompile Include="external\glfw\src\context.c" />
    <ClCompile Include="external\glfw\src\init.c" />
    <ClCompile Include="external\glfw\src\input.c" />
    <ClCompile Include="external\glfw\src\monitor.c" />
    <ClCompile Include="external\glfw\src\platform.c" />
    <ClCompile Include="external\glfw\src\vulkan.c" />
    <ClCompile Include="external\glfw\src\window.c" />
    <ClCompile Include="external\glfw\src\egl_context.c" />
    <ClCompile Include="external\glfw\src\osmesa_context.c" />
    <ClCompile Include="external\glfw\src\null_init.c" />
<ClCompile Include="external\glfw\src\null_monitor.c" />
    <ClCompile Include="external\glfw\src\null_window.c" />
    <ClCompile Include="external\glfw\src\null_joystick.c" />
    <ClCompile Include="external\glfw\src\win32_init.c" />
    <ClCompile Include="external\glfw\src\win32_module.c" />
    <ClCompile Include="external\glfw\src\win32_joystick.c" />
    <ClCompile Include="external\glfw\src\win32_monitor.c" />
    <ClCompile Include="external\glfw\src\win32_time.c" />
    <ClCompile Include="external\glfw\src\win32_thread.c" />
    <ClCompile Include="external\glfw\src\win32_window.c" />
 <ClCompile Include="external\glfw\src\wgl_context.c" />
'@

Write-Host "Adding required ImGui and GLFW source files..."
$content = $content -replace $pattern, $requiredFiles

# Add required headers (for IntelliSense)
$headerPattern = '(<ItemGroup>[\s\S]*?<ClInclude Include="framework\.h" />)'
$requiredHeaders = @'
$1
    <ClInclude Include="external\imgui\imgui.h" />
    <ClInclude Include="external\imgui\imgui_internal.h" />
    <ClInclude Include="external\imgui\imstb_rectpack.h" />
    <ClInclude Include="external\imgui\imstb_textedit.h" />
  <ClInclude Include="external\imgui\imstb_truetype.h" />
    <ClInclude Include="external\imgui\backends\imgui_impl_glfw.h" />
    <ClInclude Include="external\imgui\backends\imgui_impl_opengl3.h" />
<ClInclude Include="external\imgui\backends\imgui_impl_opengl3_loader.h" />
    <ClInclude Include="external\glfw\include\GLFW\glfw3.h" />
    <ClInclude Include="external\glfw\include\GLFW\glfw3native.h" />
'@

Write-Host "Adding required header files..."
$content = $content -replace $headerPattern, $requiredHeaders

# Save the file
$content | Set-Content $vcxprojPath -NoNewline

Write-Host "`nexeDAW.vcxproj has been thoroughly cleaned and updated!"
Write-Host "? Removed all unnecessary external files"
Write-Host "? Added only required ImGui core files"
Write-Host "? Added only required GLFW source files"
Write-Host "? Added necessary headers for IntelliSense"
