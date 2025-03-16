@echo off

:: Clear last running
del /s *.dll >nul
del /s *.o >nul
cls

:: GCC args
set GXX_ARGS=g++ src/*.o src/*.cpp -w -static -shared -DUNICODE -lshlwapi

:: Build in 32-bit mode
md output\x86 2>nul
windres -F pe-i386 src/NppPlugin.rc -o src/NppPlugin.o
%GXX_ARGS% -m32 -o output/x86/SelectInBrackets.dll
del /s *.o >nul

:: Build in 64-bit mode
md output\x64 2>nul
windres src/NppPlugin.rc -o src/NppPlugin.o
%GXX_ARGS% -o output/x64/SelectInBrackets.dll
del /s *.o >nul
