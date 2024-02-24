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
    MouseHook    = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, NULL, 0);
    KeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
}

void FreeHook()
{
    UnhookWindowsHookEx(MouseHook);
    UnhookWindowsHookEx(KeyboardHook);
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    MSLLHOOKSTRUCT *mouse = (MSLLHOOKSTRUCT *)lParam;

    switch (wParam) {
    case WM_LBUTTONDOWN:
        puts("Pressed left mouse");
        break;
    case WM_LBUTTONUP:
        puts("Released left mouse");
        break;
    case WM_RBUTTONDOWN:
        puts("Pressed right mouse");
        break;
    case WM_RBUTTONUP:
        puts("Released right mouse");
        break;
    case WM_MOUSEWHEEL: {
        short wheelDelta = HIWORD(mouse->mouseData);
        if (wheelDelta > 0) {
            puts("Scroll forward");
        } else {
            puts("Scroll backward");
        }
        break;
    }
    }
    return CallNextHookEx(MouseHook, nCode, wParam, lParam);
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    KBDLLHOOKSTRUCT *keyboard = (KBDLLHOOKSTRUCT *)lParam;
    DWORD key = keyboard->vkCode;

    switch (wParam) {
    case WM_KEYDOWN:
        if (!IsKeyDown(key)) {
            ToggleKeyDown(key);
            printf("Pressed key:  %c (0x%X)\n", key, key);
        }
        break;
    case WM_KEYUP:
        ToggleKeyDown(key);
        printf("Released key: %c (0x%X)\n", key, key);
        break;
    }
    return CallNextHookEx(KeyboardHook, nCode, wParam, lParam);
}

bool keystate[255];

bool IsKeyDown(DWORD key)
{
    return keystate[key] == true;
}

void ToggleKeyDown(DWORD key)
{
    keystate[key] = !keystate[key];
}