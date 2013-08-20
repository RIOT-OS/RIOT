@echo off

if .%1==. goto help
if exist %1 goto getSizeInfo
goto help

:getSizeInfo
C:\Keil\ARM\ARMCC\bin\armar --sizes %1 > %2
goto end

:help
echo   Syntax: getSizeInfo inFile outFile
echo.
echo   e.g.: getSizeInfo .\intermediateFiles\arm_cortexM0l_math.lib  arm_cortexM0l_math.txt

:end

