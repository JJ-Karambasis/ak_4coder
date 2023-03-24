#!/bin/bash

# If any command errors, stop the script
set -e

CurrentFilePath="$(realpath "$0")"
CurrentDirectory="$(dirname "$CurrentFilePath")"

echo $CurrentDirectory

if [ ! -d "$CurrentDirectory/../bin" ]; then
	mkdir "$CurrentDirectory/../bin"
fi