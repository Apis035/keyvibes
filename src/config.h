#pragma once

// Required library
#pragma comment(lib, "kernel32")
#pragma comment(lib, "user32")
#pragma comment(lib, "gdi32")

// MSVC for release, TCC for quick build
#ifdef _MSC_VER
#define RELEASE
#endif

// BASS unicode mode for loading files
#ifdef UNICODE
#define TBASSUNICODE BASS_UNICODE
#else
#define TBASSUNICODE 0
#endif

// Constants
#define APP_NAME            TEXT("Keyvibes")
#define WINDOW_WIDTH        260
#define WINDOW_HEIGHT       200
#define MOUSE_BUTTON_LEN    2
#define KEYBOARD_BUTTON_LEN 256
#define SAMPLE_MAX          8
