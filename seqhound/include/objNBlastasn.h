#ifndef _objNBlastasn_ 
#define _objNBlastasn_ 

#undef NLM_EXTERN
#ifdef NLM_IMPORT
#define NLM_EXTERN NLM_IMPORT
#else
#define NLM_EXTERN extern
#endif


#ifdef __cplusplus
extern "C" { /* } */
#endif


/**************************************************
*
*    Generated objects for Module NBlast
*    Generated using ASNCODE Revision: 6.15 at Apr 11, 2005  5:01 PM
*
**************************************************/

NLM_EXTERN Boolean LIBCALL
objNBlastasnAsnLoad PROTO((void));


/**************************************************
*
*    NBlastResultSet
*
**************************************************/
typedef struct struct_NBlast_Result_Set {
   Int4   query_gi;
   Int4   subject_gi;
   struct struct_NBlast_Result PNTR   results;
} NBlastResultSet, PNTR NBlastResultSetPtr;


NLM_EXTERN NBlastResultSetPtr LIBCALL NBlastResultSetFree PROTO ((NBlastResultSetPtr ));
NLM_EXTERN NBlastResultSetPtr LIBCALL NBlastResultSetNew PROTO (( void ));
NLM_EXTERN NBlastResultSetPtr LIBCALL NBlastResultSetAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL NBlastResultSetAsnWrite PROTO (( NBlastResultSetPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    NBlastResult
*
**************************************************/
typedef struct struct_NBlast_Result {
   struct struct_NBlast_Result PNTR next;
   Int4   numseg;
   ValNodePtr   starts;
   ValNodePtr   lens;
   struct struct_NBlast_Scores PNTR   scores;
} NBlastResult, PNTR NBlastResultPtr;


NLM_EXTERN NBlastResultPtr LIBCALL NBlastResultFree PROTO ((NBlastResultPtr ));
NLM_EXTERN NBlastResultPtr LIBCALL NBlastResultNew PROTO (( void ));
NLM_EXTERN NBlastResultPtr LIBCALL NBlastResultAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL NBlastResultAsnWrite PROTO (( NBlastResultPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    NBlastScores
*
**************************************************/
typedef struct struct_NBlast_Scores {
   struct struct_NBlast_Scores PNTR next;
   FloatHi   bitscore;
   FloatHi   evalue;
} NBlastScores, PNTR NBlastScoresPtr;


NLM_EXTERN NBlastScoresPtr LIBCALL NBlastScoresFree PROTO ((NBlastScoresPtr ));
NLM_EXTERN NBlastScoresPtr LIBCALL NBlastScoresNew PROTO (( void ));
NLM_EXTERN NBlastScoresPtr LIBCALL NBlastScoresAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL NBlastScoresAsnWrite PROTO (( NBlastScoresPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    NBlastGiAndEval
*
**************************************************/
typedef struct struct_NBlast_GiAndEval {
   struct struct_NBlast_GiAndEval PNTR next;
   Int4   subject_gi;
   FloatHi   evalue;
} NBlastGiAndEval, PNTR NBlastGiAndEvalPtr;


NLM_EXTERN NBlastGiAndEvalPtr LIBCALL NBlastGiAndEvalFree PROTO ((NBlastGiAndEvalPtr ));
NLM_EXTERN NBlastGiAndEvalPtr LIBCALL NBlastGiAndEvalNew PROTO (( void ));
NLM_EXTERN NBlastGiAndEvalPtr LIBCALL NBlastGiAndEvalAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL NBlastGiAndEvalAsnWrite PROTO (( NBlastGiAndEvalPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    NBlastGiAndEvalSet
*
**************************************************/
typedef struct struct_NBlast_GiAndEval_set {
   Int4   query_gi;
   struct struct_NBlast_GiAndEval PNTR   giandevals;
} NBlastGiAndEvalSet, PNTR NBlastGiAndEvalSetPtr;


NLM_EXTERN NBlastGiAndEvalSetPtr LIBCALL NBlastGiAndEvalSetFree PROTO ((NBlastGiAndEvalSetPtr ));
NLM_EXTERN NBlastGiAndEvalSetPtr LIBCALL NBlastGiAndEvalSetNew PROTO (( void ));
NLM_EXTERN NBlastGiAndEvalSetPtr LIBCALL NBlastGiAndEvalSetAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL NBlastGiAndEvalSetAsnWrite PROTO (( NBlastGiAndEvalSetPtr , AsnIoPtr, AsnTypePtr));

#ifdef __cplusplus
/* { */ }
#endif

#endif /* _objNBlastasn_ */

#undef NLM_EXTERN
#ifdef NLM_EXPORT
#define NLM_EXTERN NLM_EXPORT
#else
#define NLM_EXTERN
#endif

