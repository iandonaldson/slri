/* $Id: slri_endec.c,v 1.1.1.3 2002/03/14 09:27:15 root Exp $
***************************************************************************

  Samuel Lunenfeld Research Institute (SLRI)

  File: slri_endec.c
  Description: Endcoding/Decoding routines

  Author: Howard Feldman (feldman@mshri.on.ca)
  Date: March 2002

  Hogue Lab - University of Toronto Biochemistry Department
  Samuel Lunenfeld Research Institute, Mount Sinai Hospital

  Copyright Notice:

  Copyright (C) 2001 Mount Sinai Hospital (MSH)

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

***************************************************************************
*/

#include <ncbi.h>

static Byte dtable[256];

/* encodes a string using Base64 */
CharPtr Base64Encode(CharPtr src,Int2 len)
{
	Int4 i,n;
	Boolean hiteof=FALSE;
	CharPtr dest,pcsrc,pcdest;
	Byte igroup[3],ogroup[4];

	for (i=0;i<9;i++)	{
		dtable[i]='A'+i;
		dtable[i+9]='J'+i;
		dtable[26+i]='a'+i;
		dtable[26+i+9]='j'+i;
	}
	for (i=0;i<8;i++) {
		dtable[i+18]='S'+i;
		dtable[26+i+18]='s'+i;
	}
	for(i=0;i<10;i++) {
		dtable[52+i]='0'+i;
	}
	dtable[62]='+';
	dtable[63]='/';
	/* allocate dest */
	dest=(CharPtr)MemNew((1+4*((len+2)/3))*sizeof(Char));
	pcsrc=src;
	pcdest=dest;
	while (!hiteof) {
		igroup[0]=igroup[1]=igroup[2]=0;
		for(n=0;n<3;n++) {
			if (pcsrc-src>=len) {
				hiteof=TRUE;
				break;
			}
			igroup[n]=(Byte)(*pcsrc);
			pcsrc++;
		}
		if (n>0) {
			ogroup[0]=dtable[igroup[0]>>2];
			ogroup[1]=dtable[((igroup[0]&3)<<4)|(igroup[1]>>4)];
			ogroup[2]=dtable[((igroup[1]&0xF)<<2)|(igroup[2]>>6)];
			ogroup[3]=dtable[igroup[2]&0x3F];
			if (n<3) {
				ogroup[3]='=';
				if (n<2) {
					ogroup[2]='=';
				}
			}
			for (i= 0;i<4;i++) {
				*pcdest=(Char)ogroup[i];
				pcdest++;
			}
		}
	}
	*pcdest='\0';
	return dest;
}

/* returns 0 on error, otherwise length */
Int2 Base64Decode(CharPtr src,CharPtr *retval)
{
	Int4 i,j;
	Byte a[4],b[4],o[3];
	CharPtr dest;
	CharPtr pcsrc,pcdest;

	for (i=0;i<255;i++) {
		dtable[i]=0x80;
	}
	for (i='A';i<='I';i++) {
		dtable[i]=0+(i-'A');
	}
	for (i='J';i<='R';i++) {
		dtable[i]=9+(i-'J');
	}
	for (i='S';i<='Z';i++) {
		dtable[i]=18+(i-'S');
	}
	for (i='a';i<='i';i++) {
		dtable[i]=26+(i-'a');
	}
	for (i='j';i<='r';i++) {
		dtable[i]=35+(i-'j');
	}
	for (i='s';i<='z';i++) {
		dtable[i]=44+(i-'s');
	}
	for (i='0';i<='9';i++) {
		dtable[i]=52+(i-'0');
	}
	dtable['+']=62;
	dtable['/']=63;
	dtable['=']=0;

	dest=(CharPtr)MemNew((1+3*(StringLen(src)+3)/4)*sizeof(Char));
	pcsrc=src;
	pcdest=dest;
	while (TRUE) {
		for(i=0;i<4;i++) {
			if (*pcsrc=='\0') {
				if (i==0) {
					*pcdest='\0';
					*retval=dest;
					return (pcdest-dest);
				}
				/* Input incomplete */
				pcsrc--;
				if (*pcsrc!='=') {
					dest=MemFree(dest);
					return 0;
				}
			}
			if (dtable[*pcsrc]&0x80) {
				/* Illegal character in input */
				i--;
				pcsrc++;
				continue;
			}
			a[i]=(Byte)(*pcsrc);
			b[i]=(Byte)dtable[*pcsrc];
			pcsrc++;
		}
		o[0]=(b[0]<<2)|(b[1]>>4);
		o[1]=(b[1]<<4)|(b[2]>>2);
		o[2]=(b[2]<<6)|b[3];
		i=a[2]=='='?1:(a[3]=='='?2:3);
		for (j=0;j<i;j++) {
			*pcdest=(Char)o[j];
			pcdest++;
		}
		if (i<3) {
			*pcdest='\0';
			*retval=dest;
			return (pcdest-dest);
		}		
	}
}

/* UUENCODE/DECODE (adapted by Howard Feldman for TRADES)
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#define ENC(c) ((Char)((((c) & 077) + ' ' == '\"') ? '{' : ((c) ? ((c) & 077) + ' ': '`')))
#define DEC(c)  ((Char)(((c)=='{') ? (('\"' - ' ') & 077) : (((c) - ' ') & 077)))

/*
 * output one group of 3 bytes, pointed at by p, appends to buf
 */
void outenc(CharPtr p,CharPtr buf)
{
	Int2 c1, c2, c3, c4;
	Char pcadd[5];

	c1 = *p >> 2;
	c2 = ((*p << 4) & 060) | ((p[1] >> 4) & 017);
	c3 = ((p[1] << 2) & 074) | ((p[2] >> 6) & 03);
	c4 = p[2] & 077;
	pcadd[0]=ENC(c1);
	pcadd[1]=ENC(c2);
	pcadd[2]=ENC(c3);
	pcadd[3]=ENC(c4);
	pcadd[4]='\0';
	StringCat(buf,pcadd);
}

/*
 * copy from in to out, encoding as you go along.
 */
CharPtr UUencode(CharPtr pc, Int2 len)
{
	Int2 i;
	CharPtr buf,encbuf;

	buf=(CharPtr)MemNew((((len+2)/3)*4+1)*sizeof(Char));
	encbuf=(CharPtr)MemNew((len+4)*sizeof(Char));
	MemCopy(encbuf,pc,len*sizeof(Char));
	buf[0]=ENC(len);	
	for (i=0; i<len; i += 3)
		outenc(&encbuf[i], buf);
	encbuf=MemFree(encbuf);
	return buf;
}

/*
 * output a group of 3 bytes (4 input characters).
 * the input chars are pointed to by p, they are to
 * be output to file f.  n is used to tell us not to
 * output all of them at the end of the file.
 */
void outdec(CharPtr p, CharPtr buf, Int2 n, Int2 len)
{
	Int2 c1, c2, c3;

	c1 = DEC(*p) << 2 | DEC(p[1]) >> 4;
	c2 = DEC(p[1]) << 4 | DEC(p[2]) >> 2;
	c3 = DEC(p[2]) << 6 | DEC(p[3]);
	if (n >= 1)
		buf[len-n]=c1;
	if (n >= 2)
		buf[len-n+1]=c2;
	if (n >= 3)
		buf[len-n+2]=c3;
}

/*
 * copy from in to out, decoding as you go along.
 */
Int2 UUdecode(CharPtr buf,CharPtr *retval)
{
	CharPtr bp,bufout;
	Int2 totlen,len, i, expected;

	len=DEC(buf[0]);
	totlen=len;
	if ((len <= 0) || (buf[0] == '\n'))
		return 0;
	/* Calculate expected # of chars and pad if necessary */
	expected = ((len+2)/3)<<2;
	bufout=(CharPtr)MemNew((totlen+1)*sizeof(Char));
	for (i = StringLen(buf); i <= expected; i++)
		buf[i] = ' ';
	bp = &buf[1];
	while (len > 0) {
		outdec(bp, bufout, len, totlen);
		bp += 4;
		len -= 3;
	}
	bufout[totlen]='\0';
	*retval=bufout;
	return totlen;
}

/*
$Log: slri_endec.c,v $
Revision 1.1.1.3  2002/03/14 09:27:15  root
Automated import from Sourceforge.net

Revision 1.3  2002/03/14 03:31:27  hfeldman
Fixed potential bug with missing equal sign at end of string

Revision 1.2  2002/03/10 16:54:14  hfeldman
added base64 support for binary

Revision 1.1  2002/03/07 19:32:35  micheld
Initial Import


*/
