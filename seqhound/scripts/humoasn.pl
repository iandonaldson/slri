#!/usr/bin/perl

# $Id: humoasn.pl,v 1.2 2003/09/25 20:41:29 kaca Exp $
#*******************************************************************************************
#  Copyright (C) 2000 Mount Sinai Hospital (MSH)
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License as
#  published by the Free Software Foundation; either version 2 of
#  the License, or any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
#  See the GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the
#         Free Software Foundation, Inc.,
#         59 Temple Place, Suite 330, Boston, MA
#         02111-1307  USA
#  or visit http://www.gnu.org/copyleft/gpl.html
#
#  SPECIAL EXCEPTIONS
#
#  As a special exception, Mount Sinai Hospital gives permission to
#  link this program with the following non-GPL programs or libraries,
#  and distribute the resulting executable, without including the source
#  code for these in the source distribution:
#
#  a) CodeBase 6.5 or greater from Sequiter Software Inc.
#
#  b) The C or C++ interface to Oracle 8.x or greater, from
#     Oracle Corporation or IBM DB2 UDB.
#
#
#  PROGRAM:
#
#
#  AUTHORS:
#           Katerina Michalickova and Christopher W.V. Hogue hogue@mshri.on.ca
#
#
#  REVISION/CONTRIBUTION NOTES:
#
#  June 1, 2000  Original MSH Revision.
#                Hogue Lab - University of Toronto Biochemistry
#                Department and the Samuel Lunenfeld Research
#                Institute, Mount Sinai Hospital
#                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
#                ftp://ftp.mshri.on.ca/
#
#
#  PUBLICATION TO CITE:
#
#
#  DESCRIPTION
#  processes protein.asn and rna.asn files for human and mouse
#  these files contain XP predicted proteins and XM predicted RNAs
#  the script wraps the original bunch of Seq-entries into Bioseq-set
#
#  first ARGUMENT is file name
#
#
#******************************************************************************************


$tempin = @ARGV[0];   
$fileout = "humoasn_temp.asn"; 

open (OUT, ">$fileout") || die "Cannot create out file.\n";
open (IN, @ARGV[0]) || die "Cannot open in file.\n";

$count = 1;

print OUT "Bioseq-set ::= {\n  seq-set {\n";

while (<IN>) {
     $line = $_;
     if($count == 1)
     {
        $line =~ s/Seq-entry ::= //;
     }
     else
     {
        $line =~ s/Seq-entry ::= /,\n/;
     }
     print OUT "$line";
     $count = $count + 1;
}

print OUT "} }\n";

close (IN);
close (OUT);
#rewrite the old file with the new one
system ("mv $fileout $tempin");


# $Log: humoasn.pl,v $
# Revision 1.2  2003/09/25 20:41:29  kaca
# removed organism specific naming for temporary output files
#
# Revision 1.1.1.4  2003/01/24 09:32:58  root
# Automated import from Sourceforge.net
#
# Revision 1.5  2003/01/23 20:06:26  kaca
# added binary mode command
#
# Revision 1.4  2002/11/07 13:38:31  kaca
# changed output file name
#
# Revision 1.3  2002/10/30 21:13:15  kaca
# removed second argument
#
# Revision 1.2  2002/10/29 22:40:13  kaca
# small fix
#
# Revision 1.1  2002/10/21 21:35:27  kaca
# initial import of script which makes XP and XM files readable by mother parser
#
