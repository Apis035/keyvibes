#pragma once

#include <stdbool.h>
#include <windows.h>
#include "bass.h"

#define KV_VERSION "0.2"
#define KEYBOARD_LEN 255

typedef struct {
    DWORD start, length;
} SampleOffset;

typedef struct {
    const char *sampleFile;
    SampleOffset offsets[KEYBOARD_LEN];
} KeyboardConfig;

typedef struct {
    char flag;
    const char *optional;
    const char *description;
} Option;

extern Option
    option[];

typedef struct {
    char id;
    const char *name;
    KeyboardConfig *config;
} KeyboardList;

extern KeyboardList
    keyboardList[];

extern void
    PrintHeader(),
    PrintHelp(const char *argv0),
    PrintOption(Option *option),
    PrintKeyboardList(KeyboardList *list);

typedef struct {
    bool showHelp;
    bool verbose;
    KeyboardConfig keyboardConfig;
} Flags;

extern Flags
    flags;

extern void
    InitFlags(Flags *flags),
    ParseFlags(Flags *flags, int argc, const char **argv);

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

extern void
    LoadSampleset(HSAMPLE *sampleset, const char *file, SampleOffset *offset, size_t length),
    FreeSampleset(HSAMPLE *sampleset, size_t length),
    PlaySample(HSAMPLE sample);

extern HSAMPLE
    SliceSample(HSAMPLE sample, BASS_SAMPLE info, void *data, SampleOffset offset);