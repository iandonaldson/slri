#include <seqhound.h>

Int2 Main()
{
  
   Int4 gi = 0, i=0;
   ValNodePtr pvngi = NULL, giList=NULL;
   
   FILE* pfile = NULL;
   time_t tim1,tim2;
   
   FLinkSetPtr pNeighbours=NULL;
   FLinkSetListPtr pLNeighbours=NULL;
   FLinkSetList2Ptr pLNeighbours2=NULL, pLtN2=NULL;
   FloatHi evalue=0;
   Boolean bLimit=0;
   ILinkSetPtr p3Neighbours=NULL;

   ILinkSetListPtr pILNeighbours=NULL;


    tim1=GetSecs();
  
    if(SHoundInit(FALSE, "sh_nbhrs"))
      printf("SHoundInit OK.\n");
    else
      printf("SHoundInit failed.\n");


    if(SHoundIsInited())
      printf("SHoundIsInited OK.\n");
    else
    {
      printf("SHoundIsInited failed.\n");
      return 1;
    }

/*
	gi=135196;
	evalue=0.66667;
	bLimit=0;
	printf("I chose gi=%ld evalue=%f blimit=%d\n", gi, evalue, bLimit);
	if (!SHoundNeighboursFromGiEx(gi, &pNeighbours, evalue, bLimit)) {
		while(pNeighbours!=NULL) {
			printf("SHoundNeighboursFromGiEx successful\n");
			printf("Total Neighbours: %ld\n", (long) pNeighbours->num);
			printf("List of Gi's    : "); for (i=0; i<pNeighbours->num; i++) printf("%ld ", pNeighbours->uids[i]);
			printf("\nList of weights : "); for (i=0; i<pNeighbours->num; i++) printf("%ld ", pNeighbours->weights[i]);
			printf("\n");
			pNeighbours=pNeighbours->next;
		}
	}
	else printf("SHoundNeighboursFromGiEx failed\n");


	gi=494693;
	evalue=0.66667;
	bLimit=1;
	printf("I chose gi=%ld evalue=%f blimit=%d\n", gi, evalue, bLimit);
	if (!SHoundNeighboursFromGi(gi, &pNeighbours, evalue, bLimit)) {
		while(pNeighbours!=NULL) {
			printf("SHoundNeighboursFromGi successful\n");
			printf("Total Neighbours: %ld\n", (long) pNeighbours->num);
			printf("List of Gi's    : "); for (i=0; i<pNeighbours->num; i++) printf("%ld ", pNeighbours->uids[i]);
			printf("\nList of weights : "); for (i=0; i<pNeighbours->num; i++) printf("%e ", pNeighbours->weights[i]);
			printf("\n");
			pNeighbours=pNeighbours->next;
		}
	}
	else printf("SHoundNeighboursFromGi failed\n");

	evalue=0.66667;
	bLimit=1;
*/
	pvngi = ValNodeAddInt(NULL,1,494692);
	ValNodeAddInt(&pvngi,1,135196);
/*	
	if (!SHoundNeighboursFromGiList(pvngi, &pLNeighbours, evalue)) {
		printf("SHoundNeighboursFromGiList successful\n");
		while(pLNeighbours!=NULL) {
			printf("I chose gi=%ld evalue=%f \n", (long) pLNeighbours->id, evalue);
			if ( pLNeighbours->set1 != NULL )
			{
			    printf("Total Neighbours: %ld\n", (long) pLNeighbours->set1->num);
			    printf("List of Gi's    : "); for (i=0; i<pLNeighbours->set1->num; i++) printf("%ld ", pLNeighbours->set1->uids[i]);
			    printf("\nList of weights : "); for (i=0; i<pLNeighbours->set1->num; i++) printf("%g ", pLNeighbours->set1->weights[i]);
			    printf("\n");
			}
			else
			{
			    printf("Total Neighbours: 0\n");
			}
			
			pLNeighbours=pLNeighbours->next;
		}
	}
	else printf("SHoundNeighboursFromGiList failed\n");

	gi=494693;
	evalue=0.66667;
	if (!SHoundNeighboursOfNeighbours(gi, &pLNeighbours, evalue)) {
		while(pLNeighbours!=NULL) {
			printf("I chose gi=%ld evalue=%f \n", (long) pLNeighbours->id, evalue);
			if ( pNeighbours->set1 != NULL )
			{
			    printf("Total Neighbours: %ld\n", (long) pLNeighbours->set1->num);
			    printf("List of Gi's    : "); for (i=0; i<pLNeighbours->set1->num; i++) printf("%ld ", pLNeighbours->set1->uids[i]);
			    printf("\nList of weights : "); for (i=0; i<pLNeighbours->set1->num; i++) printf("%g ", pLNeighbours->set1->weights[i]);
			    printf("\n");
			}
			else
			{
			    printf("Total Neighbours: 0\n");
			}

			pLNeighbours=pLNeighbours->next;
		}
	}
	else printf("SHoundNeighboursOfNeighbours failed\n");

	evalue=0.66667;
	if (!SHoundNeighboursOfNeighboursList(pvngi, &pLNeighbours2, evalue)) {
		pLtN2=pLNeighbours2;
		printf("I chose evalue=%f gi= \n",evalue); while(pvngi!=NULL) {printf("%ld ", (long) pvngi->data.intvalue); pvngi=pvngi->next;}
		printf("\nThe Neighbours of these gi's are: ");
		while(pLNeighbours2!=NULL) {
			printf("%ld \n", (long) pLNeighbours2->id);
			pLNeighbours2=pLNeighbours2->next;
		}

		pLNeighbours2=pLtN2;
		while(pLNeighbours2!=NULL) {
			
			pLNeighbours=pLNeighbours2->set2;
			while(pLNeighbours!=NULL) {
				printf("\nThe Neighbours of neighbour %ld are: ", pLNeighbours2->id);
				if ( pLNeighbours->set1 != NULL )
				{
				    printf("Total Neighbours: %ld\n", (long) pLNeighbours->set1->num);
				    printf("List of Gi's    : "); for (i=0; i<pLNeighbours->set1->num; i++) printf("%ld ", pLNeighbours->set1->uids[i]);
				    printf("\nList of weights : "); for (i=0; i<pLNeighbours->set1->num; i++) printf("%g ", pLNeighbours->set1->weights[i]);
				    printf("\n");
				}
				else
				{
				    printf("Total Neighbours: 0\n");
				}

				pLNeighbours=pLNeighbours->next;
			}
			pLNeighbours2=pLNeighbours2->next;
		}
	}
	else printf("SHoundNeighboursOfNeighboursList failed\n");


	evalue=0.000001;
	bLimit=0;
	
	if (!SHoundNeighboursFromTaxID(2190, &pLNeighbours, evalue)) {
		printf("SHoundNeighboursFromTaxID successful\n");
		while(pLNeighbours!=NULL) {
			printf("I chose gi=%ld evalue=%f \n", (long) pLNeighbours->id, evalue);
			if ( pLNeighbours->set1 != NULL )
			{
			    printf("Total Neighbours: %ld\n", (long) pLNeighbours->set1->num);
			    printf("List of Gi's    : "); for (i=0; i<pLNeighbours->set1->num; i++) printf("%ld ", pLNeighbours->set1->uids[i]);
			    printf("\nList of weights : "); for (i=0; i<pLNeighbours->set1->num; i++) printf("%g ", pLNeighbours->set1->weights[i]);
			    printf("\n");
			}
			else
			{
			    printf("Total Neighbours: 0\n");
			}
			pLNeighbours=pLNeighbours->next;
		}
	}
	else printf("SHoundNeighboursFromTaxID failed\n");



	evalue=0.66667;
	if (!SHoundNeighboursOfNeighboursFromTaxID(2190, &pLNeighbours2, evalue)) {
		pLtN2=pLNeighbours2;
		printf("I chose evalue=%f gi= \n",evalue); while(pvngi!=NULL) {printf("%ld ", (long) pvngi->data.intvalue); pvngi=pvngi->next;}
		printf("\nThe Neighbours of these gi's are: ");
		while(pLNeighbours2!=NULL) {
			printf("%ld \n", (long) pLNeighbours2->id);
			pLNeighbours2=pLNeighbours2->next;
		}

		pLNeighbours2=pLtN2;
		while(pLNeighbours2!=NULL) {
			
			pLNeighbours=pLNeighbours2->set2;
			while(pLNeighbours!=NULL) {
				printf("\nThe Neighbours of neighbour %ld are: ", pLNeighbours2->id);
				if ( pLNeighbours->set1 != NULL )
				{
				    printf("Total Neighbours: %ld\n", (long) pLNeighbours->set1->num);
				    printf("List of Gi's    : "); for (i=0; i<pLNeighbours->set1->num; i++) printf("%ld ", pLNeighbours->set1->uids[i]);
				    printf("\nList of weights : "); for (i=0; i<pLNeighbours->set1->num; i++) printf("%g ", pLNeighbours->set1->weights[i]);
				    printf("\n");
				}
				else
				{
				    printf("Total Neighbours: 0\n");
				}
				pLNeighbours=pLNeighbours->next;
			}
			pLNeighbours2=pLNeighbours2->next;
		}
	}
	else printf("SHoundNeighboursOfNeighboursFromTaxID failed\n");
*/
	/*SHound3DNeighboursFromGiEx(230940,&pNeighbours,0.0001);*/
/*
	gi=135196;
	evalue=0.66667;
	bLimit=0;
	printf("I chose gi=%ld evalue=%f blimit=%d\n", gi, evalue, bLimit);

	if (!SHound3DNeighboursFromGiEx(gi, &p3Neighbours, evalue)) {
		while(p3Neighbours!=NULL) {
			printf("SHound3DNeighboursFromGiEx successful\n");
			printf("Total Neighbours: %ld\n", (long) p3Neighbours->num);
			printf("List of Gi's    : "); for (i=0; i<p3Neighbours->num; i++) printf("%ld ", p3Neighbours->uids[i]);
			printf("\nList of weights : "); for (i=0; i<p3Neighbours->num; i++) printf("%ld ", p3Neighbours->weights[i]);
			printf("\n");
			p3Neighbours=p3Neighbours->next;
		}
	}
	else printf("SHound3DNeighboursFromGiEx failed\n");

	gi=135196;
	evalue=0.66667;
	bLimit=0;
	printf("I chose gi=%ld evalue=%f blimit=%d\n", gi, evalue, bLimit);
	if (!SHound3DNeighboursFromGi(gi, &p3Neighbours, evalue)) {
		while(p3Neighbours!=NULL) {
			printf("SHound3DNeighboursFromGiEx successful\n");
			printf("Total Neighbours: %ld\n", (long) p3Neighbours->num);
			printf("List of Gi's    : "); for (i=0; i<p3Neighbours->num; i++) printf("%ld ", p3Neighbours->uids[i]);
			printf("\nList of weights : "); for (i=0; i<p3Neighbours->num; i++) printf("%ld ", p3Neighbours->weights[i]);
			printf("\n");
			p3Neighbours=p3Neighbours->next;
		}

	}
	else printf("SHound3DNeighboursFromGi failed\n");


	evalue=0.1;

	if(!	SHound3DNeighboursFromTaxID( 2190,&pILNeighbours,evalue) ){
		printf("SHound3DNeighboursFromTaxID successful\n");
		while(pILNeighbours!=NULL) {
			printf("I chose gi=%ld evalue=%f \n", (long) pILNeighbours->id, evalue);
			if ( pILNeighbours->set1 != NULL )
			{
			    printf("Total Neighbours: %ld\n", (long) pILNeighbours->set1->num);
			}
			else
			{
			    printf("Total Neighbours: 0\n");
			}
		}
	}
	else printf("SHound3DNeighboursFromTaxID failed\n");

	evalue=0.66667;

	if(!SHound3DNeighboursFromGiList(pvngi,&pILNeighbours,evalue)){
			printf("SHound3DNeighboursFromTaxID successful\n");
		while(pILNeighbours!=NULL) {
			printf("I chose gi=%ld evalue=%f \n", (long) pILNeighbours->id, evalue);
			if ( pILNeighbours->set1 != NULL )
			{
			    printf("Total Neighbours: %ld\n", (long) pILNeighbours->set1->num);
			    printf("List of Gi's    : "); for (i=0; i<pILNeighbours->set1->num; i++) printf("%ld ", pILNeighbours->set1->uids[i]);
			    printf("\nList of weights : "); for (i=0; i<pILNeighbours->set1->num; i++) printf("%g ", pILNeighbours->set1->weights[i]);
			    printf("\n");
			}
			else
			{
			    printf("Total Neighbours: 0\n");
			}
			pILNeighbours=pILNeighbours->next;
		}
	}
	else printf("SHound3DNeighboursFromGiList failed\n");

	if(!SHound3DListFromTaxID(4932,&p3Neighbours)){
				while(p3Neighbours!=NULL) {
			printf("SHound3DNeighboursFromGi successful\n");
			printf("Total Neighbours: %ld\n", (long) p3Neighbours->num);
			printf("List of Gi's    : "); for (i=0; i<p3Neighbours->num; i++) printf("%ld ", p3Neighbours->uids[i]);
			printf("\nList of weights : "); for (i=0; i<p3Neighbours->num; i++) printf("%ld ", p3Neighbours->weights[i]);
			printf("\n");
			p3Neighbours=p3Neighbours->next;
		}
	}
	else printf("SHound3DListFromTaxID failed\n");
*/

	if(!SHoundNumNeighboursInDB(&i)) printf("SHoundGiAndNumNeighboursList is %ld", i);

	if(!SHoundGiAndNumNeighboursList(&p3Neighbours)) {
		while(p3Neighbours!=NULL) {
			printf("SHoundGiAndNumNeighboursList successful\n");
			printf("Total Neighbours: %ld\n", (long) p3Neighbours->num);
			printf("List of Gi's    : "); for (i=0; i<p3Neighbours->num; i++) printf("%ld ", p3Neighbours->uids[i]);
			printf("\nList of weights : "); for (i=0; i<p3Neighbours->num; i++) printf("%ld ", p3Neighbours->weights[i]);
			printf("\n");
			p3Neighbours=p3Neighbours->next;
		}
	}
	else printf("SHoundGiAndNumNeighboursList failed\n");

    SHoundFini();
    printf("SHoundFini OK.\n");


    ValNodeFree(pvngi);
  
    ValNodeFree(giList);
   
    tim2=GetSecs();
    printf("Total elapsed time: %ld sec. ",(tim2-tim1));
    printf("Done.\n");
    return 0;
}
