#pragma once

#include <windows.h>

#include "config.h"

// Control ids
enum {
    ID_KEYVIBES,

    ID_MOUSE_SAMPLE,
    ID_MOUSE_ENABLE,
    ID_KEYBOARD_SAMPLE,
    ID_KEYBOARD_ENABLE,
};

// Global instance and window handle
extern HINSTANCE g_hInstance;
extern HWND      g_hWindow;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
