==  WINDOWS ==

***Steps 1-4 can be executed by the gen_all_asn.bat batch file in the prebind/asn/win directory.


To compile the prebind object code in MSVC

1. Place prebind_spec.asn and asntool.exe in the same directory


2. At the command line type

	asntool -m prebind_spec.asn -o prebind_spec.h -w 120 -b 10000

   A file called prebind_spec.h should appear in the directory.

3. At the command line type

	asntool -m prebind_spec.asn -G t -B objprebind -w 120 -b 10000 -I objgen.h

   Two new files will appear: objprebind.h and objprebind.c

4. Place all generated files

	prebind_spec.h
	objsprebind.h
	objprebind.c
   
   into the directory prebind/lib/object    

5. In MSCV, select File/New/Project/Win32 Static library and 

	project name: prebindlib
	location: C:\code\slri\prebind\lib\msvc

6. In msvc, add the generated file objprebind.c to the 'Source files' by right clicking on the 'Source    files' icon in the files view and selecting 'Add files to project'.
   Similarily, add prebind_spec.h and objsprebind.h to the 'Header files'.

7. Under Project/Settings/CC++ add relative paths from the .dsp file (created by step 5 in the msvc directory) to the header files for the library and for ncbi includes; in this case:

	..\object,..\..\..\..\ncbi\include

8. Under Project/Settings/General set paths for generated intermediate and final library files. For example,
	
	INtermediate files
	..\..\.\build\debug\prebindlib

	Output files
	..\..\..\build\debug

   will place the final .lib file in the directory slri\build\debug in this example.

9. Hit the compile and build buttons.

10. Add the lines

	#include <prebind_spec.h>
	#include <objsprebind.h>

    to the header files used by the .c file of your program to have access to functions in the    library and 

11. For your project that uses this library, add the relative path from the .dsp file to the prebind.h     file.  In this case, 

	..\lib\object

    and

12. Add prebindlib.lib under Project/Settings/Link/Input/Object,library modules

13. Add the relative path from the project .dsp file to the prebindlib.lib (see step 8)     under Project/Settings/Link/Additional library path; in this case:

	..\..\build\Debug


== SOLARIS ==

1. compile ncbi tool kit, or find asntool if it already exists. 
Suppose the root directory to ncbi package is $NCBI;
and the root directory to prebind package is $PREBIND.

2. Generate prebind_spec.h
% asntool -m prebind_spec.asn -o prebind_spec.h -w 120 -b 10000
NOTE: use "asntool - " for options

3. Generate prebind ASN.1 API
% asntool -m prebind_spec.asn -G t -B objprebind -w 120 -b 10000 -I $NCBI/include/objgen.h -M $NCBI/asn/general.asn
Two new files will appear: objprebind.h and objprebind.c

4. Place all generated files

	prebind_spec.h
	objsprebind.h
	objprebind.c
   
   into the directory $PREBIND/lib/object    

5. In $PREBIND/make/prebind.mk
add path:
	PREBINDOBJS = $(PREBINDDIR)/lib/object
add path to include:
	INCDIRS = ... -I$(PREBINDOBJS) ...

6. Modify $PREBIND/make/make.prebindlib
add objprebind.o to prebindlib

LIB_OBJ = $(PREBINDLIB)/prebind_db.o $(PREBINDLIB)/prebind_lib.o $(PREBINDOBJS)/objprebind.o

7. Add the lines

	#include <prebind_spec.h>
	#include <objsprebind.h>

    to the header files used by the .c file of your program to have access to functions in the library and 

== asntool help ==

AsnTool 4   arguments:

  -m  ASN.1 Module File [File In]
  -f  ASN.1 Module File [File Out]  Optional
  -X  XML DTD File
	("m" to print each module to a separate file) [File Out]  Optional
  -T  ASN.1 Tree Dump File [File Out]  Optional
  -v  Print Value File [File In]  Optional
  -p  Print Value File [File Out]  Optional
  -x  XML Data File [File Out]  Optional
  -d  Binary Value File (type required) [File In]  Optional
  -t  Binary Value Type [String]  Optional
  -e  Binary Value File [File Out]  Optional
  -o  Header File [File Out]  Optional
  -l  Loader File [File Out]  Optional
  -b  Buffer Size [Integer]  Optional
    default = 1024
    range from 512 to 10000
  -w  Word length maximum for #defines [Integer]  Optional
    default = 31
    range from 31 to 128
  -G  Generate object loader .c and .h files, 
	if used, see below parameters: [T/F]  Optional
    default = F
  -M  ASN.1 module filenames, comma separated used for external refs from the 'm', but no other action taken [File In]  Optional
  -B  Base for filename, without extensions, for generated objects and code [File Out]  Optional
  -D  During code generation, debugging level
     0 - No debugging
     1 - Shallow debugging
     2 - Deep [Integer]  Optional
    default = 0
    range from 0 to 9
  -S  Debugging filename [File Out]  Optional
    default = stderr
  -I  In generated .c, add #include to this filename [String]  Optional
  -Z  Bit twiddle for optional zero value base slots [T/F]  Optional
    default = F
  -K  In generated .c, forces name of #included asn header [String]  Optional
  -J  Register type with object manager [String]  Optional
  -L  Label for registered type [String]  Optional
  -P  XML module prefix for DOCTYPE [String]  Optional
    default = 

== prebind_transfer ==


2. asntool -m prebind_transfer.asn -o prebind_transfer.h -w 120 -b 10000

3. asntool -m prebind_transfer.asn -G t -B objPBT -w 120 -b 10000 -I /export/software/64/ncbi/include/objgen.h -M /export/software/64/ncbi/asn/general.asn

