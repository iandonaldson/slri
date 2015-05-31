echo off
rem Generating C++ code from ASN spec.

set TOOL=F:\ncbi_cxx\compilers\msvc_prj\serial\datatool\Debug\datatool.exe
set OBJ=F:\ncbi_cxx\src\objects
set INTERNAL=F:\slri\seqhound\asn

set M1=%INTERNAL%\NBlastasn.asn 
set M2=%OBJ%\cdd\cdd.asn
set M3=%OBJ%\pub\pub.asn
set M4=%OBJ%\seqset\seqset.asn
set M5=%OBJ%\seqfeat\seqfeat.asn
set M6=%OBJ%\seqloc\seqloc.asn
set M7=%OBJ%\mmdb1\mmdb1.asn
set M8=%OBJ%\mmdb3\mmdb3.asn
set M9=%OBJ%\seq\seq.asn
set M10=%OBJ%\seqalign\seqalign.asn

%TOOL% -m %M1% -of asn2nblastasn.win.log -oA -oph ..\include_cxx -opc ..\src_cxx -od nblast.def

