/* $Id: intrez_misc.c,v 1.7 2005/03/14 16:01:14 eburgess Exp $
*******************************************************************************************
  Copyright (C) 2000 Mount Sinai Hospital (MSH)

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
           Katerina Michalickova and Christopher W.V. Hogue hogue@mshri.on.ca


  REVISION/CONTRIBUTION NOTES:

  June 1, 2000  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:


  DESCRIPTION
  November 1998
  SeqHound system API

    July 1999
    added a new field to the ACCDB database: version of accession to reflect the
    NCBI versoning system
    September 1999
    added the REDUND database and took redundant info out of TAXGIFIN and TAXGI
    TAXGIFIN taken out of the system
    October 1999
    title in ACCDB now stored in Memo field
    June 2000
    removed all remote functions, they live in ambiseqhoundapi.c only the lighweight
    (no local obtion) version is maintained

******************************************************************************************/
#include <intrez_db.h>
#include <intrez_misc.h>


/**************************************************************************************
*
*
*       Miscelaneous
*
*
***************************************************************************************/
void LIBCALL PrintValNodeOfInt(ValNodePtr pvn)
{
   Int4 count = 0;

   if (pvn == NULL)
     printf("NULL\n");

   while(pvn != NULL)
    {
       /*printf("%ld\n", (long) pvn ->data.intvalue);*/
       count++;
       pvn = pvn->next;
    }
   printf("count:%ld\n", (long) count);
   return;
}


void LIBCALL PrintValNodeOfStr(ValNodePtr pvn)
{
   if (pvn == NULL)
     printf("NULL\n");

   while(pvn != NULL)
    {
       printf("%s\n", (char*) IsNullStr(pvn->data.ptrvalue));
       pvn = pvn->next;
    }
   return;
}

Boolean LIBCALL StrToInt4 (CharPtr str, Int4Ptr longval)
{
  Nlm_Char     ch;
  Nlm_Int2     i;
  Nlm_Int2     len;
  Nlm_Char     local [64];
  Nlm_Boolean  nodigits;
  Nlm_Boolean  rsult;
  long int     val;

  rsult = FALSE;
  if (longval != NULL) {
    *longval = (Nlm_Int4) 0;
  }
  len = (Nlm_Int2) Nlm_StringLen (str);
  if (len != 0) {
    rsult = TRUE;
    nodigits = TRUE;
    for (i = 0; i < len; i++) {
      ch = str [i];
      if (ch == ' ' || ch == '+' || ch == '-') {
      } else if (ch < '0' || ch > '9') {
        rsult = FALSE;
      } else {
        nodigits = FALSE;
      }
    }
    if (nodigits) {
      rsult = FALSE;
    }
    if (rsult && longval != NULL) {
      Nlm_StringNCpy (local, str, sizeof (local));
      if (sscanf (local, "%ld", &val) == 1) {
        *longval = val;
      }
    }
  }
  return rsult;
}


/*returns 1 if string is equivalent to TRUE
  returns 0 if string is equivalent to FALSE
  returns -1 if string is equivalent to neither*/
Int2 LIBCALL Misc_IswwStringTrueOrFalse(CharPtr string)
{
	Int2 i=0;
	CharPtr yes[]= {"y", "yes", "t", "true", "affirmative", NULL};
	CharPtr no[] = {"n", "no", "f", "false", "negative", NULL};

	if(string==NULL)
		return(-1);

	/*check this against the array*/
	while(yes[i] != NULL) {
		if(StringICmp(yes[i], string) == 0)
			return 1;
		i++;
	}
	i=0;
	while(no[i] != NULL) {
		if(StringICmp(no[i], string) == 0)
			return 0;
		i++;
	}
	/*nothing found from yes,no lists*/
	return (-1);
}


CharPtr  LIBCALL ValNodeToCharPtr(ValNodePtr pvn)
{

    Int4 count = 0, count_letter = 0;
    Char numb[50];
    CharPtr plist = NULL;
    ValNodePtr pvnhead = NULL, pvn2 = NULL;

    if (pvn == NULL)
       return NULL;

    pvnhead = pvn;
    /* count up how much space will have to allocate for comma-delimited list */
    while(pvn != NULL)
     {
       count++;
       sprintf(numb,"%ld", (long) pvn->data.intvalue);
       count_letter = count_letter + strlen(numb);
       pvn = pvn->next;
     }

    /* allocate memory for the comma-delimited list */
    plist = MemNew((size_t) ((count + count_letter + 1) * sizeof(Char)));
    if (plist == NULL)
     {
       ErrPostEx(SEV_ERROR,0,0, "Failed to allocated memory.");
       return NULL;
     }
    /* fill in the list */
    pvn2 = pvnhead;
    while(pvn2 != NULL)
     {
       sprintf(numb,"%ld", (long) pvn2->data.intvalue);
       strcat(plist, numb);
       pvn2 = pvn2->next;
       if (pvn2 != NULL)
         strcat(plist, ",");
     }

    return plist;
}




Boolean LIBCALL IsDivisionEST(SeqDescrPtr pdes)
{

   SeqDescrPtr pdesThis = NULL;
   OrgRefPtr por = NULL;
   BioSourcePtr pbio = NULL;
   OrgNamePtr pon = NULL;
   CharPtr division = NULL;


   if (pdes == NULL)
    {
      /* ErrPostEx(SEV_ERROR,0,0, "IsDivisionEST: Descr pointer is NULL."); */
      return FALSE;
    }

   pdesThis = pdes;
   while (pdesThis != NULL)
    {
      if (pdesThis->choice == 23)
       {
         pbio = pdesThis->data.ptrvalue;
         if (pbio == NULL)
          {
            /* ErrPostEx(SEV_ERROR,0,0, "IsDivisionEST: Null pointer to biosource (NULL)."); */
            return FALSE;
          }
       }
      pdesThis = pdesThis->next;
    }

   if (pbio == NULL)
    {
      /* ErrPostEx(SEV_ERROR,0,0, "IsDivisionEST: No source ValNode (NULL)."); */
      return FALSE;
    }

   por = pbio->org;
   if (por == NULL)
    {
      /* ErrPostEx(SEV_ERROR,0,0, "IsDivisionEST: OrgRef pointer is NULL."); */
      return FALSE;
    }

   pon = por->orgname;
   if (pon == NULL)
    {
      /* ErrPostEx(SEV_ERROR,0,0, "IsDivisionEST: OrgName pointer is NULL."); */
      return FALSE;
    }

   division = pon->div;
   if (division == NULL)
    {
      /* ErrPostEx(SEV_ERROR,0,0, "IsDivisionEST: Div pointer is NULL."); */
      return FALSE;
    }

   /* ErrPostEx(SEV_ERROR,0,0, "IsDivisionEST: Division %s.", IsNullStr(division));  */
   if (strstr(division, "EST") != NULL)
       return TRUE;

   return FALSE;
}


CharPtr LIBCALL CleanCommas(CharPtr string)
{
   int i;

   if (string == NULL)
     return NULL;

   for (i = 0; i <= (int) strlen(string); i++)
    {
      if (string[i] == ',')
          string[i] = ';';

    }
   return string;
}


CharPtr LIBCALL TrimSpace(CharPtr string)
{
   /* int i; */
   CharPtr ptr;
   int spaceCounter = 0;


/* SK: Changing the algorythm 
 * if (string == NULL)
 *   return NULL;
 *
 *  for (i = 0; i <= (int) strlen(string)-1; i++)
 *  {
 *     if ((string[i] == ' ') && (string[i+1] == ' '))
 *     {
 *        string[i] = '\0';
 *        return string;
 *     }
 *  }
 *  return string;
**/
   if (string == NULL || string[0] == '\0')
     return string;

    ptr = string;
    while ( *ptr != '\0' )
        if ( *ptr++ <= ' ' )
            spaceCounter++;

    if ( (ptr - string) <= spaceCounter )
    {
        *string = '\0';
        return string;
    }

   while( ptr > string )
   {
      if ( *ptr != ' ' && *ptr != '\0' )
      {
         *(ptr+1) = '\0';
         break;
      }
      ptr--;
   }

   return string;
}


CharPtr LIBCALL TrimSpaceButOne(CharPtr string)
{
   int i;

   if (string == NULL)
     return NULL;

   for (i = 0; i <= (int) strlen(string)-1; i++)
    {
      if ((string[i] == ' ') && (string[i+1] == ' '))
       {
          string[i+1] = '\0';
          return string;
       }
    }
   return string;
}


/* Adds a space at the end of a string, ALLOCATES DYNAMIC MEMORY FOR THE RETURN STRING */
CharPtr LIBCALL PadString(CharPtr string)
{
   CharPtr newstr = NULL;

   newstr = MemNew(strlen(string) + 2);
   if (newstr == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "PadString: Failed to allocate memory.");
      return NULL;
    }

   StringCpy(newstr, string);
   newstr[strlen(string)] = ' ';
   newstr[strlen(string) + 1] = '\0';
   return newstr;
}


Int4 LIBCALL GetGI(BioseqPtr pbs)
{
   SeqIdPtr psi = NULL;
   SeqIdPtr psiThis = NULL;
   Int4 gi = 0;

   if (pbs == NULL)
     {
        ErrPostEx(SEV_ERROR,0,0, "GetGI: Bioseq pointer is NULL.");
        return 0;
     }
   psi = pbs->id;
   if (psi == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetGI: Seq-id pointer is NULL.");
      return 0;
    }
   psiThis = psi;
   while (psiThis != NULL)
     {
       if (psiThis->choice == SEQID_GI)  /* is it GI integer? */
        {
           gi = psiThis->data.intvalue;
           if (gi == 0)
            {
              ErrPostEx(SEV_ERROR,0,0, "GetGI: Invalid GI.");
              return 0;
            }
         }
       psiThis = psiThis->next;
      }
   return gi;
}

/* returns an accession from Bioseq; allocates memory! */
CharPtr LIBCALL Misc_GetAccession(BioseqPtr pbs)
{
     SeqIdPtr psi = NULL;
     SeqIdPtr psiThis = NULL;
     TextSeqIdPtr ptsi = NULL;
     CharPtr pacc = NULL;

    /* SK: NULL ptr check */
    if (pbs == NULL)
    {
        ErrPostEx(SEV_ERROR,0,0, "Misc_GetAccession: NULL pbs.");
        return NULL;
    }

      psi = pbs->id;
      if (psi == NULL)
       {
         ErrPostEx(SEV_ERROR,0,0, "Misc_GetAccession: NULL Seq-id pointer.");
         return NULL;
       }
      psiThis = psi;
      while (psiThis != NULL)
       {
         if ((psiThis->choice == SEQID_GENBANK) ||
	 (psiThis->choice == SEQID_EMBL) ||
	 (psiThis->choice == SEQID_PIR) ||
	 (psiThis->choice == SEQID_SWISSPROT) ||
	 (psiThis->choice == SEQID_OTHER) ||
	 (psiThis->choice == SEQID_DDBJ) ||
	 (psiThis->choice == SEQID_PRF))
          {
            ptsi = psiThis->data.ptrvalue;
            if (ptsi == NULL)
            {
               ErrPostEx(SEV_ERROR,0,0, "Misc_GetAccession: NULL TextSeq-id pointer.");
               return FALSE;
            }
            if (ptsi->accession != NULL)
            {
              pacc = StrSave(ptsi->accession);
              return pacc;
            }
            else
            {
              ErrPostEx(SEV_ERROR,0,0, "Misc_GetAccession: NULL pointer to accession.");
              return NULL;
            }
          }
         psiThis = psiThis->next;
       }
       return NULL;
}

/* returns accession version from Bioseq */
Int2 LIBCALL Misc_GetVersion(BioseqPtr pbs)
{

      SeqIdPtr psi = NULL;
      SeqIdPtr psiThis = NULL;
      TextSeqIdPtr ptsi = NULL;

      /* SK: NULL ptr check */
      if (pbs == NULL)
      {
         ErrPostEx(SEV_ERROR,0,0, "Misc_GetVersion: NULL pbs.");
         return INT2_MIN;
      }

      psi = pbs->id;
      if (psi == NULL)
      {
         ErrPostEx(SEV_ERROR,0,0, "Misc_GetVersion: NULL Seq-id pointer.");
         return INT2_MIN;
      }
      psiThis = psi;
      while (psiThis != NULL)
       {
         if ((psiThis->choice == SEQID_GENBANK) ||
	 (psiThis->choice == SEQID_EMBL) ||
	 (psiThis->choice == SEQID_PIR) ||
	 (psiThis->choice == SEQID_SWISSPROT) ||
	 (psiThis->choice == SEQID_OTHER) ||
	 (psiThis->choice == SEQID_DDBJ) ||
	 (psiThis->choice == SEQID_PRF))
          {
            ptsi = psiThis->data.ptrvalue;
            if (ptsi == NULL)
            {
               ErrPostEx(SEV_ERROR,0,0, "Misc_GetVersion: NULL TextSeq-id pointer.");
               return FALSE;
            }
            if (ptsi->version != INT2_MIN)
               return ptsi->version;
            else
            {
              ErrPostEx(SEV_ERROR,0,0, "Misc_GetVersion: No version.");
              return INT2_MIN; 
            }
          }
         psiThis = psiThis->next;
       }
       return INT2_MIN;
}

/* tests if a number exists in a ValNode of integers */
Boolean LIBCALL ValNodeIsIn(Int4 num, ValNodePtr pvnval)
{
   if (num == 0)
      return FALSE;

   while (pvnval != NULL)
     {
       if (num == pvnval->data.intvalue)
         return TRUE;
       pvnval = pvnval->next;
     }
   return FALSE;
}



/* makes ValNode of integers unique */
ValNodePtr LIBCALL ValNodeUnique(ValNodePtr pvn)
{
   ValNodePtr pvnfin = NULL;

   if (pvn == NULL)
      return NULL;

   while(pvn != NULL)
    {
      if(!ValNodeIsIn(pvn->data.intvalue, pvnfin))
           pvnfin = Slri_ValNodeAddInt(&pvnfin, 0, pvn->data.intvalue);
      pvn = pvn->next;
    }
   return pvnfin;
}



/*
$Log: intrez_misc.c,v $
Revision 1.7  2005/03/14 16:01:14  eburgess
Use a define SEQID_GI in GetGi().

Revision 1.6  2004/10/19 14:41:54  eburgess
Moved functions to parse gis from bioseqs to their own file: parseasn.c.
This eliminates the dependency of seqrem on intrez_odbc.c.

Revision 1.5  2004/10/18 20:54:12  hfeldman
Changed include order for Windows

Revision 1.4  2004/10/15 20:00:45  eburgess
Moved a number of functions from mother.c here so that they can be accessed
by updkldge.c. Bug 2952 complete genomes.

Revision 1.3  2004/05/31 20:36:17  hlieu
minor compiler warning fixes for bug 2185

Revision 1.2  2004/01/06 22:32:16  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.7  2002/08/21 08:29:34  root
Automated import from Sourceforge.net

Revision 1.7  2002/08/20 21:42:58  micheld
Compiler warning/error fix - changed return and added LIBCALL to declared function

Revision 1.6  2002/08/08 22:15:43  kaca
added Misc_GetVersion

Revision 1.5  2002/07/08 16:58:02  kaca
renamed two RPS-BLAST functions Accession is DomainId

Revision 1.4  2001/11/26 16:12:05  kaca
removed unused variables

Revision 1.3  2001/11/13 23:17:34  kaca
added CleanCommas function

Revision 1.2  2001/10/19 20:01:55  kaca
cosmetic fix on SeqId choice values

Revision 1.1.1.2  2001/10/10 21:53:48  kaca
initial import

Revision 1.6  2001/10/07 19:45:00  michel
Fixed compiler warnings

Revision 1.5  2001/10/04 18:23:35  kaca
added GetAccession from Bioseq function

Revision 1.4  2001/09/21 17:40:09  kaca
consolidating GO hierarchy project

Revision 1.3  2001/08/08 00:54:27  kaca
added TrimSpacesButOne

Revision 1.2  2001/03/29 15:44:40  kaca
moved ExpandSrqEntry from intrez.c to intrez_db.c

Revision 1.1  2001/03/26 22:34:06  kaca
created slri+misc and moved some API functions into it

*/

