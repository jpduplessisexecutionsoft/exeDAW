# PowerShell script to fix exeDAW.vcxproj by removing unnecessary files and adding only required ones

$vcxprojPath = "exeDAW.vcxproj"
$content = Get-Content $vcxprojPath -Raw

# Remove all external/glfw and external/imgui ClCompile entries
$content = $content -replace '<ClCompile Include="external\\glfw\\[^"]*" />[\r\n]*', ''
$content = $content -replace '<ClCompile Include="external\\imgui\\[^"]*" />[\r\n]*', ''

# Find the ItemGroup with ClCompile entries and add our required files
$itemGroupPattern = '(<ItemGroup>[\s\S]*?<ClCompile Include="exeDAW\.cpp" />)'
$replacement = @'
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

$content = $content -replace $itemGroupPattern, $replacement

# Add include directories to all configurations
$includePattern = '(<AdditionalIncludeDirectories>)'
$includeReplacement = '<AdditionalIncludeDirectories>external\imgui;external\imgui\backends;external\glfw\include;%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>'

# Replace for each configuration
$content = $content -replace '<AdditionalIncludeDirectories>([^<]*)</AdditionalIncludeDirectories>', $includeReplacement

# If no AdditionalIncludeDirectories exists, add it after PreprocessorDefinitions
if ($content -notmatch '<AdditionalIncludeDirectories>') {
    $content = $content -replace '(<PreprocessorDefinitions>[^<]*</PreprocessorDefinitions>)', "`$1`r`n      <AdditionalIncludeDirectories>external\imgui;external\imgui\backends;external\glfw\include;%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>"
}

# Add preprocessor definitions for GLFW
$content = $content -replace '(<PreprocessorDefinitions>)([^<]*)(</PreprocessorDefinitions>)', '$1_GLFW_WIN32;$2$3'

# Add OpenGL32.lib to linker dependencies
$content = $content -replace '(<AdditionalDependencies>)([^<]*)(</AdditionalDependencies>)', '$1opengl32.lib;$2$3'

# If no AdditionalDependencies, add it
if ($content -notmatch '<AdditionalDependencies>') {
    $content = $content -replace '(<Link>)', "`$1`r`n      <AdditionalDependencies>opengl32.lib;%(AdditionalDependencies)</AdditionalDependencies>"
}

# Save the modified content
$content | Set-Content $vcxprojPath -NoNewline

Write-Host "exeDAW.vcxproj has been updated successfully!"
Write-Host "Removed all unnecessary external files"
Write-Host "Added required ImGui core and backend files"
Write-Host "Added required GLFW source files for Windows"
Write-Host "Added include directories and linker settings"
