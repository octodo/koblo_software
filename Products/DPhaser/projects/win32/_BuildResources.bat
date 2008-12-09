@ECHO OFF
SET sBatch=%0\..\..\..\BuildResources.bat
IF NOT EXIST "%sBatch%" GOTO lblNotFound
CALL %sBatch%
GOTO lblEnd

:lblNotFound
ECHO File not found:
ECHO %sBatch%
COLOR 4f
PAUSE
GOTO lblEnd

:lblEnd