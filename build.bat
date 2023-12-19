@echo off

@rem 目录
set APP_ROOT_PATH=%~dp0

@rem 设置sdk所需环境变量设置(可根据实际情况设置环境变量)
set PROJECT_NAME=Air780E
set PROJECT_DIR=%APP_ROOT_PATH%\app
set PROJECT_DIR=%PROJECT_DIR:\=/%
if "X" == "X%LSPD_MODE%" set LSPD_MODE=disable
if "X" == "X%EC618_RNDIS%" set EC618_RNDIS=disable

@rem 检查SDK
if exist "%APP_ROOT_PATH%\sdk\xmake.lua" goto :CheckSdkOk
echo SDK不存在,请使用git下载并使用git submodule update --init下载子模块。
pause
goto :eof
:CheckSdkOk
set ROOT_PATH=%APP_ROOT_PATH%\sdk
set ROOT_PATH=%ROOT_PATH:\=/%

@rem 检查xmake
xmake --version 2> nul  > nul
if "X%ERRORLEVEL%" == "X0" goto :CheckXMakeOk
echo xmake 未安装，请到https://xmake.io下载安装
pause
goto :eof
:CheckXMakeOk

@rem 给SDK打补丁(替换一些文件)
COPY /Y  "%APP_ROOT_PATH%\sdk_patch\ec618_0h00_flash.c"    "%APP_ROOT_PATH%\sdk\PLAT\core\ld\ec618_0h00_flash.c"

@rem 产生CMakeLists.txt(用于方便使用编辑器打开)构建工程
PUSHD "%APP_ROOT_PATH%\sdk" 2> nul > nul
xmake project -k cmake 2> nul > nul
xmake -v
if NOT "X%ERRORLEVEL%" == "X0" PAUSE
POPD 2> nul > nul

@rem 使用ping延时一段时间
ping localhost 2> nul > nul

