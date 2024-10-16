#include "config.h"
#include "heap.h"
#include "window.h"
#include "sampleset.h"
#include "hook.h"
#include "log.h"

// Store keyboard state to prevent keyboard repeat
static BOOL bKeyboardState[KEYBOARD_BUTTON_LEN];

static BOOL IsKeyboardButtonDown(KeyboardButton kb)
{
    return bKeyboardState[kb] == TRUE;
}

static void ToggleKeyboardButton(KeyboardButton kb)
{
    bKeyboardState[kb] = !bKeyboardState[kb];
}

HHOOK
    hhkMouse,
    hhkKeyboard;

BOOL CreateHook()
{
    hhkMouse    = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, NULL, 0);
    hhkKeyboard = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    return (hhkMouse != NULL) && (hhkKeyboard != NULL);
}

void FreeHook()
{
    UnhookWindowsHookEx(hhkMouse);
    UnhookWindowsHookEx(hhkKeyboard);
}

LRESULT CALLBACK MouseProc(int code, WPARAM wParam, LPARAM lParam)
{
    if (g_bEnableMouse) switch (wParam)
    {
        case WM_LBUTTONDOWN:
        {
            __info("Left mouse");
            PlaySample(g_hMouseSample[MOUSE_BUTTON_LEFT]);
            break;
        }
        case WM_RBUTTONDOWN:
        {
            __info("Right mouse");
            PlaySample(g_hMouseSample[MOUSE_BUTTON_RIGHT]);
            break;
        }
    }
    return CallNextHookEx(hhkMouse, code, wParam, lParam);
}

LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam)
{
    KBDLLHOOKSTRUCT *kbdll = (KBDLLHOOKSTRUCT*)lParam;
    KeyboardButton key = (KeyboardButton)kbdll->vkCode;

    if (g_bEnableKeyboard) switch (wParam)
    {
        case WM_KEYDOWN:
        {
            if (!IsKeyboardButtonDown(key))
            {
                __info("Keyboard: %c (0x%X)", key, key);
                ToggleKeyboardButton(key);
                PlaySample(g_hKeyboardSample[key]);

                // Focus to edit box when typing
                HWND hEditbox = GetDlgItem(g_hWindow, ID_TESTBOX);
                if (GetActiveWindow() == g_hWindow && GetFocus() != hEditbox) {
                    SetFocus(hEditbox);
                    __debug("Focused to edit box");
                }
            }
            break;
        }
        case WM_KEYUP:
        {
            ToggleKeyboardButton(key);
            break;
        }
    }
    return CallNextHookEx(hhkKeyboard, code, wParam, lParam);
}
