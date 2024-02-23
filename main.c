#include "keyvibes.h"
#include <windows.h>

int main()
{
    MessageBox(NULL, "Hello World!", "Hello", 0);
}

BOOL WINAPI Exit(DWORD dwCtrlType)
{
    return TRUE;
}

HHOOK
    MouseHook,
    KeyboardHook;

void InitHook()
{
}

void FreeHook()
{
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    return CallNextHookEx(MouseHook, nCode, wParam, lParam);
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    return CallNextHookEx(KeyboardHook, nCode, wParam, lParam);
}