@echo off

:: Clear last running
del *.dll
del src\*.o
cls

:: Build in 32-bit mode
cd src
windres -F pe-i386 NppPluginDemo.rc -o NppPluginDemo.o
windres -F pe-i386 DockingFeature/goLine.rc -o goLine.o
g++ *.o *.cpp DockingFeature/*.cpp -o ../NppPluginDemo.dll ^
  -m32 -w -static -shared -DUNICODE -lshlwapi
del *.o
cd ..

pause
