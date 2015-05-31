/***********************************************************************
*
**
*        Automatic header module from ASNTOOL
*
************************************************************************/

#ifndef _ASNTOOL_
#include <asn.h>
#endif

static char * asnfilename = "slristruc.h";
static AsnType atx[353] = {
  {401, "StRpsdb" ,1,0,0,0,0,1,0,0,NULL,&atx[18],&atx[1],0,&atx[19]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {439, "StRpsNode" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[3],0,&atx[200]} ,
  {0, "gi" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[5]} ,
  {302, "INTEGER" ,0,2,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "dom-id" ,128,1,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[7]} ,
  {323, "VisibleString" ,0,26,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "cdd-id" ,128,2,0,1,0,0,0,0,NULL,&atx[4],NULL,0,&atx[8]} ,
  {0, "from" ,128,3,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[9]} ,
  {0, "len" ,128,4,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[10]} ,
  {0, "score" ,128,5,0,1,0,0,0,0,NULL,&atx[4],NULL,0,&atx[11]} ,
  {0, "evalue" ,128,6,0,0,0,0,0,0,NULL,&atx[12],NULL,0,&atx[13]} ,
  {309, "REAL" ,0,9,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "bitscore" ,128,7,0,1,0,0,0,0,NULL,&atx[12],NULL,0,&atx[14]} ,
  {0, "n-missing" ,128,8,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[15]} ,
  {0, "c-missing" ,128,9,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[16]} ,
  {0, "num-of-dom" ,128,10,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {311, "SEQUENCE" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {312, "SEQUENCE OF" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {402, "SHProteinReportSetDescription" ,1,0,0,0,0,1,0,0,NULL,&atx[17],&atx[20],0,&atx[69]} ,
  {0, "title" ,128,0,0,1,0,0,0,0,NULL,&atx[6],NULL,0,&atx[21]} ,
  {0, "total-reports" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[22]} ,
  {0, "query" ,128,2,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[23]} ,
  {0, "query-report" ,128,3,0,1,0,0,0,0,NULL,&atx[24],NULL,0,&atx[66]} ,
  {404, "SHProteinReport" ,1,0,0,0,0,1,0,0,NULL,&atx[17],&atx[25],0,&atx[91]} ,
  {0, "gi" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[26]} ,
  {0, "acc" ,128,1,0,1,0,0,0,0,NULL,&atx[6],NULL,0,&atx[27]} ,
  {0, "defline" ,128,2,0,1,0,0,0,0,NULL,&atx[6],NULL,0,&atx[28]} ,
  {0, "dna" ,128,3,0,1,0,0,0,0,NULL,&atx[4],NULL,0,&atx[29]} ,
  {0, "muids" ,128,4,0,1,0,0,0,0,NULL,&atx[18],&atx[30],0,&atx[31]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {0, "pmids" ,128,5,0,1,0,0,0,0,NULL,&atx[18],&atx[32],0,&atx[33]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {0, "mmdbid" ,128,6,0,1,0,0,0,0,NULL,&atx[4],NULL,0,&atx[34]} ,
  {0, "taxid" ,128,7,0,1,0,0,0,0,NULL,&atx[4],NULL,0,&atx[35]} ,
  {0, "go" ,128,8,0,1,0,0,0,0,NULL,&atx[18],&atx[36],0,&atx[54]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[37],NULL,0,NULL} ,
  {452, "SHGoReport" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[38],0,&atx[58]} ,
  {0, "num-mol-fun" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[39]} ,
  {0, "num-bio-pro" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[40]} ,
  {0, "num-cell-com" ,128,2,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[41]} ,
  {0, "mol-fun" ,128,3,0,1,0,0,0,0,NULL,&atx[42],NULL,0,&atx[52]} ,
  {455, "SHGoAnnot" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[43],0,&atx[44]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[44],NULL,0,NULL} ,
  {456, "SHGoAnnotNode" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[45],0,&atx[63]} ,
  {0, "goid" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[46]} ,
  {0, "ontology" ,128,1,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[47]} ,
  {0, "goname" ,128,2,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[48]} ,
  {0, "evidence-code" ,128,3,0,0,0,0,0,0,NULL,&atx[18],&atx[49],0,&atx[50]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[6],NULL,0,NULL} ,
  {0, "pmid" ,128,4,0,0,0,0,0,0,NULL,&atx[18],&atx[51],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {0, "bio-pro" ,128,4,0,1,0,0,0,0,NULL,&atx[42],NULL,0,&atx[53]} ,
  {0, "cell-com" ,128,5,0,1,0,0,0,0,NULL,&atx[42],NULL,0,NULL} ,
  {0, "omim" ,128,9,0,1,0,0,0,0,NULL,&atx[18],&atx[55],0,&atx[56]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {0, "llink" ,128,10,0,1,0,0,0,0,NULL,&atx[18],&atx[57],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[58],NULL,0,NULL} ,
  {453, "SHLocusLinkReport" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[59],0,&atx[243]} ,
  {0, "llid" ,128,0,0,1,0,0,0,0,NULL,&atx[4],NULL,0,&atx[60]} ,
  {0, "locus" ,128,1,0,1,0,0,0,0,NULL,&atx[6],NULL,0,&atx[61]} ,
  {0, "products" ,128,2,0,1,0,0,0,0,NULL,&atx[18],&atx[62],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[63],NULL,0,NULL} ,
  {457, "SHLocusProduct" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[64],0,&atx[246]} ,
  {0, "gi" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[65]} ,
  {0, "defline" ,128,1,0,1,0,0,0,0,NULL,&atx[6],NULL,0,NULL} ,
  {0, "redundant-set" ,128,4,0,1,0,0,0,0,NULL,&atx[18],&atx[67],0,&atx[68]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {0, "redun-reports" ,128,5,0,1,0,0,0,0,NULL,&atx[69],NULL,0,&atx[73]} ,
  {403, "SHProteinReportSet" ,1,0,0,0,0,1,0,0,NULL,&atx[17],&atx[70],0,&atx[24]} ,
  {0, "number" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[71]} ,
  {0, "reports" ,128,1,0,0,0,0,0,0,NULL,&atx[18],&atx[72],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[24],NULL,0,NULL} ,
  {0, "neighbour-set" ,128,6,0,1,0,0,0,0,NULL,&atx[74],NULL,0,&atx[75]} ,
  {410, "FLink-set" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[96]} ,
  {0, "best-report" ,128,7,0,1,0,0,0,0,NULL,&atx[24],NULL,0,&atx[76]} ,
  {0, "neighbour-reports" ,128,8,0,1,0,0,0,0,NULL,&atx[69],NULL,0,&atx[77]} ,
  {0, "cutoff" ,128,9,0,1,0,0,0,0,NULL,&atx[12],NULL,0,&atx[78]} ,
  {0, "acc" ,128,10,0,0,0,0,0,0,NULL,&atx[79],NULL,0,&atx[80]} ,
  {301, "BOOLEAN" ,0,1,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "defline" ,128,11,0,0,0,0,0,0,NULL,&atx[79],NULL,0,&atx[81]} ,
  {0, "dna" ,128,12,0,0,0,0,0,0,NULL,&atx[79],NULL,0,&atx[82]} ,
  {0, "taxid" ,128,13,0,0,0,0,0,0,NULL,&atx[79],NULL,0,&atx[83]} ,
  {0, "omim" ,128,14,0,0,0,0,0,0,NULL,&atx[79],NULL,0,&atx[84]} ,
  {0, "muid" ,128,15,0,0,0,0,0,0,NULL,&atx[79],NULL,0,&atx[85]} ,
  {0, "pmid" ,128,16,0,0,0,0,0,0,NULL,&atx[79],NULL,0,&atx[86]} ,
  {0, "mmdb-ids" ,128,17,0,0,0,0,0,0,NULL,&atx[79],NULL,0,&atx[87]} ,
  {0, "go" ,128,18,0,0,0,0,0,0,NULL,&atx[79],NULL,0,&atx[88]} ,
  {0, "llink" ,128,19,0,0,0,0,0,0,NULL,&atx[79],NULL,0,&atx[89]} ,
  {0, "redund" ,128,20,0,0,0,0,0,0,NULL,&atx[79],NULL,0,&atx[90]} ,
  {0, "neighbours" ,128,21,0,0,0,0,0,0,NULL,&atx[79],NULL,0,NULL} ,
  {405, "Bioseq" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[92]} ,
  {406, "Biostruc" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[93]} ,
  {407, "Seq-entry" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[94]} ,
  {408, "SLRIValNode" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[95]} ,
  {409, "SLRIFasta" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[74]} ,
  {411, "StAsndbNode" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[97],0,&atx[102]} ,
  {0, "gi" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[98]} ,
  {0, "asn1" ,128,1,0,0,0,0,0,0,NULL,&atx[91],NULL,0,&atx[99]} ,
  {0, "division" ,128,2,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[100]} ,
  {0, "release" ,128,3,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[101]} ,
  {0, "type" ,128,4,0,0,0,0,0,0,NULL,&atx[6],NULL,0,NULL} ,
  {412, "StAsndb" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[103],0,&atx[104]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[96],NULL,0,NULL} ,
  {413, "StPartiNode" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[105],0,&atx[107]} ,
  {0, "gi" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[106]} ,
  {0, "partition" ,128,1,0,0,0,0,0,0,NULL,&atx[6],NULL,0,NULL} ,
  {414, "StParti" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[108],0,&atx[109]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[104],NULL,0,NULL} ,
  {415, "StNucprotNode" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[110],0,&atx[112]} ,
  {0, "gin" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[111]} ,
  {0, "gia" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {416, "StNucprot" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[113],0,&atx[114]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[109],NULL,0,NULL} ,
  {417, "StAccdbNode" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[115],0,&atx[124]} ,
  {0, "gi" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[116]} ,
  {0, "db" ,128,1,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[117]} ,
  {0, "name" ,128,2,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[118]} ,
  {0, "access" ,128,3,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[119]} ,
  {0, "chain" ,128,4,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[120]} ,
  {0, "release" ,128,5,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[121]} ,
  {0, "version" ,128,6,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[122]} ,
  {0, "title" ,128,7,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[123]} ,
  {0, "namelow" ,128,8,0,0,0,0,0,0,NULL,&atx[6],NULL,0,NULL} ,
  {418, "StAccdb" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[125],0,&atx[126]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[114],NULL,0,NULL} ,
  {419, "StPubseqNode" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[127],0,&atx[130]} ,
  {0, "gi" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[128]} ,
  {0, "muid" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[129]} ,
  {0, "pmid" ,128,2,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {420, "StPubseq" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[131],0,&atx[132]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[126],NULL,0,NULL} ,
  {421, "StTaxgiNode" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[133],0,&atx[137]} ,
  {0, "gi" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[134]} ,
  {0, "taxid" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[135]} ,
  {0, "kloodge" ,128,2,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[136]} ,
  {0, "type" ,128,3,0,0,0,0,0,0,NULL,&atx[6],NULL,0,NULL} ,
  {422, "StTaxgi" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[138],0,&atx[139]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[132],NULL,0,NULL} ,
  {423, "StChromNode" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[140],0,&atx[148]} ,
  {0, "kloodge" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[141]} ,
  {0, "taxid" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[142]} ,
  {0, "chromid" ,128,2,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[143]} ,
  {0, "chromfl" ,128,3,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[144]} ,
  {0, "access" ,128,4,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[145]} ,
  {0, "name" ,128,5,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[146]} ,
  {0, "projectid" ,128,6,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[147]} ,
  {0, "chromnum" ,128,7,0,0,0,0,0,0,NULL,&atx[6],NULL,0,NULL} ,
  {424, "StChrom" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[149],0,&atx[150]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[139],NULL,0,NULL} ,
  {425, "StSendbNode" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[151],0,&atx[153]} ,
  {0, "seid" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[152]} ,
  {0, "asn1" ,128,1,0,0,0,0,0,0,NULL,&atx[93],NULL,0,NULL} ,
  {426, "StSendb" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[154],0,&atx[155]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[150],NULL,0,NULL} ,
  {427, "StSengiNode" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[156],0,&atx[159]} ,
  {0, "seid" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[157]} ,
  {0, "gi" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[158]} ,
  {0, "division" ,128,2,0,0,0,0,0,0,NULL,&atx[6],NULL,0,NULL} ,
  {428, "StSengi" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[160],0,&atx[161]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[155],NULL,0,NULL} ,
  {429, "StMmdbNode" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[162],0,&atx[170]} ,
  {0, "mmdbid" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[163]} ,
  {0, "pdbid" ,128,1,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[164]} ,
  {0, "asn1" ,128,2,0,0,0,0,0,0,NULL,&atx[92],NULL,0,&atx[165]} ,
  {0, "bwhat" ,128,3,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[166]} ,
  {0, "models" ,128,4,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[167]} ,
  {0, "molecules" ,128,5,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[168]} ,
  {0, "size" ,128,6,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[169]} ,
  {0, "bzsize" ,128,7,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {430, "StMmdb" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[171],0,&atx[172]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[161],NULL,0,NULL} ,
  {431, "StMmgiNode" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[173],0,&atx[175]} ,
  {0, "mmdbid" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[174]} ,
  {0, "gi" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {432, "StMmgi" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[176],0,&atx[177]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[172],NULL,0,NULL} ,
  {433, "StRedundNode" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[178],0,&atx[181]} ,
  {0, "gi" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[179]} ,
  {0, "ordinal" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[180]} ,
  {0, "group" ,128,2,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {434, "StRedund" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[182],0,&atx[183]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[177],NULL,0,NULL} ,
  {435, "StDomdbNode" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[184],0,&atx[193]} ,
  {0, "mmdbid" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[185]} ,
  {0, "asn1" ,128,1,0,0,0,0,0,0,NULL,&atx[94],NULL,0,&atx[186]} ,
  {0, "pdbid" ,128,2,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[187]} ,
  {0, "chain" ,128,3,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[188]} ,
  {0, "gi" ,128,4,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[189]} ,
  {0, "domno" ,128,5,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[190]} ,
  {0, "domall" ,128,6,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[191]} ,
  {0, "domid" ,128,7,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[192]} ,
  {0, "rep" ,128,8,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {436, "StDomdb" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[194],0,&atx[195]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[183],NULL,0,NULL} ,
  {437, "StCddbNode" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[196],0,&atx[198]} ,
  {0, "gi" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[197]} ,
  {0, "asn1" ,128,1,0,0,0,0,0,0,NULL,&atx[95],NULL,0,NULL} ,
  {438, "StCddb" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[199],0,&atx[2]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[195],NULL,0,NULL} ,
  {440, "StDomNameNode" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[201],0,&atx[207]} ,
  {0, "pssmid" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[202]} ,
  {0, "accession" ,128,1,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[203]} ,
  {0, "name" ,128,2,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[204]} ,
  {0, "description" ,128,3,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[205]} ,
  {0, "length" ,128,4,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[206]} ,
  {0, "pdb-id" ,128,5,0,1,0,0,0,0,NULL,&atx[6],NULL,0,NULL} ,
  {441, "StDomNamedb" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[208],0,&atx[209]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[200],NULL,0,NULL} ,
  {442, "StHistdbNode" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[210],0,&atx[217]} ,
  {0, "gi" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[211]} ,
  {0, "oldgi" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[212]} ,
  {0, "access" ,128,2,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[213]} ,
  {0, "version" ,128,3,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[214]} ,
  {0, "date" ,128,4,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[215]} ,
  {0, "action" ,128,5,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[216]} ,
  {0, "filename" ,128,6,0,0,0,0,0,0,NULL,&atx[6],NULL,0,NULL} ,
  {443, "StHistdb" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[218],0,&atx[219]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[209],NULL,0,NULL} ,
  {444, "StGOdbNode" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[220],0,&atx[224]} ,
  {0, "llid" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[221]} ,
  {0, "goid" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[222]} ,
  {0, "pmid" ,128,2,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[223]} ,
  {0, "eviCode" ,128,3,0,0,0,0,0,0,NULL,&atx[6],NULL,0,NULL} ,
  {445, "StGOdb" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[225],0,&atx[226]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[219],NULL,0,NULL} ,
  {446, "StOMIMdbNode" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[227],0,&atx[229]} ,
  {0, "llid" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[228]} ,
  {0, "omimid" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {447, "StOMIMdb" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[230],0,&atx[231]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[226],NULL,0,NULL} ,
  {448, "StCDDdbNode" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[232],0,&atx[235]} ,
  {0, "llid" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[233]} ,
  {0, "cddid" ,128,1,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[234]} ,
  {0, "eValue" ,128,2,0,0,0,0,0,0,NULL,&atx[12],NULL,0,NULL} ,
  {449, "StCDDdb" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[236],0,&atx[237]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[231],NULL,0,NULL} ,
  {450, "StLLdbNode" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[238],0,&atx[241]} ,
  {0, "llid" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[239]} ,
  {0, "npid" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[240]} ,
  {0, "map" ,128,2,0,0,0,0,0,0,NULL,&atx[6],NULL,0,NULL} ,
  {451, "StLLdb" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[242],0,&atx[37]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[237],NULL,0,NULL} ,
  {454, "SHNeighbour" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[244],0,&atx[42]} ,
  {0, "gi" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[245]} ,
  {0, "evalue" ,128,1,0,0,0,0,0,0,NULL,&atx[12],NULL,0,NULL} ,
  {458, "StDbXrefNode" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[247],0,&atx[255]} ,
  {0, "id" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[248]} ,
  {0, "sourcedb" ,128,1,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[249]} ,
  {0, "sourceid" ,128,2,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[250]} ,
  {0, "recordid" ,128,3,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[251]} ,
  {0, "parentid" ,128,4,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[252]} ,
  {0, "link" ,128,5,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[253]} ,
  {0, "field" ,128,6,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[254]} ,
  {0, "cv" ,128,7,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {459, "StDbXref" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[256],0,&atx[257]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[246],NULL,0,NULL} ,
  {460, "SHDBXrefNode" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[258],0,&atx[262]} ,
  {0, "sourcedb" ,128,0,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[259]} ,
  {0, "recordid" ,128,1,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[260]} ,
  {0, "field" ,128,2,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[261]} ,
  {0, "cv" ,128,3,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {461, "SHDBXref" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[263],0,&atx[264]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[257],NULL,0,NULL} ,
  {462, "StBioentityNode" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[265],0,&atx[271]} ,
  {0, "id" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[266]} ,
  {0, "bioentitytypeid" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[267]} ,
  {0, "db" ,128,2,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[268]} ,
  {0, "access" ,128,3,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[269]} ,
  {0, "identifier" ,128,4,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[270]} ,
  {0, "field" ,128,5,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {463, "StBioentity" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[272],0,&atx[273]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[264],NULL,0,NULL} ,
  {464, "StBionameNode" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[274],0,&atx[288]} ,
  {0, "id" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[275]} ,
  {0, "bioentityid" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[276]} ,
  {0, "name" ,128,2,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[277]} ,
  {0, "nametypeid" ,128,3,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[278]} ,
  {0, "db" ,128,4,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[279]} ,
  {0, "access" ,128,5,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[280]} ,
  {0, "identifier" ,128,6,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[281]} ,
  {0, "field" ,128,7,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[282]} ,
  {0, "official" ,128,8,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[283]} ,
  {0, "deprecated" ,128,9,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[284]} ,
  {0, "date" ,128,10,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[285]} ,
  {0, "ruleid" ,128,11,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[286]} ,
  {0, "action" ,128,12,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[287]} ,
  {0, "actiondate" ,128,13,0,0,0,0,0,0,NULL,&atx[6],NULL,0,NULL} ,
  {465, "StBioname" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[289],0,&atx[290]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[273],NULL,0,NULL} ,
  {466, "StSecondrefsNode" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[291],0,&atx[296]} ,
  {0, "id" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[292]} ,
  {0, "bionameid" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[293]} ,
  {0, "db" ,128,2,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[294]} ,
  {0, "access" ,128,3,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[295]} ,
  {0, "field" ,128,4,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {467, "StSecondrefs" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[297],0,&atx[298]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[290],NULL,0,NULL} ,
  {468, "StGichromidNode" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[299],0,&atx[303]} ,
  {0, "id" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[300]} ,
  {0, "gi" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[301]} ,
  {0, "contiggi" ,128,2,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[302]} ,
  {0, "chromid" ,128,3,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {469, "StGichromid" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[304],0,&atx[305]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[298],NULL,0,NULL} ,
  {470, "StContigchromidNode" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[306],0,&atx[311]} ,
  {0, "id" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[307]} ,
  {0, "contiggi" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[308]} ,
  {0, "topgi" ,128,2,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[309]} ,
  {0, "chromid" ,128,3,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[310]} ,
  {0, "changed" ,128,4,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {471, "StContigchromid" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[312],0,&atx[313]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[305],NULL,0,NULL} ,
  {472, "StGichromNode" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[314],0,&atx[317]} ,
  {0, "id" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[315]} ,
  {0, "gi" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[316]} ,
  {0, "chromnum" ,128,2,0,0,0,0,0,0,NULL,&atx[6],NULL,0,NULL} ,
  {473, "StGichrom" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[318],0,&atx[319]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[313],NULL,0,NULL} ,
  {474, "StContigchromNode" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[320],0,&atx[323]} ,
  {0, "id" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[321]} ,
  {0, "contiggi" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[322]} ,
  {0, "chromnum" ,128,2,0,0,0,0,0,0,NULL,&atx[6],NULL,0,NULL} ,
  {475, "StContigchrom" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[324],0,&atx[325]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[319],NULL,0,NULL} ,
  {476, "StSourcefileNode" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[326],0,&atx[330]} ,
  {0, "id" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[327]} ,
  {0, "gi" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[328]} ,
  {0, "filename" ,128,2,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[329]} ,
  {0, "date" ,128,3,0,0,0,0,0,0,NULL,&atx[6],NULL,0,NULL} ,
  {477, "StSourcefile" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[331],0,&atx[332]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[325],NULL,0,NULL} ,
  {478, "StGeneObjectNode" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[333],0,&atx[337]} ,
  {0, "id" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[334]} ,
  {0, "taxid" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[335]} ,
  {0, "geneid" ,128,2,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[336]} ,
  {0, "status" ,128,3,0,0,0,0,0,0,NULL,&atx[6],NULL,0,NULL} ,
  {479, "StGeneObject" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[338],0,&atx[339]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[332],NULL,0,NULL} ,
  {480, "StGeneGiNode" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[340],0,&atx[344]} ,
  {0, "id" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[341]} ,
  {0, "geneobjectid" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[342]} ,
  {0, "gi" ,128,2,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[343]} ,
  {0, "moleculetype" ,128,3,0,0,0,0,0,0,NULL,&atx[6],NULL,0,NULL} ,
  {481, "StGeneGi" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[345],0,&atx[346]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[339],NULL,0,NULL} ,
  {482, "StGeneDbxrefNode" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[347],0,&atx[351]} ,
  {0, "id" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[348]} ,
  {0, "geneobjectid" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[349]} ,
  {0, "dbname" ,128,2,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[350]} ,
  {0, "dbaccess" ,128,3,0,0,0,0,0,0,NULL,&atx[6],NULL,0,NULL} ,
  {483, "StGeneDbxref" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[352],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[346],NULL,0,NULL} };

static AsnModule ampx[1] = {
  { "INTREZ-databases" , "slristruc.h",&atx[0],NULL,NULL,0,0} };

static AsnValxNodePtr avn = NULL;
static AsnTypePtr at = atx;
static AsnModulePtr amp = ampx;



/**************************************************
*
*    Defines for Module INTREZ-databases
*
**************************************************/

#define STRPSDB &at[0]
#define STRPSDB_E &at[1]

#define SHPROTEINREPORTSETDESCRIPTION &at[19]
#define SHPROTEINREPORTSETDESCRIPTION_title &at[20]
#define SHPROTEINREPORTSETDESCRIPTION_total_reports &at[21]
#define SHPROTEINREPORTSETDESCRIPTION_query &at[22]
#define SHPROTEINREPORTSETDESCRIPTION_query_report &at[23]
#define SHPROTEINREPORTSETDESCRIPTION_redundant_set &at[66]
#define SHPROTEINREPORTSETDESCRIPTION_redundant_set_E &at[67]
#define SHPROTEINREPORTSETDESCRIPTION_redun_reports &at[68]
#define SHPROTEINREPORTSETDESCRIPTION_neighbour_set &at[73]
#define SHPROTEINREPORTSETDESCRIPTION_best_report &at[75]
#define SHPROTEINREPORTSETDESCRIPTION_neighbour_reports &at[76]
#define SHPROTEINREPORTSETDESCRIPTION_cutoff &at[77]
#define SHPROTEINREPORTSETDESCRIPTION_acc &at[78]
#define SHPROTEINREPORTSETDESCRIPTION_defline &at[80]
#define SHPROTEINREPORTSETDESCRIPTION_dna &at[81]
#define SHPROTEINREPORTSETDESCRIPTION_taxid &at[82]
#define SHPROTEINREPORTSETDESCRIPTION_omim &at[83]
#define SHPROTEINREPORTSETDESCRIPTION_muid &at[84]
#define SHPROTEINREPORTSETDESCRIPTION_pmid &at[85]
#define SHPROTEINREPORTSETDESCRIPTION_mmdb_ids &at[86]
#define SHPROTEINREPORTSETDESCRIPTION_go &at[87]
#define SHPROTEINREPORTSETDESCRIPTION_llink &at[88]
#define SHPROTEINREPORTSETDESCRIPTION_redund &at[89]
#define SHPROTEINREPORTSETDESCRIPTION_neighbours &at[90]

#define SHPROTEINREPORTSET &at[69]
#define SHPROTEINREPORTSET_number &at[70]
#define SHPROTEINREPORTSET_reports &at[71]
#define SHPROTEINREPORTSET_reports_E &at[72]

#define SHPROTEINREPORT &at[24]
#define SHPROTEINREPORT_gi &at[25]
#define SHPROTEINREPORT_acc &at[26]
#define SHPROTEINREPORT_defline &at[27]
#define SHPROTEINREPORT_dna &at[28]
#define SHPROTEINREPORT_muids &at[29]
#define SHPROTEINREPORT_muids_E &at[30]
#define SHPROTEINREPORT_pmids &at[31]
#define SHPROTEINREPORT_pmids_E &at[32]
#define SHPROTEINREPORT_mmdbid &at[33]
#define SHPROTEINREPORT_taxid &at[34]
#define SHPROTEINREPORT_go &at[35]
#define SHPROTEINREPORT_go_E &at[36]
#define SHPROTEINREPORT_omim &at[54]
#define SHPROTEINREPORT_omim_E &at[55]
#define SHPROTEINREPORT_llink &at[56]
#define SHPROTEINREPORT_llink_E &at[57]

#define STASNDBNODE &at[96]
#define STASNDBNODE_gi &at[97]
#define STASNDBNODE_asn1 &at[98]
#define STASNDBNODE_division &at[99]
#define STASNDBNODE_release &at[100]
#define STASNDBNODE_type &at[101]

#define STASNDB &at[102]
#define STASNDB_E &at[103]

#define STPARTINODE &at[104]
#define STPARTINODE_gi &at[105]
#define STPARTINODE_partition &at[106]

#define STPARTI &at[107]
#define STPARTI_E &at[108]

#define STNUCPROTNODE &at[109]
#define STNUCPROTNODE_gin &at[110]
#define STNUCPROTNODE_gia &at[111]

#define STNUCPROT &at[112]
#define STNUCPROT_E &at[113]

#define STACCDBNODE &at[114]
#define STACCDBNODE_gi &at[115]
#define STACCDBNODE_db &at[116]
#define STACCDBNODE_name &at[117]
#define STACCDBNODE_access &at[118]
#define STACCDBNODE_chain &at[119]
#define STACCDBNODE_release &at[120]
#define STACCDBNODE_version &at[121]
#define STACCDBNODE_title &at[122]
#define STACCDBNODE_namelow &at[123]

#define STACCDB &at[124]
#define STACCDB_E &at[125]

#define STPUBSEQNODE &at[126]
#define STPUBSEQNODE_gi &at[127]
#define STPUBSEQNODE_muid &at[128]
#define STPUBSEQNODE_pmid &at[129]

#define STPUBSEQ &at[130]
#define STPUBSEQ_E &at[131]

#define STTAXGINODE &at[132]
#define STTAXGINODE_gi &at[133]
#define STTAXGINODE_taxid &at[134]
#define STTAXGINODE_kloodge &at[135]
#define STTAXGINODE_type &at[136]

#define STTAXGI &at[137]
#define STTAXGI_E &at[138]

#define STCHROMNODE &at[139]
#define STCHROMNODE_kloodge &at[140]
#define STCHROMNODE_taxid &at[141]
#define STCHROMNODE_chromid &at[142]
#define STCHROMNODE_chromfl &at[143]
#define STCHROMNODE_access &at[144]
#define STCHROMNODE_name &at[145]
#define STCHROMNODE_projectid &at[146]
#define STCHROMNODE_chromnum &at[147]

#define STCHROM &at[148]
#define STCHROM_E &at[149]

#define STSENDBNODE &at[150]
#define STSENDBNODE_seid &at[151]
#define STSENDBNODE_asn1 &at[152]

#define STSENDB &at[153]
#define STSENDB_E &at[154]

#define STSENGINODE &at[155]
#define STSENGINODE_seid &at[156]
#define STSENGINODE_gi &at[157]
#define STSENGINODE_division &at[158]

#define STSENGI &at[159]
#define STSENGI_E &at[160]

#define STMMDBNODE &at[161]
#define STMMDBNODE_mmdbid &at[162]
#define STMMDBNODE_pdbid &at[163]
#define STMMDBNODE_asn1 &at[164]
#define STMMDBNODE_bwhat &at[165]
#define STMMDBNODE_models &at[166]
#define STMMDBNODE_molecules &at[167]
#define STMMDBNODE_size &at[168]
#define STMMDBNODE_bzsize &at[169]

#define STMMDB &at[170]
#define STMMDB_E &at[171]

#define STMMGINODE &at[172]
#define STMMGINODE_mmdbid &at[173]
#define STMMGINODE_gi &at[174]

#define STMMGI &at[175]
#define STMMGI_E &at[176]

#define STREDUNDNODE &at[177]
#define STREDUNDNODE_gi &at[178]
#define STREDUNDNODE_ordinal &at[179]
#define STREDUNDNODE_group &at[180]

#define STREDUND &at[181]
#define STREDUND_E &at[182]

#define STDOMDBNODE &at[183]
#define STDOMDBNODE_mmdbid &at[184]
#define STDOMDBNODE_asn1 &at[185]
#define STDOMDBNODE_pdbid &at[186]
#define STDOMDBNODE_chain &at[187]
#define STDOMDBNODE_gi &at[188]
#define STDOMDBNODE_domno &at[189]
#define STDOMDBNODE_domall &at[190]
#define STDOMDBNODE_domid &at[191]
#define STDOMDBNODE_rep &at[192]

#define STDOMDB &at[193]
#define STDOMDB_E &at[194]

#define STCDDBNODE &at[195]
#define STCDDBNODE_gi &at[196]
#define STCDDBNODE_asn1 &at[197]

#define STCDDB &at[198]
#define STCDDB_E &at[199]

#define STRPSNODE &at[2]
#define STRPSNODE_gi &at[3]
#define STRPSNODE_dom_id &at[5]
#define STRPSNODE_cdd_id &at[7]
#define STRPSNODE_from &at[8]
#define STRPSNODE_len &at[9]
#define STRPSNODE_score &at[10]
#define STRPSNODE_evalue &at[11]
#define STRPSNODE_bitscore &at[13]
#define STRPSNODE_n_missing &at[14]
#define STRPSNODE_c_missing &at[15]
#define STRPSNODE_num_of_dom &at[16]

#define STDOMNAMENODE &at[200]
#define STDOMNAMENODE_pssmid &at[201]
#define STDOMNAMENODE_accession &at[202]
#define STDOMNAMENODE_name &at[203]
#define STDOMNAMENODE_description &at[204]
#define STDOMNAMENODE_length &at[205]
#define STDOMNAMENODE_pdb_id &at[206]

#define STDOMNAMEDB &at[207]
#define STDOMNAMEDB_E &at[208]

#define STHISTDBNODE &at[209]
#define STHISTDBNODE_gi &at[210]
#define STHISTDBNODE_oldgi &at[211]
#define STHISTDBNODE_access &at[212]
#define STHISTDBNODE_version &at[213]
#define STHISTDBNODE_date &at[214]
#define STHISTDBNODE_action &at[215]
#define STHISTDBNODE_filename &at[216]

#define STHISTDB &at[217]
#define STHISTDB_E &at[218]

#define STGODBNODE &at[219]
#define STGODBNODE_llid &at[220]
#define STGODBNODE_goid &at[221]
#define STGODBNODE_pmid &at[222]
#define STGODBNODE_eviCode &at[223]

#define STGODB &at[224]
#define STGODB_E &at[225]

#define STOMIMDBNODE &at[226]
#define STOMIMDBNODE_llid &at[227]
#define STOMIMDBNODE_omimid &at[228]

#define STOMIMDB &at[229]
#define STOMIMDB_E &at[230]

#define STCDDDBNODE &at[231]
#define STCDDDBNODE_llid &at[232]
#define STCDDDBNODE_cddid &at[233]
#define STCDDDBNODE_eValue &at[234]

#define STCDDDB &at[235]
#define STCDDDB_E &at[236]

#define STLLDBNODE &at[237]
#define STLLDBNODE_llid &at[238]
#define STLLDBNODE_npid &at[239]
#define STLLDBNODE_map &at[240]

#define STLLDB &at[241]
#define STLLDB_E &at[242]

#define SHGOREPORT &at[37]
#define SHGOREPORT_num_mol_fun &at[38]
#define SHGOREPORT_num_bio_pro &at[39]
#define SHGOREPORT_num_cell_com &at[40]
#define SHGOREPORT_mol_fun &at[41]
#define SHGOREPORT_bio_pro &at[52]
#define SHGOREPORT_cell_com &at[53]

#define SHLOCUSLINKREPORT &at[58]
#define SHLOCUSLINKREPORT_llid &at[59]
#define SHLOCUSLINKREPORT_locus &at[60]
#define SHLOCUSLINKREPORT_products &at[61]
#define SHLOCUSLINKREPORT_products_E &at[62]

#define SHNEIGHBOUR &at[243]
#define SHNEIGHBOUR_gi &at[244]
#define SHNEIGHBOUR_evalue &at[245]

#define SHGOANNOT &at[42]
#define SHGOANNOT_E &at[43]

#define SHGOANNOTNODE &at[44]
#define SHGOANNOTNODE_goid &at[45]
#define SHGOANNOTNODE_ontology &at[46]
#define SHGOANNOTNODE_goname &at[47]
#define SHGOANNOTNODE_evidence_code &at[48]
#define SHGOANNOTNODE_evidence_code_E &at[49]
#define SHGOANNOTNODE_pmid &at[50]
#define SHGOANNOTNODE_pmid_E &at[51]

#define SHLOCUSPRODUCT &at[63]
#define SHLOCUSPRODUCT_gi &at[64]
#define SHLOCUSPRODUCT_defline &at[65]

#define STDBXREFNODE &at[246]
#define STDBXREFNODE_id &at[247]
#define STDBXREFNODE_sourcedb &at[248]
#define STDBXREFNODE_sourceid &at[249]
#define STDBXREFNODE_recordid &at[250]
#define STDBXREFNODE_parentid &at[251]
#define STDBXREFNODE_link &at[252]
#define STDBXREFNODE_field &at[253]
#define STDBXREFNODE_cv &at[254]

#define STDBXREF &at[255]
#define STDBXREF_E &at[256]

#define SHDBXREFNODE &at[257]
#define SHDBXREFNODE_sourcedb &at[258]
#define SHDBXREFNODE_recordid &at[259]
#define SHDBXREFNODE_field &at[260]
#define SHDBXREFNODE_cv &at[261]

#define SHDBXREF &at[262]
#define SHDBXREF_E &at[263]

#define STBIOENTITYNODE &at[264]
#define STBIOENTITYNODE_id &at[265]
#define STBIOENTITYNODE_bioentitytypeid &at[266]
#define STBIOENTITYNODE_db &at[267]
#define STBIOENTITYNODE_access &at[268]
#define STBIOENTITYNODE_identifier &at[269]
#define STBIOENTITYNODE_field &at[270]

#define STBIOENTITY &at[271]
#define STBIOENTITY_E &at[272]

#define STBIONAMENODE &at[273]
#define STBIONAMENODE_id &at[274]
#define STBIONAMENODE_bioentityid &at[275]
#define STBIONAMENODE_name &at[276]
#define STBIONAMENODE_nametypeid &at[277]
#define STBIONAMENODE_db &at[278]
#define STBIONAMENODE_access &at[279]
#define STBIONAMENODE_identifier &at[280]
#define STBIONAMENODE_field &at[281]
#define STBIONAMENODE_official &at[282]
#define STBIONAMENODE_deprecated &at[283]
#define STBIONAMENODE_date &at[284]
#define STBIONAMENODE_ruleid &at[285]
#define STBIONAMENODE_action &at[286]
#define STBIONAMENODE_actiondate &at[287]

#define STBIONAME &at[288]
#define STBIONAME_E &at[289]

#define STSECONDREFSNODE &at[290]
#define STSECONDREFSNODE_id &at[291]
#define STSECONDREFSNODE_bionameid &at[292]
#define STSECONDREFSNODE_db &at[293]
#define STSECONDREFSNODE_access &at[294]
#define STSECONDREFSNODE_field &at[295]

#define STSECONDREFS &at[296]
#define STSECONDREFS_E &at[297]

#define STGICHROMIDNODE &at[298]
#define STGICHROMIDNODE_id &at[299]
#define STGICHROMIDNODE_gi &at[300]
#define STGICHROMIDNODE_contiggi &at[301]
#define STGICHROMIDNODE_chromid &at[302]

#define STGICHROMID &at[303]
#define STGICHROMID_E &at[304]

#define STCONTIGCHROMIDNODE &at[305]
#define STCONTIGCHROMIDNODE_id &at[306]
#define STCONTIGCHROMIDNODE_contiggi &at[307]
#define STCONTIGCHROMIDNODE_topgi &at[308]
#define STCONTIGCHROMIDNODE_chromid &at[309]
#define STCONTIGCHROMIDNODE_changed &at[310]

#define STCONTIGCHROMID &at[311]
#define STCONTIGCHROMID_E &at[312]

#define STGICHROMNODE &at[313]
#define STGICHROMNODE_id &at[314]
#define STGICHROMNODE_gi &at[315]
#define STGICHROMNODE_chromnum &at[316]

#define STGICHROM &at[317]
#define STGICHROM_E &at[318]

#define STCONTIGCHROMNODE &at[319]
#define STCONTIGCHROMNODE_id &at[320]
#define STCONTIGCHROMNODE_contiggi &at[321]
#define STCONTIGCHROMNODE_chromnum &at[322]

#define STCONTIGCHROM &at[323]
#define STCONTIGCHROM_E &at[324]

#define STSOURCEFILENODE &at[325]
#define STSOURCEFILENODE_id &at[326]
#define STSOURCEFILENODE_gi &at[327]
#define STSOURCEFILENODE_filename &at[328]
#define STSOURCEFILENODE_date &at[329]

#define STSOURCEFILE &at[330]
#define STSOURCEFILE_E &at[331]

#define STGENEOBJECTNODE &at[332]
#define STGENEOBJECTNODE_id &at[333]
#define STGENEOBJECTNODE_taxid &at[334]
#define STGENEOBJECTNODE_geneid &at[335]
#define STGENEOBJECTNODE_status &at[336]

#define STGENEOBJECT &at[337]
#define STGENEOBJECT_E &at[338]

#define STGENEGINODE &at[339]
#define STGENEGINODE_id &at[340]
#define STGENEGINODE_geneobjectid &at[341]
#define STGENEGINODE_gi &at[342]
#define STGENEGINODE_moleculetype &at[343]

#define STGENEGI &at[344]
#define STGENEGI_E &at[345]

#define STGENEDBXREFNODE &at[346]
#define STGENEDBXREFNODE_id &at[347]
#define STGENEDBXREFNODE_geneobjectid &at[348]
#define STGENEDBXREFNODE_dbname &at[349]
#define STGENEDBXREFNODE_dbaccess &at[350]

#define STGENEDBXREF &at[351]
#define STGENEDBXREF_E &at[352]
