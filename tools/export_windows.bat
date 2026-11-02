@echo off
chcp 65001 >nul
cd /d "%~dp0\.."
where cl >nul 2>nul
if errorlevel 1 (
 echo 请在 Visual Studio 的 x64 Native Tools Command Prompt 中运行此文件。
 pause
 exit /b 1
)
cl /nologo /std:c++17 /utf-8 /O2 /GS- /Gs1048576 /GR- /Zl /Oi- /EHs-c- project\main.cpp /Fe:ASuePotGame.exe /link /NODEFAULTLIB /ENTRY:entry /SUBSYSTEM:WINDOWS /FIXED /STACK:4194304,1048576 kernel32.lib user32.lib gdi32.lib winmm.lib msimg32.lib
if errorlevel 1 exit /b 1
echo 导出完成：ASuePotGame.exe
pause
