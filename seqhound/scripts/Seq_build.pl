# builds SeqHound on Windows platform

# $Id: Seq_build.pl,v 1.1.1.2 2002/01/18 09:24:04 root Exp $
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
#           Ruth Isserlin, Katerina Michalickova and Christopher W.V. Hogue hogue@mshri.on.ca
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
#  builds SeqHound databases
#
#******************************************************************************************

#open up the file that contains a list of all files that have to be dealt with
open(LIST, "files.list") || die "Cannot open asn.list\n";
open(LOG, ">seq_build.log") || die "Cannot open seq_build.log\n";

$flag = 'TRUE';

#loop that goes through all the files listed in the file.
#Files listed are all of .aso.Z ending.  Those are the only files we have
#downloaded.  (dictated by asnftp1.pl script)
while(<LIST>){
	$file = $_;

	#get rid of the newline character.
      @line = split(/\s+/, $file);
      $file = $line[0];

	#uncompress the file
	if(($status = system("gunzip",$file)) != 0){
		print LOG "cannot unzip $file\n";
	}

	#get rid of the .Z at the end of the name.
	@words = split(/\./, $file);
	$file = $words[0] . "\.$words[1]";

	#run the programs mother on each of the files
	#if it the first time running the program the databases
	#have to be intialized.(last flag is T instead of F)
	if($flag eq 'TRUE'){
		$status = system("mother", "-i $file -r 117 -n T -m T");
		print LOG "MOTHER -> File: $file returned: $status\n";
		$flag = 'FALSE';
	}
	else{
		$status = system("mother", "-i $file -r 117 -n T -m F");
		print LOG "MOTHER -> File: $file returned: $status\n";
	}

	#recompress the file
	if(($status = system("gzip",$file)) != 0){
		print LOG "cannot unzip $file\n";
	}

}

close(LOG);
close(FILES);
close(LIST);

# $Log: Seq_build.pl,v $
# Revision 1.1.1.2  2002/01/18 09:24:04  root
# Automated import from Sourceforge.net
#
# Revision 1.2  2002/01/17 17:35:21  kaca
# removed sendb executable
#
# Revision 1.1.1.1  2001/05/25 21:36:21  kaca
# initial import
#
# Revision 1.4  2001/02/23 20:42:25  kaca
# license change
#
# Revision 1.3  2000/10/19 16:11:18  kaca
# added CVS keywords and licenses
#		

