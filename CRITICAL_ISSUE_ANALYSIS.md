# Critical Issue Analysis - exeDAW Application Closes Immediately

## Problem Statement
The application closes/exits immediately upon launch, preventing the GUI from displaying.

---

## Root Cause Analysis

### Issue #1: WINDOW CLASS NOT REGISTERED (CRITICAL)

**Location**: `exeDAW.cpp` - `wWinMain()` function

**The Problem**:
```cpp
// In wWinMain()
LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
LoadStringW(hInstance, IDC_EXEDAW, szWindowClass, MAX_LOADSTRING);

// MISSING: MyRegisterClass() is never called!
// The window class "szWindowClass" is never registered!

g_pDAWWindow = std::make_unique<DAWWindow>();
if (!g_pDAWWindow->Create(hInstance, nCmdShow))
{
    return FALSE;  // This likely returns here!
}
```

**What Happens**:
1. `DAWWindow::Create()` is called
2. It creates a NEW window class: `L"DAWWindowClass"`
3. But the old window class from resources is never registered
4. The resource strings fail to load, causing initialization to fail

### Issue #2: RESOURCE STRING LOADING FAILS

**Location**: `exeDAW.cpp` - Line 30-31

```cpp
LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
LoadStringW(hInstance, IDC_EXEDAW, szWindowClass, MAX_LOADSTRING);
```

**The Problem**:
- These load strings from the resource file (exeDAW.rc)
- If the resource IDs don't exist or resources fail to load, `szWindowClass` becomes empty or invalid
- An empty `szWindowClass` causes subsequent operations to fail silently

### Issue #3: CONFLICTING WINDOW CLASS REGISTRATION

**Location**: `exeDAW.cpp` vs `DAWWindow.cpp`

**The Conflict**:
- `exeDAW.cpp` has `MyRegisterClass()` that registers a class using `szWindowClass` from resources
- `DAWWindow.cpp` has its own window class: `L"DAWWindowClass"`
- These are TWO DIFFERENT classes, causing confusion

**The Flow**:
1. `wWinMain()` loads resource strings
2. Creates `DAWWindow` (which creates its own class)
3. But the original window procedure expects `szWindowClass` to be registered
4. Mismatch causes creation to fail

---

## Detailed Flow Diagram

```
wWinMain()
  ?? LoadStringW(IDS_APP_TITLE) ? szTitle
  ?? LoadStringW(IDC_EXEDAW) ? szWindowClass
  ?
  ?? g_pDAWWindow = std::make_unique<DAWWindow>()
  ? ?? DAWWindow::Create()
  ?       ?? RegisterClassW("DAWWindowClass") ?
  ?       ?? CreateWindowW("DAWWindowClass", ...) ? Creates window
  ?       ?   ?? SendMessage() triggers WM_CREATE
  ?       ?? ShowWindow()
  ??? return true ?
  ?
  ?? LoadAccelerators() ? hAccelTable
  ?
  ?? Message Loop
      ?? GetMessage()
       ?? Calls DAWWindow::WindowProc()
       ?? DAWWindow::HandleMessage()
```

**POTENTIAL FAILURE POINT**: If `DAWWindow::Create()` returns `false`, the app exits immediately at line 41-42.

---

## Why It Closes Immediately

### Scenario 1: DAWWindow::Create() Returns False
```cpp
if (!g_pDAWWindow->Create(hInstance, nCmdShow))
{
    return FALSE;  // ? APPLICATION EXITS HERE
}
```

This can fail if:
1. `RegisterClassW()` fails (line 42 in DAWWindow.cpp)
2. `CreateWindowW()` fails (line 52 in DAWWindow.cpp)
3. Either returns invalid handle

### Scenario 2: Message Loop Never Starts
If `Create()` succeeds but something fails in initialization, the window might be destroyed before the message loop.

### Scenario 3: Window Destroyed Immediately
```cpp
case WM_DESTROY:
    PostQuitMessage(0);  // ? Exits message loop
    break;
```

If the window receives WM_DESTROY before any messages, the loop ends and app closes.

---

## Verification Checklist

| Issue | Likelihood | Impact |
|-------|-----------|--------|
| `DAWWindow::Create()` returns false | HIGH | App exits immediately |
| Window class registration fails | MEDIUM | Create fails |
| Window creation fails | MEDIUM | Create fails |
| WM_DESTROY called too early | LOW | Window closes early |
| Resource strings fail to load | MEDIUM | String operations fail |

---

## Solution Summary

Fix the initialization sequence:

1. **Remove the old window class registration** (MyRegisterClass)
2. **Remove resource string loading** for window class
3. **Ensure DAWWindow::Create() succeeds**
4. **Add error checking and logging**

See the fixes in the accompanying files.
