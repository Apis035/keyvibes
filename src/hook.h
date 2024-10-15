#pragma once

#include <windows.h>

BOOL CreateHook();
void FreeHook();

LRESULT CALLBACK MouseProc(int code, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam);
