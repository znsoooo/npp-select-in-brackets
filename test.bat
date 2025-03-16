@echo off
call build.bat
set NPP_ROOT=D:\Program Files\Notepad++
taskkill /f /im notepad++.exe
ping 127.0.0.1 -n 1 > nul
copy /y output\x64\SelectInBrackets.dll "%NPP_ROOT%\plugins\SelectInBrackets.dll"
start "" "%NPP_ROOT%\notepad++.exe"
