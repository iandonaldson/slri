# Microsoft Developer Studio Project File - Name="doc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Generic Project" 0x010a

CFG=doc - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "doc.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "doc.mak" CFG="doc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "doc - Win32 Release" (based on "Win32 (x86) Generic Project")
!MESSAGE "doc - Win32 Debug" (based on "Win32 (x86) Generic Project")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
MTL=midl.exe

!IF  "$(CFG)" == "doc - Win32 Release"

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

!ELSEIF  "$(CFG)" == "doc - Win32 Debug"

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

!ENDIF 

# Begin Target

# Name "doc - Win32 Release"
# Name "doc - Win32 Debug"
# Begin Group "text"

# PROP Default_Filter ".txt"
# Begin Source File

SOURCE=..\..\html\cfunctsk.txt
# End Source File
# Begin Source File

SOURCE=..\..\html\listoffuncts.txt
# End Source File
# Begin Source File

SOURCE=..\..\html\perlfuncts.txt
# End Source File
# Begin Source File

SOURCE=..\..\html\seekhead.txt
# End Source File
# Begin Source File

SOURCE=..\..\html\seektail.txt
# End Source File
# End Group
# Begin Group "perl"

# PROP Default_Filter ".pl"
# Begin Source File

SOURCE=..\..\html\gethttpcalls.pl
# End Source File
# Begin Source File

SOURCE=..\..\html\getlistfuncts.pl
# End Source File
# Begin Source File

SOURCE=..\..\html\makecfunctsdet.pl
# End Source File
# Begin Source File

SOURCE=..\..\html\makecfunctslist.pl
# End Source File
# Begin Source File

SOURCE=..\..\html\makehttpcalls.pl
# End Source File
# Begin Source File

SOURCE=..\..\html\makepfunctsdet.pl
# End Source File
# Begin Source File

SOURCE=..\..\html\makepfunctslist.pl
# End Source File
# End Group
# Begin Group "html"

# PROP Default_Filter ".html"
# Begin Source File

SOURCE=..\..\html\capi.html
# End Source File
# Begin Source File

SOURCE=..\..\html\help.html
# End Source File
# Begin Source File

SOURCE=..\..\html\index.html
# End Source File
# Begin Source File

SOURCE=..\..\html\perlapi.html
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\html\jezevcik4.gif
# End Source File
# End Target
# End Project
