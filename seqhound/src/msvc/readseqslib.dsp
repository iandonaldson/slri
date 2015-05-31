# Microsoft Developer Studio Project File - Name="readseqslib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=readseqslib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "readseqslib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "readseqslib.mak" CFG="readseqslib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "readseqslib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "readseqslib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "readseqslib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "readseqslib___Win32_Debug"
# PROP BASE Intermediate_Dir "readseqslib___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\build\DebugCB\"
# PROP Intermediate_Dir "..\..\build\DebugCB\readseqslib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /Gm /GX /ZI /Od /I "..\..\..\..\ncbi\include" /I "..\..\..\..\codebase\windows\c\source" /I "..\..\include" /I "..\..\..\lib" /I "..\..\..\..\bzip2" /I "..\..\taxon" /I "..\..\..\lib\object" /I "..\..\locuslink" /I "..\..\go" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /GZ /c
# ADD CPP /nologo /W4 /Gm /GX /ZI /Od /I "..\..\examples" /I "..\..\include" /I "..\..\..\lib" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /GZ /c
# ADD BASE RSC /l 0x1009 /d "_DEBUG"
# ADD RSC /l 0x1009 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\..\build\Debug\readseqslib_cb.lib"
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "readseqslib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "readseqslib___Win32_Release"
# PROP BASE Intermediate_Dir "readseqslib___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\build\ReleaseCB\"
# PROP Intermediate_Dir "..\..\build\ReleaseCB\readseqslib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /GX /O2 /I "..\..\..\..\ncbi\include" /I "..\..\..\..\codebase\windows\c\source" /I "..\..\include" /I "..\..\..\lib" /I "..\..\..\..\bzip2" /I "..\..\taxon" /I "..\..\..\lib\object" /I "..\..\locuslink" /I "..\..\go" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W4 /GX /O2 /I "..\..\examples" /I "..\..\include" /I "..\..\..\lib" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x1009 /d "NDEBUG"
# ADD RSC /l 0x1009 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\..\build\Release\readseqslib_cb.lib"
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "readseqslib - Win32 Debug"
# Name "readseqslib - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\examples\readseqs.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\examples\readseqs.h
# End Source File
# End Group
# End Target
# End Project
