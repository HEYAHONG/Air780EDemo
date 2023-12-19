@echo off

@rem Ŀ¼
set APP_ROOT_PATH=%~dp0

@rem ����sdk���軷����������(�ɸ���ʵ��������û�������)
set PROJECT_NAME=Air780E
set PROJECT_DIR=%APP_ROOT_PATH%\app
set PROJECT_DIR=%PROJECT_DIR:\=/%
if "X" == "X%LSPD_MODE%" set LSPD_MODE=disable
if "X" == "X%EC618_RNDIS%" set EC618_RNDIS=disable

@rem ���SDK
if exist "%APP_ROOT_PATH%\sdk\xmake.lua" goto :CheckSdkOk
echo SDK������,��ʹ��git���ز�ʹ��git submodule update --init������ģ�顣
pause
goto :eof
:CheckSdkOk
set ROOT_PATH=%APP_ROOT_PATH%\sdk
set ROOT_PATH=%ROOT_PATH:\=/%

@rem ���xmake
xmake --version 2> nul  > nul
if "X%ERRORLEVEL%" == "X0" goto :CheckXMakeOk
echo xmake δ��װ���뵽https://xmake.io���ذ�װ
pause
goto :eof
:CheckXMakeOk

@rem ��SDK�򲹶�(�滻һЩ�ļ�)
COPY /Y  "%APP_ROOT_PATH%\sdk_patch\ec618_0h00_flash.c"    "%APP_ROOT_PATH%\sdk\PLAT\core\ld\ec618_0h00_flash.c"

@rem ����CMakeLists.txt(���ڷ���ʹ�ñ༭����)��������
PUSHD "%APP_ROOT_PATH%\sdk" 2> nul > nul
xmake project -k cmake 2> nul > nul
xmake -v
if NOT "X%ERRORLEVEL%" == "X0" PAUSE
POPD 2> nul > nul

@rem ʹ��ping��ʱһ��ʱ��
ping localhost 2> nul > nul

