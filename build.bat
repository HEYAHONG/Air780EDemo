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

@rem 检查HCppBox
if exist "%APP_ROOT_PATH%\hcppbox\HCppBox\LICENSE" goto :CheckHCppBoxOk
echo HCppBox不存在,请使用git下载并使用git submodule update --init下载子模块。
pause
goto :eof
:CheckHCppBoxOk

@rem 检查xmake
xmake --version 2> nul  > nul
if "X%ERRORLEVEL%" == "X0" goto :CheckXMakeOk
echo xmake 未安装，请到https://xmake.io下载安装
pause
goto :eof
:CheckXMakeOk

@rem RC资源更新
PUSHD "%APP_ROOT_PATH%\rc" 2> nul > nul
fsgen.exe fs RC_fs.c
POPD 2> nul > nul

@rem 给SDK打补丁(替换一些文件)
COPY /Y  "%APP_ROOT_PATH%\sdk_patch\ec618_0h00_flash.c"    "%APP_ROOT_PATH%\sdk\PLAT\core\ld\ec618_0h00_flash.c"

@rem 执行Kconfig相关程序,生成config.h
set KCONFIG_AUTOHEADER=config.h
set KCONFIG_CONFIG=.config
set USE_KCONFIG=0
PUSHD "%APP_ROOT_PATH%" 2> nul > nul
"genconfig.exe" 2> nul > nul
POPD 2> nul > nul
if exist "%APP_ROOT_PATH%\config.h" set USE_KCONFIG=1

@rem 构建工程并产生CMakeLists.txt(用于方便使用编辑器打开)
PUSHD "%APP_ROOT_PATH%\sdk" 2> nul > nul
xmake -y -w
if NOT "X%ERRORLEVEL%" == "X0" PAUSE
xmake  project -k cmake
POPD 2> nul > nul

@rem 使用ping延时一段时间
ping localhost 2> nul > nul

