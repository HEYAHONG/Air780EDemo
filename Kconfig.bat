@echo off

@rem 目录
set APP_ROOT_PATH=%~dp0
set KCONFIG_CONFIG=.config
PUSHD "%APP_ROOT_PATH%" 2> nul > nul
if exist "%APP_ROOT_PATH%\guiconfig.exe"  start "GuiConfig" "%APP_ROOT_PATH%\guiconfig.exe"
POPD 2> nul > nul