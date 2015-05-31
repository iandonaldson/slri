/* $Id: pdbrep.c,v 1.3 2004/12/14 16:49:25 zwang Exp $
*******************************************************************************$Copyright
(C) 2000 Mount Sinai Hospital (MSH)

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the
         Free Software Foundation, Inc.,
         59 Temple Place, Suite 330, Boston, MA
         02111-1307  USA
  or visit http://www.gnu.org/copyleft/gpl.html

  SPECIAL EXCEPTIONS

  As a special exception, Mount Sinai Hospital gives permission to
  link this program with the following non-GPL programs or libraries,
  and distribute the resulting executable, without including the source
  code for these in the source distribution:

  a) CodeBase 6.5 or greater from Sequiter Software Inc.

  b) The C or C++ interface to Oracle 8.x or greater, from
     Oracle Corporation or IBM DB2 UDB.


  PROGRAM:


  AUTHORS:
           Katerina Michalickova and Christopher W.V. Hogue
hogue@mshri.on.ca


  REVISION/CONTRIBUTION NOTES:

  June 1, 2000  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:


  DESCRIPTION
   December 1999
   pdbrep.c
   labels representatives of non-redundant BLAST sets in DOMDB
   needs nrpdb table from ncbi ftp site ftp://ncbi.nlm.nih.gov/mmdb/nrtable

*****************************************************************************************/

#include <intrez_db.h>
#include <intrez_misc.h>
#include <intrez.h>

#define NUMARGS 2
Args myargs[NUMARGS] = {
        {"Input nrpdb table",NULL,NULL,NULL,FALSE,'i',ARG_FILE_IN,0.0,0,NULL},
        {"Is this an update","F",NULL,NULL,TRUE,'u',ARG_BOOLEAN,0.0,0,NULL}
  };


#define LOGING "pdbreplog"

#define COLUMPOS1 9       /* mmdbid position */
#define COLUMPOS2 5       /* chain name position */
#define POS1 26           /* representative position E value 10e-7*/
#define POS2 39           /* representative position E value 10e-40*/
#define POS3 52           /* representative position E value 10e-80*/
#define POS4 65           /* representative position non-identical sequences*/

/* looks for domain ids which have the mmdbid and chain from a representative table,
   assigns the representative in DOMDB to all mmdb ids collected */
Boolean AssignDOMDB(Int4 mmdbid, CharPtr pchain, Int2 order)
{

   Int4 res = 0;
   StDomdbPtr pdd = NULL, pddhead = NULL, pddtemp = NULL;
   ValNodePtr pvndomid = NULL, pvndomidhead = NULL;


   pdd = StDomdbNew();
   /* SK: NULL ptr check */
   if ( pdd == NULL )
   {
      ErrPostEx(SEV_ERROR,0,0, "AssignDOMDB: pdd is NULL.");
      return FALSE;
   }
   pdd->mmdbid = mmdbid;
   res = SearchDOMDB(&pdd);
   if(res == 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "AssignDOMDB: SearchDOMDB failed.");
      return FALSE;
    }
   if (res == -1)
    {
      ErrPostEx(SEV_ERROR,0,0, "AssignDOMDB: Mmdb ID %ld not found in DOMDB database.", (long) mmdbid);
      return TRUE;
    }
   if (res == 1)
    {
      while(pdd != NULL)
       {
         if((pchain[0] == ' ') || (strstr(pdd->chain, pchain) != NULL))   /* if the chain matches store domid */
            pvndomid = Slri_ValNodeAddInt(&pvndomid, 0, pdd->domid);
         pdd = pdd->next;
       }
      if(pvndomid == NULL)
          ErrPostEx(SEV_ERROR,0,0, "AssignDOMDB: No hits for mmdbid %ld and chain %s.\n", (long) mmdbid, IsNullStr(pchain));
      pvndomidhead = pvndomid;
      while(pvndomid != NULL)
       {
          pddtemp = StDomdbNew();
	  /* SK: NULL ptr check */
	  if ( pddtemp == NULL )
	  {
	     ErrPostEx(SEV_ERROR,0,0, "AssignDOMDB: pddtemp is NULL.");
	     return FALSE;
          }
	  
          pddtemp->domid = pvndomid->data.intvalue;
          pddtemp->rep = order;
          if(!EditRecordDOMDBByDOMID(pddtemp, TRUE))
           {
             ErrPostEx(SEV_ERROR,0,0, "AssignDOMDB: EditRecordDOMDBByDOMID failed on DOMID %ld.", (long) pdd->domid);
             return FALSE;
           }
          pddtemp = StDomdbFree(pddtemp);
          pvndomid = pvndomid->next;
       }

    }

   StDomdbFree(pddhead);
   ValNodeFree(pvndomidhead);
   return TRUE;
}




Int2 Nlm_Main(void)
{
  
   FILE *pFile;
   Char pcBuf[500];
   CharPtr pcTest = NULL;
   Char test[4];
   CharPtr pchain = NULL;
   CharPtr prep = NULL;
   CharPtr pmmdbid = NULL;
   CharPtr pline = NULL;
   CharPtr plinetemp = NULL;
   CharPtr pline3 = NULL;
   Int4 numval = 0;
   Int4 numval2 = 0;
   Int4 border[4];
   Int2 k = 0;
   Int2 flag = 0;
   Int2 h = 1;
   Boolean isUpdate = FALSE;
   Int4 count = 0;

   ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
   ErrSetLogfile(LOGING, ELOG_BANNER);
   ErrSetLogLevel(SEV_INFO);
   ErrSetMessageLevel(SEV_MAX);

   if (! GetArgs("pdbrep",NUMARGS,myargs))
	return 1;
	
   if ((pFile = FileOpen(myargs[0].strvalue, "r")) == NULL)
        return 1;

   
   test[0] = ' ';
   test[1] = '\t';
   test[2] = '\n';
   test[3] = '\0'; 

   border[0] = POS1;
   border[1] = POS2;
   border[2] = POS3;
   border[3] = POS4;

   InitCodeBase();
   if(!OpenDOMDB("DOMDB", FALSE))
    {
      ErrPostEx(SEV_ERROR,0,0, "Cannot open DOMDB database!");
      return 1;
    }

   if (myargs[1].intvalue)
     isUpdate = TRUE;

   if (!StartTransaction()) {
     ErrPostEx(SEV_ERROR, 0, 0, "Cannot start transaction!");
     return 1;
   }

   do {
         pcTest = fgets(pcBuf, (size_t)500,  pFile);
         if (pcTest)
           {
             if (pcBuf[0] != '#')
              {
                 flag = 0;
                 h = 1;
                 pline = pcTest + COLUMPOS1;
                 pmmdbid = strtok(pline, test);
                 if(!StrToInt4 (pmmdbid, &numval))
                  {
                    ErrPostEx(SEV_ERROR,0,0, "StrToInt4 failed.");
                    return 1;
                  }

                 pline3 = pcTest +  COLUMPOS2;
                 pline3[1] = '\0';
                 pchain = pline3;

                 for (k=0; k<=3; k++)
                  {
                     plinetemp = NULL;
                     numval2 = 0;
                     prep = NULL;

                     plinetemp = pcTest + border[k];
                     prep = strtok(plinetemp, test);
                     if(!StrToInt4 (prep, &numval2))
                      {
                        ErrPostEx(SEV_ERROR,0,0, "StrToInt4 failed.");
                        return 1;
                      }

                     if(numval2 == 1)
                      {
                          flag = (Int2) (flag + h);
                      }
                     h = (Int2) (h*2);
                  }
                 if (flag != 0)
                  {
                    if(!AssignDOMDB(numval, pchain, flag))
                       ErrPostEx(SEV_ERROR,0,0, "AssignDOMDB failed.");
                  }
                  if ((Byte)flag & REP_MIN_SEVEN)
                    printf("%ld\t%s\n", (long) numval, IsNullStr(pchain));
              }
  	   }

           if (!isUpdate && count >= 500) {
                  count = 0;
                  if (!CommitTransaction()) {
                     ErrPostEx(SEV_ERROR, 0, 0, "Cannot commit transaction!");
                     if (!RollBackTransaction()) {
                        ErrPostEx(SEV_ERROR,0,0, "Cannot roll back!");
                        CloseCodeBase();
                        return 1;
                     }
                     CloseCodeBase();
                     return 1;
                 }
           }


     } while (pcTest);  

     if (!CommitTransaction()) {
        ErrPostEx(SEV_ERROR, 0, 0, "Cannot commit transaction!");
        if (!RollBackTransaction()) {
           ErrPostEx(SEV_ERROR,0,0, "Cannot roll back!");
           CloseCodeBase();
           return 1;
        }
        CloseCodeBase();
        return 1;
    }

   CloseDOMDB();
   CloseCodeBase(); 

   FileClose(pFile);
   ErrPostEx(SEV_INFO,0,0, "Done!");
   return 0;
}

/*
 $Log: pdbrep.c,v $
 Revision 1.3  2004/12/14 16:49:25  zwang
 Added ability of building a transactional database.
 Added input parameter to specify if it's an update.
 See bug 3441.

 Revision 1.2  2004/01/06 22:32:11  skuznets
 Code audit has been done sucessfully.
 Possible NULL pointer issues has been eliminated.

 Revision 1.1.1.3  2002/03/25 09:27:20  root
 Automated import from Sourceforge.net

 Revision 1.3  2002/03/25 07:18:53  micheld
 minor change

 Revision 1.2  2002/02/27 16:05:07  kaca
 initial import of nr set from mmdb

 Revision 1.1.1.1  2001/05/25 21:36:19  kaca
 initial import

 Revision 1.7  2001/03/26 19:47:41  kaca
 changes in include files

 Revision 1.6  2001/02/23 20:40:06  kaca
 license change

 Revision 1.5  2001/02/19 20:10:01  kaca
 abstracted database calls

 Revision 1.4  2000/11/06 22:19:35  kaca
 added functionality around different E value BLAST sets of domains

   Revision 1.3  2000/10/13 21:54:22  kaca
   changed includes

  Revision 1.2  2000/09/20 18:49:58  kaca
  split SeqHound headers into private and public one and changed includes

 Revision 1.1.1.1  2000/09/18 17:41:56  kaca
 Initial import
*/
