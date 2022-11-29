@ECHO OFF

IF NOT EXIST ./bin mkdir bin

PUSHD src
ECHO Building project...

PUSHD Engine
CALL build.bat
POPD
IF %ERRORLEVEL% NEQ 0 (echo error:%ERRORLEVEL% && exit)

PUSHD Shader
CALL build.bat
POPD
IF %ERRORLEVEL% NEQ 0 (echo error:%ERRORLEVEL% && exit)

PUSHD App
CALL build.bat
POPD
IF %ERRORLEVEL% NEQ 0 (echo error:%ERRORLEVEL% && exit)

ECHO Successfully built the project!