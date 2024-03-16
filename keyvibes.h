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
    const char *name;
    KeyboardSampleset *sampleset;
} KeyboardList;

typedef struct {
    bool showHelp;
    bool verbose;
    KeyboardSampleset keyboardSampleset;
} Flags;

//---------------------------------------------------------
// Global variable

extern Option
    option[];

extern KeyboardList
    keyboardList[];

extern Flags
    flags;

extern HHOOK
    MouseHook,
    KeyboardHook;

extern bool
    keystate[KEYBOARD_LEN];

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
    InitFlags(Flags *flags),
    ParseFlags(Flags *flags, int argc, const char **argv);

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