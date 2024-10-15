#pragma once

#include "config.h"
#include "bass.h"

// Mouse and keyboard button
typedef enum {
    MOUSE_BUTTON_LEFT,
    MOUSE_BUTTON_RIGHT,
} MouseButton;

typedef DWORD
KeyboardButton;

// Sample slice offset
typedef struct {
    DWORD start, length;
} SampleOffset;

// Soundpack data
typedef struct {
    PCTSTR sampleName;
    PCTSTR sampleFile;
    SampleOffset offsets[MOUSE_BUTTON_LEN];
} MouseSoundpack;

typedef struct {
    PCTSTR sampleName;
    PCTSTR sampleFile;
    SampleOffset offsets[KEYBOARD_BUTTON_LEN];
} KeyboardSoundpack;

// Mouse and keyboard toggle
extern BOOL
    g_bEnableMouse,
    g_bEnableKeyboard;

// Mouse and keyboard sliced sample
extern HSAMPLE
    g_hMouseSample[MOUSE_BUTTON_LEN],
    g_hKeyboardSample[KEYBOARD_BUTTON_LEN];

// Mouse and keyboard soundpack list
extern MouseSoundpack
    *g_mspList[];

extern KeyboardSoundpack
    *g_kspList[];


void ChangeSampleset(HSAMPLE *phSampleset, size_t nSamplesetLength, PCTSTR pszSampleFile, SampleOffset *psoOffsets);
void PlaySample(HSAMPLE hSample);
