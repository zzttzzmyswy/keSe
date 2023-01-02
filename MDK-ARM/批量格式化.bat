echo off & color 0A
::指定起始文件夹
set DIR="..\"
echo DIR=%DIR%

for /R %DIR% %%f in (*.c;*.cpp;*.h) do ( 
    G:\app\AStyle_3.1_windows\AStyle\bin\AStyle.exe --options=.\new.astylerc %%f 
)
pause