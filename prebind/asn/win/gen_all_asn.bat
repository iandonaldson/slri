ECHO ON
set CHECK_ARGS=..\..\..\ncbi\build.ms ..\..\..\ncbi\asn ..\..\..\ncbi\biostruc ..\..\..\ncbi\include

cd ..
call win\check_prebind_spec.bat %CHECK_ARGS%
del ..\lib\object\prebind_spec.h
del ..\lib\object\objprebind.c
del ..\lib\object\objprebind.h
move *.c ..\lib\object
move *.h ..\lib\object

cd win

set CHECK_ARGS=
