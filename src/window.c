#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#include "window.h"
#include "config.h"
#include "log.h"
#include "sampleset.h"

HINSTANCE g_hInstance;
HWND      g_hWindow;

static BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    __debug("Creating window controls");

    HWND h;
    HFONT hFont = GetStockFont(DEFAULT_GUI_FONT);

    #define CHECKBOX(x, y, id, text) \
        h = CreateWindow(WC_BUTTON, TEXT(text), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_LEFTTEXT, \
            x, y, 70, 20, hwnd, (HMENU)id, g_hInstance, NULL); \
        SetWindowFont(h, hFont, FALSE);
    #define COMBOBOX(x, y, id) \
        h = CreateWindow(WC_COMBOBOX, TEXT(""), WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, \
            x, y, 140, 200, hwnd, (HMENU)id, g_hInstance, NULL); \
        SetWindowFont(h, hFont, FALSE);

    CHECKBOX(10, 10, ID_MOUSE_ENABLE, "Mouse:")
    COMBOBOX(90, 10, ID_MOUSE_SAMPLE)

    CHECKBOX(10, 40, ID_KEYBOARD_ENABLE, "Keyboard:")
    COMBOBOX(90, 40, ID_KEYBOARD_SAMPLE)

    h = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, TEXT(""), WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | ES_MULTILINE,
        0, 70, 244, 91, hwnd, (HMENU)ID_TESTBOX, g_hInstance, NULL);
    SetWindowFont(h, hFont, FALSE);

    __debug("Adding mouse sampleset list");
    h = GetDlgItem(hwnd, ID_MOUSE_SAMPLE);
    for (int i = 0; g_mspList[i] != NULL; i++)
    {
        ComboBox_AddString(h, g_mspList[i]->sampleName);
        __debug("  - %s", g_mspList[i]->sampleName);
    }
    ComboBox_SetCurSel(h, 0);

    __debug("Adding keyboard sampleset list");
    h = GetDlgItem(hwnd, ID_KEYBOARD_SAMPLE);
    for (int i=0; g_kspList[i] != NULL; i++)
    {
        ComboBox_AddString(h, g_kspList[i]->sampleName);
        __debug("  - %s", g_kspList[i]->sampleName);
    }
    ComboBox_SetCurSel(h, 0);

    __debug("Setting default options");

    // TODO: enable when mouse soundpack available
    Button_Enable(GetDlgItem(hwnd, ID_MOUSE_ENABLE), FALSE);
    ComboBox_Enable(GetDlgItem(hwnd, ID_MOUSE_SAMPLE), FALSE);

    // Enable keyboard hook by default
    g_bEnableKeyboard = TRUE;
    Button_SetCheck(GetDlgItem(hwnd, ID_KEYBOARD_ENABLE), TRUE);

    // Trigger keyboard soundpack change
    SendMessage(hwnd, WM_COMMAND, MAKELPARAM(ID_KEYBOARD_SAMPLE, CBN_SELCHANGE), 0);

    return TRUE;
}

static void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
        case ID_MOUSE_ENABLE:
        {
            g_bEnableMouse = Button_GetCheck(hwndCtl);
            __info("Toggled mouse hook to %s", g_bEnableMouse ? TEXT("on") : TEXT("off"));
            break;
        }
        case ID_KEYBOARD_ENABLE:
        {
            g_bEnableKeyboard = Button_GetCheck(hwndCtl);
            __info("Toggled keyboard hook to %s", g_bEnableKeyboard ? TEXT("on") : TEXT("off"));
            break;
        }
        case ID_MOUSE_SAMPLE:
        {
            if (codeNotify == CBN_SELCHANGE)
            {
                int idx = ComboBox_GetCurSel(hwndCtl);
                MouseSoundpack *soundpack = g_mspList[idx];
                __info("Changing mouse soundpack to %s", soundpack->sampleName);
                ChangeSampleset(g_hMouseSample, MOUSE_BUTTON_LEN, soundpack->sampleFile, soundpack->offsets);
            }
            break;
        }
        case ID_KEYBOARD_SAMPLE:
        {
            if (codeNotify == CBN_SELCHANGE)
            {
                int idx = ComboBox_GetCurSel(hwndCtl);
                KeyboardSoundpack *soundpack = g_kspList[idx];
                __info("Changing keyboard soundpack to %s", soundpack->sampleName);
                ChangeSampleset(g_hKeyboardSample, KEYBOARD_BUTTON_LEN, soundpack->sampleFile, soundpack->offsets);
            }
            break;
        }
    }
}

// Make control's background color transparent
static HBRUSH OnCtlColor(HWND hwnd, HDC hdc, HWND hwndChild, int type)
{
    SetBkMode(hdc, TRANSPARENT);
    return GetStockBrush(HOLLOW_BRUSH);
}

static void OnDestroy()
{
    PostQuitMessage(0);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        HANDLE_MSG(hwnd, WM_CREATE, OnCreate);
        HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
        HANDLE_MSG(hwnd, WM_DESTROY, OnDestroy);
        HANDLE_MSG(hwnd, WM_CTLCOLORSTATIC, OnCtlColor);
        default: return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
