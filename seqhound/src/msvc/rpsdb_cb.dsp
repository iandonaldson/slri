# Microsoft Developer Studio Project File - Name="rpsdb" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=rpsdb - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "rpsdb_cb.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "rpsdb_cb.mak" CFG="rpsdb - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "rpsdb - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "rpsdb - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "rpsdb - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "rpsdb___Win32_Debug"
# PROP BASE Intermediate_Dir "rpsdb___Win32_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\build\DebugCB\"
# PROP Intermediate_Dir "..\..\build\DebugCB\rpsdb"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /Gm /GX /ZI /Od /I "..\..\..\..\codebase\windows\c\source" /I "..\..\include" /I "..\..\..\lib" /I "..\..\..\..\bzip2" /I "..\..\taxon" /I "..\..\..\lib\object" /I "..\..\locuslink" /I "..\..\go" /I "..\..\..\..\ncbi\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /W4 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\..\lib" /I "..\..\..\..\bzip2" /I "..\..\taxon" /I "..\..\..\lib\object" /I "..\..\locuslink" /I "..\..\go" /I "..\..\..\..\ncbi\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x1009 /d "_DEBUG"
# ADD RSC /l 0x1009 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 shoundlocllib.lib slrilib.lib c4lib.lib bzlib.lib ncbimmdb.lib ncbicdr.lib ncbiacc.lib ncbiobj.lib ncbimain.obj ncbi.lib kernel32.lib user32.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"..\..\build\Debug\rpsdb_cb.exe" /pdbtype:sept /libpath:"..\..\..\..\ncbi\build_d.ms" /libpath:"..\..\..\build\debug" /libpath:"..\..\..\..\build\Debug" /libpath:"..\..\..\..\bzip2\Debug"
# ADD LINK32 ncbimmdb.lib ncbitool.lib ncbicdr.lib ncbiacc.lib ncbiobj.lib ncbimain.obj ncbi.lib kernel32.lib user32.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"..\..\build\DebugCB\rpsdb.exe" /pdbtype:sept /libpath:"..\..\..\..\ncbi\build_wd.ms" /libpath:"..\..\..\..\ncbi\build_d.ms" /libpath:"..\..\..\build\debug" /libpath:"..\..\..\..\build\Debug" /libpath:"..\..\..\..\bzip2\Debug"

!ELSEIF  "$(CFG)" == "rpsdb - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "rpsdb___Win32_Release"
# PROP BASE Intermediate_Dir "rpsdb___Win32_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\build\ReleaseCB\"
# PROP Intermediate_Dir "..\..\build\ReleaseCB\rpsdb"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /GX /O2 /I "..\..\..\..\codebase\windows\c\source" /I "..\..\include" /I "..\..\..\lib" /I "..\..\..\..\bzip2" /I "..\..\taxon" /I "..\..\..\lib\object" /I "..\..\locuslink" /I "..\..\go" /I "..\..\..\..\ncbi\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W4 /GX /O2 /I "..\..\..\..\codebase\windows\c\source" /I "..\..\include" /I "..\..\..\lib" /I "..\..\..\..\bzip2" /I "..\..\taxon" /I "..\..\..\lib\object" /I "..\..\locuslink" /I "..\..\go" /I "..\..\..\..\ncbi\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /c
# ADD BASE RSC /l 0x1009 /d "NDEBUG"
# ADD RSC /l 0x1009 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 shoundlocllib.lib slrilib.lib c4lib.lib bzlib.lib ncbicdr.lib ncbiacc.lib ncbiobj.lib ncbimain.obj ncbi.lib kernel32.lib user32.lib /nologo /subsystem:console /machine:I386 /nodefaultlib:"libc.lib" /out:"..\..\build\Release\rpsdb_cb.exe" /libpath:"..\..\..\..\ncbi\build.ms" /libpath:"..\..\..\build\release" /libpath:"..\..\..\..\build\release" /libpath:"..\..\..\..\bzip2\release"
# ADD LINK32 ncbimmdb.lib ncbitool.lib ncbicdr.lib ncbiacc.lib ncbiobj.lib ncbimain.obj ncbi.lib kernel32.lib user32.lib wsock32.lib /nologo /subsystem:console /machine:I386 /nodefaultlib:"libc.lib" /out:"..\..\build\ReleaseCB\rpsdb.exe" /libpath:"..\..\..\..\ncbi\build.ms" /libpath:"..\..\..\build\release" /libpath:"..\..\..\..\build\release" /libpath:"..\..\..\..\bzip2\release"
# SUBTRACT LINK32 /map /debug

!ENDIF 

# Begin Target

# Name "rpsdb - Win32 Debug"
# Name "rpsdb - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\rps\rpsdb.c
# End Source File
# Begin Source File

SOURCE=..\rpsdb_cb.c
# End Source File
# Begin Source File

SOURCE=..\rpsdbapi.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\rps\rpsdb.h
# End Source File
# Begin Source File

SOURCE=..\..\include\rpsdb_cb.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
