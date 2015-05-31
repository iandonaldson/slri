# Microsoft Developer Studio Project File - Name="goparser" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=goparser - Win32 DebugDB2
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "goparser.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "goparser.mak" CFG="goparser - Win32 DebugDB2"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "goparser - Win32 DebugCB" (based on "Win32 (x86) Console Application")
!MESSAGE "goparser - Win32 ReleaseCB" (based on "Win32 (x86) Console Application")
!MESSAGE "goparser - Win32 ReleaseDB2" (based on "Win32 (x86) Console Application")
!MESSAGE "goparser - Win32 DebugDB2" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "goparser - Win32 DebugCB"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "goparser___Win32_DebugCB"
# PROP BASE Intermediate_Dir "goparser___Win32_DebugCB"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\build\Debug"
# PROP Intermediate_Dir "..\..\..\build\Debug\goparser_cb"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\..\codebase\windows\c\source" /I "..\..\include" /I "..\..\..\lib" /I "..\..\..\..\bzip2" /I "..\..\taxon" /I "..\..\..\lib\object" /I "..\..\locuslink" /I "..\..\go" /I "..\..\..\..\ncbi\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\..\codebase\windows\c\source" /I "..\..\include" /I "..\..\..\lib" /I "..\..\..\..\bzip2" /I "..\..\taxon" /I "..\..\..\lib\object" /I "..\..\locuslink" /I "..\..\go" /I "..\..\..\..\ncbi\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 shoundlocllib.lib slrilib.lib c4lib.lib bzlib.lib ncbicdr.lib ncbiacc.lib ncbiobj.lib ncbimain.obj ncbi.lib kernel32.lib user32.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"libcd.lib" /pdbtype:sept /libpath:"..\..\..\..\ncbi\build_d.ms" /libpath:"..\..\..\build\debug" /libpath:"..\..\..\..\codebase\windows\C\MSC60" /libpath:"..\..\..\..\bzip2\Debug"
# ADD LINK32 shoundlocllib_cb.lib slrilib_cb.lib c4lib.lib bzlib.lib ncbimmdb.lib ncbicdr.lib ncbiacc.lib ncbiobj.lib ncbimain.obj ncbi.lib kernel32.lib user32.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"libcd.lib" /pdbtype:sept /libpath:"..\..\..\..\ncbi\build_d.ms" /libpath:"..\..\..\..\build\debug" /libpath:"..\..\..\build\debug" /libpath:"..\..\..\..\codebase\windows\C\MSC60" /libpath:"..\..\..\..\bzip2\Debug"

!ELSEIF  "$(CFG)" == "goparser - Win32 ReleaseCB"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "goparser___Win32_ReleaseCB"
# PROP BASE Intermediate_Dir "goparser___Win32_ReleaseCB"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\build\Release"
# PROP Intermediate_Dir "..\..\..\build\Release\goparser_cb"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\..\codebase\windows\c\source" /I "..\..\include" /I "..\..\..\lib" /I "..\..\..\..\bzip2" /I "..\..\taxon" /I "..\..\..\lib\object" /I "..\..\locuslink" /I "..\..\go" /I "..\..\..\..\ncbi\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\..\codebase\windows\c\source" /I "..\..\include" /I "..\..\..\lib" /I "..\..\..\..\bzip2" /I "..\..\taxon" /I "..\..\..\lib\object" /I "..\..\locuslink" /I "..\..\go" /I "..\..\..\..\ncbi\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 shoundlocllib.lib slrilib.lib c4lib.lib bzlib.lib ncbimmdb.lib ncbicdr.lib ncbiacc.lib ncbiobj.lib ncbimain.obj ncbi.lib kernel32.lib user32.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"libcd.lib" /pdbtype:sept /libpath:"..\..\..\..\ncbi\build_d.ms" /libpath:"..\..\..\..\build\debug" /libpath:"..\..\..\build\debug" /libpath:"..\..\..\..\codebase\windows\C\MSC60" /libpath:"..\..\..\..\bzip2\Debug"
# ADD LINK32 shoundlocllib_cb.lib slrilib_cb.lib c4lib.lib bzlib.lib ncbimmdb.lib ncbicdr.lib ncbiacc.lib ncbiobj.lib ncbimain.obj ncbi.lib kernel32.lib user32.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"libcd.lib" /pdbtype:sept /libpath:"..\..\..\..\ncbi\build_d.ms" /libpath:"..\..\..\..\build\debug" /libpath:"..\..\..\build\debug" /libpath:"..\..\..\..\codebase\windows\C\MSC60" /libpath:"..\..\..\..\bzip2\Debug"

!ELSEIF  "$(CFG)" == "goparser - Win32 ReleaseDB2"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "goparser___Win32_ReleaseDB2"
# PROP BASE Intermediate_Dir "goparser___Win32_ReleaseDB2"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\build\Release"
# PROP Intermediate_Dir "..\..\..\build\Release\goparser_db2"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\..\codebase\windows\c\source" /I "..\..\include" /I "..\..\..\lib" /I "..\..\..\..\bzip2" /I "..\..\taxon" /I "..\..\..\lib\object" /I "..\..\locuslink" /I "..\..\go" /I "..\..\..\..\ncbi\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\..\codebase\windows\c\source" /I "..\..\include" /I "..\..\..\lib" /I "..\..\..\..\bzip2" /I "..\..\taxon" /I "..\..\..\lib\object" /I "..\..\locuslink" /I "..\..\go" /I "..\..\..\..\ncbi\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 shoundlocllib.lib slrilib.lib c4lib.lib bzlib.lib ncbimmdb.lib ncbicdr.lib ncbiacc.lib ncbiobj.lib ncbimain.obj ncbi.lib kernel32.lib user32.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"libcd.lib" /pdbtype:sept /libpath:"..\..\..\..\ncbi\build_d.ms" /libpath:"..\..\..\..\build\debug" /libpath:"..\..\..\build\debug" /libpath:"..\..\..\..\codebase\windows\C\MSC60" /libpath:"..\..\..\..\bzip2\Debug"
# ADD LINK32 shoundlocllib_db2.lib slrilib_cb.lib db2cli.lib bzlib.lib ncbimmdb.lib ncbicdr.lib ncbiacc.lib ncbiobj.lib ncbimain.obj ncbi.lib kernel32.lib user32.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"libcd.lib" /pdbtype:sept /libpath:"..\..\..\..\ncbi\build_d.ms" /libpath:"..\..\..\..\build\debug" /libpath:"..\..\..\build\debug" /libpath:"..\..\..\..\codebase\windows\C\MSC60" /libpath:"..\..\..\..\bzip2\Debug"

!ELSEIF  "$(CFG)" == "goparser - Win32 DebugDB2"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "goparser___Win32_DebugDB2"
# PROP BASE Intermediate_Dir "goparser___Win32_DebugDB2"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\build\Debug\"
# PROP Intermediate_Dir "..\..\..\build\Debug\goparser_db2"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\..\codebase\windows\c\source" /I "..\..\include" /I "..\..\..\lib" /I "..\..\..\..\bzip2" /I "..\..\taxon" /I "..\..\..\lib\object" /I "..\..\locuslink" /I "..\..\go" /I "..\..\..\..\ncbi\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\..\codebase\windows\c\source" /I "..\..\include" /I "..\..\..\lib" /I "..\..\..\..\bzip2" /I "..\..\taxon" /I "..\..\..\lib\object" /I "..\..\locuslink" /I "..\..\go" /I "..\..\..\..\ncbi\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 shoundlocllib.lib slrilib.lib c4lib.lib bzlib.lib ncbimmdb.lib ncbicdr.lib ncbiacc.lib ncbiobj.lib ncbimain.obj ncbi.lib kernel32.lib user32.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"libcd.lib" /pdbtype:sept /libpath:"..\..\..\..\ncbi\build_d.ms" /libpath:"..\..\..\..\build\debug" /libpath:"..\..\..\build\debug" /libpath:"..\..\..\..\codebase\windows\C\MSC60" /libpath:"..\..\..\..\bzip2\Debug"
# ADD LINK32 shoundlocllib_db2.lib slrilib_db2.lib db2cli.lib bzlib.lib ncbimmdb.lib ncbicdr.lib ncbiacc.lib ncbiobj.lib ncbimain.obj ncbi.lib kernel32.lib user32.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"libcd.lib" /pdbtype:sept /libpath:"..\..\..\..\ncbi\build_d.ms" /libpath:"..\..\..\..\build\debug" /libpath:"..\..\..\build\debug" /libpath:"..\..\..\..\codebase\windows\C\MSC60" /libpath:"..\..\..\..\bzip2\Debug"

!ENDIF 

# Begin Target

# Name "goparser - Win32 DebugCB"
# Name "goparser - Win32 ReleaseCB"
# Name "goparser - Win32 ReleaseDB2"
# Name "goparser - Win32 DebugDB2"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\go\go_db_misc.c
# End Source File
# Begin Source File

SOURCE=..\..\go\go_parser.c
# End Source File
# Begin Source File

SOURCE=..\..\go\goparser.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\go\go_db_misc.h
# End Source File
# Begin Source File

SOURCE=..\..\go\go_parser.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
