#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>



void printStream ( int sock, char *str )
{
    int size = strlen ( str );
    int rc;
    
    rc = write ( sock, str, size );
    if ( rc < size )
    {
	printf ( "size is differ: rc = %d, size = %d\n", rc, size );
	exit ( 1 );
    }
}


char *convertSpeed ( int size, int usec )
{
    int speed;
    static char convString[100];
    int gbytes = 0, mbytes = 0, kbytes = 0, bytes = 0;
    
    
    speed = (int)(((float)size / (float) usec) * 1000000);
    
    gbytes = speed / (1024 * 1024 * 1024);
    mbytes = speed / (1024 * 1024) % 1024;
    kbytes = speed / 1024 % 1024;
     bytes = speed % 1024;

    if ( gbytes )
    {
      snprintf ( convString, 100, "%dGbytes %03dMbytes %03dKbytes\0", gbytes, mbytes, kbytes );
    }
    else if ( mbytes )
    {
      snprintf ( convString, 100, "%dMbytes %03dKbytes\0", mbytes, kbytes );
    }
    else if ( kbytes )
    {
      snprintf ( convString, 100, "%dKbytes\0", kbytes, bytes );
    }
    else
    {
      snprintf ( convString, 100, "%dbytes\0", bytes );
    }
    
    return convString;
}

void SeqHoundConnect( char *name, char *host, char *inStr, ... )
{
    sockaddr_in sin;
    int rc, slen;
    char *buf, *str, *outStr;
    char *postArgs, *tmp;
    int queryType = 1; // 1 - GET, 2 - POST
    int counter = 0, usecs = 0;
    struct timeval g_stv, g_etv;
    struct timeval stv, etv;
    unsigned int min = 9999999, max = 0, tot_avg = 0, cnt_avg = 0;
    va_list ap;
    int strLen = strlen ( inStr ) + 10240;
    
    if ( !inStr )
	return;
    
    str = (char *) malloc ( strLen );
    
    if ( !str )
	return;

    outStr = (char *) malloc ( strLen );
    
    if ( !outStr )
	return;

    va_start ( ap, inStr );
    vsnprintf ( str, strLen, inStr, ap );
    va_end ( ap );

    if ( !strncasecmp ( str, "POST ", 5 ) )
    {
	postArgs = strchr ( str, '?' );
	if ( postArgs )
	{
	    *postArgs = '\0';
	    postArgs++;
	}
	queryType = 2;
    }

    sin.sin_family = AF_INET;
    sin.sin_port = htons ( 80 );
    sin.sin_addr.s_addr = inet_addr ( host );
    slen = sizeof ( sin );


    buf = (char *) calloc ( 32 * 1024 * 1024, 1 );
    
    if ( buf == NULL )
    {
	printf ( "Cannot allocate memory\n" );
	exit ( 1 );
    }

    strcpy ( outStr, str );
    
    if ( queryType == 2 )
    {
	strcat ( outStr, " HTTP/1.0\r\n" );
	tmp = strchr ( postArgs, ' ' );
	if ( tmp )
	    *tmp = 0;
	sprintf ( outStr + strlen ( outStr ), "Host: %s\r\nContent-Length: %u\r\n\r\n", name, strlen ( postArgs ) );
	strcat ( outStr, postArgs );
	strcat ( outStr, "\r\n" );
    }

    printf ( "Request is [%s]\n\n", outStr );

    gettimeofday ( &g_stv, NULL );
    while ( counter++ < 1 )
    {
	gettimeofday ( &stv, NULL );
        int sock = socket ( PF_INET, SOCK_STREAM, 0 );
	if ( sock < 0 )
        {
    	    perror ( "socket" );
    	    exit ( 1 );
	}
        rc = connect ( sock, (struct sockaddr *)&sin, sizeof ( sin ) );
	if ( rc < 0 )
        {
    	    perror ( "connect" );
    	    exit ( 1 );
	}
	printStream ( sock, outStr );

	rc = read ( sock, buf, 65535 );
	buf[rc] = 0;
	printf ( "buf = %s\n==================\n\n", buf );
	fflush ( stdout );
	close ( sock );
	gettimeofday ( &etv, NULL );
	usecs = ( etv.tv_sec * 1000000 + etv.tv_usec ) - ( stv.tv_sec * 1000000 + stv.tv_usec );
	min = min < usecs ? min : usecs;
	max = max > usecs ? max : usecs;
	tot_avg += usecs;
	cnt_avg++;
    }
    gettimeofday ( &g_etv, NULL );
    usecs = ( g_etv.tv_sec * 1000000 + g_etv.tv_usec ) - ( g_stv.tv_sec * 1000000 + g_stv.tv_usec );
    printf ( "%s: Done in %d secs %d usecs. min = %d usecs, avg = %d usecs, max = %d usecs\n", name, usecs / 1000000, usecs % 1000000, min, tot_avg / cnt_avg, max );
    
    free (   buf  );
    free ( outStr );
    free (   str  );
}


void TestSeqHoundServer ( char *name, char *ip, char *script )
{
    struct timeval stv, etv;
    int usecs = 0;

    gettimeofday ( &stv, NULL );



    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundIsNetEntrezOn HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundNetEntrezInit&appname=shunittest HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundInit&NetEntrezOnToo=FALSE&appname=shunittest HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundExists&gi=230779 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundExists&gi=3000000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundExists&gi=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundExists&gi= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundFindAcc&acc=X88853 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundFindAcc&acc=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundFindAcc&acc= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundFindAccList&pacc=X88853,X88853,X88853 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundFindAccList&pacc=,,, HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundFindAccList&pacc= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundAccFromGi&gi=230779 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundAccFromGi&gi=3000000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundAccFromGi&gi=-1 HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundFindName&name=AF060234 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundFindName&name=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundFindName&name= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundFindNameList&pname=X88853,ECJ224437,AF165032 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundFindNameList&pname=,,, HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundFindNameList&pname= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundAccFromGiList&pgi=230779,230779,230779 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundAccFromGiList&pgi=,,, HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundAccFromGiList&pgi= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundSeqIdFromGi&gi=230779 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundSeqIdFromGi&gi=3000000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundSeqIdFromGi&gi=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundSeqIdFromGi&gi= HTTP/1.0\r\n\r\n", script );


    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundSeqIdFromGiList&pgi=230779,230779,230779 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundSeqIdFromGiList&pgi=,,, HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundSeqIdFromGiList&pgi= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundIsProtein&gi=230779 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundIsProtein&gi=3000000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundIsProtein&gi=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundIsProtein&gi= HTTP/1.0\r\n\r\n", script );


    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundIsNRFirst&gi=1172650 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundIsNRFirst&gi=3000000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundIsNRFirst&gi=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundIsNRFirst&gi= HTTP/1.0\r\n\r\n", script );



    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetDefline&gi=230779 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetDefline&gi=3000000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetDefline&gi=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetDefline&gi= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetDeflineList&pgi=230779,230779,230779 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetDeflineList&pgi=3000000000,230779,-1, HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetDeflineList&pgi=,,, HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetDeflineList&pgi= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHound3DbWhat&mmdbid=11 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHound3DbWhat&mmdbid=11111111111 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHound3DbWhat&mmdbid=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHound3DbWhat&mmdbid= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHound3DExists&mmdbid=11 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHound3DExists&mmdbid=11111111111 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHound3DExists&mmdbid=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHound3DExists&mmdbid= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetBioseq&gi=230779 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetBioseq&gi=3000000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetBioseq&gi=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetBioseq&gi= HTTP/1.0\r\n\r\n", script );


    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetBioseqList&pgi=230779,230779,230779 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetBioseqList&pgi=-1,3000000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetBioseqList&pgi=,,, HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetBioseqList&pgi= HTTP/1.0\r\n\r\n", script );

/*    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetBioseqSet&gi=value HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetBioseqSetList&pgi=value HTTP/1.0\r\n\r\n", script ); */

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetSeqEntry&gi=230779 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetSeqEntry&gi=3000000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetSeqEntry&gi=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetSeqEntry&gi= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetSeqEntryList&pgi=230779 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetSeqEntryList&pgi=3000000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetSeqEntryList&pgi=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetSeqEntryList&pgi= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetFasta&gi=230779 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetFasta&gi=3000000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetFasta&gi=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetFasta&gi= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetFastaList&pgi=230779,230779,230779 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetFastaList&pgi=-1,3000000000,111111111111111111 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetFastaList&pgi=,,, HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetFastaList&pgi= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetGenBankff&gi=230779 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetGenBankff&gi=3000000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetGenBankff&gi=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetGenBankff&gi= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetGenBankffList&pgi=230779,230779,23779 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetGenBankffList&pgi=-1,3000000000,11111111111 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetGenBankffList&pgi=,,, HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetGenBankffList&pgi= HTTP/1.0\r\n\r\n", script );


    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGet3D&mmdbid=11 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGet3D&mmdbid=1111111111111 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGet3D&mmdbid=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGet3D&mmdbid= HTTP/1.0\r\n\r\n", script );

/*    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGet3DfromPdbId&pdbid=value HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGiFrom3D&mmdbid=value&ProteinOnly=value HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGiFrom3DList&pmmdbid=value&ProteinOnly=value HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGiFromPDBchain&pdbcode=value&chain=value HTTP/1.0\r\n\r\n", script ); */
    
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHound3DFromGi&gi=230779 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHound3DFromGi&gi=3000000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHound3DFromGi&gi=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHound3DFromGi&gi= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHound3DFromGiList&pgi=230779,230779,230779 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHound3DFromGiList&pgi=-1,30000000000,11111111111111 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHound3DFromGiList&pgi=,,, HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHound3DFromGiList&pgi= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundDNAFromProtein&gi=230779 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundDNAFromProtein&gi=3000000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundDNAFromProtein&gi=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundDNAFromProtein&gi= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundDNAFromProteinList&pgi=230779,230779,230779 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundDNAFromProteinList&pgi=-1,3000000000,1111111111111 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundDNAFromProteinList&pgi=,,, HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundDNAFromProteinList&pgi= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundProteinFromDNA&gi=230779 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundProteinFromDNA&gi=3000000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundProteinFromDNA&gi=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundProteinFromDNA&gi= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundProteinFromDNAList&pgi=230779,230779,230779 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundProteinFromDNAList&pgi=-1,3000000000,11111111111 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundProteinFromDNAList&pgi=,,, HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundProteinFromDNAList&pgi= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundMuidFromGi&gi=230779 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundMuidFromGi&gi=3000000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundMuidFromGi&gi=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundMuidFromGi&gi= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundMuidFromGiList&pgi=230779,230779,230779 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundMuidFromGiList&pgi=-1,3000000000,11111111111 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundMuidFromGiList&pgi=,,, HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundMuidFromGiList&pgi= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundMuidFrom3D&mmdbid=11 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundMuidFrom3D&mmdbid=11111111111 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundMuidFrom3D&mmdbid=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundMuidFrom3D&mmdbid= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundMuidFrom3DList&pmmdbid=value HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundMuidFrom3DList&pmmdbid=value HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundMuidFrom3DList&pmmdbid=value HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundMuidFrom3DList&pmmdbid=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundTaxIDFromGi&gi=230779 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundTaxIDFromGi&gi=3000000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundTaxIDFromGi&gi=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundTaxIDFromGi&gi= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundTaxIDFromGiList&pgi=230779,230779,230779 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundTaxIDFromGiList&pgi=-1,3000000000,11111111111 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundTaxIDFromGiList&pgi=,,, HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundTaxIDFromGiList&pgi= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundTaxIDFrom3D&mmdbid=11 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundTaxIDFrom3D&mmdbid=11111111111 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundTaxIDFrom3D&mmdbid=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundTaxIDFrom3D&mmdbid= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundTaxIDFrom3DList&pmmdbid=11,11,11 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundTaxIDFrom3DList&pmmdbid=-1,11,11111111111 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundTaxIDFrom3DList&pmmdbid=,,, HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundTaxIDFrom3DList&pmmdbid= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundProteinsFromTaxID&taxid=56636 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundProteinsFromTaxID&taxid=333333333333 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundProteinsFromTaxID&taxid=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundProteinsFromTaxID&taxid= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundProteinsFromTaxIDIII&taxid=56636 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundProteinsFromTaxIDIII&taxid=11111111111111 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundProteinsFromTaxIDIII&taxid=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundProteinsFromTaxIDIII&taxid= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundProteinsFromTaxIDList&ptaxid=56636,56636,56636 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundProteinsFromTaxIDList&ptaxid=-1,11111111111,300000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundProteinsFromTaxIDList&ptaxid=,,, HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundProteinsFromTaxIDList&ptaxid= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundDNAFromTaxID&taxid=56636 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundDNAFromTaxID&taxid=3000000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundDNAFromTaxID&taxid=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundDNAFromTaxID&taxid= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundDNAFromTaxIDIII&taxid=56636 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundDNAFromTaxIDIII&taxid=3000000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundDNAFromTaxIDIII&taxid=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundDNAFromTaxIDIII&taxid= HTTP/1.0\r\n\r\n", script );

/*    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundDNAFromTaxIDList&ptaxid=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundProteinsFromOrganism&taxid=value&chromflags=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundProteinsFromOrganismList&ptaxid=value&chromflags=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundProteinsFromChromosome&chromid=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundProteinsFromChromosomeList&pchromid=value HTTP/1.0\r\n\r\n", script ); */

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundAllGenomes HTTP/1.0\r\n\r\n", script );

/*    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundChromosomeFromGenome&taxid=value&chromflags=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundChromosomeFromGenomeList&ptaxid=value&chromflags=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundDNAFromOrganism&taxid=value&chromflags=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundDNAFromOrganismList&ptaxid=value&chromflags=value HTTP/1.0\r\n\r\n", script ); */

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundDNAFromChromosome&chromid=142 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundDNAFromChromosome&chromid=3000000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundDNAFromChromosome&chromid=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundDNAFromChromosome&chromid= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundDNAFromChromosomeList&pchromid=142,142,142 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundDNAFromChromosomeList&pchromid=-1,3000000000,1111111111111 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundDNAFromChromosomeList&pchromid=,,, HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundDNAFromChromosomeList&pchromid= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundRedundantGroup&gi=230779 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundRedundantGroup&gi=3000000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundRedundantGroup&gi=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundRedundantGroup&gi= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundRedundantGroupList&pgi=230779,230779,230779 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundRedundantGroupList&pgi=-1,3000000000, HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundRedundantGroupList&pgi=,,, HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundRedundantGroupList&pgi= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundRedundantGroupIDFromGI&gi=230779 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundRedundantGroupIDFromGI&gi=3000000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundRedundantGroupIDFromGI&gi=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundRedundantGroupIDFromGI&gi= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundRedundantGroupIDFromGIList&pgi=230779,230779,230779 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundRedundantGroupIDFromGIList&pgi=-1,3000000000, HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundRedundantGroupIDFromGIList&pgi=,,, HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundRedundantGroupIDFromGIList&pgi= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundRedundantGroupFromID&group=610108 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundRedundantGroupFromID&group=3000000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundRedundantGroupFromID&group=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundRedundantGroupFromID&group= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundRedundantGroupFromIDList&pgroup=610108,610108,610108 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundRedundantGroupFromIDList&pgroup=-1,3000000000, HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundRedundantGroupFromIDList&pgroup=,,, HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundRedundantGroupFromIDList&pgroup= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundFirstOfRedundantGroupFromID&group=610108 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundFirstOfRedundantGroupFromID&group=3000000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundFirstOfRedundantGroupFromID&group=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundFirstOfRedundantGroupFromID&group= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundFirstOfRedundantGroupFromIDList&pgroup=610108,610108,610108 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundFirstOfRedundantGroupFromIDList&pgroup=-1,3000000000, HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundFirstOfRedundantGroupFromIDList&pgroup=,,, HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundFirstOfRedundantGroupFromIDList&pgroup= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetTaxChildNodes&taxid=56636 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetTaxChildNodes&taxid=3000000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetTaxChildNodes&taxid=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetTaxChildNodes&taxid= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetTaxChildNodesList&ptaxid=56636,56636,56636 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetTaxChildNodesList&ptaxid=-1,3000000000, HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetTaxChildNodesList&ptaxid=,,, HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetTaxChildNodesList&ptaxid= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetAllTaxProgeny&taxid=56636 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetAllTaxProgeny&taxid=3000000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetAllTaxProgeny&taxid=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetAllTaxProgeny&taxid= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetTaxParent&taxid=56636 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetTaxParent&taxid=3000000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetTaxParent&taxid=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetTaxParent&taxid= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetAllTaxAncestors&taxid=56636 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetAllTaxAncestors&taxid=3000000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetAllTaxAncestors&taxid=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetAllTaxAncestors&taxid= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundIsTaxMerged&taxid=56636 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundIsTaxMerged&taxid=3000000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundIsTaxMerged&taxid=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundIsTaxMerged&taxid= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundIsTaxDeleted&taxid=56636 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundIsTaxDeleted&taxid=3000000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundIsTaxDeleted&taxid=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundIsTaxDeleted&taxid= HTTP/1.0\r\n\r\n", script );

/*    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetTaxNameFromTaxIDByClass&taxid=value&type=value HTTP/1.0\r\n\r\n", script ); */

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetTaxNameFromTaxID&taxid=56636 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetTaxNameFromTaxID&taxid=3000000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetTaxNameFromTaxID&taxid=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetTaxNameFromTaxID&taxid= HTTP/1.0\r\n\r\n", script );

/*    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetTaxLineageFromTaxID&taxid=value&type=value HTTP/1.0\r\n\r\n", script ); */

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetBlastResult&gi1=230779&gi2=230779 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetBlastResult&gi1=230779&gi2=230780 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetBlastResult&gi1=230779&gi2= HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetBlastResult&gi1=&gi2= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetBlastSeqAlign&gi1=230779&gi2=230779 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetBlastSeqAlign&gi1=230779&gi2=230780 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetBlastSeqAlign&gi1=230779&gi2= HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetBlastSeqAlign&gi1=&gi2= HTTP/1.0\r\n\r\n", script );

/*    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundNeighboursFromGiEx&gi=value&evalue=value&bLimit=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundNeighboursFromGi&gi=value&evalue=value&bLimit=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundNeighboursFromGiList&gilist=value&evalue=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundNeighboursFromTaxID&taxid=value&evalue=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundNeighboursOfNeighbours&gi=value&evalue=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundNeighboursOfNeighboursList&gilist=value&evalue=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundNeighboursOfNeighboursFromTaxID&taxid=value&evalue=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHound3DNeighboursFromGiEx&gi=value&evalue=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHound3DNeighboursFromGi&gi=value&evalue=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHound3DNeighboursFromGiList&gilist=value&evalue=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHound3DNeighboursFromTaxID&taxid=value&evalue=value HTTP/1.0\r\n\r\n", script ); */

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHound3DListFromTaxID&taxid=56636 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHound3DListFromTaxID&taxid=3000000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHound3DListFromTaxID&taxid=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHound3DListFromTaxID&taxid= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGiAndNumNeighboursList HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundNumNeighboursInDB HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGiFromGOID&goid=8320 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGiFromGOID&goid=3000000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGiFromGOID&goid=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGiFromGOID&goid= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGiFromGOIDList&vnpgo=8320,8320,8320 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGiFromGOIDList&vnpgo=-1,3000000000, HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGiFromGOIDList&vnpgo=,,, HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGiFromGOIDList&vnpgo= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGOIDFromGi&gi=230779 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGOIDFromGi&gi=3000000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGOIDFromGi&gi=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGOIDFromGi&gi= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGOIDFromGiList&vnpgi=230779,230779,230779 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGOIDFromGiList&vnpgi=-1,3000000000, HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGOIDFromGiList&vnpgi=,,, HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGOIDFromGiList&vnpgi= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGOIDFromRedundantGi&gi=230779 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGOIDFromRedundantGi&gi=3000000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGOIDFromRedundantGi&gi=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGOIDFromRedundantGi&gi= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGOIDFromRedundantGiList&vnpgi=230779 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGOIDFromRedundantGiList&vnpgi=3000000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGOIDFromRedundantGiList&vnpgi=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGOIDFromRedundantGiList&vnpgi= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGOECodeFromGi&gi=230779 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGOECodeFromGi&gi=3000000000 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGOECodeFromGi&gi=-1 HTTP/1.0\r\n\r\n", script );
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGOECodeFromGi&gi= HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGOPMIDFromGi&gi=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGiFromOMIM&omimid=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGiFromOMIMList&vnpomim=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundOMIMFromGi&gi=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundOMIMFromGiList&vnpgi=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGiFromCDDID&cddid=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGiFromCDDIDList&vnpcdd=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundCDDIDFromGi&gi=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundCDDIDFromGiList&vnpgi=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundCDDScoreFromGi&gi=value&cddid=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGiFromLLID&llid=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGiFromLLIDList&vnpll=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundLLIDFromGi&gi=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundLLIDFromGiList&vnpgi=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundLocusFromGi&gi=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGODBGetRecordByReference&refType=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGODBGetChildrenOf&goid=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGODBGetParentOf&goid=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGODBGetAllAncestors&goid=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGODBGetAllChildren&goid=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGODBGetNameByID&goid=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGODBGetClassification&goid=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGODBGetHierarchyLevel&goid=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundAllGenomesWithScientificNames HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundAllGenomesWithNumProteins HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetGisByNumberOfDomains&numdom=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetDomainIdFromLabel&label=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetGisByDomainId&domain_id=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetDomainLabelFromDomainId&accession=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetGisByDomainIdAndEvalue&domain=value&evalue=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetDomainsFromGi&gi=value HTTP/1.0\r\n\r\n", script );

    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundGetDomainsFromGiWithEvalue&gi=value&evalue=value HTTP/1.0\r\n\r\n", script );


/*
    SeqHoundConnect( name, ip,  "%s?fnct=SeqHoundFini HTTP/1.0\r\n\r\n", script );


*/

    gettimeofday ( &etv, NULL );
    usecs = ( etv.tv_sec * 1000000 + etv.tv_usec ) - ( stv.tv_sec * 1000000 + stv.tv_usec );
    printf ( "!!!!!!!!!!!!!!!!!! %s: Test done in %d secs %d usecs. !!!!!!!!!!!!!!!\n", name, usecs / 1000000, usecs % 1000000 );

}



main ( int argc, char **argv )
{
    char *testfile = NULL, *buffer = NULL;
    struct timeval stv, etv;
    int io, usecs, counter = 0, rc = 0;
    unsigned int min = 9999999, max = 0, tot_avg = 0, cnt_avg = 0;

//    SeqHoundConnect( "skinner.blueprint.org", "38.112.109.88",  "GET /cgi-bin/seqhound/seqrem?fnct=SeqHoundAllGenomes HTTP/1.0\r\n\r\n" );
//    SeqHoundConnect( "skinner/fseqrem_prof", "38.112.109.88",  "GET /cgi-bin/fseqrem_prof?fnct=SeqHoundAllGenomes HTTP/1.0\r\n\r\n" );
//    SeqHoundConnect( "skinner.blueprint.org", "38.112.109.88",  "GET /fcgi-bin/fseqrem?fnct=SeqHoundAllGenomes HTTP/1.0\r\n\r\n" );
//    SeqHoundConnect( "skinner.blueprint.org", "38.112.109.88",  "POST /fcgi-bin/fseqrem?fnct=SeqHoundAllGenomes HTTP/1.0\r\n\r\n" );

//    SeqHoundConnect( "hibbert/seqrem", "38.112.109.86",  "GET /cgi-bin/seqhound/seqrem?fnct=SeqHoundAllGenomes HTTP/1.0\r\n\r\n" );
//    SeqHoundConnect( "hibbert/seqrem_mmdb", "38.112.109.86",  "GET /cgi-bin/seqhound/seqrem_mmdb?fnct=SeqHoundAllGenomes HTTP/1.0\r\n\r\n" );

//    SeqHoundConnect( "krusty/seqrem",  "38.112.109.87",  "GET /cgi-bin/seqhound/seqrem?fnct=SeqHoundAllGenomes HTTP/1.1\r\nHost: seqhound2.blueprint.org\r\n\r\n" );
//    SeqHoundConnect( "krusty/seqrem_mmdb",  "38.112.109.87",  "GET /cgi-bin/seqhound/seqrem_mmdb?fnct=SeqHoundAllGenomes HTTP/1.1\r\nHost: seqhound2.blueprint.org\r\n\r\n" );

//    SeqHoundConnect( "deep/seqrem",    "192.197.250.54", "GET /cgi-bin/seqhound/seqrem?fnct=SeqHoundAllGenomes HTTP/1.0\r\n\r\n" );


    TestSeqHoundServer( "skinner.blueprint.org", "38.112.109.88",  "POST /cgi-bin/seqhound/seqrem" );
    TestSeqHoundServer( "skinner.blueprint.org", "38.112.109.88",  "POST /fcgi-bin/fseqrem" );


    exit ( 0 );
    
    if ( argc < 2 )
    {
	printf ( "Format:\n\t%s <filename>\n", argv[0] );
	exit(1);
    }

    testfile = strdup ( argv[1] );


//    if ( ( io = open ( testfile, O_DIRECT | O_CREAT | O_WRONLY, 0664 ) ) == -1 )
    if ( ( io = open ( testfile, O_CREAT | O_WRONLY, 0664 ) ) == -1 )
    {
	perror ( "open" );
	exit ( 1 );
    }
    
    printf ( "io = %d\n", io );
    
    buffer = (char *) malloc ( 1024*1024 );
    
    if ( buffer == NULL )
    {
	printf ( "Cannot allocate memory\n" );
	exit ( 1 );
    }

    while ( counter++ < 1024 )
    {
        gettimeofday ( &stv, NULL );
	rc = write ( io, (void *)buffer, 1024 * 1024 );
	if ( rc == -1 )
	{
	    perror ( "write" );
	    exit ( 1 );
	}
	else if ( rc != 1024 * 1024 )
	{
	    printf ( "rc = %d != 1Mb\n", rc );
	}
	gettimeofday ( &etv, NULL );
	usecs = ( etv.tv_sec * 1000000 + etv.tv_usec ) - ( stv.tv_sec * 1000000 + stv.tv_usec );
	min = min < usecs ? min : usecs;
	max = max > usecs ? max : usecs;
	tot_avg += usecs;
	cnt_avg++;
	printf ( "Iteration %d done in %d secs %d usecs ( %s/sec)\n", counter, usecs / 1000000, usecs % 1000000, convertSpeed ( 1048576, usecs ) );
    }
    printf ( "Done in %d secs %d usecs.\n", tot_avg / 1000000, tot_avg % 1000000 );
    printf ( "Min = %d (%s/sec)\n", min, convertSpeed ( 1048576, min ) );
    printf ( "Max = %d (%s/sec)\n", max, convertSpeed ( 1048576, max ) );
    printf ( "Avg = %d (%s/sec)\n", tot_avg / cnt_avg, convertSpeed ( 1048576, tot_avg / cnt_avg ) );

    close ( io );

    if ( ( io = open ( testfile, O_RDONLY ) ) == -1 )
    {
	perror ( "open read" );
	exit ( 1 );
    }

    max = tot_avg = cnt_avg = counter = 0;
    min = 9999999;

    while ( counter++ < 1024 )
    {
        gettimeofday ( &stv, NULL );
	rc = read ( io, (void *)buffer, 1024 * 1024 );
	if ( rc == -1 )
	{
	    perror ( "read" );
	    exit ( 1 );
	}
	else if ( rc != 1024 * 1024 )
	{
	    printf ( "rc = %d != 1Mb\n", rc );
	}
	gettimeofday ( &etv, NULL );
	usecs = ( etv.tv_sec * 1000000 + etv.tv_usec ) - ( stv.tv_sec * 1000000 + stv.tv_usec );
	min = min < usecs ? min : usecs;
	max = max > usecs ? max : usecs;
	tot_avg += usecs;
	cnt_avg++;
	printf ( "Read iteration %d done in %d secs %d usecs ( %s/sec)\n", counter, usecs / 1000000, usecs % 1000000, convertSpeed ( 1048576, usecs ) );
    }
    printf ( "Done read file in %d secs %d usecs.\n", tot_avg / 1000000, tot_avg % 1000000 );
    printf ( "Min = %d (%s/sec)\n", min, convertSpeed ( 1048576, min ) );
    printf ( "Max = %d (%s/sec)\n", max, convertSpeed ( 1048576, max ) );
    printf ( "Avg = %d (%s/sec)\n", tot_avg / cnt_avg, convertSpeed ( 1048576, tot_avg / cnt_avg ) );

    close ( io );


    free ( buffer );

    
}









