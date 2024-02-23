#include "keyvibes.h"
#include <stdio.h>
#include <windows.h>

int main()
{
    InitHook();
    SetConsoleCtrlHandler(Exit, TRUE);
    while (GetMessage(NULL, NULL, 0, 0));
}

BOOL WINAPI Exit(DWORD dwCtrlType)
{
    FreeHook();
    ExitProcess(0);
    return TRUE;
}

HHOOK
    MouseHook,
    KeyboardHook;

void InitHook()
{
    KeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
}

void FreeHook()
{
    UnhookWindowsHookEx(KeyboardHook);
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    return CallNextHookEx(MouseHook, nCode, wParam, lParam);
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    switch (wParam) {
    case WM_KEYDOWN:
        puts("Pressed key");
        break;
    case WM_KEYUP:
        puts("Released key");
        break;
    }
    return CallNextHookEx(KeyboardHook, nCode, wParam, lParam);
}