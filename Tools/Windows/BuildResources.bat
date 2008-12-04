REM @ECHO OFF

IF "%__ProductName%" == "" ECHO Environment variable '__ProductName' not set!
IF "%__ProductName%" == "" GOTO lblError


SETLOCAL
COLOR
REM SET _saveDir=%cd%
REM CD /D %0\..\resources


REM --------------------------
REM Look for resource compiler
SETLOCAL
(set WF=)

SET sCmd=rescomp

:: Look for file in same directory as batch-script
  for %%a in ("" %PATHEXT:;= %) do (
     if not defined WF if exist "%0\..\%sCmd%%%~a" set WF=%0\..\%sCmd%%%~a)

:: Look for file in the current directory

  for %%a in ("" %PATHEXT:;= %) do (
     if not defined WF if exist "%sCmd%%%~a" set WF=%CD%\%sCmd%%%~a)

:: Look for file in the PATH

  for %%a in ("" %PATHEXT:;= %) do (
     if not defined WF for %%g in ("%sCmd%%%~a") do (
        if exist "%%~$PATH:g" set WF=%%~$PATH:g))


IF NOT DEFINED WF (
echo Error: Resource compiler not present in resource directory
echo Error: Resource compiler not found in any of the PATH directories
goto lblError
)
REM --------------------------



IF EXIST res.k2s DEL res.k2s

ECHO Y|"%WF%"

IF NOT EXIST res.k2s (
goto lblError
)

del "%__ProductName%.k2s"
rename res.k2s "%__ProductName%.k2s"
@IF ERRORLEVEL 1 GOTO lblError

cd ..

IF NOT EXIST projects\win32\debug\ MKDIR projects\win32\debug\
xcopy "resources\%__ProductName%.k2s" projects\win32\debug\ /y
@IF ERRORLEVEL 1 GOTO lblError

IF NOT EXIST projects\win32\release\ MKDIR projects\win32\release\
xcopy "resources\%__ProductName%.k2s" projects\win32\release\ /y
@IF ERRORLEVEL 1 GOTO lblError


FC /W resources\Resources.h include\Resources.h > NUL
@IF NOT %errorlevel 1 ECHO Note! Resources.h didn't change - not copied.
@IF ERRORLEVEL 1 xcopy resources\Resources.h include\ /y
@IF ERRORLEVEL 1 GOTO lblError

COLOR 2F
ECHO.
ECHO Success!
PAUSE
@COLOR
GOTO lblEnd


:lblError
@ECHO Errorlevel %errorlevel%!
@COLOR 4F
@PAUSE
@COLOR


:lblEnd
SET __ProductName=
IF NOT "%_saveDir%" == "" CD /D %_saveDir%