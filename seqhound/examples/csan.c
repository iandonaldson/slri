/* $Id: csan.c,v 1.2 2004/01/06 22:32:11 skuznets Exp $
* ===========================================================================
*
*                         COPYRIGHT NOTICE
*                Samuel Lunenfeld Research Institute
*
*  This software/database is copyrighted.
*
*  Although all reasonable efforts have been taken to ensure the accuracy
*  and reliability of the software and data, the Samuel Lunenfeld
*  Research Institute does not and cannot warrant the performance or
*  results that may be obtained by using this software or data. The
*  The Samuel Lunenfeld Research Institutes disclaims all warranties, express
*  or implied, including warranties of performance, merchantability or fitness
*  for any particular purpose.
*
* ===========================================================================
*
* File Name:  slapamol.c
*
* Author:  Christopher Hogue
*
* Version Creation Date:   04/23/98
*
* $Revision: 1.2 $
*
* File Description: Clustal-style Masked Structure Alignments
* Modifications:
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
*
* $Log: csan.c,v $
* Revision 1.2  2004/01/06 22:32:11  skuznets
* Code audit has been done sucessfully.
* Possible NULL pointer issues has been eliminated.
*
* Revision 1.1.1.2  2002/03/25 09:27:20  root
* Automated import from Sourceforge.net
*
* Revision 1.2  2002/03/25 06:57:33  micheld
* minor change
*
* Revision 1.1.1.1  2001/05/25 21:36:19  kaca
* initial import
*
* Revision 1.1.1.1  2001/05/25 20:43:00  kaca
* code examples for SeqHound
*
* Revision 1.6  2001/05/10 18:22:11  kaca
* changed include
*
* Revision 1.5  2001/05/10 18:14:47  kaca
* removed writefastafromgi function
*
* Revision 1.4  2001/04/06 22:55:51  kaca
* small fix
*
* Revision 1.3  2001/03/26 19:59:59  kaca
* changes in include files
*
* Revision 1.2  2001/02/26 15:40:44  kaca
* all Intrez calls changed to SHound calls
*
* Revision 1.1.1.1  2001/02/23 17:54:40  kaca
* moved out of Source Forge  part of SeqHound
*
* Revision 1.1.1.1  2000/06/09 18:14:05  feldman
* TraDES project
*
*
* ==========================================================================
*/

#include <csan.h>
#include <seqhound.h>

PCSAN LIBCALL NewCSAN(void)
{
   PCSAN pcsanThis = NULL;

   pcsanThis = (PCSAN) MemNew((size_t)sizeof(CSAN));
   return pcsanThis;
}



void FreeCSAN(PCSAN pcsanThis)
{
  if (pcsanThis->next) FreeCSAN(pcsanThis->next);
  if (pcsanThis->pcSeqName) MemFree(pcsanThis->pcSeqName);
  if (pcsanThis->pvnSeqAlnPart) ValNodeFreeData(pcsanThis->pvnSeqAlnPart);
  if (pcsanThis->pcSeqAln) MemFree(pcsanThis->pcSeqAln);
  return;
}



Int4 LIBCALL WriteCSAN(PCSAN pcsanThis, FILE *pFile)
{


   /* each line has pcSeqName then xxx characters from the sequence */
   /* need a pcFrom & pcTo and an iFrom and iTo to hold the array indicies */
   CharPtr pcFrom = NULL;
   CharPtr pcTo = NULL;
   Int4 iLine = 0;
   Int4 iLineCt = 0;
   Int4 iLineIn = 0;
   Int4 iPad = 0;
   PCSAN pcsanTemp = NULL;
   Char cHold = '-';
   CharPtr pcTemp = NULL;
   CharPtr pcStart = NULL;
   CharPtr pcEnd = NULL;
   Boolean bSameLen = TRUE;
   ValNodePtr pvnSeqAlnTemp;
   Int4    iTotal;
   Int4    iLen, iSlen;



   if (!pcsanThis) return 0;
   if (!pFile) return 0;


   fprintf(pFile,"CLUSTAL X\n\n\n");
   pcsanTemp = pcsanThis;
   iLen = pcsanTemp->iLen;
   while (pcsanTemp)  /* Find longest string */
     {
        /* printf("%s[%s]\n\n",  IsNullStr(pcsanTemp->pcSeqName),  IsNullStr(pcsanTemp->pcSeqAln)); */
        /* chop up the sequence into a valnode linked list of bits to print out */
        if (iLen < pcsanTemp->iLen)
          {
            iLen = pcsanTemp->iLen;  /* find length of longest sequence */
            bSameLen = FALSE;
          }
        pcsanTemp = pcsanTemp->next;
     }
   pcsanTemp = pcsanThis;
   if (!bSameLen)
   while (pcsanTemp)  /* normalize string lengths if necessary */
     {
       /* reallocate all to match the largest iLen */
       pcTemp = (CharPtr) MemNew((size_t) ((iLen * sizeof(char)) + 2));
       StringCpy(pcTemp, pcsanTemp->pcSeqAln);
       MemFree(pcsanTemp->pcSeqAln);
       pcsanTemp->pcSeqAln = pcTemp;
       iPad = iLen - StringLen(pcTemp);
       if (iPad)
         {
           StringCat(pcTemp,"-");
         }
       pcsanTemp = pcsanTemp->next;
     }

   iLine = 0;
   iLineCt = iLen / 60;
   if (iLen % 60) iLineCt++;
   /* printf("Line[%ld], LineCt[%ld]\n", (long) iLine, (long) iLineCt); */
   pcsanTemp = pcsanThis;
   while (pcsanTemp)  /* Chop and allocate pieces into linked list */
     {
      pcStart = pcsanTemp->pcSeqAln;
      pcEnd = pcStart;
      iTotal = 0;
      while (iTotal < iLen)
        {
          iSlen=0;
          do
            {
	      iTotal++;
              iSlen++;
              pcEnd++;
          /* printf("(%ld)",(long) iSlen); */
            } while ((*pcEnd != '\0') && (iSlen < 60));

          if (*pcEnd == '\0')
	    {
	      pcTemp = StringSave(pcStart);
              ValNodeAddStr(&pcsanTemp->pvnSeqAlnPart, 0,  pcTemp);
	      break;
	    }
	  else
	   {
	    cHold = *pcEnd;
            *pcEnd = '\0';
	    pcTemp = StringSave(pcStart);
            ValNodeAddStr(&pcsanTemp->pvnSeqAlnPart, 0,  pcTemp);
            *pcEnd = cHold;
	    pcStart = pcEnd;
	   }
      	}
      pcsanTemp = pcsanTemp->next;
     }
   iLine = 0;
   iLineCt = iLen / 60;
   if (iLen % 60) iLineCt++;

   while (iLine < iLineCt)
    {
     pcsanTemp = pcsanThis;
     while (pcsanTemp)
       {
       pvnSeqAlnTemp = pcsanTemp->pvnSeqAlnPart;
       iLineIn = 0;
       while (iLineIn < iLine)
         {
           pvnSeqAlnTemp = pvnSeqAlnTemp->next;
           iLineIn++;
         }
       fprintf(pFile, "%-23.23s   %s\n",IsNullStr(pcsanTemp->pcSeqName),(CharPtr) IsNullStr(pvnSeqAlnTemp->data.ptrvalue));
       fflush(pFile);
       pcsanTemp = pcsanTemp->next;
       }
     iLine++;
     fprintf(pFile, "\n");
    }

   fflush(pFile);
   return iLen;
}


PCSAN LIBCALL LoadCSAN(CharPtr pcFile)
{

   FILE *f;
   Char fullpath [PATH_MAX];
   CharPtr ptr;
   PCSAN pcsanHead = NULL;
   PCSAN pcsanTemp = NULL;
   Char pcBuf[250];
   CharPtr ppcParse[125];
   CharPtr pcTemp = NULL;
   CharPtr pcTest = NULL;
   CharPtr pcSeqPart = NULL;
   ValNodePtr pvn = NULL;
   long int val = 0;
   Int4 i = 0;
   Int4 iString = 0;
   Int4 iLen =0;
   Int4 iWC =0;
   Int4 iWL = 0;
   Int4 iGi = 0;

   if (!pcFile) return(NULL);

   ProgramPath (fullpath, sizeof (fullpath) - 1);
   ptr = StringRChr (fullpath, DIRDELIMCHR);
   if (ptr != NULL) {
      *ptr = '\0';
   }

   /* printf("FULLPATH[%s]\n",IsNullStr(fullpath)); */

   FileBuildPath (fullpath, NULL, pcFile);
   /* printf("FULLPATH[%s]\n",IsNullStr(fullpath)); */

   if ((f = FileOpen (fullpath, "r")) == NULL)
     {
      if (FindPath ("NCBI", "NCBI", "DATA", fullpath, sizeof (fullpath) - 1))
        {
          FileBuildPath (fullpath, NULL, pcFile);
          f = FileOpen (fullpath, "r");
          if (f == NULL)
	    {
             ErrPost(CTX_NCBIOBJ, 1, "Couldn't open [%s] in LoadCSA", IsNullStr(fullpath));
             return (NULL);
            }
       }
     else
       {
         ErrPost(CTX_NCBIOBJ, 1, "FindPath failed in LoadNeighbors");
         return (NULL);
       }
    }

  do   /* get each line, tokenize into ppcParse */
     {
      pcBuf[0] = '\0';
      pcTest = fgets(pcBuf,  (size_t)250,  f);
      /* printf("[%s]\n",IsNullStr(pcTest)); */
      if (pcTest)
        {
		   iLen = StringLen(pcTest);
           /* Break into words delimited by whitespace or "|" */
           /* gi|123456| */
           /* printf("LineLength %d\n",(int) iLen); */
           for (i=0; i < 125; i++) ppcParse[i] = NULL;
           pcTemp = pcTest;
           ppcParse[0] = pcTest;
           iWC = 1;
           while ((*pcTemp != '\0') && (iWC < 125))
             {
                 if ((*pcTemp == '|') || (*pcTemp == ' ') || (*pcTemp == '\t') || (*pcTemp == '\n'))
                   {
                      *pcTemp = '\0';  /* terminate current word */
                       do {
                            pcTemp++;  /* skip whitespace to next word */
                          }  while ((*pcTemp == '|') || (*pcTemp == ' ') || (*pcTemp == '\t') || (*pcTemp == '\n'));
                       ppcParse[iWC] = pcTemp;  /* start next word */
                       iWC++;
                   }
                 else
                   pcTemp++;
             }

     /* DEBUG TEST */

/*           for (i=0; i<125; i++)
             if (ppcParse[i] != NULL)
               {
                 printf("%ld/%ld [%s]\n",i,iWC, IsNullStr(ppcParse[i]));
               } */

     /* If ppcParse[0] is "gi" then this is a VALID sequence line and the last
        word parsed is the sequence */
      if (ppcParse[0][0] != '\0')
        {
           iWL = StringLen(ppcParse[0]);
           if (iWL == 2)
             if (((ppcParse[0][0] == 'g') ||
                (ppcParse[0][0] == 'G')) &&
                ((ppcParse[0][1] == 'i') ||
                (ppcParse[0][1] == 'I')))
               {
                 /* next word should be GI */
                  iGi = 0;
                  if (StrToInt4 (ppcParse[1], &iGi))
                    {
                      /* printf("GI is (%ld)\n",iGi); */
                      if (iGi == 0) goto parserr;
                      /* find the 2nd last string = sequence */
                      /* last string is stripped off line terminator, by the way... */
                      iString=2; /* start after GI */
                      while (ppcParse[iString+1] != NULL)
                        {  /* iString is the LAST sting - line terminator */
                              iString++;
                         }
                      pcsanTemp = pcsanHead;
                      while (pcsanTemp)
                        {  /* look for existing pcsan */
                         if (iGi == pcsanTemp->iGi)
                          {
                            break;
                          }
                         pcsanTemp = pcsanTemp->next;
                        }
                      if (!pcsanTemp) /* otherwise add a new one */
                        {
                          pcsanTemp = NewCSAN();
                          pcsanTemp->next = pcsanHead;
                          pcsanHead = pcsanTemp; /* put at head of list */
                          pcsanTemp->iGi = iGi;
                        }
                      /* Initialize fields */
                      pcSeqPart = NULL;
                      pcSeqPart = StringSave(ppcParse[iString-1]);
                      /* printf("SEQ [%ld]= [%s]\n",(long) iString-1, IsNullStr(pcSeqPart)); */
                      ValNodeAddStr(&pcsanTemp->pvnSeqAlnPart, 0,  pcSeqPart);
                    }  /* if GI */
               }  /* if line starts with GI */
         }  /* if line starts with a word */
      } /* if pcTest */
    } while (pcTest);


  printf("REPORT\n");
  pcsanTemp = pcsanHead;
  while (pcsanTemp)
  {
     printf("GI = [%ld]\n", (long) pcsanTemp->iGi);
     pvn = pcsanTemp->pvnSeqAlnPart;
     while (pvn)
       {
         printf(">%s<\n",(char *) IsNullStr(pvn->data.ptrvalue));
         pvn = pvn->next;
       }
     pcsanTemp = pcsanTemp->next;
  }

   FileClose(f);
   return pcsanHead;


parserr:
   FileClose(f);
   ErrPostEx(SEV_FATAL,0,0, "Error in input format \n");
   FreeCSAN(pcsanHead);
   return NULL;

}



static PMMD MMDBGetMoleculeByGI(PDNMS pdnmsThis, Int4 iGi)
{
  PMSD pmsdThis = NULL;
  PDNMM pdnmmThis = NULL;
  PMMD pmmdThis = NULL;

  if (!pdnmsThis) return NULL;
  if (!iGi) return NULL;

  pmsdThis = (PMSD) pdnmsThis->data.ptrvalue;
  pdnmmThis = pmsdThis->pdnmmHead;
  while (pdnmmThis)
    {
      pmmdThis = (PMMD) pdnmmThis->data.ptrvalue;
      if (iGi == pmmdThis->iGi) return pmmdThis;
      pdnmmThis = pdnmmThis->next;
    }
  return NULL;
}



static Int4  FillCSANWithStru(PCSAN pcsanThis, PMMD pmmdThis, Int4 iLen)
{
     Int4 iCount = 0;
     CharPtr pcA;
     PDNMG pdnmgThis = NULL;
     PMGD pmgdThis = NULL;


     if (!pcsanThis) return 0;
     if (!pmmdThis) return 0;
     if (!IsProtein(pmmdThis)) return 0;
     if (!iLen) return 0;

     pcsanThis->pcSeqAln = (CharPtr)MemNew((size_t) (1+ sizeof(char) * iLen));
     pcA = pcsanThis->pcSeqAln;
     pdnmgThis = pmmdThis->pdnmgHead;
     iCount = 0;
     while ((pdnmgThis != NULL)  && (iCount < iLen))
      {
            iCount++;
            pmgdThis = (PMGD) pdnmgThis->data.ptrvalue;
            *pcA = (char) pmgdThis->pcIUPAC[0] ;
            pcA++;
            pdnmgThis = pdnmgThis->next;
      }
     while (iCount < iLen)
       {
            *pcA = '-';
            pcA++;
            iCount++;
       }
    pcsanThis->pcSeqAln[iLen] = '\0';
    return iCount;
}




static Int4 FillCSANWithMask(PCSAN pcsanThis, PMMD pmmdThis, Int4 iLen)
{
     Int4 iCount = 0;
     CharPtr pcA;
     PDNMG pdnmgThis = NULL;
     PMGD pmgdThis = NULL;
     Char cCode = ' ';


     if (!pcsanThis) return 0;
     if (!pmmdThis) return 0;
     if (!IsProtein(pmmdThis)) return 0;
     if (!iLen) return 0;


     /* printf("Mask len=[%ld]{",(long) iLen);*/
     pcsanThis->pcSeqAln = (CharPtr)MemNew((size_t) (1+ sizeof(char) * iLen));
     pcA = pcsanThis->pcSeqAln;
     pdnmgThis = pmmdThis->pdnmgHead;
     iCount = 0;
     while ((pdnmgThis != NULL)  && (iCount < iLen))
      {
           iCount++;
           pmgdThis = (PMGD) pdnmgThis->data.ptrvalue;
           cCode = '.';  /* default loop character */
           if (pmgdThis->bPDBSecStru & (Byte) SS_HELIX)  /*author described */
              cCode = 'a';
           if (pmgdThis->bNCBISecStru & (Byte) SS_HELIX)  /* VAST ss definition */
              cCode = 'a';
           if ((pmgdThis->bPDBSecStru & (Byte) SS_HELIX) && (pmgdThis->bNCBISecStru & (Byte) SS_HELIX))
              cCode = 'A';  /* consensus !*/
           if (pmgdThis->bPDBSecStru & (Byte) SS_STRAND)  /* author described */
              cCode = 'b';
           if  (pmgdThis->bNCBISecStru & (Byte) SS_STRAND)  /* VAST ss definition */
              cCode = 'b';
           if ((pmgdThis->bPDBSecStru & (Byte) SS_STRAND) && (pmgdThis->bNCBISecStru & (Byte) SS_STRAND))
              cCode = 'B';  /* consensus */
           /* printf("%c",  cCode); */
            *pcA = cCode ;
            pcA++;
            pdnmgThis = pdnmgThis->next;
      }
     while (iCount < iLen)  /* pad to the end if this is short */
       {
            *pcA = '-';
          /* printf("%c",  '-');*/
            pcA++;
            iCount++;
       }
    pcsanThis->pcSeqAln[iLen] = '\0';
    /* printf("}\n"); */
    return iCount;
}


/* fills CSAN with the sequence from the bioseq, padded to length iLen*/
/*  WARNING not called and not tested... */
static Int4 FillCSANWithSeq(PCSAN pcsanThis, BioseqPtr pbsq, Int4 iLen)
{
     SeqPortPtr spp = NULL;
     Uint1 code = Seq_code_ncbieaa;
     Uint1 residue;
     Int4 iCount = 0;
     CharPtr pcA;

     if (!pcsanThis) return 0;
     if (!ISA_aa(pbsq->mol)) return 0;
     if (!iLen) return 0;

     spp = SeqPortNew(pbsq, 0, -1, 0, code);
     if (!spp) return 0;
     SeqPortSeek(spp, 0, SEEK_SET);
     pcsanThis->pcSeqAln = (CharPtr)MemNew((size_t) (1+ sizeof(char) * iLen));
     pcA = pcsanThis->pcSeqAln;
     residue = SeqPortGetResidue(spp);
     iCount = 0;
     while ((residue != SEQPORT_EOF) && (residue != '\0') && (iLen < iCount))
      {
            iCount++;
            *pcA = (char) residue;
            pcA++;
            residue = SeqPortGetResidue(spp);
      }
     while (iCount < iLen)
       {
            *pcA = '-';
            pcA++;
            iCount++;
      }
    pcsanThis->pcSeqAln[iLen] = '\0';
    SeqPortFree(spp);
    return iCount;
}



Int4 MMDBidfromGI(Int4 iGi)
{
    Int4 iMMDBid = 0;

    iMMDBid = 0;
    if (!iGi) return 0;
    iMMDBid = SHound3DFromGi(iGi);
    return iMMDBid;
}


PCSAN CSANStructMask(Int4 iMMDBid, Int4 iGiStruc)
{

    Int4 iNumSeq = 0;
    BiostrucPtr pbsThis = NULL;
    Int4 iStrucLen = 0;
    PDNMS pdnmsStructure = NULL;
    PMSD  pmsdThis = NULL;
    PMMD  pmmdMol = NULL;
    PCSAN pcsanStruc = NULL;
    PCSAN pcsanMask = NULL;
    Char pcName[31];


    if (iGiStruc == 0)
      return NULL;

     /* get a pdnms to the structure MMDBid, then a pdnmm to the embedded Gi */

     if((pbsThis = SHoundGet3D(iMMDBid))!= NULL)
        pdnmsStructure = MakeAModelstruc(pbsThis);

     if (!pdnmsStructure)
       {
         ErrPostEx(SEV_ERROR, 0, 0,"Fatal Error during MMDB structure retrieval %ld\n", (long) iMMDBid);
         return NULL;
       }

    pmmdMol = MMDBGetMoleculeByGI(pdnmsStructure, iGiStruc);
    pmsdThis = (PMSD) pdnmsStructure->data.ptrvalue;


    pcsanMask = NewCSAN();
    pcsanMask->iGi = iGiStruc;
    pcsanMask->next = NewCSAN();
    pcsanStruc= pcsanMask->next;
    pcsanMask->iLen = pmmdMol->iResCount;
    pcsanStruc->iGi = iGiStruc;
    pcsanStruc->iLen = pmmdMol->iResCount;

    /* name of stru = "%s" */
    /* name of SS = "!SS_%s" */

    pcName[0] = '\0';
    sprintf(pcName,"!SS_gi|%ld|pdb|%4s|%1s",(long) iGiStruc, pmsdThis->pcPDBName, pmmdMol->pcMolName);
    pcsanMask->pcSeqName = StringSave(pcName);
    if (StringLen(pcName) > 23) pcsanMask->pcSeqName[23] = '\0';
    sprintf(pcName,"gi|%ld|pdb|%4s|%1s",(long) iGiStruc, IsNullStr(pmsdThis->pcPDBName), IsNullStr(pmmdMol->pcMolName));
    pcsanStruc->pcSeqName = StringSave(pcName);
    if (StringLen(pcName) > 23) pcsanMask->pcSeqName[23] = '\0';

    /* fill the sec-structure mask with the structure, pad the remainder */
    FillCSANWithMask(pcsanMask, pmmdMol, pmmdMol->iResCount);

    /* fill the CSAN node with the structure, pad the remainder if shorter */
    FillCSANWithStru(pcsanStruc, pmmdMol, pmmdMol->iResCount);

    ClearStructures();

    return pcsanMask;

}

/*
$Log: csan.c,v $
Revision 1.2  2004/01/06 22:32:11  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.2  2002/03/25 09:27:20  root
Automated import from Sourceforge.net

Revision 1.2  2002/03/25 06:57:33  micheld
minor change

Revision 1.1.1.1  2001/05/25 21:36:19  kaca
initial import

Revision 1.1.1.1  2001/05/25 20:43:00  kaca
code examples for SeqHound

Revision 1.6  2001/05/10 18:22:11  kaca
changed include

Revision 1.5  2001/05/10 18:14:47  kaca
removed writefastafromgi function

Revision 1.4  2001/04/06 22:55:51  kaca
small fix

Revision 1.3  2001/03/26 19:59:59  kaca
changes in include files

Revision 1.2  2001/02/26 15:40:44  kaca
all Intrez calls changed to SHound calls

Revision 1.1.1.1  2001/02/23 17:54:40  kaca
moved out of Source Forge  part of SeqHound

*/

