#pragma once

#include <stdbool.h>
#include <windows.h>
#include "bass.h"

#define KEYBOARD_LEN 255

extern void
    ShowHelp();

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
    keystate[KEYBOARD_LEN];

extern bool
    IsKeyDown(DWORD key);

extern void
    ToggleKeyDown(DWORD key);

extern void
    InitAudio(),
    FreeAudio();

typedef struct {
    DWORD start, length;
} SampleOffset;

extern void
    LoadSampleset(HSAMPLE *sampleset, const char *file, SampleOffset *offset, size_t length),
    FreeSampleset(HSAMPLE *sampleset, size_t length),
    PlaySample(HSAMPLE sample);

extern HSAMPLE
    SliceSample(HSAMPLE sample, BASS_SAMPLE info, void *data, SampleOffset offset);