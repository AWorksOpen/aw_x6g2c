cd /d %~dp0
@echo off
echo. 
echo M6G2C SD��д����
echo. 
echo ������SD���̷���
set /p diskpath=
REM set diskpath=E
echo ע��:
echo     �ļ��ᱻ��д�� %diskpath% ��
echo

echo
echo **** ���SD��ǰ4M�ռ����� ****
bin\dd if=/dev/zero of=\\.\%diskpath%: bs=4k count=10

echo
echo **** �ؽ������� ****
bin\repart 2048 %diskpath%

echo
echo **** ��ʽ������ ****
bin\fat32format -c8 %diskpath%

echo 
echo **** ��дUBOOT���������� ****
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
echo ��д��ϣ���������˳�
pause>nul
