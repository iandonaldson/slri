echo OFF
rem Usage: gen_slristruc <path to NCBI main directory>
rem asntool -m slristruc.asn -Gt -M g:\code\ncbi\asn\asn.all,g:\code\ncbi\biostruc\mmdb2.asn,g:\code\ncbi\biostruc\mmdb3.asn,g:\code\ncbi\biostruc\mmdb1.asn,g:\code\ncbi\biostruc\cdd\cdd.asn -B objslriasn -w 128 -b10000

asntool -m slristruc.asn -o slristruc.h
asntool -m slristruc.asn -Gt -M ..\..\lib\asn\slrilibstruc.asn,%1\asn\asn.all,%1\biostruc\mmdb2.asn,%1\biostruc\mmdb3.asn,%1\biostruc\mmdb1.asn,%1\biostruc\cdd\cdd.asn -B objslristruc -w 128 -b10000

echo #include ^<objsset.h^> > temp1
echo #include ^<mmdbapi.h^> >> temp1
echo #include ^<objslrilibstruc.h^> >> temp1
echo #include ^<objcdd.h^> >> temp1
ren  objslristruc.c temp2
type temp1 temp2 > objslristruc.c


echo #define NLM_GENERATED_CODE_PROTO > temp3
ren  objslristruc.h temp4
type temp3 temp4 > objslristruc.h

echo #define struct_Bioseq bioseq > temp5
ren  objslristruc.h temp6
type temp5 temp6 > objslristruc.h

del /q temp*

move objslristruc.c ..\src
move objslristruc.h ..\include
move slristruc.h ..\include

rem now for nblastasn

asntool -m NBlastasn.asn -o NBlastasn.h -w 120 -b 10000
asntool -m NBlastasn.asn -G t -M %1\asn\asn.all,%1\biostruc\mmdb3.asn,%1\biostruc\mmdb1.asn,%1\biostruc\mmdb2.asn -B objNBlastasn -w 100 -b 10000
move objNBlastasn.c ../src
move objNBlastasn.h ../include
move NBlastasn.h ../include


