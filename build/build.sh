#!/bin/bash

CompileFreeType=1

pushd () {
    command pushd "$@" > /dev/null
}

popd () {
    command popd "$@" > /dev/null
}

# If any command errors, stop the script
set -e
CurrentDirectory="$(dirname "$(realpath "$0")")"

if [ ! -d "$CurrentDirectory/../bin" ]; then
	mkdir "$CurrentDirectory/../bin"
fi

FreeTypeIncludePath=-I"$CurrentDirectory/../third-party/freetype/include"
FreeTypeSrcPath="$CurrentDirectory/../third-party/freetype/src"
FreeTypeCFiles="$FreeTypeSrcPath/base/ftsystem.c 
	$FreeTypeSrcPath/base/ftinit.c 
	$FreeTypeSrcPath/base/ftdebug.c 
	$FreeTypeSrcPath/base/ftbase.c 
	$FreeTypeSrcPath/truetype/truetype.c 
	$FreeTypeSrcPath/sdf/sdf.c 
	$FreeTypeSrcPath/base/ftbitmap.c 
	$FreeTypeSrcPath/autofit/autofit.c 
	$FreeTypeSrcPath/type1/type1.c 
	$FreeTypeSrcPath/cff/cff.c 
	$FreeTypeSrcPath/cid/type1cid.c
	$FreeTypeSrcPath/pfr/pfr.c 
	$FreeTypeSrcPath/type42/type42.c
	$FreeTypeSrcPath/winfonts/winfnt.c
	$FreeTypeSrcPath/pcf/pcf.c 
	$FreeTypeSrcPath/bdf/bdf.c 
	$FreeTypeSrcPath/psaux/psaux.c 
	$FreeTypeSrcPath/psnames/psnames.c 
	$FreeTypeSrcPath/sfnt/sfnt.c 
	$FreeTypeSrcPath/smooth/smooth.c 
	$FreeTypeSrcPath/raster/raster.c 
	$FreeTypeSrcPath/gzip/ftgzip.c 
	$FreeTypeSrcPath/lzw/ftlzw.c 
	$FreeTypeSrcPath/pshinter/pshinter.c 
	$FreeTypeSrcPath/base/ftsynth.c 
	$FreeTypeSrcPath/svg/ftsvg.c"

CommonCFlags="-g -O0 -I$CurrentDirectory/../ -I$CurrentDirectory/../custom $FreeTypeIncludePath -I$CurrentDirectory/../third-party/ak_libs/stl"

Warnings="-Wno-writable-strings -Wno-null-dereference -Wno-switch -Wno-deprecated-declarations -Wno-nullability-completeness"
CompilerFlags="$CommonCFlags -std=c++17 -framework Cocoa -framework QuartzCore -framework CoreServices -framework OpenGL -framework IOKit -framework Metal -framework MetalKit ftsystem.a"

pushd "$CurrentDirectory/../bin"

if [ ! -d "$CurrentDirectory/../bin/freetype_temp" ]; then
	mkdir "$CurrentDirectory/../bin/freetype_temp"
fi

if [ $CompileFreeType == 1 ]; then
	pushd "$CurrentDirectory/../bin/freetype_temp"

	clang -c $CommonCFlags -DFT_DEBUG_LEVEL_ERROR -DFT2_BUILD_LIBRARY $FreeTypeIncludePath $FreeTypeCFiles 
	ar r "../ftsystem.a" *.o

	popd
	rm -r -d "$CurrentDirectory/../bin/freetype_temp"
fi

echo $CompilerFlags

clang++ $CompilerFlags $Warnings "$CurrentDirectory/../4ed_app_target.cpp" -shared -o "4ed_app.so"
clang++ $CompilerFlags $Warnings "$CurrentDirectory/../platform_mac/mac_4ed.mm" -o "4ed"

popd

$CurrentDirectory/../custom/build/build.sh

cp "$CurrentDirectory/../custom/bin/custom_4coder.so" "$CurrentDirectory/../bin/custom_4coder.so"

cp -R "$CurrentDirectory/../custom/bin/custom_4coder.so.dSYM" "$CurrentDirectory/../bin/custom_4coder.so.dSYM"
