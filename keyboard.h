#include "keyvibes.h"

typedef struct {
    const char *sampleFile;
    SampleOffset offsets[KEYBOARD_LEN];
} KeyboardConfig;

extern KeyboardConfig
    keyboardEgOreo,
    keyboardCherryMxBrownAbs;