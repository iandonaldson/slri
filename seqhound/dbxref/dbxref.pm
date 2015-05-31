package dbxref;

use strict;
use warnings;


my $sthiddbx;
my $sth;
my $sthdel;
my $sthdelParent;
my $sthdelChild;

# Function: getProperty
#           Read a tag from a proterty file.
# Arg1: $_[1] FileName: Property File.
# Arg2: $_[2] Tag:      Tag to read from the property file.
# Return: Value of that tag.
#         0 if tag not found.
sub getProperty
{
    open(FP1, $_[1]) or die "can't open file: $!";

    my $line;
	my $value=();
	my $tag=$_[2];
	my $found=0;
    while($line = <FP1>)
    {
        chomp($line);
		if($line=~/^\s*$tag\s*=\s*(\S+.*)$/)
		{
			if($found==0)
			{
				$value=$1;
				$found=1;
			}
			else{
				$value=$value.";".$1;
			}
		}
	}
	if($found==0)
	{
	    return 0;
	}
	else
	{
		$value=~s/\"//g;
#		print "$tag = $value\n";
		return $value;
	}
}


# Function: prepareID.
#           Prepare the select stament that gets an id from dbxref binding 
#           3 fields.
# Arg1: $_[1]: dbh Connection to the Database.
# Arg2: $_[2]: Database Name.
# Arg3: $_[3]: Table Name.
sub prepareID
{
    $sthiddbx = $_[1]->prepare("SELECT id FROM " . $_[2] . "." . $_[3] . " WHERE source_id = ? and record_id = ? and parent_id = ?;");
}


# Function: selectID.
#           Execute the select stament to get an id from dbxref.
# Arg1: $_[1]: source_id.
# Arg2: $_[2]: record_id.
# Arg3: $_[3]: parent_id.
sub selectID
{
    $sthiddbx->execute($_[1], $_[2], $_[3]);
    return $sthiddbx->fetchrow_array();
}


# Function: prepareInsert.
#           Prepare the Insert stament that populate dbxref binding 
#           7 fields.
# Arg1: $_[1]: dbh Connection to the Database.
# Arg2: $_[2]: Database Name.
# Arg3: $_[3]: Table Name.
sub prepareInsert
{
    #$sth = $_[1]->prepare("INSERT IGNORE INTO " . $_[2] . "." . $_[3] . " VALUES(?, ?, ?, ?, ?, ?, ?);");
   $sth = $_[1]->prepare("INSERT IGNORE INTO " . $_[2] . "." . $_[3] . "(source_id, record_id, parent_id, link, field, cv) VALUES(?, ?, ?, ?, ?, ?);");
}


# Function: executeInsert.
#           Execute the insert stament to populate dbxref.

# Arg2: $_[1]: source_id.
# Arg3: $_[2]: record_id.
# Arg3: $_[3]: parent_id.
# Arg3: $_[4]: link.
# Arg3: $_[5]: field.
# Arg3: $_[6]: cv.
sub executeInsert
{
    $sth->execute($_[1], $_[2], $_[3], $_[4], $_[5], $_[6]);
}


# Function: executeDelete.
#           Delete all parents and its children for a given source_id.
# Arg1: $_[1]: dbh Connection to the Database.
# Arg2: $_[2]: Database Name.
# Arg3: $_[3]: Table Name.
# Arg3: $_[4]: source_id.
sub executeDelete
{
    $sthdelChild  = $_[1]->prepare("DELETE FROM " . $_[2] . "." . $_[3] . " WHERE parent_id = ?;");
    $sthdelParent = $_[1]->prepare("DELETE FROM " . $_[2] . "." . $_[3] . " WHERE id = ?;");

    $sthdel = $_[1]->prepare("SELECT id FROM " . $_[2] . "." . $_[3] . " WHERE source_id=" . $_[4] . " AND parent_id=0;");
    $sthdel->execute();
    while(my @parentid = $sthdel->fetchrow_array())
    {
	$sthdelChild->execute($parentid[0]);
	$sthdelParent->execute($parentid[0]);
    }
}

#delete with control vocabulary info, used for SPTR records
sub executeDeleteCV
{
    $sthdelChild  = $_[1]->prepare("DELETE FROM  $_[2].$_[3] WHERE parent_id = ?;");
    $sthdelParent = $_[1]->prepare("DELETE FROM $_[2].$_[3] WHERE id = ?;");

    $sthdel = $_[1]->prepare("SELECT id FROM $_[2].$_[3] WHERE source_id=$_[4] AND parent_id=0 AND cv=$_[5];");
    $sthdel->execute();
    while(my @parentid = $sthdel->fetchrow_array())
    {
	$sthdelChild->execute($parentid[0]);
	$sthdelParent->execute($parentid[0]);
    }
}


# HASH Table Implementation.

sub new
{
    my $dbrecord = {the_source_id => 0,
		    the_isprimary_db => 0, };
    bless( $dbrecord );
    return $dbrecord;
}

sub source_id
{
    my $self = shift();
    $self->{the_source_id} = shift() if (@_);
    return $self->{the_source_id};
}

sub isprimary_db
{
    my $self = shift();
    $self->{the_isprimary_id} = shift() if (@_);
    return $self->{the_isprimary_db};
}

sub setDBrecord
{
    if ( @_ == 3)
    {
        my $self = shift();
	$self->source_id($_[0]);
	$self->isprimary_db($_[1]);
    }
    else
    {
        print("Method setDBrecord requires 2 (two) arguments.\n");
    }
}

sub print
{
    my $self = shift();
    print($self->source_id);
    print("\t");
    print($self->isprimary_db);
    print("\n");
}



# Function that creats a hash table 
# with an object that represents dbabbrev. and 
# ifprimary flag with 1 or 0 if it's primary db or not.
# $_[1] dbh Connection to the Database.
# $_[2] database name.
sub createDbHashTable
{
    my $sthdb;
    my %hash_db;
    $sthdb = $_[1]->prepare("SELECT source_id, source_db, isprimary_db from " . $_[2] . ".dbxrefsourcedb");
    $sthdb->execute();
    while(my @row = $sthdb->fetchrow_array())
    {
        my $dbrec = new dbxref;

	$dbrec->setDBrecord($row[0], $row[2]);
	$hash_db{$row[1]} = $dbrec;
    }
    return %hash_db;
}


# Function: executeDeleteGOA.
#           Delete all GOA Recors for a given DatabaseID
#           parents and its children for a given source_id.
# Arg1: $_[1]: dbh Connection to the Database.
# Arg2: $_[2]: Database id from goa_xdb..
sub executeDeleteGOA
{
    my $sthSeqDbxref = $_[1]->prepare("SELECT id from goa_seq_dbxref WHERE goa_xdb_id = ". $_[2]);
    $sthSeqDbxref->execute();
    while ( my @rowSeqDbxref_id = $sthSeqDbxref->fetchrow_array)
    {
        my $sthAssociation = $_[1]->prepare("SELECT id from goa_association WHERE goa_seq_dbxref_id=".$rowSeqDbxref_id[0]);
	$sthAssociation->execute();
	while (my @rowAssociation_id = $sthAssociation->fetchrow_array)
	{
	    my $sthReference = $_[1]->prepare("DELETE FROM goa_reference WHERE goa_association_id = ".$rowAssociation_id[0]);
	    $sthReference->execute();
	    my $sthWith = $_[1]->prepare("DELETE FROM goa_with WHERE goa_association_id = ".$rowAssociation_id[0]);
	    $sthWith->execute();
	}
	my $sthAssociation2 = $_[1]->prepare("DELETE FROM goa_association WHERE goa_seq_dbxref_id=".$rowSeqDbxref_id[0]);
	$sthAssociation2->execute();

	my $sthSeqDbxref2 = $_[1]->prepare("DELETE FROM goa_seq_dbxref WHERE id = " . $rowSeqDbxref_id[0]);
	$sthSeqDbxref2->execute();
    }
}


#close statement handler
sub sth_finish
{
	$sth->finish;
	$sthiddbx->finish;
	$sthdel->finish;
	$sthdelParent->finish;
	$sthdelChild->finish;
}

sub finishSelectID
{
    $sthiddbx->finish;
}

sub sthiddbx_finish
{
    $sthiddbx->finish;
}

sub finishInsert
{
    $sth->finish;
}

sub insert_finish
{
    $sth->finish;
}


sub displayMessage
{
      print("Usage : perl " . $_[1] . " -c -o -n -f\n");
      print("        or\n");
      print("        perl " . $_[1] . " -c\n");
      print("\tWhere:\n");
      print("\t\t-c Cluster: T = True | F = False\n");
      print("\t\t-o Offset: number of record to skip\n");
      print("\t\t-n Number: of records to process\n");
      print("\t\t-f Cluster node that will became a Synchronization file\n\n");
      print("\tIf -c = F arguments -o -n -f are optional.\n");
      die("4 input arguments are needed: Cluster Offset Quantity SyncFileName\n");
}

return 1;
