cd /d %~dp0
@echo off
echo. 
echo M6G2C SD烧写程序
echo. 
echo 请输入SD卡盘符：
set /p diskpath=
REM set diskpath=E
echo 注意:
echo     文件会被烧写在 %diskpath% 盘



echo **** 重建分区表 ****
bin\repart 20480 %diskpath%


echo **** 格式化分区 ****
format %diskpath%: /Q /y

echo " "
echo **** 烧写AWorks系统到保留扇区 ****
bin\cfimager.exe -raw -offset 0x400 -f img_imx6ul_debug_sd.imx -d %diskpath%

echo 烧写完毕，按任意键退出
pause nul