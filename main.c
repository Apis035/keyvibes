#include "keyvibes.h"
#include "keyboard.h"
#include <stdio.h>
#include <windows.h>
#include "bass.h"

KeyboardConfig keyboardConfig;
HSAMPLE keyboardSample[KEYBOARD_LEN];

int main()
{
    puts(
        "Keyvibes v0.1" "\n"
        "-------------"
    );

    keyboardConfig = keyboardEgOreo;
    InitHook();
    InitAudio();
    SetConsoleCtrlHandler(Exit, TRUE);
    while (GetMessage(NULL, NULL, 0, 0));
}

BOOL WINAPI Exit(DWORD dwCtrlType)
{
    FreeHook();
    FreeAudio();
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
            PlaySample(keyboardSample[key]);
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

bool keystate[KEYBOARD_LEN];

bool IsKeyDown(DWORD key)
{
    return keystate[key] == true;
}

void ToggleKeyDown(DWORD key)
{
    keystate[key] = !keystate[key];
}

void InitAudio()
{
    BASS_Init(-1, 44100, 0, NULL, NULL);
    LoadSampleset(keyboardSample, keyboardConfig.sampleFile, keyboardConfig.offsets, KEYBOARD_LEN);
}

void FreeAudio()
{
    FreeSampleset(keyboardSample, KEYBOARD_LEN);
    BASS_Free();
}

void LoadSampleset(HSAMPLE *sampleset, const char *file, SampleOffset *offset, size_t length)
{
    HSAMPLE sample = BASS_SampleLoad(FALSE, file, 0, 0, 1, 0);

    BASS_SAMPLE info;
    BASS_SampleGetInfo(sample, &info);

    void *data = malloc(info.length);
    BASS_SampleGetData(sample, data);

    for (size_t i = 0; i < length; i++) {
        sampleset[i] = SliceSample(sample, info, data, offset[i]);
    }

    free(data);
    BASS_SampleFree(sample);
}

void FreeSampleset(HSAMPLE *sampleset, size_t length)
{
    for (size_t i = 0; i < length; i++) {
        BASS_SampleFree(sampleset[i]);
    }
}

void PlaySample(HSAMPLE sample)
{
    HCHANNEL channel = BASS_SampleGetChannel(sample, 0);
    BASS_ChannelPlay(channel, TRUE);
}

HSAMPLE SliceSample(HSAMPLE sample, BASS_SAMPLE info, void *data, SampleOffset offset)
{
    DWORD start  = BASS_ChannelSeconds2Bytes(sample, (double)offset.start  / 1000);
    DWORD length = BASS_ChannelSeconds2Bytes(sample, (double)offset.length / 1000);

    HSAMPLE slice = BASS_SampleCreate(length, info.freq, info.chans, 1, 0);
    BASS_SampleSetData(slice, (void *)((char *)data + start));

    return slice;
}