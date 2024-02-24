#pragma once

#include <stdbool.h>
#include <windows.h>

extern BOOL WINAPI
    Exit(DWORD dwCtrlType);

extern HHOOK
    MouseHook,
    KeyboardHook;

extern void
    InitHook(),
    FreeHook();

extern LRESULT CALLBACK
    MouseProc(int nCode, WPARAM wParam, LPARAM lParam),
    KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

extern bool
    keystate[255];

extern bool
    IsKeyDown(DWORD key);

extern void
    ToggleKeyDown(DWORD key);