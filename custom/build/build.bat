@echo off

set CurrentPath=%~dp0

set CompilerFlags=-nologo -FC -Z7 -I%CurrentPath%..\..\third-party\ak_libs\stl -std:c++17

set preproc_file=4coder_command_metadata.i
set meta_opts=/P /Fi"%preproc_file%" /DMETA_PASS

pushd %CurrentPath%..\bin

cl %CompilerFlags% %meta_opts% %CurrentPath%..\4coder_default_bindings.cpp
cl %CompilerFlags% %CurrentPath%..\4coder_metadata_generator.cpp -link -out:metadata_generator.exe
metadata_generator -R "%CurrentPath%..\" "%preproc_file%"
cl %CompilerFlags% -LD %CurrentPath%..\4coder_default_bindings.cpp -link -out:custom_4coder.dll

REM file spammation preventation
del metadata_generator*
del *.exp
del *.obj
del *.lib
del %preproc_file%

popd