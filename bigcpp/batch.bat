@echo OFF


REM **************************************************************************
REM **************************************************************************


REM ---- Change this to the path of your "Warrior Kings Game Set" directory.
set WKGAMESET="C:\Program Files (x86)\Empire Interactive\Warrior Kings - Battles\saved\Warrior Kings Game Set"

REM ---- Change this to the cpp file you want to expand.
set WKCPPFILE="multi-player extensions.cpp"


REM **************************************************************************
REM **************************************************************************


set WKBIGCPPDIR=%CD%
cd %WKGAMESET%
%WKBIGCPPDIR%\bigcpp.exe %WKCPPFILE% > %WKBIGCPPDIR%\output.txt
cd %WKBIGCPPDIR%