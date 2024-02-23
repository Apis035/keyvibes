#pragma once

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
