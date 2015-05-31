# Microsoft Developer Studio Project File - Name="slrilib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=slrilib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "slrilib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "slrilib.mak" CFG="slrilib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "slrilib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "slrilib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "slrilib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\build\Debug\"
# PROP Intermediate_Dir "..\..\build\Debug\slrilib"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W4 /Gm /GX /ZI /Od /I "..\\" /I "..\..\..\ncbi\include" /I "..\..\..\ncbi" /I "..\..\..\ncbi\asnlib" /I "..\..\..\ncbi\corelib" /I "..\..\..\ncbi\object" /I "..\..\..\ncbi\connect" /I "..\..\..\ncbi\api" /I "..\..\..\ncbi\biostruc" /I "..\..\..\ncbi\biostruc\cdd" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "slrilib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\build\Release\"
# PROP Intermediate_Dir "..\..\build\Release\slrilib"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W4 /GX /O2 /I "..\\" /I "..\..\..\ncbi\include" /I "..\..\..\ncbi" /I "..\..\..\ncbi\asnlib" /I "..\..\..\ncbi\corelib" /I "..\..\..\ncbi\object" /I "..\..\..\ncbi\connect" /I "..\..\..\ncbi\api" /I "..\..\..\ncbi\biostruc" /I "..\..\..\ncbi\biostruc\cdd" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /c
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

# Name "slrilib - Win32 Debug"
# Name "slrilib - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\objslrilibstruc.c
# ADD CPP /W1
# End Source File
# Begin Source File

SOURCE=..\objslrilinkset.c
# End Source File
# Begin Source File

SOURCE=..\slri_asn.c
# End Source File
# Begin Source File

SOURCE=..\slri_cluster.c
# End Source File
# Begin Source File

SOURCE=..\slri_cookie.c
# End Source File
# Begin Source File

SOURCE=..\slri_dagtree.c
# End Source File
# Begin Source File

SOURCE=..\slri_endec.c
# End Source File
# Begin Source File

SOURCE=..\slri_mail.c
# End Source File
# Begin Source File

SOURCE=..\slri_math.c
# End Source File
# Begin Source File

SOURCE=..\slri_misc.c
# End Source File
# Begin Source File

SOURCE=..\slri_prime.c
# End Source File
# Begin Source File

SOURCE=..\slri_rbtree.c
# End Source File
# Begin Source File

SOURCE=..\slri_set.c
# End Source File
# Begin Source File

SOURCE=..\slri_sstack.c
# End Source File
# Begin Source File

SOURCE=..\slri_stat.c
# End Source File
# Begin Source File

SOURCE=..\slri_www.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\objslrilibstruc.h
# End Source File
# Begin Source File

SOURCE=..\objslrilinkset.h
# End Source File
# Begin Source File

SOURCE=..\slri.h
# End Source File
# Begin Source File

SOURCE=..\slri_asn.h
# End Source File
# Begin Source File

SOURCE=..\slri_cluster.h
# End Source File
# Begin Source File

SOURCE=..\slri_cookie.h
# End Source File
# Begin Source File

SOURCE=..\slri_dagtree.h
# End Source File
# Begin Source File

SOURCE=..\slri_endec.h
# End Source File
# Begin Source File

SOURCE=..\slri_mail.h
# End Source File
# Begin Source File

SOURCE=..\slri_math.h
# End Source File
# Begin Source File

SOURCE=..\slri_misc.h
# End Source File
# Begin Source File

SOURCE=..\slri_prime.h
# End Source File
# Begin Source File

SOURCE=..\slri_rbtree.h
# End Source File
# Begin Source File

SOURCE=..\slri_set.h
# End Source File
# Begin Source File

SOURCE=..\slri_sstack.h
# End Source File
# Begin Source File

SOURCE=..\slri_stat.h
# End Source File
# Begin Source File

SOURCE=..\slri_www.h
# End Source File
# Begin Source File

SOURCE=..\slrilibstruc.h
# End Source File
# Begin Source File

SOURCE=..\slrilinkset.h
# End Source File
# End Group
# Begin Group "asn"

# PROP Default_Filter "asn"
# Begin Source File

SOURCE=..\asn\gen_slrilibstruc.bat
# End Source File
# Begin Source File

SOURCE=..\asn\makeasn
# End Source File
# Begin Source File

SOURCE=..\asn\slrilibstruc.asn
# End Source File
# Begin Source File

SOURCE=..\asn\slrilinkset.asn
# End Source File
# End Group
# Begin Group "other"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\make.slrilib
# End Source File
# Begin Source File

SOURCE=..\slri.mk
# End Source File
# End Group
# End Target
# End Project
