/* $Id: objslrilinkset.c,v 1.5 2004/02/19 23:47:26 mjdumont Exp $ 
-- ***************************************************************************
--
--  Samuel Lunenfeld Research Institute (SLRI)
--
--  File: objslrilinkset.h
--  Description:  Modified LinkSet from NCBI's LinkSet that uses different weights
--                and can be used in a nested list
--                **** Do not use ASNTOOL to regenerate ****
--                Hard-coded LinkSet component
--
--  Author: Michel Dumontier
--  Date: April 2001 (updated Feb 2002)
--
--  Hogue Lab - University of Toronto Biochemistry Department
--  Samuel Lunenfeld Research Institute, Mount Sinai Hospital
--
--  Copyright Notice:
--
--  Copyright ©2001 Mount Sinai Hospital (MSH)
--
--  This program is free software; you can redistribute it and/or 
--  modify it under the terms of the GNU General Public License as 
--  published by the Free Software Foundation; either version 2 of 
--  the License, or any later version.
--
--  This program is distributed in the hope that it will be useful, 
--  but WITHOUT ANY WARRANTY; without even the implied warranty of 
--  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
--
--  See the GNU General Public License for more details.
--
--  You should have received a copy of the GNU General Public License 
--  along with this program; if not, write to the
--         Free Software Foundation, Inc., 
--         59 Temple Place, Suite 330, Boston, MA  
--         02111-1307  USA 
--  or visit http://www.gnu.org/copyleft/gpl.html
--
--  SPECIAL EXCEPTIONS
--
--  As a special exception, Mount Sinai Hospital gives permission to 
--  link this program with the following non-GPL programs or libraries, 
--  and distribute the resulting executable, without including the source 
--  code for these in the source distribution:
--
--  a) CodeBase 6.5 or greater from Sequiter Software Inc.
--
--  b) The C or C++ interface to Oracle 8.x or greater, from
--     Oracle Corporation or IBM DB2 UDB.
--
-- ***************************************************************************
*/
#include <asn.h>

#define NLM_GENERATED_CODE_PROTO

#include <objslrilinkset.h>

static Boolean loaded = FALSE;

#include <slrilinkset.h>

#ifndef NLM_EXTERN_LOADS
#define NLM_EXTERN_LOADS {}
#endif

NLM_EXTERN Boolean LIBCALL
objslrilinksetAsnLoad(void)
{
   if ( ! loaded) {
      NLM_EXTERN_LOADS

      if ( ! AsnLoad ())
      return FALSE;
      loaded = TRUE;
   }

   return TRUE;
}

typedef Int2 SLRILinkSetModule;
#define	SLRI_ILINK_SET (SLRILinkSetModule) 0
#define	SLRI_FLINK_SET (SLRILinkSetModule) 1
#define SLRI_CLINK_SET (SLRILinkSetModule) 2
#define SLRI_BLINK_SET (SLRILinkSetModule) 3
#define SLRI_LLINK_SET (SLRILinkSetModule) 4
#define SLRI_YLINK_SET (SLRILinkSetModule) 5



static void SLRILinkSetModuleValues(SLRILinkSetModule SLRILink_Set_Module, 
							   AsnTypePtr *localtype, AsnTypePtr *atp_num, AsnTypePtr *atp_uids, AsnTypePtr *atp_uids_E,
							   AsnTypePtr *atp_weights, AsnTypePtr *atp_weights_E)
{
	if(SLRILink_Set_Module == SLRI_ILINK_SET) {
		*localtype     = ILINK_SET;	
		*atp_num       = ILINK_SET_num;
		*atp_uids      = ILINK_SET_uids;
		*atp_uids_E    = ILINK_SET_uids_E;
		*atp_weights   = ILINK_SET_weights;
		*atp_weights_E = ILINK_SET_weights_E;
	} else if (SLRILink_Set_Module == SLRI_FLINK_SET) {
		*localtype     = FLINK_SET;	
		*atp_num       = FLINK_SET_num;
		*atp_uids      = FLINK_SET_uids;
		*atp_uids_E    = FLINK_SET_uids_E;
		*atp_weights   = FLINK_SET_weights;
		*atp_weights_E = FLINK_SET_weights_E;
	} else if (SLRILink_Set_Module == SLRI_CLINK_SET) {
		*localtype     = CLINK_SET;	
		*atp_num       = CLINK_SET_num;
		*atp_uids      = CLINK_SET_uids;
		*atp_uids_E    = CLINK_SET_uids_E;
		*atp_weights   = CLINK_SET_weights;
		*atp_weights_E = CLINK_SET_weights_E;
	} else if (SLRILink_Set_Module == SLRI_BLINK_SET) {
		*localtype     = BLINK_SET;	
		*atp_num       = BLINK_SET_num;
		*atp_uids      = BLINK_SET_uids;
		*atp_uids_E    = BLINK_SET_uids_E;
		*atp_weights   = BLINK_SET_weights;
		*atp_weights_E = BLINK_SET_weights_E;
	} else if (SLRILink_Set_Module == SLRI_LLINK_SET) {
		*localtype     = LLINK_SET;	
		*atp_num       = LLINK_SET_num;
		*atp_uids      = LLINK_SET_uids;
		*atp_uids_E    = LLINK_SET_uids_E;
		*atp_weights   = LLINK_SET_weights;
		*atp_weights_E = LLINK_SET_weights_E;
	} else if (SLRILink_Set_Module == SLRI_YLINK_SET) {
		*localtype     = YLINK_SET;	
		*atp_num       = YLINK_SET_num;
		*atp_uids      = YLINK_SET_uids;
		*atp_uids_E    = YLINK_SET_uids_E;
		*atp_weights   = YLINK_SET_weights;
		*atp_weights_E = YLINK_SET_weights_E;
	}
}

static void SLRILinkSetListModuleValues(SLRILinkSetModule SLRILink_Set_Module, 
							   AsnTypePtr *localtype, AsnTypePtr *atp_set, AsnTypePtr *atp_set_E, AsnTypePtr *atp_id)
{
	if(SLRILink_Set_Module == SLRI_ILINK_SET) {
		*localtype    = ILINK_SET_LIST;	
		*atp_set      = ILINK_SET_LIST_set1;
		*atp_set_E    = ILINK_SET_LIST_set1_E;
		*atp_id       = ILINK_SET_LIST_id;
	} else if (SLRILink_Set_Module == SLRI_FLINK_SET) {
		*localtype    = FLINK_SET_LIST;	
		*atp_set      = FLINK_SET_LIST_set1;
		*atp_set_E    = FLINK_SET_LIST_set1_E;
		*atp_id       = FLINK_SET_LIST_id;
	} else if (SLRILink_Set_Module == SLRI_CLINK_SET) {
		*localtype    = CLINK_SET_LIST;	
		*atp_set      = CLINK_SET_LIST_set1;
		*atp_set_E    = CLINK_SET_LIST_set1_E;
		*atp_id       = CLINK_SET_LIST_id;
	} else if (SLRILink_Set_Module == SLRI_BLINK_SET) {
		*localtype    = BLINK_SET_LIST;	
		*atp_set      = BLINK_SET_LIST_set1;
		*atp_set_E    = BLINK_SET_LIST_set1_E;
		*atp_id       = BLINK_SET_LIST_id;
	} else if (SLRILink_Set_Module == SLRI_LLINK_SET) {
		*localtype    = LLINK_SET_LIST;	
		*atp_set      = LLINK_SET_LIST_set1;
		*atp_set_E    = LLINK_SET_LIST_set1_E;
		*atp_id       = LLINK_SET_LIST_id;
	} else if (SLRILink_Set_Module == SLRI_YLINK_SET) {
		*localtype    = YLINK_SET_LIST;	
		*atp_set      = YLINK_SET_LIST_set1;
		*atp_set_E    = YLINK_SET_LIST_set1_E;
		*atp_id       = YLINK_SET_LIST_id;
	}
}


static void SLRILinkSetList2ModuleValues(SLRILinkSetModule SLRILink_Set_Module, 
							   AsnTypePtr *localtype, AsnTypePtr *atp_set,  AsnTypePtr *atp_set_E, AsnTypePtr *atp_id)
{
	if(SLRILink_Set_Module == SLRI_ILINK_SET) {
		*localtype    = ILINK_SET_LIST2;	
		*atp_set      = ILINK_SET_LIST2_set2;
		*atp_set_E    = ILINK_SET_LIST2_set2_E;
		*atp_id       = ILINK_SET_LIST2_id;
	} else if (SLRILink_Set_Module == SLRI_FLINK_SET) {
		*localtype    = FLINK_SET_LIST2;	
		*atp_set      = FLINK_SET_LIST2_set2;
		*atp_set_E    = FLINK_SET_LIST2_set2_E;
		*atp_id       = FLINK_SET_LIST2_id;
	} else if (SLRILink_Set_Module == SLRI_CLINK_SET) {
		*localtype    = CLINK_SET_LIST2;	
		*atp_set      = CLINK_SET_LIST2_set2;
		*atp_set_E    = CLINK_SET_LIST2_set2_E;
		*atp_id       = CLINK_SET_LIST2_id;
	} else if (SLRILink_Set_Module == SLRI_BLINK_SET) {
		*localtype    = BLINK_SET_LIST2;	
		*atp_set      = BLINK_SET_LIST2_set2;
		*atp_set_E    = BLINK_SET_LIST2_set2_E;
		*atp_id       = BLINK_SET_LIST2_id;
	} else if (SLRILink_Set_Module == SLRI_LLINK_SET) {
		*localtype    = LLINK_SET_LIST2;	
		*atp_set      = LLINK_SET_LIST2_set2;
		*atp_set_E    = LLINK_SET_LIST2_set2_E;
		*atp_id       = LLINK_SET_LIST2_id;
	} else if (SLRILink_Set_Module == SLRI_YLINK_SET) {
		*localtype    = YLINK_SET_LIST2;	
		*atp_set      = YLINK_SET_LIST2_set2;
		*atp_set_E    = YLINK_SET_LIST2_set2_E;
		*atp_id       = YLINK_SET_LIST2_id;
	}
}

static void SLRILinkSetList3ModuleValues(SLRILinkSetModule SLRILink_Set_Module, 
							   AsnTypePtr *localtype, AsnTypePtr *atp_set, AsnTypePtr *atp_set_E, AsnTypePtr *atp_id)
{
	if(SLRILink_Set_Module == SLRI_ILINK_SET) {
		*localtype    = ILINK_SET_LIST3;	
		*atp_set      = ILINK_SET_LIST3_set3;
		*atp_set_E    = ILINK_SET_LIST3_set3_E;
		*atp_id       = ILINK_SET_LIST3_id;
	} else if (SLRILink_Set_Module == SLRI_FLINK_SET) {
		*localtype    = FLINK_SET_LIST3;	
		*atp_set      = FLINK_SET_LIST3_set3;
		*atp_set_E    = FLINK_SET_LIST3_set3_E;
		*atp_id       = FLINK_SET_LIST3_id;
	} else if (SLRILink_Set_Module == SLRI_CLINK_SET) {
		*localtype    = CLINK_SET_LIST3;	
		*atp_set      = CLINK_SET_LIST3_set3;
		*atp_set_E    = CLINK_SET_LIST3_set3_E;
		*atp_id       = CLINK_SET_LIST3_id;
	} else if (SLRILink_Set_Module == SLRI_BLINK_SET) {
		*localtype    = BLINK_SET_LIST3;	
		*atp_set      = BLINK_SET_LIST3_set3;
		*atp_set_E    = BLINK_SET_LIST3_set3_E;
		*atp_id       = BLINK_SET_LIST3_id;
	} else if (SLRILink_Set_Module == SLRI_LLINK_SET) {
		*localtype    = LLINK_SET_LIST3;	
		*atp_set      = LLINK_SET_LIST3_set3;
		*atp_set_E    = LLINK_SET_LIST3_set3_E;
		*atp_id       = LLINK_SET_LIST3_id;
	} else if (SLRILink_Set_Module == SLRI_YLINK_SET) {
		*localtype    = YLINK_SET_LIST3;	
		*atp_set      = YLINK_SET_LIST3_set3;
		*atp_set_E    = YLINK_SET_LIST3_set3_E;
		*atp_id       = YLINK_SET_LIST3_id;
	}
}


/**************************************************
*
*    SLRILinkSetAsnRead()
*
**************************************************/
NLM_EXTERN Pointer LIBCALL SLRILinkSetAsnRead(AsnIoPtr aip, AsnTypePtr orig, SLRILinkSetModule SLRILink_Set_Module)
{
   DataVal av;
   AsnTypePtr atp;
   AsnReadFunc func;
   AsnTypePtr localtype = NULL, atp_num = NULL, atp_uids = NULL, atp_uids_E = NULL, atp_weights = NULL, atp_weights_E = NULL;
   Int4 i = 0, num = 0;
   Int4Ptr pi = NULL;
   ILinkSetPtr iptr = NULL;
   FLinkSetPtr fptr = NULL;
   CLinkSetPtr cptr = NULL;
   BLinkSetPtr bptr = NULL;
   LLinkSetPtr lptr = NULL;
   YLinkSetPtr yptr = NULL;


   if (! loaded) {
      if (! objslrilinksetAsnLoad()) {
         return NULL;
      }
   }
   if (aip == NULL) return NULL;

   SLRILinkSetModuleValues(SLRILink_Set_Module, &localtype, &atp_num, &atp_uids, &atp_uids_E, &atp_weights, &atp_weights_E);

	if (orig == NULL) {         /* ILinkSet ::= (self contained) */
	  atp = AsnReadId(aip, amp, localtype);
	} else {
	  atp = AsnLinkType(orig, localtype);
	}
	/* link in local tree */
	if (atp == NULL) return NULL;
	/* read the start struct */
	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
	atp = AsnReadId(aip,amp, atp);
	func = NULL;

	if (atp == atp_num) {
		if ( AsnReadVal(aip, atp, &av) <= 0) goto erret;
		num = av.intvalue;
		
		if(SLRILink_Set_Module == SLRI_ILINK_SET) {
				if((iptr = ILinkSetNew2(num)) == NULL) goto erret; /* 0 sentinel at end */
				pi = iptr->uids;
		} else if (SLRILink_Set_Module == SLRI_FLINK_SET) {
				if((fptr = FLinkSetNew2(num)) == NULL) goto erret;
				pi = fptr->uids;
		} else if (SLRILink_Set_Module == SLRI_CLINK_SET) {
				if((cptr = CLinkSetNew2(num)) == NULL) goto erret;
				pi = cptr->uids;
		} else if (SLRILink_Set_Module == SLRI_BLINK_SET) {
				if((bptr = BLinkSetNew2(num)) == NULL) goto erret;
				pi = bptr->uids;
		} else if (SLRILink_Set_Module == SLRI_LLINK_SET) {
				if((lptr = LLinkSetNew2(num)) == NULL) goto erret;
				pi = lptr->uids;
		} else if (SLRILink_Set_Module == SLRI_YLINK_SET) {
				if((yptr = YLinkSetNew2(num)) == NULL) goto erret;
				pi = yptr->uids;
		}

		atp = AsnReadId(aip,amp, atp);  /* start seq of uids */
	}

	if (atp == atp_uids) {
		if (AsnReadVal(aip, atp, &av) <= 0) goto erret; 
		i = 0;
		while((atp = AsnReadId(aip, amp, atp)) == atp_uids_E) {
			if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
			pi[i] = av.intvalue;
			i++;
			if (i > num) break;
		}
		if(atp == NULL) goto erret;
		if(i != num) {
			ErrPost(CTX_NCBIOBJ, 0, "Incorrect number of uids in Link-set. line %ld", aip->linenumber);
			goto erret;
		}
   		if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end seq of uids */
		atp = AsnReadId(aip,amp, atp);  /* start seq of weights */
	}
	if (atp == atp_weights) {
		if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
		i = 0;
		while((atp = AsnReadId(aip, amp, atp)) == atp_weights_E) {
			if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
			
		if (SLRILink_Set_Module == SLRI_ILINK_SET) {
				iptr->weights[i] = av.intvalue;
			} else if (SLRILink_Set_Module == SLRI_FLINK_SET) {
				fptr->weights[i] = av.realvalue;
			} else if (SLRILink_Set_Module == SLRI_CLINK_SET) {
				cptr->weights[i] = StringSave((CharPtr) av.ptrvalue);		
			} else if (SLRILink_Set_Module == SLRI_BLINK_SET) {
				bptr->weights[i] =  av.boolvalue;		
			} else if (SLRILink_Set_Module == SLRI_LLINK_SET) {
				lptr->weights[i] = av.bigintvalue;		
			} else if (SLRILink_Set_Module == SLRI_YLINK_SET) {
				yptr->weights[i] = *((Boolean *) av.ptrvalue);
			}
			i++;
			if (i > num) break;
		}
		if(atp == NULL) goto erret;
		if(i != num) {
			ErrPost(CTX_NCBIOBJ, 0, "Incorrect number of uids in Link-set. line %ld", aip->linenumber);
			goto erret;
		}
   		if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end seq of weights */
		if ((atp = AsnReadId(aip, amp, atp)) == NULL) goto erret;
	}

	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
	/* end struct */

ret:
	AsnUnlinkType(orig);       /* unlink local tree */
   
	if (SLRILink_Set_Module == SLRI_ILINK_SET) {
		return (Pointer) iptr;
	} else if (SLRILink_Set_Module == SLRI_FLINK_SET) {
		return (Pointer) fptr;
	} else if (SLRILink_Set_Module == SLRI_CLINK_SET) {
		return (Pointer) cptr;
	} else if (SLRILink_Set_Module == SLRI_BLINK_SET) {
		return (Pointer) bptr;
	} else if (SLRILink_Set_Module == SLRI_LLINK_SET) {
		return (Pointer) lptr;
	} else if (SLRILink_Set_Module == SLRI_YLINK_SET) {
		return (Pointer) yptr;
	}

erret:
	aip -> io_failure = TRUE;
   
	if (SLRILink_Set_Module == SLRI_ILINK_SET) {
		iptr = ILinkSetFree(iptr);
	} else if (SLRILink_Set_Module == SLRI_FLINK_SET) {
		fptr = FLinkSetFree(fptr);
	} else if (SLRILink_Set_Module == SLRI_CLINK_SET) {
		cptr = CLinkSetFree(cptr);
	} else if (SLRILink_Set_Module == SLRI_BLINK_SET) {
		bptr = BLinkSetFree(bptr);
	} else if (SLRILink_Set_Module == SLRI_LLINK_SET) {
		lptr = LLinkSetFree(lptr);
	} else if (SLRILink_Set_Module == SLRI_YLINK_SET) {
		yptr = YLinkSetFree(yptr);
	}
	goto ret;
}


NLM_EXTERN ILinkSetPtr LIBCALL ILinkSetAsnRead(AsnIoPtr aip, AsnTypePtr orig) 
{
	return (ILinkSetPtr) SLRILinkSetAsnRead(aip, orig, SLRI_ILINK_SET);
}

NLM_EXTERN FLinkSetPtr LIBCALL FLinkSetAsnRead(AsnIoPtr aip, AsnTypePtr orig) 
{
	return (FLinkSetPtr) SLRILinkSetAsnRead(aip, orig, SLRI_FLINK_SET);
}

NLM_EXTERN CLinkSetPtr LIBCALL CLinkSetAsnRead(AsnIoPtr aip, AsnTypePtr orig) 
{
	return (CLinkSetPtr) SLRILinkSetAsnRead(aip, orig, SLRI_CLINK_SET);
}

NLM_EXTERN BLinkSetPtr LIBCALL BLinkSetAsnRead(AsnIoPtr aip, AsnTypePtr orig) 
{
	return (BLinkSetPtr) SLRILinkSetAsnRead(aip, orig, SLRI_BLINK_SET);
}

NLM_EXTERN LLinkSetPtr LIBCALL LLinkSetAsnRead(AsnIoPtr aip, AsnTypePtr orig) 
{
	return (LLinkSetPtr) SLRILinkSetAsnRead(aip, orig, SLRI_LLINK_SET);
}

NLM_EXTERN YLinkSetPtr LIBCALL YLinkSetAsnRead(AsnIoPtr aip, AsnTypePtr orig) 
{
	return (YLinkSetPtr) SLRILinkSetAsnRead(aip, orig, SLRI_YLINK_SET);
}

/**************************************************
*
*    SLRILinkSetAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
SLRILinkSetAsnWrite(Pointer ptr, AsnIoPtr aip, AsnTypePtr orig, SLRILinkSetModule SLRILink_Set_Module)
{
	DataVal av;
	AsnTypePtr atp;
	Boolean retval = FALSE;
	AsnTypePtr localtype = NULL, atp_num = NULL, atp_uids = NULL, atp_uids_E = NULL, atp_weights = NULL, atp_weights_E = NULL;
	Int4Ptr pi = NULL, pi_uids = NULL;
	FloatHiPtr pf = NULL;
	Boolean * pb = NULL;
	Int8Ptr pl = NULL;
	Byte * py = NULL;
	ILinkSetPtr iptr = NULL;
	CharPtr *pc = NULL;
	FLinkSetPtr fptr = NULL;
	CLinkSetPtr cptr = NULL;
	BLinkSetPtr bptr = NULL;
	LLinkSetPtr lptr = NULL;
	YLinkSetPtr yptr = NULL;

	Int4 i = 0, num = 0;


	if (! loaded) {
		if (! objslrilinksetAsnLoad()) {
			return FALSE;
		}
	}
	if (aip == NULL) return FALSE;
 
	SLRILinkSetModuleValues(SLRILink_Set_Module, &localtype, &atp_num, &atp_uids, &atp_uids_E, &atp_weights, &atp_weights_E);
   
	atp = AsnLinkType(orig, localtype); 	/* link local tree */
		
	if (atp == NULL) return FALSE;

	if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
	if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) goto erret;

	if (SLRILink_Set_Module == SLRI_ILINK_SET) {
		iptr = (ILinkSetPtr) ptr;
	} else if (SLRILink_Set_Module == SLRI_FLINK_SET) {
		fptr = (FLinkSetPtr) ptr;
	} else if (SLRILink_Set_Module == SLRI_CLINK_SET) {
		cptr = (CLinkSetPtr) ptr;
	} else if (SLRILink_Set_Module == SLRI_BLINK_SET) {
		bptr = (BLinkSetPtr) ptr;
	} else if (SLRILink_Set_Module == SLRI_LLINK_SET) {
		lptr = (LLinkSetPtr) ptr;
	} else if (SLRILink_Set_Module == SLRI_YLINK_SET) {
		yptr = (YLinkSetPtr) ptr;
	}
	
	/* num field */
	if (SLRILink_Set_Module == SLRI_ILINK_SET) {
		num = iptr -> num;
	} else if (SLRILink_Set_Module == SLRI_FLINK_SET) {
		num = fptr -> num;
	} else if (SLRILink_Set_Module == SLRI_CLINK_SET) {
		num = cptr -> num;
	} else if (SLRILink_Set_Module == SLRI_BLINK_SET) {
		num = bptr -> num;
	} else if (SLRILink_Set_Module == SLRI_LLINK_SET) {
		num = lptr -> num;
	} else if (SLRILink_Set_Module == SLRI_YLINK_SET) {
		num = yptr -> num;
	}
	av.intvalue = num;
	retval = AsnWrite(aip, atp_num,  &av);

	/* uids field */
	if (SLRILink_Set_Module == SLRI_ILINK_SET) {
		pi_uids = iptr -> uids;
	} else if (SLRILink_Set_Module == SLRI_FLINK_SET) {
		pi_uids = fptr -> uids;
	} else if (SLRILink_Set_Module == SLRI_CLINK_SET) {
		pi_uids = cptr -> uids;
	} else if (SLRILink_Set_Module == SLRI_BLINK_SET) {
		pi_uids = bptr -> uids;
	} else if (SLRILink_Set_Module == SLRI_LLINK_SET) {
		pi_uids = lptr -> uids;
	} else if (SLRILink_Set_Module == SLRI_YLINK_SET) {
		pi_uids = yptr -> uids;
	}
	if (! AsnOpenStruct(aip, atp_uids, (Pointer) pi_uids)) goto erret;
	
	for (i = 0; i < num; i++) {
		av.intvalue = pi_uids[i];
		if (! AsnWrite(aip, atp_uids_E, &av)) goto erret;
	}
	if (! AsnCloseStruct(aip, atp_uids, (Pointer) pi_uids)) goto erret;

	/* weights field */

   	if (! AsnOpenStruct(aip, atp_weights, (Pointer) ((ILinkSetPtr) ptr) -> weights)) goto erret;
	for (i = 0; i < num; i++) {
		if (SLRILink_Set_Module == SLRI_ILINK_SET) {
			pi = iptr ->weights;
			av.intvalue = pi[i];
		} else if (SLRILink_Set_Module == SLRI_FLINK_SET) {
			pf = fptr ->weights;
			av.realvalue = pf[i];
		} else if (SLRILink_Set_Module == SLRI_CLINK_SET) {
			pc = cptr ->weights;
			av.ptrvalue = (Pointer) StringSave(pc[i]);
		} else if (SLRILink_Set_Module == SLRI_BLINK_SET) {
			pb = bptr ->weights;
			av.boolvalue = pb[i];
		} else if (SLRILink_Set_Module == SLRI_LLINK_SET) {
			pl = lptr ->weights;
			av.bigintvalue = pl[i];
		} else if (SLRILink_Set_Module == SLRI_YLINK_SET) {
			py = yptr ->weights;
			av.ptrvalue = (Pointer) &(py[i]);
		}
		if (! AsnWrite(aip, atp_weights_E, &av)) goto erret;
	}
	if (SLRILink_Set_Module == SLRI_ILINK_SET) {
		if (! AsnCloseStruct(aip, atp_weights, (Pointer) iptr -> weights)) goto erret;
	} else if (SLRILink_Set_Module == SLRI_FLINK_SET) {
		if (! AsnCloseStruct(aip, atp_weights, (Pointer) fptr -> weights)) goto erret;
	} else if (SLRILink_Set_Module == SLRI_CLINK_SET) {
		if (! AsnCloseStruct(aip, atp_weights, (Pointer) cptr -> weights)) goto erret;
	} else if (SLRILink_Set_Module == SLRI_BLINK_SET) {
		if (! AsnCloseStruct(aip, atp_weights, (Pointer) bptr -> weights)) goto erret;
	} else if (SLRILink_Set_Module == SLRI_YLINK_SET) {
		if (! AsnCloseStruct(aip, atp_weights, (Pointer) lptr -> weights)) goto erret;
	} else if (SLRILink_Set_Module == SLRI_LLINK_SET) {
		if (! AsnCloseStruct(aip, atp_weights, (Pointer) yptr -> weights)) goto erret;
	}
	if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) goto erret;
	retval = TRUE;

erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}


NLM_EXTERN Boolean LIBCALL ILinkSetAsnWrite(ILinkSetPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
	return SLRILinkSetAsnWrite((Pointer) ptr, aip, orig, SLRI_ILINK_SET);
}

NLM_EXTERN Boolean LIBCALL FLinkSetAsnWrite(FLinkSetPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
	return SLRILinkSetAsnWrite((Pointer) ptr, aip, orig, SLRI_FLINK_SET);
}

NLM_EXTERN Boolean LIBCALL CLinkSetAsnWrite(CLinkSetPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
	return SLRILinkSetAsnWrite((Pointer) ptr, aip, orig, SLRI_CLINK_SET);
}

NLM_EXTERN Boolean LIBCALL BLinkSetAsnWrite(BLinkSetPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
	return SLRILinkSetAsnWrite((Pointer) ptr, aip, orig, SLRI_BLINK_SET);
}
NLM_EXTERN Boolean LIBCALL LLinkSetAsnWrite(LLinkSetPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
	return SLRILinkSetAsnWrite((Pointer) ptr, aip, orig, SLRI_LLINK_SET);
}
NLM_EXTERN Boolean LIBCALL YLinkSetAsnWrite(YLinkSetPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
	return SLRILinkSetAsnWrite((Pointer) ptr, aip, orig, SLRI_YLINK_SET);
}

/**************************************************
*
*    SLRILinkSetListAsnRead()
*
**************************************************/
NLM_EXTERN Pointer LIBCALL SLRILinkSetListAsnRead(AsnIoPtr aip, AsnTypePtr orig, SLRILinkSetModule SLRILink_Set_Module )
{
	DataVal av;
	AsnTypePtr atp;
	AsnTypePtr localtype = NULL, atp_set = NULL, atp_set_E = NULL, atp_id = NULL;
	Boolean isError = FALSE;
	AsnReadFunc func;
	ILinkSetListPtr iptr = NULL;
	FLinkSetListPtr fptr = NULL;
	CLinkSetListPtr cptr = NULL;
	BLinkSetListPtr bptr = NULL;
	LLinkSetListPtr lptr = NULL;
	YLinkSetListPtr yptr = NULL;
	Pointer ptr = NULL;

	if (! loaded) {
		if (! objslrilinksetAsnLoad()) 
			return NULL;
	}
	if (aip == NULL) return NULL;

	SLRILinkSetListModuleValues(SLRILink_Set_Module,&localtype, &atp_set, &atp_set_E, &atp_id);

	if (orig == NULL) {
		atp = AsnReadId(aip, amp, localtype);
	} else {
		atp = AsnLinkType(orig, localtype);
	}
	/* link in local tree */
	if (atp == NULL) return NULL;

	if(SLRILink_Set_Module == SLRI_ILINK_SET) {
		if((iptr = ILinkSetListNew()) == NULL) goto erret; /* 0 sentinel at end */
		ptr = iptr;
	} else if (SLRILink_Set_Module == SLRI_FLINK_SET) {
		if((fptr = FLinkSetListNew()) == NULL) goto erret;
		ptr = fptr;
	} else if (SLRILink_Set_Module == SLRI_CLINK_SET) {
		if((cptr = CLinkSetListNew()) == NULL) goto erret;
		ptr = cptr;
	} else if (SLRILink_Set_Module == SLRI_BLINK_SET) {
		if((bptr = BLinkSetListNew()) == NULL) goto erret;
		ptr = bptr;
	} else if (SLRILink_Set_Module == SLRI_LLINK_SET) {
		if((lptr = LLinkSetListNew()) == NULL) goto erret;
		ptr = lptr;
	} else if (SLRILink_Set_Module == SLRI_YLINK_SET) {
		if((yptr = YLinkSetListNew()) == NULL) goto erret;
		ptr = yptr;
	}

	if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
		goto erret;
	}

	atp = AsnReadId(aip,amp, atp);
	func = NULL;

	if (atp == atp_set) {
		if(SLRILink_Set_Module == SLRI_ILINK_SET) {
			if((iptr -> set1 = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) ILinkSetAsnRead, (AsnOptFreeFunc) ILinkSetFree)) == NULL) goto erret;
		} else if (SLRILink_Set_Module == SLRI_FLINK_SET) {
			if((fptr -> set1 = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) FLinkSetAsnRead, (AsnOptFreeFunc) FLinkSetFree)) == NULL) goto erret;
		} else if (SLRILink_Set_Module == SLRI_CLINK_SET) {
			if((cptr -> set1 = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) CLinkSetAsnRead, (AsnOptFreeFunc) CLinkSetFree)) == NULL) goto erret;
		} else if (SLRILink_Set_Module == SLRI_BLINK_SET) {
			if((bptr -> set1 = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) BLinkSetAsnRead, (AsnOptFreeFunc) BLinkSetFree)) == NULL) goto erret;
		} else if (SLRILink_Set_Module == SLRI_LLINK_SET) {
			if((lptr -> set1 = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) LLinkSetAsnRead, (AsnOptFreeFunc) LLinkSetFree)) == NULL) goto erret;
		} else if (SLRILink_Set_Module == SLRI_YLINK_SET) {
			if((yptr -> set1 = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) YLinkSetAsnRead, (AsnOptFreeFunc) YLinkSetFree)) == NULL) goto erret;
		}  
		if (isError) goto erret;
		atp = AsnReadId(aip,amp, atp);
	}
	if (atp == atp_id) {
		if ( AsnReadVal(aip, atp, &av) <= 0) goto erret;
		if(SLRILink_Set_Module == SLRI_ILINK_SET) {
			iptr -> id = av.intvalue;
		} else if (SLRILink_Set_Module == SLRI_FLINK_SET) {
			fptr -> id = av.intvalue;
		} else if (SLRILink_Set_Module == SLRI_CLINK_SET) {
			cptr -> id = av.intvalue;
		} else if (SLRILink_Set_Module == SLRI_BLINK_SET) {
			bptr -> id = av.intvalue;
		} else if (SLRILink_Set_Module == SLRI_LLINK_SET) {
			lptr -> id = av.intvalue;
		} else if (SLRILink_Set_Module == SLRI_YLINK_SET) {
			yptr -> id = av.intvalue;
		}    
		atp = AsnReadId(aip,amp, atp);
	}

	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
	/* end struct */

ret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return ptr;

erret:
	aip -> io_failure = TRUE;
	if(SLRILink_Set_Module == SLRI_ILINK_SET) {
		iptr = ILinkSetListFree(iptr);
	} else if (SLRILink_Set_Module == SLRI_FLINK_SET) {
		fptr = FLinkSetListFree(fptr);
	} else if (SLRILink_Set_Module == SLRI_CLINK_SET) {
		cptr = CLinkSetListFree(cptr);
	} else if (SLRILink_Set_Module == SLRI_BLINK_SET) {
		bptr = BLinkSetListFree(bptr);
	} else if (SLRILink_Set_Module == SLRI_LLINK_SET) {
		lptr = LLinkSetListFree(lptr);
	} else if (SLRILink_Set_Module == SLRI_YLINK_SET) {
		yptr = YLinkSetListFree(yptr);
	}
	goto ret;
}

NLM_EXTERN ILinkSetListPtr LIBCALL ILinkSetListAsnRead(AsnIoPtr aip, AsnTypePtr orig) 
{
	return (ILinkSetListPtr) SLRILinkSetListAsnRead(aip, orig, SLRI_ILINK_SET);
}

NLM_EXTERN FLinkSetListPtr LIBCALL FLinkSetListAsnRead(AsnIoPtr aip, AsnTypePtr orig) 
{
	return (FLinkSetListPtr) SLRILinkSetListAsnRead(aip, orig, SLRI_FLINK_SET);
}

NLM_EXTERN CLinkSetListPtr LIBCALL CLinkSetListAsnRead(AsnIoPtr aip, AsnTypePtr orig) 
{
	return (CLinkSetListPtr) SLRILinkSetListAsnRead(aip, orig, SLRI_CLINK_SET);
}

NLM_EXTERN BLinkSetListPtr LIBCALL BLinkSetListAsnRead(AsnIoPtr aip, AsnTypePtr orig) 
{
	return (BLinkSetListPtr) SLRILinkSetListAsnRead(aip, orig, SLRI_BLINK_SET);
}
NLM_EXTERN LLinkSetListPtr LIBCALL LLinkSetListAsnRead(AsnIoPtr aip, AsnTypePtr orig) 
{
	return (LLinkSetListPtr) SLRILinkSetListAsnRead(aip, orig, SLRI_LLINK_SET);
}
NLM_EXTERN YLinkSetListPtr LIBCALL YLinkSetListAsnRead(AsnIoPtr aip, AsnTypePtr orig) 
{
	return (YLinkSetListPtr) SLRILinkSetListAsnRead(aip, orig, SLRI_YLINK_SET);
}

/**************************************************
*
*    SLRILinkSetListAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
SLRILinkSetListAsnWrite(Pointer ptr, AsnIoPtr aip, AsnTypePtr orig, SLRILinkSetModule SLRILink_Set_Module)
{
	DataVal av;
	AsnTypePtr atp;
	Boolean retval = FALSE;
	AsnTypePtr localtype = NULL, atp_set = NULL, atp_set_E = NULL, atp_id = NULL;

	if (! loaded) {
		if (! objslrilinksetAsnLoad()) {
			 return FALSE;
		}
	}

	if (aip == NULL) return FALSE;
	
	SLRILinkSetListModuleValues(SLRILink_Set_Module,&localtype, &atp_set, &atp_set_E, &atp_id);
	atp = AsnLinkType(orig, localtype);   /* link local tree */
	if (atp == NULL) return FALSE;

	if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
	if (! AsnOpenStruct(aip, atp, ptr)) goto erret;

	if(SLRILink_Set_Module == SLRI_ILINK_SET) {
		AsnGenericUserSeqOfAsnWrite(((ILinkSetListPtr) ptr) -> set1, (AsnWriteFunc) ILinkSetAsnWrite, aip, atp_set, atp_set_E);
		av.intvalue = ((ILinkSetListPtr) ptr)-> id;
	} else if (SLRILink_Set_Module == SLRI_FLINK_SET) {
		AsnGenericUserSeqOfAsnWrite(((FLinkSetListPtr) ptr) -> set1, (AsnWriteFunc) FLinkSetAsnWrite, aip, atp_set, atp_set_E);
		av.intvalue = ((FLinkSetListPtr) ptr) -> id;
	} else if (SLRILink_Set_Module == SLRI_CLINK_SET) {
		AsnGenericUserSeqOfAsnWrite(((CLinkSetListPtr) ptr) -> set1, (AsnWriteFunc) CLinkSetAsnWrite, aip, atp_set, atp_set_E);
		av.intvalue = ((CLinkSetListPtr) ptr) -> id;
	} else if (SLRILink_Set_Module == SLRI_BLINK_SET) {
		AsnGenericUserSeqOfAsnWrite(((BLinkSetListPtr) ptr) -> set1, (AsnWriteFunc) BLinkSetAsnWrite, aip, atp_set, atp_set_E);
		av.intvalue = ((BLinkSetListPtr) ptr) -> id;
	}  else if (SLRILink_Set_Module == SLRI_LLINK_SET) {
		AsnGenericUserSeqOfAsnWrite(((LLinkSetListPtr) ptr) -> set1, (AsnWriteFunc) LLinkSetAsnWrite, aip, atp_set, atp_set_E);
		av.intvalue = ((LLinkSetListPtr) ptr) -> id;
	}  else if (SLRILink_Set_Module == SLRI_YLINK_SET) {
		AsnGenericUserSeqOfAsnWrite(((YLinkSetListPtr) ptr) -> set1, (AsnWriteFunc) YLinkSetAsnWrite, aip, atp_set, atp_set_E);
		av.intvalue = ((YLinkSetListPtr) ptr) -> id;
	}   
	retval = AsnWrite(aip, atp_id,  &av);
	if (! AsnCloseStruct(aip, atp, ptr)) goto erret;
	retval = TRUE;

erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}


NLM_EXTERN Boolean LIBCALL ILinkSetListAsnWrite(ILinkSetListPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
	return SLRILinkSetListAsnWrite((Pointer) ptr, aip, orig, SLRI_ILINK_SET);
}

NLM_EXTERN Boolean LIBCALL FLinkSetListAsnWrite(FLinkSetListPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
	return SLRILinkSetListAsnWrite((Pointer) ptr, aip, orig, SLRI_FLINK_SET);
}

NLM_EXTERN Boolean LIBCALL CLinkSetListAsnWrite(CLinkSetListPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
	return SLRILinkSetListAsnWrite((Pointer) ptr, aip, orig, SLRI_CLINK_SET);
}
NLM_EXTERN Boolean LIBCALL BLinkSetListAsnWrite(BLinkSetListPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
	return SLRILinkSetListAsnWrite((Pointer) ptr, aip, orig, SLRI_BLINK_SET);
}
NLM_EXTERN Boolean LIBCALL LLinkSetListAsnWrite(LLinkSetListPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
	return SLRILinkSetListAsnWrite((Pointer) ptr, aip, orig, SLRI_LLINK_SET);
}
NLM_EXTERN Boolean LIBCALL YLinkSetListAsnWrite(YLinkSetListPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
	return SLRILinkSetListAsnWrite((Pointer) ptr, aip, orig, SLRI_YLINK_SET);
}

/**************************************************
*
*    SLRILinkSetList2AsnRead()
*
**************************************************/
NLM_EXTERN Pointer LIBCALL SLRILinkSetList2AsnRead(AsnIoPtr aip, AsnTypePtr orig, SLRILinkSetModule SLRILink_Set_Module )
{
	DataVal av;
	AsnTypePtr atp;
	AsnTypePtr localtype = NULL, atp_set = NULL, atp_set_E = NULL, atp_id = NULL;
	Boolean isError = FALSE;
	AsnReadFunc func;
	ILinkSetList2Ptr iptr = NULL;
	FLinkSetList2Ptr fptr = NULL;
	CLinkSetList2Ptr cptr = NULL;
	BLinkSetList2Ptr bptr = NULL;
	LLinkSetList2Ptr lptr = NULL;
	YLinkSetList2Ptr yptr = NULL;
	Pointer ptr = NULL;

	if (! loaded) {
		if (! objslrilinksetAsnLoad()) 
			return NULL;
	}
	if (aip == NULL) return NULL;

	SLRILinkSetList2ModuleValues(SLRILink_Set_Module,&localtype, &atp_set, &atp_set_E, &atp_id);

	if (orig == NULL) {
		atp = AsnReadId(aip, amp, localtype);
	} else {
		atp = AsnLinkType(orig, localtype);
	}
	/* link in local tree */
	if (atp == NULL) return NULL;

	if(SLRILink_Set_Module == SLRI_ILINK_SET) {
		if((iptr = ILinkSetList2New()) == NULL) goto erret; /* 0 sentinel at end */
		ptr = iptr;
	} else if (SLRILink_Set_Module == SLRI_FLINK_SET) {
		if((fptr = FLinkSetList2New()) == NULL) goto erret;
		ptr = fptr;
	} else if (SLRILink_Set_Module == SLRI_CLINK_SET) {
		if((cptr = CLinkSetList2New()) == NULL) goto erret;
		ptr = cptr;
	} else if (SLRILink_Set_Module == SLRI_BLINK_SET) {
		if((bptr = BLinkSetList2New()) == NULL) goto erret;
		ptr = bptr;
	} else if (SLRILink_Set_Module == SLRI_LLINK_SET) {
		if((lptr = LLinkSetList2New()) == NULL) goto erret;
		ptr = lptr;
	} else if (SLRILink_Set_Module == SLRI_YLINK_SET) {
		if((yptr = YLinkSetList2New()) == NULL) goto erret;
		ptr = yptr;
	}

	if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
		goto erret;
	}

	atp = AsnReadId(aip,amp, atp);
	func = NULL;

	if (atp == atp_set) {
		if(SLRILink_Set_Module == SLRI_ILINK_SET) {
			if((iptr -> set2 = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) ILinkSetListAsnRead, (AsnOptFreeFunc) ILinkSetListFree)) == NULL) goto erret;
		} else if (SLRILink_Set_Module == SLRI_FLINK_SET) {
			if((fptr -> set2 = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) FLinkSetListAsnRead, (AsnOptFreeFunc) FLinkSetListFree)) == NULL) goto erret;
		} else if (SLRILink_Set_Module == SLRI_CLINK_SET) {
			if((cptr -> set2 = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) CLinkSetListAsnRead, (AsnOptFreeFunc) CLinkSetListFree)) == NULL) goto erret;
		} else if (SLRILink_Set_Module == SLRI_BLINK_SET) {
			if((bptr -> set2 = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) BLinkSetListAsnRead, (AsnOptFreeFunc) BLinkSetListFree)) == NULL) goto erret;
		} else if (SLRILink_Set_Module == SLRI_LLINK_SET) {
			if((lptr -> set2 = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) LLinkSetListAsnRead, (AsnOptFreeFunc) LLinkSetListFree)) == NULL) goto erret;
		} else if (SLRILink_Set_Module == SLRI_YLINK_SET) {
			if((yptr -> set2 = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) YLinkSetListAsnRead, (AsnOptFreeFunc) YLinkSetListFree)) == NULL) goto erret;
		}  
		if (isError) goto erret;
		atp = AsnReadId(aip,amp, atp);
	}
	if (atp == atp_id) {
		if ( AsnReadVal(aip, atp, &av) <= 0) goto erret;
		if(SLRILink_Set_Module == SLRI_ILINK_SET) {
			iptr -> id = av.intvalue;
		} else if (SLRILink_Set_Module == SLRI_FLINK_SET) {
			fptr -> id = av.intvalue;
		} else if (SLRILink_Set_Module == SLRI_CLINK_SET) {
			cptr -> id = av.intvalue;
		} else if (SLRILink_Set_Module == SLRI_BLINK_SET) {
			bptr -> id = av.intvalue;
		} else if (SLRILink_Set_Module == SLRI_LLINK_SET) {
			lptr -> id = av.intvalue;
		} else if (SLRILink_Set_Module == SLRI_YLINK_SET) {
			yptr -> id = av.intvalue;
		}    
		atp = AsnReadId(aip,amp, atp);
	}

	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
	/* end struct */

ret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return ptr;

erret:
	aip -> io_failure = TRUE;
	if(SLRILink_Set_Module == SLRI_ILINK_SET) {
		iptr = ILinkSetList2Free(iptr);
	} else if (SLRILink_Set_Module == SLRI_FLINK_SET) {
		fptr = FLinkSetList2Free(fptr);
	} else if (SLRILink_Set_Module == SLRI_CLINK_SET) {
		cptr = CLinkSetList2Free(cptr);
	} else if (SLRILink_Set_Module == SLRI_BLINK_SET) {
		bptr = BLinkSetList2Free(bptr);
	} else if (SLRILink_Set_Module == SLRI_LLINK_SET) {
		lptr = LLinkSetList2Free(lptr);
	} else if (SLRILink_Set_Module == SLRI_YLINK_SET) {
		yptr = YLinkSetList2Free(yptr);
	}
	goto ret;
}

NLM_EXTERN ILinkSetList2Ptr LIBCALL ILinkSetList2AsnRead(AsnIoPtr aip, AsnTypePtr orig) 
{
	return (ILinkSetList2Ptr) SLRILinkSetList2AsnRead(aip, orig, SLRI_ILINK_SET);
}

NLM_EXTERN FLinkSetList2Ptr LIBCALL FLinkSetList2AsnRead(AsnIoPtr aip, AsnTypePtr orig) 
{
	return (FLinkSetList2Ptr) SLRILinkSetList2AsnRead(aip, orig, SLRI_FLINK_SET);
}

NLM_EXTERN CLinkSetList2Ptr LIBCALL CLinkSetList2AsnRead(AsnIoPtr aip, AsnTypePtr orig) 
{
	return (CLinkSetList2Ptr) SLRILinkSetList2AsnRead(aip, orig, SLRI_CLINK_SET);
}

NLM_EXTERN BLinkSetList2Ptr LIBCALL BLinkSetList2AsnRead(AsnIoPtr aip, AsnTypePtr orig) 
{
	return (BLinkSetList2Ptr) SLRILinkSetList2AsnRead(aip, orig, SLRI_BLINK_SET);
}

NLM_EXTERN LLinkSetList2Ptr LIBCALL LLinkSetList2AsnRead(AsnIoPtr aip, AsnTypePtr orig) 
{
	return (LLinkSetList2Ptr) SLRILinkSetList2AsnRead(aip, orig, SLRI_LLINK_SET);
}

NLM_EXTERN YLinkSetList2Ptr LIBCALL YLinkSetList2AsnRead(AsnIoPtr aip, AsnTypePtr orig) 
{
	return (YLinkSetList2Ptr) SLRILinkSetList2AsnRead(aip, orig, SLRI_YLINK_SET);
}


/**************************************************
*
*    SLRILinkSetList2AsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
SLRILinkSetList2AsnWrite(Pointer ptr, AsnIoPtr aip, AsnTypePtr orig, SLRILinkSetModule SLRILink_Set_Module)
{
	DataVal av;
	AsnTypePtr atp;
	Boolean retval = FALSE;
	AsnTypePtr localtype = NULL, atp_set = NULL, atp_set_E = NULL, atp_id = NULL;

	if (! loaded) {
		if (! objslrilinksetAsnLoad()) {
			 return FALSE;
		}
	}

	if (aip == NULL) return FALSE;
	
	SLRILinkSetList2ModuleValues(SLRILink_Set_Module,&localtype, &atp_set, &atp_set_E, &atp_id);
	atp = AsnLinkType(orig, localtype);   /* link local tree */
	if (atp == NULL) return FALSE;

	if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
	if (! AsnOpenStruct(aip, atp, ptr)) goto erret;

	if(SLRILink_Set_Module == SLRI_ILINK_SET) {
		AsnGenericUserSeqOfAsnWrite(((ILinkSetList2Ptr) ptr) -> set2, (AsnWriteFunc) ILinkSetListAsnWrite, aip, atp_set, atp_set_E);
		av.intvalue = ((ILinkSetList2Ptr) ptr)-> id;
	} else if (SLRILink_Set_Module == SLRI_FLINK_SET) {
		AsnGenericUserSeqOfAsnWrite(((FLinkSetList2Ptr) ptr) -> set2, (AsnWriteFunc) FLinkSetListAsnWrite, aip, atp_set, atp_set_E);
		av.intvalue = ((FLinkSetList2Ptr) ptr) -> id;
	} else if (SLRILink_Set_Module == SLRI_CLINK_SET) {
		AsnGenericUserSeqOfAsnWrite(((CLinkSetList2Ptr) ptr) -> set2, (AsnWriteFunc) CLinkSetListAsnWrite, aip, atp_set, atp_set_E);
		av.intvalue = ((CLinkSetList2Ptr) ptr) -> id;
	} else if (SLRILink_Set_Module == SLRI_BLINK_SET) {
		AsnGenericUserSeqOfAsnWrite(((BLinkSetList2Ptr) ptr) -> set2, (AsnWriteFunc) BLinkSetListAsnWrite, aip, atp_set, atp_set_E);
		av.intvalue = ((BLinkSetList2Ptr) ptr) -> id;
	} else if (SLRILink_Set_Module == SLRI_LLINK_SET) {
		AsnGenericUserSeqOfAsnWrite(((LLinkSetList2Ptr) ptr) -> set2, (AsnWriteFunc) LLinkSetListAsnWrite, aip, atp_set, atp_set_E);
		av.intvalue = ((LLinkSetList2Ptr) ptr) -> id;
	} else if (SLRILink_Set_Module == SLRI_YLINK_SET) {
		AsnGenericUserSeqOfAsnWrite(((YLinkSetList2Ptr) ptr) -> set2, (AsnWriteFunc) YLinkSetListAsnWrite, aip, atp_set, atp_set_E);
		av.intvalue = ((YLinkSetList2Ptr) ptr) -> id;
	}     
	retval = AsnWrite(aip, atp_id,  &av);
	if (! AsnCloseStruct(aip, atp, ptr)) goto erret;
	retval = TRUE;

erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}


NLM_EXTERN Boolean LIBCALL ILinkSetList2AsnWrite(ILinkSetList2Ptr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
	return SLRILinkSetList2AsnWrite((Pointer) ptr, aip, orig, SLRI_ILINK_SET);
}

NLM_EXTERN Boolean LIBCALL FLinkSetList2AsnWrite(FLinkSetList2Ptr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
	return SLRILinkSetList2AsnWrite((Pointer) ptr, aip, orig, SLRI_FLINK_SET);
}

NLM_EXTERN Boolean LIBCALL CLinkSetList2AsnWrite(CLinkSetList2Ptr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
	return SLRILinkSetList2AsnWrite((Pointer) ptr, aip, orig, SLRI_CLINK_SET);
}

NLM_EXTERN Boolean LIBCALL BLinkSetList2AsnWrite(BLinkSetList2Ptr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
	return SLRILinkSetList2AsnWrite((Pointer) ptr, aip, orig, SLRI_BLINK_SET);
}

NLM_EXTERN Boolean LIBCALL LLinkSetList2AsnWrite(LLinkSetList2Ptr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
	return SLRILinkSetList2AsnWrite((Pointer) ptr, aip, orig, SLRI_LLINK_SET);
}

NLM_EXTERN Boolean LIBCALL YLinkSetList2AsnWrite(YLinkSetList2Ptr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
	return SLRILinkSetList2AsnWrite((Pointer) ptr, aip, orig, SLRI_YLINK_SET);
}


/**************************************************
*
*    SLRILinkSetList3AsnRead()
*
**************************************************/
NLM_EXTERN Pointer LIBCALL SLRILinkSetList3AsnRead(AsnIoPtr aip, AsnTypePtr orig, SLRILinkSetModule SLRILink_Set_Module )
{
	DataVal av;
	AsnTypePtr atp;
	AsnTypePtr localtype = NULL, atp_set = NULL, atp_set_E = NULL,  atp_id = NULL;
	Boolean isError = FALSE;
	AsnReadFunc func;
	ILinkSetList3Ptr iptr = NULL;
	FLinkSetList3Ptr fptr = NULL;
	CLinkSetList3Ptr cptr = NULL;
	BLinkSetList3Ptr bptr = NULL;
	LLinkSetList3Ptr lptr = NULL;
	YLinkSetList3Ptr yptr = NULL;
	Pointer ptr = NULL;

	if (! loaded) {
		if (! objslrilinksetAsnLoad()) 
			return NULL;
	}
	if (aip == NULL) return NULL;

	SLRILinkSetList3ModuleValues(SLRILink_Set_Module,&localtype, &atp_set, &atp_set_E, &atp_id);

	if (orig == NULL) {
		atp = AsnReadId(aip, amp, localtype);
	} else {
		atp = AsnLinkType(orig, localtype);
	}
	/* link in local tree */
	if (atp == NULL) return NULL;

	if(SLRILink_Set_Module == SLRI_ILINK_SET) {
		if((iptr = ILinkSetList3New()) == NULL) goto erret; /* 0 sentinel at end */
		ptr = iptr;
	} else if (SLRILink_Set_Module == SLRI_FLINK_SET) {
		if((fptr = FLinkSetList3New()) == NULL) goto erret;
		ptr = fptr;
	} else if (SLRILink_Set_Module == SLRI_CLINK_SET) {
		if((cptr = CLinkSetList3New()) == NULL) goto erret;
		ptr = cptr;
	} else if (SLRILink_Set_Module == SLRI_BLINK_SET) {
		if((bptr = BLinkSetList3New()) == NULL) goto erret;
		ptr = bptr;
	} else if (SLRILink_Set_Module == SLRI_LLINK_SET) {
		if((lptr = LLinkSetList3New()) == NULL) goto erret;
		ptr = lptr;
	} else if (SLRILink_Set_Module == SLRI_YLINK_SET) {
		if((yptr = YLinkSetList3New()) == NULL) goto erret;
		ptr = yptr;
	}

	if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
		goto erret;
	}

	atp = AsnReadId(aip,amp, atp);
	func = NULL;

	if (atp == atp_set) {
		if(SLRILink_Set_Module == SLRI_ILINK_SET) {
			if((iptr -> set3 = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) ILinkSetList2AsnRead, (AsnOptFreeFunc) ILinkSetList2Free)) == NULL) goto erret;
		} else if (SLRILink_Set_Module == SLRI_FLINK_SET) {
			if((fptr -> set3 = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) FLinkSetList2AsnRead, (AsnOptFreeFunc) FLinkSetList2Free)) == NULL) goto erret;
		} else if (SLRILink_Set_Module == SLRI_CLINK_SET) {
			if((cptr -> set3 = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) CLinkSetList2AsnRead, (AsnOptFreeFunc) CLinkSetList2Free)) == NULL) goto erret;
		} else if (SLRILink_Set_Module == SLRI_BLINK_SET) {
			if((bptr -> set3 = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) BLinkSetList2AsnRead, (AsnOptFreeFunc) BLinkSetList2Free)) == NULL) goto erret;
		} else if (SLRILink_Set_Module == SLRI_LLINK_SET) {
			if((lptr -> set3 = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) LLinkSetList2AsnRead, (AsnOptFreeFunc) LLinkSetList2Free)) == NULL) goto erret;
		} else if (SLRILink_Set_Module == SLRI_YLINK_SET) {
			if((yptr -> set3 = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) YLinkSetList2AsnRead, (AsnOptFreeFunc) YLinkSetList2Free)) == NULL) goto erret;
		}     
		if (isError) goto erret;
		atp = AsnReadId(aip,amp, atp);
	}
	if (atp == atp_id) {
		if ( AsnReadVal(aip, atp, &av) <= 0) goto erret;
		if(SLRILink_Set_Module == SLRI_ILINK_SET) {
			iptr -> id = av.intvalue;
		} else if (SLRILink_Set_Module == SLRI_FLINK_SET) {
			fptr -> id = av.intvalue;
		} else if (SLRILink_Set_Module == SLRI_CLINK_SET) {
			cptr -> id = av.intvalue;
		} else if (SLRILink_Set_Module == SLRI_BLINK_SET) {
			bptr -> id = av.intvalue;
		} else if (SLRILink_Set_Module == SLRI_LLINK_SET) {
			lptr -> id = av.intvalue;
		} else if (SLRILink_Set_Module == SLRI_YLINK_SET) {
			yptr -> id = av.intvalue;
		}  
		atp = AsnReadId(aip,amp, atp);
	}

	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
	/* end struct */

ret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return ptr;

erret:
	aip -> io_failure = TRUE;
	if(SLRILink_Set_Module == SLRI_ILINK_SET) {
		iptr = ILinkSetList3Free(iptr);
	} else if (SLRILink_Set_Module == SLRI_FLINK_SET) {
		fptr = FLinkSetList3Free(fptr);
	} else if (SLRILink_Set_Module == SLRI_CLINK_SET) {
		cptr = CLinkSetList3Free(cptr);
	} else if (SLRILink_Set_Module == SLRI_BLINK_SET) {
		bptr = BLinkSetList3Free(bptr);
	} else if (SLRILink_Set_Module == SLRI_LLINK_SET) {
		lptr = LLinkSetList3Free(lptr);
	} else if (SLRILink_Set_Module == SLRI_YLINK_SET) {
		yptr = YLinkSetList3Free(yptr);
	}
	goto ret;
}

NLM_EXTERN ILinkSetList3Ptr LIBCALL ILinkSetList3AsnRead(AsnIoPtr aip, AsnTypePtr orig) 
{
	return (ILinkSetList3Ptr) SLRILinkSetList3AsnRead(aip, orig, SLRI_ILINK_SET);
}

NLM_EXTERN FLinkSetList3Ptr LIBCALL FLinkSetList3AsnRead(AsnIoPtr aip, AsnTypePtr orig) 
{
	return (FLinkSetList3Ptr) SLRILinkSetList3AsnRead(aip, orig, SLRI_FLINK_SET);
}

NLM_EXTERN CLinkSetList3Ptr LIBCALL CLinkSetList3AsnRead(AsnIoPtr aip, AsnTypePtr orig) 
{
	return (CLinkSetList3Ptr) SLRILinkSetList3AsnRead(aip, orig, SLRI_CLINK_SET);
}
NLM_EXTERN BLinkSetList3Ptr LIBCALL BLinkSetList3AsnRead(AsnIoPtr aip, AsnTypePtr orig) 
{
	return (BLinkSetList3Ptr) SLRILinkSetList3AsnRead(aip, orig, SLRI_BLINK_SET);
}
NLM_EXTERN LLinkSetList3Ptr LIBCALL LLinkSetList3AsnRead(AsnIoPtr aip, AsnTypePtr orig) 
{
	return (LLinkSetList3Ptr) SLRILinkSetList3AsnRead(aip, orig, SLRI_LLINK_SET);
}
NLM_EXTERN YLinkSetList3Ptr LIBCALL YLinkSetList3AsnRead(AsnIoPtr aip, AsnTypePtr orig) 
{
	return (YLinkSetList3Ptr) SLRILinkSetList3AsnRead(aip, orig, SLRI_YLINK_SET);
}


/**************************************************
*
*    SLRILinkSetList3AsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
SLRILinkSetList3AsnWrite(Pointer ptr, AsnIoPtr aip, AsnTypePtr orig, SLRILinkSetModule SLRILink_Set_Module)
{
	DataVal av;
	AsnTypePtr atp;
	Boolean retval = FALSE;
	AsnTypePtr localtype = NULL, atp_set = NULL, atp_set_E = NULL, atp_id = NULL;

	if (! loaded) {
		if (! objslrilinksetAsnLoad()) {
			 return FALSE;
		}
	}

	if (aip == NULL) return FALSE;
	
	SLRILinkSetList3ModuleValues(SLRILink_Set_Module,&localtype, &atp_set, &atp_set_E, &atp_id);
	atp = AsnLinkType(orig, localtype);   /* link local tree */
	if (atp == NULL) return FALSE;

	if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
	if (! AsnOpenStruct(aip, atp, ptr)) goto erret;

	if(SLRILink_Set_Module == SLRI_ILINK_SET) {
		AsnGenericUserSeqOfAsnWrite(((ILinkSetList3Ptr) ptr) -> set3, (AsnWriteFunc) ILinkSetList2AsnWrite, aip, atp_set, atp_set_E);
		av.intvalue = ((ILinkSetList3Ptr) ptr)-> id;
	} else if (SLRILink_Set_Module == SLRI_FLINK_SET) {
		AsnGenericUserSeqOfAsnWrite(((FLinkSetList3Ptr) ptr) -> set3, (AsnWriteFunc) FLinkSetList2AsnWrite, aip, atp_set, atp_set_E);
		av.intvalue = ((FLinkSetList3Ptr) ptr) -> id;
	} else if (SLRILink_Set_Module == SLRI_CLINK_SET) {
		AsnGenericUserSeqOfAsnWrite(((CLinkSetList3Ptr) ptr) -> set3, (AsnWriteFunc) CLinkSetList2AsnWrite, aip, atp_set, atp_set_E);
		av.intvalue = ((CLinkSetList3Ptr) ptr) -> id;
	} else if (SLRILink_Set_Module == SLRI_BLINK_SET) {
		AsnGenericUserSeqOfAsnWrite(((BLinkSetList3Ptr) ptr) -> set3, (AsnWriteFunc) BLinkSetList2AsnWrite, aip, atp_set, atp_set_E);
		av.intvalue = ((BLinkSetList3Ptr) ptr) -> id;
	} else if (SLRILink_Set_Module == SLRI_LLINK_SET) {
		AsnGenericUserSeqOfAsnWrite(((LLinkSetList3Ptr) ptr) -> set3, (AsnWriteFunc) LLinkSetList2AsnWrite, aip, atp_set, atp_set_E);
		av.intvalue = ((LLinkSetList3Ptr) ptr) -> id;
	} else if (SLRILink_Set_Module == SLRI_YLINK_SET) {
		AsnGenericUserSeqOfAsnWrite(((YLinkSetList3Ptr) ptr) -> set3, (AsnWriteFunc) YLinkSetList2AsnWrite, aip, atp_set, atp_set_E);
		av.intvalue = ((YLinkSetList3Ptr) ptr) -> id;
	}   
	retval = AsnWrite(aip, atp_id,  &av);
	if (! AsnCloseStruct(aip, atp, ptr)) goto erret;
	retval = TRUE;

erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}


NLM_EXTERN Boolean LIBCALL ILinkSetList3AsnWrite(ILinkSetList3Ptr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
	return SLRILinkSetList3AsnWrite((Pointer) ptr, aip, orig, SLRI_ILINK_SET);
}

NLM_EXTERN Boolean LIBCALL FLinkSetList3AsnWrite(FLinkSetList3Ptr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
	return SLRILinkSetList3AsnWrite((Pointer) ptr, aip, orig, SLRI_FLINK_SET);
}

NLM_EXTERN Boolean LIBCALL CLinkSetList3AsnWrite(CLinkSetList3Ptr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
	return SLRILinkSetList3AsnWrite((Pointer) ptr, aip, orig, SLRI_CLINK_SET);
}

NLM_EXTERN Boolean LIBCALL BLinkSetList3AsnWrite(BLinkSetList3Ptr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
	return SLRILinkSetList3AsnWrite((Pointer) ptr, aip, orig, SLRI_BLINK_SET);
}
NLM_EXTERN Boolean LIBCALL LLinkSetList3AsnWrite(LLinkSetList3Ptr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
	return SLRILinkSetList3AsnWrite((Pointer) ptr, aip, orig, SLRI_LLINK_SET);
}
NLM_EXTERN Boolean LIBCALL YLinkSetList3AsnWrite(YLinkSetList3Ptr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
	return SLRILinkSetList3AsnWrite((Pointer) ptr, aip, orig, SLRI_YLINK_SET);
}


/**************************************************
*
*    CLinkSetNew()
*
**************************************************/
NLM_EXTERN CLinkSetPtr LIBCALL CLinkSetNew(void)
{
   CLinkSetPtr ptr = MemNew((size_t) sizeof(CLinkSet));

   return ptr;
}
/*****************************************************************************
*
*   CLinkSetNew2(Int4 num)
*
*****************************************************************************/
NLM_EXTERN CLinkSetPtr LIBCALL CLinkSetNew2(Int4 num)
{
   CLinkSetPtr ptr = MemNew((size_t) sizeof(CLinkSet));
   Int4Ptr iptr = MemNew((size_t) sizeof(Int4)*(num+1));	
   CharPtr * cptr = MemNew((size_t) sizeof(CharPtr)*(num+1));
   
   ptr->num = num;   
   ptr->uids = iptr;
   ptr->weights = cptr;

   return ptr;
}

/**************************************************
*
*    CLinkSetFree()
*
**************************************************/
NLM_EXTERN CLinkSetPtr LIBCALL CLinkSetFree(CLinkSetPtr ptr)
{
   Int4 i = 0;
   if(ptr == NULL) {
      return NULL;
   }
   ptr->uids = MemFree(ptr->uids);
   for(i = 0; i < ptr->num; i++) {
	   ptr->weights[i] = MemFree(ptr->weights[i]);
   }
   ptr->weights = MemFree(ptr->weights);

   return MemFree(ptr);
}



/**************************************************
*
*    CLinkSetListNew()
*
**************************************************/
NLM_EXTERN CLinkSetListPtr LIBCALL CLinkSetListNew(void)
{
   CLinkSetListPtr ptr = MemNew((size_t) sizeof(CLinkSetList));

   return ptr;
}


/**************************************************
*
*    CLinkSetListFree()
*
**************************************************/
NLM_EXTERN CLinkSetListPtr LIBCALL CLinkSetListFree(CLinkSetListPtr ptr)
{
   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> set1, (AsnOptFreeFunc) CLinkSetFree);
   return MemFree(ptr);
}


/**************************************************
*
*    CLinkSetList2New()
*
**************************************************/
NLM_EXTERN CLinkSetList2Ptr LIBCALL CLinkSetList2New(void)
{
   CLinkSetList2Ptr ptr = MemNew((size_t) sizeof(CLinkSetList2));

   return ptr;
}


/**************************************************
*
*    CLinkSetList2Free()
*
**************************************************/
NLM_EXTERN CLinkSetList2Ptr LIBCALL CLinkSetList2Free(CLinkSetList2Ptr ptr)
{
   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> set2, (AsnOptFreeFunc) CLinkSetListFree);
   return MemFree(ptr);
}



/**************************************************
*
*    CLinkSetList3New()
*
**************************************************/
NLM_EXTERN CLinkSetList3Ptr LIBCALL CLinkSetList3New(void)
{
   CLinkSetList3Ptr ptr = MemNew((size_t) sizeof(CLinkSetList3));

   return ptr;
}


/**************************************************
*
*    CLinkSetList3Free()
*
**************************************************/
NLM_EXTERN CLinkSetList3Ptr LIBCALL CLinkSetList3Free(CLinkSetList3Ptr ptr)
{
   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> set3, (AsnOptFreeFunc) CLinkSetList2Free);
   return MemFree(ptr);
}



/**************************************************
*
*    FLinkSetNew()
*
**************************************************/
NLM_EXTERN 
FLinkSetPtr LIBCALL
FLinkSetNew(void)
{
   FLinkSetPtr ptr = MemNew((size_t) sizeof(FLinkSet));

   return ptr;

}

/*****************************************************************************
*
*   FLinkSetNew2(Int4 num)
*
*****************************************************************************/
NLM_EXTERN 
FLinkSetPtr LIBCALL
FLinkSetNew2(Int4 num)
{
   FLinkSetPtr ptr = MemNew((size_t) sizeof(FLinkSet));
   Int4Ptr iptr = MemNew((size_t) sizeof(Int4)*(num+1));	
   FloatHiPtr fptr = MemNew((size_t) sizeof(FloatHi)*(num+1));
   
   ptr->num = num;   
   ptr->uids = iptr;
   ptr->weights = fptr;

   return ptr;

}

/**************************************************
*
*    FLinkSetFree()
*
**************************************************/
NLM_EXTERN 
FLinkSetPtr LIBCALL
FLinkSetFree(FLinkSetPtr ptr)
{

   if(ptr == NULL) {
      return (FLinkSetPtr) NULL;
   }
   ptr->uids = MemFree(ptr->uids);
   ptr->weights = MemFree(ptr->weights);

   return (FLinkSetPtr)MemFree(ptr);
}



/**************************************************
*
*    FLinkSetListNew()
*
**************************************************/
NLM_EXTERN 
FLinkSetListPtr LIBCALL
FLinkSetListNew(void)
{
   FLinkSetListPtr ptr = MemNew((size_t) sizeof(FLinkSetList));

   return ptr;

}


/**************************************************
*
*    FLinkSetListFree()
*
**************************************************/
NLM_EXTERN 
FLinkSetListPtr LIBCALL
FLinkSetListFree(FLinkSetListPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> set1, (AsnOptFreeFunc) FLinkSetFree);
   return MemFree(ptr);
}




/**************************************************
*
*    FLinkSetList2New()
*
**************************************************/
NLM_EXTERN 
FLinkSetList2Ptr LIBCALL
FLinkSetList2New(void)
{
   FLinkSetList2Ptr ptr = MemNew((size_t) sizeof(FLinkSetList2));

   return ptr;

}


/**************************************************
*
*    FLinkSetList2Free()
*
**************************************************/
NLM_EXTERN 
FLinkSetList2Ptr LIBCALL
FLinkSetList2Free(FLinkSetList2Ptr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> set2, (AsnOptFreeFunc) FLinkSetListFree);
   return MemFree(ptr);
}



/**************************************************
*
*    FLinkSetList3New()
*
**************************************************/
NLM_EXTERN 
FLinkSetList3Ptr LIBCALL
FLinkSetList3New(void)
{
   FLinkSetList3Ptr ptr = MemNew((size_t) sizeof(FLinkSetList3));

   return ptr;

}


/**************************************************
*
*    FLinkSetList3Free()
*
**************************************************/
NLM_EXTERN 
FLinkSetList3Ptr LIBCALL
FLinkSetList3Free(FLinkSetList3Ptr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> set3, (AsnOptFreeFunc) FLinkSetList2Free);
   return MemFree(ptr);
}




/**************************************************
*
*    ILinkSetNew()
*
**************************************************/
NLM_EXTERN 
ILinkSetPtr LIBCALL
ILinkSetNew(void)
{
   ILinkSetPtr ptr = MemNew((size_t) sizeof(ILinkSet));

   return ptr;

}

/*****************************************************************************
*
*   ILinkSetNew2(Int4 num)
*
*****************************************************************************/
NLM_EXTERN 
ILinkSetPtr LIBCALL
ILinkSetNew2(Int4 num)
{
   ILinkSetPtr ptr = MemNew((size_t) sizeof(ILinkSet));
   Int4Ptr uptr = MemNew((size_t) sizeof(Int4)*(num+1));	
   Int4Ptr wptr = MemNew((size_t) sizeof(Int4)*(num+1));
   
   ptr->num = num;
   ptr->uids = uptr;
   ptr->weights = wptr;

   return ptr;

}

/**************************************************
*
*    ILinkSetFree()
*
**************************************************/
NLM_EXTERN 
ILinkSetPtr LIBCALL
ILinkSetFree(ILinkSetPtr ptr)
{

   if(ptr == NULL) {
      return (ILinkSetPtr) NULL;
   }
   ptr->uids = MemFree(ptr->uids);
   ptr->weights = MemFree(ptr->weights);

   return (ILinkSetPtr)MemFree(ptr);
}






/**************************************************
*
*    ILinkSetListNew()
*
**************************************************/
NLM_EXTERN 
ILinkSetListPtr LIBCALL
ILinkSetListNew(void)
{
   ILinkSetListPtr ptr = MemNew((size_t) sizeof(ILinkSetList));

   return ptr;

}


/**************************************************
*
*    ILinkSetListFree()
*
**************************************************/
NLM_EXTERN 
ILinkSetListPtr LIBCALL
ILinkSetListFree(ILinkSetListPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> set1, (AsnOptFreeFunc) ILinkSetFree);
   return MemFree(ptr);
}



/**************************************************
*
*    ILinkSetList2New()
*
**************************************************/
NLM_EXTERN 
ILinkSetList2Ptr LIBCALL
ILinkSetList2New(void)
{
   ILinkSetList2Ptr ptr = MemNew((size_t) sizeof(ILinkSetList2));

   return ptr;

}


/**************************************************
*
*    ILinkSetList2Free()
*
**************************************************/
NLM_EXTERN 
ILinkSetList2Ptr LIBCALL
ILinkSetList2Free(ILinkSetList2Ptr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> set2, (AsnOptFreeFunc) ILinkSetListFree);
   return MemFree(ptr);
}



/**************************************************
*
*    ILinkSetList3New()
*
**************************************************/
NLM_EXTERN 
ILinkSetList3Ptr LIBCALL
ILinkSetList3New(void)
{
   ILinkSetList3Ptr ptr = MemNew((size_t) sizeof(ILinkSetList3));

   return ptr;

}


/**************************************************
*
*    ILinkSetList3Free()
*
**************************************************/
NLM_EXTERN 
ILinkSetList3Ptr LIBCALL
ILinkSetList3Free(ILinkSetList3Ptr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> set3, (AsnOptFreeFunc) ILinkSetList2Free);
   return MemFree(ptr);
}


/**************************************************
*
*    BLinkSetNew()
*
**************************************************/
NLM_EXTERN BLinkSetPtr LIBCALL BLinkSetNew(void)
{
   BLinkSetPtr ptr = MemNew((size_t) sizeof(BLinkSet));

   return ptr;
}
/*****************************************************************************
*
*   BLinkSetNew2(Int4 num)
*
*****************************************************************************/
NLM_EXTERN BLinkSetPtr LIBCALL BLinkSetNew2(Int4 num)
{
   BLinkSetPtr ptr = MemNew((size_t) sizeof(BLinkSet));
   Int4Ptr iptr = MemNew((size_t) sizeof(Int4)*(num+1));	
   Boolean * bptr = MemNew((size_t) sizeof(Boolean)*(num+1));
   
   ptr->num = num;   
   ptr->uids = iptr;
   ptr->weights = bptr;

   return ptr;
}

/**************************************************
*
*    BLinkSetFree()
*
**************************************************/
NLM_EXTERN BLinkSetPtr LIBCALL BLinkSetFree(BLinkSetPtr ptr)
{
   if(ptr == NULL) {
      return NULL;
   }
   ptr->uids = MemFree(ptr->uids);
   ptr->weights = MemFree(ptr->weights);

   return MemFree(ptr);
}



/**************************************************
*
*    BLinkSetListNew()
*
**************************************************/
NLM_EXTERN BLinkSetListPtr LIBCALL BLinkSetListNew(void)
{
   BLinkSetListPtr ptr = MemNew((size_t) sizeof(BLinkSetList));

   return ptr;
}


/**************************************************
*
*    BLinkSetListFree()
*
**************************************************/
NLM_EXTERN BLinkSetListPtr LIBCALL BLinkSetListFree(BLinkSetListPtr ptr)
{
   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> set1, (AsnOptFreeFunc) BLinkSetFree);
   return MemFree(ptr);
}


/**************************************************
*
*    BLinkSetList2New()
*
**************************************************/
NLM_EXTERN BLinkSetList2Ptr LIBCALL BLinkSetList2New(void)
{
   BLinkSetList2Ptr ptr = MemNew((size_t) sizeof(BLinkSetList2));

   return ptr;
}


/**************************************************
*
*    BLinkSetList2Free()
*
**************************************************/
NLM_EXTERN BLinkSetList2Ptr LIBCALL BLinkSetList2Free(BLinkSetList2Ptr ptr)
{
   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> set2, (AsnOptFreeFunc) BLinkSetListFree);
   return MemFree(ptr);
}



/**************************************************
*
*    BLinkSetList3New()
*
**************************************************/
NLM_EXTERN BLinkSetList3Ptr LIBCALL BLinkSetList3New(void)
{
   BLinkSetList3Ptr ptr = MemNew((size_t) sizeof(BLinkSetList3));

   return ptr;
}


/**************************************************
*
*    BLinkSetList3Free()
*
**************************************************/
NLM_EXTERN BLinkSetList3Ptr LIBCALL BLinkSetList3Free(BLinkSetList3Ptr ptr)
{
   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> set3, (AsnOptFreeFunc) BLinkSetList2Free);
   return MemFree(ptr);
}

/**************************************************
*
*    LLinkSetNew()
*
**************************************************/
NLM_EXTERN LLinkSetPtr LIBCALL LLinkSetNew(void)
{
   LLinkSetPtr ptr = MemNew((size_t) sizeof(LLinkSet));

   return ptr;
}
/*****************************************************************************
*
*   LLinkSetNew2(Int4 num)
*
*****************************************************************************/
NLM_EXTERN LLinkSetPtr LIBCALL LLinkSetNew2(Int4 num)
{
   LLinkSetPtr ptr = MemNew((size_t) sizeof(LLinkSet));
   Int4Ptr iptr = MemNew((size_t) sizeof(Int4)*(num+1));	
   Int8Ptr lptr = MemNew((size_t) sizeof(Int8)*(num+1));
   
   ptr->num = num;   
   ptr->uids = iptr;
   ptr->weights = lptr;

   return ptr;
}

/**************************************************
*
*    LLinkSetFree()
*
**************************************************/
NLM_EXTERN LLinkSetPtr LIBCALL LLinkSetFree(LLinkSetPtr ptr)
{
   if(ptr == NULL) {
      return NULL;
   }
   ptr->uids = MemFree(ptr->uids);
   ptr->weights = MemFree(ptr->weights);

   return MemFree(ptr);
}



/**************************************************
*
*    LLinkSetListNew()
*
**************************************************/
NLM_EXTERN LLinkSetListPtr LIBCALL LLinkSetListNew(void)
{
   LLinkSetListPtr ptr = MemNew((size_t) sizeof(LLinkSetList));

   return ptr;
}


/**************************************************
*
*    LLinkSetListFree()
*
**************************************************/
NLM_EXTERN LLinkSetListPtr LIBCALL LLinkSetListFree(LLinkSetListPtr ptr)
{
   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> set1, (AsnOptFreeFunc) LLinkSetFree);
   return MemFree(ptr);
}


/**************************************************
*
*    LLinkSetList2New()
*
**************************************************/
NLM_EXTERN LLinkSetList2Ptr LIBCALL LLinkSetList2New(void)
{
   LLinkSetList2Ptr ptr = MemNew((size_t) sizeof(LLinkSetList2));

   return ptr;
}


/**************************************************
*
*    LLinkSetList2Free()
*
**************************************************/
NLM_EXTERN LLinkSetList2Ptr LIBCALL LLinkSetList2Free(LLinkSetList2Ptr ptr)
{
   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> set2, (AsnOptFreeFunc) LLinkSetListFree);
   return MemFree(ptr);
}



/**************************************************
*
*    LLinkSetList3New()
*
**************************************************/
NLM_EXTERN LLinkSetList3Ptr LIBCALL LLinkSetList3New(void)
{
   LLinkSetList3Ptr ptr = MemNew((size_t) sizeof(LLinkSetList3));

   return ptr;
}


/**************************************************
*
*    LLinkSetList3Free()
*
**************************************************/
NLM_EXTERN LLinkSetList3Ptr LIBCALL LLinkSetList3Free(LLinkSetList3Ptr ptr)
{
   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> set3, (AsnOptFreeFunc) LLinkSetList2Free);
   return MemFree(ptr);
}

/**************************************************
*
*    YLinkSetNew()
*
**************************************************/
NLM_EXTERN YLinkSetPtr LIBCALL YLinkSetNew(void)
{
   YLinkSetPtr ptr = MemNew((size_t) sizeof(YLinkSet));

   return ptr;
}
/*****************************************************************************
*
*   YLinkSetNew2(Int4 num)
*
*****************************************************************************/
NLM_EXTERN YLinkSetPtr LIBCALL YLinkSetNew2(Int4 num)
{
   YLinkSetPtr ptr = MemNew((size_t) sizeof(YLinkSet));
   Int4Ptr iptr = MemNew((size_t) sizeof(Int4)*(num+1));	
   Byte * yptr = MemNew((size_t) sizeof(Byte)*(num+1));
   
   ptr->num = num;   
   ptr->uids = iptr;
   ptr->weights = yptr;

   return ptr;
}

/**************************************************
*
*    YLinkSetFree()
*
**************************************************/
NLM_EXTERN YLinkSetPtr LIBCALL YLinkSetFree(YLinkSetPtr ptr)
{
   if(ptr == NULL) {
      return NULL;
   }
   ptr->uids = MemFree(ptr->uids);
   ptr->weights = MemFree(ptr->weights);

   return MemFree(ptr);
}



/**************************************************
*
*    YLinkSetListNew()
*
**************************************************/
NLM_EXTERN YLinkSetListPtr LIBCALL YLinkSetListNew(void)
{
   YLinkSetListPtr ptr = MemNew((size_t) sizeof(YLinkSetList));

   return ptr;
}


/**************************************************
*
*    YLinkSetListFree()
*
**************************************************/
NLM_EXTERN YLinkSetListPtr LIBCALL YLinkSetListFree(YLinkSetListPtr ptr)
{
   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> set1, (AsnOptFreeFunc) YLinkSetFree);
   return MemFree(ptr);
}


/**************************************************
*
*    YLinkSetList2New()
*
**************************************************/
NLM_EXTERN YLinkSetList2Ptr LIBCALL YLinkSetList2New(void)
{
   YLinkSetList2Ptr ptr = MemNew((size_t) sizeof(YLinkSetList2));

   return ptr;
}


/**************************************************
*
*    YLinkSetList2Free()
*
**************************************************/
NLM_EXTERN YLinkSetList2Ptr LIBCALL YLinkSetList2Free(YLinkSetList2Ptr ptr)
{
   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> set2, (AsnOptFreeFunc) YLinkSetListFree);
   return MemFree(ptr);
}



/**************************************************
*
*    YLinkSetList3New()
*
**************************************************/
NLM_EXTERN YLinkSetList3Ptr LIBCALL YLinkSetList3New(void)
{
   YLinkSetList3Ptr ptr = MemNew((size_t) sizeof(YLinkSetList3));

   return ptr;
}


/**************************************************
*
*    YLinkSetList3Free()
*
**************************************************/
NLM_EXTERN YLinkSetList3Ptr LIBCALL YLinkSetList3Free(YLinkSetList3Ptr ptr)
{
   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> set3, (AsnOptFreeFunc) YLinkSetList2Free);
   return MemFree(ptr);
}





/**************************************************
*
*    IVectorNew()
*
**************************************************/
NLM_EXTERN 
IVectorPtr LIBCALL
IVectorNewEx(void)
{
   IVectorPtr ptr = MemNew((size_t) sizeof(IVector));
   return ptr;
}

NLM_EXTERN 
IVectorPtr LIBCALL
IVectorNew(Int4 i)
{
   IVectorPtr ptr = IVectorNewEx();
   ptr->i = i;
   ptr->v = MemNew((size_t)sizeof(Int4)*i);
   return ptr;
}


/**************************************************
*
*    IVectorFree()
*
**************************************************/
NLM_EXTERN 
IVectorPtr LIBCALL
IVectorFree(IVectorPtr ptr)
{
   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr->v);
   return MemFree(ptr);
}


/**************************************************
*
*    IVectorAsnRead()
*
**************************************************/
NLM_EXTERN 
IVectorPtr LIBCALL
IVectorAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   IVectorPtr ptr;
   Int4 i=0;

   if (! loaded)
   {
      if (! objslrilinksetAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* IVector ::= (self contained) */
      atp = AsnReadId(aip, amp, IVECTOR);
   } else {
      atp = AsnLinkType(orig, IVECTOR);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = IVectorNewEx();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == IVECTOR_i) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> i = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == IVECTOR_v) {
      ptr -> v = (Int4Ptr) MemNew((size_t)sizeof(Int4)*(ptr->i));
	  if (AsnReadVal(aip, atp, &av) <= 0) goto erret; 
	  i = 0;
      while((atp = AsnReadId(aip, amp, atp)) == IVECTOR_v_E) {
		if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
		ptr->v[i] = av.intvalue;
		i++;
		if (i > ptr->i) break;
	  }
      if (isError && ptr -> v == NULL) {
         goto erret;
      }
	  if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end seq of v */
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {
      goto erret;
   }
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = IVectorFree(ptr);
   goto ret;
}



/**************************************************
*
*    IVectorAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
IVectorAsnWrite(IVectorPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
	Boolean retval = FALSE;
	Int4 i=0;

	if (! loaded) {
	   if (! objslrilinksetAsnLoad()) return FALSE;
	}

	if (aip == NULL) {return FALSE;}

	atp = AsnLinkType(orig, IVECTOR);   /* link local tree */
	if (atp == NULL) {return FALSE;}

	if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
	if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {goto erret;}

	av.intvalue = ptr -> i;
	retval = AsnWrite(aip, IVECTOR_i,  &av);

   	if (! AsnOpenStruct(aip, IVECTOR_v, (Pointer) ptr->v)) goto erret;
	for (i = 0; i < ptr->i; i++) {
		av.intvalue = ptr->v[i];
		if (! AsnWrite(aip, IVECTOR_v_E, &av)) goto erret;
	}
	if (! AsnCloseStruct(aip, IVECTOR_v, (Pointer) ptr->v)) goto erret;
	if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {goto erret;}
	retval = TRUE;

erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}



/**************************************************
*
*    IVectorListNew()
*
**************************************************/
NLM_EXTERN 
IVectorListPtr LIBCALL
IVectorListNew(void)
{
   IVectorListPtr ptr = MemNew((size_t) sizeof(IVectorList));

   return ptr;

}


/**************************************************
*
*    IVectorListFree()
*
**************************************************/
NLM_EXTERN 
IVectorListPtr LIBCALL
IVectorListFree(IVectorListPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> iV, (AsnOptFreeFunc) IVectorFree);
   return MemFree(ptr);
}


/**************************************************
*
*    IVectorListAsnRead()
*
**************************************************/
NLM_EXTERN 
IVectorListPtr LIBCALL
IVectorListAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   IVectorListPtr ptr;

   if (! loaded)
   {
      if (! objslrilinksetAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* IVectorList ::= (self contained) */
      atp = AsnReadId(aip, amp, IVECTORLIST);
   } else {
      atp = AsnLinkType(orig, IVECTORLIST);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = IVectorListNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == IVECTORLIST_id) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> id = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == IVECTORLIST_iV) {
      ptr -> iV = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) IVectorAsnRead, (AsnOptFreeFunc) IVectorFree);
      if (isError && ptr -> iV == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {
      goto erret;
   }
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = IVectorListFree(ptr);
   goto ret;
}



/**************************************************
*
*    IVectorListAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
IVectorListAsnWrite(IVectorListPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslrilinksetAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, IVECTORLIST);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> id;
   retval = AsnWrite(aip, IVECTORLIST_id,  &av);
   AsnGenericUserSeqOfAsnWrite(ptr -> iV, (AsnWriteFunc) IVectorAsnWrite, aip, IVECTORLIST_iV, IVECTORLIST_iV_E);
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    IMatrixNew()
*
**************************************************/
NLM_EXTERN 
IMatrixPtr LIBCALL
IMatrixNewEx(void)
{
   IMatrixPtr ptr = MemNew((size_t) sizeof(IMatrix));

   return ptr;

}
NLM_EXTERN 
IMatrixPtr LIBCALL
IMatrixNew(Int4 i, Int4 j)
{
	IMatrixPtr ptr = IMatrixNewEx();
	Int4 a,**m;
	ptr->i = i;
	ptr->j = j;
	m = MemNew((size_t)sizeof(Int4Ptr)*i);
	for(a=0;a<i;a++) m[a] = MemNew((size_t)sizeof(Int4)*j);
	ptr->m = m;
	return ptr;
}


/**************************************************
*
*    IMatrixFree()
*
**************************************************/
NLM_EXTERN 
IMatrixPtr LIBCALL
IMatrixFree(IMatrixPtr ptr)
{
	Int4 a,i,**m;
	if(ptr == NULL) { return NULL;}
	i = ptr->i;
	m = ptr->m;
	for(a=0;a<i;a++) {m[a] = MemFree(m[a]);}
	MemFree(m);
	return MemFree(ptr);
}


/**************************************************
*
*    IMatrixAsnRead()
*
**************************************************/
NLM_EXTERN 
IMatrixPtr LIBCALL
IMatrixAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   IMatrixPtr ptr;
   Int4 i,j,a,b,**m;

   if (! loaded)
   {
      if (! objslrilinksetAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {return NULL;}

   if (orig == NULL) {         /* IMatrix ::= (self contained) */
      atp = AsnReadId(aip, amp, IMATRIX);
   } else {
      atp = AsnLinkType(orig, IMATRIX);
   }
   /* link in local tree */
   if (atp == NULL) {return NULL;}

   ptr = IMatrixNewEx();
   if (ptr == NULL) {goto erret;}
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == IMATRIX_i) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {goto erret;}
      ptr -> i = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == IMATRIX_j) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {goto erret;}
      ptr -> j = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == IMATRIX_m) {
	   if (AsnReadVal(aip, atp, &av) <= 0) goto erret; 
	   i = ptr->i;
	   j = ptr->j;
	   m = MemNew((size_t)sizeof(Int4Ptr)*i);
	   for(a=0;a<i;a++) {
			m[a] = MemNew((size_t)sizeof(Int4)*j);
			for(b=0;b<j;b++) {
			    atp = AsnReadId(aip, amp, atp);
				if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
				m[a][b] = av.intvalue;
		   }
	   }
	   ptr->m = m;
	   atp = AsnReadId(aip, amp, atp);

      if (isError && ptr -> m == NULL) {goto erret;}
	  if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end of m */
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {goto erret;}
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = IMatrixFree(ptr);
   goto ret;
}



/**************************************************
*
*    IMatrixAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
IMatrixAsnWrite(IMatrixPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;
   Int4 a,b,i,j,**m;

   if (! loaded)
   {
      if (! objslrilinksetAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, IMATRIX);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = i = ptr -> i;
   retval = AsnWrite(aip, IMATRIX_i,  &av);
   av.intvalue = j = ptr -> j;
   retval = AsnWrite(aip, IMATRIX_j,  &av);

	if (! AsnOpenStruct(aip, IMATRIX_m, (Pointer) ptr->m)) goto erret;
	m = ptr->m;
	for (a = 0; a < i; a++) {
		for(b=0;b<j;b++) {
			av.intvalue = m[a][b];
			if (! AsnWrite(aip, IMATRIX_m_E, &av)) goto erret;
		}
	}

	if (! AsnCloseStruct(aip, IMATRIX_m, (Pointer) ptr->m)) goto erret;
	if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
	  goto erret;
	}
	retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}


/**************************************************
*
*    IMatrixListNew()
*
**************************************************/
NLM_EXTERN 
IMatrixListPtr LIBCALL
IMatrixListNew(void)
{
   IMatrixListPtr ptr = MemNew((size_t) sizeof(IMatrixList));

   return ptr;

}


/**************************************************
*
*    IMatrixListFree()
*
**************************************************/
NLM_EXTERN 
IMatrixListPtr LIBCALL
IMatrixListFree(IMatrixListPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> iM, (AsnOptFreeFunc) IMatrixFree);
   return MemFree(ptr);
}


/**************************************************
*
*    IMatrixListAsnRead()
*
**************************************************/
NLM_EXTERN 
IMatrixListPtr LIBCALL
IMatrixListAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   IMatrixListPtr ptr;

   if (! loaded)
   {
      if (! objslrilinksetAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* IMatrixList ::= (self contained) */
      atp = AsnReadId(aip, amp, IMATRIXLIST);
   } else {
      atp = AsnLinkType(orig, IMATRIXLIST);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = IMatrixListNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == IMATRIXLIST_id) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> id = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == IMATRIXLIST_iM) {
      ptr -> iM = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) IMatrixAsnRead, (AsnOptFreeFunc) IMatrixFree);
      if (isError && ptr -> iM == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {
      goto erret;
   }
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = IMatrixListFree(ptr);
   goto ret;
}



/**************************************************
*
*    IMatrixListAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
IMatrixListAsnWrite(IMatrixListPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslrilinksetAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, IMATRIXLIST);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> id;
   retval = AsnWrite(aip, IMATRIXLIST_id,  &av);
   AsnGenericUserSeqOfAsnWrite(ptr -> iM, (AsnWriteFunc) IMatrixAsnWrite, aip, IMATRIXLIST_iM, IMATRIXLIST_iM_E);
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    IMatrixListSetNew()
*
**************************************************/
NLM_EXTERN 
IMatrixListSetPtr LIBCALL
IMatrixListSetNew(void)
{
   IMatrixListSetPtr ptr = MemNew((size_t) sizeof(IMatrixListSet));

   return ptr;

}


/**************************************************
*
*    IMatrixListSetFree()
*
**************************************************/
NLM_EXTERN 
IMatrixListSetPtr LIBCALL
IMatrixListSetFree(IMatrixListSetPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> iML, (AsnOptFreeFunc) IMatrixFree);
   return MemFree(ptr);
}


/**************************************************
*
*    IMatrixListSetAsnRead()
*
**************************************************/
NLM_EXTERN 
IMatrixListSetPtr LIBCALL
IMatrixListSetAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   IMatrixListSetPtr ptr;

   if (! loaded)
   {
      if (! objslrilinksetAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* IMatrixListSet ::= (self contained) */
      atp = AsnReadId(aip, amp, IMATRIXLISTSET);
   } else {
      atp = AsnLinkType(orig, IMATRIXLISTSET);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = IMatrixListSetNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == IMATRIXLISTSET_id) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> id = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == IMATRIXLISTSET_iML) {
      ptr -> iML = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) IMatrixListAsnRead, (AsnOptFreeFunc) IMatrixFree);
      if (isError && ptr -> iML == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {
      goto erret;
   }
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = IMatrixListSetFree(ptr);
   goto ret;
}



/**************************************************
*
*    IMatrixListSetAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
IMatrixListSetAsnWrite(IMatrixListSetPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslrilinksetAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, IMATRIXLISTSET);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> id;
   retval = AsnWrite(aip, IMATRIXLISTSET_id,  &av);
   AsnGenericUserSeqOfAsnWrite(ptr -> iML, (AsnWriteFunc) IMatrixAsnWrite, aip, IMATRIXLISTSET_iML, IMATRIXLISTSET_iML_E);
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}

/**************************************************
*
*    FHiVectorNew()
*
**************************************************/
NLM_EXTERN 
FHiVectorPtr LIBCALL
FHiVectorNewEx(void)
{
   FHiVectorPtr ptr = MemNew((size_t) sizeof(FHiVector));
   return ptr;
}

NLM_EXTERN 
FHiVectorPtr LIBCALL
FHiVectorNew(Int4 i)
{
   FHiVectorPtr ptr = FHiVectorNewEx();
   ptr->i = i;
   ptr->v = MemNew((size_t)sizeof(FloatHi)*i);
   return ptr;
}


/**************************************************
*
*    FHiVectorFree()
*
**************************************************/
NLM_EXTERN 
FHiVectorPtr LIBCALL
FHiVectorFree(FHiVectorPtr ptr)
{
   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr->v);
   return MemFree(ptr);
}


/**************************************************
*
*    FHiVectorAsnRead()
*
**************************************************/
NLM_EXTERN 
FHiVectorPtr LIBCALL
FHiVectorAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   FHiVectorPtr ptr;
   Int4 i=0;

   if (! loaded)
   {
      if (! objslrilinksetAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* FHIVECTOR ::= (self contained) */
      atp = AsnReadId(aip, amp, FHIVECTOR);
   } else {
      atp = AsnLinkType(orig, FHIVECTOR);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = FHiVectorNewEx();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == FHIVECTOR_i) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> i = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == FHIVECTOR_v) {
      ptr -> v = (FloatHiPtr) MemNew((size_t)sizeof(FloatHi)*(ptr->i));
	  if (AsnReadVal(aip, atp, &av) <= 0) goto erret; 
	  i = 0;
      while((atp = AsnReadId(aip, amp, atp)) == FHIVECTOR_v_E) {
		if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
		ptr->v[i] = av.realvalue;
		i++;
		if (i > ptr->i) break;
	  }
      if (isError && ptr -> v == NULL) {
         goto erret;
      }
	  if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end seq of v */
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {
      goto erret;
   }
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = FHiVectorFree(ptr);
   goto ret;
}



/**************************************************
*
*    FHiVectorAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
FHiVectorAsnWrite(FHiVectorPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
	Boolean retval = FALSE;
	Int4 i=0;

	if (! loaded) {
	   if (! objslrilinksetAsnLoad()) return FALSE;
	}

	if (aip == NULL) {return FALSE;}

	atp = AsnLinkType(orig, FHIVECTOR);   /* link local tree */
	if (atp == NULL) {return FALSE;}

	if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
	if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {goto erret;}

	av.intvalue = ptr -> i;
	retval = AsnWrite(aip, FHIVECTOR_i,  &av);

   	if (! AsnOpenStruct(aip, FHIVECTOR_v, (Pointer) ptr->v)) goto erret;
	for (i = 0; i < ptr->i; i++) {
		av.realvalue = ptr->v[i];
		if (! AsnWrite(aip, FHIVECTOR_v_E, &av)) goto erret;
	}
	if (! AsnCloseStruct(aip, FHIVECTOR_v, (Pointer) ptr->v)) goto erret;
	if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {goto erret;}
	retval = TRUE;

erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}



/**************************************************
*
*    FHiVectorListNew()
*
**************************************************/
NLM_EXTERN 
FHiVectorListPtr LIBCALL
FHiVectorListNew(void)
{
   FHiVectorListPtr ptr = MemNew((size_t) sizeof(FHiVectorList));

   return ptr;

}


/**************************************************
*
*    FHiVectorListFree()
*
**************************************************/
NLM_EXTERN 
FHiVectorListPtr LIBCALL
FHiVectorListFree(FHiVectorListPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> iV, (AsnOptFreeFunc) FHiVectorFree);
   return MemFree(ptr);
}


/**************************************************
*
*    FHiVectorListAsnRead()
*
**************************************************/
NLM_EXTERN 
FHiVectorListPtr LIBCALL
FHiVectorListAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   FHiVectorListPtr ptr;

   if (! loaded)
   {
      if (! objslrilinksetAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* FHiVectorList ::= (self contained) */
      atp = AsnReadId(aip, amp, FHIVECTORLIST);
   } else {
      atp = AsnLinkType(orig, FHIVECTORLIST);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = FHiVectorListNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == FHIVECTORLIST_id) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> id = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == FHIVECTORLIST_iV) {
      ptr -> iV = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) FHiVectorAsnRead, (AsnOptFreeFunc) FHiVectorFree);
      if (isError && ptr -> iV == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {
      goto erret;
   }
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = FHiVectorListFree(ptr);
   goto ret;
}



/**************************************************
*
*    FHiVectorListAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
FHiVectorListAsnWrite(FHiVectorListPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslrilinksetAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, FHIVECTORLIST);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> id;
   retval = AsnWrite(aip, FHIVECTORLIST_id,  &av);
   AsnGenericUserSeqOfAsnWrite(ptr -> iV, (AsnWriteFunc) FHiVectorAsnWrite, aip, FHIVECTORLIST_iV, FHIVECTORLIST_iV_E);
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    FHiMatrixNew()
*
**************************************************/
NLM_EXTERN 
FHiMatrixPtr LIBCALL
FHiMatrixNewEx(void)
{
   FHiMatrixPtr ptr = MemNew((size_t) sizeof(FHiMatrix));
   return ptr;
}
NLM_EXTERN 
FHiMatrixPtr LIBCALL
FHiMatrixNew(Int4 i, Int4 j)
{
	FHiMatrixPtr ptr = FHiMatrixNewEx();
	Int4 a;
	FloatHi **m;
	ptr->i = i;
	ptr->j = j;
	m = MemNew((size_t)sizeof(FloatHiPtr)*i);
	for(a=0;a<i;a++) m[a] = MemNew((size_t)sizeof(FloatHi)*j);
	ptr->m = m;
	return ptr;
}


/**************************************************
*
*    FHiMatrixFree()
*
**************************************************/
NLM_EXTERN 
FHiMatrixPtr LIBCALL
FHiMatrixFree(FHiMatrixPtr ptr)
{
	Int4 a,i;
	FloatHi **m;
	if(ptr == NULL) { return NULL;}
	i = ptr->i;
	m = ptr->m;
	for(a=0;a<i;a++) {m[a] = MemFree(m[a]);}
	MemFree(m);
	return MemFree(ptr);
}


/**************************************************
*
*    FHiMatrixAsnRead()
*
**************************************************/
NLM_EXTERN 
FHiMatrixPtr LIBCALL
FHiMatrixAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   FHiMatrixPtr ptr;
   Int4 i,j,a,b;
   FloatHi **m;

   if (! loaded)
   {
      if (! objslrilinksetAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {return NULL;}

   if (orig == NULL) {         /* FHIMATRIX ::= (self contained) */
      atp = AsnReadId(aip, amp, FHIMATRIX);
   } else {
      atp = AsnLinkType(orig, FHIMATRIX);
   }
   /* link in local tree */
   if (atp == NULL) {return NULL;}

   ptr = FHiMatrixNewEx();
   if (ptr == NULL) {goto erret;}
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == FHIMATRIX_i) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {goto erret;}
      ptr -> i = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == FHIMATRIX_j) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {goto erret;}
      ptr -> j = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == FHIMATRIX_m) {
	   if (AsnReadVal(aip, atp, &av) <= 0) goto erret; 
	   i = ptr->i;
	   j = ptr->j;
	   m = MemNew((size_t)sizeof(FloatHiPtr)*i);
	   for(a=0;a<i;a++) {
			m[a] = MemNew((size_t)sizeof(FloatHi)*j);
			for(b=0;b<j;b++) {
			    atp = AsnReadId(aip, amp, atp);
				if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
				m[a][b] = av.intvalue;
		   }
	   }
	   ptr->m = m;
	   atp = AsnReadId(aip, amp, atp);

      if (isError && ptr -> m == NULL) {goto erret;}
	  if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end of m */
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {goto erret;}
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = FHiMatrixFree(ptr);
   goto ret;
}



/**************************************************
*
*    FHiMatrixAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
FHiMatrixAsnWrite(FHiMatrixPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;
   Int4 a,b,i,j;
   FloatHi **m;

   if (! loaded)
   {
      if (! objslrilinksetAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, FHIMATRIX);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = i = ptr -> i;
   retval = AsnWrite(aip, FHIMATRIX_i,  &av);
   av.intvalue = j = ptr -> j;
   retval = AsnWrite(aip, FHIMATRIX_j,  &av);

	if (! AsnOpenStruct(aip, FHIMATRIX_m, (Pointer) ptr->m)) goto erret;
	m = ptr->m;
	for (a = 0; a < i; a++) {
		for(b=0;b<j;b++) {
			av.realvalue = m[a][b];
			if (! AsnWrite(aip, FHIMATRIX_m_E, &av)) goto erret;
		}
	}

	if (! AsnCloseStruct(aip, FHIMATRIX_m, (Pointer) ptr->m)) goto erret;
	if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
	  goto erret;
	}
	retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}


/**************************************************
*
*    FHiMatrixListNew()
*
**************************************************/
NLM_EXTERN 
FHiMatrixListPtr LIBCALL
FHiMatrixListNew(void)
{
   FHiMatrixListPtr ptr = MemNew((size_t) sizeof(FHiMatrixList));
   return ptr;
}


/**************************************************
*
*    FHiMatrixListFree()
*
**************************************************/
NLM_EXTERN 
FHiMatrixListPtr LIBCALL
FHiMatrixListFree(FHiMatrixListPtr ptr)
{
   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> iM, (AsnOptFreeFunc) FHiMatrixFree);
   return MemFree(ptr);
}


/**************************************************
*
*    FHiMatrixListAsnRead()
*
**************************************************/
NLM_EXTERN 
FHiMatrixListPtr LIBCALL
FHiMatrixListAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   FHiMatrixListPtr ptr;

   if (! loaded)
   {
      if (! objslrilinksetAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* FHiMatrixList ::= (self contained) */
      atp = AsnReadId(aip, amp, FHIMATRIXLIST);
   } else {
      atp = AsnLinkType(orig, FHIMATRIXLIST);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = FHiMatrixListNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == FHIMATRIXLIST_id) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> id = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == FHIMATRIXLIST_iM) {
      ptr -> iM = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) FHiMatrixAsnRead, (AsnOptFreeFunc) FHiMatrixFree);
      if (isError && ptr -> iM == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {
      goto erret;
   }
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = FHiMatrixListFree(ptr);
   goto ret;
}



/**************************************************
*
*    FHiMatrixListAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
FHiMatrixListAsnWrite(FHiMatrixListPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslrilinksetAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, FHIMATRIXLIST);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> id;
   retval = AsnWrite(aip, FHIMATRIXLIST_id,  &av);
   AsnGenericUserSeqOfAsnWrite(ptr -> iM, (AsnWriteFunc) FHiMatrixAsnWrite, aip, FHIMATRIXLIST_iM, FHIMATRIXLIST_iM_E);
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    FHiMatrixListSetNew()
*
**************************************************/
NLM_EXTERN 
FHiMatrixListSetPtr LIBCALL
FHiMatrixListSetNew(void)
{
   FHiMatrixListSetPtr ptr = MemNew((size_t) sizeof(FHiMatrixListSet));

   return ptr;

}


/**************************************************
*
*    FHiMatrixListSetFree()
*
**************************************************/
NLM_EXTERN 
FHiMatrixListSetPtr LIBCALL
FHiMatrixListSetFree(FHiMatrixListSetPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> iML, (AsnOptFreeFunc) FHiMatrixFree);
   return MemFree(ptr);
}


/**************************************************
*
*    FHiMatrixListSetAsnRead()
*
**************************************************/
NLM_EXTERN 
FHiMatrixListSetPtr LIBCALL
FHiMatrixListSetAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   FHiMatrixListSetPtr ptr;

   if (! loaded)
   {
      if (! objslrilinksetAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* FHiMatrixListSet ::= (self contained) */
      atp = AsnReadId(aip, amp, FHIMATRIXLISTSET);
   } else {
      atp = AsnLinkType(orig, FHIMATRIXLISTSET);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = FHiMatrixListSetNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == FHIMATRIXLISTSET_id) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> id = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == FHIMATRIXLISTSET_iML) {
      ptr -> iML = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) FHiMatrixListAsnRead, (AsnOptFreeFunc) FHiMatrixFree);
      if (isError && ptr -> iML == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {
      goto erret;
   }
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = FHiMatrixListSetFree(ptr);
   goto ret;
}



/**************************************************
*
*    FHiMatrixListSetAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
FHiMatrixListSetAsnWrite(FHiMatrixListSetPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslrilinksetAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, FHIMATRIXLISTSET);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> id;
   retval = AsnWrite(aip, FHIMATRIXLISTSET_id,  &av);
   AsnGenericUserSeqOfAsnWrite(ptr -> iML, (AsnWriteFunc) FHiMatrixAsnWrite, aip, FHIMATRIXLISTSET_iML, FHIMATRIXLISTSET_iML_E);
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}


/**************************************************
*
*    FLoVectorNew()
*
**************************************************/
NLM_EXTERN 
FLoVectorPtr LIBCALL
FLoVectorNewEx(void)
{
   FLoVectorPtr ptr = MemNew((size_t) sizeof(FLoVector));
   return ptr;
}

NLM_EXTERN 
FLoVectorPtr LIBCALL
FLoVectorNew(Int4 i)
{
   FLoVectorPtr ptr = FLoVectorNewEx();
   ptr->i = i;
   ptr->v = MemNew((size_t)sizeof(FloatLo)*i);
   return ptr;
}


/**************************************************
*
*    FLoVectorFree()
*
**************************************************/
NLM_EXTERN 
FLoVectorPtr LIBCALL
FLoVectorFree(FLoVectorPtr ptr)
{
   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr->v);
   return MemFree(ptr);
}


/**************************************************
*
*    FLoVectorAsnRead()
*
**************************************************/
NLM_EXTERN 
FLoVectorPtr LIBCALL
FLoVectorAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   FLoVectorPtr ptr;
   Int4 i=0;

   if (! loaded)
   {
      if (! objslrilinksetAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* FLOVECTOR ::= (self contained) */
      atp = AsnReadId(aip, amp, FLOVECTOR);
   } else {
      atp = AsnLinkType(orig, FLOVECTOR);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = FLoVectorNewEx();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == FLOVECTOR_i) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> i = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == FLOVECTOR_v) {
      ptr -> v = (FloatLoPtr) MemNew((size_t)sizeof(FloatLo)*(ptr->i));
	  if (AsnReadVal(aip, atp, &av) <= 0) goto erret; 
	  i = 0;
      while((atp = AsnReadId(aip, amp, atp)) == FLOVECTOR_v_E) {
		if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
		ptr->v[i] = (FloatLo) av.realvalue;
		i++;
		if (i > ptr->i) break;
	  }
      if (isError && ptr -> v == NULL) {
         goto erret;
      }
	  if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end seq of v */
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {
      goto erret;
   }
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = FLoVectorFree(ptr);
   goto ret;
}



/**************************************************
*
*    FLoVectorAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
FLoVectorAsnWrite(FLoVectorPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
	Boolean retval = FALSE;
	Int4 i=0;

	if (! loaded) {
	   if (! objslrilinksetAsnLoad()) return FALSE;
	}

	if (aip == NULL) {return FALSE;}

	atp = AsnLinkType(orig, FLOVECTOR);   /* link local tree */
	if (atp == NULL) {return FALSE;}

	if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
	if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {goto erret;}

	av.intvalue = ptr -> i;
	retval = AsnWrite(aip, FLOVECTOR_i,  &av);

   	if (! AsnOpenStruct(aip, FLOVECTOR_v, (Pointer) ptr->v)) goto erret;
	for (i = 0; i < ptr->i; i++) {
		av.realvalue = (FloatHi) ptr->v[i];
		if (! AsnWrite(aip, FLOVECTOR_v_E, &av)) goto erret;
	}
	if (! AsnCloseStruct(aip, FLOVECTOR_v, (Pointer) ptr->v)) goto erret;
	if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {goto erret;}
	retval = TRUE;

erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}



/**************************************************
*
*    FLoVectorListNew()
*
**************************************************/
NLM_EXTERN 
FLoVectorListPtr LIBCALL
FLoVectorListNew(void)
{
   FLoVectorListPtr ptr = MemNew((size_t) sizeof(FLoVectorList));

   return ptr;

}


/**************************************************
*
*    FLoVectorListFree()
*
**************************************************/
NLM_EXTERN 
FLoVectorListPtr LIBCALL
FLoVectorListFree(FLoVectorListPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> iV, (AsnOptFreeFunc) FLoVectorFree);
   return MemFree(ptr);
}


/**************************************************
*
*    FLoVectorListAsnRead()
*
**************************************************/
NLM_EXTERN 
FLoVectorListPtr LIBCALL
FLoVectorListAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   FLoVectorListPtr ptr;

   if (! loaded)
   {
      if (! objslrilinksetAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* FLoVectorList ::= (self contained) */
      atp = AsnReadId(aip, amp, FLOVECTORLIST);
   } else {
      atp = AsnLinkType(orig, FLOVECTORLIST);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = FLoVectorListNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == FLOVECTORLIST_id) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> id = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == FLOVECTORLIST_iV) {
      ptr -> iV = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) FLoVectorAsnRead, (AsnOptFreeFunc) FLoVectorFree);
      if (isError && ptr -> iV == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {
      goto erret;
   }
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = FLoVectorListFree(ptr);
   goto ret;
}



/**************************************************
*
*    FLoVectorListAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
FLoVectorListAsnWrite(FLoVectorListPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslrilinksetAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, FLOVECTORLIST);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> id;
   retval = AsnWrite(aip, FLOVECTORLIST_id,  &av);
   AsnGenericUserSeqOfAsnWrite(ptr -> iV, (AsnWriteFunc) FLoVectorAsnWrite, aip, FLOVECTORLIST_iV, FLOVECTORLIST_iV_E);
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    CVectorNew()
*
**************************************************/
NLM_EXTERN 
CVectorPtr LIBCALL
CVectorNewEx(void)
{
   CVectorPtr ptr = MemNew((size_t) sizeof(CVector));
   return ptr;
}

NLM_EXTERN 
CVectorPtr LIBCALL
CVectorNew(Int4 i)
{
   CVectorPtr ptr = CVectorNewEx();
   ptr->i = i;
   ptr->v = (CharPtr*) MemNew((size_t)sizeof(CharPtr)*i);
   return ptr;
}


/**************************************************
*
*    CVectorFree()
*
**************************************************/
NLM_EXTERN 
CVectorPtr LIBCALL
CVectorFree(CVectorPtr ptr)
{
   Int4 i = 0;
   if(ptr == NULL) {
      return NULL;
   }
   for(i=0;i<ptr->i;i++) {
	MemFree(ptr->v[i]);
   }
   
   MemFree(ptr->v);
   return MemFree(ptr);
}


/**************************************************
*
*    CVectorAsnRead()
*
**************************************************/
NLM_EXTERN 
CVectorPtr LIBCALL
CVectorAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   CVectorPtr ptr;
   Int4 i=0;

   if (! loaded)
   {
      if (! objslrilinksetAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* CVector ::= (self contained) */
      atp = AsnReadId(aip, amp, CVECTOR);
   } else {
      atp = AsnLinkType(orig, CVECTOR);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = CVectorNewEx();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == CVECTOR_i) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> i = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == CVECTOR_v) {
      ptr -> v = (CharPtr*) MemNew((size_t)sizeof(CharPtr)*(ptr->i));
	  if (AsnReadVal(aip, atp, &av) <= 0) goto erret; 
	  i = 0;
      while((atp = AsnReadId(aip, amp, atp)) == CVECTOR_v_E) {
		if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
		ptr->v[i] = StringSave((CharPtr) av.ptrvalue);
		i++;
		if (i > ptr->i) break;
	  }
      if (isError && ptr -> v == NULL) {
         goto erret;
      }
	  if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end seq of v */
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {
      goto erret;
   }
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = CVectorFree(ptr);
   goto ret;
}



/**************************************************
*
*    CVectorAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
CVectorAsnWrite(CVectorPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
	Boolean retval = FALSE;
	Int4 i=0;

	if (! loaded) {
	   if (! objslrilinksetAsnLoad()) return FALSE;
	}

	if (aip == NULL) {return FALSE;}

	atp = AsnLinkType(orig, CVECTOR);   /* link local tree */
	if (atp == NULL) {return FALSE;}

	if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
	if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {goto erret;}

	av.intvalue = ptr -> i;
	retval = AsnWrite(aip, CVECTOR_i,  &av);

   	if (! AsnOpenStruct(aip, CVECTOR_v, (Pointer) ptr->v)) goto erret;
	for (i = 0; i < ptr->i; i++) {
		av.ptrvalue = (Pointer) StringSave(ptr->v[i]);
		if (! AsnWrite(aip, CVECTOR_v_E, &av)) goto erret;
	}
	if (! AsnCloseStruct(aip, CVECTOR_v, (Pointer) ptr->v)) goto erret;
	if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {goto erret;}
	retval = TRUE;

erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}



/**************************************************
*
*    CVectorListNew()
*
**************************************************/
NLM_EXTERN 
CVectorListPtr LIBCALL
CVectorListNew(void)
{
   CVectorListPtr ptr = MemNew((size_t) sizeof(CVectorList));

   return ptr;

}


/**************************************************
*
*    CVectorListFree()
*
**************************************************/
NLM_EXTERN 
CVectorListPtr LIBCALL
CVectorListFree(CVectorListPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> iV, (AsnOptFreeFunc) CVectorFree);
   return MemFree(ptr);
}


/**************************************************
*
*    CVectorListAsnRead()
*
**************************************************/
NLM_EXTERN 
CVectorListPtr LIBCALL
CVectorListAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   CVectorListPtr ptr;

   if (! loaded)
   {
      if (! objslrilinksetAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* CVectorList ::= (self contained) */
      atp = AsnReadId(aip, amp, CVECTORLIST);
   } else {
      atp = AsnLinkType(orig, CVECTORLIST);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = CVectorListNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == CVECTORLIST_id) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> id = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == CVECTORLIST_iV) {
      ptr -> iV = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) CVectorAsnRead, (AsnOptFreeFunc) CVectorFree);
      if (isError && ptr -> iV == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {
      goto erret;
   }
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = CVectorListFree(ptr);
   goto ret;
}



/**************************************************
*
*    CVectorListAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
CVectorListAsnWrite(CVectorListPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslrilinksetAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, CVECTORLIST);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> id;
   retval = AsnWrite(aip, CVECTORLIST_id,  &av);
   AsnGenericUserSeqOfAsnWrite(ptr -> iV, (AsnWriteFunc) CVectorAsnWrite, aip, CVECTORLIST_iV, CVECTORLIST_iV_E);
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**
  * $Log: objslrilinkset.c,v $
  * Revision 1.5  2004/02/19 23:47:26  mjdumont
  * Added FHiMatrix and lists
  *
  * Revision 1.4  2004/02/02 20:52:45  mjdumont
  * added CVector
  *
  * Revision 1.3  2003/11/05 18:41:07  michel
  * Changed FVector to FHiVector to avoid conflict with OS X's Carbon
  *
  * Revision 1.2  2003/10/20 18:38:51  michel
  * Added a FloatLo Vector ASN.1 type
  *
  * Revision 1.1.1.5  2003/08/21 08:48:05  root
  * Automated import from Sourceforge.net
  *
  * Revision 1.5  2003/08/20 14:41:30  micheld
  * Added FVector - FloatHi Vector.
  *
  */

