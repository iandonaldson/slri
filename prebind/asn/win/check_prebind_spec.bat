rem Usage: check_bind <path to asntool.exe> <path to NCBI main asn.1 files> <path to NCBI MMDB asn.1 files> <path to NCBI includes>

%1\asntool -m prebind_spec.asn -o prebind_spec.h -w 120 -b10000
%1\asntool -m prebind_spec.asn -Gt -B objprebind -w120 -b10000 -I objgen.h




rem for specs that import types from ncbi use: %1\asntool -m prebind_spec.asn -Gt -M %2\asn.all,%3\mmdb1.asn,%3\mmdb2.asn,%3\mmdb3.asn -B objprebind -w120 -b10000 -I %4\objgen.h
