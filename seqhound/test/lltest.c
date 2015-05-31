#include <ll_query.h>
#include <seqhound.h>


void printIntList ( ValNodePtr vnp )
{
   ValNodePtr v = NULL;
   Int4 count = 0;

   for ( v = vnp; v != NULL; v = v->next, ++count )
   {
      printf ( "%d ", v->data.intvalue );
   }
   printf("\nTotal: %d\n", count);
}

void printStrList ( ValNodePtr vnp )
{
   ValNodePtr v = NULL;
   Int4 count = 0;

   for ( v = vnp; v != NULL; v = v->next, ++count )
   {
      printf ( "%s ", v->data.ptrvalue );
   }
   printf("\nTotal: %d\n", count);
}

Boolean AssertIntListEquals ( ValNodePtr list1, ValNodePtr list2 )
{
   ValNodePtr v = NULL,
             v2 = NULL;

   if (NodeListLen (list1) != NodeListLen ( list2 ) )
       return FALSE;

   for ( v = list1, v2 = list2; 
         v != NULL; 
	 v = v->next, v2 = v2->next )
   {
      if ( v->data.intvalue != v2->data.intvalue )
      {
          return FALSE; 
      }
   }
   return TRUE;
}

Boolean AssertStrListEquals ( ValNodePtr list1, ValNodePtr list2 )
{
   ValNodePtr v = NULL,
             v2 = NULL;

   if (NodeListLen (list1) != NodeListLen ( list2 ) )
       return FALSE;

   for ( v = list1, v2 = list2; 
         v != NULL; 
	 v = v->next, v2 = v2->next )
   {
      if ( 0 != StringCmp(v->data.ptrvalue, v2->data.ptrvalue) )
      {
          return FALSE; 
      }
   }
   return TRUE;
}

Boolean AssertListIsEmpty ( ValNodePtr list1 )
{
   if ( NULL == list1 )
	 return TRUE;

   return FALSE;
}

Boolean AssertFloat ( FloatHi score )
{
   if ( 0 < score && 1 > score )
	   return TRUE;
   return FALSE;
}


ValNodePtr testLocusLinkSuite( int selector ) 
{
   ValNodePtr vnp    = NULL;
   ValNodePtr vnp2   = NULL;
   ValNodePtr result = NULL;
   Int4 gi = 0; 
   ValNodePtr failed = NULL;

   switch ( selector )
   {
      case 0:  vnp = SHoundGOIDFromLLID ( 2 );

	       if ( TRUE == AssertListIsEmpty (vnp) )
	          ValNodeAddStr ( &failed, 0, "GOIDFromLLID" );
	       break;

      case 1:  if ( 103950 != SHoundOMIMFromLLID ( 2 ) )
		   ValNodeAddStr ( &failed, 0, "OMIMFromLLID" );
	       /* printf ("%d\n", SHoundOMIMFromLLID ( gi ) ); */
	       break;

      case 2:  vnp = SHoundCDDIDFromLLID ( 1032 );

	       if ( TRUE == AssertListIsEmpty ( vnp ) )
		  ValNodeAddStr ( &failed, 0, "CDDIDFromLLID" );
	       /* printStrList(vnp); */
	       break;

      case 3:  NodeListFree ( result );
	       ValNodeAddInt ( &result, 0, 4557225 );
	       vnp = SHoundGiFromLLID ( 2 );

	       if ( FALSE == AssertIntListEquals ( vnp, result ) )
		  ValNodeAddStr ( &failed, 0, "GiFromLLID" );

               /* printIntList ( vnp ); */
	       break;
      case 4:  
	       vnp = SHoundLLIDFromGOIDAndECode ( 7517 , "IEA" );
	       
	       if ( TRUE == AssertListIsEmpty ( vnp ) )
	       	   ValNodeAddStr ( &failed, 0, "LLIDFromGOIDAndECode"  );

               /* printIntList ( vnp ); */
	       break;
      case 5:/* fall through */;
      case 6:  NodeListFree ( result );
	       ValNodeAddInt ( &result, 0, 2 ); 
	       vnp = SHoundLLIDFromOMIM ( 103950 );

	       if ( FALSE == AssertIntListEquals ( vnp, result ) )
		    ValNodeAddStr ( &failed, 0, "LLIDFromOMIM" );

               /* printIntList ( vnp ); */
	       break;
      case 7:  
	       vnp = SHoundLLIDFromCDDID ( "KOG1515" );

	       if ( TRUE == AssertListIsEmpty ( vnp ) )
		    ValNodeAddStr ( &failed, 0, "LLIDFromCDDID" );
               /* printIntList ( vnp ); */
	       break;
      case 8:  NodeListFree ( result );
	       ValNodeAddInt ( &result, 0, 4557225 );

	       ValNodeAddInt ( &vnp2, 0, 2 );
	       vnp = SHoundGiFromLLIDList ( vnp2 );

	       if ( FALSE == AssertIntListEquals ( vnp, result ) )
		     ValNodeAddStr ( &failed, 0, "GiFromLLIDList" );

               /* printIntList ( vnp ); */
	       break;
      case 9:  
	       vnp = SHoundGiFromGOIDAndECode ( 7517, "IEA" );

	       if ( TRUE == AssertListIsEmpty ( vnp ) )
		     ValNodeAddStr ( &failed, 0, "GiFromGOIDAndECode" );

               /* printIntList ( vnp ); */
	       break;
      case 10:  vnp = SHoundGiFromGOID ( 7517 );
	       
	       if ( TRUE == AssertListIsEmpty ( vnp ) )
		    ValNodeAddStr ( &failed, 0, "GiFromGOID" );
               /* printIntList ( vnp ); */
	       break;
      case 11: NodeListFree(vnp2);
	       ValNodeAddInt ( &vnp2, 0, 7517 );
	       vnp = SHoundGiFromGOIDList ( vnp2 );

               if ( TRUE == AssertListIsEmpty ( vnp ) )
		     ValNodeAddStr ( &failed, 0, "GiFromGOIDList" );
	       
               /* printIntList ( vnp ); */
	       break;
      case 12: NodeListFree(vnp2);
	       ValNodeAddInt ( &vnp2, 0, 7517 ); 
	       vnp = SHoundGiFromGOIDListAndECode ( vnp2, "IEA" );

	       if ( TRUE == AssertListIsEmpty ( vnp ) )
		     ValNodeAddStr ( &failed, 0, "GiFromGOIDListAndECode" );
	       
               /* printIntList ( vnp ); */
	       break;
      case 13: 
	       vnp = SHoundGOIDFromGi ( 4557225 );

	       if ( TRUE == AssertListIsEmpty ( vnp ) )
		     ValNodeAddStr ( &failed, 0, "GOIDFromGi" );

               /* printIntList ( vnp ); */
	       break;
      case 14: NodeListFree ( result );
	       ValNodeAddInt ( &result, 0, 4867 );
	       ValNodeAddInt ( &result, 0, 6886 );
	       ValNodeAddInt ( &result, 0, 8320 );
	       ValNodeAddInt ( &result, 0, 17114 );

	       ValNodeAddInt ( &vnp2, 0, 4557225 ); 
	       vnp = SHoundGOIDFromGiList ( vnp2 );

	       if ( TRUE != AssertIntListEquals ( vnp, result ) )
		     ValNodeAddStr ( &failed, 0, "GOIDFromGiList" );

               /* printIntList ( vnp ); */
	       break;
      case 15: NodeListFree ( result );
	       ValNodeAddStr ( &result, 0, "IEA" );

	       vnp = SHoundGOECodeFromGiAndGOID ( 494393, 7517 );
               
	       if ( TRUE != AssertStrListEquals ( vnp, result ) )
		     ValNodeAddStr ( &failed, 0, "GOECodeFromGiAndGOID" );
	       
	       /* printStrList ( vnp ); */
	       break;
      case 16: NodeListFree ( result );
               ValNodeAddInt ( &result, 0, 2340091 );
	       vnp = SHoundGOPMIDFromGiAndGOID ( 4557783, 4060 );

	       if ( TRUE != AssertIntListEquals ( vnp, result ) )
		     ValNodeAddStr ( &failed, 0, "GOPMIDFromGiAndGOID" );
	       
               /* printIntList ( vnp ); */
	       break;
      case 17: NodeListFree ( result );
	       ValNodeAddInt ( &result, 0, 4557783 );
	       vnp = SHoundGiFromOMIM ( 243400 );

	       if ( TRUE != AssertIntListEquals ( vnp, result ) )
		     ValNodeAddStr ( &failed, 0, "GiFromOMIM" );
	       
               /* printIntList ( vnp ); */
	       break;
      case 18: NodeListFree ( result );
               ValNodeAddInt ( &result, 0, 4501843 );
               ValNodeAddInt ( &result, 0, 4557783 );
	       
	       ValNodeAddInt ( &vnp2, 0, 243400 );
	       ValNodeAddInt ( &vnp2, 0, 107280 ); 
	       vnp = SHoundGiFromOMIMList ( vnp2 );

	       if ( TRUE != AssertIntListEquals ( vnp, result ) )
		      ValNodeAddStr ( &failed, 0, "GiFromOMIMList" );

               /* printIntList ( vnp ); */
	       break;
      case 19: NodeListFree ( result );
	       ValNodeAddInt ( &result, 0, 103950 );
	       vnp = SHoundOMIMFromGi ( 4557225 );
	       if ( TRUE != AssertIntListEquals( vnp, result ) )
		      ValNodeAddStr ( &failed, 0, "OMIMFromGi" );
               /* printIntList(vnp); */
	       break;
      case 20: NodeListFree ( result );
	       ValNodeAddInt ( &result, 0, 103950 );
	       
	       ValNodeAddInt ( &vnp2, 0, 4557225 );
	       vnp = SHoundOMIMFromGiList ( vnp2 );

	       if ( TRUE != AssertIntListEquals ( vnp, result ) )
		     ValNodeAddStr ( &failed, 0, "OMIMFromGiList" );

               /* printIntList ( vnp ); */
	       break;
      case 21: 
	       vnp = SHoundGiFromCDDID ( "pfam00079" );
	        
	       if ( TRUE == AssertListIsEmpty ( vnp ) )
		     ValNodeAddStr ( &failed, 0, "GiFromCDDID" );

               /* printIntList ( vnp ); */
	       break;
      case 22: ValNodeAddStr ( &vnp2, 0, "pfam00079" );
	       ValNodeAddStr ( &vnp2, 0, "KOG1515" ); 
	       vnp = SHoundGiFromCDDIDList ( vnp2 );

	       if ( TRUE == AssertListIsEmpty ( vnp ) )
		      ValNodeAddStr ( &failed, 0, "GiFromCDDIDList" );

               /* printIntList ( vnp ); */
	       break;
      case 23: 
	       vnp = SHoundCDDIDFromGi ( 4557225 );

	       if ( TRUE == AssertListIsEmpty ( vnp ) )
		     ValNodeAddStr ( &failed, 0, "CDDIDFromGi" );

               /* printStrList ( vnp ); */
	       break;
      case 24: NodeListFree ( result );
	       ValNodeAddStr ( &result, 0, "KOG0296" );
	       ValNodeAddStr ( &result, 0, "KOG4144" );

	       ValNodeAddInt ( &vnp2, 0, 494393 );
	       ValNodeAddInt ( &vnp2, 0, 4501845 ); 
	       vnp = SHoundCDDIDFromGiList ( vnp2 );

	       if ( TRUE == AssertListIsEmpty ( vnp ) )
		    ValNodeAddStr ( &failed, 0, "CDDIDFromGiList" );

               /* printStrList ( vnp ); */
	       break;
      case 25: 
	       if ( !AssertFloat ( 
		        SHoundCDDScoreFromGi (42660739, "COG3264")) 
	          )
	            ValNodeAddStr ( &failed, 0, "CDDScoreFromGi" ); 
		    
	       /* fprintf ( stdout, "%.8g\n", 
	       	  SHoundCDDScoreFromGi(gi, "COG3264") ); */
	       break;
      case 26:  
	       if ( 2 != SHoundLLIDFromGi ( 4557225 ) )
		   ValNodeAddStr ( &failed, 0, "LLIDFromGi" );
	       
	       /* fprintf (stdout, "%d\n", SHoundLLIDFromGi (gi) ); */
	       break;
      case 27: 
	       NodeListFree ( vnp2 );
	       ValNodeAddInt ( &vnp2, 0, 4557225 );
	       ValNodeAddInt ( &vnp2, 0, 494393 );
               vnp = SHoundLLIDFromGiList ( vnp2 ); 

	       if ( TRUE == AssertListIsEmpty ( vnp ) )
		    ValNodeAddStr ( &failed, 0, "LLIDFromGiList" );
	       /* printIntList ( vnp ); */
	       break;
      case 28:
	       if ( 0 != StringCmp ( "12p13.3-p12.3", 
				     SHoundLocusFromGi ( 4557225 ) )
		  )
		  ValNodeAddStr ( &failed, 0, "LocusFromGi" );
	       /* fprintf (stdout, "%s\n", SHoundLocusFromGi ( 494393 ) ); */
	       break;
      default:
               fprintf ( stderr, "Not a valid input value\n" );
	       break;
   }
   return failed;
}



int main ( int argc, char *argv[] )
{
   int i = 0;
   ValNodePtr vnp = NULL;
   ValNodePtr failed = NULL;

   LL_InitCodeBase();
   LL_OpenAllDBs(FALSE);

   for ( i = 0; i < 29; i++ ) 
   { 
      vnp = testLocusLinkSuite ( i );
      ValNodeLink ( &failed, vnp );
   } 

   fprintf ( stdout, "These functions failed the locuslink test: " );
   printStrList ( failed );
   return 0;
}
