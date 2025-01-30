@echo off

rem .\build.bat cl    - Build using cl compiler.
rem .\build.bat clang - Build using clang compiler.
rem add 'run' as the last argument in order to build and run.

set SOURCE=..\source\main.cpp

set INCLUDE_DIRS=/I..\third_party\SDL2-2.30.8\include

set LIB_PATHS=/LIBPATH:..\third_party\SDL2-2.30.8\lib\x64

set LIBS=SDL2.lib ^
         SDL2main.lib ^
	     shell32.lib

if "%1" == "cl" (
    if not exist build mkdir build

    pushd build
    del *.ilk
    del *.obj
    del *.pdb

    cl /std:c++20 /Zi /W3 %SOURCE% %INCLUDE_DIRS% /FeParadoxBulletHell.exe /link /SUBSYSTEM:CONSOLE %LIB_PATHS% %LIBS%

    if "%2"=="run" (
        .\ParadoxBulletHell.exe
    )
    popd
)

set SOURCE=..\source\main.cpp

set INCLUDE_DIRS=-I..\third_party\SDL2-2.30.8\include

set LIB_PATHS=-L..\third_party\SDL2-2.30.8\lib\x64

set LIBS=-lSDL2 ^
         -lSDL2main ^
	     -lshell32

if "%1" == "clang" (
    if not exist build2 mkdir build2

    pushd build2
    del *.ilk
    del *.obj
    del *.pdb

    clang++ -std=c++20 -g -Wall -Wno-switch %SOURCE% %INCLUDE_DIRS% -o ParadoxBulletHell.exe -Xlinker /subsystem:console  %LIB_PATHS% %LIBS%

    if "%2"=="run" (
        .\ParadoxBulletHell.exe
    )
    popd
)