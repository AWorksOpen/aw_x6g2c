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
echo

echo
echo **** 清除SD卡前4M空间内容 ****
bin\dd if=/dev/zero of=\\.\%diskpath%: bs=4k count=10

echo
echo **** 重建分区表 ****
bin\repart 2048 %diskpath%

echo
echo **** 格式化分区 ****
bin\fat32format -c8 %diskpath%

echo 
echo **** 烧写UBOOT到保留分区 ****
bin\cfimager.exe -raw -offset 0x400 -f firmware\u-boot.imx -d %diskpath%
@echo 

ping -c 5 127.0.0.1 > nul

echo 
echo **** COPY FILES ****
xcopy firmware\imx6g2c.dtb %diskpath%:\ /y
ping -c 1 127.0.0.1 > nul
xcopy firmware\zImage %diskpath%:\ /y
ping -c 1 127.0.0.1 > nul
xcopy firmware\img_imx6ul_debug_sd.imx %diskpath%:\ /y
echo 烧写完毕，按任意键退出
pause>nul
