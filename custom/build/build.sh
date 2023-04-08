#!/bin/bash
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

CompilerFlags="-g -O0 -std=c++17 -I$CurrentDirectory/../../third-party/ak_libs/stl"
Warnings="-Wno-writable-strings -Wno-null-dereference -Wno-switch -Wno-deprecated-declarations -Wno-nullability-completeness"

PreprocFile="4coder_command_metadata.i"
MetaMacros="-DMETA_PASS"

pushd "$CurrentDirectory/../bin"

clang++ $CompilerFlags $Warnings $MetaMacros "$CurrentDirectory/../4coder_default_bindings.cpp" -E -o $PreprocFile
clang++ $CompilerFlags $Warnings "$CurrentDirectory/../4coder_metadata_generator.cpp" -o metadata_generator

./metadata_generator -R "$CurrentDirectory/../" "$CurrentDirectory/../bin/$PreprocFile"

clang++ $CompilerFlags $Warnings "$CurrentDirectory/../4coder_default_bindings.cpp" -shared -o "custom_4coder.so"

rm ./metadata_generator
rm -rfd ./metadata_generator.dSYM
rm $PreprocFile

popd
