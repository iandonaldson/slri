# Microsoft Developer Studio Project File - Name="shoundlocllib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=shoundlocllib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "shoundlocllib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "shoundlocllib.mak" CFG="shoundlocllib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "shoundlocllib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "shoundlocllib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "shoundlocllib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "shoundlocllib___Win32_Debug"
# PROP BASE Intermediate_Dir "shoundlocllib___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "shoundlocllib___Win32_Debug"
# PROP Intermediate_Dir "shoundlocllib___Win32_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /Gm /GX /ZI /Od /I "..\..\..\..\ncbi\include" /I "..\..\..\..\codebase\windows\c\source" /I "..\..\include" /I "..\..\..\lib" /I "..\..\..\..\bzip2" /I "..\..\taxon" /I "..\..\..\lib\object" /I "..\..\locuslink" /I "..\..\go" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /GZ /c
# ADD CPP /nologo /W4 /Gm /GX /ZI /Od /I "..\..\..\..\ncbi\include" /I "..\..\..\..\codebase\windows\c\source" /I "..\..\include" /I "..\..\..\lib" /I "..\..\..\..\bzip2" /I "..\..\taxon" /I "..\..\..\lib\object" /I "..\..\locuslink" /I "..\..\go" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /GZ /c
# ADD BASE RSC /l 0x1009 /d "_DEBUG"
# ADD RSC /l 0x1009 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\..\build\Debug\shoundlocllib_cb.lib"
# ADD LIB32 /nologo /out:"..\..\..\build\Debug\shoundlocllib_cb.lib"

!ELSEIF  "$(CFG)" == "shoundlocllib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "shoundlocllib___Win32_Release"
# PROP BASE Intermediate_Dir "shoundlocllib___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "shoundlocllib___Win32_Release"
# PROP Intermediate_Dir "shoundlocllib___Win32_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /GX /O2 /I "..\..\..\..\ncbi\include" /I "..\..\..\..\codebase\windows\c\source" /I "..\..\include" /I "..\..\..\lib" /I "..\..\..\..\bzip2" /I "..\..\taxon" /I "..\..\..\lib\object" /I "..\..\locuslink" /I "..\..\go" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W4 /GX /O2 /I "..\..\..\..\ncbi\include" /I "..\..\..\..\codebase\windows\c\source" /I "..\..\include" /I "..\..\..\lib" /I "..\..\..\..\bzip2" /I "..\..\taxon" /I "..\..\..\lib\object" /I "..\..\locuslink" /I "..\..\go" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x1009 /d "NDEBUG"
# ADD RSC /l 0x1009 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\..\build\Release\shoundlocllib_cb.lib"
# ADD LIB32 /nologo /out:"..\..\..\build\Release\shoundlocllib_cb.lib"

!ENDIF 

# Begin Target

# Name "shoundlocllib - Win32 Debug"
# Name "shoundlocllib - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\go_query.c
# End Source File
# Begin Source File

SOURCE=..\intrez.c
# End Source File
# Begin Source File

SOURCE=..\intrez_cfg.c
# End Source File
# Begin Source File

SOURCE=..\intrez_misc.c
# End Source File
# Begin Source File

SOURCE=..\ll_query.c
# End Source File
# Begin Source File

SOURCE=..\mmdblocl.c
# End Source File
# Begin Source File

SOURCE=..\nbr.c
# End Source File
# Begin Source File

SOURCE=..\objll.c
# ADD CPP /W1
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

SOURCE=..\objslritax.c
# ADD CPP /W1
# End Source File
# Begin Source File

SOURCE=..\seq_util.c
# End Source File
# Begin Source File

SOURCE=..\slri_post.c
# End Source File
# Begin Source File

SOURCE=..\taxdb_cons.c
# End Source File
# Begin Source File

SOURCE=..\taxdb_enum.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\include\go_db.h
# End Source File
# Begin Source File

SOURCE=..\..\include\go_query.h
# End Source File
# Begin Source File

SOURCE=..\..\include\intrez.h
# End Source File
# Begin Source File

SOURCE=..\..\include\intrez_cb.h
# End Source File
# Begin Source File

SOURCE=..\..\include\intrez_cfg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\intrez_db.h
# End Source File
# Begin Source File

SOURCE=..\..\include\intrez_defs.h
# End Source File
# Begin Source File

SOURCE=..\..\include\intrez_misc.h
# End Source File
# Begin Source File

SOURCE=..\..\include\intrez_priv.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ll.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ll_db.h
# End Source File
# Begin Source File

SOURCE=..\..\locuslink\ll_db_misc.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ll_query.h
# End Source File
# Begin Source File

SOURCE=..\..\include\NBlastasn.h
# End Source File
# Begin Source File

SOURCE=..\..\include\nbr.h
# End Source File
# Begin Source File

SOURCE=..\..\include\nbr_db.h
# End Source File
# Begin Source File

SOURCE=..\..\locuslink\objll.h
# End Source File
# Begin Source File

SOURCE=..\..\include\objNBlastasn.h
# End Source File
# Begin Source File

SOURCE=..\..\include\objslristruc.h
# End Source File
# Begin Source File

SOURCE=..\..\include\objslritax.h
# End Source File
# Begin Source File

SOURCE=..\..\include\seqhound.h
# End Source File
# Begin Source File

SOURCE=..\..\include\slri_mmdblocl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\slri_post.h
# End Source File
# Begin Source File

SOURCE=..\..\include\slristruc.h
# End Source File
# Begin Source File

SOURCE=..\..\include\slritax.h
# End Source File
# Begin Source File

SOURCE=..\..\include\taxdb_cons.h
# End Source File
# Begin Source File

SOURCE=..\..\include\taxdb_db.h
# End Source File
# Begin Source File

SOURCE=..\..\include\taxdb_enum.h
# End Source File
# End Group
# Begin Group "cb"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\go_cb.c
# End Source File
# Begin Source File

SOURCE=..\..\include\go_db_p.h
# End Source File
# Begin Source File

SOURCE=..\intrez_cb.c
# End Source File
# Begin Source File

SOURCE=..\ll_cb.c
# End Source File
# Begin Source File

SOURCE=..\..\include\ll_db_p.h
# End Source File
# Begin Source File

SOURCE=..\nbr_cb.c
# End Source File
# Begin Source File

SOURCE=..\..\include\nbr_db_p.h
# End Source File
# Begin Source File

SOURCE=..\taxdb_cb.c
# End Source File
# Begin Source File

SOURCE=..\..\include\taxdb_db_p.h
# End Source File
# Begin Source File

SOURCE=..\..\include\taxdb_enum_p.h
# End Source File
# End Group
# Begin Group "db2"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\go_db2.c
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\intrez_db2.c
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\ll_db2.c
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\nbr_db2.c
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\taxdb_db2.c
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# End Group
# End Target
# End Project
