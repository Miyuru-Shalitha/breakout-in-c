@echo off

set SOURCE=..\source\main.cpp

set INCLUDE_DIRS=/I..\third_party\SDL2-2.30.8\include

set LIB_PATHS=/LIBPATH:..\third_party\SDL2-2.30.8\lib\x64

set LIBS=SDL2.lib ^
         SDL2main.lib ^
	 shell32.lib

if not exist build mkdir build

pushd build
del *.ilk
del *.obj
del *.pdb

cl /std:c++20 /Zi /W3 %SOURCE% %INCLUDE_DIRS% /FeParadoxBreakout.exe /link /SUBSYSTEM:CONSOLE %LIB_PATHS% %LIBS%

if "%1"=="run" (
    .\ParadoxBreakout.exe
)
popd
