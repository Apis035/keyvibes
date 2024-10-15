@echo off
if "%1" == "cl" (
    if not exist obj mkdir obj
    cl /DUNICODE /D_UNICODE src\*.c lib\bass.lib /Fo:obj\ /Fe:kv.exe /O1 /link /subsystem:windows /entry:cmain
) else (
    tcc src/*.c -L. -lbass -run
)
