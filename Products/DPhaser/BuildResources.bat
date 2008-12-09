
@echo off

REM   -------------------------  PRODUCT NAME ---------------------------
REM   Set product name here! This will become the name of the .k2s file.
REM   *Don't* use quotes even if name contains space characters.
REM   -------------------------------------------------------------------
SET __ProductName=DoesntMatter

COLOR 1f
CD /d "%0\..\.."


ECHO Searching for resource compiler...
: ECHO ON
:lblLoop_SearchForResComp
ECHO %cd%
SET sResComp=Tools\Windows\ResComp.exe
IF EXIST %sResComp% GOTO lbl_FoundResComp
IF "%cd:~3,1%"=="" (
	@ECHO OFF
	ECHO Error: Resource compiler application not found
	GOTO lblError
) ELSE (
	CD ..
)
GOTO lblLoop_SearchForResComp

:lbl_FoundResComp
@ECHO OFF
SET sResComp=%cd%\%sResComp%
ECHO %sResComp%
SET sResToInc=%sResComp%\..\ResourceToInclude.exe


CD /d "%0\.."
cd resources
IF EXIST res.k2s DEL res.k2s

: ECHO Y|rescomp.exe
ECHO Y|%sResComp%

IF NOT EXIST res.k2s (
	goto lblError
)

IF EXIST ResourcesInc.cpp DEL ResourcesInc.cpp
ECHO Prepare include file of .k2s file:
%sResToInc% ResourcesInc.cpp res.k2s
@IF ERRORLEVEL 1 GOTO lblError

IF EXIST "%__ProductName%.k2s" DEL "%__ProductName%.k2s"
rename res.k2s "%__ProductName%.k2s"
@IF ERRORLEVEL 1 GOTO lblError

cd ..

: IF NOT EXIST projects\win32\debug\ MKDIR projects\win32\debug\
: xcopy "resources\%__ProductName%.k2s" projects\win32\debug\ /y
: @IF ERRORLEVEL 1 GOTO lblError

: IF NOT EXIST projects\win32\release\ MKDIR projects\win32\release\
: xcopy "resources\%__ProductName%.k2s" projects\win32\release\ /y
: @IF ERRORLEVEL 1 GOTO lblError

SET bCopy=0
IF NOT EXIST include\resources.h (
	SET bCopy=1
) ELSE (
	FC resources\resources.h include\resources.h > NUL
	IF ERRORLEVEL 1 SET bCopy=1
)
IF "%bCopy%"=="0" ECHO Resources.h unchanged: Skipped replace.
IF "%bCopy%"=="1" xcopy resources\resources.h include\ /y
@IF ERRORLEVEL 1 GOTO lblError

SET bCopy=0
IF NOT EXIST source\ResourcesInc.cpp (
	SET bCopy=1
) ELSE (
	FC resources\ResourcesInc.cpp source\ResourcesInc.cpp > NUL
	IF ERRORLEVEL 1 SET bCopy=1
)
IF "%bCopy%"=="0" ECHO ResourcesInc.cpp unchanged: Skipped replace.
IF "%bCopy%"=="1" XCOPY resources\ResourcesInc.cpp source\ /y
@IF ERRORLEVEL 1 GOTO lblError

COLOR 2f
ECHO.
ECHO Success!
PAUSE
GOTO lblEnd


:lblError
@ECHO Errorlevel %errorlevel%!
COLOR 4f
@PAUSE


:lblEnd
SET __ProductName=