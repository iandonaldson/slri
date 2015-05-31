" ---------------------------------------------------------------------------- "
" $Id: .vimrc,v 1.1 2004/12/22 19:31:29 kzheng Exp $
" ---------------------------------------------------------------------------- "
set ic!
syntax on				" syntax highlighting
"set dir=>/var/tmp/vi.recover		" dir for swapfiles
set nowrap
set ruler				" see statusline
set tabstop=4
set shiftwidth=4
set expandtab  "enter a real tab character use Ctrl-V<Tab> key sequence.
set list listchars=tab:»·,trail:·  "highlight tab and $
set softtabstop=4   " makes the spaces feel like real tabs
set backspace=2

set showmode
set showcmd
set splitbelow				" new window at bottom
set noerrorbells
set vb t_vb=				" shut up
set laststatus=2
set noignorecase
set shortmess=atI
set showmatch				" No thanks!

set nocompatible
set comments=b:#,:%,fb:-,n:>,n:)
set magic
set shell=/usr/bin/bash
set number				" numbering lines
"set showbreak=\ \ \ \ \ \ \ \
set autoindent
set sidescroll=1
set smartindent
set cindent
set cinoptions=>s,e0,n0,f0,{0,}0,^0,:0,=s,ps,ts,c1,+s,(2s,us,)20,*30,gs,hs
set smarttab
set viminfo=%,'50,\"1000,:100,/50,n~/.viminfo
set errorformat=%f:%l:%c:%t:%m
set autowrite
set makeef=/tmp/vim.err
set history=250
set helpheight=16
set tags=./tags,tags
set nobackup				" no backup files
"set writebackup
set backupdir=.,/var/tmp/vi.backup,~/,/tmp
"set patchmode=.orig
"set wildmenu
set ttyfast
set swapsync=fsync
set suffixes=.o,.bak,.BAK,.orig
set verbose=0
set undolevels=8192
set filetype=on

autocmd BufNewFile,BufRead *.c,*.h	call FT_c()

" Filename
hi User1 ctermfg=black       ctermbg=gray
" Modified Flag
hi User2 ctermfg=darkred     ctermbg=gray
" ReadOnly Flag
hi User3 ctermfg=darkred     ctermbg=gray
" MODE
hi User4 ctermfg=black       ctermbg=gray
" Lineno
hi User5 ctermfg=black       ctermbg=gray
" Percent
hi User6 ctermfg=black       ctermbg=gray
" UNUSED
hi User7 ctermfg=darkgreen   ctermbg=gray
" Character
hi User8 ctermfg=black       ctermbg=gray
" Background
hi User9 ctermfg=black       ctermbg=gray

set statusline=%<%1*%-16.20f%2*%m%3*%r%9*%=%-3(%8*%.3b%)\ 0x%-3.3B%5*%6.6l,%-4.4c%-6.12o%06(%6*%-p%%%)

""""""""""""""""""""
" Using vim within pine
au BufNewFile,BufRead	/var/tmp/pico.*	set textwidth=72 nowritebackup nobackup formatoptions+=tcq


"""""""""""""""""
" Colors
"
" red=1, green=2, yellow=3, blue=4, magenta=5, cyan=6, white=7 

"highlight Comment term=bold ctermfg=3 ctermbg=0
highlight cComment term=bold ctermfg=6 ctermbg=0


" Function Key Mappings
unmap <F1>
map <F1>	:<C-M>
map <F2>	:w<C-M>
map <F3>	:u<C-M>
map <F4>	:redo<C-M>
map <F5>	:set nowrap<C-M>
map <F6>	:set wrap<C-M>
map <F7>	:cq<C-M>
map <F8>	:w!<C-M>
map <F9>	:q<C-M>
map <F10>	:q!<C-M>
map <F11>	:wq<C-M>
map <F12>	:wq!<C-M>

"map @r 0O....+....1....+....2....+....3....+....4....+....5....+....6....+....7....+....8

" Delete All Spaces
map DS	:%s/[ <Tab>]//g<C-M>
" Delete Empty Lines
map DR	:g/^$/d<C-M>
" Delete Empty Lines + Lines w/whitespace only
map DE	:g/^[<C-I> ]*$/d<C-M>
" Change Lines w/WhiteSpace only to \n
map DW	:%s/^[<C-I> ]*$//g<C-M>
" Insert current time
map DT	o<C-R>=strftime("%a %b %d %T %Z %Y")<C-M><C-M>

"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
""" C Programming - FileType *.c/*.h
"
function FT_c()
	" (C)
	map ID	o/*
* Copyright (C) 2000 by Ron Scott <ron@HostUniverse.net>
* All rights reserved.
*
* $Id: .vimrc,v 1.1 2004/12/22 19:31:29 kzheng Exp $
*/
7k$i<ESC>dd:7<C-M>
	" surround line with /* */
	map CS	I/*A*/
	" C start
	map ST	o/*
* $Id: .vimrc,v 1.1 2004/12/22 19:31:29 kzheng Exp $
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char **argv)
{


return 0;
}4k$i
	" simple main()
	map MN	oint main(int argc, char **argv)
{
<Tab>

return 0;
}3k$i
	" for loop
	map FR	ofor (i = 0; i < x; i++) {
}O
	" while loop
	map WH	owhile () {
}O
	" do/while loop
	map DW	odo {
} while ();O
	" switch
	map SW	oswitch () {
}Ocase 0:
break;
default:
break;2k$i
	" getopt
	map GO	owhile ((ch = getopt(argc, argv, "hv")) != -1) {
switch (ch) {
case 'v':
break;
case '?':
case 'h':
break;
default:
break;
}
}
argc -= optind;
argv += optind;
11k$i
	" fprintf
	map FF	ofprintf(stdout, "\n");
	map FE	ofprintf(stderr, "\n");
	" malloc/calloc
	map MA	oif ((buf = malloc()) == NULL) {
perror("malloc");
exit(1);
}
	map CA	oif ((buf = calloc(,sizeof())) == NULL) {
perror("calloc";
exit(1);
}
	" open/fopen/popen
	map FD	oif ((fd = open(filename, O_WRONLY\|O_CREAT\|O_TRUNC)) == -1) {
perror("open");
exit(1);
}
4k$i
	map FP	oif ((fp = fopen(filename, "r")) == NULL) {
perror("fopen");
exit(1);
}
4k$i
	map PP	oif ((pp = popen(progname, "r")) == NULL) {
perror("popen");
exit(1);
}
4k$i
	"	C beautify the file...
	map <C-i>	:%!indent -nv<C-M>
	"	...or indent it
	"map IND	:!indent %
	"
	"map <C-w>	:cp<C-M>
	"map <C-e>	:cn<C-M>
	map <C-a>	:cp
	map <C-e>	:cn
	" CTAGS
	map <C-t>	:!(cd %:p:h;/usr/local/bin/ctags *.[ch])&
	" Insert closing brackets
	":inoremap { {<C-M>}<ESC>i
	""""""""""""""""""""""""""""""""""""""""""""""
	" HTML SPECIFIC
	map TT	o(void) fprintf(stdout, "<TABLE WIDTH=\"100%%\">\n");
	map TD	o(void) fprintf(stdout, " <TR>\n");<C-M>(void) fprintf(stdout, "   <TD></TD>\n");<C-M>(void) fprintf(stdout, " </TR>\n");
	map FR	o(void) fprintf(stdout, "<FORM NAME=\"\" METHOD=\"\" ACTION=\"\">\n");
	map IR	o<INPUT TYPE=\"radio\" NAME=\"\" VALUE=\"\">
	map IT	o<INPUT TYPE=\"text\" NAME=\"\" SIZE=\"\" MAXLENGTH=\"\">
	map IS	o<SELECT NAME=\"\"></SELECT>
	map IX	o<TEXTAREA NAME=\"\" COLS=\"\" ROWS=\"\"></TEXTAREA>
	" COPYRIGHT
	map CP	:r /etc/COPYRIGHT
:0
dd
endfunction

" Makefile
map OF	o-O2 -mcpu=k6 -march=k6 -pipe -fexpensive-optimizations
map WF	o-W -Wall -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align -Wwrite-strings -Wconversion -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Wnested-externs -Winline -Wundef -Wbad-function-cast -Waggregate-return
map CF	o-O2 -mcpu=k6 -march=k6 -pipe -fexpensive-optimizations -W -Wall -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align -Wwrite-strings -Wconversion -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Wnested-externs -Winline
map DF	o-D_POSIX_SOURCE -D_XOPEN_SOURCE
"map XX	o-Wenum-clash -Woverloaded-virtual

" Yep, and finally
set secure

"
" EOF
"

" sample quickfix script for bsd make - needs ccfilter (www.FreshMeat.net ??)

"#!/bin/ksh
"# ---------------------------------------------------------------------------- #
"# $Id: .vimrc,v 1.1 2004/12/22 19:31:29 kzheng Exp $
"# ---------------------------------------------------------------------------- #
"
"ERRORFILE=""		# if empty, defaults to ./errors.err
"CCFARGS=""		# other aruments to ccfilter
"
"# ---------------------------------------------------------------------------- #
"
"#ulimit -c 0
"
"ARGS="$@"
"
"# ---------------------------------------------------------------------------- #
"
"if [[ -z ${ERRORFILE} ]]; then
"	EFILE="errors.err"
"else
"	CCFARGS="${CFARGS} -f ${ERRORFILE}"
"	EFILE=${ERRORFILE}
"fi
"
"# ---------------------------------------------------------------------------- #
"
"function cleanup
"{
"#	[[ -z ${WRKSRC} ]] || unset WRKSRC
"	[[ -f ${EFILE} ]] && rm -f ${EFILE} > /dev/null 2>&1
"}
"
"trap "cleanup; exit 2" 1 2 15
"
"#export WRKSRC=$(make -V WRKSRC)
"
"function compile
"{
"	make ${ARGS} 2>&1 | tee $(tty) | ccfilter ${CCFARGS}
"	[[ -s ${EFILE} ]] && return 1
"	return 0
"}
"
"function editfile
"{
"	echo ""
"	read JUNK
"	vim -q ${EFILE} && return 1
"	return 0
"}
"
"until compile ; do
"	editfile && break
"done
"
"cleanup
"
"exit 0
"
"# ---------------------------------------------------------------------------- #
"#
"# EOF
"#
