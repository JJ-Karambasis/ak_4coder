@echo off

set CompileFreeType=0

set CurrentPath=%~dp0

set FreeTypeIncludePath=-I%CurrentPath%..\third-party\freetype\include
set FreeTypeSrcPath=%CurrentPath%..\third-party\freetype\src
set FreeTypeCFiles=%FreeTypeSrcPath%\base\ftsystem.c ^
	%FreeTypeSrcPath%\base\ftinit.c ^
	%FreeTypeSrcPath%\base\ftdebug.c ^
	%FreeTypeSrcPath%\base\ftbase.c ^
	%FreeTypeSrcPath%\truetype\truetype.c ^
	%FreeTypeSrcPath%\sdf\sdf.c ^
	%FreeTypeSrcPath%\base\ftbitmap.c ^
	%FreeTypeSrcPath%\autofit\autofit.c ^
	%FreeTypeSrcPath%\type1\type1.c ^
	%FreeTypeSrcPath%\cff\cff.c ^
	%FreeTypeSrcPath%\cid\type1cid.c ^
	%FreeTypeSrcPath%\pfr\pfr.c ^
	%FreeTypeSrcPath%\type42\type42.c ^
	%FreeTypeSrcPath%\winfonts\winfnt.c ^
	%FreeTypeSrcPath%\pcf\pcf.c ^
	%FreeTypeSrcPath%\bdf\bdf.c ^
	%FreeTypeSrcPath%\psaux\psaux.c ^
	%FreeTypeSrcPath%\psnames\psnames.c ^
	%FreeTypeSrcPath%\sfnt\sfnt.c ^
	%FreeTypeSrcPath%\smooth\smooth.c ^
	%FreeTypeSrcPath%\raster\raster.c ^
	%FreeTypeSrcPath%\gzip\ftgzip.c ^
	%FreeTypeSrcPath%\lzw\ftlzw.c ^
	%FreeTypeSrcPath%\pshinter\pshinter.c ^
	%FreeTypeSrcPath%\base\ftsynth.c ^
	%FreeTypeSrcPath%\svg\ftsvg.c

set CompilerFlags=-nologo -FC -Z7 -std:c++17 -I%CurrentPath%..\ -I%CurrentPath%..\custom %FreeTypeIncludePath% -I%CurrentPath%..\third-party\ak_libs\stl

pushd %CurrentPath%..\bin

IF %CompileFreeType% == 1 (
IF NOT EXIST %CurrentPath%..\bin\freetype_temp mkdir %CurrentPath%..\bin\freetype_temp
pushd %CurrentPath%..\bin\freetype_temp
cl %CompilerFlags% -DFT_DEBUG_LEVEL_ERROR -DFT2_BUILD_LIBRARY %FreeTypeIncludePath% -LD -c %FreeTypeCFiles%
lib -nologo -out:..\ftsystem.lib *obj
popd
RMDIR /S /Q %CurrentPath%..\bin\freetype_temp
)

cl %CompilerFlags% -LD %CurrentPath%..\4ed_app_target.cpp -link -out:4ed_app.dll
cl %CompilerFlags% %Warnings% %CurrentPath%..\platform_win32\win32_4ed.cpp -link gdi32.lib user32.lib opengl32.lib winmm.lib comdlg32.lib userenv.lib ftsystem.lib -out:4ed.exe
popd

call %CurrentPath%..\custom\build\build.bat

set CurrentPath=%~dp0
copy "%CurrentPath%..\custom\bin\*.*" "%CurrentPath%..\bin"