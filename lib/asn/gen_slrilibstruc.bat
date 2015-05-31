echo OFF
rem Usage: gen_slriasn <path to asntool.exe> <path to NCBI main directory>
rem asntool -m slriasn.asn -Gt -M c:\code\ncbi\asn\asn.all,c:\code\ncbi\biostruc\mmdb2.asn,c:\code\ncbi\biostruc\mmdb3.asn,c:\code\ncbi\biostruc\mmdb1.asn -B objslriasn -w 128 -b10000

asntool -m slrilibstruc.asn -o slrilibstruc.h 
asntool -m slrilibstruc.asn -Gt -M %1\asn\asn.all,%1\biostruc\mmdb2.asn,%1\biostruc\mmdb3.asn,%1\biostruc\mmdb1.asn -B objslrilibstruc -w 128 -b10000

echo #include ^<objsset.h^> > temp1
echo #include ^<mmdbapi.h^> >> temp1
ren  objslrilibstruc.c temp2
type temp1 temp2 > objslrilibstruc.c 


echo #define NLM_GENERATED_CODE_PROTO > temp3
ren  objslrilibstruc.h temp4
type temp3 temp4 > objslrilibstruc.h

echo #define struct_Bioseq bioseq > temp5
ren  objslrilibstruc.h temp6
type temp5 temp6 > objslrilibstruc.h

del /q temp*


move objslrilibstruc.c ..\
move objslrilibstruc.h ..\
move slrilibstruc.h ..\

