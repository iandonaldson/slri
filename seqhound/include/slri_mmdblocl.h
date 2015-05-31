/* 
$Id: slri_mmdblocl.h,v 1.1.1.1 2001/10/11 17:37:13 gbader Exp $
***************************************************************************
  Copyright (C) 2000, 
  Mount Sinai Hospital (MSH). All Rights Reserved. 

  WARNING - THIS FILE CONTAINS TRADE SECRET INFORMATION 
            YOU MAY NOT USE OR POSESS THIS SOURCE CODE 
            OUTSIDE OF MSH WITHOUT PERMISSION GRANTED BY 
            DR. C.W.V. HOGUE and MSH.

  PROGRAM:  TRADES package: foldtraj, maketrj and viewtrj
            MMDB Local Header

  AUTHORS:  
            Howard J. Feldman           (feldman@mshri.on.ca)
            and Christopher W.V. Hogue  (hogue@mshri.on.ca)


  REVISION/CONTRIBUTION NOTES:

  June 1, 2000  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry Department 
                and the Samuel Lunenfeld Research Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:

  Feldman, Howard J. and Hogue, Christopher W.V. (2000)
  A Fast Method to Sample Real Protein Conformational Space.  
  Proteins, 39(2), 112-131.

  LICENSE:
  
  NO LICENSE TO USE, DISTRIBUTE OR COPY THIS FILE IS GRANTED.
  YOU MAY NOT USE THIS FILE OUTSIDE OF THE HOGUE LABORATORY AT MSH.

  ANY USE, REPRODUCTION OR DISTRIBUTION OF THE PROGRAM OUTSIDE OF
  MOUNT SINAI HOSPITAL IS IN VIOLATION OF COPYRIGHT LAW
  AND CONSTITUTES THEFT OF INTELLECTUAL PROPERTY AND IS SUBJECT TO
  CRIMINAL PROSECUTION.

  IN NO EVENT SHALL MOUNT SINAI HOSPITAL OR THE UNIVERSITY OF TORONTO BE 
  LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR 
  CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE 
  OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF MOUNT SINAI HOSPITAL OR
  THE UNIVERSITY OF TORONTO HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 

  MOUNT SINAI HOSPITAL AND THE UNIVERSITY OF TORONTO SPECIFICALLY DISCLAIMS 
  ANY IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
  PURPOSE. THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED 
  HEREUNDER IS PROVIDED "AS IS" WITHOUT WARANTY OF ANY KIND, EXPRESSED OR 
  IMPLIED. MOUNT SINAI HOSPITAL AND THE UNIVERSITY OF TORONTO HAVE NO 
  OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR 
  MODIFICATIONS. 
***************************************************************************
*/


#ifndef SLRI_MMDBLOCL_H
#define SLRI_MMDBLOCL_H

#include <mmdbapi1.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DocUid
#define DocUid Int4
#endif

Boolean LIBCALL IterateMMDBId(void);
void LIBCALL ResetMMDBIterate(void);
Int4 LIBCALL GetMMDBIdCurrent(void);
Boolean LIBCALL MMDBInit (void);
void LIBCALL MMDBFini (void);
BiostrucPtr LIBCALL MMDBBiostrucGet(DocUid uid, Int4 mdlLvl, Int4 maxModels);
DocUid  LIBCALL MMDBEvalPDB (CharPtr str);

#ifdef __cplusplus
}
#endif

#endif /* SLRI_MMDBLOCL_H */

/*  
$Log: slri_mmdblocl.h,v $
Revision 1.1.1.1  2001/10/11 17:37:13  gbader
Automated import from Sourceforge.net

Revision 1.1.1.2  2001/10/10 21:53:47  kaca
initial import

Revision 1.3  2001/09/10 16:57:16  feldman
Renamed mmdbbiostrucget back to normal

Revision 1.2  2001/09/04 21:25:44  michel
Moved mmdblocl.c out of seqhound local library so that it can be directly compiled with the cbmmdb parser
This allows clustlib to use MMDBBiostrucGet with SeqHound (which calls SHoundGet3D)
or without SeqHound (in which case mmdblocl.c must be compiled with application)
SeqHound's MMDBBiostrucGet in mmdblocl.c has been renamed to MMDBBiostrucGetHere

Revision 1.1  2001/03/26 19:46:02  kaca
reorganized headers to support a single header for the local and remote APIs

Revision 1.1  2001/03/15 18:54:51  feldman
removed mmdblocl from all makefiles which dont need it and added
my own mmdblocl.h to avoid using NCBI's which is different


*/
