echo off
rem Generating C++ code from slrilibstruc ASN.1 spec.

set ROOT=C:\code

set TOOL=%ROOT%\ncbi_cxx\compilers\msvc_prj\serial\datatool\DebugMT\datatool.exe
set OBJ=%ROOT%\ncbi_cxx\src\objects
set INTERNAL=%ROOT%\slri\lib\asn
set MODULE=%INTERNAL%\slrilibstruc.asn
set SLRILIBSTRUC=..\..\lib\asn
set M1=%SLRILIBSTRUC%\slrilibstruc.asn
set M2=%OBJ%\cdd\cdd.asn
set M3=%OBJ%\pub\pub.asn
set M4=%OBJ%\seqset\seqset.asn
set M5=%OBJ%\seqfeat\seqfeat.asn
set M6=%OBJ%\seqloc\seqloc.asn
set M7=%OBJ%\mmdb1\mmdb1.asn
set M8=%OBJ%\mmdb3\mmdb3.asn
set M9=%OBJ%\seq\seq.asn
set M10=%OBJ%\seqalign\seqalign.asn

%TOOL% -m %MODULE% -M "%M10% %M9% %M6% %M7% %M4% %M1% %M2%" -of asn2slrilibstruc.win.log -oA -oph ..\include -opc ..\src -od slrilibstruc.def
rem move *.hpp ..\include
rem move *.cpp ..\src

