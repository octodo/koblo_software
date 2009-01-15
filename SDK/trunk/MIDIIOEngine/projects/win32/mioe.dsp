# Microsoft Developer Studio Project File - Name="MIDIIOEngine" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=MIDIIOEngine - Win32 DebugDirectX
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mioe.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mioe.mak" CFG="MIDIIOEngine - Win32 DebugDirectX"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MIDIIOEngine - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "MIDIIOEngine - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Release 2.0/008 Software Development/003 Source Code/INetSynth/projects/win32", ZOKAAAAA"
# PROP Scc_LocalPath "..\..\..\inetsynth\projects\win32"
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MIDIIOEngine - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "..\..\..\BaseUtilities\interface" /I "..\..\interface" /I "..\..\include" /I "..\..\..\DataEngine\interface" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"mioeInternal.h" /FD /c
# ADD BASE RSC /l 0x406 /d "NDEBUG"
# ADD RSC /l 0x406 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "MIDIIOEngine - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "..\..\..\BaseUtilities\interface" /I "..\..\interface" /I "..\..\include" /I "..\..\..\DataEngine\interface" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"mioeInternal.h" /FD /GZ /c
# ADD BASE RSC /l 0x406 /d "_DEBUG"
# ADD RSC /l 0x406 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "MIDIIOEngine - Win32 Release"
# Name "MIDIIOEngine - Win32 Debug"
# Begin Group "Interface"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\interface\IDeviceManager.h
# End Source File
# Begin Source File

SOURCE=..\..\interface\IIDevice.h
# End Source File
# Begin Source File

SOURCE=..\..\interface\IIDeviceCallback.h
# End Source File
# Begin Source File

SOURCE=..\..\interface\IODevice.h
# End Source File
# Begin Source File

SOURCE=..\..\interface\mioe.h
# End Source File
# End Group
# Begin Group "include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\CDeviceManagerWin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\CIDeviceWin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\CODeviceWin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\mioeInternal.h
# End Source File
# End Group
# Begin Group "source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\source\CDeviceManagerWin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\CIDeviceWin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\CODeviceWin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\mioeInternal.cpp
# ADD CPP /Yc"mioeInternal.h"
# End Source File
# End Group
# End Target
# End Project
