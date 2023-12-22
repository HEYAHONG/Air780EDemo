@echo off

@rem 目录
set APP_ROOT_PATH=%~dp0

if exist "%APP_ROOT_PATH%\guiconfig.exe"  start "GuiConfig" "%APP_ROOT_PATH%\guiconfig.exe"