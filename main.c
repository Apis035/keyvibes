#include <stdio.h>
#include <windows.h>
#include "keyvibes.h"
#include "bass.h"

//---------------------------------------------------------
// Global variable

Option OPTION[] = {
    {'h', "", "Print this help message"},
    {'k', "set", "Set keyboard sampleset"},
    {'v', "", "Print pressed key information"},
    {0, NULL, NULL},
};

Config CONFIG;

//---------------------------------------------------------
// Main

int main(int argc, char **argv)
{
    PrintHeader();

    InitConfig();
    ParseFlags(argc, argv);

    if (CONFIG.flags.showHelp) {
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
    printf("Usage: %s [OPTION...]\n", argv0);
    printf("Options:\n");
    PrintOption(OPTION);
    printf("Keyboard sampleset:\n");
    PrintKeyboardList(KEYBOARD_LIST);
    exit(0);
}

void PrintOption(Option *OPTION)
{
    for (size_t i = 0; OPTION[i].flag; i++) {
        printf("  -%c %-8s %s\n",
            OPTION[i].flag,
            OPTION[i].optional,
            OPTION[i].description
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

void InitConfig()
{
    CONFIG.flags.showHelp = false;
    CONFIG.flags.verbose = false;
    CONFIG.keyboardSampleset = *KEYBOARD_LIST[0].sampleset;
}

void ParseFlags(int argc, const char **argv)
{
    for (int i = 1; i < argc && argv[i][0] == '-'; i++) {
        switch (argv[i][1]) {
        case 'h':
            CONFIG.flags.showHelp = true;
            break;

        case 'k': {
            bool validFlag = false;
            char id = argv[i][2] ? argv[i][2] : argv[++i][0];

            for (int j = 0; KEYBOARD_LIST[j].id; j++) {
                if (id == KEYBOARD_LIST[j].id) {
                    CONFIG.keyboardSampleset = *KEYBOARD_LIST[j].sampleset;
                    printf("Using keyboard sampleset: %s\n", KEYBOARD_LIST[j].sampleset->name);
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
            CONFIG.flags.verbose = true;
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
    MOUSE_HOOK,
    KEYBOARD_HOOK;

void InitHook()
{
    MOUSE_HOOK    = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, NULL, 0);
    KEYBOARD_HOOK = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
}

void FreeHook()
{
    UnhookWindowsHookEx(MOUSE_HOOK);
    UnhookWindowsHookEx(KEYBOARD_HOOK);
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    MSLLHOOKSTRUCT *mouse = (MSLLHOOKSTRUCT *)lParam;

    switch (wParam) {
    case WM_LBUTTONDOWN:
        if (CONFIG.flags.verbose) puts("Pressed left mouse");
        break;
    case WM_LBUTTONUP:
        if (CONFIG.flags.verbose) puts("Released left mouse");
        break;
    case WM_RBUTTONDOWN:
        if (CONFIG.flags.verbose) puts("Pressed right mouse");
        break;
    case WM_RBUTTONUP:
        if (CONFIG.flags.verbose) puts("Released right mouse");
        break;
    case WM_MOUSEWHEEL: {
        short wheelDelta = HIWORD(mouse->mouseData);
        if (CONFIG.flags.verbose) {
            if (wheelDelta > 0) {
                puts("Scroll forward");
            } else {
                puts("Scroll backward");
            }
        }
        break;
    }
    }
    return CallNextHookEx(MOUSE_HOOK, nCode, wParam, lParam);
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    KBDLLHOOKSTRUCT *keyboard = (KBDLLHOOKSTRUCT *)lParam;
    DWORD key = keyboard->vkCode;

    switch (wParam) {
    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
        if (!IsKeyDown(key)) {
            ToggleKeyDown(key);
            PlaySample(CONFIG.keyboardSample[key]);
            if (CONFIG.flags.verbose) printf("Pressed key:  %c (0x%X)\n", key, key);
        }
        break;
    case WM_KEYUP:
        ToggleKeyDown(key);
        if (CONFIG.flags.verbose) printf("Released key: %c (0x%X)\n", key, key);
        break;
    }
    return CallNextHookEx(KEYBOARD_HOOK, nCode, wParam, lParam);
}

//---------------------------------------------------------
// Repeating keypress prevention

bool KEYSTATE[KEYBOARD_LEN];

bool IsKeyDown(DWORD key)
{
    return KEYSTATE[key] == true;
}

void ToggleKeyDown(DWORD key)
{
    KEYSTATE[key] = !KEYSTATE[key];
}

//---------------------------------------------------------
// Audio

void InitAudio()
{
    BASS_Init(-1, 44100, 0, NULL, NULL);
    LoadSampleset(CONFIG.keyboardSample, CONFIG.keyboardSampleset.sampleFile, CONFIG.keyboardSampleset.offsets, KEYBOARD_LEN);
}

void FreeAudio()
{
    FreeSampleset(CONFIG.keyboardSample, KEYBOARD_LEN);
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