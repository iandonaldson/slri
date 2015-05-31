/* $Id: gen_cxx.cpp,v 1.2 2004/02/05 19:59:31 hlieu Exp $ */ 
/***************************************************************************
  Copyright (C) 2001 Mount Sinai Hospital (MSH)
    
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public License
  as published by the Free Software Foundation; either version 2.1 of
  the License, or any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not, write to the
         Free Software Foundation, Inc.,
         59 Temple Place, Suite 330, Boston, MA
         02111-1307  USA
  or visit http://www.gnu.org/licenses/lgpl.txt

  SPECIAL EXCEPTIONS

  As a special exception, Mount Sinai Hospital gives permission to
  link this program with the following non-GPL programs or libraries,
  and distribute the resulting executable, without including the source
  code for these in the source distribution:

  a) CodeBase 6.5 or greater from Sequiter Software Inc.

  b) The C or C++ interface to Oracle 8.x or greater, from
     Oracle Corporation or IBM DB2 UDB.



  REVISION/CONTRIBUTION NOTES:

  September 1, 2002  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:

  AUTHORS: Hao Lieu (hlieu@blueprint.org)

  PROGRAM: comgen_cxx
                updates genff.
                This application should be run through one of the scripts:
                   1) ncbi.bacteria.pl
                   2) 

                Use ncbi.bacteria.pl if you are doing an update to genff.
                Use 2) if you intend to rebuild genff from scratch.

                These two scripts will compute the correct input paramters to 
                comgen_cxx. To see the usage output, run this application as:
                    ./comgen_cxx -h


***********************************************************************
*/
#include <corelib/ncbistd.hpp>
#include <corelib/ncbistre.hpp>
#include <corelib/ncbiapp.hpp>
#include <corelib/ncbiargs.hpp>
#include <corelib/ncbiobj.hpp>
#include <serial/serial.hpp>
#include <serial/objistr.hpp>
#include <serial/objistrasn.hpp>
#include <serial/objostr.hpp>
#include <serial/objcopy.hpp>
#include <serial/typeref.hpp>
#include <objects/seqset/Seq_entry.hpp>
#include <objects/seqset/Bioseq_set.hpp>
#include <objects/seq/Bioseq.hpp>
#include <objects/seq/Seq_descr.hpp>
#include <objects/seq/Seqdesc.hpp>
#include <objects/seq/Seqdesc_.hpp>
#include <objects/seqfeat/BioSource.hpp>
#include <objects/seqfeat/SubSource.hpp>
#include <objects/seqfeat/Org_ref.hpp>
#include <objects/general/Dbtag.hpp>
#include <objects/general/Object_id.hpp>

#include <list>

USING_NCBI_SCOPE;
using namespace objects;

class Test : public CNcbiApplication
{
public:
   virtual void Init ( void );
   virtual int  Run ( void );
   void AddRecord ( string organism, string dir, string accession, int kloodge, CNcbiOfstream &fout );

   /* types */
   typedef CRef< CSeqdesc > TRefSeqdesc;
   typedef list< TRefSeqdesc > Tdata;
   typedef CBioSource TBSource;
   typedef CRef< CDbtag > TRefDbTag;
   typedef vector< TRefDbTag > TDb;
   typedef CRef< CSubSource > TRefSubSource;
   typedef list< TRefSubSource > TSource; 
   typedef CRef< CSeq_entry > TRefSeqEntry;
   typedef list< TRefSeqEntry > TSeqEntrySet;

private:

   /* gets the taxon id from COrg_ref */
   int GetTaxId( COrg_ref &org );
   int GetChromosomeType( CBioSource &bio, string &name, string file );
};



/*****
 * the EGenome types (and subtypes) as defined in CBioSource_Base.hpp
 * (and CSubSource_Base.hpp).
 * Some of the types intentional left as blanks because they are not used.
 * You may fill them in as the need arises.
 *****/
const string type[] = {
                     "", " chromosome", " chloroplast", " chromoplast",
                     " kinetoplast", " mitochondrion", " plastid", " macronuclear",
                     " extrachromosomal", " plasmid", " transposon", " insertion sequence",
                     "", " proviral", " virion", " nucleomorph", "",
                     "", " proplastid", " endogenous virus"
                     }; 

const string subtype[] = {
                         "", " chromosome", " map", " clone", 
                         " haplotype", " genotype", "",
                         "", "", "", "", "", "", "", "", "", "",
                         "", " plasmid", "", " segment", " endogenous virus",
                         " transgenic", "", "", ""
                         };
/*****
 * The integer representation of the chromosome type. So from above, see that
 * element type[1] is "chromosome", integer equivalent of this is iType[1] = 1.
 *****/
#define PLASMID        8
#define CHROM          1
#define EXTRACHROM     16
#define MITO           2
const int iType[]    = {0, CHROM, 0, 0, 0, MITO, 0, 0, EXTRACHROM, 
                        PLASMID, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

const int iSubtype[] = {0, CHROM, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
                        0, 0, 0, PLASMID, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


/*****
 * COMFF gets generated by ncbi.bacteria.pl.
 * COMFF holds a listing of new complete genomes and their
 *    .asn files to be parsed by this application.
 * COMFF also gets deleted by ncbi.bacteria.pl after
 *    this parser is complete.
 *
 *     COMFF can be set by specifying as an optional input parameter
 *     ./comgen_cxx -kloodge INT -infile COMFF
 *
 *     but will default to this if not entered.
 *****/
#define COMFF "../genomes/comff"



/******
 * GENFF the file to update.
 *     /path/to/genff
 *
 *     Can be set by specifying as an optional input parameter
 *     ./comgen_cxx -kloodge INT -infile GENFF -outfile
 ******/
#define GENFF "../genomes/genff"



/******
 * GetTaxid:
 *       Retrieves the taxid from an COrg_ref structure.
 *       Returns -1 as taxid if not found. Will log error if returns -1/0.
 ******/
int Test::GetTaxId ( COrg_ref &org ) 
{
   int id = 0;
   if ( org.IsSetDb() ) 
   {
      vector < TRefDbTag > &db_v = org.SetDb();
      vector< TRefDbTag >::iterator it;
      for( it = db_v.begin(); it != db_v.end(); it++ )
      {
         /* my copy of the c++ toolkit predates the web documentation */
#if 0
         /* so this wasn't defined yet */
         if( (*it)->IsSetDb() && CDbtagType_taxon == (*it)->GetType() )
#else
         /* so use this, not as elegant */
         if( (*it)->IsSetDb() && "taxon" == (*it)->GetDb() )
#endif
            break;
      }
      if ( it == db_v.end() )  { cerr << "no taxid." << endl; return id; } 

      /* comfirm that we are working with the tax id */
      if ( (*it)->IsSetTag() )
      {
         if ( CObject_id_Base::e_Id == (*it)->GetTag().Which() )
            id = (*it)->GetTag().GetId();
      }
      for ( it = db_v.begin(); it != db_v.end(); it++ )
         (*it)->Reset();
   }

   if ( id <= 0 )
      cerr << "Invalid taxid: " << id << endl;

   return id;
}


/******
 * MakeFullName:
 *       Constructs the full name of the organism based on the
 *       asn 'bio'. Additions the the organims name are based
 *       on the type of chromosome in question (see type[]) above
 *       for specifics.
 *       Will also add additional information
 *       such as plasmid/chromsome name. 
 *       Currently, only considers chromosome/plasmid types, 
 *       but easily altered for other chromosome types. 
 *
 *       eg. SomeBacteria Name chromsome I
 *           SomeBacteria Name plasmid pTEIR
 *           SomeOtherBacteria Name
 ******/
int Test::GetChromosomeType( CBioSource& bio, string &molecule, string file )
{
   int chromType = 1; 

   /* check if subtype is set */
   if ( ! bio.IsSetGenome() && ! bio.IsSetSubtype() ) 
   {
      cerr << "No source or subsource " << molecule << " " + file << flush;
      cerr << ". Update " << chromType << " chromosome type by hand." << endl;
      return chromType; 
   }
   else if ( bio.IsSetGenome() ){ 

       molecule += type[ bio.GetGenome() ];
       chromType = iType [ bio.GetGenome() ];
   } 

   /* a subtype is specified, usually contains chrom # or plasmid name */
   if ( bio.IsSetSubtype() ) { 
   
      list< TRefSubSource > &sub = bio.SetSubtype();
      list< TRefSubSource >::iterator it;
      for ( it = sub.begin(); it != sub.end(); it++ )
         if ( (*it)->IsSetName() && (*it)->IsSetSubtype() )
             break;

      // bio will already specify the chromosome type
      if( ! bio.IsSetGenome() ) molecule += subtype [ (*it)->SetSubtype() ];

      // add chromosome # or plasmid name
      if ( CSubSource_Base::eSubtype_other != (*it)->SetSubtype() ) molecule += " " + (*it)->SetName();
      if (! bio.IsSetGenome() ) chromType = iSubtype [ (*it)->SetSubtype() ]; 

      for ( it = sub.begin(); it != sub.end(); it++)
         (*it)->Reset();

   }
    
   return chromType;
}




/*****
 * AddRecord: Adds a new record to 'genff' 
 *     'organism'   is the bacteria name as it appears on ncbi's ftp site
 *     'file'       is the asn file eg NC_03421.asn
 *     'kloodge'    is the kloodge of this organism
 *     'genff'      is the file to write new record to
 *****/
void Test::AddRecord( string organism, string dir, string file, int kloodge, CNcbiOfstream &out )
{
  /* read in the file */
  auto_ptr< CObjectIStream > asn_in( CObjectIStream::Open( dir + "/" + file + ".src", eSerial_AsnText ) );
  CSeq_descr descr;
  *asn_in >> descr; 

  /* we now have a descr object */ 
  list< TRefSeqdesc > &des = descr.Set();
  list< TRefSeqdesc >::iterator it;
  for ( it = des.begin(); it != des.end(); it++ )
     if ( CSeqdesc_Base::e_Source == (*it)->Which() )
        break;
 
  TBSource &bio = (*it)->SetSource();

  /* get taxid */
  COrg_ref &org = bio.SetOrg();
  int taxid = this->GetTaxId( org );

  /*get chromosome type */
  int itype = this->GetChromosomeType ( bio, organism, file );

  /* extract accession */
  string accession = NStr::Replace ( file, ".asn", "");

  string record;
  record += NStr::IntToString(taxid) + "\t";
  record += NStr::IntToString(kloodge) + "\t";
  record += NStr::IntToString ( itype ) + "\t";
  record += accession + "\t" + organism;

  out << record << endl;
} 


/********
 * Carries out the main functionality.
 ********/
int Test::Run( void )
{
#define SIZE1 100 
#define SIZE2 25
   char bact[SIZE1], asn[SIZE2], c;

   CArgs args = GetArgs();
   int max_klge = args["kloodge"].AsInteger();

   string dir = args["dir"].AsString();

   /* open up file */
   CNcbiIfstream fin  ( args["infile"].AsString().c_str() );
   CNcbiOfstream fout ( args["outfile"].AsString().c_str(), ios::app ); 

   while ( fin.getline( bact, SIZE1 - 1 ) )
   {
      if ( '\t' == (c = fin.peek()) )
         while ( fin.getline( asn, SIZE2 - 1 ) )
         {
            string file = NStr::TruncateSpaces( string ( asn ));
            if ( 0 == NStr::CompareCase (bact, "H sapiens")    || 
                 0 == NStr::CompareCase (bact, "M musculus")   || 
                 0 == NStr::CompareCase (bact, "R norvegicus")
               ) 
            {
               string organism;
               if ( 0 == NStr::CompareCase ( bact, "H sapiens" ) )
                  organism = NStr::Replace( string(bact), "H", "Homo", organism);
               else if ( 0 == NStr::CompareCase( bact, "M musculus" ) )
                  organism = NStr::Replace( string(bact), "M", "Mus", organism);
               else if ( 0 == NStr::CompareCase( bact, "R norvegicus" ) )
                  organism = NStr::Replace( string(bact), "R", "Rattus", organism);

               /* humans, mouse and rat released as bioseq set */
               // this->AddRecord( organism, NStr::TruncateSpaces( string( asn ) ), ++max_klge, fout );
               this->AddRecord( organism, dir, file, ++max_klge, fout );
            } else {
               /* all other organisms released as seqentry */
               // string file = dir + "/" + NStr::TruncateSpaces( string ( asn ));
               this->AddRecord( string(bact), dir, file, ++max_klge, fout ); 
            }
            if( '\t' != (c = fin.peek()) ) break;
         }
   }
   fin.close();
   fout.close();
   return 0;
}


/******
 * Specifies the input parameters.
 *    To see what the input parameters are, run the
 *    application with '-h'
 ******/
void Test::Init ( void )
{

   auto_ptr< CArgDescriptions > arg_desc ( new CArgDescriptions );
   arg_desc->SetUsageContext( GetArguments().GetProgramBasename(),
                             "updates genff with the newest complete genome" ); 
   arg_desc->AddKey
             ( "kloodge", "MandatoryValue", 
               "mandatory: current maximum kloodge in genff.\n\tThe next record will increment on this.",
               CArgDescriptions::eInteger );

   arg_desc->AddDefaultKey
             ( "infile", "DefaultKey",
               "optional: the location and name of comff",
               CArgDescriptions::eString, 
               COMFF );

   arg_desc->AddDefaultKey
             ( "outfile", "DefaultKey",
               "optional: the location and name of genff",
               CArgDescriptions::eString, 
               GENFF );

   arg_desc->AddKey
             ( "dir", "MandatoryValue",
               "mandatory: directory where asn files are downloaded.",
               CArgDescriptions::eString );

   SetupArgDescriptions( arg_desc.release() );   
}


int main(int argc, const char* argv[]){

  Test theTestApp;
  return theTestApp.AppMain(argc, argv, 0, eDS_Default, 0, "comgen_cxx");
}




/* 
 * $Log: gen_cxx.cpp,v $
 * Revision 1.2  2004/02/05 19:59:31  hlieu
 * added checks for robustness
 *
 * Revision 1.1  2004/02/03 22:55:43  hlieu
 * renamed genff_cxx to gen to maintain consistency with chrom;
 * minor fixes in parser
 *
 * Revision 1.1  2004/01/30 22:10:19  hlieu
 * renaming comgen_cxx.cpp to genff_cxx.cpp to properly reflect that
 * it builds genff, and not confuse it with comgen.c
 *
 * Revision 1.4  2004/01/30 18:28:49  hlieu
 * added many features
 *
 * Revision 1.3  2004/01/29 19:12:16  hlieu
 * completed genff rebuilder, added proper chromosome type finding,
 * and subtype matching, renamed MakeFullName to GetChromosomeType,
 * Added support for seqentry and bioseq set and more
 *
 * Revision 1.2  2004/01/26 19:54:19  hlieu
 * added write to genff, added command line parameters for more
 * configurability, moved GetBioSource into main, added diagnostic outputs
 *
 * Revision 1.1  2004/01/23 23:34:47  hlieu
 * new comgen auto updater
 *
 *
 */
