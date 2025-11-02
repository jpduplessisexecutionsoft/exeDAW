#include "DAWWindow.h"
#include <commctrl.h>
#include <windowsx.h>
#include <sstream>
#include <iomanip>
#include <map>
#include <cmath>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "opengl32.lib")

static std::map<HWND, DAWWindow*> g_WindowToInstance;

DAWWindow::DAWWindow()
    : hWnd_(nullptr), hMDIClient_(nullptr), hInstance_(nullptr),
    hMainMenu_(nullptr), hDC_(nullptr), hGLRC_(nullptr),
 isPlaybackActive_(false), isRecordingActive_(false), currentZoom_(1.0f)
{
    daw_ = std::make_shared<DAWApplication>();
}

DAWWindow::~DAWWindow()
{
    Destroy();
}

void DAWWindow::Destroy()
{
    for (auto& kv : children_)
    {
     if (kv.second.hglrc)
   {
  wglMakeCurrent(nullptr, nullptr);
       wglDeleteContext(kv.second.hglrc);
        }
        if (kv.second.hdc)
 {
       ReleaseDC(kv.second.hwnd, kv.second.hdc);
        }
    }
    children_.clear();
  hwndToType_.clear();

    if (hMainMenu_) DestroyMenu(hMainMenu_);
    if (hMDIClient_) DestroyWindow(hMDIClient_);
    if (hWnd_) DestroyWindow(hWnd_);

    hMainMenu_ = nullptr;
    hMDIClient_ = nullptr;
    hWnd_ = nullptr;
}

DAWWindow* DAWWindow::GetInstance(HWND hWnd)
{
    auto it = g_WindowToInstance.find(hWnd);
    if (it != g_WindowToInstance.end()) return it->second;
    return nullptr;
}

HWND DAWWindow::GetChildWindow(MDIChildType type) const
{
    auto it = children_.find(type);
    if (it != children_.end()) return it->second.hwnd;
    return nullptr;
}

void DAWWindow::ShowChild(MDIChildType type, bool show)
{
    HWND child = GetChildWindow(type);
    if (child)
    {
        ShowWindow(child, show ? SW_SHOW : SW_HIDE);
    auto it = children_.find(type);
        if (it != children_.end()) it->second.visible = show;
    }
}

void DAWWindow::SetChildVisible(MDIChildType type, bool visible)
{
    ShowChild(type, visible);
}

bool DAWWindow::Create(HINSTANCE hInstance, int nCmdShow)
{
    hInstance_ = hInstance;
    InitCommonControls();

    const wchar_t PARENT_CLASS_NAME[] = L"DAWMDIParent";
    WNDCLASSW wcParent = {};
    wcParent.lpfnWndProc = ParentWindowProc;
    wcParent.hInstance = hInstance;
    wcParent.lpszClassName = PARENT_CLASS_NAME;
    wcParent.hCursor = LoadCursor(nullptr, IDC_ARROW);
 wcParent.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
wcParent.style = CS_VREDRAW | CS_HREDRAW;
    wcParent.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    RegisterClassW(&wcParent);

    const wchar_t CHILD_CLASS_NAME[] = L"DAWMDIChildGL";
    WNDCLASSW wcChild = {};
    wcChild.lpfnWndProc = ChildWindowProc;
    wcChild.hInstance = hInstance;
    wcChild.lpszClassName = CHILD_CLASS_NAME;
    wcChild.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcChild.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcChild.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
 RegisterClassW(&wcChild);

    CreateMainMenuBar();

    hWnd_ = CreateWindowExW(0, PARENT_CLASS_NAME, L"exeDAW - Professional Digital Audio Workstation",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1600, 1000,
nullptr, hMainMenu_, hInstance, nullptr);
    if (!hWnd_) return false;
    g_WindowToInstance[hWnd_] = this;

    CLIENTCREATESTRUCT ccs = {};
    hMDIClient_ = CreateWindowW(L"MDICLIENT", nullptr,
      WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL,
      0, 0, 0, 0,
        hWnd_, nullptr, hInstance, (LPVOID)&ccs);
    if (!hMDIClient_) return false;
    g_WindowToInstance[hMDIClient_] = this;

    daw_->Initialize();

    CreateMDIChildren();

    ShowWindow(hWnd_, nCmdShow);
    UpdateWindow(hWnd_);
    SetTimer(hWnd_, RENDER_TIMER_ID, RENDER_TIMER_MS, nullptr);
    return true;
}

void DAWWindow::CreateMainMenuBar()
{
    hMainMenu_ = CreateMenu();
    if (!hMainMenu_) return;

    HMENU hFileMenu = CreatePopupMenu();
    AppendMenu(hFileMenu, MFT_STRING, ID_FILE_NEW, L"&New\tCtrl+N");
    AppendMenu(hFileMenu, MFT_STRING, ID_FILE_OPEN, L"&Open\tCtrl+O");
    AppendMenu(hFileMenu, MFT_STRING, ID_FILE_SAVE, L"&Save\tCtrl+S");
    AppendMenu(hFileMenu, MFT_STRING, ID_FILE_SAVEAS, L"Save &As...\tCtrl+Shift+S");
    AppendMenu(hFileMenu, MFT_SEPARATOR, 0, nullptr);
    AppendMenu(hFileMenu, MFT_STRING, ID_FILE_EXIT, L"E&xit\tAlt+F4");
    AppendMenu(hMainMenu_, 0, (UINT_PTR)hFileMenu, L"&File");

    HMENU hEditMenu = CreatePopupMenu();
    AppendMenu(hEditMenu, MFT_STRING, ID_EDIT_UNDO, L"&Undo\tCtrl+Z");
    AppendMenu(hEditMenu, MFT_STRING, ID_EDIT_REDO, L"&Redo\tCtrl+Y");
    AppendMenu(hEditMenu, MFT_SEPARATOR, 0, nullptr);
    AppendMenu(hEditMenu, MFT_STRING, ID_EDIT_CUT, L"Cu&t\tCtrl+X");
    AppendMenu(hEditMenu, MFT_STRING, ID_EDIT_COPY, L"&Copy\tCtrl+C");
    AppendMenu(hEditMenu, MFT_STRING, ID_EDIT_PASTE, L"&Paste\tCtrl+V");
    AppendMenu(hEditMenu, MFT_STRING, ID_EDIT_DELETE, L"&Delete\tDel");
    AppendMenu(hEditMenu, MFT_SEPARATOR, 0, nullptr);
    AppendMenu(hEditMenu, MFT_STRING, ID_EDIT_SELECTALL, L"Select &All\tCtrl+A");
    AppendMenu(hMainMenu_, 0, (UINT_PTR)hEditMenu, L"&Edit");

    HMENU hViewMenu = CreatePopupMenu();
    AppendMenu(hViewMenu, MFT_STRING, ID_VIEW_MENUBAR, L"&Menu Bar");
    AppendMenu(hViewMenu, MFT_STRING, ID_VIEW_TRANSPORT, L"&Transport");
    AppendMenu(hViewMenu, MFT_STRING, ID_VIEW_SEQUENCER, L"&Sequencer");
    AppendMenu(hViewMenu, MFT_STRING, ID_VIEW_INSPECTOR, L"&Inspector");
    AppendMenu(hViewMenu, MFT_STRING, ID_VIEW_MIXER, L"Mi&xer");
    AppendMenu(hViewMenu, MFT_STRING, ID_VIEW_BROWSER, L"&Browser");
    AppendMenu(hViewMenu, MFT_STRING, ID_VIEW_PROPERTIES, L"&Properties");
    AppendMenu(hMainMenu_, 0, (UINT_PTR)hViewMenu, L"&View");

    HMENU hHelpMenu = CreatePopupMenu();
    AppendMenu(hHelpMenu, MFT_STRING, ID_HELP_ABOUT, L"&About");
    AppendMenu(hMainMenu_, 0, (UINT_PTR)hHelpMenu, L"&Help");
}

void DAWWindow::CreateMDIChildren()
{
    CreateMDIChildWindow(L"Menu Bar", MDIChildType::MenuBar, 0, 0, 1600, 32);
    CreateMDIChildWindow(L"Transport", MDIChildType::Transport, 0, 34, 1600, 54);
    CreateMDIChildWindow(L"Sequencer", MDIChildType::Sequencer, 0, 92, 1200, 600);
    CreateMDIChildWindow(L"Inspector", MDIChildType::Inspector, 1210, 92, 390, 600);
    
    CreateMDIChildWindow(L"Mixer", MDIChildType::Mixer, 0, 700, 1600, 200);
    ShowWindow(GetChildWindow(MDIChildType::Mixer), SW_HIDE);

    CreateMDIChildWindow(L"Browser", MDIChildType::Browser, 1210, 92, 390, 600);
    ShowWindow(GetChildWindow(MDIChildType::Browser), SW_HIDE);

    CreateMDIChildWindow(L"Properties", MDIChildType::Properties, 0, 700, 400, 200);
    ShowWindow(GetChildWindow(MDIChildType::Properties), SW_HIDE);
}

HWND DAWWindow::CreateMDIChildWindow(const wchar_t* title, MDIChildType type, int x, int y, int w, int h)
{
    const wchar_t CHILD_CLASS_NAME[] = L"DAWMDIChildGL";

  MDICREATESTRUCT mcs = {};
    mcs.szClass = CHILD_CLASS_NAME;
    mcs.szTitle = title;
    mcs.hOwner = hInstance_;
    mcs.x = x;
    mcs.y = y;
    mcs.cx = w;
    mcs.cy = h;
    mcs.style = WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME;
    mcs.lParam = 0;

    HWND child = (HWND)SendMessageW(hMDIClient_, WM_MDICREATE, 0, (LPARAM)&mcs);
    if (!child) return nullptr;

    g_WindowToInstance[child] = this;

    MDIChildContext ctx(type);
    ctx.hwnd = child;
    ctx.hdc = GetDC(child);
    if (!ctx.hdc) return child;

    if (!SetupPixelFormat(ctx.hdc)) return child;

    ctx.hglrc = wglCreateContext(ctx.hdc);
    if (ctx.hglrc) wglMakeCurrent(ctx.hdc, ctx.hglrc);

    ctx.ui.reset(new UIManager());
    if (ctx.ui) ctx.ui->Initialize(ctx.hdc);

    children_[type] = std::move(ctx);
    hwndToType_[child] = type;

    BuildChildUI(child, type);
    return child;
}

bool DAWWindow::SetupPixelFormat(HDC hdc)
{
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR), 1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA, 32,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        24, 8, 0, PFD_MAIN_PLANE, 0, 0, 0, 0
    };
    int fmt = ChoosePixelFormat(hdc, &pfd);
    if (!fmt) return false;
    return SetPixelFormat(hdc, fmt, &pfd) == TRUE;
}

void DAWWindow::BuildChildUI(HWND child, MDIChildType type)
{
    auto it = children_.find(type);
    if (it == children_.end()) return;
    MDIChildContext& ctx = it->second;

    RECT rc;
    GetClientRect(child, &rc);
    int w = rc.right - rc.left, h = rc.bottom - rc.top;
    ctx.ui->Resize(w, h);

    switch (type)
    {
    case MDIChildType::MenuBar:
    {
        auto menu = std::make_shared<UIMenuBar>(RectF{0, 0, (float)w, (float)h});
    menu->AddItem(L"File", [this]() { OnFileOpen(); });
 menu->AddItem(L"Edit", [this]() { OnEditUndo(); });
      menu->AddItem(L"View", [this]() { OnViewSequencer(); });
        menu->AddItem(L"Help", [this]() { OnHelpAbout(); });
      ctx.ui->Add(menu);
        break;
    }

    case MDIChildType::Transport:
    {
        float x = 10, y = 10, bw = 80, bh = (float)h - 20, sp = 10;
        ctx.ui->Add(std::make_shared<UIButton>(RectF{x, y, bw, bh}, L"Play", [this]() { OnPlayClick(); }));
    x += bw + sp;
        ctx.ui->Add(std::make_shared<UIButton>(RectF{x, y, bw, bh}, L"Pause", [this]() { OnPauseClick(); }));
  x += bw + sp;
     ctx.ui->Add(std::make_shared<UIButton>(RectF{x, y, bw, bh}, L"Stop", [this]() { OnStopClick(); }));
        x += bw + sp;
ctx.ui->Add(std::make_shared<UIButton>(RectF{x, y, bw, bh}, L"Record", [this]() { OnRecordClick(); }));
        x += bw + sp + 20;
        ctx.ui->Add(std::make_shared<UIButton>(RectF{x, y, bw, bh}, L"Metronome", [this]() { OnMetronomeClick(); }));
        break;
  }

    case MDIChildType::Sequencer:
 case MDIChildType::Inspector:
    case MDIChildType::Mixer:
    case MDIChildType::Browser:
    case MDIChildType::Properties:
    {
    break;
    }

    default:
        break;
    }
}

void DAWWindow::RenderChild(HWND child)
{
    auto typeIt = hwndToType_.find(child);
    if (typeIt == hwndToType_.end()) return;

    auto it = children_.find(typeIt->second);
    if (it == children_.end()) return;

    MDIChildContext& ctx = it->second;
    if (!ctx.hdc || !ctx.hglrc) return;

    wglMakeCurrent(ctx.hdc, ctx.hglrc);

    RECT rc;
    GetClientRect(child, &rc);
    int w = rc.right - rc.left, h = rc.bottom - rc.top;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, h, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(0.06f, 0.06f, 0.08f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (it->second.type == MDIChildType::Sequencer)
    {
        glColor3f(0.2f, 0.2f, 0.25f);
        glBegin(GL_LINES);
        for (int gx = 0; gx < w; gx += 50)
        {
         glVertex2i(gx, 0);
            glVertex2i(gx, h);
        }
        for (int gy = 0; gy < h; gy += 50)
   {
  glVertex2i(0, gy);
            glVertex2i(w, gy);
        }
        glEnd();

        glColor3f(0.35f, 0.78f, 1.0f);
        glBegin(GL_LINE_STRIP);
      for (int i = 0; i < w; ++i)
   {
    float t = (float)i / w;
     float y = h / 2 - sinf(t * 6.28f) * h * 0.25f;
      glVertex2f((float)i, y);
        }
        glEnd();
    }

    if (ctx.ui)
    {
      ctx.ui->Resize(w, h);
        ctx.ui->Render();
    }

    SwapBuffers(ctx.hdc);
}

void DAWWindow::Redraw()
{
    for (auto& kv : children_)
    {
     if (kv.second.visible && kv.second.hwnd)
        {
            RenderChild(kv.second.hwnd);
        }
    }
}

void DAWWindow::RedrawAll()
{
    for (auto& kv : children_)
    {
        if (kv.second.hwnd)
    {
          InvalidateRect(kv.second.hwnd, nullptr, FALSE);
        }
    }
}
LRESULT DAWWindow::HandleMenuCommand(UINT commandId)
{
    switch (commandId)
    {
    case ID_FILE_NEW: OnFileNew(); break;
    case ID_FILE_OPEN: OnFileOpen(); break;
    case ID_FILE_SAVE: OnFileSave(); break;
    case ID_FILE_SAVEAS: OnFileSaveAs(); break;
    case ID_FILE_EXIT: OnFileExit(); break;

    case ID_EDIT_UNDO: OnEditUndo(); break;
    case ID_EDIT_REDO: OnEditRedo(); break;
    case ID_EDIT_CUT: OnEditCut(); break;
    case ID_EDIT_COPY: OnEditCopy(); break;
    case ID_EDIT_PASTE: OnEditPaste(); break;
    case ID_EDIT_DELETE: OnEditDelete(); break;
    case ID_EDIT_SELECTALL: OnEditSelectAll(); break;

    case ID_VIEW_MENUBAR: OnViewMenuBar(); break;
    case ID_VIEW_TRANSPORT: OnViewTransport(); break;
    case ID_VIEW_SEQUENCER: OnViewSequencer(); break;
    case ID_VIEW_INSPECTOR: OnViewInspector(); break;
    case ID_VIEW_MIXER: OnViewMixer(); break;
    case ID_VIEW_BROWSER: OnViewBrowser(); break;
    case ID_VIEW_PROPERTIES: OnViewProperties(); break;

    case ID_HELP_ABOUT: OnHelpAbout(); break;

    default: return 0;
    }
    return 0;
}

LRESULT DAWWindow::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_TIMER:
     if (wParam == RENDER_TIMER_ID)
      {
      RedrawAll();
            return 0;
        }
  break;

    case WM_COMMAND:
        return HandleMenuCommand(LOWORD(wParam));

    case WM_CREATE:
      return 0;

    case WM_SIZE:
    {
        if (hMDIClient_)
        {
  RECT rc;
            GetClientRect(hWnd_, &rc);
       MoveWindow(hMDIClient_, 0, 0, rc.right, rc.bottom, TRUE);
        }
      return 0;
    }

    case WM_DESTROY:
    {
        KillTimer(hWnd_, RENDER_TIMER_ID);
        PostQuitMessage(0);
        return 0;
    }

    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MOUSEMOVE:
    {
        POINT p;
        GetCursorPos(&p);
        for (auto& kv : children_)
        {
     if (!kv.second.visible || !kv.second.hwnd) continue;

          RECT rc;
        GetWindowRect(kv.second.hwnd, &rc);
       if (PtInRect(&rc, p))
      {
     ScreenToClient(kv.second.hwnd, &p);
       float x = (float)p.x, y = (float)p.y;
    if (kv.second.ui)
    {
          if (message == WM_LBUTTONDOWN)
     kv.second.ui->OnMouseDown(x, y);
else if (message == WM_LBUTTONUP)
       kv.second.ui->OnMouseUp(x, y);
           else if (message == WM_MOUSEMOVE)
           kv.second.ui->OnMouseMove(x, y);
  }
              InvalidateRect(kv.second.hwnd, nullptr, FALSE);
     break;
}
    }
        return 0;
    }
    }

    return DefWindowProcW(hWnd_, message, wParam, lParam);
}

LRESULT CALLBACK DAWWindow::ParentWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    DAWWindow* pThis = GetInstance(hWnd);
    if (pThis)
        return pThis->HandleMessage(message, wParam, lParam);
    return DefWindowProcW(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK DAWWindow::ChildWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    DAWWindow* pThis = GetInstance(GetParent(hWnd));
    if (!pThis)
return DefWindowProcW(hWnd, message, wParam, lParam);

    switch (message)
    {
    case WM_PAINT:
    {
  PAINTSTRUCT ps;
     BeginPaint(hWnd, &ps);
   pThis->RenderChild(hWnd);
        EndPaint(hWnd, &ps);
  return 0;
  }

    case WM_SIZE:
    {
  auto typeIt = pThis->hwndToType_.find(hWnd);
        if (typeIt != pThis->hwndToType_.end())
        {
 pThis->BuildChildUI(hWnd, typeIt->second);
        }
        pThis->RenderChild(hWnd);
        return 0;
    }

    case WM_DESTROY:
    {
        auto typeIt = pThis->hwndToType_.find(hWnd);
 if (typeIt != pThis->hwndToType_.end())
    {
  auto it = pThis->children_.find(typeIt->second);
  if (it != pThis->children_.end())
            {
       if (it->second.hglrc)
          {
         wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(it->second.hglrc);
      }
    if (it->second.hdc)
      ReleaseDC(hWnd, it->second.hdc);
            }
}
        break;
    }
    }
    return DefWindowProcW(hWnd, message, wParam, lParam);
}

void DAWWindow::OnFileNew() {}
void DAWWindow::OnFileOpen() {}
void DAWWindow::OnFileSave() {}
void DAWWindow::OnFileSaveAs() {}
void DAWWindow::OnFileExit() { PostQuitMessage(0); }
void DAWWindow::OnEditUndo() {}
void DAWWindow::OnEditRedo() {}
void DAWWindow::OnEditCut() {}
void DAWWindow::OnEditCopy() {}
void DAWWindow::OnEditPaste() {}
void DAWWindow::OnEditDelete() {}
void DAWWindow::OnEditSelectAll() {}
void DAWWindow::OnViewMenuBar() { ShowChild(MDIChildType::MenuBar, true); }
void DAWWindow::OnViewTransport() { ShowChild(MDIChildType::Transport, true); }
void DAWWindow::OnViewSequencer() { ShowChild(MDIChildType::Sequencer, true); }
void DAWWindow::OnViewInspector() { ShowChild(MDIChildType::Inspector, true); }
void DAWWindow::OnViewMixer() { ShowChild(MDIChildType::Mixer, true); }
void DAWWindow::OnViewBrowser() { ShowChild(MDIChildType::Browser, true); }
void DAWWindow::OnViewProperties() { ShowChild(MDIChildType::Properties, true); }
void DAWWindow::OnHelpAbout() { MessageBoxW(hWnd_, L"exeDAW - Professional Digital Audio Workstation\nVersion 1.0", L"About exeDAW", MB_OK | MB_ICONINFORMATION); }

void DAWWindow::OnPlayClick() { isPlaybackActive_ = true; if (daw_) daw_->PlayAll(); }
void DAWWindow::OnPauseClick() { isPlaybackActive_ = false; if (daw_) daw_->PauseAll(); }
void DAWWindow::OnStopClick() { isPlaybackActive_ = false; if (daw_) daw_->StopAll(); }
void DAWWindow::OnRecordClick() { isRecordingActive_ = !isRecordingActive_; if (daw_ && isRecordingActive_) daw_->GetTransport()->Record(); }
void DAWWindow::OnMetronomeClick() {}
void DAWWindow::OnSettingsClick() {}
void DAWWindow::OnAddTrackClick() {}
void DAWWindow::OnLoadAudioClick() {}
void DAWWindow::OnDeleteTrackClick() {}
void DAWWindow::OnLoopCheckClick() {}
void DAWWindow::OnSnapToggleClick() {}
void DAWWindow::OnZoomChange() {}
void DAWWindow::UpdateTempoDisplay() {}
