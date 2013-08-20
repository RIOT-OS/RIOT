@echo off

if .%1==. goto help
if exist %1 goto getSizeInfo
goto help

:getSizeInfo
arm-none-eabi-size -t %1 > %2
goto end

:help
echo   Syntax: getSizeInfo inFile outFile
echo.
echo   e.g.: getSizeInfo .\intermediateFiles\libarm_cortexM0l_math.a  arm_cortexM0l_math.txt

:end

