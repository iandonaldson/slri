# Microsoft Developer Studio Project File - Name="vastblst" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=vastblst - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vastblst_cb.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vastblst_cb.mak" CFG="vastblst - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vastblst - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "vastblst - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vastblst - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vastblst___Win32_Debug"
# PROP BASE Intermediate_Dir "vastblst___Win32_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\build\DebugCB\"
# PROP Intermediate_Dir "..\..\build\DebugCB\vastblst"
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
# ADD BASE LINK32 shoundlocllib.lib slrilib.lib c4lib.lib bzlib.lib ncbimmdb.lib ncbicdr.lib ncbiacc.lib ncbiobj.lib ncbimain.obj ncbi.lib kernel32.lib user32.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"..\..\build\Debug\vastblst_cb.exe" /pdbtype:sept /libpath:"..\..\..\..\ncbi\build_d.ms" /libpath:"..\..\..\build\debug" /libpath:"..\..\..\..\build\Debug" /libpath:"..\..\..\..\bzip2\Debug"
# ADD LINK32 ncbimmdb.lib ncbicdr.lib ncbiacc.lib ncbiobj.lib ncbimain.obj ncbi.lib kernel32.lib user32.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"..\..\build\DebugCB\vastblst.exe" /pdbtype:sept /libpath:"..\..\..\..\ncbi\build_d.ms" /libpath:"..\..\..\build\debug" /libpath:"..\..\..\..\build\Debug" /libpath:"..\..\..\..\bzip2\Debug"

!ELSEIF  "$(CFG)" == "vastblst - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vastblst___Win32_Release"
# PROP BASE Intermediate_Dir "vastblst___Win32_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\build\ReleaseCB\"
# PROP Intermediate_Dir "..\..\build\ReleaseCB\vastblst"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /GX /O2 /I "..\..\..\..\codebase\windows\c\source" /I "..\..\include" /I "..\..\..\lib" /I "..\..\..\..\bzip2" /I "..\..\taxon" /I "..\..\..\lib\object" /I "..\..\locuslink" /I "..\..\go" /I "..\..\..\..\ncbi\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W4 /GX /O2 /I "..\..\..\..\codebase\windows\c\source" /I "..\..\include" /I "..\..\..\lib" /I "..\..\..\..\bzip2" /I "..\..\taxon" /I "..\..\..\lib\object" /I "..\..\locuslink" /I "..\..\go" /I "..\..\..\..\ncbi\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x1009 /d "NDEBUG"
# ADD RSC /l 0x1009 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 shoundlocllib.lib slrilib.lib c4lib.lib bzlib.lib ncbicdr.lib ncbiacc.lib ncbiobj.lib ncbimain.obj ncbi.lib kernel32.lib user32.lib /nologo /subsystem:console /machine:I386 /nodefaultlib:"libc.lib" /out:"..\..\build\Release\vastblst_cb.exe" /libpath:"..\..\..\..\ncbi\build.ms" /libpath:"..\..\..\build\release" /libpath:"..\..\..\..\build\release" /libpath:"..\..\..\..\bzip2\release"
# ADD LINK32 ncbimmdb.lib ncbicdr.lib ncbiacc.lib ncbiobj.lib ncbimain.obj ncbi.lib kernel32.lib user32.lib wsock32.lib /nologo /subsystem:console /machine:I386 /nodefaultlib:"libc.lib" /out:"..\..\build\ReleaseCB\vastblst.exe" /libpath:"..\..\..\..\ncbi\build.ms" /libpath:"..\..\..\build\release" /libpath:"..\..\..\..\build\release" /libpath:"..\..\..\..\bzip2\release"

!ENDIF 

# Begin Target

# Name "vastblst - Win32 Debug"
# Name "vastblst - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\domains\vastblst.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
