# Microsoft Developer Studio Project File - Name="perl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=perl - Win32 none
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "perl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "perl.mak" CFG="perl - Win32 none"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "perl - Win32 none" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "perl___Win32_none"
# PROP BASE Intermediate_Dir "perl___Win32_none"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "perl___Win32_none"
# PROP Intermediate_Dir "perl___Win32_none"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x1009 /d "_DEBUG"
# ADD RSC /l 0x1009 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# Begin Target

# Name "perl - Win32 none"
# Begin Group "PERL_mod"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\perl\SeqHound.pm
# End Source File
# End Group
# Begin Group "PERL_test"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\perl\sample.org
# End Source File
# Begin Source File

SOURCE=..\..\perl\test.pl
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\perl\Makefile.PL
# End Source File
# Begin Source File

SOURCE=..\..\perl\MANIFEST
# End Source File
# Begin Source File

SOURCE=..\..\perl\README
# End Source File
# Begin Source File

SOURCE=..\..\perl\README_details
# End Source File
# End Target
# End Project
