# Microsoft Developer Studio Project File - Name="prebindlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=prebindlib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "prebindlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "prebindlib.mak" CFG="prebindlib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "prebindlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "prebindlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "prebindlib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\prebind\build\release\"
# PROP Intermediate_Dir "..\..\..\prebind\build\release\prebindlib\"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\..\codebase\windows\c\source" /I "..\..\..\..\slri\prebind\include" /I "..\..\..\..\slri\prebind\lib\object" /I "..\..\..\..\slri\lib" /I "..\..\..\..\ncbi\include" /I "..\..\..\..\slri\seqhound\include" /I "..\..\..\..\slri\bind\lib" /I "..\..\..\..\slri\bind\lib\object" /I "..\..\..\..\slri\textindex" /I "..\..\..\..\slri\tindex\object" /I "..\..\..\..\slri\mobidick\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x1009 /d "NDEBUG"
# ADD RSC /l 0x1009 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "prebindlib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\prebind\build\debug\"
# PROP Intermediate_Dir "..\..\..\prebind\build\debug\prebindlib\"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\..\codebase\windows\c\source" /I "..\..\..\..\slri\prebind\include" /I "..\..\..\..\slri\prebind\lib\object" /I "..\..\..\..\slri\lib" /I "..\..\..\..\ncbi\include" /I "..\..\..\..\slri\seqhound\include" /I "..\..\..\..\slri\bind\lib" /I "..\..\..\..\slri\bind\lib\object" /I "..\..\..\..\slri\textindex" /I "..\..\..\..\slri\tindex\object" /I "..\..\..\..\slri\mobidick\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x1009 /d "_DEBUG"
# ADD RSC /l 0x1009 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "prebindlib - Win32 Release"
# Name "prebindlib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\object\objprebind.c
# End Source File
# Begin Source File

SOURCE=..\prebind_db.c
# End Source File
# Begin Source File

SOURCE=..\prebind_lib.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
