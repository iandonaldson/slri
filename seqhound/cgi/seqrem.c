/* $Id: seqrem.c,v 1.69 2005/03/31 18:43:35 vgu Exp $
******************************************************************************************
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
     Oracle Corporation.


  PROGRAM: SeqHound API Remote CGI
 

  AUTHORS: Katerina Michalickova
           Michel Dumontier
	   Hao Lieu
	   and Christopher W.V. Hogue hogue@mshri.on.ca


  REVISION/CONTRIBUTION NOTES:

  June 1, 2000  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:


  DESCRIPTION
  September 1999
  cgi for remote SeqHound API


******************************************************************************************/
#ifdef SHFCGI
#include <fcgi_stdio.h>
#include <fcgi_config.h>
#endif

#ifdef ODBC
#include <intrez_odbc.h>
#else
#include <intrez_db.h>
#endif

#include <intrez_misc.h>
#include <slri.h>
#include <seqhound.h>
#include <objNBlastasn.h>
#include <connect/ncbi_http_connector.h>
#include <connect/ncbi_util.h>

#define BIN_MODE
/* This define applies to changing the type of ASN that is written to client    */
/* Make sure that the client (seqhoundapi.c in seqhoundrem lib) has the correct */
/* setting for reading the file (i.e. binary or text )                          */
/* Comment out the define to use text asn mode                                  */
#ifdef BIN_MODE
#define ASN_MODE ASNIO_BIN_OUT
#else
#define ASN_MODE ASNIO_TEXT_OUT 
#endif
#define FILE_MODE "wb" /* don't change this */
#define GROUP "#;#"    /*a separator for return group*/
#define Q_R   "#~#"   /*a separator for query-return pair*/

/* Global info pointer */
WWWInfoPtr ginfo;

Boolean bVerbose = FALSE;
Boolean NetEntrezOn = FALSE;

SLRI_ERR SeqHoundInit(void)
{
  CharPtr papp = NULL;
  CharPtr pnet = NULL;
  CharPtr pcThis = NULL;
  Int4 IndexArgs = -1;


  printf("Content-type: text/html\r\n\r\n");
  if(!SHoundIsInited())
   {
     fprintf(stdout, "SEQHOUND_OK\n");
     fprintf(stdout, "FALSE\n");
   }
  else
   {
     if ((IndexArgs = WWWFindName(ginfo,"NetEntrezOnToo")) >= 0)
      {
         pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
         pnet = StringSave(pcThis);
      }
     if ((pnet == NULL) || (strlen(pnet) == 0))
      {
         ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
         fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
         return SLRI_FAIL;
      }
     if ((IndexArgs = WWWFindName(ginfo,"appname")) >= 0)
      {	
          pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
          papp = StringSave(pcThis);
      }
     if ((papp == NULL) || (strlen(papp) == 0))
      {
        ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
        fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
        MemFree(pnet);
        return SLRI_FAIL;
      }
#ifdef INTREZ_NET
     /* HL:
     SHoundGetFasta also uses net entrez 
     (NetEntrezGetFasta) based on value of NetEntrezOn */
     if(strcmp(pnet, "TRUE") == 0)
     {
	/* HL: Uncomment this line to activate NetEntrezGetFasta
	   NetEntrezOn = TRUE; */
        if (!EntrezInit(papp, FALSE, NULL))
        {
           ErrPostEx(SEV_FATAL, 0, 0, "EntrezInit failed.");
           fprintf(stdout, "SEQHOUND_OK\n");
           fprintf(stdout, "FALSE\n");
        } else { 
           EntrezFini();
           fprintf(stdout, "SEQHOUND_OK\n");
           fprintf(stdout, "TRUE\n");
	}
     }
#endif
     /* regardless whether NetEntrezOnToo is set to true or false
       if ( 0 == strcmp ( pnet, "TRUE" ) || 
            0 == strcmp ( pnet, "FALSE" ) ) */
     
     {
        fprintf(stdout, "SEQHOUND_OK\n");
        fprintf(stdout, "TRUE\n");
     }

     MemFree(pnet);
     MemFree(papp);
   }
   return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundIsNetEntrezOn(void)
{
   printf("Content-type: text/html\r\n\r\n");
#ifdef INTREZ_NET
   if (!EntrezInit("seqrem", FALSE, NULL))
   {
      ErrPostEx(SEV_FATAL, 0, 0, "EntrezInit failed.");
      fprintf(stdout, "SEQHOUND_OK\n");
      fprintf(stdout, "FALSE\n");
   } else {
      EntrezFini();
      fprintf(stdout, "SEQHOUND_OK\n");
      fprintf(stdout, "TRUE\n");
   }
#endif
   fprintf(stdout, "SEQHOUND_OK\n");
   fprintf(stdout, "TRUE\n");
   return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundNetEntrezInit(void)
{
   printf("Content-type: text/html\r\n\r\n");
#ifdef INTREZ_NET
   if (!EntrezInit("seqrem", FALSE, NULL))
   {
      ErrPostEx(SEV_FATAL,0,0, "EntrezInit failed.\n");
      fprintf(stdout, "SEQHOUND_OK\n");
      fprintf(stdout, "FALSE\n");
   }
   else
   {
      EntrezFini();
      fprintf(stdout, "SEQHOUND_OK\n");
      fprintf(stdout, "TRUE\n");
   }
#endif
   fprintf(stdout, "SEQHOUND_OK\n");
   fprintf(stdout, "TRUE\n");
   return SLRI_SUCCESS;
}


SLRI_ERR PostError(CharPtr errortype, CharPtr error)
{
	if(errortype == NULL || error == NULL) {
		ErrPostEx(SEV_ERROR,0,0, "PostError:  Invalid Parameters.");
		printf("Content-type: text/html\r\n\r\n");
		fprintf(stdout, "SEQHOUND_ERROR PostError:  Invalid Parameters.\n");
		return SLRI_FAIL;
	}
	ErrPostEx(SEV_ERROR,0,0, "%s", IsNullStr(error));
	printf("Content-type: text/html\r\n\r\n");
	fprintf(stdout, "%s %s\n", IsNullStr(errortype), IsNullStr(error));

	return SLRI_SUCCESS;
}



CharPtr FLinkSet2CommaStr(FLinkSetPtr pfl)
{
   Int4 i = 0;
   CharPtr pstr = NULL;
   CharPtr tmpStr = NULL;
   ByteStorePtr bsp = NULL;

   if(pfl == NULL) {
     ErrPostEx(SEV_ERROR,0,0, "FLinkSet2CommaStr: Invalid parameter.");
     return NULL;
   }

   bsp = BSNew(10);
   /* SK: NULL ptr check */
   if ( bsp == NULL )
      return NULL;

   BSSeek(bsp, 0L, SEEK_SET);

   for(i = 0; i < (Int4) pfl->num; i++) {
     tmpStr = Ltostr((Int4) pfl->uids[i], 0);
     BSWrite(bsp, tmpStr, StringLen(tmpStr));
     if(i != pfl->num-1)
      BSWrite(bsp, ",", 1);
   }

   pstr = BSStringSave(bsp);
   BSFree(bsp);

   return pstr;
}



CharPtr FLinkSetList2CommaStr(FLinkSetListPtr pfll)
{
   Int4 i = 0;
   CharPtr pstr = NULL;
   CharPtr tmpStr = NULL;
   ByteStorePtr bsp = NULL;

   if(pfll == NULL)
   {
     ErrPostEx(SEV_ERROR,0,0, "FLinkSetList2CommaStr: Invalid parameter.");
     return NULL;
   }

   bsp = BSNew(10);
   /* SK: NULL ptr check */
   if ( bsp == NULL )
      return NULL;

   BSSeek(bsp, 0L, SEEK_SET);

   while(pfll != NULL)
   {
     /* SK: NULL ptr check */
     if ( pfll->set1 != NULL )
     {
        for(i = 0;  i < (Int4) pfll->set1->num; i++)
        {
           tmpStr = Ltostr((Int4) pfll->set1->uids[i], 0);
           BSWrite(bsp, tmpStr, StringLen(tmpStr));
           if((pfll->next == NULL) && (i == pfll->set1->num-1))
             ;
           else
             BSWrite(bsp, ",", 1);
        }
     }
     pfll = pfll->next;
   }

   pstr = BSStringSave(bsp);
   BSFree(bsp);

   return pstr;
}


CharPtr ILinkSet2CommaStr(ILinkSetPtr pfl)
{
   Int4 i = 0;
   CharPtr pstr = NULL;
   CharPtr tmpStr = NULL;
   ByteStorePtr bsp = NULL;

   if(pfl == NULL)
   {
     ErrPostEx(SEV_ERROR,0,0, "FLinkSet2CommaStr: Invalid parameter.");
     return NULL;
   }

   bsp = BSNew(10);
   /* SK: NULL ptr check */
   if ( bsp == NULL )
      return NULL;

   BSSeek(bsp, 0L, SEEK_SET);

   for(i = 0; i < (Int4) pfl->num; i++)
   {
     tmpStr = Ltostr((Int4) pfl->uids[i], 0);
     BSWrite(bsp, tmpStr, StringLen(tmpStr));
     if(i != pfl->num-1)
      BSWrite(bsp, ",", 1);
   }

   pstr = BSStringSave(bsp);
   BSFree(bsp);

   return pstr;
}



CharPtr ILinkSetList2CommaStr(ILinkSetListPtr pfll)
{
   Int4 i = 0;
   CharPtr pstr = NULL;
   CharPtr tmpStr = NULL;
   ByteStorePtr bsp = NULL;

   if(pfll == NULL)
   {
     ErrPostEx(SEV_ERROR,0,0, "FLinkSetList2CommaStr: Invalid parameter.");
     return NULL;
   }

   bsp = BSNew(10);
   /* SK: NULL ptr check */
   if ( bsp == NULL )
      return NULL;

   BSSeek(bsp, 0L, SEEK_SET);

   while(pfll != NULL)
   {
     /* SK: NULL ptr check */
     if ( pfll->set1 != NULL )
     {
        for(i = 0; i < (Int4) pfll->set1->num; i++)
        {
           tmpStr = Ltostr((Int4) pfll->set1->uids[i], 0);
           BSWrite(bsp, tmpStr, StringLen(tmpStr));
           if((pfll->next == NULL) && (i == pfll->set1->num-1))
             ;
           else
             BSWrite(bsp, ",", 1);
        }
     }
     pfll = pfll->next;
   }

   pstr = BSStringSave(bsp);
   BSFree(bsp);

   return pstr;
}



SLRI_ERR SeqHoundFini(void)
{
  printf("Content-type: text/html\r\n\r\n");
  if(!SHoundIsInited())
   {
     fprintf(stdout, "SEQHOUND_OK\n");
     fprintf(stdout, "FALSE\n");
   }
  else
   {
     fprintf(stdout, "SEQHOUND_OK\n");
     fprintf(stdout, "TRUE\n");
   }
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundFindAcc(void)
{
  CharPtr pcThis = NULL;
  CharPtr pacc = NULL;
  Int4 gi = 0;
  Int4 IndexArgs = -1;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"acc")) >= 0)
   {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       pacc = StringSave(pcThis);
   }
  if ((pacc == NULL) || (strlen(pacc) == 0))
   {
      ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  gi = SHoundFindAcc(pacc);
  fprintf(stdout, "SEQHOUND_OK\n");
  fprintf(stdout, "%ld\n", (long) gi);
  MemFree(pacc);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundFindAccList(void)
{
  CharPtr pcThis = NULL;
  CharPtr pacc = NULL;
  ValNodePtr pvnacc = NULL;
  ValNodePtr pvngi = NULL;
  CharPtr pstr = NULL;
  Int4 IndexArgs = -1;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"pacc")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pacc = StringSave(pcThis);
   }
  if (pacc == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  pvnacc = Misc_CommaStrString2ValNodeList(pacc, ",");
  if(pvnacc == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Misc_CommaStrString2ValNodeList failed.");
      fprintf(stdout, "SEQHOUND_ERROR ValNode conversion error.\n");
      MemFree(pacc);
      return SLRI_FAIL;
   }
  pvngi = SHoundFindAccList(pvnacc);
  pstr = Misc_SaveValNodeListAsString(pvngi);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(pacc);
  MemFree(pstr);
  ValNodeFreeData(pvnacc);
  ValNodeFree(pvngi);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundFindName(void)
{
  CharPtr pcThis = NULL;
  CharPtr pacc = NULL;
  Int4 gi = 0;
  Int4 IndexArgs = -1;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"name")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pacc = StringSave(pcThis);
   }
  if ((pacc == NULL) || (strlen(pacc)==0))
   {
      ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  gi = SHoundFindName(pacc);
  fprintf(stdout, "SEQHOUND_OK\n");
  fprintf(stdout, "%ld\n", (long) gi);
  MemFree(pacc);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundFindNameList(void)
{
  CharPtr pcThis = NULL;
  CharPtr pacc = NULL;
  ValNodePtr pvnacc = NULL;
  ValNodePtr pvngi = NULL;
  CharPtr pstr = NULL;
  Int4 IndexArgs = -1;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"pname")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pacc = StringSave(pcThis);
   }
  if (pacc == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  pvnacc = Misc_CommaStrString2ValNodeList(pacc, ",");
  if(pvnacc == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Misc_CommaStrString2ValNodeList failed.");
      fprintf(stdout, "SEQHOUND_ERROR ValNode conversion error.\n");
      MemFree(pacc);
      return SLRI_FAIL;
   }
  pvngi = SHoundFindNameList(pvnacc);
  pstr = Misc_SaveValNodeListAsString(pvngi);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(pacc);
  MemFree(pstr);
  ValNodeFreeData(pvnacc);
  ValNodeFree(pvngi);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGetAccFromRetiredGi(void)
{
  CharPtr pcThis = NULL;
  CharPtr giptr = NULL;
  CharPtr pacc = NULL;
  Int4 IndexArgs = -1;
  Int4 gival = 0;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"gi")) >= 0)
    {
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       giptr = StringSave(pcThis);
    }
  if (giptr == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
     }
  if(!StrToInt4 (giptr, &gival))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(giptr);
      return SLRI_FAIL;
   }
  pacc = SHoundGetAccFromRetiredGi(gival);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pacc == NULL)
   fprintf(stdout, "(null)\n");
  else
   fprintf(stdout, "%s\n", pacc);
  MemFree(giptr);
  MemFree(pacc);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundAccFromGi(void)
{
  CharPtr pcThis = NULL;
  CharPtr giptr = NULL;
  CharPtr pacc = NULL;
  Int4 IndexArgs = -1;
  Int4 gival = 0;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"gi")) >= 0)
    {
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       giptr = StringSave(pcThis);
    }
  if (giptr == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
     }
  if(!StrToInt4 (giptr, &gival))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(giptr);
      return SLRI_FAIL;
   }
  pacc = SHoundAccFromGi(gival);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pacc == NULL)
   fprintf(stdout, "(null)\n");
  else
   fprintf(stdout, "%s\n", pacc);
  MemFree(giptr);
  MemFree(pacc);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundAccFromGiList(void)
{
  CharPtr pcThis = NULL;
  CharPtr pgi = NULL;
  ValNodePtr pvnacc = NULL;
  ValNodePtr pvngi = NULL;
  CharPtr pstr = NULL;
  Int4 IndexArgs = -1;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"pgi")) >= 0)
    {
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       pgi = StringSave(pcThis);
    }
  if (pgi == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
    }
  pvngi = Misc_CommaIntString2ValNodeList(pgi, ",");
  if(pvngi == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Misc_CommaIntString2ValNodeList failed.");
      fprintf(stdout, "SEQHOUND_ERROR Misc_CommaIntString2ValNodeList failed.\n");
      MemFree(pgi);
      return SLRI_FAIL;
   }
  pvnacc =  SHoundAccFromGiList(pvngi);
  if(pvnacc == NULL)
     ErrPostEx(SEV_ERROR,0,0, "SHoundAccFromGiList returned NULL.");
  pstr = Misc_SaveStrValNodeListAsString(pvnacc);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(pgi);
  MemFree(pstr);
  ValNodeFreeData(pvnacc);
  ValNodeFree(pvngi);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundTitleFromGi(void)
{
  CharPtr pcThis = NULL;
  CharPtr giptr = NULL;
  CharPtr ptitle = NULL;
  Int4 IndexArgs = -1;
  Int4 gival = 0;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"gi")) >= 0)
    {
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       giptr = StringSave(pcThis);
    }
  if (giptr == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
     }
  if(!StrToInt4 (giptr, &gival))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(giptr);
      return SLRI_FAIL;
   }
  ptitle = SHoundTitleFromGi(gival);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(ptitle == NULL)
   fprintf(stdout, "(null)\n");
  else
   fprintf(stdout, "%s\n", ptitle);
  MemFree(giptr);
  MemFree(ptitle);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundTitleFromGiList(void)
{
  CharPtr pcThis = NULL;
  CharPtr pgi = NULL;
  ValNodePtr pvntitle = NULL;
  ValNodePtr pvngi = NULL;
  CharPtr pstr = NULL;
  Int4 IndexArgs = -1;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"pgi")) >= 0)
    {
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       pgi = StringSave(pcThis);
    }
  if (pgi == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
    }
  pvngi = Misc_CommaIntString2ValNodeList(pgi, ",");
  if(pvngi == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Misc_CommaIntString2ValNodeList failed.");
      fprintf(stdout, "SEQHOUND_ERROR Misc_CommaIntString2ValNodeList failed.\n");
      MemFree(pgi);
      return SLRI_FAIL;
   }
  pvntitle =  SHoundTitleFromGiList(pvngi);
  if(pvntitle == NULL)
     ErrPostEx(SEV_ERROR,0,0, "SHoundTitleFromGiList returned NULL.");
  pstr = Misc_SaveStrValNodeListAsString(pvntitle);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(pgi);
  MemFree(pstr);
  ValNodeFreeData(pvntitle);
  ValNodeFree(pvngi);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundSeqIdFromGi(void)
{
  CharPtr pcThis = NULL;
  CharPtr giptr = NULL;
  SeqIdPtr psi = NULL;
  AsnIoPtr aip = NULL;
  FILE *StdOot=NULL;
  SeqIdPtr psinew = NULL;
  Int4 IndexArgs = -1;
  Int4 gival = 0;

  if ((IndexArgs = WWWFindName(ginfo,"gi")) >= 0)
    {
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       giptr = StringSave(pcThis);
    }
  if (giptr == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
     }
  if(!StrToInt4 (giptr, &gival))
   {
       ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
       MemFree(giptr);
       return SLRI_FAIL;
   }
  psi =  SHoundSeqIdFromGi(gival);
  if(psi == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "SHoundSeqIdFromGi returned NULL.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
      MemFree(giptr);
      return SLRI_FAIL;
   }
  StdOot=FileOpen("stdout", FILE_MODE);
  if (StdOot == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Unable to open a file.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Unable to open a file.\n");
      MemFree(giptr);
      FreeSeqId(psi);
      return SLRI_FAIL;
   }
  if((aip=AsnIoNew(ASN_MODE, StdOot, NULL,NULL,NULL)) == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Unable to open ASN.1 IO.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Unable to open ASN.1 IO.\n");
      MemFree(giptr);
      FreeSeqId(psi);
      FileClose(StdOot);
      return SLRI_FAIL;
   }
  psinew = psi;
  printf("Content-type: application/x-octet-stream\n\n");
  fprintf(stdout, "SEQHOUND_OK\n");
  SLRISeqIdAsnWrite(psi, aip, NULL);
  AsnIoClose(aip);
  FreeSeqId(psinew);
  FileClose(StdOot);
  MemFree(giptr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundSeqIdFromGiList(void)
{
  CharPtr pcThis = NULL;
  CharPtr pgi = NULL;
  ValNodePtr pvngi = NULL;
  ValNodePtr pvnpsi = NULL;
  AsnIoPtr aip = NULL;
  FILE *StdOot=NULL;
  Int4 IndexArgs = -1;


  if ((IndexArgs = WWWFindName(ginfo,"pgi")) >= 0)
    {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       pgi = StringSave(pcThis);
    }
  if (pgi == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
    }
  pvngi = Misc_CommaIntString2ValNodeList(pgi, ",");
  if(pvngi == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Misc_CommaIntString2ValNodeList failed.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Misc_CommaIntString2ValNodeList failed.\n");
      MemFree(pgi);
      return SLRI_FAIL;
   }
  pvnpsi = SHoundSeqIdFromGiList(pvngi);
  if(pvnpsi == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "SHoundSeqIdFromGiList returned NULL.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
      MemFree(pgi);
      ValNodeFree(pvngi);
      return SLRI_FAIL;
   }
  StdOot=FileOpen("stdout", FILE_MODE);
  if (StdOot == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Unable to open a file.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Unable to open a file.\n");
      MemFree(pgi);
      ValNodeFree(pvngi);
      FreeVNSeqIdList(pvnpsi);
      return SLRI_FAIL;
   }
  if((aip=AsnIoNew(ASN_MODE, StdOot, NULL,NULL,NULL)) == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Unable to open ASN.1 IO.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Unable to open ASN.1 IO.\n");
      MemFree(pgi);
      ValNodeFree(pvngi);
      FreeVNSeqIdList(pvnpsi);
      FileClose(StdOot);
      return SLRI_FAIL;
   }
  printf("Content-type: application/x-octet-stream\n\n");
  fprintf(stdout, "SEQHOUND_OK\n");
  SLRIValNodeAsnWrite((SLRIValNodePtr) pvnpsi, aip, NULL);
  AsnIoClose(aip);
  FileClose(StdOot);
  FreeVNSeqIdList(pvnpsi);
  MemFree(pgi);
  ValNodeFree(pvngi);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundSeqIdFromGiPL(void)
{
  CharPtr pcThis = NULL;
  CharPtr giptr = NULL;
  Int4 gival = 0;
  SeqIdPtr psi = NULL, psinew = NULL;
  AsnIoPtr aip = NULL;
  FILE *StdOot=NULL;
  Int4 IndexArgs = -1;

  if ((IndexArgs = WWWFindName(ginfo,"gi")) >= 0)
    {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       giptr = StringSave(pcThis);
    }
  if (giptr == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
     }
  if(!StrToInt4 (giptr, &gival))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(giptr);
      return SLRI_FAIL;
   }
  psi =  SHoundSeqIdFromGi(gival);
  if(psi == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "SHoundSeqIdFromGi returned NULL.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
      MemFree(giptr);
      return SLRI_FAIL;
   }
  if((aip=AsnIoNew(ASNIO_TEXT_OUT, stdout, NULL,NULL,NULL)) == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Unable to open ASN.1 IO.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Unable to open ASN.1 IO.\n");
      MemFree(giptr);
      FreeSeqId(psi);
      FileClose(StdOot);
      return SLRI_FAIL;
   }
  psinew = psi;
  printf("Content-type: text/html\r\n\r\n");
  fprintf(stdout, "SEQHOUND_OK\n");
  SLRISeqIdAsnWrite(psi, aip, NULL);
  AsnIoClose(aip);
  FreeSeqId(psinew);
  MemFree(giptr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundSeqIdFromGiListPL(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pgi = NULL;
  ValNodePtr pvngi = NULL;
  ValNodePtr pvnpsi = NULL;
  AsnIoPtr aip = NULL;
  FILE *StdOot=NULL;

  if ((IndexArgs = WWWFindName(ginfo,"pgi")) >= 0)
    {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       pgi = StringSave(pcThis);
    }
  if (pgi == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
    }
  pvngi = Misc_CommaIntString2ValNodeList(pgi, ",");
  if(pvngi == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Misc_CommaIntString2ValNodeList failed.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Misc_CommaIntString2ValNodeList failed.\n");
      MemFree(pgi);
      return SLRI_FAIL;
   }
  pvnpsi = SHoundSeqIdFromGiList(pvngi);
  if(pvnpsi == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "SHoundSeqIdFromGiList returned NULL.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
      MemFree(pgi);
      ValNodeFree(pvngi);
      return SLRI_FAIL;
   }
  if((aip=AsnIoNew(ASNIO_TEXT_OUT, stdout, NULL,NULL,NULL)) == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Unable to open ASN.1 IO.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Unable to open ASN.1 IO.\n");
      MemFree(pgi);
      ValNodeFree(pvngi);
      FreeVNSeqIdList(pvnpsi);
      FileClose(StdOot);
      return SLRI_FAIL;
   }
  printf("Content-type: text/html\r\n\r\n");
  fprintf(stdout, "SEQHOUND_OK\n");
  SLRIValNodeAsnWrite((SLRIValNodePtr) pvnpsi, aip, NULL);
  AsnIoClose(aip);
  FreeVNSeqIdList(pvnpsi);
  MemFree(pgi);
  ValNodeFree(pvngi);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundMoleculeType(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr giptr = NULL;
  Int4 gi = 0;
  CharPtr moletype = 0;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"gi")) >= 0)
    {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       giptr = StringSave(pcThis);
    }
  if (giptr == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
     }
  if(!StrToInt4 (giptr, &gi))
   {
       ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
       fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
       MemFree(giptr);
       return SLRI_FAIL;
   }

  moletype = SHoundMoleculeType(gi);
  fprintf(stdout, "SEQHOUND_OK\n");
  fprintf(stdout, "%s\n", IsNullStr(moletype));
  MemFree(giptr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundIsProtein(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr giptr = NULL;
  Int4 gival = 0;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"gi")) >= 0)
    {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       giptr = StringSave(pcThis);
    }
  if (giptr == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
     }
  if(!StrToInt4 (giptr, &gival))
   {
       ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
       fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
       MemFree(giptr);
       return SLRI_FAIL;
   }
  if(SHoundIsProtein(gival))
   {
     fprintf(stdout, "SEQHOUND_OK\n");
     fprintf(stdout, "TRUE\n");
   }
  else
   {
     fprintf(stdout, "SEQHOUND_OK\n");
     fprintf(stdout, "FALSE\n");
   }
  MemFree(giptr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundIsNRFirst(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr giptr = NULL;
  Int4 gival = 0;


  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"gi")) >= 0)
    {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       giptr = StringSave(pcThis);
    }
  if (giptr == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
     }
  if(!StrToInt4 (giptr, &gival))
   {
       ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
       fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
       MemFree(giptr);
       return SLRI_FAIL;
   }
  if(SHoundIsNRFirst(gival))
   {
       fprintf(stdout, "SEQHOUND_OK\n");
       fprintf(stdout, "TRUE\n");
   }
  else
   {
       fprintf(stdout, "SEQHOUND_OK\n");
       fprintf(stdout, "FALSE\n");
   }
  MemFree(giptr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundExists(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr giptr = NULL;
  Int4 gival = 0;


  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"gi")) >= 0)
    {	
        pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
        giptr = StringSave(pcThis);
    }
  if (giptr == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
     }
  if(!StrToInt4 (giptr, &gival))
   {
       ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
       fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
       MemFree(giptr);
       return SLRI_FAIL;
   }
  if(SHoundExists(gival))
   {
       fprintf(stdout, "SEQHOUND_OK\n");
       fprintf(stdout, "TRUE\n");
   }
  else
   {
       fprintf(stdout, "SEQHOUND_OK\n");
       fprintf(stdout, "FALSE\n");
   }
  MemFree(giptr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHound3DExists(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr mmdbidptr = NULL;
  Int4 mmdbid = 0;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"mmdbid")) >= 0)
    {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       mmdbidptr = StringSave(pcThis);
    }
  if (mmdbidptr == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
     }
  if(!StrToInt4 (mmdbidptr, &mmdbid))
    {
       ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
       fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
       MemFree(mmdbidptr);
       return SLRI_FAIL;
   }
  if(SHound3DExists(mmdbid))
   {
      fprintf(stdout, "SEQHOUND_OK\n");
      fprintf(stdout, "TRUE\n");
   }
  else
   {
      fprintf(stdout, "SEQHOUND_OK\n");
      fprintf(stdout, "FALSE\n");
   }
  MemFree(mmdbidptr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGetLargestMMDBID(void)
{

   Int4 mmdbid = 0;
   printf("Content-type: text/html\r\n\r\n");
   mmdbid = SHoundGetLargestMMDBID();
   if (mmdbid > 0) {
       fprintf(stdout, "SEQHOUND_OK\n");
       fprintf(stdout, "%ld\n", (long) mmdbid);
       return SLRI_SUCCESS;
    }
   else {

       ErrPostEx(SEV_ERROR,0,0, "SeqHoundGetLargestMMDBID: invalid largest mmdb id.");

       fprintf(stdout, "SeqHoundGetLargestMMDBID: invalid largest mmdb id.\n");
       return SLRI_FAIL;
   }
}

SLRI_ERR SeqHound3DbWhat(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr mmdbidptr = NULL;
  Int4 mmdbid = 0;
  Int4 bwhat = 0;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"mmdbid")) >= 0)
    {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       mmdbidptr = StringSave(pcThis);
    }
  if (mmdbidptr == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
     }
  if(!StrToInt4 (mmdbidptr, &mmdbid))
   {
       ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
       fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
       MemFree(mmdbidptr);
       return SLRI_FAIL;
   }
  bwhat = SHound3DbWhat(mmdbid);
  fprintf(stdout, "SEQHOUND_OK\n");
  fprintf(stdout, "%ld\n", (long) bwhat);
  MemFree(mmdbidptr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGetBioseq(void)
{
   Int4 IndexArgs = -1;
   CharPtr pcThis = NULL;
   CharPtr giptr = NULL;
   Int4 gival = 0;
   AsnIoPtr aip = NULL;
   FILE *StdOot=NULL;
   BioseqPtr pbs = NULL;

   if ((IndexArgs = WWWFindName(ginfo,"gi")) >= 0)
    {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       giptr = StringSave(pcThis);
    }
   if (giptr == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
    }
   if(!StrToInt4 (giptr, &gival))
    {
       ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
       MemFree(giptr);
       return SLRI_FAIL;
    }
   pbs = SHoundGetBioseq(gival);
   if (pbs == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetBioseq returned NULL.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
       MemFree(giptr);
       return SLRI_FAIL;
    }
   /* return the bioseq to the client */
   StdOot=FileOpen("stdout", FILE_MODE);
   if (StdOot == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Unable to open a file.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Unable to open a file.\n");
       MemFree(giptr);
       BioseqFree(pbs);
       return SLRI_FAIL;
    }
   if((aip=AsnIoNew(ASN_MODE, StdOot, NULL,NULL,NULL)) == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Unable to open ASN.1 IO.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Unable to open ASN.1 IO.\n");
       MemFree(giptr);
       BioseqFree(pbs);
       FileClose(StdOot);
       return SLRI_FAIL;
    }
   printf("Content-type: application/x-octet-stream\n\n");
   fprintf(stdout, "SEQHOUND_OK\n");
   BioseqAsnWrite(pbs, aip, NULL);
   AsnIoClose(aip);
   FileClose(StdOot);
   BioseqFree(pbs);
   MemFree(giptr);
   return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGetBioseqList(void)
{
   Int4 IndexArgs = -1;
   CharPtr pcThis = NULL;
   ValNodePtr pvngi = NULL;
   ValNodePtr pvnbs = NULL;
   AsnIoPtr aip = NULL;
   FILE *StdOot=NULL;
   CharPtr pgi = NULL;

   if ((IndexArgs = WWWFindName(ginfo,"pgi")) >= 0)
    {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       pgi = StringSave(pcThis);
    }
   if (pgi == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
    }
   pvngi = Misc_CommaIntString2ValNodeList(pgi, ",");
   if(pvngi == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Misc_CommaIntString2ValNodeList failed.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Misc_CommaIntString2ValNodeList failed.\n");
       MemFree(pgi);
       return SLRI_FAIL;
    }
   pvnbs = SHoundGetBioseqList(pvngi);
   if(pvnbs == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetBioseqList returned NULL.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
       MemFree(pgi);
       ValNodeFree(pvngi);
       return SLRI_FAIL;
    }
   StdOot=FileOpen("stdout", FILE_MODE);
   if (StdOot == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Unable to open a file.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Unable to open a file.\n");
       MemFree(pgi);
       ValNodeFree(pvngi);
       FreeBioseqList(pvnbs);
       return SLRI_FAIL;
    }
   if((aip=AsnIoNew(ASN_MODE, StdOot, NULL,NULL,NULL)) == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Unable to open ASN.1 IO.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Unable to open ASN.1 IO.\n");
       MemFree(pgi);
       ValNodeFree(pvngi);
       FreeBioseqList(pvnbs);
       FileClose(StdOot);
       return SLRI_FAIL;
    }
   printf("Content-type: application/x-octet-stream\n\n");
   fprintf(stdout, "SEQHOUND_OK\n");
   SLRIValNodeAsnWrite((SLRIValNodePtr) pvnbs, aip, NULL);
   AsnIoClose(aip);
   FileClose(StdOot);
   FreeBioseqList(pvnbs);
   MemFree(pgi);
   ValNodeFree(pvngi);
   return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGetBioseqPL(void)
{
   Int4 IndexArgs = -1;
   CharPtr pcThis = NULL;
   CharPtr giptr = NULL;
   Int4 gival = 0;
   AsnIoPtr aip = NULL;
   BioseqPtr pbs = NULL;


   if ((IndexArgs = WWWFindName(ginfo,"gi")) >= 0)
    {	
        pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
        giptr = StringSave(pcThis);
    }
   if (giptr == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");

       return SLRI_FAIL;
    }
   if(!StrToInt4 (giptr, &gival))
    {
       ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
       MemFree(giptr);
       return SLRI_FAIL;
    }
   pbs = SHoundGetBioseq(gival);
   if (pbs == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetBioseq returned NULL.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
       MemFree(giptr);
       return SLRI_FAIL;
    }
   if((aip=AsnIoNew(ASNIO_TEXT_OUT, stdout, NULL,NULL,NULL)) == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Unable to open ASN.1 IO.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Unable to open ASN.1 IO.\n");
       MemFree(giptr);
       BioseqFree(pbs);
       return SLRI_FAIL;
    }
   printf("Content-type: text/html\r\n\r\n");
   fprintf(stdout, "SEQHOUND_OK\n");
   BioseqAsnWrite(pbs, aip, NULL);
   AsnIoClose(aip);
   BioseqFree(pbs);
   MemFree(giptr);
   return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGetBioseqListPL(void)
{
   Int4 IndexArgs = -1;
   CharPtr pcThis = NULL;
   ValNodePtr pvngi = NULL;
   ValNodePtr pvnbs = NULL;
   FILE *StdOot=NULL;
   CharPtr pgi = NULL;
   AsnIoPtr aip = NULL;

   if ((IndexArgs = WWWFindName(ginfo,"pgi")) >= 0)
    {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       pgi = StringSave(pcThis);
    }
   if (pgi == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
    }
   pvngi = Misc_CommaIntString2ValNodeList(pgi, ",");
   if(pvngi == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Misc_CommaIntString2ValNodeList failed.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Misc_CommaIntString2ValNodeList failed.\n");
       MemFree(pgi);
       return SLRI_FAIL;
    }
   pvnbs = SHoundGetBioseqList(pvngi);
   if(pvnbs == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetBioseqList returned NULL.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
       MemFree(pgi);
       ValNodeFree(pvngi);
       return SLRI_FAIL;
    }
   if((aip=AsnIoNew(ASNIO_TEXT_OUT, stdout, NULL,NULL,NULL)) == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Unable to open ASN.1 IO.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Unable to open ASN.1 IO.\n");
       MemFree(pgi);
       ValNodeFree(pvngi);
       FreeBioseqList(pvnbs);
       FileClose(StdOot);
       return SLRI_FAIL;
    }
   printf("Content-type: text/html\r\n\r\n");
   fprintf(stdout, "SEQHOUND_OK\n");
   SLRIValNodeAsnWrite((SLRIValNodePtr) pvnbs, aip, NULL);
   AsnIoClose(aip);
   FreeBioseqList(pvnbs);
   MemFree(pgi);
   ValNodeFree(pvngi);
   return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGetBioseqSet(void)
{
   Int4 IndexArgs = -1;
   CharPtr pcThis = NULL;
   CharPtr giptr = NULL;
   Int4 gival = 0;
   BioseqSetPtr pbss = NULL;
   FILE *StdOot=NULL;
   AsnIoPtr aip = NULL;

   if ((IndexArgs = WWWFindName(ginfo,"gi")) >= 0)
    {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       giptr = StringSave(pcThis);
    }
   if (giptr == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
    }
   if(!StrToInt4 (giptr, &gival))
    {
       ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
       MemFree(giptr);
       return SLRI_FAIL;
    }
   pbss = SHoundGetBioseqSet(gival);
   if (pbss == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetBioseqSet returned NULL.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
       MemFree(giptr);
       return SLRI_FAIL;
    }
   /* return the bioseq to the client */
   StdOot=FileOpen("stdout", FILE_MODE);
   if (StdOot == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Unable to open a file.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Unable to open a file.\n");
       MemFree(giptr);
       BioseqSetFree(pbss);
       return SLRI_FAIL;
    }
   if((aip=AsnIoNew(ASN_MODE, StdOot, NULL,NULL,NULL)) == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Unable to open ASN.1 IO.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Unable to open ASN.1 IO.\n");
       MemFree(giptr);
       BioseqSetFree(pbss);
       FileClose(StdOot);
       return SLRI_FAIL;
    }
   printf("Content-type: application/x-octet-stream\n\n");
   fprintf(stdout, "SEQHOUND_OK\n");
   BioseqSetAsnWrite(pbss, aip, NULL);
   AsnIoClose(aip);
   FileClose(StdOot);
   BioseqSetFree(pbss);
   MemFree(giptr);
   return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGetBioseqSetList(void)
{
   Int4 IndexArgs = -1;
   CharPtr pcThis = NULL;
   CharPtr pgi = NULL;
   ValNodePtr pvngi = NULL;
   ValNodePtr pvnbs = NULL;
   FILE *StdOot=NULL;
   AsnIoPtr aip = NULL;

   if ((IndexArgs = WWWFindName(ginfo,"pgi")) >= 0)
    {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       pgi = StringSave(pcThis);
    }
   if (pgi == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
    }
   pvngi = Misc_CommaIntString2ValNodeList(pgi, ",");
   if(pvngi == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Misc_CommaIntString2ValNodeList failed.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Misc_CommaIntString2ValNodeList failed.\n");
       MemFree(pgi);
       return SLRI_FAIL;
    }
   pvnbs = SHoundGetBioseqSetList(pvngi);
   if(pvnbs == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetBioseqList returned NULL.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
       MemFree(pgi);
       ValNodeFree(pvngi);
       return SLRI_FAIL;
    }
   StdOot=FileOpen("stdout", FILE_MODE);
   if (StdOot == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Unable to open a file.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Unable to open a file.\n");
       MemFree(pgi);
       ValNodeFree(pvngi);
       FreeBioseqSetList(pvnbs);
       return SLRI_FAIL;
    }
   if((aip=AsnIoNew(ASN_MODE, StdOot, NULL,NULL,NULL)) == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Unable to open ASN.1 IO.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Unable to open ASN.1 IO.\n");
       MemFree(pgi);
       ValNodeFree(pvngi);
       FreeBioseqSetList(pvnbs);
       FileClose(StdOot);
       return SLRI_FAIL;
    }
   printf("Content-type: application/x-octet-stream\n\n");
   fprintf(stdout, "SEQHOUND_OK\n");
   SLRIValNodeAsnWrite((SLRIValNodePtr) pvnbs, aip, NULL);
   AsnIoClose(aip);
   FileClose(StdOot);
   FreeBioseqSetList(pvnbs);
   MemFree(pgi);
   ValNodeFree(pvngi);
   return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGetBioseqSetPL(void)
{
   Int4 IndexArgs = -1;
   CharPtr pcThis = NULL;
   CharPtr giptr = NULL;
   AsnIoPtr aip = NULL;
   Int4 gival = 0;
   BioseqSetPtr pbss = NULL;

   if ((IndexArgs = WWWFindName(ginfo,"gi")) >= 0)
    {	
        pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
        giptr = StringSave(pcThis);
    }
   if (giptr == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
    }
   if(!StrToInt4 (giptr, &gival))
    {
       ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
       MemFree(giptr);
       return SLRI_FAIL;
    }
   pbss = SHoundGetBioseqSet(gival);
   if (pbss == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetBioseqSet returned NULL.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
       MemFree(giptr);
       return SLRI_FAIL;
    }
   /* return the bioseq to the client */
   if((aip=AsnIoNew(ASNIO_TEXT_OUT, stdout, NULL,NULL,NULL)) == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Unable to open ASN.1 IO.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Unable to open ASN.1 IO.\n");
       MemFree(giptr);
       BioseqSetFree(pbss);
       return SLRI_FAIL;
    }
   printf("Content-type: text/html\r\n\r\n");
   fprintf(stdout, "SEQHOUND_OK\n");
   BioseqSetAsnWrite(pbss, aip, NULL);
   AsnIoClose(aip);
   BioseqSetFree(pbss);
   MemFree(giptr);
   return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGetBioseqSetListPL(void)
{
   Int4 IndexArgs = -1;
   CharPtr pcThis = NULL;
   CharPtr pgi = NULL;
   ValNodePtr pvngi = NULL;
   ValNodePtr pvnbs = NULL;
   AsnIoPtr aip = NULL;

   if ((IndexArgs = WWWFindName(ginfo,"pgi")) >= 0)
    {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       pgi = StringSave(pcThis);
    }
   if (pgi == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
    }
   pvngi = Misc_CommaIntString2ValNodeList(pgi, ",");
   if(pvngi == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Misc_CommaIntString2ValNodeList failed.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Misc_CommaIntString2ValNodeList failed.\n");
       MemFree(pgi);
       return SLRI_FAIL;
    }
   pvnbs = SHoundGetBioseqSetList(pvngi);
   if(pvnbs == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetBioseqList returned NULL.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
       MemFree(pgi);
       ValNodeFree(pvngi);
       return SLRI_FAIL;
    }
   if((aip=AsnIoNew(ASNIO_TEXT_OUT, stdout, NULL,NULL,NULL)) == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Unable to open ASN.1 IO.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Unable to open ASN.1 IO.\n");
       MemFree(pgi);
       ValNodeFree(pvngi);
       FreeBioseqSetList(pvnbs);
       return SLRI_FAIL;
    }
   printf("Content-type: text/html\r\n\r\n");
   fprintf(stdout, "SEQHOUND_OK\n");
   SLRIValNodeAsnWrite((SLRIValNodePtr) pvnbs, aip, NULL);
   AsnIoClose(aip);
   FreeBioseqSetList(pvnbs);
   MemFree(pgi);
   ValNodeFree(pvngi);
   return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGetSeqEntry(void)
{
   Int4 IndexArgs = -1;
   CharPtr pcThis = NULL;
   CharPtr giptr = NULL;
   Int4 gival = 0;
   SeqEntryPtr pse = NULL;
   AsnIoPtr aip = NULL;
   FILE *StdOot=NULL;

   if ((IndexArgs = WWWFindName(ginfo,"gi")) >= 0)
    {	
        pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
        giptr = StringSave(pcThis);
    }
   if (giptr == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
    }
   if(!StrToInt4 (giptr, &gival))
    {
       ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
       MemFree(giptr);
       return SLRI_FAIL;
    }
   pse = SHoundGetSeqEntry(gival);
   if (pse == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetSeqEntry returned NULL.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
       MemFree(giptr);
       return SLRI_FAIL;
    }
   /* return the bioseq to the client */
   StdOot=FileOpen("stdout", FILE_MODE);
   if (StdOot == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Unable to open a file.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Unable to open a file.\n");
       MemFree(giptr);
       SeqEntryFree(pse);
       return SLRI_FAIL;
    }
   if((aip=AsnIoNew(ASN_MODE, StdOot, NULL,NULL,NULL)) == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Unable to open ASN.1 IO.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Unable to open ASN.1 IO.\n");
       MemFree(giptr);
       SeqEntryFree(pse);
       FileClose(StdOot);
       return SLRI_FAIL;
    }
   printf("Content-type: application/x-octet-stream\n\n");
   fprintf(stdout, "SEQHOUND_OK\n");
   SeqEntryAsnWrite(pse, aip, NULL);
   AsnIoClose(aip);
   FileClose(StdOot);
   SeqEntryFree(pse);
   MemFree(giptr);
   return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGetSeqEntryList(void)
{
   Int4 IndexArgs = -1;
   CharPtr pcThis = NULL;
   CharPtr pgi = NULL;
   ValNodePtr pvnbs = NULL;
   AsnIoPtr aip = NULL;
   FILE *StdOot=NULL;
   ValNodePtr pvngi = NULL;

   if ((IndexArgs = WWWFindName(ginfo,"pgi")) >= 0)
    {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       pgi = StringSave(pcThis);
    }
   if (pgi == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
    }
   pvngi = Misc_CommaIntString2ValNodeList(pgi, ",");
   if(pvngi == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Misc_CommaIntString2ValNodeList failed.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Misc_CommaIntString2ValNodeList failed.\n");
       MemFree(pgi);
       return SLRI_FAIL;
    }
   pvnbs = SHoundGetSeqEntryList(pvngi);
   if(pvnbs == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetSeqEntryList returned NULL.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
       MemFree(pgi);
       ValNodeFree(pvngi);
       return SLRI_FAIL;
    }
   StdOot=FileOpen("stdout", FILE_MODE);
   if (StdOot == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Unable to open a file.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Unable to open a file.\n");
       MemFree(pgi);
       ValNodeFree(pvngi);
       FreeSeqEntryList(pvnbs);
       return SLRI_FAIL;
    }
   if((aip=AsnIoNew(ASN_MODE, StdOot, NULL,NULL,NULL)) == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Unable to open ASN.1 IO.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Unable to open ASN.1 IO.\n");
       MemFree(pgi);
       ValNodeFree(pvngi);
       FreeSeqEntryList(pvnbs);
       FileClose(StdOot);
       return SLRI_FAIL;
    }
   printf("Content-type: application/x-octet-stream\n\n");
   fprintf(stdout, "SEQHOUND_OK\n");
   SLRIValNodeAsnWrite((SLRIValNodePtr) pvnbs, aip, NULL);
   AsnIoClose(aip);
   FileClose(StdOot);
   FreeSeqEntryList(pvnbs);
   MemFree(pgi);
   ValNodeFree(pvngi);
   return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGetSeqEntryPL(void)
{
   Int4 IndexArgs = -1;
   CharPtr pcThis = NULL;
   CharPtr giptr = NULL;
   Int4 gival = 0;
   SeqEntryPtr pse = NULL;
   AsnIoPtr aip = NULL;

   if ((IndexArgs = WWWFindName(ginfo,"gi")) >= 0)
    {	
        pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
        giptr = StringSave(pcThis);
    }
   if (giptr == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
    }
   if(!StrToInt4 (giptr, &gival))
    {
       ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
       MemFree(giptr);
       return SLRI_FAIL;
    }
   pse = SHoundGetSeqEntry(gival);
   if (pse == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetSeqEntry returned NULL.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
       MemFree(giptr);
       return SLRI_FAIL;
    }
   /* return the bioseq to the client */
   if((aip=AsnIoNew(ASNIO_TEXT_OUT, stdout, NULL,NULL,NULL)) == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Unable to open ASN.1 IO.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Unable to open ASN.1 IO.\n");
       MemFree(giptr);
       SeqEntryFree(pse);
       return SLRI_FAIL;
    }
   printf("Content-type: text/html\r\n\r\n");
   fprintf(stdout, "SEQHOUND_OK\n");
   SeqEntryAsnWrite(pse, aip, NULL);
   AsnIoClose(aip);
   SeqEntryFree(pse);
   MemFree(giptr);
   return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGetSeqEntryListPL(void)
{
   Int4 IndexArgs = -1;
   CharPtr pcThis = NULL;
   CharPtr pgi = NULL;
   ValNodePtr pvnbs = NULL;
   AsnIoPtr aip = NULL;
   ValNodePtr pvngi = NULL;

   if ((IndexArgs = WWWFindName(ginfo,"pgi")) >= 0)
    {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       pgi = StringSave(pcThis);
    }
   if (pgi == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
    }
   pvngi = Misc_CommaIntString2ValNodeList(pgi, ",");
   if(pvngi == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Misc_CommaIntString2ValNodeList failed.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Misc_CommaIntString2ValNodeList failed.\n");
       MemFree(pgi);
       return SLRI_FAIL;
    }
   pvnbs = SHoundGetSeqEntryList(pvngi);
   if(pvnbs == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetSeqEntryList returned NULL.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
       MemFree(pgi);
       ValNodeFree(pvngi);
       return SLRI_FAIL;
    }
   if((aip=AsnIoNew(ASNIO_TEXT_OUT, stdout, NULL,NULL,NULL)) == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Unable to open ASN.1 IO.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Unable to open ASN.1 IO.\n");
       MemFree(pgi);
       ValNodeFree(pvngi);
       FreeSeqEntryList(pvnbs);
       return SLRI_FAIL;
    }
   printf("Content-type: text/html\r\n\r\n");
   fprintf(stdout, "SEQHOUND_OK\n");
   SLRIValNodeAsnWrite((SLRIValNodePtr) pvnbs, aip, NULL);
   AsnIoClose(aip);
   FreeSeqEntryList(pvnbs);
   MemFree(pgi);
   ValNodeFree(pvngi);
   return SLRI_SUCCESS;
}

CharPtr NetEntrezGetFasta(Int4 gi)
{
  CONNECTOR connector;
  SConnNetInfo *net_info;
  THCC_Flags flags = 0;
  CONN conn;
  size_t n;
  Char blob[1000];
  EIO_Status status;
  CharPtr url = "http://www.ncbi.nlm.nih.gov/entrez/eutils/efetch.fcgi?db=protein&rettype=fasta&id=";
  CharPtr the_url = NULL;

  ByteStorePtr replyBS = BSNew(10);
  the_url = MemNew((StringLen(url) * sizeof(Char)) + 20);
  if(!the_url)
  {
     ErrPostEx(SEV_ERROR, 0, 0, "[NetEntrezGetFasta]: Could not allocate memory");
     return NULL;
  } 
  sprintf(the_url, "%s%ld", url, (long) gi);

  if(!(net_info = ConnNetInfo_Create(0)))
  {
    ErrPostEx(SEV_ERROR, 0, 0, "[NetEntrezGetFasta]: NetEntrez not working.");
    return NULL;
  }
  if(!(ConnNetInfo_ParseURL(net_info, the_url)))
  {
    ErrPostEx(SEV_ERROR, 0, 0, "[NetEntrezGetFasta]: NetEntrez not working.");
    return NULL;
  }
  if(!(connector = HTTP_CreateConnector(net_info, 0, flags)))
  {
    ErrPostEx(SEV_ERROR, 0, 0, "[NetEntrezGetFasta]: NetEntrez not working.");
    return NULL;
  }
  if(CONN_Create(connector, &conn) != eIO_Success)
  {
    ErrPostEx(SEV_ERROR, 0, 0, "[NetEntrezGetFasta]: NetEntrez not working.");
    return NULL;
  }
  for(;;){
    status = CONN_Read(conn, (void*)blob, (size_t)sizeof(blob), &n, eIO_Plain);
    if(status != eIO_Success) break;
    BSWrite(replyBS, (Nlm_VoidPtr)blob, (size_t)n);
    /*fwrite(blob, 1, n, stdout); */
  }
  CONN_Close(conn);
  return BSStringSave(replyBS);
}


SLRI_ERR SeqHoundGetFasta(void)
{
   Int4 IndexArgs = -1;
   CharPtr pcThis = NULL;
   CharPtr giptr = NULL;
   Int4 gival = 0;
   SLRIFastaPtr pfa = NULL;
   AsnIoPtr aip = NULL;


   if ((IndexArgs = WWWFindName(ginfo,"gi")) >= 0)
    {	
        pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
        giptr = StringSave(pcThis);
    }
   if (giptr == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
    }
   if(!StrToInt4 (giptr, &gival))
    {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(giptr);
      return SLRI_FAIL;
    }
   pfa = SHoundGetFasta(gival);
   if (pfa == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetFasta returned NULL.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
       MemFree(giptr);
       return SLRI_FAIL;
    }
   /*  does not work in binary....only in the text form...no idea why
   StdOot=FileOpen("stdout", FILE_MODE);
   if (StdOot == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Unable to open a file.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Unable to open a file.\n");
       SLRIFastaFree(pfa);
       MemFree(giptr);
       return SLRI_FAIL;
    }
   if((aip=AsnIoNew(ASN_MODE, StdOot, NULL,NULL,NULL)) == NULL)
    {
              ErrPostEx(SEV_ERROR,0,0, "Unable to open ASN.1 IO.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Unable to open ASN.1 IO.\n");
       MemFree(giptr);
       FileClose(StdOot);
       SLRIFastaFree(pfa);
       return SLRI_FAIL;
    }
   printf("Content-type: application/x-octet-stream\n\n");
   SLRIFastaAsnWrite(pfa, aip, NULL);
   AsnIoFlush(aip);
   AsnIoClose(aip);
   FileClose(StdOot);
   SLRIFastaFree(pfa);
   MemFree(giptr);
   */
   if((aip=AsnIoNew(ASNIO_TEXT_OUT, stdout, NULL,NULL,NULL)) == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Unable to open ASN.1 IO.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Unable to open ASN.1 IO.\n");
       MemFree(giptr);
       SLRIFastaFree(pfa);
       return SLRI_FAIL;
    }
   printf("Content-type: text/html\r\n\r\n");
   fprintf(stdout, "SEQHOUND_OK\n");
   SLRIFastaAsnWrite(pfa, aip, NULL);
   AsnIoClose(aip);
   SLRIFastaFree(pfa);
   MemFree(giptr);
   return SLRI_SUCCESS;

}

SLRI_ERR SeqHoundGetFastaList(void)
{
   Int4 IndexArgs = -1;
   CharPtr pcThis = NULL;
   CharPtr pgi = NULL;
   ValNodePtr pvngi = NULL;
   ValNodePtr pvnfa = NULL;
   AsnIoPtr aip = NULL;

   if ((IndexArgs = WWWFindName(ginfo,"pgi")) >= 0)
    {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       pgi = StringSave(pcThis);
    }
   if (pgi == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
    }
   pvngi = Misc_CommaIntString2ValNodeList(pgi, ",");
   if(pvngi == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Misc_CommaIntString2ValNodeList failed.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Misc_CommaIntString2ValNodeList failed.\n");
       MemFree(pgi);
       return SLRI_FAIL;
    }
   pvnfa = SHoundGetFastaList(pvngi);
   if(pvnfa == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetFastaList returned NULL.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
       MemFree(pgi);
       ValNodeFree(pvngi);
       return SLRI_FAIL;
    }
   /*  does not work....only in the text form...no idea why
   StdOot=FileOpen("stdout", FILE_MODE);
   if (StdOot == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Unable to open a file.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Unable to open a file.\n");
       MemFree(pgi);
       ValNodeFree(pvngi);
       FreeFastaList(pvnfa);
       return SLRI_FAIL;
    }
   if((aip=AsnIoNew(ASN_MODE, StdOot, NULL,NULL,NULL)) == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Unable to open ASN.1 IO.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Unable to open ASN.1 IO.\n");
       MemFree(pgi);
       ValNodeFree(pvngi);
       FreeFastaList(pvnfa);
       FileClose(StdOot);
       return SLRI_FAIL;
    }
   printf("Content-type: application/x-octet-stream\n\n");
   SLRIValNodeAsnWrite((SLRIValNodePtr) pvnfa, aip, NULL);
   AsnIoClose(aip);
   FileClose(StdOot);
   FreeFastaList(pvnfa);
   ValNodeFree(pvngi);
   MemFree(pgi);
   */

   if((aip=AsnIoNew(ASNIO_TEXT_OUT, stdout, NULL,NULL,NULL)) == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Unable to open ASN.1 IO.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Unable to open ASN.1 IO.\n");
       MemFree(pgi);
       ValNodeFree(pvngi);
       FreeFastaList(pvnfa);
       return SLRI_FAIL;
    }
   printf("Content-type: text/html\r\n\r\n");
   fprintf(stdout, "SEQHOUND_OK\n");
   SLRIValNodeAsnWrite((SLRIValNodePtr) pvnfa, aip, NULL);
   AsnIoClose(aip);
   FreeFastaList(pvnfa);
   MemFree(pgi);
   ValNodeFree(pvngi);
   return SLRI_SUCCESS;

}

SLRI_ERR SeqHoundGetFastaPL(void)
{
   Int4 IndexArgs = -1;
   CharPtr pcThis = NULL;
   CharPtr giptr = NULL;
   Int4 gival = 0;
   SLRIFastaPtr pfa = NULL;


   if ((IndexArgs = WWWFindName(ginfo,"gi")) >= 0)
    {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       giptr = StringSave(pcThis);
    }
   if (giptr == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");

       return SLRI_FAIL;
    }
   if(!StrToInt4 (giptr, &gival))
    {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");

      MemFree(giptr);
      return SLRI_FAIL;
    }
   pfa = SHoundGetFasta(gival);
   /* if( NULL == pfa && NetEntrezOn )
   {
      CharPtr p, ps;

      if((p = NetEntrezGetFasta(gival)) != NULL)
      {
         pfa = SLRIFastaNew();
         ps = strtok(p, "\n");
         pfa->defline = defline = (char*) MemNew(sizeof(Char) * (StringLen(ps) + 1));
         strcpy(pfa->defline, ++ps);        
         pfa->sequence = sequence = (char*) MemNew(1001 * sizeof(Char));
         while((ps = strtok(NULL, "\n")) != NULL)
            pfa->sequence = strcat(pfa->sequence, ps);
      }
      if(p) MemFree(p);
   } */
   if (pfa == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetFasta returned NULL.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
       MemFree(giptr);
       return SLRI_FAIL;
    }
   printf("Content-type: text/html\r\n\r\n");
   fprintf(stdout, "SEQHOUND_OK\n");
   if(pfa->defline != NULL) fprintf(stdout, ">%s\n", pfa->defline); /* 1st '>' is printed here */
   if(pfa->sequence != NULL) fprintf(stdout, "%s\n", pfa->sequence);
   SLRIFastaFree(pfa);
   MemFree(giptr);
   return SLRI_SUCCESS;

}

SLRI_ERR SeqHoundGetFastaListPL(void)
{
   Int4 IndexArgs = -1;
   CharPtr pcThis = NULL;
   CharPtr pgi = NULL;
   ValNodePtr pvngi = NULL;
   ValNodePtr pvnfa = NULL;
   SLRIFastaPtr pfa = NULL;

   if ((IndexArgs = WWWFindName(ginfo,"pgi")) >= 0)
    {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       pgi = StringSave(pcThis);
    }
   if (pgi == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
    }
   pvngi = Misc_CommaIntString2ValNodeList(pgi, ",");
   if(pvngi == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Misc_CommaIntString2ValNodeList failed.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Misc_CommaIntString2ValNodeList failed.\n");
       MemFree(pgi);
       return SLRI_FAIL;
    }
   pvnfa = SHoundGetFastaList(pvngi);
   /* accessing net entrez */
   /* if( NULL == pvnfa && NetEntrezOn )
   {
     CharPtr p, ps;
     ValNodePtr v;
     for( v = pvngi; v != NULL; v = v->next )
     { 
        if((p = NetEntrezGetFasta(v->data.intvalue)) != NULL)
        {
           pfa = SLRIFastaNew();
           pfa->sequence = sequence = (char*) MemNew(StringLen(p) * sizeof(Char));
           ps = strtok(p, "\n");
           pfa->defline = defline = (char*) MemNew(sizeof(char) * (StringLen(ps) + 1));

           if( NULL == sequence || NULL == defline )
           {
              ErrPostEx(SEV_ERROR, 0, 0, "[SHoundGetFastaListPL]: Allocation error.");
              printf("Content-type: text/html\r\n\r\n");
              fprintf(stdout, "SEQHOUND_ERROR Allocation error.");
              MemFree(pgi); ValNodeFree(pvngi); FreeFastaList(pvnfa);
              return SLRI_FAIL;
           }
           strcpy(pfa->defline, ++ps);
           while((ps = strtok(NULL, "\n")) != NULL)
              pfa->sequence = strcat(pfa->sequence, ps);
           ValNodeAddPointer(&pvnfa, 0, pfa);
        } 
     } 
   } */ 

   if(pvnfa == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetFastaList returned NULL.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
       MemFree(pgi);
       ValNodeFree(pvngi);
       return SLRI_FAIL;
    }

   printf("Content-type: text/html\r\n\r\n");
   fprintf(stdout, "SEQHOUND_OK\n");
  
   while(pvnfa != NULL)
   {
      pfa = pvnfa->data.ptrvalue;
      if(pfa->defline != NULL) fprintf(stdout, ">%s\n", pfa->defline);
      if(pfa->sequence != NULL) fprintf(stdout, "%s\n", pfa->sequence);
      pvnfa = pvnfa->next;
   }
   FreeFastaList(pvnfa);
   MemFree(pgi);
   ValNodeFree(pvngi);
   return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGet3D(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr mmdbidptr = NULL;
  Int4 mmdbid = 0;
  BiostrucPtr pbio = NULL;
  AsnIoPtr aip = NULL;
  FILE *StdOot=NULL;


  if ((IndexArgs = WWWFindName(ginfo,"mmdbid")) >= 0)
    {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      mmdbidptr = StringSave(pcThis);
    }
  if (mmdbidptr == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
     }
  if(!StrToInt4 (mmdbidptr, &mmdbid))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(mmdbidptr);
      return SLRI_FAIL;
   }

  pbio = SHoundGet3D(mmdbid);
  if(pbio == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "SHoundGet3D returned NULL.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
      MemFree(mmdbidptr);
      
      return SLRI_FAIL;
   }
  StdOot=FileOpen("stdout", FILE_MODE);
  if (StdOot == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Unable to open a file.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Unable to open a file.\n");
      MemFree(mmdbidptr);
      BiostrucFree(pbio);
      
      return SLRI_FAIL;
   }
  if((aip=AsnIoNew(ASN_MODE, StdOot, NULL,NULL,NULL)) == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Unable to open ASN.1 IO.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Unable to open ASN.1 IO.\n");
      MemFree(mmdbidptr);
      BiostrucFree(pbio);
      FileClose(StdOot);
      
      return SLRI_FAIL;
   }
  printf("Content-type: application/x-octet-stream\n\n");
  fprintf(stdout, "SEQHOUND_OK\n");
  BiostrucAsnWrite(pbio, aip, NULL);
  BiostrucFree(pbio);
  AsnIoClose(aip);
  FileClose(StdOot);
  MemFree(mmdbidptr);
  
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGetFixed3D(void)
{ Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr mmdbidptr = NULL,mdlLvlptr=NULL,maxModelsptr=NULL;
  Int4 mmdbid = 0,mdlLvl=0,maxModels=0;
  BiostrucPtr pbio = NULL;
  AsnIoPtr aip = NULL;
  FILE *StdOot=NULL;


  if ((IndexArgs = WWWFindName(ginfo,"mmdbid")) >= 0)
    {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      mmdbidptr = StringSave(pcThis);
    }
  if (mmdbidptr == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
     }
  if(!StrToInt4 (mmdbidptr, &mmdbid))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(mmdbidptr);
      return SLRI_FAIL;
   }

  if ((IndexArgs = WWWFindName(ginfo,"mdlLvl")) >= 0)
    {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      mdlLvlptr = StringSave(pcThis);
    }
  if (mdlLvlptr == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      MemFree(mmdbidptr);
      return SLRI_FAIL;
     }
  if(!StrToInt4 (mdlLvlptr, &mdlLvl))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(mdlLvlptr);
      MemFree(mmdbidptr);
      return SLRI_FAIL;
   }

  if ((IndexArgs = WWWFindName(ginfo,"maxModels")) >= 0)
    {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      maxModelsptr = StringSave(pcThis);
    }
  if (maxModelsptr == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      MemFree(mdlLvlptr);
      MemFree(mmdbidptr);
      return SLRI_FAIL;
     }
  if(!StrToInt4 (maxModelsptr, &maxModels))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(maxModelsptr);
      MemFree(mdlLvlptr);
      MemFree(mmdbidptr);
      return SLRI_FAIL;
   }
  pbio = SHoundGetFixed3D(mmdbid,mdlLvl,maxModels);
  if(pbio == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "SHoundGetFixed3D returned NULL.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
      MemFree(mmdbidptr); 
      MemFree(maxModelsptr);
      MemFree(mdlLvlptr);
      
      return SLRI_FAIL;
   }
  StdOot=FileOpen("stdout", FILE_MODE);
  if (StdOot == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Unable to open a file.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Unable to open a file.\n");
      MemFree(mmdbidptr);
      MemFree(maxModelsptr);
      MemFree(mdlLvlptr);
      BiostrucFree(pbio);
      
      return SLRI_FAIL;
   }
  if((aip=AsnIoNew(ASN_MODE, StdOot, NULL,NULL,NULL)) == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Unable to open ASN.1 IO.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Unable to open ASN.1 IO.\n");
      MemFree(mmdbidptr);
      MemFree(maxModelsptr);
      MemFree(mdlLvlptr);
      BiostrucFree(pbio);
      FileClose(StdOot);
      
      return SLRI_FAIL;
   }
  printf("Content-type: application/x-octet-stream\n\n");
  fprintf(stdout, "SEQHOUND_OK\n");
  BiostrucAsnWrite(pbio, aip, NULL);
  BiostrucFree(pbio);
  AsnIoClose(aip);
  FileClose(StdOot);
  MemFree(mmdbidptr);
  MemFree(mdlLvlptr);
  MemFree(maxModelsptr);
  
  return SLRI_SUCCESS;

}

SLRI_ERR SeqHoundGet3DEx(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr mmdbidptr = NULL,mdlLvlptr=NULL,maxModelsptr=NULL;
  Int4 mmdbid = 0,mdlLvl=0,maxModels=0;
  BiostrucPtr pbio = NULL;
  AsnIoPtr aip = NULL;
  FILE *StdOot=NULL;


  if ((IndexArgs = WWWFindName(ginfo,"mmdbid")) >= 0)
    {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      mmdbidptr = StringSave(pcThis);
    }
  if (mmdbidptr == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
     }
  if(!StrToInt4 (mmdbidptr, &mmdbid))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(mmdbidptr);
      return SLRI_FAIL;
   }

  if ((IndexArgs = WWWFindName(ginfo,"mdlLvl")) >= 0)
    {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      mdlLvlptr = StringSave(pcThis);
    }
  if (mdlLvlptr == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      MemFree(mmdbidptr);
      return SLRI_FAIL;
     }
  if(!StrToInt4 (mdlLvlptr, &mdlLvl))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(mdlLvlptr);
      MemFree(mmdbidptr);
      return SLRI_FAIL;
   }

  if ((IndexArgs = WWWFindName(ginfo,"maxModels")) >= 0)
    {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      maxModelsptr = StringSave(pcThis);
    }
  if (maxModelsptr == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      MemFree(mdlLvlptr);
      MemFree(mmdbidptr);
      return SLRI_FAIL;
     }
  if(!StrToInt4 (maxModelsptr, &maxModels))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(maxModelsptr);
      MemFree(mdlLvlptr);
      MemFree(mmdbidptr);
      return SLRI_FAIL;
   }
  pbio = SHoundGet3DEx(mmdbid,mdlLvl,maxModels);
  if(pbio == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "SHoundGet3DEx returned NULL.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
      MemFree(mmdbidptr); 
      MemFree(maxModelsptr);
      MemFree(mdlLvlptr);
      
      return SLRI_FAIL;
   }
  StdOot=FileOpen("stdout", FILE_MODE);
  if (StdOot == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Unable to open a file.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Unable to open a file.\n");
      MemFree(mmdbidptr);
      MemFree(maxModelsptr);
      MemFree(mdlLvlptr);
      BiostrucFree(pbio);
      
      return SLRI_FAIL;
   }
  if((aip=AsnIoNew(ASN_MODE, StdOot, NULL,NULL,NULL)) == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Unable to open ASN.1 IO.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Unable to open ASN.1 IO.\n");
      MemFree(mmdbidptr);
      MemFree(maxModelsptr);
      MemFree(mdlLvlptr);
      BiostrucFree(pbio);
      FileClose(StdOot);
      
      return SLRI_FAIL;
   }
  printf("Content-type: application/x-octet-stream\n\n");
  fprintf(stdout, "SEQHOUND_OK\n");
  BiostrucAsnWrite(pbio, aip, NULL);
  BiostrucFree(pbio);
  AsnIoClose(aip);
  FileClose(StdOot);
  MemFree(mmdbidptr);
  MemFree(mdlLvlptr);
  MemFree(maxModelsptr);
  
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGet3DPL(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr mmdbidptr = NULL;
  Int4 mmdbid = 0;
  BiostrucPtr pbio = NULL;
  AsnIoPtr aip = NULL;


  if ((IndexArgs = WWWFindName(ginfo,"mmdbid")) >= 0)
    {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       mmdbidptr = StringSave(pcThis);
    }
  if (mmdbidptr == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
     }
  if(!StrToInt4 (mmdbidptr, &mmdbid))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(mmdbidptr);
      return SLRI_FAIL;
   }
   
  pbio = SHoundGet3D(mmdbid);
  if(pbio == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "SHoundGet3D returned NULL.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
      MemFree(mmdbidptr);
      
      return SLRI_FAIL;
   }
  if((aip=AsnIoNew(ASNIO_TEXT_OUT, stdout, NULL,NULL,NULL)) == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Unable to open ASN.1 IO.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Unable to open ASN.1 IO.\n");
      MemFree(mmdbidptr);
      BiostrucFree(pbio);
      
      return SLRI_FAIL;
   }
  printf("Content-type: text/html\r\n\r\n");
  fprintf(stdout, "SEQHOUND_OK\n");
  BiostrucAsnWrite(pbio, aip, NULL);
  BiostrucFree(pbio);
  AsnIoClose(aip);
  MemFree(mmdbidptr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGet3DfromPdbId(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  BiostrucPtr pbio = NULL;
  AsnIoPtr aip = NULL;
  FILE *StdOot=NULL;
  CharPtr pdbidptr = NULL;

  if ((IndexArgs = WWWFindName(ginfo,"pdbid")) >= 0)
    {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pdbidptr = StringSave(pcThis);
    }
  if (pdbidptr == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
     }
  pbio = SHoundGet3DfromPdbId(pdbidptr);
  if(pbio == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "SHoundGet3D returned NULL.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
      MemFree(pdbidptr);
      
      return SLRI_FAIL;
   }
  StdOot=FileOpen("stdout", FILE_MODE);
  if (StdOot == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Unable to open a file.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Unable to open a file.\n");
      MemFree(pdbidptr);
      BiostrucFree(pbio);
      
      return SLRI_FAIL;
   }
  if((aip=AsnIoNew(ASN_MODE, StdOot, NULL,NULL,NULL)) == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Unable to open ASN.1 IO.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Unable to open ASN.1 IO.\n");
      MemFree(pdbidptr);
      BiostrucFree(pbio);
      FileClose(StdOot);
      
      return SLRI_FAIL;
   }
  printf("Content-type: application/x-octet-stream\n\n");
  fprintf(stdout, "SEQHOUND_OK\n");
  BiostrucAsnWrite(pbio, aip, NULL);
  BiostrucFree(pbio);
  AsnIoClose(aip);
  FileClose(StdOot);
  MemFree(pdbidptr);
  
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGet3DfromPdbIdEx(void)
{
  Int4 IndexArgs = -1;
  Int4 mdlLvl=0,maxModels=0;
  CharPtr pcThis = NULL;
  BiostrucPtr pbio = NULL;
  AsnIoPtr aip = NULL;
  FILE *StdOot=NULL;
  CharPtr pdbidptr = NULL,mdlLvlptr=NULL,maxModelsptr=NULL;

  if ((IndexArgs = WWWFindName(ginfo,"pdbid")) >= 0)
    {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pdbidptr = StringSave(pcThis);
    }
  if (pdbidptr == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
     }

  if ((IndexArgs = WWWFindName(ginfo,"mdlLvl")) >= 0)
    {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      mdlLvlptr = StringSave(pcThis);
    }
  if (mdlLvlptr == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      MemFree(pdbidptr);
      return SLRI_FAIL;
     }
  if(!StrToInt4 (mdlLvlptr, &mdlLvl))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(mdlLvlptr);
      MemFree(pdbidptr);
      return SLRI_FAIL;
   }

  if ((IndexArgs = WWWFindName(ginfo,"maxModels")) >= 0)
    {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      maxModelsptr = StringSave(pcThis);
    }
  if (maxModelsptr == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      MemFree(mdlLvlptr);
      MemFree(pdbidptr);
      return SLRI_FAIL;
     }
  if(!StrToInt4 (maxModelsptr, &maxModels))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(maxModelsptr);
      MemFree(mdlLvlptr);
      MemFree(pdbidptr);
      return SLRI_FAIL;
   }
  pbio = SHoundGet3DfromPdbIdEx(pdbidptr,mdlLvl,maxModels);
  if(pbio == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "SHoundGet3D returned NULL.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
      MemFree(pdbidptr);
      MemFree(maxModelsptr);
      MemFree(mdlLvlptr); 
      
      return SLRI_FAIL;
   }
  StdOot=FileOpen("stdout", FILE_MODE);
  if (StdOot == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Unable to open a file.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Unable to open a file.\n");
      MemFree(pdbidptr);
      BiostrucFree(pbio);
      
      return SLRI_FAIL;
   }
  if((aip=AsnIoNew(ASN_MODE, StdOot, NULL,NULL,NULL)) == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Unable to open ASN.1 IO.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Unable to open ASN.1 IO.\n");
      MemFree(pdbidptr);
      MemFree(maxModelsptr);
      MemFree(mdlLvlptr);
      BiostrucFree(pbio);
      FileClose(StdOot);
      
      return SLRI_FAIL;
   }
  printf("Content-type: application/x-octet-stream\n\n");
  fprintf(stdout, "SEQHOUND_OK\n");
  BiostrucAsnWrite(pbio, aip, NULL);
  BiostrucFree(pbio);
  AsnIoClose(aip);
  FileClose(StdOot);
  MemFree(pdbidptr);
  MemFree(mdlLvlptr);
  MemFree(maxModelsptr);
  
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGet3DfromPdbIdPL(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  BiostrucPtr pbio = NULL;
  AsnIoPtr aip = NULL;
  FILE *StdOot=NULL;
  CharPtr pdbidptr = NULL;

  if ((IndexArgs = WWWFindName(ginfo,"pdbid")) >= 0)
    {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pdbidptr = StringSave(pcThis);
    }
  if (pdbidptr == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
     }
  pbio = SHoundGet3DfromPdbId(pdbidptr);
  if(pbio == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "SHoundGet3D returned NULL.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
      MemFree(pdbidptr);
      
      return SLRI_FAIL;
   }
  if((aip=AsnIoNew(ASNIO_TEXT_OUT, stdout, NULL,NULL,NULL)) == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Unable to open ASN.1 IO.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Unable to open ASN.1 IO.\n");
      MemFree(pdbidptr);
      BiostrucFree(pbio);
      
      return SLRI_FAIL;
   }
  printf("Content-type: text/html\r\n\r\n");
  fprintf(stdout, "SEQHOUND_OK\n");
  BiostrucAsnWrite(pbio, aip, NULL);
  BiostrucFree(pbio);
  AsnIoClose(aip);
  FileClose(StdOot);
  MemFree(pdbidptr);
  
  return SLRI_SUCCESS;
}


SLRI_ERR SeqHoundPDBFromMMDB(void)
{
  Int4 IndexArgs = -1;
  Int4 mmdbid = 0;
  CharPtr pcThis = NULL;
  CharPtr mmdbptr = NULL;
  CharPtr pdbid= NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"mmdbid")) >= 0)
    {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       mmdbptr = StringSave(pcThis);
    }
  if (mmdbptr == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, " SeqHoundPDBFromMMDB: Failed to get parameter value.");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
     }
  if(!StrToInt4 (mmdbptr, &mmdbid))
   {
       ErrPostEx(SEV_ERROR,0,0, "SeqHoundPDBFromMMDB: Invalid function parameter.");
       fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
       MemFree(mmdbptr);
       return SLRI_FAIL;
   }
  pdbid = SHoundPDBFromMMDB(mmdbid);
  fprintf(stdout, "SEQHOUND_OK\n");
  fprintf(stdout, "%s\n", IsNullStr(pdbid));
  MemFree(mmdbptr);
  MemFree(pdbid);
  
  return SLRI_SUCCESS;
}


SLRI_ERR SeqHoundGiFrom3D(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pmmdbid = NULL;
  CharPtr pprotonly = NULL;
  Int4 mmdbidval = 0;
  CharPtr pstr = NULL;
  ValNodePtr pvngi = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"mmdbid")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pmmdbid = StringSave(pcThis);
   }
  if (pmmdbid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  if ((IndexArgs = WWWFindName(ginfo,"ProteinOnly")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pprotonly = StringSave(pcThis);
   }
  if (pprotonly == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      MemFree(pmmdbid);
      return SLRI_FAIL;
   }
  if(!StrToInt4 (pmmdbid, &mmdbidval))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(pprotonly);
      MemFree(pmmdbid);
      return SLRI_FAIL;
   }
  if(strcmp(pprotonly, "TRUE") == 0)
      pvngi = SHoundGiFrom3D(mmdbidval, TRUE);
  else if (strcmp(pprotonly, "FALSE") == 0)
      pvngi = SHoundGiFrom3D(mmdbidval, FALSE);
  else
    {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(pprotonly);
      MemFree(pmmdbid);
      return SLRI_FAIL;
    }
  pstr = Misc_SaveValNodeListAsString(pvngi);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(pmmdbid);
  MemFree(pprotonly);
  ValNodeFree(pvngi);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGiFrom3DList(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pmmdbid = NULL;
  CharPtr pprotonly = NULL;
  ValNodePtr pvnmmdbid = NULL;
  ValNodePtr pvngi = NULL;
  CharPtr pstr = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"pmmdbid")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pmmdbid = StringSave(pcThis);
   }
  if (pmmdbid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");

      return SLRI_FAIL;
   }
  if ((IndexArgs = WWWFindName(ginfo,"ProteinOnly")) >= 0)
   {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       pprotonly = StringSave(pcThis);
   }
  if (pprotonly == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      MemFree(pmmdbid);
      return SLRI_FAIL;
   }
  pvnmmdbid = Misc_CommaIntString2ValNodeList(pmmdbid, ",");
  if(pvnmmdbid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Misc_CommaIntString2ValNodeList failed.");
      fprintf(stdout, "SEQHOUND_ERROR ValNode conversion error.\n");
      MemFree(pmmdbid);
      MemFree(pprotonly);
      return SLRI_FAIL;
   }
  if(strcmp(pprotonly, "TRUE") == 0)
      pvngi = SHoundGiFrom3DList(pvnmmdbid, TRUE);
  else if (strcmp(pprotonly, "FALSE") == 0)
      pvngi = SHoundGiFrom3DList(pvnmmdbid, FALSE);
  else
    {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(pmmdbid);
      MemFree(pprotonly);
      return SLRI_FAIL;
    }
  pstr = Misc_SaveValNodeListAsString(pvngi);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(pmmdbid);
  MemFree(pprotonly);
  ValNodeFree(pvnmmdbid);
  ValNodeFree(pvngi);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGiFromPDBchain(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pcode = NULL;
  CharPtr pchain = NULL;
  Int4 gival = 0;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"pdbcode")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pcode = StringSave(pcThis);
   }
  if ((pcode == NULL) || (strlen(pcode)==0))
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");

      return SLRI_FAIL;
   }
  if ((IndexArgs = WWWFindName(ginfo,"chain")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pchain = StringSave(pcThis);
   }
  if (pchain == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      MemFree(pcode);
      return SLRI_FAIL;
   }
  if(strlen(pchain) == 0)
    gival = SHoundGiFromPDBchain(pcode, NULL);
  else
    gival = SHoundGiFromPDBchain(pcode, pchain);
  fprintf(stdout, "SEQHOUND_OK\n");
  fprintf(stdout, "%ld\n", (long) gival);
  MemFree(pcode);
  MemFree(pchain);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHound3DFromGi(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pgi = NULL;
  Int4 gival = 0;
  Int4 mmdbid = 0;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"gi")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pgi = StringSave(pcThis);
   }
  if (pgi == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  if(!StrToInt4 (pgi, &gival))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(pgi);
      return SLRI_FAIL;
   }
  mmdbid = SHound3DFromGi(gival);
  fprintf(stdout, "SEQHOUND_OK\n");
  fprintf(stdout, "%ld\n", (long) mmdbid);
  MemFree(pgi);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHound3DFromGiList(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pgi = NULL;
  ValNodePtr pvngi = NULL;
  ValNodePtr pvnmmdbid = NULL;
  CharPtr pstr = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"pgi")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pgi = StringSave(pcThis);
   }
  if (pgi == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  pvngi = Misc_CommaIntString2ValNodeList(pgi, ",");
  if(pvngi == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Misc_CommaIntString2ValNodeList failed.");
      fprintf(stdout, "SEQHOUND_ERROR ValNode conversion error.\n");
      MemFree(pgi);
      return SLRI_FAIL;
   }
  pvnmmdbid = SHound3DFromGiList(pvngi);
  pstr = Misc_SaveValNodeListAsString(pvnmmdbid);
  fprintf(stdout, "SEQHOUND_OK\n");
  if (pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf(stdout, "(null)\n");
  MemFree(pgi);
  ValNodeFree(pvnmmdbid);
  ValNodeFree(pvngi);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundDNAFromProtein(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pgi = NULL;
  Int4 gival = 0;
  Int4 gin = 0;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"gi")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pgi = StringSave(pcThis);
   }
  if (pgi == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  if(!StrToInt4 (pgi, &gival))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(pgi);
      return SLRI_FAIL;
   }
  gin = SHoundDNAFromProtein(gival);
  fprintf(stdout, "SEQHOUND_OK\n");
  fprintf(stdout, "%ld\n", (long) gin);
  MemFree(pgi);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundDNAFromProteinList(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pgi = NULL;
  ValNodePtr pvngi = NULL;
  ValNodePtr pvngin = NULL;
  CharPtr pstr = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"pgi")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pgi = StringSave(pcThis);
   }
  if (pgi == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  pvngi = Misc_CommaIntString2ValNodeList(pgi, ",");
  if(pvngi == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Misc_CommaIntString2ValNodeList failed.");
      fprintf(stdout, "SEQHOUND_ERROR ValNode conversion error.\n");
      MemFree(pgi);
      return SLRI_FAIL;
   }
  pvngin = SHoundDNAFromProteinList(pvngi);
  pstr = Misc_SaveValNodeListAsString(pvngin);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(pgi);
  ValNodeFree(pvngin);
  ValNodeFree(pvngi);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundProteinFromDNA(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pgi = NULL;
  Int4 gival = 0;
  ValNodePtr pvngia = NULL;
  CharPtr pstr = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"gi")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pgi = StringSave(pcThis);
   }
  if (pgi == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  if(!StrToInt4 (pgi, &gival))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(pgi);
      return SLRI_FAIL;
   }
  pvngia = SHoundProteinFromDNA(gival);
  pstr = Misc_SaveValNodeListAsString(pvngia);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(pgi);
  ValNodeFree(pvngia);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundProteinFromDNAList(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pgi = NULL;
  ValNodePtr pvngia = NULL;
  ValNodePtr pvngi = NULL;
  CharPtr pstr = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"pgi")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pgi = StringSave(pcThis);
   }
  if (pgi == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  pvngi = Misc_CommaIntString2ValNodeList(pgi, ",");
  if(pvngi == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Misc_CommaIntString2ValNodeList failed.");
      fprintf(stdout, "SEQHOUND_ERROR ValNode conversion error.\n");
      MemFree(pgi);
      return SLRI_FAIL;
   }
  pvngia = SHoundProteinFromDNAList(pvngi);
  pstr = Misc_SaveValNodeListAsString(pvngia);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(pgi);
  ValNodeFree(pvngia);
  ValNodeFree(pvngi);
  MemFree(pstr);
  return SLRI_SUCCESS;
}


/*Retrieves the pmids or muids given a Gi and the reference id queried.
 *if want PubMed ID, set 'ReferenceType'==1, if want MedLine ID, set 
 *'ReferenceType'=2.
 *This function deprecates SeqhoundMuidFromGi
 */
SLRI_ERR SeqHoundGetReferenceIDFromGi(void)
{
  Int4 argshere[2];
  CharPtr pgi = NULL, preftype = NULL;
  ValNodePtr pvnref=NULL;
  CharPtr pstr = NULL;
  Int4 gival = 0, reftypeval=0;
  Boolean bSuccess=FALSE;
  
  printf("Content-type: text/html\r\n\r\n");
  if( ((argshere[0] = WWWFindName(ginfo, "gi")) >0)&&
      ((argshere[1] = WWWFindName(ginfo, "referencetype")) >0))
   {
    if(  ((pgi = StringSave(WWWGetValueByIndex(ginfo, argshere[0]))) !=NULL)
       &&((preftype= StringSave(WWWGetValueByIndex(ginfo, argshere[1])))!=NULL))
     {
      
      if (StrToInt4 (pgi, &gival) >0 && StrToInt4(preftype, &reftypeval) >0)
      {
       pvnref=SHoundGetReferenceIDFromGi(gival, reftypeval);
       if (pvnref!=NULL)
       {
         pstr=Misc_SaveValNodeListAsString(pvnref);
         fprintf(stdout, "SEQHOUND_OK\n");
         if (pstr!=NULL) fprintf(stdout, "%s\n", pstr);
         else fprintf(stdout, "(null)\n");
         MemFree(pstr);
         bSuccess =TRUE;
       }
       else{
         ErrPostEx(SEV_ERROR, 0, 0, "SHoundGetReferenceIDFromGi: vnpref is NULL.");
         fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
       }
     }else{
       ErrPostEx(SEV_ERROR, 0, 0, "SHoundGetReferenceIDFromGi:Invalid function parameter.");
       fprintf(stdout," SEQHOUND_ERROR SHoundGetReferenceIDFromGi invalid function parameter.\n");
      }
   }else{
    ErrPostEx(SEV_ERROR,0,0,"SHoundGetReferenceIDFromGi:failed to get parameter value.");
    fprintf(stdout, "SEQHOUND_ERROR SHoundGetReferenceIDFromGi failed ot get parameter value.\n");
    }
 }else{
  ErrPostEx(SEV_ERROR,0,0,"SHoundGetReferenceIDFromGi:failed to get parameter values.");
  fprintf(stdout, "SEQHOUND_ERROR SHoundGetReferenceIDFromGi: failed to get parameter values.\n");
 }
 if(pvnref) ValNodeFree(pvnref);
 if(pgi) MemFree(pgi);
 if(preftype) MemFree(preftype);

 if (!bSuccess){
  ErrPostEx(SEV_ERROR,0,0,"SHoundGetReferenceIDFromGi failed.");
  return SLRI_FAIL;
 }   
 return SLRI_SUCCESS;
}


SLRI_ERR SeqHoundMuidFromGi(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pgi = NULL;
  ValNodePtr pvnmuid = NULL;
  CharPtr pstr = NULL;
  Int4 gival = 0;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"gi")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pgi = StringSave(pcThis);
   }
  if (pgi == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  if(!StrToInt4 (pgi, &gival))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(pgi);
      return SLRI_FAIL;
   }
  pvnmuid = SHoundMuidFromGi(gival);
  pstr = Misc_SaveValNodeListAsString(pvnmuid);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(pgi);
  ValNodeFree(pvnmuid);
  MemFree(pstr);
  return SLRI_SUCCESS;
}


/* Retrieves the pmids or muids given a gi list and the type of 
 * reference identifiers you want. Set ReferenceType to 1 if you want
 * PubMed ID, set ReferenceType to 2 if you want MedLine ID.
 *
 * The return value will not contain duplicates.
 *
 * This function deprecates SHoundMuidFromGiList */

SLRI_ERR SeqHoundGetReferenceIDFromGiList(void)
{
  Int4 argshere[2];
  CharPtr pgi = NULL, preftype = NULL;
  ValNodePtr pvnref=NULL;
  ValNodePtr pvngilist=NULL;
  CharPtr pstr = NULL;
  Int4  reftypeval=0;
  
  printf("Content-type: text/html\r\n\r\n");
  argshere[0] = WWWFindName(ginfo, "gilist");
  argshere[1] = WWWFindName(ginfo, "reftype");

  if (argshere[0] >=0 && argshere[1] >=0)
  {
    pgi = StringSave(WWWGetValueByIndex(ginfo, argshere[0]));
    preftype= StringSave(WWWGetValueByIndex(ginfo, argshere[1]));
  }
  if (pgi == NULL || preftype == NULL)
  {
    ErrPostEx(SEV_ERROR,0,0,"SHoundGetReferenceIDFromGiList:failed to get parameter value.");
    fprintf(stdout, "SEQHOUND_ERROR SHoundGetReferenceIDFromGiList failed ot get parameter value.\n");
    if (pgi) MemFree(pgi);
	if (preftype) MemFree(preftype);
	return SLRI_FAIL; 
  }
  pvngilist = Misc_CommaIntString2ValNodeList(pgi, ",");
  if (pvngilist ==NULL )
  {
    ErrPostEx(SEV_ERROR, 0, 0, "[SHoundGetReferenceIDFromGiList]:Misc_CommaIntString2ValNodeList failed.");
    fprintf(stdout," SEQHOUND_ERROR SHoundGetReferenceIDFromGiList: SEQHOUND_ERROR Misc_CommaIntString2ValNodeList failed.\n");
    MemFree(pgi);
    MemFree(preftype);
    return SLRI_FAIL;
  }
  if (!StrToInt4(preftype, &reftypeval))
  {
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundGetReferenceIDFromGiList:StrToInt4 failed.");
    fprintf(stdout," SEQHOUND_ERROR SHoundGetReferenceIDFromGiList: SEQHOUND_ERROR StrToInt4 failed.\n");
    MemFree(preftype);
    MemFree(pgi);
    ValNodeFree(pvngilist);
    return SLRI_FAIL;
  }
  pvnref=SHoundGetReferenceIDFromGiList(pvngilist, (Int2)reftypeval);
  if (pvnref==NULL)
  {
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundGetReferenceIDFromGiList return NULL.");
  }
  pstr=Misc_SaveValNodeListAsString(pvnref);
  fprintf(stdout, "SEQHOUND_OK\n");
  if (pstr!=NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf(stdout, "(null)\n");
 
  if (pstr) MemFree(pstr);
  if (pgi) MemFree(pgi);
  if (pvnref) ValNodeFree(pvnref);
  if (pvngilist) ValNodeFree(pvngilist);
  if (preftype) MemFree(preftype);
 
  return SLRI_SUCCESS;
}




SLRI_ERR SeqHoundMuidFromGiList(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pgi = NULL;
  ValNodePtr pvngi = NULL;
  ValNodePtr pvnmuid = NULL;
  CharPtr pstr = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"pgi")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pgi = StringSave(pcThis);
   }
  if (pgi == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  pvngi = Misc_CommaIntString2ValNodeList(pgi, ",");
  if(pvngi == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Misc_CommaIntString2ValNodeList failed.");
      fprintf(stdout, "SEQHOUND_ERROR ValNode conversion error.\n");
      MemFree(pgi);
      return SLRI_FAIL;
   }
  pvnmuid = SHoundMuidFromGiList(pvngi);
  pstr = Misc_SaveValNodeListAsString(pvnmuid);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(pgi);
  ValNodeFree(pvnmuid);
  ValNodeFree(pvngi);
  MemFree(pstr);
  return SLRI_SUCCESS;
}



/* Retrieves the gis associated with a publication id 'ReferenceID'.
 * If 'ReferenceID' is a pubmed publication, set 'ReferenceType' == 1,
 * if 'ReferenceID' is a medline publication, set 'ReferenceType' == 2, 
 *
 * A publication may contain many gis, eg a high throughput publication.
 *
 * The return list may contain duplicates.
 */
SLRI_ERR SeqHoundGiFromReferenceID(void)
{
  Int4 argshere[2];
  CharPtr prefid = NULL, preftype = NULL;
  ValNodePtr pvngi=NULL;
  CharPtr pstr = NULL;
  Int4 refidval = 0, reftypeval=0;
  Boolean bSuccess=FALSE;
  
  printf("Content-type: text/html\r\n\r\n");
  if( ((argshere[0] = WWWFindName(ginfo, "referenceid")) >0)&&
      ((argshere[1] = WWWFindName(ginfo, "referencetype")) >0))
   {
    if(  ((prefid = StringSave(WWWGetValueByIndex(ginfo, argshere[0]))) !=NULL)
       &&((preftype= StringSave(WWWGetValueByIndex(ginfo, argshere[1])))!=NULL))
     {
      
      if (StrToInt4 (prefid, &refidval) >0 && StrToInt4(preftype, &reftypeval) >0)
      {
       pvngi=SHoundGiFromReferenceID(refidval, reftypeval);
       if (pvngi!=NULL)
       {
         pstr=Misc_SaveValNodeListAsString(pvngi);
         fprintf(stdout, "SEQHOUND_OK\n");
         if (pstr!=NULL) fprintf(stdout, "%s\n", pstr);
         else fprintf(stdout, "(null)\n");
         MemFree(pstr);
         bSuccess =TRUE;
       }
       else{
         ErrPostEx(SEV_ERROR, 0, 0, "SHoundGiFromReferenceID: vnpgi is NULL.");
         fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
       }
     }else{
       ErrPostEx(SEV_ERROR, 0, 0, "SHoundGiFromReferenceID:Invalid function parameter.");
       fprintf(stdout," SEQHOUND_ERROR SHoundGiFromReferenceID invalid function parameter.\n");
      }
   }else{
    ErrPostEx(SEV_ERROR,0,0,"SHoundGiFromReferenceID:failed to get parameter value.");
    fprintf(stdout, "SEQHOUND_ERROR SHoundGiFromReferenceID failed ot get parameter value.\n");
    }
 }else{
  ErrPostEx(SEV_ERROR,0,0,"SHoundGiFromReferenceID:failed to get parameter values.");
  fprintf(stdout, "SEQHOUND_ERROR SHoundGiFromReferenceID: failed to get parameter values.\n");
 }
 if(pvngi) ValNodeFree(pvngi);
 if(prefid) MemFree(prefid);
 if(preftype) MemFree(preftype);

 if (!bSuccess){
  ErrPostEx(SEV_ERROR,0,0,"SHoundGiFromReferenceID failed.");
  return SLRI_FAIL;
 }   
 return SLRI_SUCCESS;
}


/* Retrieves all the gi's associated with the references found in 'ReferenceList' of
 * 'ReferenceType', where 'ReferenceType' == 1 if a PubMed publication, or 
 * 'ReferenceType' == 2 if a Medline publication.
 *
 * Return value may contain duplicates.
 *
 * Return value may be very long for some publication, eg for a high through put publication.
 * */
SLRI_ERR SeqHoundGiFromReferenceList(void)
{
 Int4 argshere[2];
  CharPtr prefid = NULL, preftype = NULL;
  ValNodePtr pvnreflist=NULL;
  ValNodePtr pvngilist=NULL;
  CharPtr pstr = NULL;
  Int4  reftypeval=0;
  
  printf("Content-type: text/html\r\n\r\n");
  
  argshere[0] =  WWWFindName(ginfo, "reflist");
  argshere[1] =  WWWFindName(ginfo, "reftype");
  if( argshere[0] >=0 && argshere[1] >=0 )
  {
    prefid = StringSave(WWWGetValueByIndex(ginfo, argshere[0]));
    preftype= StringSave(WWWGetValueByIndex(ginfo, argshere[1]));
  }
  if (prefid == NULL || preftype == NULL)
  {
    ErrPostEx(SEV_ERROR,0,0,"SHoundGiFromReferenceList:failed to get parameter value.");
    fprintf(stdout, "SEQHOUND_ERROR SHoundGiFromReferenceList failed ot get parameter value.\n");
	if (prefid) MemFree(prefid);
	if (preftype) MemFree(preftype);
    return SLRI_FAIL; 
  }
 
  pvnreflist = Misc_CommaIntString2ValNodeList(prefid, ",");
  if (pvnreflist ==NULL )
  {
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundGiFromReferenceList:Misc_CommaIntString2ValNodeList failed.");
    fprintf(stdout," SEQHOUND_ERROR SHoundGiFromReferenceList: SEQHOUND_ERROR Misc_CommaIntString2ValNodeList failed.\n");
    MemFree(prefid);
    MemFree(preftype);
    return SLRI_FAIL;
  }
  if (!StrToInt4(preftype, &reftypeval))
  {
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundGiFromReferenceList:StrToInt4 failed.");
    fprintf(stdout," SEQHOUND_ERROR SHoundGiFromReferenceList: SEQHOUND_ERROR StrToInt4 failed.\n");
    MemFree(preftype);
    MemFree(prefid);
    ValNodeFree(pvnreflist);
    return SLRI_FAIL;
  }
  pvngilist=SHoundGiFromReferenceList(pvnreflist, (Int2)reftypeval);
  if (pvngilist==NULL)
  {
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundGiFromReferenceList return NULL.");
  }
  pstr=Misc_SaveValNodeListAsString(pvngilist);
  fprintf(stdout, "SEQHOUND_OK\n");
  if (pstr!=NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf(stdout, "(null)\n");
 
  if (pstr) MemFree(pstr);
  if (prefid) MemFree(prefid);
  if (pvnreflist) ValNodeFree(pvnreflist);
  if (pvngilist) ValNodeFree(pvngilist);
  if (preftype) MemFree(preftype);
 
  return SLRI_SUCCESS;
}


SLRI_ERR SeqHoundMuidFrom3D(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pmmdbid = NULL;
  Int4 mmdbidval = 0;
  ValNodePtr pvnmuid = NULL;
  CharPtr pstr = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"mmdbid")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pmmdbid = StringSave(pcThis);
   }
  if (pmmdbid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  if(!StrToInt4 (pmmdbid, &mmdbidval))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(pmmdbid);
      return SLRI_FAIL;
   }
  pvnmuid = SHoundMuidFrom3D(mmdbidval);
  pstr = Misc_SaveValNodeListAsString(pvnmuid);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(pmmdbid);
  ValNodeFree(pvnmuid);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundMuidFrom3DList(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pmmdbid = NULL;
  ValNodePtr pvnmmdbid = NULL;
  ValNodePtr pvnmuid = NULL;
  CharPtr pstr = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"pmmdbid")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pmmdbid = StringSave(pcThis);
   }
  if (pmmdbid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  pvnmmdbid = Misc_CommaIntString2ValNodeList(pmmdbid, ",");
  if(pvnmmdbid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Misc_CommaIntString2ValNodeList failed.");
      fprintf(stdout, "SEQHOUND_ERROR ValNode conversion error.\n");
      MemFree(pmmdbid);
      return SLRI_FAIL;
   }
  pvnmuid = SHoundMuidFrom3DList(pvnmmdbid);
  pstr = Misc_SaveValNodeListAsString(pvnmuid);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(pmmdbid);
  ValNodeFree(pvnmuid);
  ValNodeFree(pvnmmdbid);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundTaxIDFromGi(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pgi = NULL;
  Int4 gival = 0;
  Int4 taxid = 0;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"gi")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pgi = StringSave(pcThis);
   }
  if (pgi == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  if(!StrToInt4 (pgi, &gival))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(pgi);
      return SLRI_FAIL;
   }
  taxid = SHoundTaxIDFromGi(gival);
  fprintf(stdout, "SEQHOUND_OK\n");
  fprintf(stdout, "%ld\n", (long) taxid);
  MemFree(pgi);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundTaxIDFromGiList(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pstr = NULL;
  ValNodePtr pvngi = NULL;
  ValNodePtr pvntaxid = NULL;
  CharPtr pgi = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"pgi")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pgi = StringSave(pcThis);
   }
  if (pgi == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  pvngi = Misc_CommaIntString2ValNodeList(pgi, ",");
  if(pvngi == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Misc_CommaIntString2ValNodeList failed.");
      fprintf(stdout, "SEQHOUND_ERROR ValNode conversion error.\n");
      MemFree(pgi);
      return SLRI_FAIL;
   }
  pvntaxid = SHoundTaxIDFromGiList(pvngi);
  pstr = Misc_SaveValNodeListAsString(pvntaxid);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(pgi);
  ValNodeFree(pvntaxid);
  ValNodeFree(pvngi);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundTaxIDFrom3D(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pstr = NULL;
  CharPtr pmmdbid = NULL;
  Int4 mmdbidval = 0;
  ValNodePtr pvntaxid = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"mmdbid")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pmmdbid = StringSave(pcThis);
   }
  if (pmmdbid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  if(!StrToInt4 (pmmdbid, &mmdbidval))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(pmmdbid);
      return SLRI_FAIL;
   }
  pvntaxid = SHoundTaxIDFrom3D(mmdbidval);
  pstr = Misc_SaveValNodeListAsString(pvntaxid);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(pmmdbid);
  ValNodeFree(pvntaxid);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundTaxIDFrom3DList(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pstr = NULL;
  ValNodePtr pvnmmdbid = NULL;
  CharPtr pmmdbid = NULL;
  ValNodePtr pvntaxid = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"pmmdbid")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pmmdbid = StringSave(pcThis);
   }
  if (pmmdbid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  pvnmmdbid = Misc_CommaIntString2ValNodeList(pmmdbid, ",");
  if(pvnmmdbid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Misc_CommaIntString2ValNodeList failed.");
      fprintf(stdout, "SEQHOUND_ERROR ValNode conversion error.\n");
      MemFree(pmmdbid);
      return SLRI_FAIL;
   }
  pvntaxid = SHoundTaxIDFrom3DList(pvnmmdbid);
  pstr = Misc_SaveValNodeListAsString(pvntaxid);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(pmmdbid);
  ValNodeFree(pvntaxid);
  ValNodeFree(pvnmmdbid);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundProteinsFromTaxID(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pstr = NULL;
  CharPtr ptaxid = NULL;
  Int4 taxidval = 0;
  ValNodePtr pvngi = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"taxid")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      ptaxid = StringSave(pcThis);
   }
  if (ptaxid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  if(!StrToInt4 (ptaxid, &taxidval))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(ptaxid);
      return SLRI_FAIL;
   }
  pvngi = SHoundProteinsFromTaxID(taxidval);
  pstr = Misc_SaveValNodeListAsString(pvngi);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(ptaxid);
  ValNodeFree(pvngi);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundProteinsFromTaxIDIII(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pstr = NULL;
  CharPtr ptaxid = NULL;
  Int4 taxidval = 0;
  ValNodePtr pvngi = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"taxid")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      ptaxid = StringSave(pcThis);
   }
  if (ptaxid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  if(!StrToInt4 (ptaxid, &taxidval))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(ptaxid);
      return SLRI_FAIL;
   }
  pvngi = SHoundProteinsFromTaxIDIII(taxidval);
  pstr = Misc_SaveValNodeListAsString(pvngi);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(ptaxid);
  ValNodeFree(pvngi);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundProteinsFromTaxIDList(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pstr = NULL;
  CharPtr ptaxid = NULL;
  ValNodePtr pvntaxid = NULL;
  ValNodePtr pvngi = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"ptaxid")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      ptaxid = StringSave(pcThis);
   }
  if (ptaxid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  pvntaxid = Misc_CommaIntString2ValNodeList(ptaxid, ",");
  if(pvntaxid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Misc_CommaIntString2ValNodeList failed.");
      fprintf(stdout, "SEQHOUND_ERROR ValNode conversion error.\n");
      MemFree(ptaxid);
      return SLRI_FAIL;
   }
  pvngi = SHoundProteinsFromTaxIDList(pvntaxid);
  pstr = Misc_SaveValNodeListAsString(pvngi);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(ptaxid);
  ValNodeFree(pvngi);
  ValNodeFree(pvntaxid);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundDNAFromTaxID(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pstr = NULL;
  CharPtr ptaxid = NULL;
  Int4 taxidval = 0;
  ValNodePtr pvngi = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"taxid")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      ptaxid = StringSave(pcThis);
   }
  if (ptaxid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  if(!StrToInt4 (ptaxid, &taxidval))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(ptaxid);
      return SLRI_FAIL;
   }
  pvngi = SHoundDNAFromTaxID(taxidval);
  pstr = Misc_SaveValNodeListAsString(pvngi);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(ptaxid);
  ValNodeFree(pvngi);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundDNAFromTaxIDIII(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pstr = NULL;
  CharPtr ptaxid = NULL;
  Int4 taxidval = 0;
  ValNodePtr pvngi = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"taxid")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      ptaxid = StringSave(pcThis);
   }
  if (ptaxid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  if(!StrToInt4 (ptaxid, &taxidval))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(ptaxid);
      return SLRI_FAIL;
   }
  pvngi = SHoundDNAFromTaxIDIII(taxidval);
  pstr = Misc_SaveValNodeListAsString(pvngi);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(ptaxid);
  ValNodeFree(pvngi);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundDNAFromTaxIDList(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pstr = NULL;
  CharPtr ptaxid = NULL;
  ValNodePtr pvntaxid = NULL;
  ValNodePtr pvngi = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"ptaxid")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      ptaxid = StringSave(pcThis);
   }
  if (ptaxid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  pvntaxid = Misc_CommaIntString2ValNodeList(ptaxid, ",");
  if(pvntaxid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Misc_CommaIntString2ValNodeList failed.");
      fprintf(stdout, "SEQHOUND_ERROR ValNode conversion error.\n");

      MemFree(ptaxid);
      return SLRI_FAIL;
   }
  pvngi = SHoundDNAFromTaxIDList(pvntaxid);
  pstr = Misc_SaveValNodeListAsString(pvngi);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(ptaxid);
  ValNodeFree(pvngi);
  ValNodeFree(pvntaxid);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundProteinsFromOrganism(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr ptaxid = NULL;
  CharPtr pstr = NULL;
  CharPtr pchromfl = NULL;
  Int4 taxidval = 0;
  Int4 chromflval = 0;
  ValNodePtr pvngia = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"taxid")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      ptaxid = StringSave(pcThis);
   }
  if (ptaxid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  if ((IndexArgs = WWWFindName(ginfo,"chromflags")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pchromfl = StringSave(pcThis);
   }
  if (pchromfl == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      MemFree(ptaxid);
      return SLRI_FAIL;
   }
  if(!StrToInt4 (ptaxid, &taxidval))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(ptaxid);
      MemFree(pchromfl);
      return SLRI_FAIL;
   }
  if(!StrToInt4 (pchromfl, &chromflval))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(ptaxid);
      MemFree(pchromfl);
      return SLRI_FAIL;
   }
  pvngia = SHoundProteinsFromOrganism(taxidval, (Byte) chromflval);
  pstr = Misc_SaveValNodeListAsString(pvngia);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(ptaxid);
  MemFree(pchromfl);
  ValNodeFree(pvngia);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundProteinsFromOrganismList(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pstr = NULL;
  CharPtr ptaxid = NULL;
  CharPtr pchromfl = NULL;
  Int4 chromflval = 0;
  ValNodePtr pvngia = NULL;
  ValNodePtr pvntaxid = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"ptaxid")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      ptaxid = StringSave(pcThis);
   }
  if (ptaxid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  if ((IndexArgs = WWWFindName(ginfo,"chromflags")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pchromfl = StringSave(pcThis);
   }
  if (pchromfl == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      MemFree(ptaxid);
      return SLRI_FAIL;
   }
  if(!StrToInt4 (pchromfl, &chromflval))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(ptaxid);
      MemFree(pchromfl);
      return SLRI_FAIL;
   }
  pvntaxid = Misc_CommaIntString2ValNodeList(ptaxid, ",");
  if(pvntaxid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Misc_CommaIntString2ValNodeList failed.");
      fprintf(stdout, "SEQHOUND_ERROR ValNode conversion error.\n");
      MemFree(ptaxid);
      MemFree(pchromfl);
      return SLRI_FAIL;
   }
  pvngia = SHoundProteinsFromOrganismList(pvntaxid, (Byte) chromflval);
  pstr = Misc_SaveValNodeListAsString(pvngia);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(ptaxid);
  MemFree(pchromfl);
  ValNodeFree(pvntaxid);
  ValNodeFree(pvngia);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundProteinsFromChromosome(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pstr = NULL;
  CharPtr pchromid = NULL;
  Int4 chromidval = 0;
  ValNodePtr pvngia = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"chromid")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pchromid = StringSave(pcThis);
   }
  if (pchromid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  if(!StrToInt4 (pchromid, &chromidval))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(pchromid);
      return SLRI_FAIL;
   }
  pvngia = SHoundProteinsFromChromosome(chromidval);
  pstr = Misc_SaveValNodeListAsString(pvngia);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(pchromid);
  ValNodeFree(pvngia);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundProteinsFromChromosomeList(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pstr = NULL;
  CharPtr pchromid = NULL;
  ValNodePtr pvnchromid = NULL;
  ValNodePtr pvngia = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"pchromid")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pchromid = StringSave(pcThis);
   }
  if (pchromid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  pvnchromid = Misc_CommaIntString2ValNodeList(pchromid, ",");
  if(pvnchromid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Misc_CommaIntString2ValNodeList failed.");
      fprintf(stdout, "SEQHOUND_ERROR ValNode conversion error.\n");
      MemFree(pchromid);
      return SLRI_FAIL;
   }
  pvngia = SHoundProteinsFromChromosomeList(pvnchromid);
  pstr = Misc_SaveValNodeListAsString(pvngia);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(pchromid);
  ValNodeFree(pvnchromid);
  ValNodeFree(pvngia);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundAllGenomes(void)
{
  CharPtr pstr = NULL;
  ValNodePtr pvngen = NULL;

  printf("Content-type: text/html\r\n\r\n");
  pvngen = SHoundAllGenomes();
  pstr = Misc_SaveValNodeListAsString(pvngen);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  ValNodeFree(pvngen);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundChromosomeFromGenome(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pstr = NULL;
  CharPtr ptaxid = NULL;
  CharPtr pchromfl = NULL;
  Int4 taxidval = 0;
  Int4 chromflval = 0;
  ValNodePtr pvnchromid = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"taxid")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      ptaxid = StringSave(pcThis);
   }
  if (ptaxid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  if ((IndexArgs = WWWFindName(ginfo,"chromflags")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pchromfl = StringSave(pcThis);
   }
  if (pchromfl == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      MemFree(ptaxid);
      return SLRI_FAIL;
   }
  if(!StrToInt4 (ptaxid, &taxidval))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(ptaxid);
      MemFree(pchromfl);
      return SLRI_FAIL;
   }
  if(!StrToInt4 (pchromfl, &chromflval))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(ptaxid);
      MemFree(pchromfl);
      return SLRI_FAIL;
   }
  pvnchromid = SHoundChromosomeFromGenome(taxidval, (Byte) chromflval);
  pstr = Misc_SaveValNodeListAsString(pvnchromid);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(ptaxid);
  MemFree(pchromfl);
  ValNodeFree(pvnchromid);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundChromosomeFromGenomeList(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pstr = NULL;
  CharPtr ptaxid = NULL;
  CharPtr pchromfl = NULL;
  Int4 chromflval = 0;
  ValNodePtr pvntaxid = NULL;
  ValNodePtr pvnchromid = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"ptaxid")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      ptaxid = StringSave(pcThis);
   }
  if (ptaxid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  if ((IndexArgs = WWWFindName(ginfo,"chromflags")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pchromfl = StringSave(pcThis);
   }
  if (pchromfl == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      MemFree(ptaxid);
      return SLRI_FAIL;
   }
  if(!StrToInt4 (pchromfl, &chromflval))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(ptaxid);
      MemFree(pchromfl);
      return SLRI_FAIL;
   }
  pvntaxid = Misc_CommaIntString2ValNodeList(ptaxid, ",");
  if(pvntaxid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Misc_CommaIntString2ValNodeList failed.");
      fprintf(stdout, "SEQHOUND_ERROR ValNode conversion error.\n");
      MemFree(ptaxid);
      MemFree(pchromfl);
      return SLRI_FAIL;
   }
  pvnchromid = SHoundChromosomeFromGenomeList(pvntaxid, (Byte) chromflval);
  pstr = Misc_SaveValNodeListAsString(pvnchromid);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(ptaxid);
  MemFree(pchromfl);
  ValNodeFree(pvntaxid);
  ValNodeFree(pvnchromid);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundDNAFromOrganism(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pstr = NULL;
  CharPtr ptaxid = NULL;
  CharPtr pchromfl = NULL;
  Int4 taxidval = 0;
  Int4 chromflval = 0;
  ValNodePtr pvngin = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"taxid")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      ptaxid = StringSave(pcThis);
   }
  if (ptaxid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  if ((IndexArgs = WWWFindName(ginfo,"chromflags")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pchromfl = StringSave(pcThis);
   }
  if (pchromfl == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      MemFree(ptaxid);
      return SLRI_FAIL;
   }
  if(!StrToInt4 (ptaxid, &taxidval))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(ptaxid);
      MemFree(pchromfl);
      return SLRI_FAIL;
   }
  if(!StrToInt4 (pchromfl, &chromflval))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(ptaxid);
      MemFree(pchromfl);
      return SLRI_FAIL;
   }
  pvngin = SHoundDNAFromOrganism(taxidval, (Byte) chromflval);
  pstr = Misc_SaveValNodeListAsString(pvngin);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(ptaxid);
  MemFree(pchromfl);
  ValNodeFree(pvngin);
  MemFree(pstr);
  return SLRI_SUCCESS;
}


SLRI_ERR SeqHoundRNAFromOrganism(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pstr = NULL;
  CharPtr ptaxid = NULL;
  CharPtr pchromfl = NULL;
  Int4 taxidval = 0;
  Int4 chromflval = 0;
  ValNodePtr pvngin = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"taxid")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      ptaxid = StringSave(pcThis);
   }
  if (ptaxid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  if ((IndexArgs = WWWFindName(ginfo,"chromflags")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pchromfl = StringSave(pcThis);
   }
  if (pchromfl == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      MemFree(ptaxid);
      return SLRI_FAIL;
   }
  if(!StrToInt4 (ptaxid, &taxidval))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(ptaxid);
      MemFree(pchromfl);
      return SLRI_FAIL;
   }
  if(!StrToInt4 (pchromfl, &chromflval))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(ptaxid);
      MemFree(pchromfl);
      return SLRI_FAIL;
   }
  pvngin = SHoundRNAFromOrganism(taxidval, (Byte) chromflval);
  pstr = Misc_SaveValNodeListAsString(pvngin);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(ptaxid);
  MemFree(pchromfl);
  ValNodeFree(pvngin);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundDNAFromOrganismList(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pstr = NULL;
  Int4 chromflval = 0;
  ValNodePtr pvntaxid = NULL;
  ValNodePtr pvngin = NULL;
  CharPtr ptaxid = NULL;
  CharPtr pchromfl = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"ptaxid")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      ptaxid = StringSave(pcThis);
   }
  if (ptaxid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  if ((IndexArgs = WWWFindName(ginfo,"chromflags")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pchromfl = StringSave(pcThis);
   }
  if (pchromfl == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      MemFree(ptaxid);
      return SLRI_FAIL;
   }
  if(!StrToInt4 (pchromfl, &chromflval))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(ptaxid);
      MemFree(pchromfl);
      return SLRI_FAIL;
   }
  pvntaxid = Misc_CommaIntString2ValNodeList(ptaxid, ",");
  if(pvntaxid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Misc_CommaIntString2ValNodeList failed.");
      fprintf(stdout, "SEQHOUND_ERROR ValNode conversion error.\n");
      MemFree(ptaxid);
      MemFree(pchromfl);
      return SLRI_FAIL;
   }
  pvngin = SHoundDNAFromOrganismList(pvntaxid, (Byte) chromflval);
  pstr = Misc_SaveValNodeListAsString(pvngin);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(ptaxid);
  MemFree(pchromfl);
  ValNodeFree(pvntaxid);
  ValNodeFree(pvngin);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundDNAFromUnknownChromosome(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pstr = NULL;
  CharPtr ptaxid = NULL;
  Int4 taxidval = 0;
  ValNodePtr pvngin = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"taxid")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      ptaxid = StringSave(pcThis);
   }
  if (ptaxid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  
  if(!StrToInt4 (ptaxid, &taxidval))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(ptaxid);
       return SLRI_FAIL;
   }
 
  pvngin = SHoundDNAFromUnknownChromosome(taxidval);
  pstr = Misc_SaveValNodeListAsString(pvngin);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(ptaxid);
  ValNodeFree(pvngin);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundRNAFromUnknownChromosome(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pstr = NULL;
  CharPtr ptaxid = NULL;
  Int4 taxidval = 0;
  ValNodePtr pvngin = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"taxid")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      ptaxid = StringSave(pcThis);
   }
  if (ptaxid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  
  if(!StrToInt4 (ptaxid, &taxidval))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(ptaxid);
       return SLRI_FAIL;
   }
 
  pvngin = SHoundRNAFromUnknownChromosome(taxidval);
  pstr = Misc_SaveValNodeListAsString(pvngin);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(ptaxid);
  ValNodeFree(pvngin);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundProteinsFromUnknownChromosome(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pstr = NULL;
  CharPtr ptaxid = NULL;
  Int4 taxidval = 0;
  ValNodePtr pvngin = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"taxid")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      ptaxid = StringSave(pcThis);
   }
  if (ptaxid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  
  if(!StrToInt4 (ptaxid, &taxidval))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(ptaxid);
       return SLRI_FAIL;
   }
 
  pvngin = SHoundProteinsFromUnknownChromosome(taxidval);
  pstr = Misc_SaveValNodeListAsString(pvngin);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(ptaxid);
  ValNodeFree(pvngin);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundDNAFromChromosome(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pstr = NULL;
  CharPtr pchromid = NULL;
  Int4 chromidval = 0;
  ValNodePtr pvngin = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"chromid")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pchromid = StringSave(pcThis);
   }
  if (pchromid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  if(!StrToInt4 (pchromid, &chromidval))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(pchromid);
      return SLRI_FAIL;
   }
  pvngin = SHoundDNAFromChromosome(chromidval);
  pstr = Misc_SaveValNodeListAsString(pvngin);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(pchromid);
  ValNodeFree(pvngin);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundRNAFromChromosome(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pstr = NULL;
  CharPtr pchromid = NULL;
  Int4 chromidval = 0;
  ValNodePtr pvngin = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"chromid")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pchromid = StringSave(pcThis);
   }
  if (pchromid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  if(!StrToInt4 (pchromid, &chromidval))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(pchromid);
      return SLRI_FAIL;
   }
  pvngin = SHoundRNAFromChromosome(chromidval);
  pstr = Misc_SaveValNodeListAsString(pvngin);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(pchromid);
  ValNodeFree(pvngin);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundDNAFromChromosomeList(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pstr = NULL;
  CharPtr pchromid = NULL;
  ValNodePtr pvnchromid = NULL;
  ValNodePtr pvngin = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"pchromid")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pchromid = StringSave(pcThis);
   }
  if (pchromid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  pvnchromid = Misc_CommaIntString2ValNodeList(pchromid, ",");
  if(pvnchromid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Misc_CommaIntString2ValNodeList failed.");
      fprintf(stdout, "SEQHOUND_ERROR ValNode conversion error.\n");
      MemFree(pchromid);
      return SLRI_FAIL;
   }
  pvngin = SHoundDNAFromChromosomeList(pvnchromid);
  pstr = Misc_SaveValNodeListAsString(pvngin);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(pchromid);
  ValNodeFree(pvnchromid);
  ValNodeFree(pvngin);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundRedundantGroup(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pstr = NULL;
  CharPtr pgi = NULL;
  Int4 gival = 0;
  ValNodePtr pvngi = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"gi")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pgi = StringSave(pcThis);
   }
  if (pgi == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  if(!StrToInt4 (pgi, &gival))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(pgi);
      return SLRI_FAIL;
   }
  pvngi = SHoundRedundantGroup(gival);
  pstr = Misc_SaveValNodeListAsString(pvngi);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(pgi);
  ValNodeFree(pvngi);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundRedundantGroupList(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pstr = NULL;
  CharPtr pgi = NULL;
  ValNodePtr pvngi = NULL;
  ValNodePtr pvngifin = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"pgi")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pgi = StringSave(pcThis);
   }
  if (pgi == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  pvngi = Misc_CommaIntString2ValNodeList(pgi, ",");
  if(pvngi == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Misc_CommaIntString2ValNodeList failed.");
      fprintf(stdout, "SEQHOUND_ERROR ValNode conversion error.\n");
      MemFree(pgi);
      return SLRI_FAIL;
   }
  pvngifin = SHoundRedundantGroupList(pvngi);
  pstr = Misc_SaveValNodeListAsString(pvngifin);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(pgi);
  ValNodeFree(pvngifin);
  ValNodeFree(pvngi);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundRedundantGroupIDFromGI(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pgi = NULL;
  Int4 gival = 0;
  Int4 groupid = 0;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"gi")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pgi = StringSave(pcThis);
   }
  if (pgi == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");

      return SLRI_FAIL;
   }
  if(!StrToInt4 (pgi, &gival))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(pgi);
      return SLRI_FAIL;
   }
  groupid = SHoundRedundantGroupIDFromGI(gival);
  fprintf(stdout, "SEQHOUND_OK\n");
  fprintf(stdout, "%ld\n", (long) groupid);
  MemFree(pgi);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundRedundantGroupIDFromGIList(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pstr = NULL;
  CharPtr pgi = NULL;
  ValNodePtr pvngi = NULL;
  ValNodePtr pvngifin = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"pgi")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pgi = StringSave(pcThis);
   }
  if (pgi == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  pvngi = Misc_CommaIntString2ValNodeList(pgi, ",");
  if(pvngi == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Misc_CommaIntString2ValNodeList failed.");
      fprintf(stdout, "SEQHOUND_ERROR ValNode conversion error.\n");
      MemFree(pgi);
      return SLRI_FAIL;
   }
  pvngifin = SHoundRedundantGroupIDFromGIList(pvngi);
  pstr = Misc_SaveValNodeListAsString(pvngifin);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(pgi);
  ValNodeFree(pvngifin);
  ValNodeFree(pvngi);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundRedundantGroupFromID(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pstr = NULL;
  CharPtr pgroup = NULL;
  Int4 groupval = 0;
  ValNodePtr pvngi = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"group")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pgroup = StringSave(pcThis);
   }
  if (pgroup == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  if(!StrToInt4 (pgroup, &groupval))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(pgroup);
      return SLRI_FAIL;
   }
  pvngi = SHoundRedundantGroupFromID(groupval);
  pstr = Misc_SaveValNodeListAsString(pvngi);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(pgroup);
  ValNodeFree(pvngi);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundRedundantGroupFromIDList(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pstr = NULL;
  CharPtr pgroup = NULL;
  ValNodePtr pvngroup = NULL;
  ValNodePtr pvngifin = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"pgroup")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pgroup = StringSave(pcThis);
   }
  if (pgroup == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  pvngroup = Misc_CommaIntString2ValNodeList(pgroup, ",");
  if(pvngroup == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Misc_CommaIntString2ValNodeList failed.");
      fprintf(stdout, "SEQHOUND_ERROR ValNode conversion error.\n");
      MemFree(pgroup);
      return SLRI_FAIL;
   }
  pvngifin = SHoundRedundantGroupFromIDList(pvngroup);
  pstr = Misc_SaveValNodeListAsString(pvngifin);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(pgroup);
  ValNodeFree(pvngifin);
  ValNodeFree(pvngroup);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundFirstOfRedundantGroupFromID(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pgroup = NULL;
  Int4 groupval = 0;
  Int4 gival = 0;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"group")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pgroup = StringSave(pcThis);
   }
  if (pgroup == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  if(!StrToInt4 (pgroup, &groupval))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(pgroup);
      return SLRI_FAIL;
   }
  gival = SHoundFirstOfRedundantGroupFromID(groupval);
  fprintf(stdout, "SEQHOUND_OK\n");
  fprintf(stdout, "%ld\n", (long) gival);
  MemFree(pgroup);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundFirstOfRedundantGroupFromIDList(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pstr = NULL;
  CharPtr pgroup = NULL;
  ValNodePtr pvngroup = NULL;
  ValNodePtr pvngifin = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"pgroup")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pgroup = StringSave(pcThis);
   }
  if (pgroup == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  pvngroup = Misc_CommaIntString2ValNodeList(pgroup, ",");
  if(pvngroup == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Misc_CommaIntString2ValNodeList failed.");
      fprintf(stdout, "SEQHOUND_ERROR ValNode conversion error.\n");
      MemFree(pgroup);
      return SLRI_FAIL;
   }
  pvngifin = SHoundFirstOfRedundantGroupFromIDList(pvngroup);
  pstr = Misc_SaveValNodeListAsString(pvngifin);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(pgroup);
  ValNodeFree(pvngifin);
  ValNodeFree(pvngroup);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGetTaxChildNodes(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pstr = NULL;
  CharPtr ptaxid = NULL;
  Int4 taxidval = 0;
  ValNodePtr pvnnodes = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"taxid")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      ptaxid = StringSave(pcThis);
   }
  if (ptaxid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  if(!StrToInt4 (ptaxid, &taxidval))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(ptaxid);
      return SLRI_FAIL;
   }
  pvnnodes = SHoundGetTaxChildNodes(taxidval);
  pstr = Misc_SaveValNodeListAsString(pvnnodes);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(ptaxid);
  ValNodeFree(pvnnodes);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGetTaxChildNodesList(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pstr = NULL;
  CharPtr ptaxid = NULL;
  ValNodePtr pvntaxid = NULL;
  ValNodePtr pvnnodes = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"ptaxid")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      ptaxid = StringSave(pcThis);
   }
  if (ptaxid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  pvntaxid = Misc_CommaIntString2ValNodeList(ptaxid, ",");
  if(pvntaxid== NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Misc_CommaIntString2ValNodeList failed.");
      fprintf(stdout, "SEQHOUND_ERROR ValNode conversion error.\n");
      MemFree(ptaxid);
      return SLRI_FAIL;
   }
  pvnnodes = SHoundGetTaxChildNodesList(pvntaxid);
  pstr = Misc_SaveValNodeListAsString(pvnnodes);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(ptaxid);
  ValNodeFree(pvnnodes);
  ValNodeFree(pvntaxid);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGetAllTaxProgeny(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pstr = NULL;
  CharPtr ptaxid = NULL;
  Int4 taxidval = 0;
  ValNodePtr pvnnodes = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"taxid")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      ptaxid = StringSave(pcThis);
   }
  if (ptaxid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  if(!StrToInt4 (ptaxid, &taxidval))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(ptaxid);
      return SLRI_FAIL;
   }
  pvnnodes = SHoundGetAllTaxProgeny(taxidval);
  pstr = Misc_SaveValNodeListAsString(pvnnodes);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(ptaxid);
  ValNodeFree(pvnnodes);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGetTaxParent(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr ptaxid = NULL;
  Int4 taxidval = 0;
  Int4 taxidvalfin = 0;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"taxid")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      ptaxid = StringSave(pcThis);
   }
  if (ptaxid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  if(!StrToInt4 (ptaxid, &taxidval))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(ptaxid);
      return SLRI_FAIL;
   }
  taxidvalfin = SHoundGetTaxParent(taxidval);
  fprintf(stdout, "SEQHOUND_OK\n");
  fprintf(stdout, "%ld\n", (long) taxidvalfin);
  MemFree(ptaxid);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGetAllTaxAncestors(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pstr = NULL;
  CharPtr ptaxid = NULL;
  Int4 taxidval = 0;
  ValNodePtr pvnnodes = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"taxid")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      ptaxid = StringSave(pcThis);
   }
  if (ptaxid == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  if(!StrToInt4 (ptaxid, &taxidval))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(ptaxid);
      return SLRI_FAIL;
   }
  pvnnodes = SHoundGetAllTaxAncestors(taxidval);
  pstr = Misc_SaveValNodeListAsString(pvnnodes);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(ptaxid);
  ValNodeFree(pvnnodes);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundIsTaxMerged(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  Int4 taxidval = 0;
  CharPtr ptaxid = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"taxid")) >= 0)
    {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       ptaxid = StringSave(pcThis);
    }
  if (ptaxid == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
     }
  if(!StrToInt4 (ptaxid, &taxidval))
   {
       ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
       fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
       MemFree(ptaxid);
       return SLRI_FAIL;
   }
  if(SHoundIsTaxMerged(taxidval))
   {
     fprintf(stdout, "SEQHOUND_OK\n");
     fprintf(stdout, "TRUE\n");
   }
  else
   {
     fprintf(stdout, "SEQHOUND_OK\n");
     fprintf(stdout, "FALSE\n");
   }
  MemFree(ptaxid);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGetNewTaxIDFromMergedTaxID(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  Int4 taxidval = 0, newtaxid=0;
  CharPtr ptaxid = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"taxid")) >= 0)
    {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       ptaxid = StringSave(pcThis);
    }
  if (ptaxid == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "[SeqHoundGetNewTaxIDFromMergedTaxID]: Failed to get parameter value.");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
     }
  if(!StrToInt4 (ptaxid, &taxidval))
   {
       ErrPostEx(SEV_ERROR,0,0, "[SeqHoundGetNewTaxIDFromMergedTaxID]: Invalid function parameter.");
       fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
       MemFree(ptaxid);
       return SLRI_FAIL;
   }
  newtaxid= SHoundGetNewTaxIDFromMergedTaxID(taxidval);
  fprintf(stdout, "SEQHOUND_OK\n");
  fprintf(stdout, "%ld\n", (long) newtaxid);
  MemFree(ptaxid);
  return SLRI_SUCCESS;
}


SLRI_ERR SeqHoundIsTaxDeleted(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  Int4 taxidval = 0;
  CharPtr ptaxid = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"taxid")) >= 0)
    {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       ptaxid = StringSave(pcThis);
    }
  if (ptaxid == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
     }
  if(!StrToInt4 (ptaxid, &taxidval))
   {
       ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
       fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
       MemFree(ptaxid);
       return SLRI_FAIL;
   }
  if(SHoundIsTaxDeleted(taxidval))
   {
     fprintf(stdout, "SEQHOUND_OK\n");
     fprintf(stdout, "TRUE\n");
   }
  else
   {
     fprintf(stdout, "SEQHOUND_OK\n");
     fprintf(stdout, "FALSE\n");
   }
  MemFree(ptaxid);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGetTaxNameFromTaxIDByClass(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  Int4 taxidval = 0;
  CharPtr ptaxid = NULL;
  CharPtr ptype = NULL;
  Int4 typeval = 0;
  CharPtr pname = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"taxid")) >= 0)
    {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       ptaxid = StringSave(pcThis);
    }
  if (ptaxid == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
     }
  if ((IndexArgs = WWWFindName(ginfo,"type")) >= 0)
    {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       ptype = StringSave(pcThis);
    }
  if (ptype == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       MemFree(ptaxid);
       return SLRI_FAIL;
     }
  if(!StrToInt4 (ptaxid, &taxidval))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(ptaxid);
      MemFree(ptype);
      return SLRI_FAIL;
   }
  if(!StrToInt4 (ptype, &typeval))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(ptaxid);
      MemFree(ptype);
      return SLRI_FAIL;
   }
  pname = SHoundGetTaxNameFromTaxIDByClass(taxidval, (Uint2) typeval);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pname != NULL) fprintf(stdout, "%s\n", pname);
  else fprintf(stdout, "(null)\n");
  MemFree(ptaxid);
  MemFree(ptype);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGetTaxNameFromTaxID(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  Int4 taxidval = 0;
  CharPtr ptaxid = NULL;
  CharPtr pname = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"taxid")) >= 0)
    {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       ptaxid = StringSave(pcThis);
    }
  if (ptaxid == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
     }
  if(!StrToInt4 (ptaxid, &taxidval))
   {
       ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
       fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
       MemFree(ptaxid);
       return SLRI_FAIL;
   }
  pname = SHoundGetTaxNameFromTaxID(taxidval);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pname != NULL) fprintf(stdout, "%s\n", pname);
  else fprintf(stdout, "(null)\n");
  MemFree(ptaxid);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGetTaxLineageFromTaxID(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  Int4 taxidval = 0;
  CharPtr ptaxid = NULL;
  CharPtr ptype = NULL;
  Int4 typeval = 0;
  CharPtr pname = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"taxid")) >= 0)
    {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       ptaxid = StringSave(pcThis);
    }
  if (ptaxid == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
     }
  if ((IndexArgs = WWWFindName(ginfo,"type")) >= 0)
    {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       ptype = StringSave(pcThis);
    }
  if (ptype == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       MemFree(ptaxid);
       return SLRI_FAIL;
     }
  if(!StrToInt4 (ptaxid, &taxidval))
   {
       ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
       fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
       MemFree(ptaxid);
       MemFree(ptype);
       return SLRI_FAIL;
   }
  if(!StrToInt4 (ptype, &typeval))
   {
       ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
       fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
       MemFree(ptaxid);
       MemFree(ptype);
       return SLRI_FAIL;
   }
  pname = SHoundGetTaxLineageFromTaxID(taxidval, (Uint2) typeval);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pname != NULL) fprintf(stdout, "%s\n", pname);
  else fprintf(stdout, "(null)\n");
  MemFree(ptaxid);
  MemFree(ptype);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGetGenBankff(void)
{
   Int4 IndexArgs = -1;
   CharPtr pcThis = NULL;
   CharPtr giptr = NULL;
   Int4 gival = 0;

   if ((IndexArgs = WWWFindName(ginfo,"gi")) >= 0)
    {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       giptr = StringSave(pcThis);
    }
   if (giptr == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
    }
   if(!StrToInt4 (giptr, &gival))
    {
       ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
       MemFree(giptr);
       return SLRI_FAIL;
    }
   if(gival <= 0)
   {
       ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
       MemFree(giptr);
       return SLRI_FAIL;
    }
   printf("Content-type: text/html\r\n\r\n");
   fprintf(stdout, "SEQHOUND_OK\n");
   if(!SHoundGetGenBankff(gival, stdout))
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundGetGenBankff returned FALSE.");
      MemFree(giptr);
      return SLRI_FAIL;
    }
   MemFree(giptr);
   return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGetGenBankffList(void)
{
   Int4 IndexArgs = -1;
   CharPtr pcThis = NULL;
   CharPtr pgi = NULL;
   ValNodePtr pvngi = NULL;
   FILE *StdOot=NULL;

   if ((IndexArgs = WWWFindName(ginfo,"pgi")) >= 0)
    {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       pgi = StringSave(pcThis);
    }
   if (pgi == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
    }
   pvngi = Misc_CommaIntString2ValNodeList(pgi, ",");
   if(pvngi == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "Misc_CommaIntString2ValNodeList failed.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Misc_CommaIntString2ValNodeList failed.\n");
      MemFree(pgi);
      return SLRI_FAIL;
    }
   StdOot=FileOpen("stdout", "w");
   if(StdOot == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "Cannnot open output file.");
      fprintf(stdout, "SEQHOUND_ERROR Cannnot open output file.\n");
      MemFree(pgi);
      ValNodeFree(pvngi);
      return SLRI_FAIL;
    }
   printf("Content-type: text/html\r\n\r\n");
   fprintf(stdout, "SEQHOUND_OK\n");
   if(!SHoundGetGenBankffList(pvngi, StdOot))
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundGetGenBankffList returned FALSE.");
      MemFree(pgi);
      ValNodeFree(pvngi);
      FileClose(StdOot);
      return SLRI_FAIL;
    }

   MemFree(pgi);
   ValNodeFree(pvngi);
   FileClose(StdOot);
   return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundXMLBioseqPL(void)
{
   Int4 IndexArgs = -1;
   CharPtr pcThis = NULL;
   CharPtr giptr = NULL;
   Int4 gival = 0;
   BioseqPtr pbs = NULL;
   ByteStorePtr bspXML = NULL;
   AsnIoBSPtr aipbs = NULL;
   CharPtr bufXML = NULL;

   if ((IndexArgs = WWWFindName(ginfo,"gi")) >= 0)
    {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       giptr = StringSave(pcThis);
    }
   if (giptr == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
    }
   if(!StrToInt4 (giptr, &gival))
    {
       ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
       MemFree(giptr);
       return SLRI_FAIL;
    }
   pbs = SHoundGetBioseq(gival);
   if(pbs == NULL)
   {
       ErrPostEx(SEV_ERROR,0,0, "Cannot get bioseq.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Cannot get bioseq.\n");
       MemFree(giptr);
       return SLRI_FAIL;
   }
   bspXML = BSNew(5000);
   /* SK: NULL ptr check */
   if ( bspXML == NULL )
   {
       ErrPostEx(SEV_ERROR,0,0, "BSNew failed.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR BSNew failed.\n");
       BioseqFree(pbs);
       MemFree(giptr);
       return SLRI_FAIL;
   }

   aipbs = AsnIoBSOpen("w", bspXML);
   if (aipbs == NULL)
   {
       ErrPostEx(SEV_ERROR,0,0, "Cannot open IO.\n");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Cannot open IO.\n");
       MemFree(giptr);
       BioseqFree(pbs);
       BSFree(bspXML);
       return SLRI_FAIL;
   }
   /* SK: NULL ptr check */
   if ( aipbs->aip != NULL )
      aipbs->aip->type |= ASNIO_XML;
   if (!BioseqAsnWrite(pbs, aipbs->aip, NULL))
   {
     ErrPostEx(SEV_ERROR,0,0, "BioseqAsnWrite failed.\n");
     fprintf(stdout, "SEQHOUND_ERROR .\n");
     AsnIoBSClose(aipbs);
     MemFree(giptr);
     BSFree(bspXML);
     BioseqFree(pbs);
     return SLRI_FAIL;
   }
   AsnIoFlush(aipbs->aip);
   bufXML = BSStringSave(bspXML);
   printf("Content-type: text/html\r\n\r\n");
   fprintf(stdout, "SEQHOUND_OK\n");
   fprintf(stdout, "%s", IsNullStr(bufXML));
   fflush(stdout);
   AsnIoBSClose(aipbs);
   BSFree(bspXML);
   MemFree(bufXML);
   BioseqFree(pbs);

   MemFree(giptr);
   return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGetXMLSeqEntryPL(void)
{
   Int4 IndexArgs = -1;
   CharPtr pcThis = NULL;
   CharPtr giptr = NULL;
   Int4 gival = 0;
   SeqEntryPtr pse = NULL;
   ByteStorePtr bspXML = NULL;
   AsnIoBSPtr aipbs = NULL;
   CharPtr bufXML = NULL;

   if ((IndexArgs = WWWFindName(ginfo,"gi")) >= 0)
    {
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       giptr = StringSave(pcThis);
    }
   if (giptr == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
    }
   if(!StrToInt4 (giptr, &gival))
    {
       ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
       MemFree(giptr);
       return SLRI_FAIL;
    }
   pse = SHoundGetSeqEntry(gival);
   if(pse == NULL)
   {
       ErrPostEx(SEV_ERROR,0,0, "Cannot get bioseq.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Cannot get bioseq.\n");
       MemFree(giptr);
       return SLRI_FAIL;
   }
   bspXML = BSNew(5000);
   /* SK: NULL ptr check */
   if(bspXML == NULL)
   {
       ErrPostEx(SEV_ERROR,0,0, "BSNew failed.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR BSNew failed.\n");
       SeqEntryFree(pse);
       MemFree(giptr);
       return SLRI_FAIL;
   }

   aipbs = AsnIoBSOpen("w", bspXML);
   if (aipbs == NULL)
   {
       ErrPostEx(SEV_ERROR,0,0, "Cannot open IO.\n");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Cannot open IO.\n");
       MemFree(giptr);
       SeqEntryFree(pse);
       BSFree(bspXML);
       return SLRI_FAIL;
   }
   /* SK: NULL ptr check */
   if ( aipbs->aip != NULL )
      aipbs->aip->type |= ASNIO_XML;
   if (!SeqEntryAsnWrite(pse, aipbs->aip, NULL))  /* write out in ASCII ASN.1 form */
   {
     ErrPostEx(SEV_ERROR,0,0, "BioseqAsnWrite failed.\n");
     fprintf(stdout, "SEQHOUND_ERROR .\n");
     AsnIoBSClose(aipbs);
     MemFree(giptr);
     SeqEntryFree(pse);
     AsnIoBSClose(aipbs);
     BSFree(bspXML);
     return SLRI_FAIL;
   }
   AsnIoFlush(aipbs->aip);
   bufXML = BSStringSave(bspXML);
   printf("Content-type: text/html\r\n\r\n");
   fprintf(stdout, "SEQHOUND_OK\n");
   fprintf(stdout, "%s", IsNullStr(bufXML));
   fflush(stdout);
   AsnIoBSClose(aipbs);
   BSFree(bspXML);
   MemFree(bufXML);
   SeqEntryFree(pse);
   MemFree(giptr);
   return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGetXML3DPL(void)
{
   Int4 IndexArgs = -1;
   CharPtr pcThis = NULL;
   CharPtr giptr = NULL;
   Int4 gival = 0;
   BiostrucPtr pbio = NULL;
   ByteStorePtr bspXML = NULL;
   AsnIoBSPtr aipbs = NULL;
   CharPtr bufXML = NULL;

   if ((IndexArgs = WWWFindName(ginfo,"mmdbid")) >= 0)
    {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       giptr = StringSave(pcThis);
    }
   if (giptr == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
    }
   if(!StrToInt4 (giptr, &gival))
    {
       ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
       MemFree(giptr);
       return SLRI_FAIL;
    }
   pbio = SHoundGet3D(gival);
   if(pbio == NULL)
   {
       ErrPostEx(SEV_ERROR,0,0, "Cannot get bioseq.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Cannot get bioseq.\n");
       MemFree(giptr);
       
       return SLRI_FAIL;
   }
   bspXML = BSNew(5000);
   /* SK: NULL ptr check */
   if(bspXML == NULL)
   {
       ErrPostEx(SEV_ERROR,0,0, "Cannot get bioseq.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Cannot get bioseq.\n");
       BiostrucFree(pbio);
       MemFree(giptr);
       
       return SLRI_FAIL;
   }
   aipbs = AsnIoBSOpen("w", bspXML);
   if (aipbs == NULL)
   {
       ErrPostEx(SEV_ERROR,0,0, "Cannot open IO.\n");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Cannot open IO.\n");
       MemFree(giptr);
       BiostrucFree(pbio);
       BSFree(bspXML);
       
       return SLRI_FAIL;
   }
   /* SK: NULL ptr check */
   if ( aipbs->aip != NULL )
      aipbs->aip->type |= ASNIO_XML;
   if (!BiostrucAsnWrite(pbio, aipbs->aip, NULL))  /* write out in ASCII ASN.1 form */
   {
     ErrPostEx(SEV_ERROR,0,0, "BioseqAsnWrite failed.\n");
     fprintf(stdout, "SEQHOUND_ERROR .\n");
     AsnIoBSClose(aipbs);
     MemFree(giptr);
     BiostrucFree(pbio);
     AsnIoBSClose(aipbs);
     BSFree(bspXML);
     
     return SLRI_FAIL;
   }
   AsnIoFlush(aipbs->aip);
   bufXML = BSStringSave(bspXML);
   printf("Content-type: text/html\r\n\r\n");
   fprintf(stdout, "SEQHOUND_OK\n");
   fprintf(stdout, "%s", IsNullStr(bufXML));
   fflush(stdout);
   AsnIoBSClose(aipbs);
   BSFree(bspXML);
   MemFree(bufXML);
   BiostrucFree(pbio);
   MemFree(giptr);
   
   return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGetPDB3DPL(void)
{
   Int4 IndexArgs = -1;
   CharPtr pcThis = NULL;
   CharPtr giptr = NULL;
   Int4 gival = 0;
   BiostrucPtr pbio = NULL;
   Int2 result = 0;
   PDNMS pmodelstruc=NULL;

   if ((IndexArgs = WWWFindName(ginfo,"mmdbid")) >= 0)
    {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       giptr = StringSave(pcThis);
    }
   if (giptr == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
    }
   if(!StrToInt4 (giptr, &gival))
    {
       ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
       MemFree(giptr);
       return SLRI_FAIL;
    }
   pbio = SHoundGet3D(gival);
   if(pbio == NULL)
   {
       ErrPostEx(SEV_ERROR,0,0, "Cannot get bioseq.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Cannot get bioseq.\n");
       MemFree(giptr);
       return SLRI_FAIL;
   }
   pmodelstruc=MakeAModelstruc(pbio);
   if(pmodelstruc == NULL)
   {
       ErrPostEx(SEV_ERROR,2,1,"SHoundGetPDB3D: MakeAModelstruc failed.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Cannot get biostruc.\n");
       MemFree(giptr);
       return SLRI_FAIL;
   }
   printf("Content-type: text/html\r\n\r\n");
   fprintf(stdout, "SEQHOUND_OK\n");
   result = WritePDBAllModel(pmodelstruc, stdout);
   if(result != 1)
    {
       ErrPostEx(SEV_ERROR,2,1,"SHoundGetPDB3D: WritePDBAllModel failed.");
       ClearStructures();
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Cannot write PDB.\n");
       MemFree(giptr);
       return SLRI_FAIL;
    }
   MemFree(giptr);
   return SLRI_SUCCESS;
}



/* BLAST result database functions */
SLRI_ERR SeqHoundGetBlastResult (void);
SLRI_ERR SeqHoundGetBlastSeqAlign(void);
/* Neighbour Function prototypes */
SLRI_ERR SeqHoundNeighboursFromGiEx(void);
SLRI_ERR SeqHoundNeighboursFromGi(void);
SLRI_ERR SeqHoundNeighboursFromGiList(void);
SLRI_ERR SeqHoundNeighboursFromTaxID(void);
SLRI_ERR SeqHoundNeighboursOfNeighbours(void);
SLRI_ERR SeqHoundNeighboursOfNeighboursList(void);
SLRI_ERR SeqHoundNeighboursOfNeighboursFromTaxID(void);
SLRI_ERR SeqHound3DNeighboursFromGiEx(void);
SLRI_ERR SeqHound3DNeighboursFromGi(void);
SLRI_ERR SeqHound3DNeighboursFromGiList(void);
SLRI_ERR SeqHound3DNeighboursFromTaxID(void);
SLRI_ERR SeqHound3DListFromTaxID(void);
SLRI_ERR SeqHoundGiAndNumNeighboursList(void);
SLRI_ERR SeqHoundNumNeighboursInDB(void);
CharPtr FLinkSet2CommaStr(FLinkSetPtr pfl);
CharPtr FLinkSetList2CommaStr(FLinkSetListPtr pfll);
CharPtr ILinkSet2CommaStr(ILinkSetPtr pil);
CharPtr ILinkSetList2CommaStr(ILinkSetListPtr pill);



SLRI_ERR SeqHoundGetBlastResult(void)
{
	Int4 argshere[3];
	CharPtr pcgi1 = NULL, pcgi2 = NULL;
	Int4 gi1 = 0;
	Int4 gi2 = 0;
	NBlastResultSetPtr pResult = NULL;
	FILE* fp = NULL;
	AsnIoPtr aip = NULL;
	Boolean bSuccess = FALSE;

	if (((argshere[0] = WWWFindName(ginfo,"gi1")) > 0) &&
		((argshere[1] = WWWFindName(ginfo,"gi2")) > 0) ) {

		if(((pcgi1 = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) &&
			((pcgi2 = StringSave(WWWGetValueByIndex(ginfo,argshere[1]))) != NULL) ) {

			if( ((StrToInt4(pcgi1,&gi1)) > 0) &&
				((StrToInt4(pcgi2,&gi2)) > 0) ) {

				if(!SHoundGetBlastResult(gi1, gi2, &pResult )) {				
					if(pResult) {
						if((fp = FileOpen("stdout", FILE_MODE)) != NULL) {				
							if((aip=AsnIoNew(ASN_MODE, fp, NULL,NULL,NULL)) != NULL) {
								printf("Content-type: application/x-octet-stream\n\n");
								fprintf(stdout, "SEQHOUND_OK\n");
								NBlastResultSetAsnWrite(pResult, aip, NULL);
								AsnIoClose(aip);
								bSuccess = TRUE;
							} else PostError("SEQHOUND_ERROR", "Unable to open ASN.1 IO.");
							FileClose(fp);
						} else PostError("SEQHOUND_ERROR", "Unable to open file.");
						NBlastResultSetFree(pResult);
					} else {
						PostError("SEQHOUND_NULL", "NULL return value.");
						bSuccess = TRUE;
					}
				} else PostError("SEQHOUND_ERROR", "Function returned error.");
			} else PostError("SEQHOUND_ERROR", "Invalid function parameter.");
		} else PostError("SEQHOUND_ERROR", "Failed to save parameter values.");
	} else PostError("SEQHOUND_ERROR", "Failed to get parameter values.");
	if(pcgi1) MemFree(pcgi1);
	if(pcgi2) MemFree(pcgi2);

	if(bSuccess == FALSE) {
		ErrPostEx(SEV_ERROR,0,0,"SeqHoundGetBlastResult failed.");
		return SLRI_FAIL;
	}
	return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGetBlastSeqAlign(void)
{
	Int4 argshere[3];
	CharPtr pcgi1 = NULL, pcgi2 = NULL;
	Int4 gi1 = 0;
	Int4 gi2 = 0;
	SeqAlignPtr sap = NULL;
	FILE* fp = NULL;
	AsnIoPtr aip = NULL;
	Boolean bSuccess = FALSE;

	if (((argshere[0] = WWWFindName(ginfo,"gi1")) > 0) &&
		((argshere[1] = WWWFindName(ginfo,"gi2")) > 0) ) {

		if(((pcgi1 = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) &&
			((pcgi2 = StringSave(WWWGetValueByIndex(ginfo,argshere[1]))) != NULL) ) {

			if( ((StrToInt4(pcgi1,&gi1)) > 0) &&
				((StrToInt4(pcgi2,&gi2)) > 0) ) {

				if(!SHoundGetBlastSeqAlign(gi1, gi2, &sap )) {				
					if(sap) {
						if((fp = FileOpen("stdout", FILE_MODE)) != NULL) {				
							if((aip=AsnIoNew(ASN_MODE, fp, NULL,NULL,NULL)) != NULL) {
								printf("Content-type: application/x-octet-stream\n\n");
								fprintf(stdout, "SEQHOUND_OK\n");
								SeqAlignAsnWrite(sap, aip, NULL);
								AsnIoClose(aip);
								bSuccess = TRUE;
							} else PostError("SEQHOUND_ERROR", "Unable to open ASN.1 IO.");
							FileClose(fp);
						} else PostError("SEQHOUND_ERROR", "Unable to open file.");
						SeqAlignFree(sap);
					} else {
						PostError("SEQHOUND_NULL", "NULL return value.");
						bSuccess = TRUE;
					}
				} else PostError("SEQHOUND_ERROR", "Function returned error.");
			} else PostError("SEQHOUND_ERROR", "Invalid function parameter.");
		} else PostError("SEQHOUND_ERROR", "Failed to save parameter values.");
	} else PostError("SEQHOUND_ERROR", "Failed to get parameter values.");
	if(pcgi1) MemFree(pcgi1);
	if(pcgi2) MemFree(pcgi2);

	if(bSuccess == FALSE) {
		ErrPostEx(SEV_ERROR,0,0,"SeqHoundGetBlastSeqAlign failed.");
		return SLRI_FAIL;
	}
	return SLRI_SUCCESS;
}


/* Functions */
SLRI_ERR SeqHoundNeighboursFromGiEx(void)
{
  Int4 argshere[3];
  CharPtr pcgi = NULL, pcstop = NULL, pceval = NULL, pclimit = NULL;
  Int4 gi = 0;
  int blimit = 0;
  float evalue = 0.0;
  FLinkSetPtr pNeighbours = NULL;
  FILE* fp = NULL;
  AsnIoPtr aip = NULL;
  Boolean bSuccess = FALSE;
  
  if (((argshere[0] = WWWFindName(ginfo,"gi")) > 0) &&
      ((argshere[1] = WWWFindName(ginfo,"evalue")) > 0) &&
      ((argshere[2] = WWWFindName(ginfo,"bLimit")) >= 0) ) {		
    
    if(((pcgi = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) &&
       ((pceval = StringSave(WWWGetValueByIndex(ginfo,argshere[1]))) != NULL) &&
       ((pclimit = StringSave(WWWGetValueByIndex(ginfo,argshere[2]))) != NULL) ) {
      
      if( ((StrToInt4(pcgi,&gi)) > 0) &&
	  ((evalue = strtod(pceval,&pcstop)) > 0.0) &&
	  ((blimit = atoi(pclimit)) >= 0) ) {
	
	if(!SHoundNeighboursFromGiEx((Int4) gi, &pNeighbours, (FloatHi) evalue, (Boolean) blimit)) {				
	  if(pNeighbours) {
	    if((fp = FileOpen("stdout", FILE_MODE)) != NULL) {				
	      if((aip=AsnIoNew(ASN_MODE, fp, NULL,NULL,NULL)) != NULL) {
		printf("Content-type: application/x-octet-stream\n\n");
		fprintf(stdout, "SEQHOUND_OK\n");
		FLinkSetAsnWrite(pNeighbours, aip, NULL);
		AsnIoClose(aip);
		bSuccess = TRUE;
	      } else PostError("SEQHOUND_ERROR", "Unable to open ASN.1 IO.");
	      FileClose(fp);
	    } else PostError("SEQHOUND_ERROR", "Unable to open file.");
	    FLinkSetFree(pNeighbours);
	  } else {
	    PostError("SEQHOUND_NULL", "NULL return value.");
	    bSuccess = TRUE;
	  }
	} else PostError("SEQHOUND_ERROR", "Function returned error.");
      } else PostError("SEQHOUND_ERROR", "Invalid function parameter.");
    } else PostError("SEQHOUND_ERROR", "Failed to save parameter values.");
  } else PostError("SEQHOUND_ERROR", "Failed to get parameter values.");
  if(pcgi) MemFree(pcgi);
  if(pceval) MemFree(pceval);
  if(pclimit) MemFree(pclimit);
  
  if(bSuccess == FALSE) {
    ErrPostEx(SEV_ERROR,0,0,"SeqHoundNeighboursFromGiEx failed.");
    return SLRI_FAIL;
  }
  return SLRI_SUCCESS;
}


SLRI_ERR SeqHoundNeighboursFromGi(void)
{
	Int4 argshere[3];
	CharPtr pcgi = NULL, pcstop = NULL, pceval = NULL, pclimit = NULL;
	Int4 gi = 0;
	int blimit = 0;
	double evalue = 0.0;
	FLinkSetPtr pNeighbours = NULL;
	FILE* fp = NULL;
	AsnIoPtr aip = NULL;
	Boolean bSuccess = FALSE;

	if (((argshere[0] = WWWFindName(ginfo,"gi")) >= 0) &&
		((argshere[1] = WWWFindName(ginfo,"evalue")) > 0) &&
		((argshere[2] = WWWFindName(ginfo,"bLimit")) >= 0) ) {	

		if(((pcgi = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) &&
			((pceval = StringSave(WWWGetValueByIndex(ginfo,argshere[1]))) != NULL) &&
			((pclimit = StringSave(WWWGetValueByIndex(ginfo,argshere[2]))) != NULL) ) {

			if(((StrToInt4(pcgi,&gi)) > 0) &&
				((evalue = strtod(pceval,&pcstop)) > 0.0) &&
				((blimit = atoi(pclimit)) >= 0) ) {

				if(!SHoundNeighboursFromGi((Int4) gi, &pNeighbours, (FloatHi) evalue, (Boolean) blimit)) {
					if(pNeighbours) {
						if((fp = FileOpen("stdout", FILE_MODE)) != NULL) {
							if((aip=AsnIoNew(ASN_MODE, fp, NULL,NULL,NULL)) != NULL) {
								printf("Content-type: application/x-octet-stream\n\n");
								fprintf(stdout, "SEQHOUND_OK\n");
								FLinkSetAsnWrite(pNeighbours, aip, NULL);
								AsnIoClose(aip);
								bSuccess = TRUE;
							} else PostError("SEQHOUND_ERROR", "Unable to open ASN.1 IO.");
							FileClose(fp);
						} else PostError("SEQHOUND_ERROR", "Unable to open file.");
						FLinkSetFree(pNeighbours);
					} else {
						PostError("SEQHOUND_NULL", "NULL return value.");
						bSuccess = TRUE;	
					}
				} else PostError("SEQHOUND_ERROR", "Function returned error.");
			} else PostError("SEQHOUND_ERROR", "Invalid function parameter.");
		} else PostError("SEQHOUND_ERROR", "Failed to save parameter values.");
	} else PostError("SEQHOUND_ERROR", "Failed to get parameter values.");
	if(pcgi) MemFree(pcgi);
	if(pceval) MemFree(pceval);
	if(pclimit) MemFree(pclimit);

	if(bSuccess == FALSE) {
		ErrPostEx(SEV_ERROR,0,0,"SeqHoundNeighboursFromGi failed.");
		return SLRI_FAIL;
	}
	return SLRI_SUCCESS;
}


SLRI_ERR SeqHoundNeighboursFromGiPL(void)
{
	Int4 argshere[3];
	CharPtr pcgi = NULL, pcstop = NULL, pceval = NULL, pclimit = NULL;
	Int4 gi = 0;
	int blimit = 0;
	double evalue = 0.0;
	FLinkSetPtr pNeighbours = NULL;
	Boolean bSuccess = FALSE;
        CharPtr pneig = NULL;
	
	if (((argshere[0] = WWWFindName(ginfo,"gi")) >= 0) &&
		((argshere[1] = WWWFindName(ginfo,"evalue")) > 0) &&
		((argshere[2] = WWWFindName(ginfo,"bLimit")) >= 0) ) {	

		if(((pcgi = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) &&
			((pceval = StringSave(WWWGetValueByIndex(ginfo,argshere[1]))) != NULL) &&
			((pclimit = StringSave(WWWGetValueByIndex(ginfo,argshere[2]))) != NULL) ) {

			if(((StrToInt4(pcgi,&gi)) > 0) &&
				((evalue = strtod(pceval,&pcstop)) > 0.0) &&
				((blimit = atoi(pclimit)) >= 0) ) {

				if(!SHoundNeighboursFromGi((Int4) gi, &pNeighbours, (FloatHi) evalue, (Boolean) blimit)) {
					if(pNeighbours) {
						pneig = FLinkSet2CommaStr(pNeighbours);
						if(pneig) {
						       printf("Content-type: text/html\r\n\r\n");
						       fprintf(stdout, "SEQHOUND_OK\n");
						       fprintf(stdout, "%s\n", pneig);
						       bSuccess = TRUE;
						       MemFree(pneig);
						} else PostError("SEQHOUND_ERROR", "FLinkSet2CommaStr failed.");
						FLinkSetFree(pNeighbours);
					} else {
						PostError("SEQHOUND_NULL", "NULL return value.");
						bSuccess = TRUE;	
					}
				} else PostError("SEQHOUND_ERROR", "Function returned error.");
			} else PostError("SEQHOUND_ERROR", "Invalid function parameter.");
		} else PostError("SEQHOUND_ERROR", "Failed to save parameter values.");
	} else PostError("SEQHOUND_ERROR", "Failed to get parameter values.");
	if(pcgi) MemFree(pcgi);
	if(pceval) MemFree(pceval);
	if(pclimit) MemFree(pclimit);

	if(bSuccess == FALSE) {
		ErrPostEx(SEV_ERROR,0,0,"SeqHoundNeighboursFromGi failed.");
		return SLRI_FAIL;
	}
	return SLRI_SUCCESS;
}


SLRI_ERR SeqHoundNeighboursFromGiList(void)
{
	Int4 argshere[2];
	CharPtr pcgilist = NULL, pcstop = NULL, pceval = NULL;
	double evalue = 0.0;
	CharPtr sep = ",";
	ValNodePtr pvngilist = NULL;
	FLinkSetListPtr pNeighboursList = NULL;
	FLinkSetList2Ptr pPackage = NULL;
	FILE* fp = NULL;
	AsnIoPtr aip = NULL;
	Boolean bSuccess = FALSE;

	if (((argshere[0] = WWWFindName(ginfo,"gilist")) >= 0) &&
		((argshere[1] = WWWFindName(ginfo,"evalue")) > 0) ) {

		if(((pcgilist = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) &&
			((pceval = StringSave(WWWGetValueByIndex(ginfo,argshere[1]))) != NULL) ) {

			if( ((pvngilist = Misc_CommaIntString2ValNodeList(pcgilist, sep)) != NULL) &&
				((evalue = strtod(pceval,&pcstop)) > 0.0) ) {
				
				if(!SHoundNeighboursFromGiList(pvngilist, &pNeighboursList, (FloatHi) evalue)) {
					if(pNeighboursList != NULL) {
						if((fp = FileOpen("stdout", FILE_MODE)) != NULL) {					
							if((aip=AsnIoNew(ASN_MODE, fp, NULL,NULL,NULL)) != NULL) {
								printf("Content-type: application/x-octet-stream\n\n");
								fprintf(stdout, "SEQHOUND_OK\n");
								pPackage = FLinkSetList2New();
                                                                /* SK: NULL ptr check */
                                                                if ( pPackage != NULL )
								   pPackage->set2 = pNeighboursList;
								FLinkSetList2AsnWrite(pPackage, aip, NULL);
								AsnIoClose(aip);
                                                                /* SK: NULL ptr check */
                                                                if ( pPackage != NULL )
                                                                {
								   pPackage->set2 = NULL;
								   pPackage = FLinkSetList2Free(pPackage);
                                                                }
								bSuccess = TRUE;
							} else PostError("SEQHOUND_ERROR", "Unable to open ASN.1 IO.");
							FileClose(fp);
						} else PostError("SEQHOUND_ERROR", "Unable to open file.");
						FLinkSetListFree(pNeighboursList);
					} else {
						PostError("SEQHOUND_NULL", "NULL return value.");
						bSuccess = TRUE;
					}
				} else PostError("SEQHOUND_ERROR", "Function returned error.");			
			} else PostError("SEQHOUND_ERROR", "Invalid function parameter.");
		} else PostError("SEQHOUND_ERROR", "Failed to save parameter values.");
	} else PostError("SEQHOUND_ERROR", "Failed to get parameter values.");
	if(pvngilist) ValNodeFree(pvngilist);
	if(pcgilist) MemFree(pcgilist);
	if(pceval) MemFree(pceval);
		
	if(bSuccess == FALSE) {
		ErrPostEx(SEV_ERROR,0,0,"SeqHoundNeighboursFromGiList failed.");
		return SLRI_FAIL;
	}
	return SLRI_SUCCESS;
}


SLRI_ERR SeqHoundNeighboursFromGiListPL(void)
{
	Int4 argshere[2];
	CharPtr pcgilist = NULL, pcstop = NULL, pceval = NULL;
	double evalue = 0.0;
	CharPtr sep = ",";
	ValNodePtr pvngilist = NULL;
	FLinkSetListPtr pNeighboursList = NULL;
	Boolean bSuccess = FALSE;
	CharPtr pneig = NULL;

	if (((argshere[0] = WWWFindName(ginfo,"gilist")) >= 0) &&
		((argshere[1] = WWWFindName(ginfo,"evalue")) > 0) ) {

		if(((pcgilist = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) &&
			((pceval = StringSave(WWWGetValueByIndex(ginfo,argshere[1]))) != NULL) ) {

			if( ((pvngilist = Misc_CommaIntString2ValNodeList(pcgilist, sep)) != NULL) &&
				((evalue = strtod(pceval,&pcstop)) > 0.0) ) {
				
				if(!SHoundNeighboursFromGiList(pvngilist, &pNeighboursList, (FloatHi) evalue)) {
					if(pNeighboursList != NULL) {
					        pneig = FLinkSetList2CommaStr(pNeighboursList);
						if(pneig) {
							printf("Content-type: text/html\r\n\r\n");
					        	fprintf(stdout, "SEQHOUND_OK\n");
							fprintf(stdout, "%s\n", pneig);
							bSuccess = TRUE;
							MemFree(pneig);
						} else PostError("SEQHOUND_ERROR", "FLinkSetList2CommaStr failed.");
						FLinkSetListFree(pNeighboursList);
					} else {
						PostError("SEQHOUND_NULL", "NULL return value.");
						bSuccess = TRUE;
					}
				} else PostError("SEQHOUND_ERROR", "Function returned error.");			
			} else PostError("SEQHOUND_ERROR", "Invalid function parameter.");
		} else PostError("SEQHOUND_ERROR", "Failed to save parameter values.");
	} else PostError("SEQHOUND_ERROR", "Failed to get parameter values.");
	if(pvngilist) ValNodeFree(pvngilist);
	if(pcgilist) MemFree(pcgilist);
	if(pceval) MemFree(pceval);
		
	if(bSuccess == FALSE) {
		ErrPostEx(SEV_ERROR,0,0,"SeqHoundNeighboursFromGiList failed.");
		return SLRI_FAIL;
	}
	return SLRI_SUCCESS;
}


SLRI_ERR SeqHoundNeighboursFromTaxID(void)
{
	Int4 argshere[2];
	CharPtr pctaxid = NULL, pcstop = NULL, pceval = NULL;
	Int4 taxid = 0;
	double evalue = 0.0;
	FLinkSetListPtr pNeighboursList = NULL;
	FLinkSetList2Ptr pPackage = NULL;
	FILE* fp = NULL;
	AsnIoPtr aip = NULL;
	Boolean bSuccess = FALSE;

	if (((argshere[0] = WWWFindName(ginfo,"taxid")) >= 0) &&
		((argshere[1] = WWWFindName(ginfo,"evalue")) > 0) ) {

		if(((pctaxid = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) &&
			((pceval = StringSave(WWWGetValueByIndex(ginfo,argshere[1]))) != NULL) ) {

			if( ((StrToInt4(pctaxid,&taxid)) > 0) &&
				((evalue = strtod(pceval,&pcstop)) > 0.0) ) {

				if(!SHoundNeighboursFromTaxID((Int4) taxid, &pNeighboursList, (FloatHi) evalue)) {
					if(pNeighboursList) {
						if((fp = FileOpen("stdout", FILE_MODE)) != NULL) {
							if((aip=AsnIoNew(ASN_MODE, fp, NULL,NULL,NULL)) != NULL) {
								printf("Content-type: application/x-octet-stream\n\n");
								fprintf(stdout, "SEQHOUND_OK\n");
								pPackage = FLinkSetList2New();
								/* SK: NULL ptr check */
								if ( pPackage != NULL )
								   pPackage->set2 = pNeighboursList;
								FLinkSetList2AsnWrite(pPackage, aip, NULL);
								AsnIoClose(aip);
								/* SK: NULL ptr check */
								if ( pPackage != NULL )
								{
								   pPackage->set2 = NULL;
								   pPackage = FLinkSetList2Free(pPackage);
								}
								bSuccess = TRUE;
							} else PostError("SEQHOUND_ERROR", "Unable to open ASN.1 IO.");
							FileClose(fp);
						} else PostError("SEQHOUND_ERROR", "Unable to open file.");
						FLinkSetListFree(pNeighboursList);
					} else {
						PostError("SEQHOUND_NULL", "NULL return value.");
						bSuccess = TRUE;
					}
				} else PostError("SEQHOUND_ERROR", "Function returned error.");
			} else PostError("SEQHOUND_ERROR", "Invalid function parameter.");
		} else PostError("SEQHOUND_ERROR", "Failed to save parameter values.");
	} else PostError("SEQHOUND_ERROR", "Failed to get parameter values.");
	if(pctaxid) MemFree(pctaxid);
	if(pceval) MemFree(pceval);

	if(bSuccess == FALSE) {
		ErrPostEx(SEV_ERROR,0,0,"SeqHoundNeighboursFromTaxID failed.");
		return SLRI_FAIL;
	}
	return SLRI_SUCCESS;
}


SLRI_ERR SeqHoundNeighboursFromTaxIDPL(void)
{
	Int4 argshere[2];
	CharPtr pctaxid = NULL, pcstop = NULL, pceval = NULL;
	Int4 taxid = 0;
	double evalue = 0.0;
	FLinkSetListPtr pNeighboursList = NULL;
	Boolean bSuccess = FALSE;
        CharPtr pneig = NULL;

	if (((argshere[0] = WWWFindName(ginfo,"taxid")) >= 0) &&
		((argshere[1] = WWWFindName(ginfo,"evalue")) > 0) ) {

		if(((pctaxid = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) &&
			((pceval = StringSave(WWWGetValueByIndex(ginfo,argshere[1]))) != NULL) ) {

			if( ((StrToInt4(pctaxid,&taxid)) > 0) &&
				((evalue = strtod(pceval,&pcstop)) > 0.0) ) {

				if(!SHoundNeighboursFromTaxID((Int4) taxid, &pNeighboursList, (FloatHi) evalue)) {
					if(pNeighboursList) {
						pneig = FLinkSetList2CommaStr(pNeighboursList);
						if(pneig) {
					        	printf("Content-type: text/html\r\n\r\n");
					        	fprintf(stdout, "SEQHOUND_OK\n");
							fprintf(stdout, "%s\n", pneig);
							bSuccess = TRUE;
							MemFree(pneig);
						} else PostError("SEQHOUND_ERROR", "FLinkSetList2CommaStr failed.");
						FLinkSetListFree(pNeighboursList);
					} else {
						PostError("SEQHOUND_NULL", "NULL return value.");
						bSuccess = TRUE;
					}
				} else PostError("SEQHOUND_ERROR", "Function returned error.");
			} else PostError("SEQHOUND_ERROR", "Invalid function parameter.");
		} else PostError("SEQHOUND_ERROR", "Failed to save parameter values.");
	} else PostError("SEQHOUND_ERROR", "Failed to get parameter values.");
	if(pctaxid) MemFree(pctaxid);
	if(pceval) MemFree(pceval);

	if(bSuccess == FALSE) {
		ErrPostEx(SEV_ERROR,0,0,"SeqHoundNeighboursFromTaxID failed.");
		return SLRI_FAIL;
	}
	return SLRI_SUCCESS;
}


SLRI_ERR SeqHoundNeighboursOfNeighbours(void)
{
	Int4 argshere[2];
	CharPtr pcgi = NULL, pcstop = NULL, pceval = NULL;
	Int4 gi = 0;
	double evalue = 0.0;
	FLinkSetListPtr pGINeighboursList = NULL;
	FLinkSetList2Ptr pPackage = NULL;
	FILE* fp = NULL;
	AsnIoPtr aip = NULL;
	Boolean bSuccess = FALSE;

	if (((argshere[0] = WWWFindName(ginfo,"gi")) >= 0) &&
		((argshere[1] = WWWFindName(ginfo,"evalue")) > 0) ) {

		if(((pcgi = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) &&
			((pceval = StringSave(WWWGetValueByIndex(ginfo,argshere[1]))) != NULL) ) {

			if( ((StrToInt4(pcgi,&gi)) > 0) &&
				((evalue = strtod(pceval,&pcstop)) > 0.0) ) {

				if(!SHoundNeighboursOfNeighbours((Int4) gi, &pGINeighboursList, (FloatHi) evalue)) {
					if(pGINeighboursList) {
						if((fp = FileOpen("stdout", FILE_MODE)) != NULL) {
							if((aip=AsnIoNew(ASN_MODE, fp, NULL,NULL,NULL)) != NULL) {
								printf("Content-type: application/x-octet-stream\n\n");
								fprintf(stdout, "SEQHOUND_OK\n");
								pPackage = FLinkSetList2New();
								/* SK: NULL ptr check */
								if ( pPackage != NULL )
								   pPackage->set2 = pGINeighboursList;
								FLinkSetList2AsnWrite(pPackage, aip, NULL);
								AsnIoClose(aip);
								/* SK: NULL ptr check */
								if ( pPackage != NULL )
								{
								   pPackage->set2 = NULL;
								   pPackage = FLinkSetList2Free(pPackage);
								}
								bSuccess = TRUE;
							} else PostError("SEQHOUND_ERROR", "Unable to open ASN.1 IO.");
							FileClose(fp);
						} else PostError("SEQHOUND_ERROR", "Unable to open file.");
						FLinkSetListFree(pGINeighboursList);
					} else {
						PostError("SEQHOUND_NULL", "NULL return value.");
						bSuccess = TRUE;
					}
				} else PostError("SEQHOUND_ERROR", "Function returned error.");
			} else PostError("SEQHOUND_ERROR", "Invalid function parameter.");	
		} else PostError("SEQHOUND_ERROR", "Failed to save parameter values.");
	} else PostError("SEQHOUND_ERROR", "Failed to get parameter values.");	
	if(pcgi) MemFree(pcgi);
	if(pceval) MemFree(pceval);

	if(bSuccess == FALSE) {
		ErrPostEx(SEV_ERROR,0,0,"SeqHoundNeighboursOfNeighbours failed.");
		return SLRI_FAIL;
	}
	return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundNeighboursOfNeighboursList(void)
{
	Int4 argshere[2];
	CharPtr pcgilist = NULL, pceval = NULL, pcstop = NULL;
	double evalue = 0.0;
	CharPtr sep = ",";
	ValNodePtr pvngilist = NULL;
	FLinkSetList2Ptr pNeighboursList = NULL;
	FLinkSetList3Ptr pPackage = NULL;
	FILE* fp = NULL;
	AsnIoPtr aip = NULL;
	Boolean bSuccess = FALSE;

	if (((argshere[0] = WWWFindName(ginfo,"gilist")) >= 0) &&
		((argshere[1] = WWWFindName(ginfo,"evalue")) > 0) ) {

		if(((pcgilist = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) &&
			((pceval = StringSave(WWWGetValueByIndex(ginfo,argshere[1]))) != NULL) ) {

			if( ((pvngilist = Misc_CommaIntString2ValNodeList(pcgilist, sep)) != NULL) &&
				((evalue = strtod(pceval,&pcstop)) > 0.0) ) {

				if(!SHoundNeighboursOfNeighboursList(pvngilist, &pNeighboursList, (FloatHi) evalue)) {
					if(pNeighboursList) {
						if((fp = FileOpen("stdout", FILE_MODE)) != NULL) {					
							if((aip=AsnIoNew(ASN_MODE, fp, NULL,NULL,NULL)) != NULL) {
								printf("Content-type: application/x-octet-stream\n\n");
								fprintf(stdout, "SEQHOUND_OK\n");
								pPackage = FLinkSetList3New();
								/* SK: NULL ptr check */
								if ( pPackage != NULL )
								   pPackage->set3 = pNeighboursList;
								FLinkSetList3AsnWrite(pPackage, aip, NULL);
								AsnIoClose(aip);
								/* SK: NULL ptr check */
								if ( pPackage != NULL )
								{
								   pPackage->set3 = NULL;
								   pPackage = FLinkSetList3Free(pPackage);
								}
								bSuccess = TRUE;
							} else PostError("SEQHOUND_ERROR", "Unable to open ASN.1 IO.");
							FileClose(fp);
						} else PostError("SEQHOUND_ERROR", "Unable to open file.");
						FLinkSetList2Free(pNeighboursList);
					} else {
						PostError("SEQHOUND_NULL", "NULL return value.");
						bSuccess = TRUE;
					}
				} else PostError("SEQHOUND_ERROR", "Function returned error.");				
			} else PostError("SEQHOUND_ERROR", "Invalid function parameter.");
		} else PostError("SEQHOUND_ERROR", "Failed to save parameter values.");
	} else PostError("SEQHOUND_ERROR", "Failed to get parameter values.");
	if(pvngilist) ValNodeFree(pvngilist);
	if(pcgilist) MemFree(pcgilist);
	if(pceval) MemFree(pceval);	
	
	if(bSuccess == FALSE) {
		ErrPostEx(SEV_ERROR,0,0,"SeqHoundNeighboursOfNeighboursList failed.");
		return SLRI_FAIL;
	}
	return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundNeighboursOfNeighboursFromTaxID(void)
{
	Int4 argshere[2];
	CharPtr pctaxid = NULL, pcstop = NULL, pceval = NULL;
	Int4 taxid = 0;
	double evalue = 0.0;
	FLinkSetList2Ptr pNeighboursList = NULL;
	FLinkSetList3Ptr pPackage = NULL;
	FILE* fp = NULL;
	AsnIoPtr aip = NULL;
	Boolean bSuccess = FALSE;

	if (((argshere[0] = WWWFindName(ginfo,"taxid")) >= 0) &&
		((argshere[1] = WWWFindName(ginfo,"evalue")) > 0) ) {

		if(((pctaxid = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) &&
			((pceval = StringSave(WWWGetValueByIndex(ginfo,argshere[1]))) != NULL) ) {

			if( ((StrToInt4(pctaxid,&taxid)) > 0) &&
				((evalue = strtod(pceval,&pcstop)) > 0.0) ) {

				if(!SHoundNeighboursOfNeighboursFromTaxID((Int4) taxid, &pNeighboursList, (FloatHi) evalue)) {
					if(pNeighboursList) {
						if((fp = FileOpen("stdout", FILE_MODE)) != NULL) {
							if((aip=AsnIoNew(ASN_MODE, fp, NULL,NULL,NULL)) != NULL) {
								printf("Content-type: application/x-octet-stream\n\n");
								fprintf(stdout, "SEQHOUND_OK\n");
								pPackage = FLinkSetList3New();
								/* SK: NULL ptr check */
								if ( pPackage != NULL )
								    pPackage->set3 = pNeighboursList;
								FLinkSetList3AsnWrite(pPackage, aip, NULL);
								AsnIoClose(aip);
								/* SK: NULL ptr check */
								if ( pPackage != NULL )
								{
								   pPackage->set3 = NULL;
								   pPackage = FLinkSetList3Free(pPackage);
								}
								AsnIoClose(aip);
								bSuccess = TRUE;
							} else PostError("SEQHOUND_ERROR", "Unable to open ASN.1 IO.");
							FileClose(fp);
						} else PostError("SEQHOUND_ERROR", "Unable to open file.");
						FLinkSetList2Free(pNeighboursList);
					} else {
						PostError("SEQHOUND_NULL", "NULL return value.");
						bSuccess = TRUE;
					}
				} else PostError("SEQHOUND_ERROR", "Function returned error.");				
			} else PostError("SEQHOUND_ERROR", "Invalid function parameter.");
		} else PostError("SEQHOUND_ERROR", "Failed to save parameter values.");
	} else PostError("SEQHOUND_ERROR", "Failed to get parameter values.");
	if(pctaxid) MemFree(pctaxid);
	if(pceval) MemFree(pceval);

	if(bSuccess == FALSE) {
		ErrPostEx(SEV_ERROR,0,0,"SeqHoundNeighboursFromTaxID failed.");
		return SLRI_FAIL;
	}
	return SLRI_SUCCESS;
}


SLRI_ERR SeqHound3DNeighboursFromGiEx(void)
{
	Int4 argshere[2];
	CharPtr pcgi = NULL, pcstop = NULL, pceval = NULL;
	Int4 gi = 0;
	double evalue = 0.0;
	ILinkSetPtr p3DNeighbours = NULL;
	FILE* fp = NULL;
	AsnIoPtr aip = NULL;
	Boolean bSuccess = FALSE;

	if (((argshere[0] = WWWFindName(ginfo,"gi")) >= 0) &&
		((argshere[1] = WWWFindName(ginfo,"evalue")) > 0) ) {

		if(((pcgi = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) &&
			((pceval = StringSave(WWWGetValueByIndex(ginfo,argshere[1]))) != NULL) ) {

			if(((StrToInt4(pcgi,&gi)) > 0) &&
				((evalue = strtod(pceval,&pcstop)) > 0.0) ) {

				if(!SHound3DNeighboursFromGiEx((Int4) gi, &p3DNeighbours,(FloatHi) evalue)) {
					if(p3DNeighbours) {
						if((fp = FileOpen("stdout", FILE_MODE)) != NULL) {
							if((aip=AsnIoNew(ASN_MODE, fp, NULL,NULL,NULL)) != NULL) {
								printf("Content-type: application/x-octet-stream\n\n");
								fprintf(stdout, "SEQHOUND_OK\n");
								ILinkSetAsnWrite(p3DNeighbours, aip, NULL);
								AsnIoClose(aip);
								bSuccess = TRUE;
		  					} else PostError("SEQHOUND_ERROR", "Unable to open ASN.1 IO.");
							FileClose(fp);
						} else PostError("SEQHOUND_ERROR", "Unable to open file.");
						ILinkSetFree(p3DNeighbours);
					} else {
						PostError("SEQHOUND_NULL", "NULL return value.");
						bSuccess = TRUE;
					}
				} else PostError("SEQHOUND_ERROR", "Function returned error.");				
			} else PostError("SEQHOUND_ERROR", "Invalid function parameter.");
		} else PostError("SEQHOUND_ERROR", "Failed to save parameter values.");
	} else PostError("SEQHOUND_ERROR", "Failed to get parameter values.");
	if(pcgi) MemFree(pcgi);
	if(pceval) MemFree(pceval);

	if(bSuccess == FALSE) {
		ErrPostEx(SEV_ERROR,0,0,"SeqHound3DNeighboursOfGiEx failed.");
		return SLRI_FAIL;
	}
	return SLRI_SUCCESS;
}


SLRI_ERR SeqHound3DNeighboursFromGi(void)
{
	Int4 argshere[2];
	CharPtr pcgi = NULL, pceval = NULL, pcstop = NULL;
	Int4 gi = 0;
	double evalue = 0.0;
	ILinkSetPtr p3DNeighbours = NULL;
	FILE* fp = NULL;
	AsnIoPtr aip = NULL;
	Boolean bSuccess = FALSE;

	if (((argshere[0] = WWWFindName(ginfo,"gi")) >= 0) &&
		((argshere[1] = WWWFindName(ginfo,"evalue")) > 0) ) {

		if(((pcgi = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) &&
			((pceval = StringSave(WWWGetValueByIndex(ginfo,argshere[1]))) != NULL) ) {

			if(((StrToInt4(pcgi,&gi)) > 0) &&
				((evalue = strtod(pceval,&pcstop)) > 0.0) ) {

				if(!SHound3DNeighboursFromGiEx((Int4) gi, &p3DNeighbours, (FloatHi) evalue)) {
					if(p3DNeighbours) {
						if((fp = FileOpen("stdout", FILE_MODE)) != NULL) {
							if((aip=AsnIoNew(ASN_MODE, fp, NULL,NULL,NULL)) != NULL) {
								printf("Content-type: application/x-octet-stream\n\n");
								fprintf(stdout, "SEQHOUND_OK\n");
								ILinkSetAsnWrite(p3DNeighbours, aip, NULL);
								AsnIoClose(aip);
								bSuccess = TRUE;
			  					} else PostError("SEQHOUND_ERROR", "Unable to open ASN.1 IO.");
							FileClose(fp);
						} else PostError("SEQHOUND_ERROR", "Unable to open file.");
						ILinkSetFree(p3DNeighbours);
					} else {
						PostError("SEQHOUND_NULL", "NULL return value.");
						bSuccess = TRUE;
					}
				} else PostError("SEQHOUND_ERROR", "Function returned error.");				
			} else PostError("SEQHOUND_ERROR", "Invalid function parameter.");
		} else PostError("SEQHOUND_ERROR", "Failed to save parameter values.");
	} else PostError("SEQHOUND_ERROR", "Failed to get parameter values.");
	if(pcgi) MemFree(pcgi);
	if(pceval) MemFree(pceval);

	if(bSuccess == FALSE) {
		ErrPostEx(SEV_ERROR,0,0,"SeqHound3DNeighboursOfGi failed.");
		return SLRI_FAIL;
	}
	return SLRI_SUCCESS;
}


SLRI_ERR SeqHound3DNeighboursFromGiPL(void)
{
	Int4 argshere[2];
	CharPtr pcgi = NULL, pceval = NULL, pcstop = NULL;
	Int4 gi = 0;
	double evalue = 0.0;
	ILinkSetPtr p3DNeighbours = NULL;
	Boolean bSuccess = FALSE;
	CharPtr pneig = NULL;

	if (((argshere[0] = WWWFindName(ginfo,"gi")) >= 0) &&
		((argshere[1] = WWWFindName(ginfo,"evalue")) > 0) ) {

		if(((pcgi = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) &&
			((pceval = StringSave(WWWGetValueByIndex(ginfo,argshere[1]))) != NULL) ) {

			if(((StrToInt4(pcgi,&gi)) > 0) &&
				((evalue = strtod(pceval,&pcstop)) > 0.0) ) {

				if(!SHound3DNeighboursFromGiEx((Int4) gi, &p3DNeighbours, (FloatHi) evalue)) {
					if(p3DNeighbours) {
						pneig = ILinkSet2CommaStr(p3DNeighbours);
						if(pneig) {
						       printf("Content-type: text/html\r\n\r\n");
						       fprintf(stdout, "SEQHOUND_OK\n");
						       fprintf(stdout, "%s\n", pneig);
						       bSuccess = TRUE;
						       MemFree(pneig);
						} else PostError("SEQHOUND_ERROR", "ILinkSet2CommaStr failed.");
						ILinkSetFree(p3DNeighbours);
					} else {
						PostError("SEQHOUND_NULL", "NULL return value.");
						bSuccess = TRUE;
					}
				} else PostError("SEQHOUND_ERROR", "Function returned error.");				
			} else PostError("SEQHOUND_ERROR", "Invalid function parameter.");
		} else PostError("SEQHOUND_ERROR", "Failed to save parameter values.");
	} else PostError("SEQHOUND_ERROR", "Failed to get parameter values.");
	if(pcgi) MemFree(pcgi);
	if(pceval) MemFree(pceval);

	if(bSuccess == FALSE) {
		ErrPostEx(SEV_ERROR,0,0,"SeqHound3DNeighboursOfGi failed.");
		return SLRI_FAIL;
	}
	return SLRI_SUCCESS;
}



SLRI_ERR SeqHound3DNeighboursFromGiList(void)
{
	Int4 argshere[2];
	CharPtr pcgilist = NULL, pceval = NULL, pcstop = NULL;
	double evalue = 0.0;
	CharPtr sep = ",";
	ValNodePtr pvngilist = NULL;
	ILinkSetListPtr p3DNeighboursList = NULL;
	ILinkSetList2Ptr pPackage = NULL;
	FILE* fp = NULL;
	AsnIoPtr aip = NULL;
	Boolean bSuccess = FALSE;

	if (((argshere[0] = WWWFindName(ginfo,"gilist")) >= 0) &&
		((argshere[1] = WWWFindName(ginfo,"evalue")) > 0) ) {

		if(((pcgilist = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) &&
			((pceval = StringSave(WWWGetValueByIndex(ginfo,argshere[1]))) != NULL) ) {

			if(((pvngilist = Misc_CommaIntString2ValNodeList(pcgilist, sep)) != NULL) &&
				((evalue = strtod(pceval,&pcstop)) > 0.0) ) {

				if(!SHound3DNeighboursFromGiList(pvngilist, &p3DNeighboursList, (FloatHi) evalue)) {
					if(p3DNeighboursList) {
						if((fp = FileOpen("stdout", FILE_MODE)) != NULL) {
							if((aip=AsnIoNew(ASN_MODE, fp, NULL,NULL,NULL)) != NULL) {
								printf("Content-type: application/x-octet-stream\n\n");
								fprintf(stdout, "SEQHOUND_OK\n");
								pPackage = ILinkSetList2New();
								/* SK: NULL ptr check */
								if ( pPackage != NULL )
								   pPackage->set2 = p3DNeighboursList;
								ILinkSetList2AsnWrite(pPackage, aip, NULL);
								AsnIoClose(aip);
								/* SK: NULL ptr check */
								if ( pPackage != NULL )
								{
								   pPackage->set2 = NULL;
								   pPackage = ILinkSetList2Free(pPackage);
								}
								bSuccess = TRUE;
		  					} else PostError("SEQHOUND_ERROR", "Unable to open ASN.1 IO.");
							FileClose(fp);
						} else PostError("SEQHOUND_ERROR", "Unable to open file.");
						ILinkSetListFree(p3DNeighboursList);
					} else {
						PostError("SEQHOUND_NULL", "NULL return value.");
						bSuccess = TRUE;
					}
				} else PostError("SEQHOUND_ERROR", "Function returned error.");				
			} else PostError("SEQHOUND_ERROR", "Invalid function parameter.");
		} else PostError("SEQHOUND_ERROR", "Failed to save parameter values.");
	} else PostError("SEQHOUND_ERROR", "Failed to get parameter values.");
	if(pvngilist) ValNodeFree(pvngilist);
	if(pcgilist) MemFree(pcgilist);
	if(pceval) MemFree(pceval);

	if(bSuccess == FALSE) {
		ErrPostEx(SEV_ERROR,0,0,"SeqHound3DNeighboursOfGiList failed.");
		return SLRI_FAIL;
	}
	return SLRI_SUCCESS;
}

SLRI_ERR SeqHound3DNeighboursFromGiListPL(void)
{
	Int4 argshere[2];
	CharPtr pcgilist = NULL, pceval = NULL, pcstop = NULL;
	double evalue = 0.0;
	CharPtr sep = ",";
	ValNodePtr pvngilist = NULL;
	ILinkSetListPtr p3DNeighboursList = NULL;
	Boolean bSuccess = FALSE;
	CharPtr pneig = NULL;

	if (((argshere[0] = WWWFindName(ginfo,"gilist")) >= 0) &&
		((argshere[1] = WWWFindName(ginfo,"evalue")) > 0) ) {

		if(((pcgilist = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) &&
			((pceval = StringSave(WWWGetValueByIndex(ginfo,argshere[1]))) != NULL) ) {

			if(((pvngilist = Misc_CommaIntString2ValNodeList(pcgilist, sep)) != NULL) &&
				((evalue = strtod(pceval,&pcstop)) > 0.0) ) {

				if(!SHound3DNeighboursFromGiList(pvngilist, &p3DNeighboursList, (FloatHi) evalue)) {
					if(p3DNeighboursList) {
					        pneig = ILinkSetList2CommaStr(p3DNeighboursList);
						if(pneig) {
							printf("Content-type: text/html\r\n\r\n");
					        	fprintf(stdout, "SEQHOUND_OK\n");
							fprintf(stdout, "%s\n", pneig);
							bSuccess = TRUE;
							MemFree(pneig);
						} else PostError("SEQHOUND_ERROR", "ILinkSetList2CommaStr failed.");	
						ILinkSetListFree(p3DNeighboursList);
					} else {
						PostError("SEQHOUND_NULL", "NULL return value.");
						bSuccess = TRUE;
					}
				} else PostError("SEQHOUND_ERROR", "Function returned error.");				
			} else PostError("SEQHOUND_ERROR", "Invalid function parameter.");
		} else PostError("SEQHOUND_ERROR", "Failed to save parameter values.");
	} else PostError("SEQHOUND_ERROR", "Failed to get parameter values.");
	if(pvngilist) ValNodeFree(pvngilist);
	if(pcgilist) MemFree(pcgilist);
	if(pceval) MemFree(pceval);

	if(bSuccess == FALSE) {
		ErrPostEx(SEV_ERROR,0,0,"SeqHound3DNeighboursOfGiList failed.");
		return SLRI_FAIL;
	}
	return SLRI_SUCCESS;
}


SLRI_ERR SeqHound3DNeighboursFromTaxID(void)
{
	Int4 argshere[2];
	CharPtr pctaxid = NULL, pceval = NULL, pcstop = NULL;
	Int4 taxid = 0;
	double evalue = 0.0;
	ILinkSetListPtr p3DNeighboursList = NULL;
	ILinkSetList2Ptr pPackage = NULL;
	FILE* fp = NULL;
	AsnIoPtr aip = NULL;
	Boolean bSuccess = FALSE;

	if (((argshere[0] = WWWFindName(ginfo,"taxid")) >= 0) &&
		((argshere[1] = WWWFindName(ginfo,"evalue")) > 0) ) { 	

		if(((pctaxid = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) &&
			((pceval = StringSave(WWWGetValueByIndex(ginfo,argshere[1]))) != NULL) ) {

			if(((StrToInt4(pctaxid,&taxid)) > 0) &&
				((evalue = strtod(pceval,&pcstop)) > 0.0) ) {
		
				if(!SHound3DNeighboursFromTaxID(taxid, &p3DNeighboursList, (FloatHi) evalue)) {
					if(p3DNeighboursList) {
						if((fp = FileOpen("stdout", FILE_MODE)) != NULL) {					
							if((aip=AsnIoNew(ASN_MODE, fp, NULL,NULL,NULL)) != NULL) {
								printf("Content-type: application/x-octet-stream\n\n");
								fprintf(stdout, "SEQHOUND_OK\n");
								pPackage = ILinkSetList2New();
								/* SK: NULL ptr check */
								if ( pPackage != NULL )
								   pPackage->set2 = p3DNeighboursList;
								ILinkSetList2AsnWrite(pPackage, aip, NULL);
								AsnIoClose(aip);
								/* SK: NULL ptr check */
								if ( pPackage != NULL )
								{
								   pPackage->set2 = NULL;
								   pPackage = ILinkSetList2Free(pPackage);
								}
								AsnIoClose(aip);
								bSuccess = TRUE;
		  					} else PostError("SEQHOUND_ERROR", "Unable to open ASN.1 IO.");
							FileClose(fp);
						} else PostError("SEQHOUND_ERROR", "Unable to open file.");
						ILinkSetListFree(p3DNeighboursList);
					} else {
						PostError("SEQHOUND_NULL", "NULL return value.");
						bSuccess = TRUE;
					}
				} else PostError("SEQHOUND_ERROR", "Function returned error.");				
			} else PostError("SEQHOUND_ERROR", "Invalid function parameter.");
		} else PostError("SEQHOUND_ERROR", "Failed to save parameter values.");
	} else PostError("SEQHOUND_ERROR", "Failed to get parameter values.");
	if(pctaxid) MemFree(pctaxid);
	if(pceval) MemFree(pceval);

	if(bSuccess == FALSE) {
		ErrPostEx(SEV_ERROR,0,0,"SeqHound3DNeighboursFromTaxID failed.");
		return SLRI_FAIL;
	}
	return SLRI_SUCCESS;
}


SLRI_ERR SeqHound3DNeighboursFromTaxIDPL(void)
{
	Int4 argshere[2];
	CharPtr pctaxid = NULL, pceval = NULL, pcstop = NULL;
	Int4 taxid = 0;
	double evalue = 0.0;
	ILinkSetListPtr p3DNeighboursList = NULL;
	Boolean bSuccess = FALSE;
	CharPtr pneig = NULL;

	if (((argshere[0] = WWWFindName(ginfo,"taxid")) >= 0) &&
		((argshere[1] = WWWFindName(ginfo,"evalue")) > 0) ) { 	

		if(((pctaxid = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) &&
			((pceval = StringSave(WWWGetValueByIndex(ginfo,argshere[1]))) != NULL) ) {

			if(((StrToInt4(pctaxid,&taxid)) > 0) &&
				((evalue = strtod(pceval,&pcstop)) > 0.0) ) {
		
				if(!SHound3DNeighboursFromTaxID(taxid, &p3DNeighboursList, (FloatHi) evalue)) {
					if(p3DNeighboursList) {
						pneig = ILinkSetList2CommaStr(p3DNeighboursList);
						if(pneig) {
							printf("Content-type: text/html\r\n\r\n");
					        	fprintf(stdout, "SEQHOUND_OK\n");
							fprintf(stdout, "%s\n", pneig);
							bSuccess = TRUE;
							MemFree(pneig);
						} else PostError("SEQHOUND_ERROR", "ILinkSetList2CommaStr failed.");	
						ILinkSetListFree(p3DNeighboursList);
					} else {
						PostError("SEQHOUND_NULL", "NULL return value.");
						bSuccess = TRUE;
					}
				} else PostError("SEQHOUND_ERROR", "Function returned error.");				
			} else PostError("SEQHOUND_ERROR", "Invalid function parameter.");
		} else PostError("SEQHOUND_ERROR", "Failed to save parameter values.");
	} else PostError("SEQHOUND_ERROR", "Failed to get parameter values.");
	if(pctaxid) MemFree(pctaxid);
	if(pceval) MemFree(pceval);

	if(bSuccess == FALSE) {
		ErrPostEx(SEV_ERROR,0,0,"SeqHound3DNeighboursFromTaxID failed.");
		return SLRI_FAIL;
	}
	return SLRI_SUCCESS;
}


SLRI_ERR SeqHound3DListFromTaxID(void)
{
	Int4 argshere[2];
	CharPtr pctaxid = NULL;
	Int4 taxid = 0;
	ILinkSetPtr p3DList = NULL;
	FILE* fp = NULL;
	AsnIoPtr aip = NULL;
	Boolean bSuccess = FALSE;

	if((argshere[0] = WWWFindName(ginfo,"taxid")) >= 0) {
		if(((pctaxid = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) ) {
			if(StrToInt4(pctaxid,&taxid) > 0) {				
				if(!SHound3DListFromTaxID(taxid, &p3DList)) {
					if(p3DList != NULL) {
						if((fp = FileOpen("stdout", FILE_MODE)) != NULL) {					
							if((aip=AsnIoNew(ASN_MODE, fp, NULL,NULL,NULL)) != NULL) {
								printf("Content-type: application/x-octet-stream\n\n");
								fprintf(stdout, "SEQHOUND_OK\n");
								ILinkSetAsnWrite(p3DList, aip, NULL);
								AsnIoClose(aip);
								bSuccess = TRUE;
		  					} else PostError("SEQHOUND_ERROR", "Unable to open ASN.1 IO.");
							FileClose(fp);
						} else PostError("SEQHOUND_ERROR", "Unable to open file.");
						ILinkSetFree(p3DList);
					} else {
						PostError("SEQHOUND_NULL", "NULL return value.");
						bSuccess = TRUE;
					}
				} else PostError("SEQHOUND_ERROR", "Function returned error.");				
			} else PostError("SEQHOUND_ERROR", "Invalid function parameter.");
		} else PostError("SEQHOUND_ERROR", "Failed to save parameter values.");
	} else PostError("SEQHOUND_ERROR", "Failed to get parameter values.");
	if(pctaxid) MemFree(pctaxid);

	if(bSuccess == FALSE) {
		ErrPostEx(SEV_ERROR,0,0,"SeqHound3DListFromTaxID failed.");
		return SLRI_FAIL;
	}
	return SLRI_SUCCESS;
}


SLRI_ERR SeqHound3DListFromTaxIDPL(void)
{
	Int4 argshere[2];
	CharPtr pctaxid = NULL;
	Int4 taxid = 0;
	ILinkSetPtr p3DList = NULL;
	Boolean bSuccess = FALSE;
	CharPtr pneig = NULL;

	if((argshere[0] = WWWFindName(ginfo,"taxid")) >= 0) {
		if(((pctaxid = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) ) {
			if(StrToInt4(pctaxid,&taxid) > 0) {				
				if(!SHound3DListFromTaxID(taxid, &p3DList)) {
					if(p3DList != NULL) {
						pneig = ILinkSet2CommaStr(p3DList);
						if(pneig) {
							printf("Content-type: text/html\r\n\r\n");
					        	fprintf(stdout, "SEQHOUND_OK\n");
							fprintf(stdout, "%s\n", pneig);
							bSuccess = TRUE;
							MemFree(pneig);
						} else PostError("SEQHOUND_ERROR", "ILinkSet2CommaStr failed.");	
						ILinkSetFree(p3DList);
					} else {
						PostError("SEQHOUND_NULL", "NULL return value.");
						bSuccess = TRUE;
					}
				} else PostError("SEQHOUND_ERROR", "Function returned error.");				
			} else PostError("SEQHOUND_ERROR", "Invalid function parameter.");
		} else PostError("SEQHOUND_ERROR", "Failed to save parameter values.");
	} else PostError("SEQHOUND_ERROR", "Failed to get parameter values.");
	if(pctaxid) MemFree(pctaxid);

	if(bSuccess == FALSE) {
		ErrPostEx(SEV_ERROR,0,0,"SeqHound3DListFromTaxID failed.");
		return SLRI_FAIL;
	}
	return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundSequenceLength(void)
{
  CharPtr pcThis = NULL;
  CharPtr giptr = NULL;
  Int4 seqlen=-1;
  Int4 IndexArgs = -1;
  Int4 gival = 0;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"gi")) >= 0)
    {
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       giptr = StringSave(pcThis);
    }
  if (giptr == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SeqHoundSequenceLength: Failed to get parameter value.");
       fprintf(stdout, "SeqHoundSequenceLength: SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
     }
  if(!StrToInt4 (giptr, &gival))
   {
      ErrPostEx(SEV_ERROR,0,0, "SeqHoundSequenceLength: Invalid function parameter.");
      fprintf(stdout, "SeqHoundSequenceLength: SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(giptr);
      return SLRI_FAIL;
   }
  seqlen = SHoundSequenceLength(gival);
  fprintf(stdout, "SEQHOUND_OK\n");
  fprintf(stdout, "%ld\n", (long) seqlen);
  MemFree(giptr);
  return SLRI_SUCCESS;
}


SLRI_ERR SeqHoundGiAndNumNeighboursList(void)
{
	ILinkSetPtr ilsp = NULL;
	FILE* fp = NULL;
	AsnIoPtr aip = NULL;
	Boolean bSuccess = FALSE;

	if(!SHoundGiAndNumNeighboursList(&ilsp)) {
		if(ilsp != NULL) {
			if((fp = FileOpen("stdout", FILE_MODE)) != NULL) {
				if((aip=AsnIoNew(ASN_MODE, fp, NULL,NULL,NULL)) != NULL) {
					printf("Content-type: application/x-octet-stream\n\n");
					fprintf(stdout, "SEQHOUND_OK\n");
					ILinkSetAsnWrite(ilsp, aip, NULL);
					AsnIoClose(aip);
					bSuccess = TRUE;	
			  	} else PostError("SEQHOUND_ERROR", "Unable to open ASN.1 IO.");
				FileClose(fp);
			} else PostError("SEQHOUND_ERROR", "Unable to open file.");
			ILinkSetFree(ilsp);
		} else PostError("SEQHOUND_NULL", "NULL return value.");
	} else PostError("SEQHOUND_ERROR", "Function returned error.");				

	if(bSuccess == FALSE) {
		ErrPostEx(SEV_ERROR,0,0,"SeqHoundGiAndNumNeighboursList failed.");
		return SLRI_FAIL;
	}
	return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundNumNeighboursInDB(void)
{
	Int4 num = 0;
	FILE* fp = NULL;
	Boolean bSuccess = FALSE;

	if(!SHoundNumNeighboursInDB(&num)) {
		if(num > 0) {
			if((fp = FileOpen("stdout", FILE_MODE)) != NULL) {
				printf("Content-type: application/x-octet-stream\n\n");
				fprintf(stdout, "SEQHOUND_OK\n");
				fprintf(stdout, "%ld\n",(long)num); 			
				bSuccess = TRUE;
				FileClose(fp);
			} else PostError("SEQHOUND_ERROR", "Unable to open file.");
		} else PostError("SEQHOUND_NULL", "NULL return value.");
	} else PostError("SEQHOUND_ERROR", "Function returned error.");				

	if(bSuccess == FALSE) {
		ErrPostEx(SEV_ERROR,0,0,"SHoundNumNeighboursInDB failed.");
		return SLRI_FAIL;
	}
	return SLRI_SUCCESS;
}


/*retrieve a list of goid from a locus link id */
SLRI_ERR SeqHoundGOIDFromLLID(void)
{
  Int4 argshere[2];
  Int4 llid=0;
  CharPtr pstr = NULL, pcllid = NULL;
  ValNodePtr pvngolist=NULL;
  
  printf("Content-type: text/html\r\n\r\n");
  argshere[0] = WWWFindName(ginfo, "llid");
  if (argshere[0] >=0)
  {
    pcllid = StringSave(WWWGetValueByIndex(ginfo, argshere[0]));
  }
  if(StrToInt4(pcllid, &llid) <= 0)
  {
    ErrPostEx(SEV_ERROR,0,0,"SHoundGOIDFromLLID:failed to get parameter value.");
    fprintf(stdout, "SEQHOUND_ERROR SHoundGOIDFromLLID failed ot get parameter value.\n");
    if(pcllid)  MemFree(pcllid);
    return SLRI_FAIL; 
  }
  pvngolist=SHoundGOIDFromLLID(llid);
  if (pvngolist == NULL)
  {
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundGOIDFromLLID return NULL.");
    fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
    MemFree(pcllid);
    return SLRI_FAIL;
  }
  pstr=Misc_SaveValNodeListAsString(pvngolist);
  fprintf(stdout, "SEQHOUND_OK\n");
  if (pstr!=NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf(stdout, "(null)\n");
 
  if (pstr) MemFree(pstr);
  if (pcllid) MemFree(pcllid);
  if (pvngolist) ValNodeFree(pvngolist);
 
  return SLRI_SUCCESS;
}

/*retrive an OMIM id from a locus link id*/
SLRI_ERR SeqHoundOMIMFromLLID(void)
{
  Int4 argshere[2];
  Int4 llid=0, omimid = 0;
  CharPtr pcllid = NULL;
  
  printf("Content-type: text/html\r\n\r\n");
  argshere[0] = WWWFindName(ginfo, "llid");
  if (argshere[0] >=0)
  {
    pcllid = StringSave(WWWGetValueByIndex(ginfo, argshere[0]));
  }
  if(StrToInt4(pcllid, &llid) <= 0)
  {
    ErrPostEx(SEV_ERROR,0,0,"SHoundOMIMFromLLID:failed to get parameter value.");
    fprintf(stdout, "SEQHOUND_ERROR SHoundOMIMFromLLID failed ot get parameter value.\n");
    if(pcllid)  MemFree(pcllid);
    return SLRI_FAIL; 
  }
  omimid =SHoundOMIMFromLLID(llid);
  if (omimid == 0)
  {
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundOMIMFromLLID return NULL.");
    fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
    MemFree(pcllid);
    return SLRI_FAIL;
  }
  fprintf(stdout, "SEQHOUND_OK\n");
  fprintf(stdout, "%ld\n", (long) omimid);
 
  if (pcllid) MemFree(pcllid);
  return SLRI_SUCCESS;
}

/*retrieves a list of Conserved domain ids from a locus link id*/
SLRI_ERR SeqHoundCDDIDFromLLID(void)
{
  Int4 argshere[2];
  Int4 llid=0;
  CharPtr pstr = NULL, pcllid = NULL;
  ValNodePtr pvncddlist=NULL;
  
  printf("Content-type: text/html\r\n\r\n");
  argshere[0] = WWWFindName(ginfo, "llid");
  if (argshere[0] >=0)
  {
    pcllid = StringSave(WWWGetValueByIndex(ginfo, argshere[0]));
  }
  if(StrToInt4(pcllid, &llid) <= 0)
  {
    ErrPostEx(SEV_ERROR,0,0,"SHoundCDDIDFromLLID:failed to get parameter value.");
    fprintf(stdout, "SEQHOUND_ERROR SHoundCDDIDFromLLID failed ot get parameter value.\n");
    if(pcllid)  MemFree(pcllid);
    return SLRI_FAIL; 
  }
  pvncddlist=SHoundCDDIDFromLLID(llid);
  if (pvncddlist == NULL)
  {
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundCDDIDFromLLID return NULL.");
    fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
    MemFree(pcllid);
    return SLRI_FAIL;
  }
  pstr=Misc_SaveStrValNodeListAsString(pvncddlist);
  fprintf(stdout, "SEQHOUND_OK\n");
  if (pstr!=NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf(stdout, "(null)\n");
 
  if (pstr) MemFree(pstr);
  if (pcllid) MemFree(pcllid);
  if (pvncddlist) ValNodeFreeData(pvncddlist);
 
  return SLRI_SUCCESS;
}

/*retrieves a list of locus link ids from a goid and an evidence code */
SLRI_ERR SeqHoundLLIDFromGOIDAndECode(void)
{
  Int4 argshere[2];
  Int4 goid=0;
  CharPtr pcgoid = NULL, pstr = NULL, pecode = NULL;
  ValNodePtr vnpllid = NULL;
 
  printf("Content-type: text/html\r\n\r\n");
  argshere[0] = WWWFindName(ginfo,"goid");
  argshere[1] = WWWFindName(ginfo,"ecode");
  if (argshere[0] >=0 && argshere[1] >=0)
  {
    pcgoid = StringSave(WWWGetValueByIndex(ginfo, argshere[0]));
    pecode = StringSave(WWWGetValueByIndex(ginfo, argshere[1]));
  } 
  if( pcgoid == NULL || pecode == NULL || strlen(pecode) == 0)
  {
    ErrPostEx(SEV_ERROR,0,0, "SHoundLLIDFromGOIDAndECode Failed to get parameter value.");
    fprintf(stdout, "SEQHOUND_ERROR SHoundLLIDFromGOIDAndECode Failed to get parameter value.\n");
    if (pcgoid) MemFree (pcgoid);
    if (pecode) MemFree (pecode);
    return SLRI_FAIL;
  }	
  if( StrToInt4(pcgoid,&goid) <=0) 
  {
    ErrPostEx(SEV_ERROR,0,0, "SHoundLLIDFromGOIDAndECode StrInt4 failed.");
    fprintf(stdout, "SEQHOUND_ERROR SHoundLLIDFromGOIDAndECode StrToInt4 failed.\n");
    if (pcgoid) MemFree (pcgoid);
    if (pecode) MemFree (pecode);
    return SLRI_FAIL;
  }
  vnpllid = SHoundLLIDFromGOIDAndECode(goid, pecode);
  if(vnpllid == NULL)
  {
    MemFree(pcgoid);
    MemFree(pecode);
    ErrPostEx(SEV_ERROR,0,0, "SHoundLLIDFromGOIDAndECode returned a null list.");
    fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
    return SLRI_FAIL;
  }
  pstr = Misc_SaveValNodeListAsString(vnpllid);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf(stdout, "(null)\n");
	
  if(pcgoid) MemFree(pcgoid);
  if(pecode) MemFree(pecode);
  if(pstr)   MemFree(pstr);
  if(vnpllid) ValNodeFree(vnpllid);
  
  return SLRI_SUCCESS;
}


/*retrieves a list of locus link ids from OMIM id*/
SLRI_ERR SeqHoundLLIDFromOMIM(void)
{
  Int4 argshere[2];
  Int4 omimid=0;
  CharPtr pstr = NULL, pcomimid = NULL;
  ValNodePtr pvnllidlist=NULL;
  
  printf("Content-type: text/html\r\n\r\n");
  argshere[0] = WWWFindName(ginfo, "omimid");
  if (argshere[0] >=0)
  {
    pcomimid = StringSave(WWWGetValueByIndex(ginfo, argshere[0]));
  }
  if(StrToInt4(pcomimid, &omimid) <= 0)
  {
    ErrPostEx(SEV_ERROR,0,0,"SHoundLLIDFromOMIM:failed to get parameter value.");
    fprintf(stdout, "SEQHOUND_ERROR SHoundLLIDFromOMIM failed ot get parameter value.\n");
    if(pcomimid)  MemFree(pcomimid);
    return SLRI_FAIL; 
  }
  pvnllidlist=SHoundLLIDFromOMIM(omimid);
  if (pvnllidlist == NULL)
  {
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundLLIDFromOMIM return NULL.");
    fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
    MemFree(pcomimid);
    return SLRI_FAIL;
  }
  pstr=Misc_SaveValNodeListAsString(pvnllidlist);
  fprintf(stdout, "SEQHOUND_OK\n");
  if (pstr!=NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf(stdout, "(null)\n");
 
  if (pstr) MemFree(pstr);
  if (pcomimid) MemFree(pcomimid);
  if (pvnllidlist) ValNodeFree(pvnllidlist);
 
  return SLRI_SUCCESS;
}
 
 
/*retrieves a list of LLID from a conserved domain id*/
SLRI_ERR SeqHoundLLIDFromCDDID(void)
{
  Int4 argshere[2];
  CharPtr pstr = NULL, pccddid = NULL;
  ValNodePtr pvnllidlist=NULL;
  
  printf("Content-type: text/html\r\n\r\n");
  argshere[0] = WWWFindName(ginfo, "cddid");
  if (argshere[0] >=0)
  {
    pccddid = StringSave(WWWGetValueByIndex(ginfo, argshere[0]));
  }
  pvnllidlist=SHoundLLIDFromCDDID(pccddid);
  if (pvnllidlist == NULL)
  {
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundLLIDFromCDDID return NULL.");
    fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
    MemFree(pccddid);
    return SLRI_FAIL;
  }
  pstr=Misc_SaveValNodeListAsString(pvnllidlist);
  fprintf(stdout, "SEQHOUND_OK\n");
  if (pstr!=NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf(stdout, "(null)\n");
 
  if (pstr) MemFree(pstr);
  if (pccddid) MemFree(pccddid);
  if (pvnllidlist) ValNodeFree(pvnllidlist);
 
  return SLRI_SUCCESS;
} 

SLRI_ERR SeqHoundGiFromGOID(void)
{
  Int4 argshere[2];
  CharPtr pcgoid = NULL, pstr = NULL;
  Int4 goid = 0;
  ValNodePtr vnpgi= NULL;
  Boolean bSuccess = FALSE;

  printf("Content-type: text/html\r\n\r\n");
  if ((argshere[0] = WWWFindName(ginfo,"goid")) >= 0) {
    if(((pcgoid = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) ) {
      if(StrToInt4(pcgoid,&goid) > 0) {
	
	vnpgi = SHoundGiFromGOID(goid);
	if(vnpgi != NULL) {
	  pstr = Misc_SaveValNodeListAsString(vnpgi);
	
	  fprintf(stdout, "SEQHOUND_OK\n");
	  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
	  else fprintf(stdout, "(null)\n");
	  MemFree(pstr);
	  bSuccess = TRUE;
	} else {
	  ErrPostEx(SEV_ERROR,0,0, "SHoundGiFromGOID: vnpgi is NULL.");
	  fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
	}
      } else {
	ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
	fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      }
    } else {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
    }
  } else {
    ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter values.");
    fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter values.\n");
  }
  if(vnpgi) ValNodeFree(vnpgi);
  if(pcgoid) MemFree(pcgoid);

  if(!bSuccess) {
    ErrPostEx(SEV_ERROR,0,0,"SeqHoundGiFromGOID failed.");
    return SLRI_FAIL;
  }
  return SLRI_SUCCESS;
}


/* returns the gi with the specified goid and evidence code. Consult
 * www.geneontology.org for valid evidence code, eg "ND", "IPI" etc.
 * return value is a list of gi with redundant */
SLRI_ERR SeqHoundGiFromGOIDAndECode(void)
{
  Int4 argshere[2];
  Int4 goid=0;
  CharPtr pcgoid = NULL, pstr = NULL, pecode = NULL;
  ValNodePtr vnpgi = NULL;
  Boolean bSuccess = FALSE;

  printf("Content-type: text/html\r\n\r\n");
  if (((argshere[0] = WWWFindName(ginfo,"goid")) >= 0) &&
	  ((argshere[1] = WWWFindName(ginfo,"ecode")) >=0) ){
   if( ((pcgoid = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL)
	 &&((pecode = StringSave(WWWGetValueByIndex(ginfo,argshere[1]))) != NULL)
	 && (strlen(pecode)!=0) ) {
     if( StrToInt4(pcgoid,&goid) >0) {
	   vnpgi = SHoundGiFromGOIDAndECode(goid, pecode);
	   if(vnpgi != NULL) {
	    pstr = Misc_SaveValNodeListAsString(vnpgi);
	    fprintf(stdout, "SEQHOUND_OK\n");
	    if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
	    else fprintf(stdout, "(null)\n");
	    MemFree(pstr);
	    bSuccess = TRUE;
	   } else {
	   ErrPostEx(SEV_ERROR,0,0, "SHoundGiFromGOIDAndECode returned a null list.");
	   fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
	   }
	 } else {
	   ErrPostEx(SEV_ERROR,0,0, "SHoundGiFromGOIDAndECode StrInt4 failed.");
	   fprintf(stdout, "SEQHOUND_ERROR SHoundGiFromGOIDAndECode StrToInt4 failed.\n");
	 }					
    } else {
      ErrPostEx(SEV_ERROR,0,0, "SHoundGiFromGOIDAndECode Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR SHoundGiFromGOIDAndECode Failed to get parameter value.\n");
    }
  } else {
    ErrPostEx(SEV_ERROR,0,0, "SHoundGiFromGOIDAndECode Failed to get parameter values.");
    fprintf(stdout, "SEQHOUND_ERROR SHoundGiFromGOIDAndECode Failed to get parameter values.\n");
  }
  if(pcgoid) MemFree(pcgoid);
  if(pecode) MemFree(pecode);
  if(vnpgi) ValNodeFree(vnpgi);
  if(!bSuccess) {
    ErrPostEx(SEV_ERROR,0,0,"SHoundGiFromGOIDAndECode failed.");
    return SLRI_FAIL;
  }
  return SLRI_SUCCESS;
}



SLRI_ERR SeqHoundGiFromGOIDList(void)
{
  Int4 argshere[2];
  CharPtr pcgolist = NULL, pstr = NULL;
  ValNodePtr vnpgo = NULL, vnpgi = NULL;
  Boolean bSuccess = FALSE;

  printf("Content-type: text/html\r\n\r\n");
  if (((argshere[0] = WWWFindName(ginfo,"vnpgo")) >= 0)) {

    if( ((pcgolist = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) ) {

      if( ((vnpgo = Misc_CommaIntString2ValNodeList(pcgolist, ",")) != NULL) ) {
	
	vnpgi = SHoundGiFromGOIDList(vnpgo);
	if(vnpgi != NULL) {
	  pstr = Misc_SaveValNodeListAsString(vnpgi);
	  fprintf(stdout, "SEQHOUND_OK\n");
	  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
	  else fprintf(stdout, "(null)\n");
	  MemFree(pstr);
	  bSuccess = TRUE;
	} else {
	  ErrPostEx(SEV_ERROR,0,0, "SHoundGiFromGOIDList returned a null list.");
	  fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
	}
      } else {
	ErrPostEx(SEV_ERROR,0,0, "Failed to save as a valnodelist.");
	fprintf(stdout, "SEQHOUND_ERROR Failed to save as a ValNodeList.\n");
      }					
    } else {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
    }
  } else {
    ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter values.");
    fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter values.\n");
  }
  if(pcgolist) MemFree(pcgolist);
  if(vnpgo) ValNodeFree(vnpgo);
  if(vnpgi) ValNodeFree(vnpgi);
  if(!bSuccess) {
    ErrPostEx(SEV_ERROR,0,0,"SeqHoundGiFromGOIDList failed.");
    return SLRI_FAIL;
  }
  return SLRI_SUCCESS;
}


/* return all the gis for given goid list and the evidence code*/
SLRI_ERR SeqHoundGiFromGOIDListAndECode(void){

  Int4 argshere[2];
  CharPtr pgolist = NULL, pecode = NULL;
  ValNodePtr pvngolist=NULL, pvngilist=NULL;
  CharPtr pstr = NULL;
  
  printf("Content-type: text/html\r\n\r\n");
  argshere[0] = WWWFindName(ginfo, "golist");
  argshere[1] = WWWFindName(ginfo, "ecode");

  if (argshere[0] >=0 && argshere[1] >=0)
  {
    pgolist = StringSave(WWWGetValueByIndex(ginfo, argshere[0]));
    pecode  = StringSave(WWWGetValueByIndex(ginfo, argshere[1]));
  }
  if (pgolist == NULL || pecode == NULL || strlen(pecode)==0 )
  {
    ErrPostEx(SEV_ERROR,0,0,"SHoundGiFromGOIDListAndECodet:failed to get parameter value.");
    fprintf(stdout, "SEQHOUND_ERROR SHoundGiFromGOIDListAndECode failed ot get parameter value.\n");
    if(pgolist) MemFree(pgolist);
	if(pecode)  MemFree(pecode);
	return SLRI_FAIL; 
  }
  pvngolist = Misc_CommaIntString2ValNodeList(pgolist, ",");
  if (pvngolist ==NULL )
  {
    ErrPostEx(SEV_ERROR, 0, 0, "[SHoundGiFromGOIDListAndECode]:Misc_CommaIntString2ValNodeList failed.");
    fprintf(stdout," SEQHOUND_ERROR SHoundGiFromGOIDListAndECode: SEQHOUND_ERROR Misc_CommaIntString2ValNodeList failed.\n");
    MemFree(pgolist);
    MemFree(pecode);
    return SLRI_FAIL;
  }
  pvngilist=SHoundGiFromGOIDListAndECode(pvngolist, pecode);
  if (pvngilist == NULL)
  {
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundGiFromGOIDListAndECode return NULL.");
  }
  pstr=Misc_SaveValNodeListAsString(pvngilist);
  fprintf(stdout, "SEQHOUND_OK\n");
  if (pstr!=NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf(stdout, "(null)\n");
 
  if (pstr) MemFree(pstr);
  if (pgolist) MemFree(pgolist);
  if (pecode)  MemFree(pecode);
  if (pvngolist) ValNodeFree(pvngolist);
  if (pvngilist) ValNodeFree(pvngilist);
 
  return SLRI_SUCCESS;
}


SLRI_ERR SeqHoundGOIDFromGi(void)
{
  Int4 argshere[2];
  CharPtr pcgi = NULL, pstr = NULL;
  Int4 gi = 0;
  ValNodePtr vnpgoid= NULL;
  Boolean bSuccess = FALSE;

  printf("Content-type: text/html\r\n\r\n");
  if ((argshere[0] = WWWFindName(ginfo,"gi")) >= 0) {
    if(((pcgi = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) ) {
      if(StrToInt4(pcgi,&gi) > 0) {

	vnpgoid = SHoundGOIDFromGi(gi);
	if(vnpgoid != NULL) {
	  pstr = Misc_SaveValNodeListAsString(vnpgoid);

	  fprintf(stdout, "SEQHOUND_OK\n");
	  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
	  else fprintf(stdout, "(null)\n");
	  MemFree(pstr);
	  bSuccess = TRUE;
	} else {
	  ErrPostEx(SEV_ERROR,0,0, "SHoundGOIDFromGi: vnpgoid is NULL.");
	  fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
	}
      } else {
	ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
	fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      }
    } else {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
    }
  } else {
    ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter values.");
    fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter values.\n");
  }
  if(vnpgoid) ValNodeFree(vnpgoid);
  if(pcgi) MemFree(pcgi);

  if(!bSuccess) {
    ErrPostEx(SEV_ERROR,0,0,"SeqHoundGOIDFromGi failed.");
    return SLRI_FAIL;
  }
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGOIDFromRedundantGi(void)
{
  Int4 argshere[2];
  CharPtr pcgi = NULL, pstr = NULL;
  Int4 gi = 0;
  ValNodePtr vnpgoid= NULL;
  Boolean bSuccess = FALSE;

  printf("Content-type: text/html\r\n\r\n");
  if ((argshere[0] = WWWFindName(ginfo,"gi")) >= 0) {
    if(((pcgi = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) ) {
      if(StrToInt4(pcgi,&gi) > 0) {

	vnpgoid = SHoundGOIDFromRedundantGi(gi);
	if(vnpgoid != NULL) {
	  pstr = Misc_SaveValNodeListAsString(vnpgoid);

	  fprintf(stdout, "SEQHOUND_OK\n");
	  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
	  else fprintf(stdout, "(null)\n");
	  MemFree(pstr);
	  bSuccess = TRUE;
	} else {
	  ErrPostEx(SEV_ERROR,0,0, "SHoundGOIDFromRedundantGi: vnpgoid is NULL.");
	  fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
	}
      } else {
	ErrPostEx(SEV_ERROR,0,0, "SHoundGOIDFromRedundantGi: Invalid function parameter.");
	fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      }
    } else {
      ErrPostEx(SEV_ERROR,0,0, "SHoundGOIDFromRedundantGi: Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
    }
  } else {
    ErrPostEx(SEV_ERROR,0,0, "SHoundGOIDFromRedundantGi: Failed to get parameter values.");
    fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter values.\n");
  }
  if(vnpgoid) ValNodeFree(vnpgoid);
  if(pcgi) MemFree(pcgi);

  if(!bSuccess) {
    ErrPostEx(SEV_ERROR,0,0,"SeqHoundGOIDFromRedundantGi failed.");
    return SLRI_FAIL;
  }
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGOIDFromGiList(void)
{
  Int4 argshere[2];
  CharPtr pcgilist = NULL, pstr = NULL;
  ValNodePtr vnpgo = NULL, vnpgi = NULL;
  Boolean bSuccess = FALSE;

  printf("Content-type: text/html\r\n\r\n");
  if (((argshere[0] = WWWFindName(ginfo,"vnpgi")) >= 0)) {

    if( ((pcgilist = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) ) {

      if( ((vnpgi = Misc_CommaIntString2ValNodeList(pcgilist, ",")) != NULL) ) {
	
	vnpgo = SHoundGOIDFromGiList(vnpgi);
	if(vnpgo != NULL) {
	  pstr = Misc_SaveValNodeListAsString(vnpgo);
	  fprintf(stdout, "SEQHOUND_OK\n");
	  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
	  else fprintf(stdout, "(null)\n");
	  MemFree(pstr);
	  bSuccess = TRUE;
	} else {
	  ErrPostEx(SEV_ERROR,0,0, "SHoundGOIDFromGiList returned a null list.");
	  fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
	}
      } else {
	ErrPostEx(SEV_ERROR,0,0, "Failed to save as a valnodelist.");
	fprintf(stdout, "SEQHOUND_ERROR Failed to save as a ValNodeList.\n");
      }
    } else {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
    }
  } else {
    ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter values.");
    fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter values.\n");
  }
  if(pcgilist) MemFree(pcgilist);
  if(vnpgo) ValNodeFree(vnpgo);
  if(vnpgi) ValNodeFree(vnpgi);
  if(!bSuccess) {
    ErrPostEx(SEV_ERROR,0,0,"SeqHoundGOIDFromGiList failed.");
    return SLRI_FAIL;
  }
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGOIDFromRedundantGiList(void)
{
  Int4 argshere[2];
  CharPtr pcgilist = NULL, pstr = NULL;
  ValNodePtr vnpgo = NULL, vnpgi = NULL;
  Boolean bSuccess = FALSE;

  printf("Content-type: text/html\r\n\r\n");
  if (((argshere[0] = WWWFindName(ginfo,"vnpgi")) >= 0)) {

    if( ((pcgilist = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) ) {

      if( ((vnpgi = Misc_CommaIntString2ValNodeList(pcgilist, ",")) != NULL) ) {

	vnpgo = SHoundGOIDFromRedundantGiList(vnpgi);
	if(vnpgo != NULL) {
	  pstr = Misc_SaveValNodeListAsString(vnpgo);
	  fprintf(stdout, "SEQHOUND_OK\n");
	  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
	  else fprintf(stdout, "(null)\n");
	  MemFree(pstr);
	  bSuccess = TRUE;
	} else {
	  ErrPostEx(SEV_ERROR,0,0, "SHoundGOIDFromGiList returned a null list.");
	  fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
	}
      } else {
	ErrPostEx(SEV_ERROR,0,0, "Failed to save as a valnodelist.");
	fprintf(stdout, "SEQHOUND_ERROR Failed to save as a ValNodeList.\n");
      }
    } else {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
    }
  } else {
    ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter values.");
    fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter values.\n");
  }
  if(pcgilist) MemFree(pcgilist);
  if(vnpgo) ValNodeFree(vnpgo);
  if(vnpgi) ValNodeFree(vnpgi);
  if(!bSuccess) {
    ErrPostEx(SEV_ERROR,0,0,"SeqHoundGOIDFromGiList failed.");
    return SLRI_FAIL;
  }
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGOECodeFromGiAndGOID(void)
{
  Int4 argshere[2];
  CharPtr pcgi = NULL, pcgoid=NULL, pstr = NULL;
  Int4 gi = 0, goid=0;
  ValNodePtr vnpecode = NULL;
  Boolean bSuccess = FALSE;

  printf("Content-type: text/html\r\n\r\n");
  if (  ((argshere[0] = WWWFindName(ginfo,"gi")) > 0)
      &&((argshere[1] = WWWFindName(ginfo,"goid")) > 0) )
  {
    if( ((pcgi = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL)
      &&((pcgoid=StringSave(WWWGetValueByIndex(ginfo,argshere[1]))) != NULL) )
    {
      if(StrToInt4(pcgi,&gi) > 0 && StrToInt4(pcgoid, &goid) >0 ) {
	
	vnpecode = SHoundGOECodeFromGiAndGOID(gi, goid);
	if(vnpecode != NULL) {
	  pstr = Misc_SaveStrValNodeListAsString(vnpecode);
	
	  fprintf(stdout, "SEQHOUND_OK\n");
	  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
	  else fprintf(stdout, "(null)\n");
	  MemFree(pstr);
	  bSuccess = TRUE;
	} else {
	  ErrPostEx(SEV_ERROR,0,0, "SHoundGOECodeFromGiAndGOID: vnpecode is NULL.");
	  fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
	}
      } else {
	ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
	fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      }
    } else {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
    }
  } else {
    ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter values.");
    fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter values.\n");
  }
  if(vnpecode) ValNodeFreeData(vnpecode);
  if(pcgi) MemFree(pcgi);
  if(pcgoid) MemFree(pcgoid);

  if(!bSuccess) {
    ErrPostEx(SEV_ERROR,0,0,"SeqHoundGOECodeFromGiAndGOID failed.");
    return SLRI_FAIL;
  }
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGOPMIDFromGiAndGOID(void)
{
  Int4 argshere[2];
  CharPtr pcgi = NULL, pcgoid=NULL, pstr = NULL;
  Int4 gi = 0, goid=0;
  ValNodePtr vnpgopmid= NULL;
  Boolean bSuccess = FALSE;

  printf("Content-type: text/html\r\n\r\n");
  if( ((argshere[0] = WWWFindName(ginfo,"gi")) > 0)&&
      ((argshere[1] = WWWFindName(ginfo,"goid")) > 0) )	
  {
    if(  ((pcgi = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL)
       &&((pcgoid = StringSave(WWWGetValueByIndex(ginfo,argshere[1])))!=NULL) )
    {
      if(StrToInt4(pcgi,&gi) > 0 && StrToInt4(pcgoid,&goid) > 0) {
	
	vnpgopmid = SHoundGOPMIDFromGiAndGOID(gi, goid);
	if(vnpgopmid != NULL) {
	  pstr = Misc_SaveValNodeListAsString(vnpgopmid);
	
	  fprintf(stdout, "SEQHOUND_OK\n");
	  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
	  else fprintf(stdout, "(null)\n");
	  MemFree(pstr);
	  bSuccess = TRUE;
	} else {
	  ErrPostEx(SEV_ERROR,0,0, "SHoundGOPMIDFromGiAndGOID: vnpgopmid is NULL.");
	  fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
	}
      } else {
	ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
	fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      }
    } else {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
    }
  } else {
    ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter values.");
    fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter values.\n");
  }
  if(vnpgopmid) ValNodeFree(vnpgopmid);
  if(pcgi) MemFree(pcgi);
  if(pcgoid) MemFree(pcgoid);

  if(!bSuccess) {
    ErrPostEx(SEV_ERROR,0,0,"SHoundGOPMIDFromGiAndGOID failed.");
    return SLRI_FAIL;
  }
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGiFromOMIMList(void)
{
  Int4 argshere[2];
  CharPtr pcomimlist = NULL, pstr = NULL;
  ValNodePtr vnpomim = NULL, vnpgi = NULL;
  Boolean bSuccess = FALSE;

  printf("Content-type: text/html\r\n\r\n");
  if (((argshere[0] = WWWFindName(ginfo,"vnpomim")) >= 0)) {

    if( ((pcomimlist = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) ) {

      if( ((vnpomim = Misc_CommaIntString2ValNodeList(pcomimlist, ",")) != NULL) ) {
	
	vnpgi = SHoundGiFromOMIMList(vnpomim);
	if(vnpgi != NULL) {
	  pstr = Misc_SaveValNodeListAsString(vnpgi);
	  fprintf(stdout, "SEQHOUND_OK\n");
	  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
	  else fprintf(stdout, "(null)\n");
	  MemFree(pstr);
	  bSuccess = TRUE;
	} else {
	  ErrPostEx(SEV_ERROR,0,0, "SHoundGiFromOMIMList returned a null list.");
	  fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
	}
      } else {
	ErrPostEx(SEV_ERROR,0,0, "Failed to save as a valnodelist.");
	fprintf(stdout, "SEQHOUND_ERROR Failed to save as a ValNodeList.\n");
      }					
    } else {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
    }
  } else {
    ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter values.");
    fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter values.\n");
  }
  if(pcomimlist) MemFree(pcomimlist);
  if(vnpomim) ValNodeFree(vnpomim);
  if(vnpgi) ValNodeFree(vnpgi);
  if(!bSuccess) {
    ErrPostEx(SEV_ERROR,0,0,"SeqHoundGiFromOMIMList failed.");
    return SLRI_FAIL;
  }
  return SLRI_SUCCESS;
}


SLRI_ERR SeqHoundGiFromOMIM(void)
{
  Int4 argshere[2];
  CharPtr pcomimid = NULL, pstr = NULL;
  Int4 omimid = 0;
  ValNodePtr vnpgi = NULL;
  Boolean bSuccess = FALSE;

  printf("Content-type: text/html\r\n\r\n");
  if ((argshere[0] = WWWFindName(ginfo,"omimid")) >= 0) {
    if(((pcomimid = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) ) {
      if(StrToInt4(pcomimid,&omimid) > 0) {
	
	vnpgi = SHoundGiFromOMIM(omimid);
	if(vnpgi != NULL) {
	  pstr = Misc_SaveValNodeListAsString(vnpgi);
	
	  fprintf(stdout, "SEQHOUND_OK\n");
	  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
	  else fprintf(stdout, "(null)\n");
	  MemFree(pstr);
	  bSuccess = TRUE;
	} else {
	  ErrPostEx(SEV_ERROR,0,0, "SHoundGiFromOMIM: vnpgoid is NULL.");
	  fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
	}
      } else {
	ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
	fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      }
    } else {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
    }
  } else {
    ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter values.");
    fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter values.\n");
  }
  if(vnpgi) ValNodeFree(vnpgi);
  if(pcomimid) MemFree(pcomimid);

  if(!bSuccess) {
    ErrPostEx(SEV_ERROR,0,0,"SHoundGiFromOMIM failed.");
    return SLRI_FAIL;
  }
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundOMIMFromGi(void)
{
  Int4 argshere[2];
  CharPtr pcgi = NULL, pstr = NULL;
  Int4 gi = 0;
  ValNodePtr vnpomim = NULL;
  Boolean bSuccess = FALSE;

  printf("Content-type: text/html\r\n\r\n");
  if ((argshere[0] = WWWFindName(ginfo,"gi")) >= 0) {
    if(((pcgi = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) ) {
      if(StrToInt4(pcgi,&gi) > 0) {
	
	vnpomim = SHoundOMIMFromGi(gi);
	if(vnpomim != NULL) {
	  pstr = Misc_SaveValNodeListAsString(vnpomim);
	
	  fprintf(stdout, "SEQHOUND_OK\n");
	  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
	  else fprintf(stdout, "(null)\n");
	  MemFree(pstr);
	  bSuccess = TRUE;
	} else {
	  ErrPostEx(SEV_ERROR,0,0, "SHoundOMIMFromGi: vnpomim is NULL.");
	  fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
	}
      } else {
	ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
	fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      }
    } else {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
    }
  } else {
    ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter values.");
    fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter values.\n");
  }
  if(vnpomim) ValNodeFree(vnpomim);
  if(pcgi) MemFree(pcgi);

  if(!bSuccess) {
    ErrPostEx(SEV_ERROR,0,0,"SHoundOMIMFromGi failed.");
    return SLRI_FAIL;
  }
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundOMIMFromGiList(void)
{
  Int4 argshere[2];
  CharPtr pcgilist = NULL, pstr = NULL;
  ValNodePtr vnpomim = NULL, vnpgi = NULL;
  Boolean bSuccess = FALSE;

  printf("Content-type: text/html\r\n\r\n");
  if (((argshere[0] = WWWFindName(ginfo,"vnpgi")) >= 0)) {

    if( ((pcgilist = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) ) {

      if( ((vnpgi = Misc_CommaIntString2ValNodeList(pcgilist, ",")) != NULL) ) {
	
	vnpomim = SHoundOMIMFromGiList(vnpgi);
	if(vnpomim != NULL) {
	  pstr = Misc_SaveValNodeListAsString(vnpomim);
	  fprintf(stdout, "SEQHOUND_OK\n");
	  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
	  else fprintf(stdout, "(null)\n");
	  MemFree(pstr);
	  bSuccess = TRUE;
	} else {
	  ErrPostEx(SEV_ERROR,0,0, "SHoundOMIMFromGiList returned a null list.");
	  fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
	}
      } else {
	ErrPostEx(SEV_ERROR,0,0, "Failed to save as a valnodelist.");
	fprintf(stdout, "SEQHOUND_ERROR Failed to save as a ValNodeList.\n");
      }					
    } else {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
    }
  } else {
    ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter values.");
    fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter values.\n");
  }
  if(pcgilist) MemFree(pcgilist);
  if(vnpgi) ValNodeFree(vnpgi);
  if(vnpomim) ValNodeFree(vnpomim);
  if(!bSuccess) {
    ErrPostEx(SEV_ERROR,0,0,"SHoundOMIMFromGiList failed.");
    return SLRI_FAIL;
  }
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGiFromCDDID(void)
{
  Int4 argshere[2];
  CharPtr pccddid = NULL, pstr = NULL;
  ValNodePtr vnpgi= NULL;
  Boolean bSuccess = FALSE;

  printf("Content-type: text/html\r\n\r\n");
  if ((argshere[0] = WWWFindName(ginfo,"cddid")) >= 0) {
    pccddid = StringSave(WWWGetValueByIndex(ginfo,argshere[0]));
    if( (pccddid != NULL) && (strlen(pccddid) != 0) ) {
	
      vnpgi = SHoundGiFromCDDID(pccddid);
      if(vnpgi != NULL) {
	pstr = Misc_SaveValNodeListAsString(vnpgi);
	
	fprintf(stdout, "SEQHOUND_OK\n");
	if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
	else fprintf(stdout, "(null)\n");
	MemFree(pstr);
	bSuccess = TRUE;
      } else {
	ErrPostEx(SEV_ERROR,0,0, "SHoundGiFromCDDID: return value is NULL.");
	fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
      }
    } else {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
    }
  } else {
    ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter values.");
    fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter values.\n");
  }
  if(vnpgi) ValNodeFree(vnpgi);
  if(pccddid) MemFree(pccddid);

  if(!bSuccess) {
    ErrPostEx(SEV_ERROR,0,0,"SHoundGiFromCDDID failed.");
    return SLRI_FAIL;
  }
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGiFromCDDIDList(void)
{
  Int4 argshere[2];
  CharPtr pccddidlist = NULL, pstr = NULL;
  ValNodePtr vnpcddid = NULL, vnpgi = NULL;
  Boolean bSuccess = FALSE;

  printf("Content-type: text/html\r\n\r\n");
  if (((argshere[0] = WWWFindName(ginfo,"vnpcdd")) >= 0)) {

    if( ((pccddidlist = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) ) {

      if( ((vnpcddid = Misc_CommaStrString2ValNodeList(pccddidlist, ",")) != NULL) ) {
	
	vnpgi = SHoundGiFromCDDIDList(vnpcddid);
	if(vnpgi != NULL) {
	  pstr = Misc_SaveValNodeListAsString(vnpgi);
	  fprintf(stdout, "SEQHOUND_OK\n");
	  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
	  else fprintf(stdout, "(null)\n");
	  MemFree(pstr);
	  bSuccess = TRUE;
	} else {
	  ErrPostEx(SEV_ERROR,0,0, "SHoundGiFromCDDIDList returned a null list.");
	  fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
	}
      } else {
	ErrPostEx(SEV_ERROR,0,0, "Failed to save as a valnodelist.");
	fprintf(stdout, "SEQHOUND_ERROR Failed to save as a ValNodeList.\n");
      }					
    } else {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
    }
  } else {
    ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter values.");
    fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter values.\n");
  }
  if(pccddidlist) MemFree(pccddidlist);
  if(vnpcddid) ValNodeFreeData(vnpcddid);
  if(vnpgi) ValNodeFree(vnpgi);
  if(!bSuccess) {
    ErrPostEx(SEV_ERROR,0,0,"SeqHoundGiFromCDDIDList failed.");
    return SLRI_FAIL;
  }
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundCDDIDFromGi(void)
{
  Int4 argshere[2];
  CharPtr pcgi = NULL, pstr = NULL;
  Int4 gi = 0;
  ValNodePtr vnpcdd = NULL;
  Boolean bSuccess = FALSE;

  printf("Content-type: text/html\r\n\r\n");
  if ((argshere[0] = WWWFindName(ginfo,"gi")) >= 0) {
    if(((pcgi = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) ) {
      if(StrToInt4(pcgi,&gi) > 0) {
	
	vnpcdd = SHoundCDDIDFromGi(gi);
	if(vnpcdd != NULL) {
	  pstr = Misc_SaveStrValNodeListAsString(vnpcdd);
	
	  fprintf(stdout, "SEQHOUND_OK\n");
	  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
	  else fprintf(stdout, "(null)\n");
	  MemFree(pstr);
	  bSuccess = TRUE;
	} else {
	  ErrPostEx(SEV_ERROR,0,0, "SHoundCDDIDFromGi: vnpomim is NULL.");
	  fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
	}
      } else {
	ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
	fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      }
    } else {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
    }
  } else {
    ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter values.");
    fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter values.\n");
  }
  if(vnpcdd) ValNodeFree(vnpcdd);
  if(pcgi) MemFree(pcgi);

  if(!bSuccess) {
    ErrPostEx(SEV_ERROR,0,0,"SeqHoundCDDIDFromGi failed.");
    return SLRI_FAIL;
  }
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundCDDIDFromGiList(void)
{
  Int4 argshere[2];
  CharPtr pcgilist = NULL, pstr = NULL;
  ValNodePtr vnpcdd = NULL, vnpgi = NULL;
  Boolean bSuccess = FALSE;

  printf("Content-type: text/html\r\n\r\n");
  if (((argshere[0] = WWWFindName(ginfo,"vnpgi")) >= 0)) {

    if( ((pcgilist = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) ) {

      if( ((vnpgi = Misc_CommaIntString2ValNodeList(pcgilist, ",")) != NULL) ) {
	
	vnpcdd = SHoundCDDIDFromGiList(vnpgi);
	if(vnpcdd != NULL) {
	  pstr = Misc_SaveStrValNodeListAsString(vnpcdd);
	  fprintf(stdout, "SEQHOUND_OK\n");
	  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
	  else fprintf(stdout, "(null)\n");
	  MemFree(pstr);
	  bSuccess = TRUE;
	} else {
	  ErrPostEx(SEV_ERROR,0,0, "SHoundCDDIDFromGiList returned a null list.");
	  fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
	}
      } else {
	ErrPostEx(SEV_ERROR,0,0, "Failed to save as a valnodelist.");
	fprintf(stdout, "SEQHOUND_ERROR Failed to save as a ValNodeList.\n");
      }					
    } else {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
    }
  } else {
    ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter values.");
    fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter values.\n");
  }
  if(pcgilist) MemFree(pcgilist);
  if(vnpgi) ValNodeFree(vnpgi);
  if(vnpcdd) ValNodeFree(vnpcdd);
  if(!bSuccess) {
    ErrPostEx(SEV_ERROR,0,0,"SeqHoundCDDIDFromGiList failed.");
    return SLRI_FAIL;
  }
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundCDDScoreFromGi(void)
{
  Int4 argshere[2];
  CharPtr pcgi = NULL, pccddid = NULL;
  Int4 gi = 0;
  FloatHi score = 0;
  Boolean bSuccess = FALSE;

  printf("Content-type: text/html\r\n\r\n");
  if ( ((argshere[0] = WWWFindName(ginfo,"gi")) >= 0) &&
       ((argshere[1] = WWWFindName(ginfo, "cddid")) >= 0)) {

    if( ((pcgi = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) &&
	((pccddid = StringSave(WWWGetValueByIndex(ginfo,argshere[1]))) != NULL) ) {

      if( (StrToInt4(pcgi,&gi) > 0) && (pccddid != NULL) ) {
	
	score = SHoundCDDScoreFromGi(gi, pccddid);
	if(score > 0) {
	  fprintf(stdout, "SEQHOUND_OK\n");
	  fprintf(stdout, "%f\n", (float)score);
	  bSuccess = TRUE;
	} else {
	  ErrPostEx(SEV_ERROR,0,0, "SeqHoundCDDScoreFromGi: score is invalid.");
	  fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
	  fprintf(stdout, "%f\n", (float)score);
	}
      } else {
	ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
	fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      }
    } else {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
    }
  } else {
    ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter values.");
    fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter values.\n");
  }
  if(pccddid) MemFree(pccddid);
  if(pcgi) MemFree(pcgi);

  if(!bSuccess) {
    ErrPostEx(SEV_ERROR,0,0,"SeqHoundCDDScoreFromGi failed.");
    return SLRI_FAIL;
  }
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGiFromLLID(void)
{
  Int4 argshere[2];
  CharPtr pcll = NULL, pstr = NULL;
  Int4 ll = 0;
  ValNodePtr vnpgi = NULL;
  Boolean bSuccess = FALSE;

  printf("Content-type: text/html\r\n\r\n");
  if ((argshere[0] = WWWFindName(ginfo,"llid")) >= 0) {
    if(((pcll = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) ) {
      if(StrToInt4(pcll,&ll) > 0) {
	
	vnpgi = SHoundGiFromLLID(ll);
	if(vnpgi != NULL) {
	  pstr = Misc_SaveValNodeListAsString(vnpgi);
	
	  fprintf(stdout, "SEQHOUND_OK\n");
	  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
	  else fprintf(stdout, "(null)\n");
	  MemFree(pstr);
	  bSuccess = TRUE;
	} else {
	  ErrPostEx(SEV_ERROR,0,0, "SHoundGiFromLLID: vnpgi is NULL.");
	  fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
	}
      } else {
	ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
	fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      }
    } else {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
    }
  } else {
    ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter values.");
    fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter values.\n");
  }
  if(vnpgi) ValNodeFree(vnpgi);
  if(pcll) MemFree(pcll);

  if(!bSuccess) {
    ErrPostEx(SEV_ERROR,0,0,"SeqHoundGiFromLLID failed.");
    return SLRI_FAIL;
  }
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGiFromLLIDList(void)
{
  Int4 argshere[2];
  CharPtr pclllist = NULL, pstr = NULL;
  ValNodePtr vnpgi = NULL, vnpll = NULL;
  Boolean bSuccess = FALSE;

  printf("Content-type: text/html\r\n\r\n");
  if (((argshere[0] = WWWFindName(ginfo,"vnpll")) >= 0)) {

    if( ((pclllist = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) ) {

      if( ((vnpll = Misc_CommaIntString2ValNodeList(pclllist, ",")) != NULL) ) {
	
	vnpgi = SHoundGiFromLLIDList(vnpll);
	if(vnpgi != NULL) {
	  pstr = Misc_SaveValNodeListAsString(vnpgi);
	  fprintf(stdout, "SEQHOUND_OK\n");
	  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
	  else fprintf(stdout, "(null)\n");
	  MemFree(pstr);
	  bSuccess = TRUE;
	} else {
	  ErrPostEx(SEV_ERROR,0,0, "SHoundGiFromLLIDList returned a null list.");
	  fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
	}
      } else {
	ErrPostEx(SEV_ERROR,0,0, "Failed to save as a valnodelist.");
	fprintf(stdout, "SEQHOUND_ERROR Failed to save as a ValNodeList.\n");
      }					
    } else {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
    }
  } else {
    ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter values.");
    fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter values.\n");
  }
  if(pclllist) MemFree(pclllist);
  if(vnpgi) ValNodeFree(vnpgi);
  if(vnpll) ValNodeFree(vnpll);
  if(!bSuccess) {
    ErrPostEx(SEV_ERROR,0,0,"SeqHoundGiFromLLIDList failed.");
    return SLRI_FAIL;
  }
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundLLIDFromGi(void)
{
  Int4 argshere[2];
  CharPtr pcgi = NULL;
  Int4 gi = 0;
  Int4 llid = 0;
  Boolean bSuccess = FALSE;

  printf("Content-type: text/html\r\n\r\n");
  if ((argshere[0] = WWWFindName(ginfo,"gi")) >= 0) {
    if(((pcgi = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) ) {
      if(StrToInt4(pcgi,&gi) > 0) {
	
	llid = SHoundLLIDFromGi(gi);
	if(llid != 0) {
	  fprintf(stdout, "SEQHOUND_OK\n");
	  fprintf(stdout, "%d\n", llid);
	  bSuccess = TRUE;
	} else {
	  ErrPostEx(SEV_ERROR,0,0, "SHoundLLIDFromGi: Invalid return value.");
	  fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
	}
      } else {
	ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
	fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      }
    } else {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
    }
  } else {
    ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter values.");
    fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter values.\n");
  }
  if(pcgi) MemFree(pcgi);

  if(!bSuccess) {
    ErrPostEx(SEV_ERROR,0,0,"SeqHoundLLIDFromGi failed.");
    return SLRI_FAIL;
  }
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundLLIDFromGiList(void)
{
  Int4 argshere[2];
  CharPtr pcgilist = NULL, pstr = NULL;
  ValNodePtr vnpgi = NULL, vnpll = NULL;
  Boolean bSuccess = FALSE;

  printf("Content-type: text/html\r\n\r\n");
  if (((argshere[0] = WWWFindName(ginfo,"vnpgi")) >= 0)) {

    if( ((pcgilist = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) ) {

      if( ((vnpgi = Misc_CommaIntString2ValNodeList(pcgilist, ",")) != NULL) ) {
	
	vnpll = SHoundLLIDFromGiList(vnpgi);
	if(vnpll != NULL) {
	  pstr = Misc_SaveValNodeListAsString(vnpll);
	  fprintf(stdout, "SEQHOUND_OK\n");
	  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
	  else fprintf(stdout, "(null)\n");
	  MemFree(pstr);
	  bSuccess = TRUE;
	} else {
	  ErrPostEx(SEV_ERROR,0,0, "SHoundLLIDFromGiList returned a null list.");
	  fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
	}
      } else {
	ErrPostEx(SEV_ERROR,0,0, "Failed to save as a valnodelist.");
	fprintf(stdout, "SEQHOUND_ERROR Failed to save as a ValNodeList.\n");
      }					
    } else {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
    }
  } else {
    ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter values.");
    fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter values.\n");
  }
  if(pcgilist) MemFree(pcgilist);
  if(vnpgi) ValNodeFree(vnpgi);
  if(vnpll) ValNodeFree(vnpll);
  if(!bSuccess) {
    ErrPostEx(SEV_ERROR,0,0,"SeqHoundLLIDFromGiList failed.");
    return SLRI_FAIL;
  }
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundLocusFromGi(void)
{
  Int4 argshere[2];
  CharPtr pcgi = NULL;
  Int4 gi = 0;
  CharPtr locus = NULL;
  Boolean bSuccess = FALSE;

  printf("Content-type: text/html\r\n\r\n");
  if ((argshere[0] = WWWFindName(ginfo,"gi")) >= 0) {
    if(((pcgi = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) ) {
      if(StrToInt4(pcgi,&gi) > 0) {
	
	locus = SHoundLocusFromGi(gi);
	if(locus != NULL) {
	
	  fprintf(stdout, "SEQHOUND_OK\n");
	  fprintf(stdout, "%s\n", locus);
	  bSuccess = TRUE;
	} else {
	  ErrPostEx(SEV_ERROR,0,0, "SHoundLocusFromGi: locus is NULL.");
	  fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
	}
      } else {
	ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
	fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      }
    } else {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
    }
  } else {
    ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter values.");
    fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter values.\n");
  }
  if(locus) MemFree(locus);
  if(pcgi) MemFree(pcgi);

  if(!bSuccess) {
    ErrPostEx(SEV_ERROR,0,0,"SeqHoundLocusFromGi failed.");
    return SLRI_FAIL;
  }
  return SLRI_SUCCESS;
}


SLRI_ERR SeqHoundGODBGetRecordByReference(void)
{
  Int4 argshere[2];
  CharPtr pcreftype = NULL, pstr = NULL;
  ValNodePtr vnp = NULL;
  Boolean bSuccess = FALSE;

  printf("Content-type: text/html\r\n\r\n");
  if ((argshere[0] = WWWFindName(ginfo,"refType")) >= 0) {
    if(((pcreftype = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL)
		&& (strlen(pcreftype)!=0) ) {
	
      vnp = SHoundGODBGetRecordByReference(pcreftype);
      if(vnp != NULL) {
	pstr = Misc_SaveValNodeListAsString(vnp);
	fprintf(stdout, "SEQHOUND_OK\n");
	fprintf(stdout, "%s\n", IsNullStr(pstr));
	MemFree(pstr);
	bSuccess = TRUE;
      } else {
	ErrPostEx(SEV_ERROR,0,0, "SHoundGODBGetRecordByReference: NULL return value.");
	fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
	}
    } else {
      ErrPostEx(SEV_ERROR,0,0, "SHoundGODBGetRecordByReference: Failed to get parameter value.");
      fprintf(stdout, "SHoundGODBGetRecordByReference: SEQHOUND_ERROR Failed to get parameter value.\n");
    }
  } else {
    ErrPostEx(SEV_ERROR,0,0, "SHoundGODBGetRecordByReference: Failed to get parameter values.");
    fprintf(stdout, "SHoundGODBGetRecordByReference: SEQHOUND_ERROR Failed to get parameter values.\n");
  }
  if(pcreftype) MemFree(pcreftype);

  if(!bSuccess) {
    ErrPostEx(SEV_ERROR,0,0,"SeqHoundGODBGetRecordByReference failed.");
    return SLRI_FAIL;
  }
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGODBGetChildrenOf(void)
{
  Int4 argshere[2];
  CharPtr pcgoid = NULL, pstr = NULL;
  Int4 goid = 0;
  ValNodePtr vnp = NULL;
  Boolean bSuccess = FALSE;

  printf("Content-type: text/html\r\n\r\n");
  if ((argshere[0] = WWWFindName(ginfo,"goid")) >= 0) {
    if(((pcgoid = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) ) {
      if(StrToInt4(pcgoid,&goid) > 0) {
	
	vnp = SHoundGODBGetChildrenOf(goid);
	if(vnp != NULL) {
	  pstr = Misc_SaveValNodeListAsString(vnp);
	  fprintf(stdout, "SEQHOUND_OK\n");
	  fprintf(stdout, "%s\n", IsNullStr(pstr));
	  MemFree(pstr);
	  bSuccess = TRUE;
	} else {
	  ErrPostEx(SEV_ERROR,0,0, "SHoundGODBGetChildrenOf: NULL return value.");
	  fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
	}
      } else {
	ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
	fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      }
    } else {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
    }
  } else {
    ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter values.");
    fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter values.\n");
  }
  if(pcgoid) MemFree(pcgoid);

  if(!bSuccess) {
    ErrPostEx(SEV_ERROR,0,0,"SeqHoundGODBGetChildrenOf failed.");
    return SLRI_FAIL;
  }
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGODBGetParentOf(void)
{
  Int4 argshere[2];
  CharPtr pcgoid = NULL, pstr = NULL;
  Int4 goid = 0;
  ValNodePtr vnp = NULL;
  Boolean bSuccess = FALSE;

  printf("Content-type: text/html\r\n\r\n");
  if ((argshere[0] = WWWFindName(ginfo,"goid")) >= 0) {
    if(((pcgoid = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) ) {
      if(StrToInt4(pcgoid,&goid) > 0) {
	
	vnp = SHoundGODBGetParentOf(goid);
	if(vnp != NULL) {
	  pstr = Misc_SaveValNodeListAsString(vnp);
	  fprintf(stdout, "SEQHOUND_OK\n");
	  fprintf(stdout, "%s\n", IsNullStr(pstr));
	  MemFree(pstr);
	  bSuccess = TRUE;
	} else {
	  ErrPostEx(SEV_ERROR,0,0, "SHoundGODBGetParentOf: NULL return value.");
	  fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
	}
      } else {
	ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
	fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      }
    } else {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
    }
  } else {
    ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter values.");
    fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter values.\n");
  }
  if(pcgoid) MemFree(pcgoid);

  if(!bSuccess) {
    ErrPostEx(SEV_ERROR,0,0,"SeqHoundGODBGetParentOf failed.");
    return SLRI_FAIL;
  }
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGODBGetAllAncestors(void)
{
  Int4 argshere[2];
  CharPtr pcgoid = NULL, pstr = NULL;
  Int4 goid = 0;
  ValNodePtr vnp = NULL;
  Boolean bSuccess = FALSE;

  printf("Content-type: text/html\r\n\r\n");
  if ((argshere[0] = WWWFindName(ginfo,"goid")) >= 0) {
    if(((pcgoid = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) ) {
      if(StrToInt4(pcgoid,&goid) > 0) {
	
	vnp = SHoundGODBGetAllAncestors(goid);
	if(vnp != NULL) {
	  pstr = Misc_SaveValNodeListAsString(vnp);
	  fprintf(stdout, "SEQHOUND_OK\n");
	  fprintf(stdout, "%s\n", IsNullStr(pstr));
	  MemFree(pstr);
	  bSuccess = TRUE;
	} else {
	  ErrPostEx(SEV_ERROR,0,0, "SHoundGODBGetAllAncestors: NULL return value.");
	  fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
	}
      } else {
	ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
	fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      }
    } else {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
    }
  } else {
    ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter values.");
    fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter values.\n");
  }
  if(pcgoid) MemFree(pcgoid);

  if(!bSuccess) {
    ErrPostEx(SEV_ERROR,0,0,"SeqHoundGODBGetAllAncestors failed.");
    return SLRI_FAIL;
  }
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGODBGetAllChildren(void)
{
  Int4 argshere[2];
  CharPtr pcgoid = NULL, pstr = NULL;
  Int4 goid = 0;
  ValNodePtr vnp = NULL;
  Boolean bSuccess = FALSE;

  printf("Content-type: text/html\r\n\r\n");
  if ((argshere[0] = WWWFindName(ginfo,"goid")) >= 0) {
    if(((pcgoid = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) ) {
      if(StrToInt4(pcgoid,&goid) > 0) {
	
	vnp = SHoundGODBGetAllChildren(goid);
	if(vnp != NULL) {
	  pstr = Misc_SaveValNodeListAsString(vnp);
	  fprintf(stdout, "SEQHOUND_OK\n");
	  fprintf(stdout, "%s\n", IsNullStr(pstr));
	  MemFree(pstr);
	  bSuccess = TRUE;
	} else {
	  ErrPostEx(SEV_ERROR,0,0, "SHoundGODBGetAllChildren: NULL return value.");
	  fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
	}
      } else {
	ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
	fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      }
    } else {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
    }
  } else {
    ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter values.");
    fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter values.\n");
  }
  if(pcgoid) MemFree(pcgoid);

  if(!bSuccess) {
    ErrPostEx(SEV_ERROR,0,0,"SeqHoundGODBGetAllChildren failed.");
    return SLRI_FAIL;
  }
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGODBGetNameByID(void)
{
  Int4 argshere[2];
  CharPtr pcgoid = NULL;
  Int4 goid = 0;
  CharPtr name = NULL;
  Boolean bSuccess = FALSE;

  printf("Content-type: text/html\r\n\r\n");
  if ((argshere[0] = WWWFindName(ginfo,"goid")) >= 0) {
    if(((pcgoid = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) ) {
      if(StrToInt4(pcgoid,&goid) > 0) {
	  name = SHoundGODBGetNameByID(goid);
	  if(name != NULL) {
	  fprintf(stdout, "SEQHOUND_OK\n");
	  fprintf(stdout, "%s\n", name);
	  fflush(stdout);
	  bSuccess = TRUE;
	} else {
	  ErrPostEx(SEV_ERROR,0,0, "SHoundGODBGetNameByID: NULL return value.");
	  fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
	}
      } else {
	ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
	fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      }
    } else {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
    }
  } else {
    ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter values.");
    fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter values.\n");
  }
  if(pcgoid) MemFree(pcgoid);
  if(name) MemFree(name);
  if(!bSuccess) {
    ErrPostEx(SEV_ERROR,0,0,"SeqHoundGODBGetNameByID failed.");
    return SLRI_FAIL;
  }
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGODBGetClassification(void)
{
  Int4 argshere[2];
  CharPtr pcgoid = NULL;
  Int4 goid = 0;
  CharPtr class = NULL;
  Boolean bSuccess = FALSE;

  printf("Content-type: text/html\r\n\r\n");
  if ((argshere[0] = WWWFindName(ginfo,"goid")) >= 0) {
    if(((pcgoid = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) ) {
      if(StrToInt4(pcgoid,&goid) > 0) {
	  class = SHoundGODBGetClassification(goid);
	  if(class != NULL) {
	  fprintf(stdout, "SEQHOUND_OK\n");
	  fprintf(stdout, "%s\n", class);
	  fflush(stdout);
	  bSuccess = TRUE;
	} else {
	  ErrPostEx(SEV_ERROR,0,0, "SHoundGODBGetClassification: NULL return value.");
	  fprintf(stdout, "SEQHOUND_NULL Null return value.\n");
	}
      } else {
	ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
	fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      }
    } else {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
    }
  } else {
    ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter values.");
    fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter values.\n");
  }
  if(pcgoid) MemFree(pcgoid);
  if(class) MemFree(class);
  if(!bSuccess) {
    ErrPostEx(SEV_ERROR,0,0,"SeqHoundGODBGetClassification failed.");
    return SLRI_FAIL;
  }
  return SLRI_SUCCESS;
}

/* function disabled since it has a wrong logic
SLRI_ERR SeqHoundGODBGetHierarchyLevel(void)
{
  Int4 argshere[2];
  CharPtr pcgoid = NULL;
  Int4 goid = 0;
  Int4 iLevel = 0;
  Boolean bSuccess = FALSE;

  printf("Content-type: text/html\r\n\r\n");
  if ((argshere[0] = WWWFindName(ginfo,"goid")) >= 0) {
    if(((pcgoid = StringSave(WWWGetValueByIndex(ginfo,argshere[0]))) != NULL) ) {
      if(StrToInt4(pcgoid,&goid) > 0) {
	
	iLevel = SHoundGODBGetHierarchyLevel(goid);
	if(iLevel > 0) {
	  fprintf(stdout, "SEQHOUND_OK\n");
	  fprintf(stdout, "%d\n", iLevel);
	  bSuccess = TRUE;
	} else {
	  ErrPostEx(SEV_ERROR,0,0, "SHoundGODBGetHierarchyLevel: NULL return value.");
	  fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
	}
      } else {
	ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
	fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      }
    } else {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
    }
  } else {
    ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter values.");
    fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter values.\n");
  }
  if(pcgoid) MemFree(pcgoid);

  if(!bSuccess) {
    ErrPostEx(SEV_ERROR,0,0,"SeqHoundGODBGetHierarachyLevel failed.");
    return SLRI_FAIL;
  }
  return SLRI_SUCCESS;
}*/

SLRI_ERR SeqHoundAllGenomesWithScientificNames(void)
{
	CLinkSetPtr ptr = NULL;
	FILE* fp = NULL;
	AsnIoPtr aip = NULL;
	Boolean bSuccess = FALSE;

	if(SHoundAllGenomesWithScientificNames(&ptr) == SLRI_SUCCESS) {
		if(ptr != NULL) {
			if((fp = FileOpen("stdout", FILE_MODE)) != NULL) {
				if((aip=AsnIoNew(ASN_MODE, fp, NULL,NULL,NULL)) != NULL) {
					printf("Content-type: application/x-octet-stream\n\n");
					fprintf(stdout, "SEQHOUND_OK\n");
					CLinkSetAsnWrite(ptr, aip, NULL);
					AsnIoClose(aip);
					bSuccess = TRUE;	
			  	} else PostError("SEQHOUND_ERROR", "Unable to open ASN.1 IO.");
				FileClose(fp);
			} else PostError("SEQHOUND_ERROR", "Unable to open file.");
			CLinkSetFree(ptr);
		} else PostError("SEQHOUND_NULL", "NULL return value.");
	} else PostError("SEQHOUND_ERROR", "Function returned error.");				

	if(bSuccess == FALSE) {
		ErrPostEx(SEV_ERROR,0,0,"SeqHoundAllGenomesWithScientificNames failed.");
		return SLRI_FAIL;
	}
	return SLRI_SUCCESS;
}


/*Retrieves scientics names of all Genomes. Since Perl and BioPerl remote API functions cant read binary
  file as implemented in SeqHoundAllGemomesWithScientificNames, this is the version which retrieves the 
  plain text of all taxids and their associated names */
SLRI_ERR SeqHoundAllGenomesWithScientificNamesPL(void)
{
	CLinkSetPtr ptr = NULL;
	Int4 i=0;
	Boolean bSuccess = FALSE;

        printf("Content-type: text/html\r\n\r\n");
	if(SHoundAllGenomesWithScientificNames(&ptr) == SLRI_SUCCESS) {
	        fprintf(stdout, "SEQHOUND_OK\n");
		if( ptr != NULL) {
		        for (i = 0; i<ptr->num; i++)
			{
			      fprintf (stdout, "TAXID:%ld		Name:%s\n", ptr->uids[i], ptr->weights[i]);
			}
			bSuccess = TRUE;
			CLinkSetFree(ptr);
			
		} else { 
		     fprintf (stdout, "(null)\n");
		}
	} else PostError("SEQHOUND_ERROR", "SeqHoundAllGenomesWithScientificNamesPL: internal call function returned error.");				

	if(bSuccess == FALSE) {
		ErrPostEx(SEV_ERROR,0,0,"SeqHoundAllGenomesWithScientificNamesPL failed.");
		return SLRI_FAIL;
	}
	return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundAllGenomesWithNumProteins(void)
{
	ILinkSetPtr ptr = NULL;
	FILE* fp = NULL;
	AsnIoPtr aip = NULL;
	Boolean bSuccess = FALSE;
        
	if(SHoundAllGenomesWithNumProteins(&ptr) == SLRI_SUCCESS) {
		if(ptr != NULL) {
			if((fp = FileOpen("stdout", FILE_MODE)) != NULL) {
				if((aip=AsnIoNew(ASN_MODE, fp, NULL,NULL,NULL)) != NULL) {
					printf("Content-type: application/x-octet-stream\n\n");
					fprintf(stdout, "SEQHOUND_OK\n");
					ILinkSetAsnWrite(ptr, aip, NULL);
					AsnIoClose(aip);
					bSuccess = TRUE;	
			  	} else PostError("SEQHOUND_ERROR", "Unable to open ASN.1 IO.");
				FileClose(fp);
			} else PostError("SEQHOUND_ERROR", "Unable to open file.");
			ILinkSetFree(ptr);
		} else PostError("SEQHOUND_NULL", "NULL return value.");
	} else PostError("SEQHOUND_ERROR", "Function returned error.");				

	if(bSuccess == FALSE) {
		ErrPostEx(SEV_ERROR,0,0,"SeqHoundAllGenomesWithNumProteins failed.");
		return SLRI_FAIL;
	}
	return SLRI_SUCCESS;
}

/*retrieves number of proteins of all genomes. Since Perl and BioPerl remote API functions cant read
  binary file as implemented in SeqHoundAllGemomesWithNumProteins, this is the version which retrieves 
  the plain text of all taxids and their associated number of proteins */
SLRI_ERR SeqHoundAllGenomesWithNumProteinsPL(void)
{
	ILinkSetPtr ptr = NULL;
	Int4 i=0;
	Boolean bSuccess = FALSE;

        printf("Content-type: text/html\r\n\r\n");
	if(SHoundAllGenomesWithNumProteins(&ptr) == SLRI_SUCCESS) {
	        fprintf(stdout, "SEQHOUND_OK\n");
		if( ptr != NULL) {
		        for (i = 0; i<ptr->num; i++)
			{
			      fprintf (stdout, "TAXID:%ld		NumProteins:%ld\n", ptr->uids[i], ptr->weights[i]);
			}
			bSuccess = TRUE;
			ILinkSetFree(ptr);
			
		} else { 
		     fprintf (stdout, "(null)\n");
		}
	} else PostError("SEQHOUND_ERROR", "SeqHoundAllGenomesWithNumProteinsPL: internal call function returned error.");				

	if(bSuccess == FALSE) {
		ErrPostEx(SEV_ERROR,0,0,"SeqHoundAllGenomesWithNumProteinsPL failed.");
		return SLRI_FAIL;
	}
	return SLRI_SUCCESS;
}
  

SLRI_ERR SeqHoundGetDefline(void)
{
  CharPtr pcThis = NULL;
  CharPtr giptr = NULL;
  CharPtr pacc = NULL;
  Int4 IndexArgs = -1;
  Int4 gival = 0;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"gi")) >= 0)
    {
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       giptr = StringSave(pcThis);
    }
  if (giptr == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
     }
  if(!StrToInt4 (giptr, &gival))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(giptr);
      return SLRI_FAIL;
   }
  pacc = SHoundGetDefline(gival);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pacc == NULL)
   fprintf(stdout, "(null)\n");
  else
   fprintf(stdout, "%s\n", pacc);
  MemFree(giptr);
  MemFree(pacc);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGetDeflineList(void)
{
  CharPtr pcThis = NULL;
  CharPtr pgi = NULL;
  ValNodePtr pvnacc = NULL;
  ValNodePtr pvngi = NULL;
  CharPtr pstr = NULL;
  Int4 IndexArgs = -1;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"pgi")) >= 0)
    {
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       pgi = StringSave(pcThis);
    }
  if (pgi == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
    }
  pvngi = Misc_CommaIntString2ValNodeList(pgi, ",");
  if(pvngi == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Misc_CommaIntString2ValNodeList failed.");
      fprintf(stdout, "SEQHOUND_ERROR Misc_CommaIntString2ValNodeList failed.\n");
      MemFree(pgi);
      return SLRI_FAIL;
   }
  pvnacc =  SHoundGetDeflineList(pvngi);
  if(pvnacc == NULL)
     ErrPostEx(SEV_ERROR,0,0, "SHoundGetDeflineList returned NULL.");
  pstr = Misc_SaveStrValNodeListAsString(pvnacc);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(pgi);
  MemFree(pstr);
  ValNodeFreeData(pvnacc);
  ValNodeFree(pvngi);
  return SLRI_SUCCESS;
}

/* RPS functions */

SLRI_ERR SeqHoundGetGisByNumberOfDomains(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pgi = NULL;
  Int4 gival = 0;
  ValNodePtr pvngia = NULL;
  CharPtr pstr = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"numdom")) >= 0)
   {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pgi = StringSave(pcThis);
   }
  if (pgi == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
      return SLRI_FAIL;
   }
  if(!StrToInt4 (pgi, &gival))
   {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(pgi);
      return SLRI_FAIL;
   }
  pvngia = SHoundGetGisByNumberOfDomains(gival);
  pstr = Misc_SaveValNodeListAsString(pvngia);
  fprintf(stdout, "SEQHOUND_OK\n");
  if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
  else fprintf (stdout, "(null)\n");
  MemFree(pgi);
  ValNodeFree(pvngia);
  MemFree(pstr);
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGetDomainIdFromLabel(void)
{
  Int4 argshere[2];
  CharPtr pccddid = NULL, pstr = NULL;
  ValNodePtr vnpgi= NULL;
  Boolean bSuccess = FALSE;

  printf("Content-type: text/html\r\n\r\n");
  if ((argshere[0] = WWWFindName(ginfo,"label")) >= 0) {
    pccddid = StringSave(WWWGetValueByIndex(ginfo,argshere[0]));
    if( (pccddid != NULL) && (strlen(pccddid) != 0) ) {
	
      vnpgi = SHoundGetDomainIdFromLabel(pccddid);
      if(vnpgi != NULL) {
	pstr = Misc_SaveStrValNodeListAsString(vnpgi);
	
	fprintf(stdout, "SEQHOUND_OK\n");
	if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
	else fprintf(stdout, "(null)\n");
	MemFree(pstr);
	bSuccess = TRUE;
      } else {
	ErrPostEx(SEV_ERROR,0,0, "SHoundGetDomainIdFromLabel: return value is NULL.");
	fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
      }
    } else {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
    }
  } else {
    ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter values.");
    fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter values.\n");
  }
  if(vnpgi) ValNodeFree(vnpgi);
  if(pccddid) MemFree(pccddid);

  if(!bSuccess) {
    ErrPostEx(SEV_ERROR,0,0,"SHoundGetDomainIdFromLabel failed.");
    return SLRI_FAIL;
  }
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGetGisByDomainId(void)
{
  Int4 argshere[2];
  CharPtr pccddid = NULL, pstr = NULL;
  ValNodePtr vnpgi= NULL;
  Boolean bSuccess = FALSE;

  printf("Content-type: text/html\r\n\r\n");
  if ((argshere[0] = WWWFindName(ginfo,"domain_id")) >= 0) {
    pccddid = StringSave(WWWGetValueByIndex(ginfo,argshere[0]));
    if( (pccddid != NULL) && (strlen(pccddid) != 0) ) {
	
      vnpgi = SHoundGetGisByDomainId(pccddid);
      if(vnpgi != NULL) {
	pstr = Misc_SaveValNodeListAsString(vnpgi);
	
	fprintf(stdout, "SEQHOUND_OK\n");
	if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
	else fprintf(stdout, "(null)\n");
	MemFree(pstr);
	bSuccess = TRUE;
      } else {
	ErrPostEx(SEV_ERROR,0,0, "SHoundGetGisByDomainId: return value is NULL.");
	fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
      }
    } else {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
    }
  } else {
    ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter values.");
    fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter values.\n");
  }
  if(vnpgi) ValNodeFree(vnpgi);
  if(pccddid) MemFree(pccddid);

  if(!bSuccess) {
    ErrPostEx(SEV_ERROR,0,0,"SHoundGetGisByDomainId failed.");
    return SLRI_FAIL;
  }
  return SLRI_SUCCESS;
}


SLRI_ERR SeqHoundGetDomainLabelFromDomainId(void)
{
  Int4 argshere[2];
  CharPtr pcgoid = NULL;
  CharPtr label = NULL;
  Boolean bSuccess = FALSE;

  printf("Content-type: text/html\r\n\r\n");
  if ((argshere[0] = WWWFindName(ginfo,"accession")) >= 0) {
    pcgoid = StringSave(WWWGetValueByIndex(ginfo,argshere[0]));
    if( (pcgoid != NULL) && (strlen(pcgoid) != 0) ) {
      label = SHoundGetDomainLabelFromDomainId(pcgoid);
      if(label != NULL) {
	fprintf(stdout, "SEQHOUND_OK\n");
	fprintf(stdout, "%s\n", label);
	fflush(stdout);
	bSuccess = TRUE;
      } else {
	ErrPostEx(SEV_ERROR,0,0, "SHoundGetDomainLabelFromDomainId: NULL return value.");
	fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
      }
    } else {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
    }
  } else {
    ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter values.");
    fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter values.\n");
  }
  if(pcgoid) MemFree(pcgoid);
  if(label) MemFree(label);
  if(!bSuccess) {
    ErrPostEx(SEV_ERROR,0,0,"SeqHoundGetDomainLabelFromDomainId failed.");
    return SLRI_FAIL;
  }
  return SLRI_SUCCESS;
}


SLRI_ERR SeqHoundGetGisByDomainIdAndEvalue(void)
{
  Int4 argshere[2];
  CharPtr pccddid = NULL, pstr = NULL, pceval = NULL, pcstop = NULL;
  ValNodePtr vnpgi= NULL;
  Boolean bSuccess = FALSE;
  float evalue = 0.0;

  printf("Content-type: text/html\r\n\r\n");
  if (((argshere[0] = WWWFindName(ginfo,"domain")) >= 0) &&
      ((argshere[1] = WWWFindName(ginfo, "evalue")) > 0) )
  {
    pccddid = StringSave(WWWGetValueByIndex(ginfo,argshere[0]));
    pceval = StringSave(WWWGetValueByIndex(ginfo, argshere[1]));

    if( (pccddid != NULL) && (strlen(pccddid) != 0) && 
	((evalue = strtod(pceval, &pcstop)) > 0.0) ) {
	
      vnpgi = SHoundGetGisByDomainIdAndEvalue(pccddid, evalue);
      if(vnpgi != NULL) {
	pstr = Misc_SaveValNodeListAsString(vnpgi);
	
	fprintf(stdout, "SEQHOUND_OK\n");
	if(pstr != NULL) fprintf(stdout, "%s\n", pstr);
	else fprintf(stdout, "(null)\n");
	MemFree(pstr);
	bSuccess = TRUE;
      } else {
	ErrPostEx(SEV_ERROR,0,0, "SHoundGetGisByDomainIdAndEvalue: return value is NULL.");
	fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
      }
    } else {
      ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
    }
  } else {
    ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter values.");
    fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter values.\n");
  }
  if(vnpgi) ValNodeFree(vnpgi);
  if(pccddid) MemFree(pccddid);

  if(!bSuccess) {
    ErrPostEx(SEV_ERROR,0,0,"SHoundGetGisByDomainIdAndEvalue failed.");
    return SLRI_FAIL;
  }
  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGetDomainsFromGi(void)
{
   Int4 IndexArgs = -1;
   CharPtr pcThis = NULL;
   CharPtr giptr = NULL;
   Int4 gival = 0;
   AsnIoPtr aip = NULL;
   FILE *StdOot=NULL;
   StRpsdbPtr pbs = NULL;

   if ((IndexArgs = WWWFindName(ginfo,"gi")) >= 0)
    {
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       giptr = StringSave(pcThis);
    }
   if (giptr == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
    }
   if(!StrToInt4 (giptr, &gival))
    {
       ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
       MemFree(giptr);
       return SLRI_FAIL;
    }
   pbs = SHoundGetDomainsFromGi(gival);
   if (pbs == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetDomainsFromGi returned NULL.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
       MemFree(giptr);
       return SLRI_FAIL;
    }
   /* return the bioseq to the client */
   StdOot=FileOpen("stdout", FILE_MODE);
   if (StdOot == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Unable to open a file.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Unable to open a file.\n");
       MemFree(giptr);
       StRpsdbFree(pbs);
       return SLRI_FAIL;
    }
   if((aip=AsnIoNew(ASN_MODE, StdOot, NULL,NULL,NULL)) == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Unable to open ASN.1 IO.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Unable to open ASN.1 IO.\n");
       MemFree(giptr);
       StRpsdbFree(pbs);
       FileClose(StdOot);
       return SLRI_FAIL;
    }
   printf("Content-type: application/x-octet-stream\n\n");
   fprintf(stdout, "SEQHOUND_OK\n");
   StRpsdbAsnWrite(pbs, aip, NULL);
   AsnIoClose(aip);
   FileClose(StdOot);
   StRpsdbFree(pbs);
   MemFree(giptr);
   return SLRI_SUCCESS;
}


SLRI_ERR SeqHoundGetDomainsFromGiPL(void)
{
   Int4 IndexArgs = -1;
   CharPtr pcThis = NULL;
   CharPtr giptr = NULL;
   Int4 gival = 0;
   AsnIoPtr aip = NULL;
   StRpsdbPtr pbs = NULL;


   if ((IndexArgs = WWWFindName(ginfo,"gi")) >= 0)
    {
        pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
        giptr = StringSave(pcThis);
    }
   if (giptr == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");

       return SLRI_FAIL;
    }
   if(!StrToInt4 (giptr, &gival))
    {
       ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
       MemFree(giptr);
       return SLRI_FAIL;
    }
   pbs = SHoundGetDomainsFromGi(gival);
   if (pbs == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetDomainsFromGi returned NULL.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
       MemFree(giptr);
       return SLRI_FAIL;
    }
   if((aip=AsnIoNew(ASNIO_TEXT_OUT, stdout, NULL,NULL,NULL)) == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Unable to open ASN.1 IO.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Unable to open ASN.1 IO.\n");
       MemFree(giptr);
       StRpsdbFree(pbs);
       return SLRI_FAIL;
    }
   printf("Content-type: text/html\r\n\r\n");
   fprintf(stdout, "SEQHOUND_OK\n");
   StRpsdbAsnWrite(pbs, aip, NULL);
   AsnIoClose(aip);
   StRpsdbFree(pbs);
   MemFree(giptr);
   return SLRI_SUCCESS;
}


SLRI_ERR SeqHoundGetDomainsFromGiWithEvalue(void)
{
   Int4 argsphere[2];
   CharPtr giptr = NULL, pceval = NULL, pcstop = NULL;
   Int4 gival = 0;
   AsnIoPtr aip = NULL;
   FILE *StdOot=NULL;
   StRpsdbPtr pbs = NULL;
   float evalue = 0.0;

   if ( ((argsphere[0] = WWWFindName(ginfo,"gi")) >= 0) &&
	((argsphere[1] = WWWFindName(ginfo,"evalue")) > 0) )
     {
       giptr = StringSave(WWWGetValueByIndex(ginfo,argsphere[0]));
       pceval = StringSave(WWWGetValueByIndex(ginfo, argsphere[1]));
     }
   if (giptr == NULL || pceval == NULL)
     {
       ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
					}
   if(!StrToInt4 (giptr, &gival) ||
      ((evalue = strtod(pceval, &pcstop)) < 0.0) )
     {
       ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
       MemFree(giptr); MemFree(pceval);
       return SLRI_FAIL;
     }
   pbs = SHoundGetDomainsFromGiWithEvalue(gival, evalue);
   if (pbs == NULL)
     {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetDomainsFromGiWithEvalue returned NULL.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
       MemFree(giptr); MemFree(pceval);
       return SLRI_FAIL;
     }
   /* return the bioseq to the client */
   StdOot=FileOpen("stdout", FILE_MODE);
   if (StdOot == NULL)
     {
       ErrPostEx(SEV_ERROR,0,0, "Unable to open a file.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Unable to open a file.\n");
       MemFree(giptr); MemFree(pceval);
       StRpsdbFree(pbs);
       return SLRI_FAIL;
     }
   if((aip=AsnIoNew(ASN_MODE, StdOot, NULL,NULL,NULL)) == NULL)
     {
       ErrPostEx(SEV_ERROR,0,0, "Unable to open ASN.1 IO.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Unable to open ASN.1 IO.\n");
       MemFree(giptr);
       MemFree(pceval);
       StRpsdbFree(pbs);
       FileClose(StdOot);
       return SLRI_FAIL;
     }
   printf("Content-type: application/x-octet-stream\n\n");
   fprintf(stdout, "SEQHOUND_OK\n");
   StRpsdbAsnWrite(pbs, aip, NULL);
   AsnIoClose(aip);
   FileClose(StdOot);
   StRpsdbFree(pbs);
   MemFree(giptr); MemFree(pceval);
   return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGetDomainsFromGiWithEvaluePL(void)
{
   Int4 argsphere[2];
   CharPtr giptr = NULL, pceval = NULL, pcstop = NULL;
   Int4 gival = 0;
   AsnIoPtr aip = NULL;
   StRpsdbPtr pbs = NULL;
   float evalue = 0.0;

   if ( ((argsphere[0] = WWWFindName(ginfo,"gi")) >= 0) &&
	((argsphere[1] = WWWFindName(ginfo, "evalue")) > 0) )
    {
        giptr = StringSave(WWWGetValueByIndex(ginfo,argsphere[0]));
        pceval = StringSave(WWWGetValueByIndex(ginfo, argsphere[1]));
    }
   if (giptr == NULL || pceval == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");

       return SLRI_FAIL;
    }
   if(!StrToInt4 (giptr, &gival) ||
      ((evalue = strtod(pceval, &pcstop)) < 0.0) )
    {
       ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
       MemFree(giptr); MemFree(pceval);
       return SLRI_FAIL;
    }
   pbs = SHoundGetDomainsFromGiWithEvalue(gival, evalue);
   if (pbs == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetDomainsFromGiWithEvalue returned NULL.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
       MemFree(giptr); MemFree(pceval);
       StRpsdbFree(pbs);
       return SLRI_FAIL;
    }
   if((aip=AsnIoNew(ASNIO_TEXT_OUT, stdout, NULL,NULL,NULL)) == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Unable to open ASN.1 IO.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Unable to open ASN.1 IO.\n");
       MemFree(giptr); MemFree(pceval);
       StRpsdbFree(pbs);
       return SLRI_FAIL;
    }
   printf("Content-type: text/html\r\n\r\n");
   fprintf(stdout, "SEQHOUND_OK\n");
   StRpsdbAsnWrite(pbs, aip, NULL);
   AsnIoClose(aip);
   StRpsdbFree(pbs);
   MemFree(giptr); MemFree(pceval);
   return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGetDomainsFromGiListWithEvalue(void)
{
	Int4 argsphere[2];
	CharPtr giptr = NULL, pceval = NULL, pcstop = NULL;
	ValNodePtr pvngi = NULL;
	AsnIoPtr aip = NULL;
	FILE *StdOot=NULL;
	StRpsdbPtr pbs = NULL;
	float evalue = 0.0;

	if ( ((argsphere[0] = WWWFindName(ginfo,"pgi")) >= 0) &&
		((argsphere[1] = WWWFindName(ginfo,"evalue")) > 0) )
	{
		giptr = StringSave(WWWGetValueByIndex(ginfo,argsphere[0]));
		pceval = StringSave(WWWGetValueByIndex(ginfo, argsphere[1]));
	}
	if (giptr == NULL || pceval == NULL)
	{
		ErrPostEx(SEV_ERROR,0,0, "Failed to get parameter value.");
		printf("Content-type: text/html\r\n\r\n");
		fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
		return SLRI_FAIL;
	}
	pvngi = Misc_CommaIntString2ValNodeList(giptr, ",");
	if(pvngi==NULL ||
	((evalue = strtod(pceval, &pcstop)) < 0.0) )
	{
		ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
		printf("Content-type: text/html\r\n\r\n");
		fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
		MemFree(giptr); MemFree(pceval);
		return SLRI_FAIL;
	}
	pbs = SHoundGetDomainsFromGiListWithEvalue(pvngi, evalue);
	if (pbs == NULL)
	{
		ErrPostEx(SEV_ERROR,0,0, "SHoundGetDomainsFromGiListWithEvalue returned NULL.");
		printf("Content-type: text/html\r\n\r\n");
		fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
		MemFree(giptr); MemFree(pceval);
		return SLRI_FAIL;
	}
	/* return the bioseq to the client */
	StdOot=FileOpen("stdout", FILE_MODE);
	if (StdOot == NULL)
	{
		ErrPostEx(SEV_ERROR,0,0, "Unable to open a file.");
		printf("Content-type: text/html\r\n\r\n");
		fprintf(stdout, "SEQHOUND_ERROR Unable to open a file.\n");
		MemFree(giptr); MemFree(pceval);
		StRpsdbFree(pbs);
		return SLRI_FAIL;
	}
	if((aip=AsnIoNew(ASN_MODE, StdOot, NULL,NULL,NULL)) == NULL)
	{
		ErrPostEx(SEV_ERROR,0,0, "Unable to open ASN.1 IO.");
		printf("Content-type: text/html\r\n\r\n");
		fprintf(stdout, "SEQHOUND_ERROR Unable to open ASN.1 IO.\n");
		MemFree(giptr);
		MemFree(pceval);
		StRpsdbFree(pbs);
		FileClose(StdOot);
		return SLRI_FAIL;
	}
	printf("Content-type: application/x-octet-stream\n\n");
	fprintf(stdout, "SEQHOUND_OK\n");
	StRpsdbAsnWrite(pbs, aip, NULL);
	AsnIoClose(aip);
	FileClose(StdOot);
	StRpsdbFree(pbs);
	MemFree(giptr); MemFree(pceval);
	ValNodeFree(pvngi);
	return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundGetNameByGi(void)
{
	CharPtr name_ptr	= NULL;
	CharPtr giptr		= NULL;
	Int4 gival			= 0;
	CharPtr pcThis		= NULL;
	Int4 IndexArgs		= -1;
	
	printf("Content-type: text/html\r\n\r\n");
	if ((IndexArgs = WWWFindName(ginfo,"gi")) >= 0)
	{	
		pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
		giptr = StringSave(pcThis);
	}
	if (!giptr)
	{
		ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
		fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
		return SLRI_FAIL;
	}
	if(!StrToInt4 (giptr, &gival))
	{
		ErrPostEx(SEV_ERROR,0,0, "Invalid function parameter.");
		fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
		MemFree(giptr);
		return SLRI_FAIL;
	}
	name_ptr = SHoundGetNameByGi(gival);
	fprintf(stdout, "SEQHOUND_OK\n");
	fprintf(stdout, "%s\n", IsNullStr(name_ptr));
	MemFree(name_ptr);
	return SLRI_SUCCESS;

}

/*
 * Retrieves a SHProteinReportSetDescription object
 * from SeqHound for a given gi value and a format integer. 
 * The report can be in 2 formats, XML or ASN text.
 * To specify ASN text, the query string 'format' variable 
 * is set to 2, XML, set 'format' to 1. 
 * XML is the default format.
 */
SLRI_ERR SeqHoundProteinReportSetDescription(void)
{
   Int4 iIndxArgs[2]                       = {0},  /* the index to the arguments */
        iArgs[2]                           = {0},  /* arguments in int */
        i                                  = 0;    /* a counter */

   Char errbuf[500]                        = {0},  /* error buffer */
        *args[3]                           = {0};  /* 1, 2 are args in string, 
                                                      3 is final buffer */
   SHProteinReportSetDescriptionPtr ppr    = NULL; /* object to retrieve */
   ByteStorePtr bspX                       = NULL; /* buffer to write object into */
   AsnIoBSPtr aipbs                        = NULL; /* asn read/write buffer */


   if ( ((iIndxArgs[0] = WWWFindName(ginfo,"gi")) >= 0)   &&
        ((iIndxArgs[1] = WWWFindName(ginfo, "format")) >= 0)
      )
   {	
       args[0]  = StringSave(WWWGetValueByIndex(ginfo, iIndxArgs[0]));
       args[1] = StringSave(WWWGetValueByIndex(ginfo, iIndxArgs[1]));
   }

   if ( (args[0] == NULL || args[1] == NULL) || 
        !(StrToInt4(args[0], &iArgs[0])) ||
        !(StrToInt4(args[1], &iArgs[1]))
      )
   { 
      StringCpy ( errbuf, "Failed to get parameter value.");
      goto end; 
   }

   ppr = SHProteinReportSetDescriptionNew ();
   if( ! ppr || !(ppr->query = iArgs[0])   ||
       ! SHoundProteinReportSetDescription(ppr, REPORT_ALL) 
     )
   {
      StringCpy ( errbuf, "Cannot get ProteinReportSetDescription.");
      goto end;
   }

   if ( ! (bspX = BSNew(5000))                || 
        ! (aipbs = AsnIoBSOpen ( "w", bspX))  ||
        ! aipbs->aip 
      )
   {
      StringCpy ( errbuf, "ByteStore IO failed.");
      goto end;
   }

   switch ( iArgs[1] ) 
   {
      case 2: aipbs->aip->type |= ASNIO_TEXT_OUT; break;
      case 1:
      default: aipbs->aip->type |= ASNIO_XML; break;
   }

   if ( ! SHProteinReportSetDescriptionAsnWrite(ppr, aipbs->aip, NULL) )
   {
     StringCpy ( errbuf, "SHProteinReportSetDescriptionAsnWrite failed." );
     goto end;
   }

   AsnIoFlush ( aipbs->aip );
   AsnIoBSClose ( aipbs );
   args[2] = BSStringSave ( bspX );
   SHProteinReportSetDescriptionFree ( ppr );

   fprintf ( stdout, "Content-type: text/html\r\n\r\n" );
   fprintf ( stdout, "SEQHOUND_OK\n" );
   fprintf ( stdout, "%s", IsNullStr(args[2]) );
   fflush  ( stdout );

   BSFree ( bspX );
   for ( i = 0; i < 3; i++ )
      MemFree(args[i]);
   return SLRI_SUCCESS;

end:
   ErrPostEx ( SEV_ERROR, 0, 0, errbuf );
   fprintf ( stdout, "Content-type: text/html\r\n\r\n" );
   fprintf ( stdout, "SEQHOUND_ERROR %s\n", errbuf );

   AsnIoBSClose ( aipbs );
   SHProteinReportSetDescriptionFree ( ppr );
   BSFree ( bspX );
   for ( i = 0; i < 3; i++ )
      MemFree(args[i]);
   return SLRI_FAIL;
}

#ifdef ODBC 

SLRI_ERR SeqHoundDBXREFGetDBXrefListBySourceRecord(void)
{
    Int4 argsIndex[4];
    CharPtr pdbname = NULL, precordid = NULL, pfield = NULL, pcv = NULL;
    int intcv;
    ValNodePtr pvncv = NULL;
    SHDBXrefPtr pSHDBXref = NULL, pauxSHDBXref = NULL;
    CharPtr pstr = NULL;
    Int4 IndexArgs = -1;
  
    printf("Content-type: text/html\r\n\r\n");
    
    if ( ( (argsIndex[0] = WWWFindName(ginfo, "dbname")) >= 0) &&
	 ( (argsIndex[1] = WWWFindName(ginfo, "recordid")) >= 0) &&
	 ( (argsIndex[2] = WWWFindName(ginfo, "field")) >= 0) &&
	 ( (argsIndex[3] = WWWFindName(ginfo, "cv")) >= 0) 
	)
    {	
	pdbname   = StringSave( WWWGetValueByIndex(ginfo, argsIndex[0]));
	precordid = StringSave( WWWGetValueByIndex(ginfo, argsIndex[1]));
	pfield    = StringSave( WWWGetValueByIndex(ginfo, argsIndex[2]));
	pcv       = StringSave( WWWGetValueByIndex(ginfo, argsIndex[3]));
	intcv = atol(pcv);
    }
    if ( ( (pdbname == NULL)   || (strlen(pdbname) == 0) ) && 
	 ( (precordid == NULL) || (strlen(precordid) == 0) ) &&
	 ( (pfield == NULL)    || (strlen(pfield) == 0) ) &&
	 ( (pcv == NULL)       || (strlen(pcv) == 0) ) )
    {
	ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
	fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
	return SLRI_FAIL;
    }

    if (strcmp(pfield, "NULL") == 0)
	pSHDBXref = SHoundDBXREFGetDBXrefListBySourceRecord(pdbname, precordid, NULL, intcv);
    else
	pSHDBXref = SHoundDBXREFGetDBXrefListBySourceRecord(pdbname, precordid, pfield, intcv);

    if (pSHDBXref == NULL)
    {
	ErrPostEx(SEV_ERROR,0,0, "SHoundDBXREFGetDBXrefListBySourceRecord returned NULL.");
    }
    pauxSHDBXref = pSHDBXref;

    fprintf(stdout, "SEQHOUND_OK\n");

    if(pSHDBXref != NULL)
    {
	while(pSHDBXref != NULL)
	{
	    fprintf(stdout, "%s,", pSHDBXref->sourcedb);
	    fprintf(stdout, "%s,", pSHDBXref->recordid);
	    fprintf(stdout, "%s,", pSHDBXref->field);
	    fprintf(stdout, "%d", pSHDBXref->cv);
	    if(pSHDBXref->next != NULL)
		fprintf(stdout, ",");
	    pSHDBXref = pSHDBXref->next;
	}
    }
  else 
      fprintf (stdout, "(null)\n");

    pSHDBXref = pSHDBXref;

    MemFree(pdbname);
    MemFree(precordid);
    MemFree(pfield);
    MemFree(pcv);
    SHDBXrefFree(pSHDBXref);
    return SLRI_SUCCESS;
}

          
SLRI_ERR SeqHoundDBXREFGetSourceRecordListByDBXref(void)
{
    Int4 argsIndex[4];
    CharPtr pdbname = NULL, precordid = NULL, pfield = NULL, pcv = NULL;
    int intcv;
    CharPtr pacc = NULL;
    ValNodePtr pvncv = NULL;
    SHDBXrefPtr pSHDBXref = NULL, pauxSHDBXref = NULL;
    
    CharPtr pstr = NULL;
    Int4 IndexArgs = -1;
    
    printf("Content-type: text/html\r\n\r\n");
     if ( ( (argsIndex[0] = WWWFindName(ginfo, "dbname")) >= 0) &&
	  ( (argsIndex[1] = WWWFindName(ginfo, "recordid")) >= 0) &&
	  ( (argsIndex[2] = WWWFindName(ginfo, "field")) >= 0) &&
	  ( (argsIndex[3] = WWWFindName(ginfo, "cv")) >= 0) 
	)
     {	
	pdbname   = StringSave( WWWGetValueByIndex(ginfo, argsIndex[0]));
	precordid = StringSave( WWWGetValueByIndex(ginfo, argsIndex[1]));
	pfield    = StringSave( WWWGetValueByIndex(ginfo, argsIndex[2]));
	pcv       = StringSave( WWWGetValueByIndex(ginfo, argsIndex[3]));
	intcv = atol(pcv);
    }
     if ( ( (pdbname == NULL)   || (strlen(pdbname) == 0) ) && 
	 ( (precordid == NULL) || (strlen(precordid) == 0) ) &&
	 ( (pfield == NULL)    || (strlen(pfield) == 0) ) &&
	 ( (pcv == NULL)       || (strlen(pcv) == 0) ) )
    {
	ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
	fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
	return SLRI_FAIL;
    }

    if (strcmp(pfield, "NULL") == 0)
	pSHDBXref = SHoundDBXREFGetSourceRecordListByDBXref(pdbname, precordid, NULL, intcv);
    else
	pSHDBXref = SHoundDBXREFGetSourceRecordListByDBXref(pdbname, precordid, pfield, intcv);
    
    if (pSHDBXref == NULL)
    {
	ErrPostEx(SEV_ERROR,0,0, "SHoundDBXREFGetSourceRecordListByDBXref returned NULL.");
    }
    
    pauxSHDBXref = pSHDBXref;
    
    fprintf(stdout, "SEQHOUND_OK\n");
    
    if(pSHDBXref != NULL)
    {
	while(pSHDBXref != NULL)
	{
	    fprintf(stdout, "%s,", pSHDBXref->sourcedb);
	    fprintf(stdout, "%s,", pSHDBXref->recordid);
	    fprintf(stdout, "%s,", pSHDBXref->field);
	    fprintf(stdout, "%d", pSHDBXref->cv);
	    if(pSHDBXref->next != NULL)
		fprintf(stdout, ",");
	    pSHDBXref = pSHDBXref->next;
	}
    }
    else 
	fprintf (stdout, "(null)\n");

    MemFree(pdbname);
    MemFree(precordid);
    MemFree(pfield);
    MemFree(pcv);
    SHDBXrefFree(pSHDBXref); 

    return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundDBXREFGetListOfSourceDBNames(void)
{
    ValNodePtr pvncv = NULL;
    CharPtr pstr = NULL;
    Int4 IndexArgs = -1;
    
    printf("Content-type: text/html\r\n\r\n");
    
    pvncv = SHoundDBXREFGetListOfSourceDBNames();

    if (pvncv == NULL)
	ErrPostEx(SEV_ERROR,0,0, "SHoundDBXREFGetListOfSourceDBNames returned NULL.");

    pstr = Misc_SaveStrValNodeListAsString(pvncv);
  
    fprintf(stdout, "SEQHOUND_OK\n");
    
    if(pstr != NULL) 
	fprintf(stdout, "%s\n", pstr);
    else 
	fprintf (stdout, "(null)\n");
    
    MemFree(pstr);
    ValNodeFreeData(pvncv);
    return SLRI_SUCCESS;  
}

SLRI_ERR SeqHoundDBXREFGetListOfXreferedDBNames(void)
{
    ValNodePtr pvncv = NULL;
    CharPtr pstr = NULL;
    Int4 IndexArgs = -1;
    
    printf("Content-type: text/html\r\n\r\n");
    
    pvncv = SHoundDBXREFGetListOfXrefDBNames();

    if (pvncv == NULL)
	ErrPostEx(SEV_ERROR,0,0, "SHoundDBXREFGetListOfXrefDBNames returned NULL.");

    pstr = Misc_SaveStrValNodeListAsString(pvncv);
  
    fprintf(stdout, "SEQHOUND_OK\n");
    
    if(pstr != NULL) 
	fprintf(stdout, "%s\n", pstr);
    else 
	fprintf (stdout, "(null)\n");
    
    MemFree(pstr);
    ValNodeFreeData(pvncv);
    return SLRI_SUCCESS;  
}


SLRI_ERR SeqHoundDBXREFGetListOfAllDBNames(void)
{
    ValNodePtr pvncv = NULL;
    CharPtr pstr = NULL;
    Int4 IndexArgs = -1;
    
    printf("Content-type: text/html\r\n\r\n");
    
    pvncv = SHoundDBXREFGetListOfAllDBNames();

    if (pvncv == NULL)
	ErrPostEx(SEV_ERROR,0,0, "SHoundDBXREFGetListOfAllDBNames returned NULL.");

    pstr = Misc_SaveStrValNodeListAsString(pvncv);
  
    fprintf(stdout, "SEQHOUND_OK\n");
    
    if(pstr != NULL) 
	fprintf(stdout, "%s\n", pstr);
    else 
	fprintf (stdout, "(null)\n");
    
    MemFree(pstr);
    ValNodeFreeData(pvncv);
    return SLRI_SUCCESS;  
}


SLRI_ERR SeqHoundDBXREFGetSourceRecordIDListByDBName(void)
{
    CharPtr pdbname = NULL;
    CharPtr pacc = NULL;
    ValNodePtr pvnStr = NULL;
    CharPtr pstr = NULL;
    Int4 IndexArgs = -1;
  
    printf("Content-type: text/html\r\n\r\n");
    
    if ((IndexArgs = WWWFindName(ginfo, "dbname")) >= 0)
    {	
	pdbname = StringSave(WWWGetValueByIndex(ginfo, IndexArgs));
    }
    if ((pdbname == NULL) || (strlen(pdbname) == 0))
    {
	ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
	fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
	return SLRI_FAIL;
    }

  pvnStr = SHoundDBXREFGetSourceRecordIDListByDBName(pdbname);

  if (pvnStr == NULL)
      ErrPostEx(SEV_ERROR,0,0, "SHoundDBXREFGetSourceRecordIDListByDBName returned NULL.");

  pstr = Misc_SaveStrValNodeListAsString(pvnStr);

  fprintf(stdout, "SEQHOUND_OK\n");

  if(pstr != NULL) 
      fprintf(stdout, "%s\n", pstr);
  else 
      fprintf (stdout, "(null)\n");

  MemFree(pstr);
  ValNodeFreeData(pvnStr);

  return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundDBXREFGetXrefRecordIDListByDBName(void)
{
    CharPtr pdbname = NULL;
    ValNodePtr pvncv = NULL;
    CharPtr pstr = NULL;
    Int4 IndexArgs = -1;
  
    printf("Content-type: text/html\r\n\r\n");
    
    if ((IndexArgs = WWWFindName(ginfo, "dbname")) >= 0)
    {	
	pdbname = StringSave(WWWGetValueByIndex(ginfo, IndexArgs));
    }
    if ((pdbname == NULL) || (strlen(pdbname) == 0))
    {
	ErrPostEx(SEV_ERROR,0,0, " Failed to get parameter value.");
	fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
	return SLRI_FAIL;
    }

    pvncv = SHoundDBXREFGetXrefRecordIDListByDBName(pdbname);

    if (pvncv == NULL)
	ErrPostEx(SEV_ERROR,0,0, "SHoundDBXREFGetXrefRecordIDListByDBName returned NULL.");

    pstr = Misc_SaveStrValNodeListAsString(pvncv);

    fprintf(stdout, "SEQHOUND_OK\n");

    if(pstr != NULL) 
	fprintf(stdout, "%s\n", pstr);
    else 
	fprintf (stdout, "(null)\n");

    MemFree(pdbname);
    ValNodeFreeData(pvncv);

    return SLRI_SUCCESS;
}

SLRI_ERR SeqHoundDBXREFGetCVList(void)
{

  ValNodePtr pvncv = NULL;
  CharPtr pstr = NULL;
  Int4 IndexArgs = -1;
  
  printf("Content-type: text/html\r\n\r\n");

  pvncv = SHoundDBXREFGetCVList();

  if (pvncv == NULL)
      ErrPostEx(SEV_ERROR,0,0, "SHoundDBXREFGetCVList returned NULL.");

  pstr = Misc_SaveStrValNodeListAsString(pvncv);

  fprintf(stdout, "SEQHOUND_OK\n");

  if(pstr != NULL) 
      fprintf(stdout, "%s\n", pstr);
  else 
      fprintf (stdout, "(null)\n");

  MemFree(pstr);
  ValNodeFreeData(pvncv);
  return SLRI_SUCCESS;
}


SLRI_ERR SeqHoundDBXREFGetXrefSourceFieldName(void)
{
    ValNodePtr pvncv = NULL;
    CharPtr pstr = NULL;
    Int4 IndexArgs = -1;
  
    printf("Content-type: text/html\r\n\r\n");
    
    pvncv = SHoundDBXREFGetXrefSourceFieldName();

    if (pvncv == NULL)
	ErrPostEx(SEV_ERROR,0,0, "SHoundDBXREFGetCVList returned NULL.");

    pstr = Misc_SaveStrValNodeListAsString(pvncv);
  
    fprintf(stdout, "SEQHOUND_OK\n");
    
    if(pstr != NULL) 
	fprintf(stdout, "%s\n", pstr);
    else 
	fprintf (stdout, "(null)\n");
    
    MemFree(pstr);
    ValNodeFreeData(pvncv);
    return SLRI_SUCCESS;
}

#endif


/******************
 *RY: March 2004
 *retrives a list of redunant gis associated with the query gis
 *list have query-return sequence pair
 */ 
SLRI_ERR SeqHoundRedundantGroupKeyedList(void)
{
  Int4 IndexArgs = -1;
  CharPtr pcThis = NULL;
  CharPtr pstr = NULL;
  CharPtr pgi = NULL;
  ValNodePtr pvngi = NULL, v=NULL, tmp=NULL;
  ValNodePtr pvngifin = NULL;

  printf("Content-type: text/html\r\n\r\n");
  if ((IndexArgs = WWWFindName(ginfo,"pgi")) >= 0)
  {	
      pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
      pgi = StringSave(pcThis);
  }
  if (pgi == NULL)
  {
      ErrPostEx(SEV_ERROR,0,0, "[SeqHoundRedundantGroupKeyedList]: Failed to get parameter value.");
      fprintf(stdout, "SEQHOUND_ERROR  Failed to get parameter value.\n");
      return SLRI_FAIL;
  }
  /*keep the keyed list in order as it was queried*/
  pvngi = Misc_CommaIntString2ValNodeListOrdered(pgi, ",");
  if(pvngi == NULL)
  {
      ErrPostEx(SEV_ERROR,0,0, "[SeqHoundRedundantGroupKeyedList]: Misc_CommaIntString2ValNodeList failed.");
      fprintf(stdout, "SEQHOUND_ERROR: ValNode conversion error.\n");
      MemFree(pgi);
      return SLRI_FAIL;
  }
   
  pvngifin = SHoundRedundantGroupKeyedList(pvngi);
  /*RY: print out*/
  fprintf(stdout, "SEQHOUND_OK\n");
  for (v= pvngifin; v!=NULL; v=v->next)
  {
      if (v->choice==0)
      {
	   fprintf (stdout, "%ld", (long) v->data.intvalue);
	   fprintf (stdout, Q_R);
      }
      if (v->choice==1)
      {
           tmp = v->data.ptrvalue;
	   if (tmp != NULL)
	   {
	       pstr = Misc_SaveValNodeListAsString(tmp); 
	       fprintf (stdout, "%s", pstr);
	   }
	   else {
	       fprintf (stdout, "NULL");
	   }
	   fprintf (stdout, GROUP);
      }
   }

  MemFree(pgi);
  MemFree(pstr);
  ValNodeFree(pvngifin);
  ValNodeFree(pvngi);
  ValNodeFree(v);
  ValNodeFree(tmp);
  
  return SLRI_SUCCESS;
}


/*********************
 *RY: March, 2004
 *retrives a VolNodeList with query-return pair Fasta data
 *********************/
 SLRI_ERR SeqHoundGetFastaKeyedList(void)
{
   Int4 IndexArgs = -1;
   CharPtr pcThis = NULL;
   CharPtr pgi = NULL;
   ValNodePtr pvngi = NULL, v= NULL, pvnfa = NULL;
   SLRIFastaPtr tmp =NULL;
   AsnIoPtr aip = NULL;

   if ((IndexArgs = WWWFindName(ginfo,"pgi")) >= 0)
    {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       pgi = StringSave(pcThis);
    }
   if (pgi == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "[SeqHoundGetFastaKeyedList]: Failed to get parameter value.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR: Failed to get parameter value.\n");
       return SLRI_FAIL;
    }
   /*keep the keyed list in order as it was queried*/
   pvngi = Misc_CommaIntString2ValNodeListOrdered(pgi, ",");
   if(pvngi == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "[SeqHoundGetFastaKeyedList]: Misc_CommaIntString2ValNodeList failed.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR : Misc_CommaIntString2ValNodeList failed.\n");
       MemFree(pgi);
       return SLRI_FAIL;
    }
  
    pvnfa = SHoundGetFastaKeyedList(pvngi);

#ifdef NETENTREZON     
   /* accessing net entrez */
   if( NULL == pvnfa && NetEntrezOn )
   {
     for( v = pvngi; v != NULL; v = v->next )
     { 
        gi = v->data.intvalue;
	ValNodeAddInt(&pvnfa, 0, gi); /*RY: add the query node, choice ==0 and data.intvalue=gi*/
        if((p = NetEntrezGetFasta(gi)) != NULL)
        {
	   pfa = SLRIFastaNew();
           pfa->sequence = sequence = (char*) MemNew(StringLen(p) * sizeof(Char));
           ps = strtok(p, "\n");
           pfa->defline = defline = (char*) MemNew(sizeof(char) * (StringLen(ps) + 1));

           if( NULL == sequence || NULL == defline )
           {
              ErrPostEx(SEV_ERROR, 0, 0, "[SHoundGetFastaListKeyedList]: Allocation error.");
              printf("Content-type: text/html\r\n\r\n");
              fprintf(stdout, "SEQHOUND_ERROR [SHoundGetFastaListKeyedList]: Allocation error.");
              MemFree(pgi); ValNodeFree(pvngi); ValNodeFree(pvnfa);
              return SLRI_FAIL;
           }
           strcpy(pfa->defline, ++ps);
           while((ps = strtok(NULL, "\n")) != NULL)
	   {
              pfa->sequence = strcat(pfa->sequence, ps);
	   }
           /*RY: add return value node, set choice==1, data= SLRIFasta structure*/
	   pvnfa = Slri_ValNodeAddPointerAtEnd(&pvnfa, 1, pfa);
        } 
	else  
	{
	    pvnfa = Slri_ValNodeAddPointerAtEnd(&pvnfa, 1, NULL); 
	}
	       
     } /* end for */
   } /* end if */

#endif

   if(pvnfa == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "[SHoundGetFastaKeyedList] returned NULL.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
       MemFree(pgi);
       ValNodeFree(pvngi);
       return SLRI_FAIL;
    }

   printf("Content-type: text/html\r\n\r\n");
   fprintf(stdout, "SEQHOUND_OK\n");
   
   /*print out*/
   for( v = pvnfa; v != NULL; v = v->next )
   {
      if (v->choice==0)
      {
	   fprintf (stdout, "%ld", (long) v->data.intvalue);
	   fprintf (stdout, Q_R);
      }

      if (v->choice==1)
      {
          tmp = v->data.ptrvalue;
	  if (tmp != NULL)
	  {
              if((aip=AsnIoNew(ASNIO_TEXT_OUT, stdout, NULL,NULL,NULL)) == NULL)
              {
                  ErrPostEx(SEV_ERROR,0,0, "[SeqHoundGetFastaKeyedList]: Unable to open ASN.1 IO.");
                  printf("Content-type: text/html\r\n\r\n");
                  fprintf(stdout, "SEQHOUND_ERROR Unable to open ASN.1 IO.\n");
              }
	  
	      else
	      {
	          SLRIFastaAsnWrite((SLRIFastaPtr)tmp, aip, NULL);
	      }
	      AsnIoClose(aip);
	  }
	  else
	  {
	      fprintf (stdout, "NULL");
	  }
	  fprintf (stdout, GROUP);
      }
   }
   
   if (pvnfa) ValNodeFree(pvnfa);
   if (pvngi) ValNodeFree(pvngi);
   if (v) ValNodeFree(v);
   if (pgi) MemFree(pgi);
   
   return SLRI_SUCCESS;
}


/* Retrieves the fasta structure with the query redundant group id */ 
SLRI_ERR SeqHoundGetFastaFromRedundantGroupID(void)
{
   Int4 IndexArgs = -1;
   CharPtr pcThis = NULL;
   CharPtr gidptr = NULL;
   Int4 gidval = 0;
   SLRIFastaPtr pfa = NULL;
   AsnIoPtr aip = NULL;


   if ((IndexArgs = WWWFindName(ginfo,"gid")) >= 0)
    {	
        pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
        gidptr = StringSave(pcThis);
    }
   if (gidptr == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "[SeqHoundGetFastaFromRedundantGroupID]: Failed to get parameter value.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
    }
   if(!StrToInt4 (gidptr, &gidval))
    {
      ErrPostEx(SEV_ERROR,0,0, "[SeqHoundGetFastaFromRedundantGroupID]: StrToInt4 Invalid function parameter.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(gidptr);
      return SLRI_FAIL;
    }
   pfa = SHoundGetFastaFromRedundantGroupID(gidval);
   if (pfa == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "[SeqHoundGetFastaFromRedundantGroupID] returned NULL.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
       MemFree(gidptr);
       return SLRI_FAIL;
    }
   if((aip=AsnIoNew(ASNIO_TEXT_OUT, stdout, NULL,NULL,NULL)) == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "[SeqHoundGetFastaFromRedundantGroupID]: Unable to open ASN.1 IO.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Unable to open ASN.1 IO.\n");
       MemFree(gidptr);
       SLRIFastaFree(pfa);
       return SLRI_FAIL;
    }
   printf("Content-type: text/html\r\n\r\n");
   fprintf(stdout, "SEQHOUND_OK\n");
   SLRIFastaAsnWrite(pfa, aip, NULL);
   AsnIoClose(aip);
   SLRIFastaFree(pfa);
   MemFree(gidptr);
   return SLRI_SUCCESS;

}


/* Retrieves the fasta structure (in plain text, include defline and sequence) with the query redundant group id */ 
SLRI_ERR SeqHoundGetFastaFromRedundantGroupIDPL(void)
{
   Int4 IndexArgs = -1;
   CharPtr pcThis = NULL;
   CharPtr gidptr = NULL;
   Int4 gidval = 0;
   SLRIFastaPtr pfa = NULL;
   AsnIoPtr aip = NULL;


   if ((IndexArgs = WWWFindName(ginfo,"gid")) >= 0)
    {	
        pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
        gidptr = StringSave(pcThis);
    }
   if (gidptr == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "[SeqHoundGetFastaFromRedundantGroupIDPL]: Failed to get parameter value.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
    }
   if(!StrToInt4 (gidptr, &gidval))
    {
      ErrPostEx(SEV_ERROR,0,0, "[SeqHoundGetFastaFromRedundantGroupIDPL]: StrToInt4 Invalid function parameter.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(gidptr);
      return SLRI_FAIL;
    }
   pfa = SHoundGetFastaFromRedundantGroupID(gidval);
   if (pfa == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "[SeqHoundGetFastaFromRedundantGroupIDPL] returned NULL.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
       MemFree(gidptr);
       return SLRI_FAIL;
    }
   printf("Content-type: text/html\r\n\r\n");
   fprintf(stdout, "SEQHOUND_OK\n");
   if(pfa->defline != NULL) fprintf(stdout, ">%s\n", pfa->defline);
   if(pfa->sequence != NULL) fprintf(stdout, "%s\n", pfa->sequence);
   SLRIFastaFree(pfa);
   MemFree(gidptr);
   return SLRI_SUCCESS;

}


/* Retrieves the a list of fasta structure with the query list of redundant group ids */ 
SLRI_ERR SeqHoundGetFastaFromRedundantGroupIDKeyedList(void)
{

   Int4 IndexArgs = -1;
   CharPtr pcThis = NULL;
   CharPtr pgid = NULL;
   ValNodePtr pvngid = NULL, v= NULL, pvnfa = NULL;
   SLRIFastaPtr tmp =NULL;
   AsnIoPtr aip = NULL;

   if ((IndexArgs = WWWFindName(ginfo,"pgid")) >= 0)
    {	
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       pgid = StringSave(pcThis);
    }
   if (pgid == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "[SeqHoundGetFastaFromRedundantGroupIDKeyedList]: Failed to get parameter value.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR: Failed to get parameter value.\n");
       return SLRI_FAIL;
    }
   /*keep the keyed list in order as it was queried*/
   pvngid = Misc_CommaIntString2ValNodeListOrdered(pgid, ",");
   if(pvngid == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "[SeqHoundGetFastaFromRedundantGroupIDKeyedList]: Misc_CommaIntString2ValNodeList failed.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR: Misc_CommaIntString2ValNodeList failed.\n");
       MemFree(pgid);
       return SLRI_FAIL;
    }
  
    pvnfa = SHoundGetFastaFromRedundantGroupIDKeyedList(pvngid);
    if(pvnfa == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "[SHoundGetFastaFromRedundantGroupIDKeyedList] returned NULL.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
       MemFree(pgid);
       ValNodeFree(pvngid);
       return SLRI_FAIL;
    }

   printf("Content-type: text/html\r\n\r\n");
   fprintf(stdout, "SEQHOUND_OK\n");
   
   /*print out*/
   for( v = pvnfa; v != NULL; v = v->next )
   {
      if (v->choice==0)
      {
	   fprintf (stdout, "%ld", (long) v->data.intvalue);
	   fprintf (stdout, Q_R);
      }

      if (v->choice==1)
      {
          tmp = v->data.ptrvalue;
	  if (tmp != NULL)
	  {
              if((aip=AsnIoNew(ASNIO_TEXT_OUT, stdout, NULL,NULL,NULL)) == NULL)
              {
                  ErrPostEx(SEV_ERROR,0,0, "[SeqHoundGetFastaFromRedundantGroupIDKeyedList]: Unable to open ASN.1 IO.");
                  printf("Content-type: text/html\r\n\r\n");
                  fprintf(stdout, "SEQHOUND_ERROR Unable to open ASN.1 IO.\n");
              }
	  
	      else
	      {
	          SLRIFastaAsnWrite((SLRIFastaPtr)tmp, aip, NULL);
	      }
	      AsnIoClose(aip);
	  }
	  else
	  {
	      fprintf (stdout, "NULL");
	  }
	  fprintf (stdout, GROUP);
      }
   }
   if (pvnfa) ValNodeFree(pvnfa);
   if (pvngid) ValNodeFree(pvngid);
   if (v) ValNodeFree(v);
   if (pgid) MemFree(pgid);
   
   return SLRI_SUCCESS;

}


/* Retrieves SLRI fasta structure (in plain text, include defline and sequence) with the query gi */ 
SLRI_ERR SeqHoundIntrezGetCDSLRIFastaPL(void)
{
   Int4 IndexArgs = -1;
   CharPtr pcThis = NULL;
   CharPtr giptr = NULL;
   Int4 gival = 0;
   SLRIFastaPtr pfa = NULL;


   if ((IndexArgs = WWWFindName(ginfo,"gi")) >= 0)
    {	
        pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
        giptr = StringSave(pcThis);
    }
   if (giptr == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "[SeqHoundIntrezGetCDSLRIFastaPL]: Failed to get parameter value.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
    }
   if(!StrToInt4 (giptr, &gival))
    {
      ErrPostEx(SEV_ERROR,0,0, "[SeqHoundIntrezGetCDSLRIFastaPL]: StrToInt4 Invalid function parameter.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(giptr);
      return SLRI_FAIL;
    }
   pfa = IntrezGetCDSLRIFasta(gival);
   if (pfa == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "[SeqHoundIntrezGetCDSLRIFastaPL] returned NULL.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
       MemFree(giptr);
       return SLRI_FAIL;
    }
   printf("Content-type: text/html\r\n\r\n");
   fprintf(stdout, "SEQHOUND_OK\n");
   if(pfa->defline != NULL) fprintf(stdout, ">%s\n", pfa->defline);
   if(pfa->sequence != NULL) fprintf(stdout, "%s\n", pfa->sequence);
   SLRIFastaFree(pfa);
   MemFree(giptr);
   return SLRI_SUCCESS;
}

/* Retrieves SLRI fasta structure with the query gi, bug 2929*/ 
SLRI_ERR SeqHoundIntrezGetCDSLRIFasta(void)
{
   Int4 IndexArgs = -1;
   CharPtr pcThis = NULL;
   CharPtr giptr = NULL;
   Int4 gival = 0;
   SLRIFastaPtr pfa = NULL;
   AsnIoPtr aip = NULL;


   if ((IndexArgs = WWWFindName(ginfo,"gi")) >= 0)
    {	
        pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
        giptr = StringSave(pcThis);
    }
   if (giptr == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "[SeqHoundIntrezGetCDSLRIFasta]: Failed to get parameter value.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Failed to get parameter value.\n");
       return SLRI_FAIL;
    }
   if(!StrToInt4 (giptr, &gival))
    {
      ErrPostEx(SEV_ERROR,0,0, "[SeqHoundIntrezGetCDSLRIFasta]: StrToInt4 Invalid function parameter.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Invalid function parameter.\n");
      MemFree(giptr);
      return SLRI_FAIL;
    }
   pfa = IntrezGetCDSLRIFasta(gival);
   if (pfa == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "[SeqHoundIntrezGetCDSLRIFasta] returned NULL.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_NULL NULL return value.\n");
       MemFree(giptr);
       return SLRI_FAIL;
    }
   if((aip=AsnIoNew(ASNIO_TEXT_OUT, stdout, NULL,NULL,NULL)) == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "Unable to open ASN.1 IO.");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR Unable to open ASN.1 IO.\n");
       MemFree(giptr);
       SLRIFastaFree(pfa);
       return SLRI_FAIL;
    }
   printf("Content-type: text/html\r\n\r\n");
   fprintf(stdout, "SEQHOUND_OK\n");
   SLRIFastaAsnWrite(pfa, aip, NULL);
   AsnIoClose(aip);
   SLRIFastaFree(pfa);
   MemFree(giptr);
   return SLRI_SUCCESS;
    
}


SLRI_ERR ProcessFnctRequest(CharPtr pfnct)
{
   if (strcmp(pfnct, "SeqHoundInit") == 0) {
      if(SeqHoundInit() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundIsNetEntrezOn") == 0) {
      if(SeqHoundIsNetEntrezOn() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundNetEntrezInit") == 0) {
      if(SeqHoundNetEntrezInit() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundFini") == 0) {
      if(SeqHoundFini() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGetAccFromRetiredGi") == 0) {
      if(SeqHoundGetAccFromRetiredGi() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundFindAcc") == 0) {
      if(SeqHoundFindAcc() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundFindAccList") == 0) {
      if(SeqHoundFindAccList() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundFindName") == 0) {
      if(SeqHoundFindName() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundFindNameList") == 0) {
      if(SeqHoundFindNameList() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundAccFromGi") == 0) {
      if(SeqHoundAccFromGi() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundAccFromGiList") == 0) {
      if(SeqHoundAccFromGiList() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundTitleFromGi") == 0) {
      if(SeqHoundTitleFromGi() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundTitleFromGiList") == 0) {
      if(SeqHoundTitleFromGiList() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundSeqIdFromGi") == 0) {
      if(SeqHoundSeqIdFromGi() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundSeqIdFromGiList") == 0) {
      if(SeqHoundSeqIdFromGiList() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundSeqIdFromGiPL") == 0) {
      if(SeqHoundSeqIdFromGiPL() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundSeqIdFromGiListPL") == 0) {
      if(SeqHoundSeqIdFromGiListPL() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundMoleculeType") == 0){
       if(SeqHoundMoleculeType() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundIsProtein") == 0) {
      if(SeqHoundIsProtein() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundIsNRFirst") == 0) {
      if(SeqHoundIsNRFirst() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundExists") == 0) {
      if(SeqHoundExists() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHound3DExists") == 0) {
      if(SeqHound3DExists() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGetLargestMMDBID") == 0) {
      if(SeqHoundGetLargestMMDBID() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHound3DbWhat") == 0) {
      if(SeqHound3DbWhat() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGetBioseq") == 0) {
      if(SeqHoundGetBioseq() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGetBioseqList") == 0) {
      if(SeqHoundGetBioseqList() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGetBioseqPL") == 0) {
      if(SeqHoundGetBioseqPL() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGetBioseqListPL") == 0) {
      if(SeqHoundGetBioseqListPL() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGetBioseqSet") == 0) {
      if(SeqHoundGetBioseqSet() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGetBioseqSetList") == 0) {
      if(SeqHoundGetBioseqSetList() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGetBioseqSetPL") == 0) {
      if(SeqHoundGetBioseqSetPL() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGetBioseqSetListPL") == 0) {
      if(SeqHoundGetBioseqSetListPL() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGetSeqEntry") == 0) {
      if(SeqHoundGetSeqEntry() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGetSeqEntryList") == 0) {
      if(SeqHoundGetSeqEntryList() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGetSeqEntryPL") == 0) {
      if(SeqHoundGetSeqEntryPL() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGetSeqEntryListPL") == 0) {
      if(SeqHoundGetSeqEntryListPL() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGetFasta") == 0) {
      if(SeqHoundGetFasta() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGetFastaList") == 0) {
      if(SeqHoundGetFastaList() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGetFastaPL") == 0) {
      if(SeqHoundGetFastaPL() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGetFastaListPL") == 0) {
      if(SeqHoundGetFastaListPL() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGet3D") == 0) {
      if(SeqHoundGet3D() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }   else if (strcmp(pfnct, "SeqHoundGetFixed3D") == 0) {
      if(SeqHoundGetFixed3D() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGet3DEx") == 0) {
      if(SeqHoundGet3DEx() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGet3DPL") == 0) {
      if(SeqHoundGet3DPL() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGet3DfromPdbId") == 0) {
      if(SeqHoundGet3DfromPdbId() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGet3DfromPdbIdEx") == 0) {
      if(SeqHoundGet3DfromPdbIdEx() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGet3DfromPdbIdPL") == 0) {
      if(SeqHoundGet3DfromPdbIdPL() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundPDBFromMMDB") ==0 ) {
      if(SeqHoundPDBFromMMDB() == SLRI_FAIL){
        MemFree(pfnct);
	return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGiFrom3D") == 0) {
      if(SeqHoundGiFrom3D() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGiFrom3DList") == 0) {
      if(SeqHoundGiFrom3DList() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGiFromPDBchain") == 0) {
      if(SeqHoundGiFromPDBchain() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHound3DFromGi") == 0) {
      if(SeqHound3DFromGi() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHound3DFromGiList") == 0) {
      if(SeqHound3DFromGiList() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundDNAFromProtein") == 0) {
      if(SeqHoundDNAFromProtein() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundDNAFromProteinList") == 0) {
      if(SeqHoundDNAFromProteinList() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundProteinFromDNA") == 0) {
      if(SeqHoundProteinFromDNA() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundProteinFromDNAList") == 0) {
      if(SeqHoundProteinFromDNAList() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGetReferenceIDFromGi") == 0) {
      if(SeqHoundGetReferenceIDFromGi() == SLRI_FAIL){
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundMuidFromGi") == 0) {
      if(SeqHoundMuidFromGi() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGetReferenceIDFromGiList") == 0) {
      if(SeqHoundGetReferenceIDFromGiList() == SLRI_FAIL){
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundMuidFromGiList") == 0) {
      if(SeqHoundMuidFromGiList() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGiFromReferenceID") == 0) {
      if(SeqHoundGiFromReferenceID() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGiFromReferenceList") == 0) {
      if(SeqHoundGiFromReferenceList() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundMuidFrom3D") == 0) {
      if(SeqHoundMuidFrom3D() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundMuidFrom3DList") == 0) {
      if(SeqHoundMuidFrom3DList() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundTaxIDFromGi") == 0) {
      if(SeqHoundTaxIDFromGi() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundTaxIDFromGiList") == 0) {
      if(SeqHoundTaxIDFromGiList() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundTaxIDFrom3D") == 0) {
      if(SeqHoundTaxIDFrom3D() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundTaxIDFrom3DList") == 0) {
      if(SeqHoundTaxIDFrom3DList() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundProteinsFromTaxID") == 0) {
      if(SeqHoundProteinsFromTaxID() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundProteinsFromTaxIDIII") == 0) {
      if(SeqHoundProteinsFromTaxIDIII() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundProteinsFromTaxIDList") == 0) {
      if(SeqHoundProteinsFromTaxIDList() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundDNAFromTaxID") == 0) {
      if(SeqHoundDNAFromTaxID() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundDNAFromTaxIDIII") == 0) {
      if(SeqHoundDNAFromTaxIDIII() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundDNAFromTaxIDList") == 0) {
      if(SeqHoundDNAFromTaxIDList() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundProteinsFromOrganism") == 0) {
      if(SeqHoundProteinsFromOrganism() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundProteinsFromOrganismList") == 0) {
      if(SeqHoundProteinsFromOrganismList() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundProteinsFromChromosome") == 0) {
      if(SeqHoundProteinsFromChromosome() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundProteinsFromChromosomeList") == 0) {
      if(SeqHoundProteinsFromChromosomeList() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundAllGenomes") == 0) {
      if(SeqHoundAllGenomes() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundChromosomeFromGenome") == 0) {
      if(SeqHoundChromosomeFromGenome() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundChromosomeFromGenomeList") == 0) {
      if(SeqHoundChromosomeFromGenomeList() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundDNAFromOrganism") == 0) {
      if(SeqHoundDNAFromOrganism() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundRNAFromOrganism") == 0) {
      if(SeqHoundRNAFromOrganism() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundDNAFromOrganismList") == 0) {
      if(SeqHoundDNAFromOrganismList() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundDNAFromUnknownChromosome") == 0) {
      if(SeqHoundDNAFromUnknownChromosome() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundRNAFromUnknownChromosome") == 0) {
	 if(SeqHoundRNAFromUnknownChromosome() == SLRI_FAIL) {
	   MemFree(pfnct);
	   return 1;
	 }
   }
   else if (strcmp(pfnct, "SeqHoundProteinsFromUnknownChromosome") == 0) {
      if(SeqHoundProteinsFromUnknownChromosome() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundDNAFromChromosome") == 0) {
      if(SeqHoundDNAFromChromosome() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundRNAFromChromosome") == 0) {
	 if(SeqHoundRNAFromChromosome() == SLRI_FAIL) {
	   MemFree(pfnct);
	   return 1;
	 }
   }
   else if (strcmp(pfnct, "SeqHoundDNAFromChromosomeList") == 0) {
      if(SeqHoundDNAFromChromosomeList() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundRedundantGroup") == 0) {
      if(SeqHoundRedundantGroup() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundRedundantGroupList") == 0) {
      if(SeqHoundRedundantGroupList() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundRedundantGroupIDFromGI") == 0) {
      if(SeqHoundRedundantGroupIDFromGI() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundRedundantGroupIDFromGIList") == 0) {
      if(SeqHoundRedundantGroupIDFromGIList() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundRedundantGroupFromID") == 0) {
      if(SeqHoundRedundantGroupFromID() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundRedundantGroupFromIDList") == 0) {
      if(SeqHoundRedundantGroupFromIDList() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundFirstOfRedundantGroupFromID") == 0) {
      if(SeqHoundFirstOfRedundantGroupFromID() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundFirstOfRedundantGroupFromIDList") == 0) {
      if(SeqHoundFirstOfRedundantGroupFromIDList() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGetTaxChildNodes") == 0) {
      if(SeqHoundGetTaxChildNodes() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGetTaxChildNodesList") == 0) {
      if(SeqHoundGetTaxChildNodesList() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGetAllTaxProgeny") == 0) {
      if(SeqHoundGetAllTaxProgeny() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGetTaxParent") == 0) {
      if(SeqHoundGetTaxParent() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGetAllTaxAncestors") == 0) {
      if(SeqHoundGetAllTaxAncestors() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundIsTaxMerged") == 0) {
      if(SeqHoundIsTaxMerged() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGetNewTaxIDFromMergedTaxID") == 0) {
      if(SeqHoundGetNewTaxIDFromMergedTaxID() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundIsTaxDeleted") == 0) {
      if(SeqHoundIsTaxDeleted() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGetTaxNameFromTaxIDByClass") == 0) {
      if(SeqHoundGetTaxNameFromTaxIDByClass() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGetTaxNameFromTaxID") == 0) {
      if(SeqHoundGetTaxNameFromTaxID() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGetTaxLineageFromTaxID") == 0) {
      if(SeqHoundGetTaxLineageFromTaxID() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGetGenBankff") == 0) {
      if(SeqHoundGetGenBankff() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGetGenBankffList") == 0) {
      if(SeqHoundGetGenBankffList() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundXMLBioseqPL") == 0) {
      if(SeqHoundXMLBioseqPL() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGetXMLSeqEntryPL") == 0) {
      if(SeqHoundGetXMLSeqEntryPL() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGetXML3DPL") == 0) {
      if(SeqHoundGetXML3DPL() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGetPDB3DPL") == 0) {
      if(SeqHoundGetPDB3DPL() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGetDefline") == 0) {
      if(SeqHoundGetDefline() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGetDeflineList") == 0) {
      if(SeqHoundGetDeflineList() == SLRI_FAIL) {
        MemFree(pfnct);
        return 1;
      }
   }
   else if (strcmp(pfnct, "SeqHoundGetBlastResult") == 0) {
     if(SeqHoundGetBlastResult()) {
			MemFree(pfnct);
			return 1;
		}
   } else if (strcmp(pfnct, "SeqHoundGetBlastSeqAlign") == 0) {
     if(SeqHoundGetBlastSeqAlign()) {
			MemFree(pfnct);
			return 1;
		}
   } else if (strcmp(pfnct, "SeqHoundNeighboursFromGiEx") == 0) {
     if(SeqHoundNeighboursFromGiEx()) {
			MemFree(pfnct);
			return 1;
		}
   } else if (strcmp(pfnct, "SeqHoundNeighboursFromGi") == 0) {
     if(SeqHoundNeighboursFromGi()) {
			MemFree(pfnct);
			return 1;
                }
   } else if (strcmp(pfnct, "SeqHoundNeighboursFromGiPL") == 0) {
     if(SeqHoundNeighboursFromGiPL()) {
			MemFree(pfnct);
			return 1;
		}
   }else if (strcmp(pfnct, "SeqHoundNeighboursFromGiList") == 0) {
     if(SeqHoundNeighboursFromGiList()) {
			MemFree(pfnct);
			return 1;
		}
   } else if (strcmp(pfnct, "SeqHoundNeighboursFromGiListPL") == 0) {
     if(SeqHoundNeighboursFromGiListPL()) {
			MemFree(pfnct);
			return 1;
		}
   } else if (strcmp(pfnct, "SeqHoundNeighboursFromTaxID") == 0) {
     if(SeqHoundNeighboursFromTaxID()) {
			MemFree(pfnct);
			return 1;
		}
   }else if (strcmp(pfnct, "SeqHoundNeighboursFromTaxIDPL") == 0) {
     if(SeqHoundNeighboursFromTaxIDPL()) {
			MemFree(pfnct);
			return 1;
		}
   } else if (strcmp(pfnct, "SeqHoundNeighboursOfNeighbours") == 0) {
     if(SeqHoundNeighboursOfNeighbours()) {
			MemFree(pfnct);
			return 1;
		}
   } else if (strcmp(pfnct, "SeqHoundNeighboursOfNeighboursList") == 0) {
     if(SeqHoundNeighboursOfNeighboursList()) {
			MemFree(pfnct);
			return 1;
		}
   } else if (strcmp(pfnct, "SeqHoundNeighboursOfNeighboursFromTaxID") == 0) {
     if(SeqHoundNeighboursOfNeighboursFromTaxID()) {
			MemFree(pfnct);
			return 1;
		}
   } else if (strcmp(pfnct, "SeqHound3DNeighboursFromGiEx") == 0) {
     if(SeqHound3DNeighboursFromGiEx()) {
			MemFree(pfnct);
			return 1;
		}
   } else if (strcmp(pfnct, "SeqHound3DNeighboursFromGi") == 0) {
     if(SeqHound3DNeighboursFromGi()) {
			MemFree(pfnct);
			return 1;
		}
   } else if (strcmp(pfnct, "SeqHound3DNeighboursFromGiPL") == 0) {
     if(SeqHound3DNeighboursFromGiPL()) {
			MemFree(pfnct);
			return 1;
		}
   } else if (strcmp(pfnct, "SeqHound3DNeighboursFromGiList") == 0) {
     if(SeqHound3DNeighboursFromGiList()) {
			MemFree(pfnct);
			return 1;
		}
   } else if (strcmp(pfnct, "SeqHound3DNeighboursFromGiListPL") == 0) {
     if(SeqHound3DNeighboursFromGiListPL()) {
			MemFree(pfnct);
			return 1;
		}
   } else if (strcmp(pfnct, "SeqHound3DNeighboursFromTaxID") == 0) {
     if(SeqHound3DNeighboursFromTaxID()) {
			MemFree(pfnct);
			return 1;
		}
   } else if (strcmp(pfnct, "SeqHound3DNeighboursFromTaxIDPL") == 0) {
     if(SeqHound3DNeighboursFromTaxIDPL()) {
			MemFree(pfnct);
			return 1;
		}
   } else if (strcmp(pfnct, "SeqHound3DListFromTaxID") == 0) {
     if(SeqHound3DListFromTaxID()) {
			MemFree(pfnct);
			return 1;
		}
   } else if (strcmp(pfnct, "SeqHound3DListFromTaxIDPL") == 0) {
     if(SeqHound3DListFromTaxIDPL()) {
			MemFree(pfnct);
			return 1;
		}
   } else if(strcmp(pfnct, "SeqHoundSequenceLength") == 0) {
     if(SeqHoundSequenceLength()) {
              		MemFree(pfnct);
	      		return 1;
	   	}
   } else if (strcmp(pfnct, "SeqHoundGiAndNumNeighboursList") == 0) {
      if(SeqHoundGiAndNumNeighboursList()) {
			MemFree(pfnct);
			return 1;
		}
   } else if (strcmp(pfnct, "SeqHoundNumNeighboursInDB") == 0) {
      if(SeqHoundNumNeighboursInDB()) {
			MemFree(pfnct);
			return 1;
		}
   }
   else if (strcmp(pfnct, "SeqHoundGOIDFromLLID") == 0) {
     if(SeqHoundGOIDFromLLID()){
       MemFree(pfnct);
       return 1;
     }
   }
   else if (strcmp(pfnct, "SeqHoundOMIMFromLLID") == 0) {
     if(SeqHoundOMIMFromLLID()){
       MemFree(pfnct);
       return 1;
     }
   }
   else if (strcmp(pfnct, "SeqHoundCDDIDFromLLID") == 0) {
     if(SeqHoundCDDIDFromLLID()){
       MemFree(pfnct);
       return 1;
     }
   }
   else if (strcmp(pfnct, "SeqHoundLLIDFromGOIDAndECode") == 0) {
     if(SeqHoundLLIDFromGOIDAndECode()){
       MemFree(pfnct);
       return 1;
     }
   }
   else if (strcmp(pfnct, "SeqHoundLLIDFromOMIM") == 0) {
     if(SeqHoundLLIDFromOMIM()){
       MemFree(pfnct);
       return 1;
     }
   }
   else if (strcmp(pfnct, "SeqHoundLLIDFromCDDID") == 0) {
     if(SeqHoundLLIDFromCDDID()){
       MemFree(pfnct);
       return 1;
     }
   }
   else if (strcmp(pfnct, "SeqHoundGiFromGOID") == 0) {
     if(SeqHoundGiFromGOID()){
       MemFree(pfnct);
       return 1;
     }
   }
   else if (strcmp(pfnct, "SeqHoundGiFromGOIDAndECode") == 0) {
     if(SeqHoundGiFromGOIDAndECode()){
       MemFree(pfnct);
       return 1;
     }
   } 
   else if (strcmp(pfnct, "SeqHoundGiFromGOIDListAndECode") == 0) {
     if(SeqHoundGiFromGOIDListAndECode()){
       MemFree(pfnct);
       return 1;
     }
   } 
   else if (strcmp(pfnct, "SeqHoundGiFromGOIDList") == 0) {
     if(SeqHoundGiFromGOIDList()){
       MemFree(pfnct);
       return 1;
     }
   } else if (strcmp(pfnct, "SeqHoundGOIDFromGi") == 0) {
     if(SeqHoundGOIDFromGi()){
       MemFree(pfnct);
       return 1;
     }
   } else if (strcmp(pfnct, "SeqHoundGOIDFromGiList") == 0) {
     if(SeqHoundGOIDFromGiList()){
       MemFree(pfnct);
       return 1;
     }
   } else if (strcmp(pfnct, "SeqHoundGOIDFromRedundantGi") == 0) {
     if(SeqHoundGOIDFromRedundantGi()){
       MemFree(pfnct);
       return 1;
     }
   } else if (strcmp(pfnct, "SeqHoundGOIDFromRedundantGiList") == 0) {
     if(SeqHoundGOIDFromRedundantGiList()){
       MemFree(pfnct);
       return 1;
     }
   } else if (strcmp(pfnct, "SeqHoundGOECodeFromGiAndGOID") == 0) {
     if(SeqHoundGOECodeFromGiAndGOID()){
       MemFree(pfnct);
       return 1;
     }
   }  else if (strcmp(pfnct, "SeqHoundGOPMIDFromGiAndGOID") == 0) {
     if(SeqHoundGOPMIDFromGiAndGOID()){
       MemFree(pfnct);
       return 1;
     }
   } else if (strcmp(pfnct, "SeqHoundGiFromOMIMList") == 0) {
     if(SeqHoundGiFromOMIMList()) {
       MemFree(pfnct);
       return 1;
     }
   } else if (strcmp(pfnct, "SeqHoundGiFromOMIM") == 0) {
     if(SeqHoundGiFromOMIM()) {
       MemFree(pfnct);
       return 1;
     }
   } else if (strcmp(pfnct, "SeqHoundOMIMFromGi") == 0) {
     if(SeqHoundOMIMFromGi()) {
       MemFree(pfnct);
       return 1;
     }
   } else if (strcmp(pfnct, "SeqHoundOMIMFromGiList") == 0) {
     if(SeqHoundOMIMFromGiList()) {
       MemFree(pfnct);
       return 1;
     }
   } else if (strcmp(pfnct, "SeqHoundGiFromCDDID") == 0) {
     if(SeqHoundGiFromCDDID()) {
       MemFree(pfnct);
       return 1;
     }
   } else if (strcmp(pfnct, "SeqHoundGiFromCDDIDList") == 0) {
     if(SeqHoundGiFromCDDIDList()) {
       MemFree(pfnct);
       return 1;
     }
   } else if (strcmp(pfnct, "SeqHoundCDDIDFromGi") == 0) {
      if(SeqHoundCDDIDFromGi()) {
      MemFree(pfnct);
      return 1;
      }
   } else if(strcmp(pfnct, "SeqHoundCDDIDFromGiList") == 0) {
     if(SeqHoundCDDIDFromGiList()) {
       MemFree(pfnct);
       return 1;
     }
   } else if (strcmp(pfnct, "SeqHoundCDDScoreFromGi") == 0) {
     if(SeqHoundCDDScoreFromGi()) {
       MemFree(pfnct);
       return 1;
     }
   } else if (strcmp(pfnct, "SeqHoundGiFromLLID") == 0) {
     if (SeqHoundGiFromLLID()) {
       MemFree(pfnct);
       return 1;
     }
   } else if (strcmp(pfnct, "SeqHoundGiFromLLIDList") == 0) {
     if(SeqHoundGiFromLLIDList()) {
       MemFree(pfnct);
       return 1;
     }
   } else if (strcmp(pfnct, "SeqHoundLLIDFromGi") == 0) {
     if(SeqHoundLLIDFromGi()) {
       MemFree(pfnct);
       return 1;
     }
   } else if (strcmp(pfnct, "SeqHoundLLIDFromGiList") == 0) {
     if(SeqHoundLLIDFromGiList()) {
       MemFree(pfnct);
       return 1;
     }
   } else if (strcmp(pfnct, "SeqHoundLocusFromGi") == 0) {
     if(SeqHoundLocusFromGi() ) {
       MemFree(pfnct);
       return 1;
     }
   } /*else if (strcmp(pfnct, "SeqHoundGODBGetRecordByID") == 0) {
     if(SeqHoundGODBGetRecordByID() ) {
       MemFree(pfnct);
       return 1;
     }
   }*/ else if (strcmp(pfnct, "SeqHoundGODBGetRecordByReference") == 0) {
     if(SeqHoundGODBGetRecordByReference() ) {
       MemFree(pfnct);
       return 1;
     }
   } else if (strcmp(pfnct, "SeqHoundGODBGetChildrenOf") == 0) {
     if(SeqHoundGODBGetChildrenOf() ) {
       MemFree(pfnct);
       return 1;
     }
   } else if (strcmp(pfnct, "SeqHoundGODBGetParentOf") == 0) {
     if(SeqHoundGODBGetParentOf() ) {
       MemFree(pfnct);
       return 1;
     }
   } else if (strcmp(pfnct, "SeqHoundGODBGetAllAncestors") == 0) {
     if(SeqHoundGODBGetAllAncestors() ) {
       MemFree(pfnct);
       return 1;
     }
   } else if (strcmp(pfnct, "SeqHoundGODBGetAllChildren") == 0) {
     if(SeqHoundGODBGetAllChildren() ) {
       MemFree(pfnct);
       return 1;
     }
   } else if (strcmp(pfnct, "SeqHoundGODBGetNameByID") == 0) {
     if(SeqHoundGODBGetNameByID() ) {
       MemFree(pfnct);
       return 1;
     }
   } else if (strcmp(pfnct, "SeqHoundGODBGetClassification") == 0) {
     if(SeqHoundGODBGetClassification() ) {
       MemFree(pfnct);
       return 1;
     }
   }
   /* function disabled since it has a wrong logic
   else if (strcmp(pfnct, "SeqHoundGODBGetHierarchyLevel") == 0)
   {
     if(SeqHoundGODBGetHierarchyLevel() )
     {
       MemFree(pfnct);
       return 1;
     }
   }*/
   else if (strcmp(pfnct, "SeqHoundAllGenomesWithScientificNames") == 0) {
     if(SeqHoundAllGenomesWithScientificNames() ) {
       MemFree(pfnct);
       return 1;
     }
   }else if (strcmp(pfnct, "SeqHoundAllGenomesWithScientificNamesPL") == 0) {
     if(SeqHoundAllGenomesWithScientificNamesPL() ) {
       MemFree(pfnct);
       return 1;
     }
   }else if (strcmp(pfnct, "SeqHoundAllGenomesWithNumProteins") == 0) {
     if(SeqHoundAllGenomesWithNumProteins() ) {
       MemFree(pfnct);
       return 1;
     }
   }else if (strcmp(pfnct, "SeqHoundAllGenomesWithNumProteinsPL") == 0) {
     if(SeqHoundAllGenomesWithNumProteinsPL() ) {
       MemFree(pfnct);
       return 1;
     }
   }else if (strcmp(pfnct, "SeqHoundGetGisByDomainId") == 0) {
     if(SeqHoundGetGisByDomainId() ) {
       MemFree(pfnct);
       return 1;
     }
   }else if (strcmp(pfnct, "SeqHoundGetDomainsFromGi") == 0) {
     if(SeqHoundGetDomainsFromGi() ) {
       MemFree(pfnct);
       return 1;
     }
   }else if (strcmp(pfnct, "SeqHoundGetDomainsFromGiPL") == 0) {
     if(SeqHoundGetDomainsFromGiPL() ) {
       MemFree(pfnct);
       return 1;
     }
   }else if (strcmp(pfnct, "SeqHoundGetGisByDomainIdAndEvalue") == 0) {
     if(SeqHoundGetGisByDomainIdAndEvalue() ) {
       MemFree(pfnct);
       return 1;
     }
   }else if (strcmp(pfnct, "SeqHoundGetDomainsFromGiWithEvalue") == 0) {
     if(SeqHoundGetDomainsFromGiWithEvalue() ) {
       MemFree(pfnct);
       return 1;
     }
   }else if (strcmp(pfnct, "SeqHoundGetDomainsFromGiWithEvaluePL") == 0) {
     if(SeqHoundGetDomainsFromGiWithEvaluePL() ) {
       MemFree(pfnct);
       return 1;
     }
   }else if (strcmp(pfnct, "SeqHoundGetDomainsFromGiListWithEvalue")==0){
   	if(SeqHoundGetDomainsFromGiListWithEvalue()) {
		MemFree(pfnct);
		return 1;
	}

   }else if (strcmp(pfnct, "SeqHoundGetGisByNumberOfDomains") == 0) {
     if(SeqHoundGetGisByNumberOfDomains() ) {
       MemFree(pfnct);
       return 1;
     }
   }else if (strcmp(pfnct, "SeqHoundGetDomainLabelFromDomainId") == 0) {
     if(SeqHoundGetDomainLabelFromDomainId() ) {
       MemFree(pfnct);
       return 1;
     }
   }else if (strcmp(pfnct, "SeqHoundGetDomainIdFromLabel") == 0) {
     if(SeqHoundGetDomainIdFromLabel() ) {
       MemFree(pfnct);
       return 1;
     }
   }else if (strcmp(pfnct, "SeqHoundGetNameByGi") == 0) {
     if(SeqHoundGetNameByGi() ) {
       MemFree(pfnct);
       return 1;
     }
   } else if ( 0 == strcmp ( pfnct, "SeqHoundProteinReportSetDescription" ) ) {
      if ( SeqHoundProteinReportSetDescription () ) {
         MemFree ( pfnct );
	 return 1;
      }
   }
#ifdef ODBC 
   else if (strcmp(pfnct, "SeqHoundDBXREFGetDBXrefListBySourceRecord") == 0)
    {      
	if(SeqHoundDBXREFGetDBXrefListBySourceRecord() == SLRI_FAIL) 
	{
	    MemFree(pfnct);
	    return 1;
	}
    }
    else if (strcmp(pfnct, "SeqHoundDBXREFGetSourceRecordListByDBXref") == 0)
    {      
	if(SeqHoundDBXREFGetSourceRecordListByDBXref() == SLRI_FAIL) 
	{
	    MemFree(pfnct);
	    return 1;
	}
    }
    else if (strcmp(pfnct, "SeqHoundDBXREFGetListOfSourceDBNames") == 0)
    {      
	if(SeqHoundDBXREFGetListOfSourceDBNames() == SLRI_FAIL) 
	{
	    MemFree(pfnct);
	    return 1;
	}
    }
    else if (strcmp(pfnct, "SeqHoundDBXREFGetListOfXreferedDBNames") == 0)
    {    
	if(SeqHoundDBXREFGetListOfXreferedDBNames() == SLRI_FAIL) 
	{
	    MemFree(pfnct);
	    return 1;
	}
    }
    else if (strcmp(pfnct, "SeqHoundDBXREFGetListOfAllDBNames") == 0)
    {    
	if(SeqHoundDBXREFGetListOfAllDBNames() == SLRI_FAIL) 
	{
	    MemFree(pfnct);
	    return 1;
	}
    }
    else if (strcmp(pfnct, "SeqHoundDBXREFGetSourceRecordIDListByDBName") == 0)
    {
	if(SeqHoundDBXREFGetSourceRecordIDListByDBName() == SLRI_FAIL) 
	{
	    MemFree(pfnct);
	    return 1;
	}
    }
    else if (strcmp(pfnct, "SeqHoundDBXREFGetXrefRecordIDListByDBName") == 0)
    {
	if(SeqHoundDBXREFGetXrefRecordIDListByDBName() == SLRI_FAIL) 
	{
	    MemFree(pfnct);
	    return 1;
	}
    }
    else if (strcmp(pfnct, "SeqHoundDBXREFGetCVList") == 0)
    {
	if(SeqHoundDBXREFGetCVList() == SLRI_FAIL) 
	{
	    MemFree(pfnct);
	    return 1;
	}
    }
    else if (strcmp(pfnct, "SeqHoundDBXREFGetXrefSourceFieldName") == 0)
    {
	if(SeqHoundDBXREFGetXrefSourceFieldName() == SLRI_FAIL) 
	{
	    MemFree(pfnct);
	    return 1;
	}
    }
#endif
    
    /* insert more functions here */
   
    else if (strcmp(pfnct, "SeqHoundRedundantGroupKeyedList") == 0)
    {
	if(SeqHoundRedundantGroupKeyedList() == SLRI_FAIL) 
	{
	    MemFree(pfnct);
	    return 1;
	}
    }
    else if (strcmp(pfnct, "SeqHoundGetFastaKeyedList") == 0)
    {
	if(SeqHoundGetFastaKeyedList() == SLRI_FAIL) 
	{
	    MemFree(pfnct);
	    return 1;
	}
    }
    else if (strcmp(pfnct, "SeqHoundGetFastaFromRedundantGroupID") == 0)
    {
	if(SeqHoundGetFastaFromRedundantGroupID() == SLRI_FAIL) 
	{
	    MemFree(pfnct);
	    return 1;
	}
    }
    else if (strcmp(pfnct, "SeqHoundGetFastaFromRedundantGroupIDPL") == 0)
    {
	if(SeqHoundGetFastaFromRedundantGroupIDPL() == SLRI_FAIL) 
	{
	    MemFree(pfnct);
	    return 1;
	}
    }
    else if (strcmp(pfnct, "SeqHoundGetFastaFromRedundantGroupIDKeyedList") == 0)
    {
	if(SeqHoundGetFastaFromRedundantGroupIDKeyedList() == SLRI_FAIL) 
	{
	    MemFree(pfnct);
	    return 1;
	}
    } 
	else if (strcmp(pfnct, "SeqHoundIntrezGetCDSLRIFastaPL") == 0)
	{
	if(SeqHoundIntrezGetCDSLRIFastaPL() == SLRI_FAIL)
	{
		MemFree(pfnct);
		return 1;
	}
	}
	else if (strcmp(pfnct, "SeqHoundIntrezGetCDSLRIFasta") == 0)
	{
	if(SeqHoundIntrezGetCDSLRIFasta() == SLRI_FAIL)
	{
		MemFree(pfnct);
		return 1;
	}
	}
   else
    {
      ErrPostEx(SEV_ERROR,0,0, " Failed to process function value.");
      printf("Content-type: text/html\r\n\r\n");
      fprintf(stdout, "SEQHOUND_ERROR Failed to process function value.\n");
      MemFree(pfnct);
      return 1;
    }
   return SLRI_SUCCESS;

}

Int2 Main()
{
   CharPtr pfnct = NULL;
   Int4 IndexArgs = -1;
   CharPtr pcThis = NULL;
   /* SK: */
   CharPtr strptr = NULL;
   CharPtr pgiparameter = NULL;
   CharPtr pgiparameter2 = NULL;
   Boolean openMMDB = FALSE;

   ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
   BSErrLogOpen("seqrem_logs", ELOG_BANNER | ELOG_APPEND);
   /*ErrSetLogfile("seqrem_logs", ELOG_BANNER | ELOG_APPEND );*/
   ErrSetLogLevel(SEV_INFO);
   ErrSetMessageLevel(SEV_MAX);


   if(!SHoundInitReadOnly(FALSE, "seqhoundtest"))
   {
       ErrPostEx(SEV_ERROR,0,0, "SHoundInit failed.<br>");
       printf("Content-type: text/html\r\n\r\n");
       fprintf(stdout, "SEQHOUND_ERROR SHoundInit failed.\n");
       BSErrLogClose();
       return 1;
   }
#ifdef SHFCGI
   if(!OpenMMDBAPI(0,NULL))  /*KM 030304 for normal cgi, migrate to only functions which need it*/
   {
       ErrPostEx(SEV_FATAL,2,1,"Unable to open MMDBAPI");
       fprintf(stdout, "SEQHOUND_ERROR Open MMDBAPI failed.\n");
       BSErrLogClose();
       return 2;
   }
   openMMDB = TRUE;
#endif
#ifdef SHFCGI
   /* begin fast cgi while loop */	
   while (FCGI_Accept() >= 0)
   {
     BSErrLogOpen("seqrem_logs", ELOG_BANNER | ELOG_APPEND);
#endif

   if (WWWGetArgs(&ginfo)!= WWWErrOk)
   {
        ErrPostEx(SEV_ERROR,0,0, "Failed to process posting.");
        printf("Content-type: text/html\r\n\r\n");
        fprintf(stdout, "SEQHOUND_ERROR Failed to process posting\n");
#ifdef SHFCGI
        BSErrLogClose();
        continue;
#else
        BSErrLogClose();
        return 1;
#endif
   }
   if ((IndexArgs = WWWFindName(ginfo,"fnct")) >= 0)
   {	
        pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
        pfnct = StringSave(pcThis);
   }
   if ((pfnct == NULL) || (strlen(pfnct) == 0))
   {
        ErrPostEx(SEV_ERROR,0,0, "Failed to get function value.");
        printf("Content-type: text/html\r\n\r\n");
        fprintf(stdout, "SEQHOUND_ERROR Failed to get function value.\n");
#ifdef SHFCGI
        BSErrLogClose();
        continue;
#else
        BSErrLogClose();
        return 1;
#endif
   }
   /* SK: */
   strptr = WWWGetAddress(ginfo);
   ErrPostEx(SEV_INFO,0,0, "Address: %s", IsNullStr(strptr));
   ErrPostEx(SEV_INFO,0,0, "Function: %s", IsNullStr(pfnct));

   if ((IndexArgs = WWWFindName(ginfo,"gi")) >= 0) {
     pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
     pgiparameter = StringSave(pcThis) ;
     ErrPostEx(SEV_INFO,0,0, "GI number(s): %s ", pgiparameter);
     MemFree(pgiparameter);
   }
   else if ((IndexArgs = WWWFindName(ginfo,"pgi")) >= 0) {
     pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
     pgiparameter = StringSave(pcThis) ;     
     ErrPostEx(SEV_INFO,0,0, "GI number(s): %s ", pgiparameter);
     MemFree(pgiparameter);
   }
   else if ((IndexArgs = WWWFindName(ginfo,"gi1")) >= 0) {
     pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
     pgiparameter = StringSave(pcThis) ;
     if ((IndexArgs = WWWFindName(ginfo,"gi2")) >= 0) {
       pcThis = WWWGetValueByIndex(ginfo,IndexArgs);
       pgiparameter2 =StringSave(pcThis);
       ErrPostEx(SEV_INFO,0,0, "GI number(s): %s,%s ", pgiparameter, pgiparameter2);
       MemFree(pgiparameter);
       MemFree(pgiparameter2);
     }
     else {
       ErrPostEx(SEV_INFO,0,0, "GI number(s): %s ", pgiparameter);
       MemFree(pgiparameter);
     }
   }
#ifndef SHFCGI 
   /* Bug 4694. For structure related queries, open mmdbapi prior to calling function */
   if((StrCmp(pfnct,"SeqHoundGet3D") == 0) || (StrCmp(pfnct, "SeqHoundGet3DEx") == 0) || (StrCmp(pfnct, "SeqHoundGetFixed3D") == 0) || (StrCmp(pfnct, "SeqHoundGet3DfromPdbIdEx") ==0 ) || (StrCmp(pfnct, "SeqHoundGet3DfromPdbId") ==0) || (StrCmp(pfnct, "SeqHound3DbWhat") == 0) || (StrCmp(pfnct, "SeqHoundPDBFromMMDB") == 0) || (StrCmp(pfnct, "SeqHoundGet3DPL") == 0) || (StrCmp(pfnct, "SeqHoundGet3DfromPdbIdPL") == 0) || (StrCmp(pfnct, "SeqHoundGetXML3DPL") == 0 ) || (StrCmp(pfnct, "SeqHoundGetPDB3DPL") == 0))
   {
     if( openMMDB == FALSE )
     {
       if(!OpenMMDBAPI(0,NULL))
       {
           ErrPostEx(SEV_FATAL,2,1,"Unable to open MMDBAPI");
           fprintf(stdout, "SEQHOUND_ERROR Open MMDBAPI failed.\n");
           BSErrLogClose();
           return 2;
       }
       openMMDB = TRUE;
     }
   }
#endif
   if(ProcessFnctRequest(pfnct) == SLRI_FAIL)
   {
#ifdef SHFCGI
       BSErrLogClose();
       continue;
#else
     BSErrLogClose();
     return 1;
#endif
   }

   WWWInfoFree(ginfo);
   MemFree(pfnct);
   ErrPostEx(SEV_INFO,0,0, "Done!");

#ifdef SHFCGI
   BSErrLogClose();
} /* end of the fast cgi loop */
#endif

   SHoundFini();
   BSErrLogClose();
   return 0;
}

/*
$Log: seqrem.c,v $
Revision 1.69  2005/03/31 18:43:35  vgu
Bug 5174 fix

Revision 1.68  2005/03/16 20:09:50  vgu
Fix to bug 5115: Centralize open and close MMDBAPI calls

Revision 1.67  2005/03/02 20:27:08  ryao
changed query and return pair symbol from '~' to '#~#', since some gis might contain '~' in defline, example gi 9971716. bug [4100], this change also need to be updated on all the remote APIs

Revision 1.66  2005/03/02 16:55:27  eburgess
Added API calls SHoundRNAFromChromosome, SHoundRNAFromOrganism,
SHoundRNAFromUnknownChromosome, and SHoundRNAFromOrganismList. Bug 4914

Revision 1.65  2005/02/15 21:07:13  eburgess
Added SeqHoundProteinsFromUnknownChromosome() and
SeqHoundDNAFromUnknownChromosome.

Revision 1.64  2005/02/10 21:10:59  vgu
Bug 4694: Open/Close MMDB in non fast cgiseqrem

Revision 1.63  2005/02/01 21:58:01  ryao
fixed the bug in SeqHoundGet3DPL. This fix allows to retrieve text format of ASN.1 structure instead of binary format. See bug [2919]

Revision 1.62  2005/01/21 20:56:20  vgu
Bug 4254 fix:Make fseqrem process exit after handling mmdb API queries to release virtual memory to system

Revision 1.57  2005/01/19 20:17:33  vgu
bug 4254 fix: remote query name start with SeqHound instead of SHound

Revision 1.56  2005/01/19 17:39:29  vgu
bug 4254 additional fix: add coverage for APIs SHoundGetPDB3D and SHoundGetFixed3D

Revision 1.55  2005/01/18 21:47:04  vgu
Make fast cgi process exit after serving requests for large mmdb objects bug number 4254

Revision 1.54  2004/11/22 14:56:59  mmatan

Modified shoundgetfixed3d so that is takes the same parameters as shoundget3dex.
See bug 3930 for details.

Revision 1.53  2004/11/18 19:06:48  mmatan
Adding function SHoundGetFixed3D to the remote API server.  See bug 3930.

Revision 1.52  2004/11/02 16:33:22  ryao
changed the GROUP separator from ';' to '#;#'. bug 3787 related.

Revision 1.51  2004/09/30 17:50:22  ryao
Added a function to retrieve merged taxid: SeqHoundGetNewTaxIDFromMergedTaxID. bug [3474]

Revision 1.50  2004/09/07 19:49:58  ryao
Implemented IntrezGetCDSLRIFasta and IntrezGetCDSLRIFastaPL. bug [2929]

Revision 1.49  2004/09/01 21:01:30  zwang
Added SeqHoundTitleFromGi into ProcessFnctRequest.

Revision 1.48  2004/08/12 17:57:34  zwang
New API added to get title by gi from ACCDB

Revision 1.47  2004/07/14 21:40:31  ryao
Removed unnecessary html tag, minor fix.

Revision 1.46  2004/07/14 20:08:45  ryao
Added functions: SeqHoundAllGenomesWithScientificNamesPL  SeqHoundAllGenomesWithNumProteinsPL in order to allow corresponding Perl and Bioperl remote API functions to get return value instead of reading from binary structure.

Revision 1.45  2004/06/22 19:07:11  ryao
Added SeqHoundGetFastaFromRedundantGroupIDPL, retrieve a fasta strucutre in plain text (includes defline and sequence). bug 2565 related.

Revision 1.44  2004/06/16 15:59:49  ryao
Modified 3 keyed list functions to keep it the same order as it was queried.

Revision 1.43  2004/05/31 20:47:29  hlieu
minor fixes to index out of bound, bug 2185

Revision 1.42  2004/05/31 20:19:15  hlieu
minor fixes to prevent warnings in bug 2185

Revision 1.41  2004/05/20 18:06:46  ryao
added new locus link functions

Revision 1.40  2004/05/17 20:21:09  hlieu
remove warnings

Revision 1.39  2004/05/17 20:11:37  hlieu
remove C++ style comments

Revision 1.38  2004/05/10 16:28:11  ryao
removed unused variables

Revision 1.37  2004/05/03 13:17:48  eburgess
Fix errors in cross-compile: declare variables at the start of a block and
do not indent #include statements.

Revision 1.36  2004/04/07 21:22:32  ryao
Added SHoundGetFastaFromRedundantGroupID SHoundGetFastaFromRedundantGroupIDKeyedList. Bug [1996] related.

Revision 1.35  2004/04/07 21:08:10  ryao
Added SHoundGetFastaFromRedundantGroupID SHoundGetFastaFromRedundantGroupIDKeyedList. Bug [1997] related

Revision 1.34  2004/04/05 17:54:10  ryao
minor change

Revision 1.33  2004/04/05 16:48:36  ryao
Added SHoundGetFastaKeyedList SHoundRedundantGroupKeyedList

Revision 1.32  2004/04/02 18:34:05  mmatan
Add GI logging functionality for compliance with swissprot licensing terms

Revision 1.31  2004/03/11 18:38:35  hlieu
fixed minor error

Revision 1.30  2004/03/11 16:19:29  hlieu
undefined net entrez access

Revision 1.29  2004/03/08 17:30:49  hlieu
added different formats for protein report, cleaned up variable usage

Revision 1.28  2004/03/05 23:05:34  hlieu
added format to protein report

Revision 1.27  2004/03/05 21:40:25  hlieu
fixed proteinreport

Revision 1.26  2004/03/05 21:05:30  hlieu
added protein report access

Revision 1.25  2004/03/05 20:03:31  kmichali
removed bstdt.val dictionary load and added ifdefs for fast CGI

Revision 1.24  2004/03/04 02:06:31  idonalds
Moved OpenMMDBAPI so that it is only called if the MMDBAPI is required by the SeqHound API function being called

Revision 1.23  2004/02/06 20:55:31  ryao
added SHoundPDBFromMMDB to retrieve PDB identifier from mmdbid

Revision 1.22  2004/01/22 16:20:17  zwang
Include _odbc.h when ODBC is defined, otherwise include _db.h

Revision 1.21  2004/01/20 17:13:25  hlieu
added #defines for conditional compilation of DBXREF currently defined
for ODBC version only

Revision 1.20  2004/01/14 18:43:32  rcavero
Previous version was break when compiling in Strict C.

Revision 1.19  2004/01/13 15:16:47  hlieu
disabled NetEntrezOn

Revision 1.18  2004/01/09 22:12:24  ryao
add SHoundGetLargestMMDBID

Revision 1.17  2004/01/06 22:32:10  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.16  2004/01/06 20:24:47  hlieu
added NetEntrez support for SHoundGetFastaListPL

Revision 1.15  2004/01/05 21:43:42  hlieu
added NetEntrez support for GetFastaPL

Revision 1.14  2003/12/16 17:30:32  ryao
bug fix in functions SeqHoundGODBGetRecordByReference SeqHoundGiFromGOIDAndECode SeqHoundGiFromGOIDListAndECode

Revision 1.13  2003/11/26 15:37:25  ryao
added SHoundGetReferenceIDFromGiList SHoundGiFromReferenceList SHoundGiFromGOIDListAndECode

Revision 1.12  2003/11/21 15:35:32  ryao
remove ReferenceIDFromGiList

Revision 1.11  2003/11/20 23:31:43  ryao
fixed bug in SHoundSequenceLength

Revision 1.9  2003/11/11 20:07:23  haocl
fixed fastalist

Revision 1.8  2003/11/06 21:04:41  haocl
fixed fasta format in GetFastaPL

Revision 1.7  2003/10/24 17:53:12  feldman
Added SHoundGet3DEx function to allow user to choose model level and num models when fetching biostruc

Revision 1.6  2003/10/09 20:15:07  ryao
add SeqHoundGetReferenceIDFromGi

Revision 1.4  2003/09/03 19:43:36  volodya
added function SeqHoundGetNameByGi

Revision 1.3  2003/09/03 17:57:08  ryao
change prototype of SHoundMoleculeType

Revision 1.2  2003/08/22 15:25:53  haocl
added GetAccFromRetiredGi

Revision 1.1.1.20  2003/08/16 08:45:56  root
Automated import from Sourceforge.net

Revision 1.23  2003/08/13 17:41:26  haocl
added SeqHoundSequenceLength

Revision 1.22  2002/09/27 16:52:58  kaca
swithched to  read only mode for SHound initialization

Revision 1.21  2002/08/07 21:34:03  kaca
removed unused variables

Revision 1.20  2002/07/29 22:30:45  betel
Added GetDomainsFromGiListWithEvalue for local&remote C api (no Perl or C++ yet)

Revision 1.19  2002/07/25 17:37:08  kaca
renamed SeqHoundXMLSeqEntry to SeqHoundGetXMLSeqEntry to match C API

Revision 1.18  2002/07/15 15:01:32  kaca
removed word-wrap

Revision 1.17  2002/07/15 14:52:34  kaca
removed unused variables

Revision 1.16  2002/07/11 19:07:53  haocl
added missing RPS functions

Revision 1.15  2002/07/08 17:31:00  kaca
removed unused variables

Revision 1.14  2002/05/13 17:53:17  haocl
added rps functions

Revision 1.13  2002/03/25 21:04:06  micheld
Returned SHoundGet3D to binary mode.

Revision 1.12  2002/03/25 08:15:56  micheld
Changed binary mode to text mode for SHoundGet3D on client & server
(doesn't appear to work in binary mode)

Revision 1.11  2002/03/25 07:51:31  micheld
New genome functions. changed includes to new slrilinkset.h

Revision 1.9  2002/02/06 23:56:45  micheld
Changed TaxDB_ Functions to SHound*Tax* function call, legacy preserved for C api but *NOT* PERL

Revision 1.8  2002/01/15 17:31:03  kaca
disabled SHoundGODBGetHierarchyLevel due to a wrong logic,renamed all Intrez functions into SHound and changed includes

Revision 1.7  2001/12/19 04:17:04  kaca
error reporting cosmetic fixes

Revision 1.6  2001/12/11 19:10:42  kaca
removed SHoundIsInited function, status managed by a global

Revision 1.5  2001/12/05 00:52:06  micheld
Added access to BLAST pairwise results... returns either NBLASTDB format or seqalign

Revision 1.4  2001/11/13 21:46:22  kaca
new defline and goid annotation functions

Revision 1.3  2001/11/08 00:52:15  kaca
fixed log closing

Revision 1.2  2001/11/07 17:23:35  kaca
removed unused variables

Revision 1.1.1.2  2001/10/10 21:53:46  kaca
initial import

Revision 1.59  2001/10/05 17:30:17  kaca
removed unused variables

Revision 1.58  2001/10/04 18:15:00  kaca
modularized and fused development of slow and fast CGI

Revision 1.57  2001/09/27 20:44:26  kaca
added neighbour functions specific to PERL API

Revision 1.56  2001/09/25 19:44:35  kaca
consolidating functional annotation project

Revision 1.55  2001/09/21 17:42:20  kaca
renamed local GODB functions to the same as remote API functions

Revision 1.54  2001/09/03 20:31:08  michel
Mainly fixed compiler warnings and changed CODE4 reference for GO and LL

Revision 1.53  2001/08/30 18:12:01  haocl
changed implementation of SHoundGODBGetClassification to return CharPtr

Revision 1.52  2001/08/29 20:23:19  haocl
fixed SeqHoundGODBGetAllChildren

Revision 1.51  2001/08/29 20:15:30  haocl
gene ontology functions corrected

Revision 1.50  2001/08/29 20:07:13  haocl
added gene ontology functions

Revision 1.49  2001/08/29 16:38:44  haocl
fixed errors from renaming SHoundECodeFromGi

Revision 1.48  2001/08/29 16:34:47  haocl
renamed SeqHoundECodeFromGi

Revision 1.47  2001/08/28 18:56:14  haocl
changed SeqHoundCDDScoreFromGi

Revision 1.46  2001/08/23 15:52:29  michel
Cosmetic changes

Revision 1.45  2001/08/22 19:35:01  haocl
added all the locuslink functions

Revision 1.43  2001/08/21 13:30:02  haocl
locus link functions added

Revision 1.42  2001/08/20 14:48:07  haocl
removed all of haocl's function

Revision 1.41  2001/08/20 14:34:10  haocl
locuslink functions added

Revision 1.40  2001/08/17 17:29:28  haocl
fixed a small improper function call

Revision 1.39  2001/08/17 17:17:35  haocl
added locuslink function

Revision 1.38  2001/08/15 22:32:00  kaca
removed printf statements

Revision 1.37  2001/08/15 20:39:48  michel
minor bug fix

Revision 1.36  2001/08/15 20:28:06  haocl
locuslink fun tion

Revision 1.35  2001/08/15 19:07:37  michel
bug fixes on new functions

Revision 1.34  2001/08/15 16:19:35  michel
error fix

Revision 1.33  2001/08/15 16:13:40  michel
Added extra error checking and more functionality

Revision 1.32  2001/08/14 14:31:33  michel
Tested Neighbours Functions with the exception of 3D neighbours
Tested SHound3DListFromTaxID - returns all redundant 3D structures

Revision 1.31  2001/08/09 15:49:08  michel
minor changes

Revision 1.30  2001/08/09 15:36:49  michel
Changed string conversion functions
Added more error checking

Revision 1.29  2001/08/07 20:13:58  michel
Added option to change ASNIO mode to text for testing purposes (as define)

Revision 1.28  2001/08/07 19:54:39  michel
Added CGI functions to main

Revision 1.27  2001/08/07 18:54:49  michel
Tested SHoundNeighboursFromGiEx
Added the rest of the remote API and CGI calls (untested)

Revision 1.26  2001/08/03 19:02:55  michel
Added remote API SHoundNeighboursFromGiEx (untested) and CGI call

Revision 1.25  2001/06/11 18:10:44  kaca
GetFastaPL added to accomodate PERL API

Revision 1.22  2001/06/08 22:04:47  kaca
more fixes

Revision 1.21  2001/06/08 18:16:11  kaca
PERL API error checking fixes

Revision 1.20  2001/06/07 23:04:19  kaca
AccFromGi fix

Revision 1.18  2001/06/07 19:24:53  kaca
corrected some spelling

Revision 1.16  2001/05/28 21:33:08  kaca
added functions for PERL API which return text instead of binaries

Revision 1.15  2001/05/28 14:18:20  kaca
bug fix

Revision 1.14  2001/04/27 23:12:42  kaca
added SHoundFindName and SHoundFindNameList functions

Revision 1.13  2001/04/06 22:36:20  kaca
added SHoundGiFromPDBchain

Revision 1.12  2001/04/02 18:14:57  kaca
fixed memory leaks caused by exiting due to an error

Revision 1.9  2001/03/29 16:31:46  kaca
added slri_misc include

Revision 1.8  2001/03/26 19:47:29  kaca
changes in include files

Revision 1.7  2001/03/19 22:13:21  kaca
added Get3DfromPdbId function

Revision 1.6  2001/03/09 19:19:31  kaca
added SeqHoundFirstOfRedundantGroupFromID functions

Revision 1.5  2001/03/08 22:33:31  kaca
added flat file functionality

Revision 1.4  2001/02/07 18:50:50  kaca
removed unused variables

Revision 1.3  2001/02/07 16:45:05  kaca
cosmetic changes

Revision 1.2  2000/11/01 23:27:55  kaca
added redundancy and taxonomy functions

Revision 1.1.1.1  2000/09/18 17:41:55  kaca
Initial import

Revision 1.1  2000/07/06 20:35:48  kaca
added license

*/



