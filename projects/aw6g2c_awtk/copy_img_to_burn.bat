@echo off
echo. 
echo ���ƿ�ִ���ļ�����������д����
echo.
echo ���ļ������ڿ���SDK

set /p ="���븴���ĸ����а棿1)Debug 2)Release 3)RelWithDebInfo 4)MinSizeRel :" <nul

set /p type=
set DIR_NAME=Debug
IF "%type%"=="2" set DIR_NAME=Release
IF "%type%"=="3" set DIR_NAME=RelWithDebInfo
IF "%type%"=="4" set DIR_NAME=MinSizeRel

echo "��ѡ����%DIR_NAME%"

xcopy %DIR_NAME%\img_aw6g2c_debug.bin ..\..\..\tools\SDK����\04.��д����\��дAworks��SD��NandFlash����\image_tools\img_imx6ul_debug.bin



