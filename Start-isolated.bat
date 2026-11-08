@echo off
setlocal
set "APPDATA=%~dp0isolated-save"
if not exist "%APPDATA%" mkdir "%APPDATA%"
if not exist "%APPDATA%\" (
  echo Cannot create isolated save directory.
  pause
  exit /b 1
)
start "" /D "%~dp0" "%~dp0ASuePotGame.exe"
