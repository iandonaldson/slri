# Microsoft Developer Studio Project File - Name="shoundremlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=shoundremlib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "shoundremlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "shoundremlib.mak" CFG="shoundremlib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "shoundremlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "shoundremlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "shoundremlib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\build\Debug\"
# PROP Intermediate_Dir "..\..\..\build\Debug\shoundremlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W4 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\..\lib" /I "..\..\..\lib\object" /I "..\..\..\..\ncbi\include" /I "..\..\..\..\ncbi" /I "..\..\..\..\ncbi\asnlib" /I "..\..\..\..\ncbi\corelib" /I "..\..\..\..\ncbi\object" /I "..\..\..\..\ncbi\biostruc" /I "..\..\..\..\ncbi\biostruc\cdd" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "SEQHOUNDNBRS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "shoundremlib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\build\Release\"
# PROP Intermediate_Dir "..\..\..\build\Release\shoundremlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W4 /GX /O2 /I "..\..\include" /I "..\..\..\lib" /I "..\..\..\lib\object" /I "..\..\..\..\ncbi\include" /I "..\..\..\..\ncbi" /I "..\..\..\..\ncbi\asnlib" /I "..\..\..\..\ncbi\corelib" /I "..\..\..\..\ncbi\object" /I "..\..\..\..\ncbi\biostruc" /I "..\..\..\..\ncbi\biostruc\cdd" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "SEQHOUNDNBRS" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "shoundremlib - Win32 Debug"
# Name "shoundremlib - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\intrez_misc.c
# End Source File
# Begin Source File

SOURCE=..\objNBlastasn.c
# ADD CPP /W1
# End Source File
# Begin Source File

SOURCE=..\objslristruc.c
# ADD CPP /W1
# End Source File
# Begin Source File

SOURCE=..\seq_util.c
# End Source File
# Begin Source File

SOURCE=..\seqhoundapi.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\include\intrez_misc.h
# End Source File
# Begin Source File

SOURCE=..\..\include\objslristruc.h
# End Source File
# Begin Source File

SOURCE=..\..\include\seqhound.h
# End Source File
# Begin Source File

SOURCE=..\..\include\seqhoundapi.h
# End Source File
# End Group
# End Target
# End Project
