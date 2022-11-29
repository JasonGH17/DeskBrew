@ECHO OFF
SETLOCAL EnableDelayedExpansion

SET files=
FOR /R %%f in (*.cpp) do (
    SET files=!files! %%f
)

SET outName=DeskBrew
SET cstd=-std=c++17
SET cflags=-g -Wall -Werror -Wextra -pedantic
SET include=-I../ -I../Engine -I%VULKAN_SDK%/Include
SET linker=-L../../bin -lengine
SET defines=-D_DEBUG -DDB_IMPORT

ECHO Building %outName% executable
g++ -o ../../bin/%outName%.exe %files% %cstd% %include% %linker% %defines%