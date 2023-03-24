@echo off

set CurrentDirectory=%~dp0

IF NOT EXIST "%CurrentDirectory%..\bin" (
mkdir "%CurrentDirectory%..\bin"
)

set SrcRoots=%CurrentDirectory%..\
set CustomRoots=%SrcRoots%custom

set ThirdPartyPath=%CurrentDirectory%..\third-party\
set AKSTL_Path=%ThirdPartyPath%ak_libs\stl

REM set opts=/W4 /wd4310 /wd4100 /wd4201 /wd4505 /wd4996 /wd4127 /wd4510 /wd4512 /wd4610 /wd4390 /wd4189 /WX
set opts=%opts% /GR- /EHa- /nologo /FC /Zi  -I%AKSTL_Path% -std:c++17
REM set opts=%opts% /I%SrcRoots% /I%CustomRoots%
REM set opts=%opts%

cl %opts% kernel32.lib %CurrentDirectory%\4ed_build.cpp
if %ERRORLEVEL% neq 0 (goto END)


:END