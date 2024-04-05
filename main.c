#include "keyvibes.h"
#include <stdio.h>
#include <windows.h>
#include "bass.h"

//---------------------------------------------------------
// Global variable

HSAMPLE keyboardSample[KEYBOARD_LEN];

Option option[] = {
    {'h', "", "Print this help message"},
    {'k', "set", "Set keyboard sampleset"},
    {'v', "", "Print pressed key information"},
    {0, NULL, NULL},
};

Flags flags;

//---------------------------------------------------------
// Main

int main(int argc, char **argv)
{
    PrintHeader();

    InitFlags(&flags);
    ParseFlags(&flags, argc, argv);

    if (flags.showHelp) {
        PrintHelp(argv[0]);
    }

    if (argc < 2) {
        puts("Use -h flag to display help message.");
    }

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

//---------------------------------------------------------
// Help message

void PrintHeader()
{
    puts(
        "Keyvibes v" KV_VERSION "\n"
        "-------------"
    );
}

void PrintHelp(const char *argv0)
{
    printf("Usage: %s [option...]\n", argv0);
    printf("Options:\n");
    PrintOption(option);
    printf("Keyboard sampleset:\n");
    PrintKeyboardList(keyboardList);
    exit(0);
}

void PrintOption(Option *option)
{
    for (size_t i = 0; option[i].flag; i++) {
        printf("  -%c %-8s %s\n",
            option[i].flag,
            option[i].optional,
            option[i].description
        );
    }
}

void PrintKeyboardList(KeyboardList *list)
{
    for (size_t i = 0; list[i].id; i++) {
        printf("  %c  %s\n", list[i].id, list[i].sampleset->name);
    }
}

//---------------------------------------------------------
// Flag parsing

void InitFlags(Flags *flags)
{
    flags->showHelp = false;
    flags->verbose = false;
    flags->keyboardSampleset = *keyboardList[0].sampleset;
}

void ParseFlags(Flags *flags, int argc, const char **argv)
{
    for (int i = 1; i < argc && argv[i][0] == '-'; i++) {
        switch (argv[i][1]) {
        case 'h':
            flags->showHelp = true;
            break;

        case 'k': {
            bool validFlag = false;
            char id = argv[i][2] ? argv[i][2] : argv[++i][0];

            for (int j = 0; keyboardList[j].id; j++) {
                if (id == keyboardList[j].id) {
                    flags->keyboardSampleset = *keyboardList[j].sampleset;
                    printf("Using keyboard sampleset: %s\n", keyboardList[j].sampleset->name);
                    validFlag = true;
                    break;
                }
            }

            if (!validFlag) {
                printf("Unknown keyboard sampleset flag: %c\n", argv[i][0]);
            }
            break;
        }

        case 'v':
            flags->verbose = true;
            break;

        default:
            printf("Unknown flag: %s\n", argv[i]);
            break;
        }
    }
}

//---------------------------------------------------------
// Hooks

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
        if (flags.verbose) puts("Pressed left mouse");
        break;
    case WM_LBUTTONUP:
        if (flags.verbose) puts("Released left mouse");
        break;
    case WM_RBUTTONDOWN:
        if (flags.verbose) puts("Pressed right mouse");
        break;
    case WM_RBUTTONUP:
        if (flags.verbose) puts("Released right mouse");
        break;
    case WM_MOUSEWHEEL: {
        short wheelDelta = HIWORD(mouse->mouseData);
        if (flags.verbose) {
            if (wheelDelta > 0) {
                puts("Scroll forward");
            } else {
                puts("Scroll backward");
            }
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
            if (flags.verbose) printf("Pressed key:  %c (0x%X)\n", key, key);
        }
        break;
    case WM_KEYUP:
        ToggleKeyDown(key);
        if (flags.verbose) printf("Released key: %c (0x%X)\n", key, key);
        break;
    }
    return CallNextHookEx(KeyboardHook, nCode, wParam, lParam);
}

//---------------------------------------------------------
// Repeating keypress prevention

bool keystate[KEYBOARD_LEN];

bool IsKeyDown(DWORD key)
{
    return keystate[key] == true;
}

void ToggleKeyDown(DWORD key)
{
    keystate[key] = !keystate[key];
}

//---------------------------------------------------------
// Audio

void InitAudio()
{
    BASS_Init(-1, 44100, 0, NULL, NULL);
    LoadSampleset(keyboardSample, flags.keyboardSampleset.sampleFile, flags.keyboardSampleset.offsets, KEYBOARD_LEN);
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

    HSAMPLE slice = BASS_SampleCreate(length, info.freq, info.chans, 5, 0);
    BASS_SampleSetData(slice, (void *)((char *)data + start));

    return slice;
}