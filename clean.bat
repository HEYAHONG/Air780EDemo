@echo off

@rem 目录
set APP_ROOT_PATH=%~dp0

if exist "%APP_ROOT_PATH%\sdk\clean.bat" call "%APP_ROOT_PATH%\sdk\clean.bat"
if exist "%APP_ROOT_PATH%\sdk\out" del /s /q "%APP_ROOT_PATH%\sdk\out"
if exist "%APP_ROOT_PATH%\sdk\build" del /s /q "%APP_ROOT_PATH%\sdk\build"