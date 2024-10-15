#include <tchar.h>
#include <windows.h>
#include <windowsx.h>

#include "heap.h"
#include "config.h"
#include "log.h"
#include "window.h"
#include "hook.h"
#include "bass.h"

HANDLE g_hHeap;

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PTSTR pCmdLine, int nCmdShow)
{
    BOOL bBassOk, bHookOk;

    LogInit();

    g_hHeap     = GetProcessHeap();
    g_hInstance = hInstance;

    if (bBassOk = BASS_Init(-1, 44100, 0, 0, NULL))
        __info("Initialized BASS"); else
        __warn("Failed to initialize BASS");

    if (bHookOk = CreateHook())
        __info("Created input hook"); else
        __warn("Failed to create input hook");

    WNDCLASS wc = {
        .style         = 0,
        .lpfnWndProc   = WindowProc,
        .cbClsExtra    = 0,
        .cbWndExtra    = 0,
        .hInstance     = hInstance,
        .hIcon         = LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION)),
        .hCursor       = LoadCursor(NULL, IDC_ARROW),
        .hbrBackground = (HBRUSH)COLOR_WINDOWFRAME,
        .lpszMenuName  = NULL,
        .lpszClassName = APP_NAME,
    };

    if (RegisterClass(&wc))
        __debug("Registered window class"); else
        __fatal("Failed to register window class");

    // TCC is bugging with window size and border thickness
    #ifdef RELEASE
    #define THICKFRAME 0
    #else
    #define THICKFRAME WS_THICKFRAME
    #endif

    if (g_hWindow = CreateWindow(APP_NAME, APP_NAME, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | THICKFRAME,
        CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, g_hInstance, NULL))
        __info("Created window"); else
        __fatal("Failed to create window");

    ShowWindow(g_hWindow, nCmdShow);

    __debug("Entering message loop");

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (bBassOk)
    {
        __info("Closing BASS");
        BASS_Free();
    }

    if (bHookOk)
    {
        __info("Closing input hook");
        FreeHook();
    }

    __info("Application ended, returning code: %d", msg.wParam);
    return msg.wParam;
}

// Custom entry for MSVC for small executable size
#ifdef RELEASE
void cmain()
{
    int ret = _tWinMain(GetModuleHandle(NULL), NULL, NULL, SW_SHOWDEFAULT);
    ExitProcess(ret);
}
#endif
