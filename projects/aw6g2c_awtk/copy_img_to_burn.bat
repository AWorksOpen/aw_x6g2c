@echo off
echo. 
echo 复制可执行文件用于制作烧写镜像
echo.
echo 此文件仅用于开发SDK

set /p ="你想复制哪个发行版？1)Debug 2)Release 3)RelWithDebInfo 4)MinSizeRel :" <nul

set /p type=
set DIR_NAME=Debug
IF "%type%"=="2" set DIR_NAME=Release
IF "%type%"=="3" set DIR_NAME=RelWithDebInfo
IF "%type%"=="4" set DIR_NAME=MinSizeRel

echo "你选择了%DIR_NAME%"

xcopy %DIR_NAME%\img_aw6g2c_debug.bin ..\..\..\tools\SDK发布\04.烧写工具\烧写Aworks到SD或NandFlash工具\image_tools\img_imx6ul_debug.bin



