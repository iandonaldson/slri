/*
$Id: polymerfix.c,v 1.6 2005/03/22 17:30:34 vgu Exp $
***************************************************************************
  Copyright (C) 2004 Mount Sinai Hospital (MSH)

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


  PROGRAM:


  AUTHORS:
            Howard J. Feldman           (feldman@mshri.on.ca)
            Kevin A. Snyder             (ksnyder@blueprint.org)
            John J. Salama              (jjsalama@blueprint.org)
            and Christopher W.V. Hogue  (hogue@mshri.on.ca)


  REVISION/CONTRIBUTION NOTES:

  November 4 2004  Original MSH Revision.
                   Hogue Lab - University of Toronto Biochemistry
                   Department and the Samuel Lunenfeld Research
                   Institute, Mount Sinai Hospital
                   http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                   ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:
*************************************************************************** */

#include <mmdbapi.h>
#include <utilpub.h>

#define BL_MIN_ALLOWED 1.0	/* ignore IMB bonds shorter than this length */
/*#define BL_SACC_MAX 1.70*/	/* maximum length of covalent bond for saccharides */

typedef struct Mol_Pair
{
	Int4 mol1;
	Int4 mol2;
} MolPair, *MolPairPtr;

static CharPtr *pdbresname;
static Int4Ptr pdbresnum;
static ValNodePtr *mollist,vnpResInMol;

/* determines if given molecule is a sugar or not */
static Boolean IsSugar(PFB pfb)
{
	PMMD pmmd;
	PDNMG pdnmg;
	PMGD pmgd;
	Char buf[PATH_MAX];
	static CharPtr sugars="147|149|16G|1AR|1GL|1GN|1NA|2DG|2FG|2FL|2FP|2GL|3FM|3MF|5RP|6PG|A2G|AAB|AAL|ABC|ABD|ABE|ABF|ACG|ACI|ACR|ACX|ADG|ADR|AFL|AFP|AGC|AGL|AHR|AIG|ALL|AOG|ARA|ARB|ARI|ASG|ASO|AXP|B2G|B4G|BCD|BDG|BDR|BFP|BGC|BGL|BGP|BHG|BMA|BNG|BOG|BRI|C3X|C4X|C5X|CAP|CBI|CBS|CDR|CEG|CHO|CR1|CR6|CRA|CTO|CTR|CTT|D6G|DAF|DAG|DDA|DDB|DDL|DEL|DFR|DFX|DGD|DGS|DIG|DLF|DLG|DMU|DOM|DP5|DRI|DSR|E4P|EBG|EJT|EPG|ERI|F6P|FBP|FCA|FCB|FCT|FDP|FDQ|FFC|FRU|FUC|FXP|G16|G1P|G2F|G4D|G4S|G6D|G6P|G6S|GAC|GAD|GAL|GC4|GCD|GCN|GCO|GCS|GCT|GCU|GCV|GCW|GE1|GFP|GL2|GL5|GL7|GL9|GLA|GLB|GLC|GLD|GLF|GLG|GLO|GLP|GLS|GLT|GLW|GMH|GP1|GP4|GPM|GS1|GS4|GSA|GSD|GTE|GTH|GTR|GTZ|GU0|GU1|GU2|GU3|GU4|GU5|GU6|GU8|GU9|GUP|GYP|H2P|IDG|IDR|IDS|IDT|IDU|IN1|IPT|KDA|KDO|LAG|LAK|LAT|LGC|LGU|LM2|LMT|LMU|LOX|M6P|MA1|MA2|MA3|MAB|MAG|MAL|MAN|MAT|MAV|MAW|MBG|MDA|MDM|MFA|MFB|MFU|MGA|MGL|MMA|MRP|MTT|MUG|MXY|NAA|NAG|NBG|NED|NG6|NGA|NGL|NGS|NTP|OPG|OPM|ORP|P3M|PA5|PNA|PNG|PRP|R1P|RAA|RAE|RAM|RAO|RG1|RHA|RIB|RIP|RNS|RNT|ROB|RPA|RUB|SCR|SGA|SGC|SGN|SSG|SUC|T6P|TCB|TDG|TIA|TM5|TM6|TMR|TOA|TOC|TRE|TYV|VG1|X2F|XBP|XDN|XDP|XIF|XIM|XLS|XUL|XYP|XYS|";

	if (pfb->bMe==AM_MGD) {
		pmgd=(PMGD)pfb;
		sprintf(buf,"|%s|",pmgd->pcGraphName);
		if (StringStr(sugars,buf))
			return TRUE;
		return FALSE;
	}
	if (pfb->bMe!=AM_MMD)
		return FALSE;
	pmmd=(PMMD)pfb;
	pdnmg=pmmd->pdnmgHead;
	while (pdnmg) {
		pmgd=(PMGD)(pdnmg->data.ptrvalue);
		sprintf(buf,"|%s|",pmgd->pcGraphName);
		if (StringStr(sugars,buf))
			return TRUE;
		pdnmg=pdnmg->next;
	}
	return FALSE;
}

/* finds first occurrence of integer i in ValNode list of integers vnp */
static ValNodePtr ValNodeFindInt(ValNodePtr vnp,Int4 i)
{
	while (vnp) {
		if (vnp->data.intvalue==i)
			return vnp;
		vnp=vnp->next;
	}
	return NULL;
}

/* finds first occurrence of integer i in ValNode list of integers vnp and deletes it */
static void ValNodeKill(ValNodePtr *vnp,Int4 i)
{
	ValNodePtr vnpHere,vnpLast=NULL;

	vnpHere=*vnp;
	while (vnpHere) {
		if (vnpHere->data.intvalue==i) {
			if (vnpLast==NULL)
				*vnp=vnpHere->next;
			else
				vnpLast->next=vnpHere->next;
			MemFree(vnpHere);
			return;
		}
		vnpLast=vnpHere;
		vnpHere=vnpHere->next;
	}
	return;
}

/* return branch points of 'marked' residues */
static ValNodePtr GetBranchPoints(ValNodePtr vnpFull, ValNodePtr *list)
{
	ValNodePtr vnpHere,vnpHead=NULL,vnp;

	vnp=vnpFull;
	while (vnp) {
		/* get what is attached to this residue if it is 'marked' */
		if (vnp->choice) {
			vnpHere=list[vnp->data.intvalue];
			while (vnpHere) {
				/* check if residue is marked or not */
				if (ValNodeFindInt(vnpFull,vnpHere->data.intvalue)->choice==0) {
					/* add to list if not already there */
					if (ValNodeFindInt(vnpHead,vnpHere->data.intvalue)==NULL) {
						ValNodeAddInt(&vnpHead,0,vnpHere->data.intvalue);
					}
				}
				vnpHere=vnpHere->next;
			}
		}
		vnp=vnp->next;
	}
	/* now delete the Head list from the original list and append head to the end */
	vnp=vnpHead;
	while (vnp) {
		ValNodeKill(&vnpFull,vnp->data.intvalue);
		vnp=vnp->next;
	}
	vnp=vnpFull;
	while (vnp->next)
		vnp=vnp->next;
	vnp->next=vnpHead;
	return vnpHead;
}

/* removes edge between mol1 and mol2 from the graph represented by list; does
   nothing if the edge does not exist */
static void RemoveNode(ValNodePtr *list,Int4 mol1,Int4 mol2)
{
	ValNodePtr vnpHere,vnpLast;

	vnpLast=NULL;
	vnpHere=list[mol1];
	while (vnpHere) {
		if (vnpHere->data.intvalue==mol2)
			break;
		vnpLast=vnpHere;
		vnpHere=vnpHere->next;
	}
	if (vnpHere) {
		if (vnpLast==NULL)
			list[mol1]=vnpHere->next;
		else {
			vnpLast->next=vnpHere->next;
		}
		MemFree(vnpHere);
	}
	vnpLast=NULL;
	vnpHere=list[mol2];
	while (vnpHere) {
		if (vnpHere->data.intvalue==mol1)
			break;
		vnpLast=vnpHere;
		vnpHere=vnpHere->next;
	}
	if (vnpHere) {
		if (vnpLast==NULL)
			list[mol2]=vnpHere->next;
		else {
			vnpLast->next=vnpHere->next;
		}
		MemFree(vnpHere);
	}
}

/* determine the next available chain letter to use for newly created molecules */
CharPtr NextFreeChain(PMSD pmsd)
{
	Boolean chains[62];
	Int2 cnt;
	Char chain='\0';
	PDNMM pdnmm;
	PMMD pmmd;
	Char res[2];

	for (cnt=0;cnt<62;cnt++)
		chains[cnt]=FALSE;
	pdnmm=pmsd->pdnmmHead;
	while (pdnmm) {
		pmmd=(PMMD)(pdnmm->data.ptrvalue);
		if (pmmd->pcMolName)
			chain=(pmmd->pcMolName)[0];
		if (chain>='A' && chain <='Z')
			chains[chain-'A']=TRUE;
		else if (chain>='0' && chain <='9')
			chains[chain-'0'+26]=TRUE;
		else if (chain>='a' && chain <='z')
			chains[chain-'a'+26+10]=TRUE;
		pdnmm=pdnmm->next;
	}
	cnt=0;
	while (cnt<62) {
		if (chains[cnt]==FALSE)
			break;
		cnt++;
	}
	if (cnt<26)
		chain=(Char)('A'+cnt);
	else if (cnt<36)
		chain=(Char)('0'+cnt-26);
	else if (cnt<62)
		chain=(Char)('a'+cnt-36);
	else
		return NULL;
	res[0]=chain;
	res[1]='\0';
	return StringSave(res);
}

/* sort by residue number, and residue name where number is identical */
int LIBCALLBACK ResCmpCallback(Nlm_VoidPtr vp1, Nlm_VoidPtr vp2)
{
	ValNodePtr vnp1,vnp2;
	Int4 mol1,mol2;

	vnp1=*(ValNodePtr *)vp1;
	vnp2=*(ValNodePtr *)vp2;
	mol1=vnp1->data.intvalue;
	mol2=vnp2->data.intvalue;
	if (pdbresnum[mol1]!=pdbresnum[mol2])
		return pdbresnum[mol1]-pdbresnum[mol2];
	return StringCmp(pdbresname[mol1],pdbresname[mol2]);
}

/* sort in reverse order by choice field (which holds position along path found */
int LIBCALLBACK ResCmpCallback2(Nlm_VoidPtr vp1, Nlm_VoidPtr vp2)
{
	ValNodePtr vnp1,vnp2;

	vnp1=*(ValNodePtr *)vp1;
	vnp2=*(ValNodePtr *)vp2;
	return vnp2->choice-vnp1->choice;
}

/* this is a recursive function that searches for a path through the molecules in the list
   vnpResInMol along the grpah represented by mollist.  mollen is the length remaining that
   needs to be traced to visit each node exactly once.  molnum is the current position in the
   graph.  Return value is zero upon success, 1 upon reaching a dead end.  The actual path
   is stored in the choice field of vnpResInMol, allowing easy extracting of the path once
   complete */
static Int2 FindLongestPath(Int4 molnum,Int4 mollen)
{
	ValNodePtr vnp,vnpNext;
	Int4 pathlen;

	if (mollen==0)
		return 0;
	/* see what's attahed to current molecule */
	vnp=mollist[molnum];
	while (vnp) {
		vnpNext=ValNodeFindInt(vnpResInMol,vnp->data.intvalue);
		/* should never be NULL... */
		if (vnpNext->choice==0) {
			/* not traversed yet */
			vnpNext->choice=mollen;
			pathlen=FindLongestPath(vnpNext->data.intvalue,mollen-1);
			if (pathlen==0)
				return 0;
			vnpNext->choice=0;
		}
		vnp=vnp->next;
	}
	return 1;
}

static FloatLo GetDistSqd(PALD pald1,PALD pald2)
{
	FloatLo d,dsqd;

	if (pald1->iFloatNo>3 && pald2->iFloatNo>3) {
		/* if occupancy of either atom <= 0.5, assume bond is garbage */
		if (pald1->pflvData[3]<=0.5 || pald2->pflvData[3]<=0.5)
			return 0.0;
	}
	dsqd=0.0;
	d=AtomLocX(pald1)-AtomLocX(pald2);
	dsqd=dsqd+d*d;
	d=AtomLocY(pald1)-AtomLocY(pald2);
	dsqd=dsqd+d*d;
	d=AtomLocZ(pald1)-AtomLocZ(pald2);
	dsqd=dsqd+d*d;
	return dsqd;
}

Boolean HasAnAtom(PMAD pmad, CharPtr nam)
{
	PMGD pmgd;
	PVNMA pvnma;
	PMAD pmadHere;
	CharPtr pc;

	pc=TrimSpacesAroundString(nam);
	pmgd=(PMGD)(pmad->pfbParent);
	pvnma=pmgd->pvnmaAHead;
	while (pvnma) {
		pmadHere=(PMAD)(pvnma->data.ptrvalue);
		if (StringStr(pmadHere->pcAName,pc)) {
			if (GetAtomLocs(pmadHere,1))
				return TRUE;
			else
				return FALSE;
		}
		pvnma=pvnma->next;
	}
	return FALSE;
}

PMSD FixBiostrucPolymers (PMSD pmsd,Int4 quiet)
{
	PDNMM pdnmm,pdnmmNew,pdnmmTmp;
	PMMD pmmd1,pmmd2,*pmmdarr,pmmdNew,pmmd;
	PDNMG pdnmg,pdnmgLast;
	PMGD pmgd,pmgd1,pmgd2;
	PVNMB pvnmbIMB,pvnmbLast,pvnmbNext,pvnmbHere;
	PMBD pmbd;
	PMAD pmad1,pmad2;
	PALD pald1,pald2;
	Int4 dim,cnt,molnum,curmol,nextmol,prevresnum,mollen,firstmol,fullen;
	Int4Ptr molnumtoresnum;
        Int2 mol1,mol2,resnum,el1,el2;
	ValNodePtr vnp,vnpHere,vnpLast,vnpNext,vnpBadPairs=NULL,vnp2,vnpResInBranch,vnpCyclic=NULL;
	Char buf[PATH_MAX];
	Char sequence[PATH_MAX];
	Boolean inmol,badmol,nohet,foundsmallest,killbond;
	CharPtr *molnames,curmolname,prevresname,pc;
	FloatLo d;
	long lnum=0;
	int inum=0,inum2=0;
	ValNodePtr newmols=NULL,newmol,vnpMol;
	MolPairPtr pair,mpp;

	if (!pmsd)
		return NULL;
	dim=pmsd->iMolCount;
	pvnmbIMB=pmsd->pvnmbIMBHead;
	if (!pvnmbIMB)
		return pmsd;
	mollist=(ValNodePtr *)MemNew((dim+1)*sizeof(ValNodePtr));
	molnames=(CharPtr *)MemNew((dim+1)*sizeof(CharPtr));
	pmmdarr=(PMMD *)MemNew((dim+1)*sizeof(PMMD));
	pdbresnum=(Int4Ptr)MemNew((dim+1)*sizeof(Int4));
	pdbresname=(CharPtr *)MemNew((dim+1)*sizeof(CharPtr));
	pdnmm=pmsd->pdnmmHead;
	while (pdnmm) {
		molnames[pdnmm->choice]=((PMMD)(pdnmm->data.ptrvalue))->pcMolName;
		pmmdarr[pdnmm->choice]=(PMMD)(pdnmm->data.ptrvalue);
		if (sscanf(((PMGD)(((PMMD)(pdnmm->data.ptrvalue))->pdnmgHead->data.ptrvalue))->pcGraphNum,"%ld",&lnum))
			pdbresnum[pdnmm->choice]=(Int4)lnum;
		pdbresname[pdnmm->choice]=((PMGD)(((PMMD)(pdnmm->data.ptrvalue))->pdnmgHead->data.ptrvalue))->pcGraphNum;
		pdnmm=pdnmm->next;
	}
	pvnmbLast=NULL;
	while (pvnmbIMB) {
		pvnmbNext=pvnmbIMB->next;
		pmbd=(PMBD)(pvnmbIMB->data.ptrvalue);
		pmad1=pmbd->pmadFrom;
		pmad2=pmbd->pmadTo;
                el1=pmad1->pvnmaLink->choice;
                el2=pmad2->pvnmaLink->choice;
		pmmd1=(PMMD)(pmad1->pfbParent->pfbParent);
		pmmd2=(PMMD)(pmad2->pfbParent->pfbParent);
		pald1=GetAtomLocs(pmad1,1);
		pald2=GetAtomLocs(pmad2,1);
		if (pald1 && pald2) {
			/* check for unrealistically short bond lengths */
			d=GetDistSqd(pald1,pald2);
			if (d<BL_MIN_ALLOWED*BL_MIN_ALLOWED) {
				/* bad molecule pair, remove from mollist when done */
				pair=(MolPairPtr)MemNew(sizeof(MolPair));
				pair->mol1=pmmd1->pdnmmLink->choice;
				pair->mol2=pmmd2->pdnmmLink->choice;
				ValNodeAddPointer(&vnpBadPairs,0,(VoidPtr)pair);
				pvnmbLast=pvnmbIMB;
				pvnmbIMB=pvnmbNext;
				continue;
			}
                }
                /* check for hydrogen */
                if ((el1==1) || (el2==1)) {
			/* remove bond */
			if (pvnmbLast==NULL)
				pmsd->pvnmbIMBHead=pvnmbNext;
			else
				pvnmbLast->next=pvnmbNext;
			/* free pvnmbHere */
			MemFree(pvnmbIMB->data.ptrvalue);
			MemFree(pvnmbIMB);

/*                      pair=(MolPairPtr)MemNew(sizeof(MolPair));
                        pair->mol1=pmmd1->pdnmmLink->choice;
                        pair->mol2=pmmd2->pdnmmLink->choice;
                        ValNodeAddPointer(&vnpBadPairs,0,(VoidPtr)pair);
                        pvnmbLast=pvnmbIMB;*/

                        pvnmbIMB=pvnmbNext;
                        continue;
                }
		if (IsSugar((PFB)pmmd1) && IsSugar((PFB)pmmd2)) {
			killbond=FALSE;
			/* check if bond is 'real' or not - no O-O, C-C, etc allowed in sugars */
			if (pmad1->pvnmaLink->choice==pmad2->pvnmaLink->choice) {
				killbond=TRUE;
			}
			/* no H-bonds of course */
			if (pmad1->pvnmaLink->choice==1 || pmad2->pvnmaLink->choice==1) {
				killbond=TRUE;
			}
			/* check if bond makes C have two O's attached, would be bad */
			if ((pmad1->pvnmaLink->choice==6) && (pmad2->pvnmaLink->choice==8)) {
				StringCpy(buf,pmad1->pcAName);
				pc=StringChr(buf,'C');
				if (pc) {
					*pc='O';
					if (HasAnAtom(pmad1,buf))
						killbond=TRUE;
				}
			}
			if ((pmad1->pvnmaLink->choice==8) && (pmad2->pvnmaLink->choice==6)) {
				StringCpy(buf,pmad2->pcAName);
				pc=StringChr(buf,'C');
				if (pc) {
					*pc='O';
					if (HasAnAtom(pmad2,buf))
						killbond=TRUE;
				}
			}
			/* check distance for C-O etc */
/*			if ((pmad1->pvnmaLink->choice==6) || (pmad2->pvnmaLink->choice==6)) {
				if (GetDistSqd(pald1,pald2)>BL_SACC_MAX*BL_SACC_MAX)
					killbond=TRUE;
			}*/
			if (killbond) {
				/* remove bond */
				if (pvnmbLast==NULL)
					pmsd->pvnmbIMBHead=pvnmbNext;
				else
					pvnmbLast->next=pvnmbNext;
				/* free pvnmbHere */
				MemFree(pvnmbIMB->data.ptrvalue);
				MemFree(pvnmbIMB);
				pvnmbIMB=pvnmbNext;
				continue;
			}
		}
		if (pmmd1==pmmd2) {
			ErrPostEx(SEV_FATAL,0,0,"Modelstruc integrity error, cannot continue");
		}
		if (IsHeterogen((PFB)pmmd1) && IsHeterogen((PFB)pmmd2)) {
			/* found pair of bonded heterogen molecules */
			mol1=pmmd1->pdnmmLink->choice;
			mol2=pmmd2->pdnmmLink->choice;
			/* keep list of who is bonded to who */
			if (ValNodeFindInt(mollist[mol1],mol2)==NULL)
				ValNodeAddInt(&mollist[mol1],0,mol2);
			if (ValNodeFindInt(mollist[mol2],mol1)==NULL)
				ValNodeAddInt(&mollist[mol2],0,mol1);
		}
		pvnmbLast=pvnmbIMB;
		pvnmbIMB=pvnmbNext;
	}
	/* remove bad pairs */
	vnp=vnpBadPairs;
	while (vnp) {
		pair=(MolPairPtr)(vnp->data.ptrvalue);
		RemoveNode(mollist,pair->mol1,pair->mol2);
		vnp=vnp->next;
	}
	if (vnpBadPairs)
		vnpBadPairs=ValNodeFree(vnpBadPairs);
	/* sort mollists */
	for (cnt=0;cnt<=dim;cnt++) {
		if (mollist[cnt])
			VnpHeapSort(&mollist[cnt],ResCmpCallback);
	}
	cnt=0;
	inmol=FALSE;
	/* mollist contains list of all molecules covalently bound to given mol */
	while (cnt<=dim) {
		/* derive supposed polymer sequence */
		if (mollist[cnt]) {
			/* find a seed molecule and generate list of all nodes in graph */
			vnpResInMol=NULL;
			ValNodeAddInt(&vnpResInMol,0,cnt);
			vnp2=vnpResInMol;
			while (vnp2) {
				vnp=mollist[vnp2->data.intvalue];
				while (vnp) {
					if (ValNodeFindInt(vnpResInMol,vnp->data.intvalue)==NULL)
						ValNodeAddInt(&vnpResInMol,0,vnp->data.intvalue);
					vnp=vnp->next;
				}
				vnp2=vnp2->next;
			}
			/* now vnpResInMol contains a non-redundant list of 'residues' for our new molecule */
			/* sort them by PDB numbering */
			VnpHeapSort(&vnpResInMol,ResCmpCallback);
			vnp=vnpResInMol;
			firstmol=1;
			mollen=ValNodeLen(vnpResInMol);
			fullen=mollen;
			while (vnp) {
				/* find a path through starting with vnp */
				vnp->choice=mollen;
				if (FindLongestPath(vnp->data.intvalue,mollen-1)==0)
					break;
				vnp->choice=0;
				vnp=vnp->next;
			}
			if (vnp==NULL) {
				/* no path through molecule */
				/* Branched molecule detected */
				mollen--;
				vnpResInBranch=vnpResInMol;
				while (fullen) {
					while (mollen>0) {
						vnp=vnpResInBranch;
						while (vnp) {
							/* find a path through starting with any unused vnp */
							vnp->choice=mollen;
							if (FindLongestPath(vnp->data.intvalue,mollen-1)==0)
								break;
							vnp->choice=0;
							vnp=vnp->next;
						}
						if (vnp)
							break;
						mollen--;
					}
					if (mollen<=1 && vnp==NULL)
						ErrPostEx(SEV_FATAL,0,0,"Unexpected logic error, cannot continue");
					/* now have longest branch possible, starting at vnp->data.intvalue */
					/* add mollen to residue numbers of last piece */
					if (!firstmol) {
						vnp=vnpResInMol;
						cnt=vnpResInMol->choice;
						while (cnt) {
							(vnp->choice)+=mollen;
							cnt--;
							vnp=vnp->next;
						}
					}
					/* sort in order of molecule with unassigned at the end (zeroes come last anyways) */
					vnp=NULL;
					VnpHeapSort(&vnpResInMol,ResCmpCallback2);
					fullen-=mollen;
					/* fullen is how many residues should have zeroes still */
					firstmol=0;
					mollen=fullen;
					vnpResInBranch=GetBranchPoints(vnpResInMol,mollist);
				}
			}
			/* sort in order of molecule */
			VnpHeapSort(&vnpResInMol,ResCmpCallback2);
			/* remove atoms from molecule from table */
			/* we can remove only nodes for pairs in vnpResInMol; then any remaining
			   pairs amongst these residues represent additional bonds i.e. a cycle
			   which can be added to a separate list of cyclic bonds, for later reference */
			vnp2=vnpResInMol;
			while (vnp2->next) {
				RemoveNode(mollist,vnp2->data.intvalue,vnp2->next->data.intvalue);
				vnp2=vnp2->next;
			}
			vnp2=vnpResInMol;
			while (vnp2) {
				while (mollist[vnp2->data.intvalue]) {
					mpp=(MolPairPtr)MemNew(sizeof(MolPair));
					mpp->mol1=vnp2->data.intvalue;
					mpp->mol2=mollist[vnp2->data.intvalue]->data.intvalue;
					/* ensure mol1 is smaller */
					if (mpp->mol1>mpp->mol2) {
						cnt=mpp->mol1;
						mpp->mol1=mpp->mol2;
						mpp->mol2=cnt;
					}
					RemoveNode(mollist,mpp->mol1,mpp->mol2);
					ValNodeAddPointer(&vnpCyclic,0,(VoidPtr)mpp);
				}
				vnp2=vnp2->next;
			}
			ValNodeAddPointer(&newmols,0,(VoidPtr)vnpResInMol);
			cnt=0;
		}
		else
			cnt++;
	}
	/* at this point newmols contains a list of the molecules that must be built */
	vnp=newmols;
	while (vnp) {
		vnpMol=(ValNodePtr)(vnp->data.ptrvalue);
		vnpHere=vnpMol;
		if (!quiet)
			printf("Merging molecules");
		while (vnpHere) {
			if (!quiet)
				printf(" %ld",(long)(vnpHere->data.intvalue));
			vnpHere=vnpHere->next;
		}
		if (!quiet)
			printf("\n");
		/* grab list of molecule numbers for first polymer and begin work */
		pmmdNew=NewMMD();
		/* populate new molecule object */
		pmmdNew->bWhat=(Byte)AM_POLY;
		pmsd->bWhat = (Byte) (pmsd->bWhat | (Byte)AM_POLY);
		/* possibly need to remove AM_HET from PMSD too */
		nohet=TRUE;
		pdnmm=pmsd->pdnmmHead;
		while (pdnmm) {
			pmmd=(PMMD)(pdnmm->data.ptrvalue);
			if ((pmmd->bWhat) & (Byte)AM_HET) {
				nohet=FALSE;
				break;
			}
			pdnmm=pdnmm->next;
		}
		if (nohet) {
			pmsd->bWhat-=(Byte)AM_HET;
		}
		pmmdNew->pfbParent=(PFB)pmsd;
		pmmdNew->iChainId=pmmdarr[vnpMol->data.intvalue]->iChainId;
		pmmdNew->pcMolName=NextFreeChain(pmsd);
		pmmdNew->pcSeqId=StringSave(pmmdarr[vnpMol->data.intvalue]->pcSeqId);
		pmmdNew->pMolDescr=pmmdarr[vnpMol->data.intvalue]->pMolDescr;
		/* going to be freed anyways */
		pmmdarr[vnpMol->data.intvalue]->pMolDescr=NULL;
		vnpHere=pmmdNew->pMolDescr;
		while (vnpHere) {
			if (vnpHere->choice==BiomolDescr_molecule_type)
				vnpHere->data.intvalue=Molecule_type_other_biopolymer;
			vnpHere=vnpHere->next;
		}
		pmmdNew->pSeqId = pmmdarr[vnpMol->data.intvalue]->pSeqId;
		pmmdarr[vnpMol->data.intvalue]->pSeqId=NULL;
		pdnmmNew=DValNodeNew(NULL);
		pdnmmNew->choice=vnpMol->data.intvalue;
		pdnmmNew->data.ptrvalue=(VoidPtr)pmmdNew;
		pmmdNew->pdnmmLink=pdnmmNew;
		pmmdNew->iResCount=ValNodeLen(vnpMol);
		vnpHere=vnpMol;
		resnum=1;
		molnumtoresnum=(Int4Ptr)MemNew((dim+1)*sizeof(Int4));
		/* build residue list of new molecule */
		while (vnpHere) {
			/* get residue */
			if (pmmdarr[vnpHere->data.intvalue]->pvnmbIRBHead)
				ErrPostEx(SEV_FATAL,0,0,"Unexpected inter-residue bonds found, cannot continue");
			pdnmg=pmmdarr[vnpHere->data.intvalue]->pdnmgHead;
			pmmdarr[vnpHere->data.intvalue]->pdnmgHead=NULL;
			if (resnum==1) {
				pmmdNew->pdnmgHead=pdnmg;
			}
			else {
				pdnmgLast->next=pdnmg;
			}
			pmgd=(PMGD)(pdnmg->data.ptrvalue);
			pmgd->pfbParent=(PFB)pmmdNew;
			pdnmgLast=pdnmg;
			pdnmg->choice=resnum;
			molnumtoresnum[vnpHere->data.intvalue]=resnum;
			pdnmg->next=NULL;
			resnum++;
			vnpHere=vnpHere->next;
		}
		/* clean up intermolecular bonds */
		/* clean up interresidue bonds */
		pvnmbIMB=pmsd->pvnmbIMBHead;
		pvnmbLast=NULL;
		while (pvnmbIMB) {
			pmbd=(PMBD)(pvnmbIMB->data.ptrvalue);
			pmad1=pmbd->pmadFrom;
			pmad2=pmbd->pmadTo;
			if ((PMMD)(pmad1->pfbParent->pfbParent)==pmmdNew && (PMMD)(pmad2->pfbParent->pfbParent)==pmmdNew) {
				/* found bond in pmmdNew, so move it */
				if (pvnmbLast==NULL) {
					pmsd->pvnmbIMBHead=pvnmbIMB->next;
				}
				else {
					pvnmbLast->next=pvnmbIMB->next;
				}
				pvnmbNext=pvnmbIMB->next;
				(pmsd->iIMBCount)--;
				/* note adding IRB bonds to HEAD of IRB list */
				pvnmbIMB->next=pmmdNew->pvnmbIRBHead;
				pmmdNew->pvnmbIRBHead=pvnmbIMB;
				(pmmdNew->iIRBCount)++;
				pvnmbIMB=pvnmbNext;
			}
			else {
				pvnmbLast=pvnmbIMB;
				pvnmbIMB=pvnmbIMB->next;
			}
		}
		/* label sugar residues with their linkages */
		if (IsSugar((PFB)pmmdNew)) {
			pvnmbHere=pmmdNew->pvnmbIRBHead;
			pvnmbLast=NULL;
			while (pvnmbHere) {
				pvnmbNext=pvnmbHere->next;
				pmbd=(PMBD)(pvnmbHere->data.ptrvalue);
				pmad1=pmbd->pmadFrom;
				pmad2=pmbd->pmadTo;
				pmgd1=(PMGD)(pmad1->pfbParent);
				pmgd2=(PMGD)(pmad2->pfbParent);
				if (pmgd1->pdnmgLink->choice>pmgd2->pdnmgLink->choice) {
					pmad1=pmbd->pmadTo;
					pmad2=pmbd->pmadFrom;
					pmgd1=(PMGD)(pmad1->pfbParent);
					pmgd2=(PMGD)(pmad2->pfbParent);
				}
				inum=0;
				inum2=0;
				if (sscanf(pmad1->pcAName+2,"%d",&inum) && sscanf(pmad2->pcAName+2,"%d",&inum2)) {
					if ((inum>0 || !IsSugar((PFB)pmgd1)) && (inum2>0 || !IsSugar((PFB)pmgd2))) {
						if (pmgd2->pdnmgLink->choice-pmgd1->pdnmgLink->choice>1) {
							/* could be branched OR cyclic here */
							vnpLast=NULL;
							vnpHere=vnpCyclic;
							while (vnpHere) {
								vnpNext=vnpHere->next;
								mpp=(MolPairPtr)(vnpHere->data.ptrvalue);
								/* remove unless cyclic (name has '(' at start already) */
								if (pmgd2->pcGraphName[0]!='(' && (molnumtoresnum[mpp->mol1]==pmgd1->pdnmgLink->choice && molnumtoresnum[mpp->mol2]==pmgd2->pdnmgLink->choice) || (molnumtoresnum[mpp->mol1]==pmgd2->pdnmgLink->choice && molnumtoresnum[mpp->mol2]==pmgd1->pdnmgLink->choice)) {
									/* remove from cyclic list */
									if (vnpLast==NULL)
										vnpCyclic=vnpNext;
									else
										vnpLast->next=vnpNext;
									vnpHere->next=NULL;
									vnpHere=ValNodeFreeData(vnpHere);
									break;
								}
								vnpLast=vnpHere;
								vnpHere=vnpNext;
							}
							/* will be cyclic if it has 2 bonds so update name below */
							if (pmgd2->pcGraphName[0]=='(')
								sprintf(buf,"%s",pmgd2->pcGraphName);
							else
								sprintf(buf,"(%d,%d-%d)%s",pmgd1->pdnmgLink->choice,inum,inum2,pmgd2->pcGraphName);
						}
						else
							sprintf(buf,"(%d-%d)%s",inum,inum2,pmgd2->pcGraphName);
						MemFree(pmgd2->pcGraphName);
						pmgd2->pcGraphName=StringSave(buf);
					}
				}
				pvnmbLast=pvnmbHere;
				pvnmbHere=pvnmbNext;
			}
		}
		/* check for cyclic */
		vnpHere=vnpCyclic;
		while (vnpHere) {
			mpp=(MolPairPtr)(vnpHere->data.ptrvalue);
			pdnmg=pmmdNew->pdnmgHead;
			while (pdnmg && pdnmg->choice!=molnumtoresnum[mpp->mol1])
				pdnmg=pdnmg->next;
			/* vnpCyclic contains cyclic bonds from ALL molecules not just current one */
			if (pdnmg) {
				pmgd=(PMGD)(pdnmg->data.ptrvalue);
				pvnmbHere=pmmdNew->pvnmbIRBHead;
				while (pvnmbHere) {
					pmbd=(PMBD)(pvnmbHere->data.ptrvalue);
					pmad1=pmbd->pmadFrom;
					pmad2=pmbd->pmadTo;
					pmgd1=(PMGD)(pmad1->pfbParent);
					pmgd2=(PMGD)(pmad2->pfbParent);
					inum=0;
					inum2=0;
					if (sscanf(pmad1->pcAName+2,"%d",&inum) && sscanf(pmad2->pcAName+2,"%d",&inum2)) {
						if (inum && inum2) {
							if (molnumtoresnum[mpp->mol1]==pmgd1->pdnmgLink->choice && molnumtoresnum[mpp->mol2]==pmgd2->pdnmgLink->choice) {
								sprintf(buf,"(c%d,%d-%d)%s",molnumtoresnum[mpp->mol2],inum2,inum,pmgd1->pcGraphName);
								break;
							}
							else if(molnumtoresnum[mpp->mol1]==pmgd2->pdnmgLink->choice && molnumtoresnum[mpp->mol2]==pmgd1->pdnmgLink->choice) {
								sprintf(buf,"(c%d,%d-%d)%s",molnumtoresnum[mpp->mol2],inum,inum2,pmgd1->pcGraphName);
								break;
							}
						}
					}
					pvnmbHere=pvnmbHere->next;
				}
				if (pvnmbHere==NULL)
					sprintf(buf,"(c%d)%s",molnumtoresnum[mpp->mol2],pmgd->pcGraphName);
				MemFree(pmgd->pcGraphName);
				pmgd->pcGraphName=StringSave(buf);
			}
			vnpHere=vnpHere->next;
		}
		MemFree(molnumtoresnum);
		/* put seqeunce into mol descr */
		StringCpy(sequence,"Mol shortname: ");
		pdnmg = pmmdNew->pdnmgHead;
		while (pdnmg) {
			pmgd = (PMGD)pdnmg->data.ptrvalue;
			StringCat(sequence, pmgd->pcGraphName);
			if (pdnmg->next) {
				StringCat(sequence, "-");
			}
			pdnmg = pdnmg->next;
		}
		ValNodeAddPointer(&(pmmdNew->pMolDescr),BiomolDescr_other_comment,(VoidPtr)StringSave(sequence));
		/* add molecule to master pdnmm list */
		pdnmm=pmmdarr[vnpMol->data.intvalue]->pdnmmLink;
		if (pdnmm->last) {
			pdnmm->last->next=pdnmmNew;
		}
		pdnmmNew->last=pdnmm->last;
		if (pdnmm->next) {
			pdnmm->next->last=pdnmmNew;
		}
		pdnmmNew->next=pdnmm->next;
		if (pmsd->pdnmmHead==pdnmm)
			pmsd->pdnmmHead=pdnmmNew;
		FreeMMD((PMMD)(pdnmm->data.ptrvalue));
		MemFree(pdnmm);
		vnpHere=vnpMol->next;
		while (vnpHere) {
			/* free the molecules we do not need anymore */
			/* subtract removed molecules */
			(pmsd->iMolCount)--;
			pdnmm=pmmdarr[vnpHere->data.intvalue]->pdnmmLink;
			if (pdnmm->last)
				pdnmm->last->next=pdnmm->next;
			if (pdnmm->next)
				pdnmm->next->last=pdnmm->last;
			if (pmsd->pdnmmHead==pdnmm)
				pmsd->pdnmmHead=pdnmm->next;
			FreeMMD((PMMD)(pdnmm->data.ptrvalue));
			pdnmmTmp=pdnmm->next;
			MemFree(pdnmm);
			while (pdnmmTmp) {
				pmmd=(PMMD)(pdnmmTmp->data.ptrvalue);
				(pmmd->iChainId)--;
				/* renumber only when done */
				/*(pdnmmTmp->choice)--;*/
				pdnmmTmp=pdnmmTmp->next;
			}
			vnpHere=vnpHere->next;
		}
		vnp=vnp->next;
	}
	/* renumber mols sequentially */
	pdnmm=pmsd->pdnmmHead;
	cnt=1;
	while (pdnmm) {
		pdnmm->choice=cnt;
		cnt++;
		pdnmm=pdnmm->next;
	}
	if (vnpCyclic)
		vnpCyclic=ValNodeFreeData(vnpCyclic);
	MemFree(molnames);
	MemFree(mollist);
	MemFree(pmmdarr);
	MemFree(pdbresnum);
	MemFree(pdbresname);
	vnp=newmols;
	while (vnp) {
		vnpMol=(ValNodePtr)(vnp->data.ptrvalue);
		ValNodeFree(vnpMol);
		vnp=vnp->next;
	}
	ValNodeFree(newmols);
	return pmsd;
}

/* returns NULL on error, otherwise returns corrected Biostruc
   bsp is destroyed (freed) in the process */
BiostrucPtr BiostrucPolymerFix(BiostrucPtr bsp,Int4 quiet)
{
	PDNMS pdnms;
	PMSD pmsd;
	Char fnam[PATH_MAX];
	BiostrucPtr bspnew;
	AsnIoPtr aip;

	if (bsp==NULL)
		return NULL;
	pdnms=MakeAModelstruc(bsp);
	if (pdnms==NULL) {
		ErrPostEx(SEV_ERROR,1,1,"[BiostrucPolymerFix] Unable to convert Biostruc to Modelstruc");
		return NULL;
	}
	pmsd=(PMSD)(pdnms->data.ptrvalue);
	pmsd=FixBiostrucPolymers(pmsd,quiet);
	pdnms=pmsd->pdnmsLink;
	TmpNam(fnam);
	if (!WriteAsnAllModel(pdnms,fnam,SAVE_BINARY)) {
		ErrPostEx(SEV_ERROR,1,1,"[BiostrucPolymerFix] Unable to write out biostruc to %s",fnam);
		return NULL;
	}
	FreeAModelstruc(pdnms);
	bspnew=NULL;
	aip=AsnIoOpen(fnam,"rb");
	bspnew=BiostrucAsnRead(aip,NULL);
	AsnIoClose(aip);
	FileRemove(fnam);
	if (bspnew==NULL) {
		ErrPostEx(SEV_ERROR,1,1,"[BiostrucPolymerFix] Unable to read back corrected Biostruc");
	}
	return bspnew;
}

/*
$Log: polymerfix.c,v $
Revision 1.6  2005/03/22 17:30:34  vgu
Change license to GPL

Revision 1.5  2005/03/21 16:48:09  hfeldman
changed back to remove pairs with a close bond, but remove bond if 2 bonds to an H

Revision 1.4  2005/03/16 23:24:41  hfeldman
changed logic - only delete bond, not whole molecule pair, if bond is garbage

Revision 1.3  2005/03/15 17:50:06  hfeldman
only check for H-bonded to non-solvent molecules

Revision 1.2  2005/03/14 22:15:00  hfeldman
forbid H inter-molecular bonds

Revision 1.1  2004/11/18 19:03:10  mmatan
This file contains functionality for "fixing" a biostruc.  See bug 3930 for details

Revision 1.12  2004/11/17 18:48:01  hfeldman
added biostruc wrapper function

Revision 1.11  2004/11/16 20:06:43  hfeldman
Added quiet mode

Revision 1.10  2004/11/15 23:14:42  hfeldman
ignore occupancy < 0.5 atoms/bonds

Revision 1.9  2004/11/15 19:58:07  ksnyder
Added back include statement for utilpub.h

Revision 1.8  2004/11/10 22:56:46  hfeldman
Got rid of sugar bond length cutoff
Check if C has 2 O's attached - not allowed

Revision 1.7  2004/11/10 01:39:33  hfeldman
corrected naming for cyclic and other strange molecules

Revision 1.4  2004/11/05 18:17:10  hfeldman
Added support for branched sugars

Revision 1.3  2004/11/04 21:26:18  hfeldman
Rewrote residue ordering portion of algorithm - looks for any path along nodes in graph now,
irregardless of PDB/NCBI numbering - still had some trouble in naming carbohydrates possibly

Revision 1.2  2004/11/04 15:41:56  hfeldman
Added checks for overlapping molecules and other various imporovements


*/

