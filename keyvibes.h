#pragma once

#include <stdbool.h>
#include <windows.h>
#include "bass.h"
#include "config.h"

//---------------------------------------------------------
// Type

typedef struct {
    DWORD start, length;
} SampleOffset;

typedef struct {
    const char *name;
    const char *sampleFile;
    SampleOffset offsets[KEYBOARD_LEN];
} KeyboardSampleset;

typedef struct {
    char flag;
    const char *optional;
    const char *description;
} Option;

typedef struct {
    char id;
    KeyboardSampleset *sampleset;
} KeyboardList;

typedef struct {
    struct {
        bool showHelp;
        bool verbose;
    } flags;
    KeyboardSampleset keyboardSampleset;
    HSAMPLE keyboardSample[KEYBOARD_LEN];
} Config;

//---------------------------------------------------------
// Global variable

extern Option
    OPTION[];

extern KeyboardList
    KEYBOARD_LIST[];

extern Config
    CONFIG;

extern HHOOK
    MOUSE_HOOK,
    KEYBOARD_HOOK;

extern bool
    KEYSTATE[KEYBOARD_LEN];

//---------------------------------------------------------
// Function prototype

extern BOOL WINAPI
    Exit(DWORD dwCtrlType);

extern void
    PrintHeader(),
    PrintHelp(const char *argv0),
    PrintOption(Option *option),
    PrintKeyboardList(KeyboardList *list);

extern void
    InitConfig(),
    ParseFlags(int argc, const char **argv);

extern void
    InitHook(),
    FreeHook();

extern LRESULT CALLBACK
    MouseProc(int nCode, WPARAM wParam, LPARAM lParam),
    KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

extern bool
    IsKeyDown(DWORD key);

extern void
    ToggleKeyDown(DWORD key);

extern void
    InitAudio(),
    FreeAudio();

extern void
    LoadSampleset(HSAMPLE *sampleset, const char *file, SampleOffset *offset, size_t length),
    FreeSampleset(HSAMPLE *sampleset, size_t length),
    PlaySample(HSAMPLE sample);

extern HSAMPLE
    SliceSample(HSAMPLE sample, BASS_SAMPLE info, void *data, SampleOffset offset);