@ECHO OFF
SETLOCAL EnableDelayedExpansion

SET files=
FOR /R %%f in (*.cpp) do (
    SET files=!files! %%f
)

SET outName=engine
SET cstd=-std=c++17
SET cflags=-g -shared -Wall -Werror -Wextra -pedantic
SET include=-I%VULKAN_SDK%/Include -I.. -I.
SET linker=-luser32 -L%VULKAN_SDK%/Lib -lvulkan-1 -lopengl32 -lglu32 -lgdi32 
SET defines=-D_DEBUG -DVK_DEBUG -DVK_USE_PLATFORM_WIN32_KHR -DDB_EXPORT -D_CRT_SECURE_NO_WARNINGS

ECHO Building %outName%.dll
g++ -o ../../bin/%outName%.dll %files% %cflags% %cstd% %include% %linker% %defines%