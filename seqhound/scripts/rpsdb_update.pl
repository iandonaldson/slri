#!/usr/bin/perl -w
#$Id: rpsdb_update.pl,v 1.1.1.2 2002/11/04 09:34:06 root Exp $

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
#  PROGRAM:# RPSDB update script
# 	Doron Betel (betel@mshri.on.ca)
# 	SLRI, Toronto, Canada
# 	Oct. 29 ,2002
#
#
#  AUTHORS: Doron Betel betel@mshri.on.ca
#           and Christopher W.V. Hogue hogue@mshri.on.ca
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
#  cron which updates REDUND database
#
#
#******************************************************************************************

#########################################
#Paths and variables that need to be
#specified for every machine
#########################################
#admin mail
$admail = "betel\@mshri.on.ca";

#path to old REDUND table (no ending slash)
$path_old_table="/seqhound/update/old_redund_tables/";

#path to new REDUND table (no ending slash)
$path_new_table="/seqhound/datab/";

#path to the production RPSDB table
$path_production_rpsdb="/seqhound/rps/";

#path to old RPSDB table should be identical to
#the one in .interzrc
$path_update_rpsdb="/seqhound/rps/old_tables/";

#path to CDD matrices
$path_cdd="/seqhound/rps/cdd/";

#path to RpsBlastToDB
$path_rpsblast="/seqhound/update/RpsBlastToDB";

#path to comp_redund
$path_compredund="/seqhound/update/comp_redund";

$email="RPSDB update\n--------------------\n";
#run comp_redund
print "$path_compredund -o $path_old_table -n $path_new_table\n";
system("$path_compredund -o $path_old_table -n $path_new_table");
$email=$email."$path_compredund -o $path_old_table -n $path_new_table\n";

#produced files
$toblast=`echo *toblast`;
$todelete=`echo *todelete`;
chomp($toblast);
chomp($todelete);

print $toblast." ".$todelete."\n";
if($toblast eq "" or $todelete eq ""){
	$email=$email."redundant compare failed\n";
	print "redundant compare failed";
	&SendMail($email);
	exit;
}

#RPSDB arguments
$update_rpsdb_tables="MASTER";
$arguments="-i $toblast -d $todelete -m 2 -b F -p $path_cdd -f All -r $update_rpsdb_tables";

#run RPSBLAST
print "$path_rpsblast $arguments\n";
system("$path_rpsblast $arguments");
$email=$email."$path_rpsblast $arguments\n";
$update_rpsdb_tables=$update_rpsdb_tables."*";
$path_update_rpsdb=$path_update_rpsdb.$update_rpsdb_tables;

#remove files and move new tables to old table
print "cp  $path_update_rpsdb $path_production_rpsdb\n";
system("cp  $path_update_rpsdb $path_production_rpsdb");
$email=$email."cp  $update_rpsdb_tables $path_production_rpsdb\n";

$path_new_table=$path_new_table."REDUND*";
print "cp $path_new_table $path_old_table\n";
system("cp $path_new_table $path_old_table");
$email=$email."cp $path_new_table $path_old_table\n";

print "rm $toblast; rm $todelete\n";
system("rm $toblast; rm $todelete;");

$email=$email."rm $toblast; rm $todelete\n";
@logfile=`tail -20 rpsDB.log`;
&SendMail($email, @logfile);
system("rm rpsDB.log");

#Email sub routine
sub SendMail
{
	#$mymail = "| mail ".$admail;
	$subject="rpsdb update script messages";

	open(MAIL,"| mail -s \"$subject\" $admail") or die "Can't fork for mail: $!\n";
	for my $val (@_){
		print MAIL $val;
	}
	close(MAIL);
}

#$Log: rpsdb_update.pl,v $
#Revision 1.1.1.2  2002/11/04 09:34:06  root
#Automated import from Sourceforge.net
#
#Revision 1.2  2002/11/03 23:53:30  betel
#Fixed script errors
#
#Revision 1.1  2002/10/29 19:36:27  betel
#RPSBLASTDB update script
#
