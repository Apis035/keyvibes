# Keyvibes

A lightweight application to play fake keyboard typing sound.

Idea and keyboard sound from [Mechvibes](https://mechvibes.com/).

The main idea that driven me to create this is to have the same app but fast and lightwight in size and memory, compared to Mechvibes that packages a chromium browser and uses 200 MB of RAM just to play some sounds.

## Todo/planned features

- [X] Command line options
- [ ] Volume adjustment
- [ ] Mouse click and scroll sound
- [ ] Minimize to system tray
- [ ] User custom soundpack
- [ ] Split key press/release audio
- [ ] Volume/pan/pitch randomizer for more sound variety

## Building

Run `build.bat`.

Uses [TCC](https://bellard.org/tcc/) to compile.

To use GCC, replace `tcc` in `build.bat` to `gcc`.

You can also use MSVC to compile.

## Library

Uses [BASS](https://www.un4seen.com/) to play sound.