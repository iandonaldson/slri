# Microsoft Developer Studio Project File - Name="prebindsearcher" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=prebindsearcher - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "prebindsearcher.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "prebindsearcher.mak" CFG="prebindsearcher - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "prebindsearcher - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "prebindsearcher - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "prebindsearcher - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\prebind\build\release\"
# PROP Intermediate_Dir "..\..\..\prebind\build\release\prebindsearcher\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Od /I "..\..\..\..\codebase\windows\c\source" /I "..\..\..\..\slri\prebind\include" /I "..\..\..\..\slri\prebind\lib\object" /I "..\..\..\..\slri\lib" /I "..\..\..\..\ncbi\include" /I "..\..\..\..\slri\seqhound\include" /I "..\..\..\..\slri\bind\lib" /I "..\..\..\..\slri\bind\lib\object" /I "..\..\..\..\slri\textindex" /I "..\..\..\..\slri\tindex\object" /I "..\..\..\..\slri\mobidick\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x1009 /d "NDEBUG"
# ADD RSC /l 0x1009 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 c4lib.lib ncbicdr.lib ncbi.lib ncbimmdb.lib ncbiobj.lib ncbinacc.lib netcli.lib netentr.lib ncbimain.obj slrilib.lib prebindlib.lib bindlib.lib shoundremlib.lib user32.lib wsock32.lib /nologo /subsystem:console /machine:I386 /nodefaultlib:"libcmtd.lib" /nodefaultlib:"libcmt" /libpath:"..\..\..\..\codebase\windows\c\msc60" /libpath:"..\..\..\..\ncbi\build_d.ms" /libpath:"..\..\..\..\slri\build\release" /libpath:"..\..\..\..\slri\prebind\build\release"

!ELSEIF  "$(CFG)" == "prebindsearcher - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\prebind\build\debug\"
# PROP Intermediate_Dir "..\..\..\prebind\build\debug\prebindsearcher\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\..\codebase\windows\c\source" /I "..\..\..\..\slri\prebind\include" /I "..\..\..\..\slri\prebind\lib\object" /I "..\..\..\..\slri\lib" /I "..\..\..\..\ncbi\include" /I "..\..\..\..\slri\seqhound\include" /I "..\..\..\..\slri\bind\lib" /I "..\..\..\..\slri\bind\lib\object" /I "..\..\..\..\slri\textindex" /I "..\..\..\..\slri\tindex\object" /I "..\..\..\..\slri\mobidick\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x1009 /d "_DEBUG"
# ADD RSC /l 0x1009 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 c4lib.lib ncbicdr.lib ncbi.lib ncbimmdb.lib ncbiobj.lib ncbinacc.lib netcli.lib netentr.lib ncbimain.obj slrilib.lib prebindlib.lib bindlib.lib shoundremlib.lib user32.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"libcmtd.lib" /nodefaultlib:"libcmt" /pdbtype:sept /libpath:"..\..\..\..\codebase\windows\c\msc60" /libpath:"..\..\..\..\ncbi\build_wd.ms" /libpath:"..\..\..\..\slri\build\debug" /libpath:"..\..\..\..\slri\prebind\build\debug"

!ENDIF 

# Begin Target

# Name "prebindsearcher - Win32 Release"
# Name "prebindsearcher - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\prebind_searcher.c
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
