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



echo **** �ؽ������� ****
bin\repart 20480 %diskpath%


echo **** ��ʽ������ ****
format %diskpath%: /Q /y

echo " "
echo **** ��дAWorksϵͳ���������� ****
bin\cfimager.exe -raw -offset 0x400 -f img_imx6ul_debug_sd.imx -d %diskpath%

echo ��д��ϣ���������˳�
pause nul