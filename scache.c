/*
 * $Source: r:/source/aspi/RCS/scache.c,v $
 * $Revision $
 * $Date: 1999/08/18 00:20:01 $
 * $Locker:  $
 *
 *	Change performance settings of SCSI devices.
 *
 * $Log: scache.c,v $
 * Revision 1.6  1999/08/18 00:20:01  vitus
 * - updated location of defines.h (moved)
 * - changed function comments to new layout
 *
 * Revision 1.5  1998/11/07 21:21:00  vitus
 * - added missing include<string.h>
 *
 * Revision 1.4  1998/03/03 03:12:22  vitus
 * - added modification of CD-ROM spin down (parameter '-s')
 *
 * Revision 1.3  1997/09/22 02:26:17  vitus
 * commented
 *
 * Revision 1.2  1997/09/21 03:50:52  vitus
 * modifies control page and disconnect page, too
 * new command line interface
 *
 * Revision 1.1  1997/09/19 23:37:22  vitus
 * Initial revision
 * ----------------------------------------
 * Sample code to demonstrate use of ASPI Interface.
 */
static char const id[]="$Id: scache.c,v 1.6 1999/08/18 00:20:01 vitus Exp $";

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define INCL_DOS
#include <os2.h>

#include "../lib/defines.h"
#include "scsi.h"
#include "srb.h"
#include "aspio.h"



PUBLIC char	szPrgName[_MAX_PATH];

PRIVATE struct {
    int		quiet : 1;			/* 1: don't dump so many bytes */
} hsFlags;


/*
 * Meanings of all SCSI releated bit fields:
 * 0	don't change this feature
 * 1	disable feature
 * 2	enable feature
 */
PRIVATE struct {
    int		access : 1;			/* 1: display them at least */
    int		wce : 3;
    int		rce : 3;
    int		pre : 3;
    int		prio : 3;
} hsCache = {0};

PRIVATE struct {
    int		access : 1;
    int		queue : 3;
    int		reorder : 3;
} hsControl = {0};

PRIVATE struct {
    int		access : 1;
    int		buffer : 3;
    int		inactive : 3;
    int		disconnect : 3;
    int		connect : 3;
} hsDisconnect = {0};

PRIVATE struct {
    int		access : 1;
    USHORT	timeout;			/* [seconds], 0 = no change */
} hsSpin = {0};

PRIVATE UCHAR	bAdp;
PRIVATE UCHAR	bTarget;
PRIVATE UCHAR	bLUN;
PRIVATE UCHAR	abData[5000];			/* to read/write pages */






PRIVATE void
DumpBuffer(PVOID const arg1,ULONG const len)
{
    ULONG i;
    PUCHAR p = arg1;

    for( i = 0; i < len; ++i, ++p )
    {
	printf("%02X", *p);
	if( ((i + 1) % 16) == 0 )
	    printf("\n");
	else if( ((i + 1) % 8) == 0 )
	    printf("-");
	else
	    printf(" ");
    }
    putchar('\n');
}




/*# ----------------------------------------------------------------------
 * Usage(void),	Help(void)
 *
 * PARAMETER
 *	(none)
 *
 * RETURNS
 *	(nothing)
 *
 * GLOBAL
 *	(none)
 *
 * DESPRIPTION
 *	Kind of online help.
 *
 * REMARKS
 */
PRIVATE void
Usage(void)
{
    printf("%s [-?] [<settings>] <ha> <target> <lun>\n", szPrgName);
}

PRIVATE void
Help(void)
{
    Usage();
    printf("\n -?\t\tthis text\n");
    printf(" -v\t\tquiet\n");
    printf(" +c:<string>\tenable cache page settings\n"
	   " -c:<string>\tdisable cache page settings\n"
	   "\t\tw\tenable/disable write cache\n"
	   "\t\tr\tenable/disable read cache\n"
	   "\t\tp\tenable/disable pre-fetch (read-ahead)\n"
	   "\t\ti\tmake pre-fetch more/less important\n");
    printf(" +q:<string>\tenable control page settings\n"
	   " -q:<string>\tdisable control page settings\n"
	   "\t\tq\tenable/disable tagged queuing\n"
	   "\t\tr\tenable/disable reordering\n");
    printf(" +d:<string>\tenable disconnect-reconnect page settings\n"
	   " -d:<string>\tdisable disconnect-reconnect page settings\n"
	   "\t\tb\tenable/disable buffer full ratio\n"
	   "\t\ti\tenable/disable bus inactivity limit\n"
	   "\t\td\tenable/disable disconnect limit\n"
	   "\t\tc\tenable/disable connect limit\n");
    printf(" -s:<seconds>\tchange timeout until CD-ROM stops spinning\n");
    printf("\nUse empty <string> to display page\n");
}




/*# ----------------------------------------------------------------------
 * ExamineArgs(argc,argv)
 *
 * PARAMETER
 *	argc,argv	see main()
 *
 * RETURNS
 *	(nothing)
 *
 * GLOBAL
 *	(nearly all)
 *
 * DESPRIPTION
 *	Analyses command line switches and arguments and updates
 *	global variables to reflect them.
 *
 * REMARKS
 */
PRIVATE void
ExamineArgs(int argc,char *argv[])
{
    int		i;
    unsigned	a = -1, t = -1, l = -1;


    strcpy(szPrgName, argv[0]);

    while( argc > 1  &&  (argv[1][0] == '-' ||  argv[1][0] == '+') )
    {
	switch( argv[1][1] )
	{
	  case '?':
	    Help();
	    exit(0);

	  case 'v':
	    if( argv[1][0] == '-' )
		hsFlags.quiet = 1;
	    else
		hsFlags.quiet = 0;
	    break;

	  case 'c':
	    hsCache.access = 1;
	    if( argv[1][2] == ':' )
		for( i = 3; argv[1][i] != '\0'; ++i )
		{
		    switch( argv[1][i] )
		    {
		      case 'w':
			hsCache.wce = (argv[1][0] == '-' ? 1 : 2);
			break;
		      case 'r':
			hsCache.rce = (argv[1][0] == '-' ? 1 : 2);
			break;
		      case 'p':
			hsCache.pre = (argv[1][0] == '-' ? 1 : 2);
			break;
		      case 'i':
			hsCache.prio = (argv[1][0] == '-' ? 1 : 2);
			break;
		      default:
			Usage();
			exit(1);
		    }
		}
	    break;

	  case 'q':
	    hsControl.access = 1;
	    if( argv[1][2] == ':' )
		for( i = 3; argv[1][i] != '\0'; ++i )
		{
		    switch( argv[1][i] )
		    {
		      case 'q':
			hsControl.queue = (argv[1][0] == '-' ? 1 : 2);
			break;
		      case 'r':
			hsControl.reorder = (argv[1][0] == '-' ? 1 : 2);
			break;
		      default:
			Usage();
			exit(1);
		    }
		}
	    break;

	  case 'd':
	    hsDisconnect.access = 1;
	    if( argv[1][2] == ':' )
		for( i = 3; argv[1][i] != '\0'; ++i )
		{
		    switch( argv[1][i] )
		    {
		      case 'b':
			hsDisconnect.buffer = (argv[1][0] == '-' ? 1 : 2);
			break;
		      case 'i':
			hsDisconnect.inactive = (argv[1][0] == '-' ? 1 : 2);
			break;
		      case 'd':
			hsDisconnect.disconnect = (argv[1][0] == '-' ? 1 : 2);
			break;
		      case 'c':
			hsDisconnect.connect = (argv[1][0] == '-' ? 1 : 2);
			break;
		      default:
			Usage();
			exit(1);
		    }
		}
	    break;

	  case 's':
	    hsSpin.access = 1;
	    if( argv[1][2] == ':' )
	    {
		hsSpin.timeout = 0;
		sscanf(&argv[1][3], "%hu", &hsSpin.timeout);
		if( hsSpin.timeout == 0 )
		{
		    Usage();
		    exit(1);
		}
	    }
	    break;

	  default:
	    Usage();
	    exit(1);
	}
	--argc;
	++argv;
    }


    if( argc == 4 )
    {
	sscanf(argv[1], " %u", &a);
	sscanf(argv[2], " %u", &t);
	sscanf(argv[3], " %u", &l);
    }
    if( a > 7  ||  t > 7  ||  l > 7 )
    {
	Usage();
	exit(1);
    }
    bAdp = a;
    bTarget = t;
    bLUN = l;

    return;
}




/*# ----------------------------------------------------------------------
 * ModifyCachePage(void)
 *
 * PARAMETER
 *	(none)
 *
 * RETURNS
 *	APIRET
 *
 * GLOBAL
 *	bAdp,bTarget,bLUN
 *	abData
 *	hsControl
 *
 * DESPRIPTION
 *	Try to check/modify page 8 (caching).
 *	Things to be modified are coded in 'hsCache'.
 *
 * REMARKS
 */
PRIVATE APIRET
ModifyCachePage(void)
{
    APIRET	rc;
    unsigned const	pgno = 8;
    UCHAR		mdlen;			/* mode data length */
    PSCSI_MODEPAGE_8	page;
    int			modify = 0;		/* write new value? */


    printf("---- Mode Page %u\tcache page ----\n", pgno);

    do
    {
	rc = AspiModeSense(bAdp, bTarget, bLUN, pgno, 0, abData, 250);
	if( rc != 0 )
	{
	    fprintf(stderr, "\nAspiModeSense - rc %lu (%#lx)\n", rc, rc);
	    DumpBuffer(&strLastSense, sizeof(strLastSense));
	    AHSense(abData, &strLastSense),	printf(abData);
	    break;				/* skip this page */
	}

	page = (PVOID)&abData[sizeof(SCSI_MODEPAGE_HDR)
			     +sizeof(SCSI_MODEPAGE_DESCR)];

	mdlen = abData[0] + 1;		/* 'does not include itself' */
	if( hsFlags.quiet == 0 )
	    DumpBuffer(abData, mdlen);
	if( (page->pcode & 0x7F) != 0x08 )
	{
	    printf("Unknown format!\n");
	    printf("Dumping complete buffer for mode page %u\n", pgno);
	    DumpBuffer(abData, mdlen);
	    break;				/* skip this page */
	}

	printf("Dumping mode page %u (current values)\n", pgno);
	DumpBuffer(page, sizeof(*page));

	if( hsCache.rce == 1  &&  (page->cache & 0x01) == 0 )
	{
	    page->cache |= 0x01;		/* disable read cache */
	    ++modify;
	}
	else if( hsCache.rce == 2  &&  (page->cache & 0x01) != 0 )
	{
	    page->cache &= ~0x01;		/* don't disable read cache */
	    ++modify;
	}

	if( hsCache.wce == 1  &&  (page->cache & 0x04) != 0 )
	{
	    page->cache &= ~0x04;		/* don't enable write cache */
	    ++modify;
	}
	else if( hsCache.wce == 2  &&  (page->cache & 0x04) == 0 )
	{
	    page->cache |= 0x04;		/* enable write cache */
	    ++modify;
	}

	if( hsCache.prio == 1  &&  page->priority != 0 )
	{
	    page->priority = 0;			/* don't distinguish */
	    ++modify;
	}
	else if( hsCache.prio == 2  &&  page->priority != 0x11 )
	{
	    page->priority = 0x11;		/* keep prefetched data longer */
	    ++modify;
	}

	if( hsCache.pre == 1
	    &&  (page->disprefetch[0] | page->disprefetch[1]) != 0 )
	{
	    /* Disable all pre-fetches */

	    page->disprefetch[0] = 0;
	    page->disprefetch[1] = 0;		/* LSB */
	    page->minprefetch[0] = 0;
	    page->minprefetch[1] = 0;		/* LSB */
	    page->maxprefetch[0] = 0;
	    page->maxprefetch[1] = 0;		/* LSB */
	    page->prefetchceiling[0] = 0;
	    page->prefetchceiling[1] = 0;	/* LSB */
	    ++modify;
	}
	else if( hsCache.pre == 2
		 &&  (page->disprefetch[0] | page->disprefetch[1]) == 0 )
	{
	    /* Enable pre-fetch but disable it
	     * on reads with more than 8KB data. */

	    page->cache &= ~0x02;		/* no multipliers, real counts */
	    page->disprefetch[0] = 0;
	    page->disprefetch[1] = 32;		/* LSB */
	    page->minprefetch[0] = 0;
	    page->minprefetch[1] = 1;		/* LSB */
	    page->maxprefetch[0] = 0;
	    page->maxprefetch[1] = 16;		/* LSB */
	    page->prefetchceiling[0] = 0;
	    page->prefetchceiling[1] = 16;	/* LSB */
	    ++modify;
	}

	if( modify )
	{
	    page->pcode &= 0x3f;		/* mask reserved bits */
	    abData[0] = 0;			/* reserved in MODE SELECT */

	    printf("Dumping new mode page %u and header\n", pgno);
	    DumpBuffer(abData, mdlen);
	    rc = AspiModeSelect(bAdp, bTarget, bLUN, 0, abData, mdlen);
	    if( rc != 0 )
	    {
		fprintf(stderr, "\nAspiModeSelect - rc %lu (%#lx)\n", rc, rc);
		DumpBuffer(&strLastSense, sizeof(strLastSense));
		AHSense(abData, &strLastSense),	printf(abData);
		break;				/* skip this page */
	    }
	}
    }
    while( 0 );
    return rc;
}




/*# ----------------------------------------------------------------------
 * ModifyControlPage(void)
 *
 * PARAMETER
 *	(none)
 *
 * RETURNS
 *	APIRET
 *
 * GLOBAL
 *	bAdp,bTarget,bLUN
 *	abData
 *	hsControl
 *
 * DESPRIPTION
 *	Try to check/modify page A (control).
 *	Things to be modified are coded in 'hsControl'.
 *
 * REMARKS
 */
PRIVATE APIRET
ModifyControlPage(void)
{
    APIRET	rc;
    unsigned const	pgno = 0x0A;
    UCHAR		mdlen;			/* mode data length */
    PSCSI_MODEPAGE_A	page;
    int			modify = 0;		/* write new value? */


    printf("---- Mode Page %u\tcontrol page ----\n", pgno);

    do
    {
	rc = AspiModeSense(bAdp, bTarget, bLUN, pgno, 0, abData, 250);
	if( rc != 0 )
	{
	    fprintf(stderr, "\nAspiModeSense - rc %lu (%#lx)\n", rc, rc);
	    DumpBuffer(&strLastSense, sizeof(strLastSense));
	    AHSense(abData, &strLastSense),	printf(abData);
	    break;				/* skip this page */
	}

	page = (PVOID)&abData[sizeof(SCSI_MODEPAGE_HDR)
			     +sizeof(SCSI_MODEPAGE_DESCR)];

	mdlen = abData[0] + 1;		/* 'does not include itself' */
	if( hsFlags.quiet == 0 )
	    DumpBuffer(abData, mdlen);
	if( (page->pcode & 0x7F) != 0x0A )
	{
	    printf("Unknown format!\n");
	    printf("Dumping complete buffer for mode page %u\n", pgno);
	    DumpBuffer(abData, mdlen);
	    break;				/* skip this page */
	}

	printf("Dumping mode page %u (current values)\n", pgno);
	DumpBuffer(page, sizeof(*page));

	if( hsControl.queue == 1  &&  (page->queuing & 0x01) == 0 )
	{
	    page->queuing |= 0x01;		/* disable tagged queuing */
	    ++modify;
	}
	else if( hsControl.queue == 2  &&  (page->queuing & 0x01) != 0 )
	{
	    page->queuing &= ~0x01;		/* don't disable TQ */
	    ++modify;
	}

	if( hsControl.reorder == 1  &&  (page->queuing & 0xF0) == 0x10 )
	{
	    page->queuing &= 0x0F;		/* 'restricted reordering */
	    ++modify;
	}
	else if( hsControl.reorder == 2  &&  (page->queuing & 0xF0) != 0x10 )
	{
	    page->queuing &= 0x0F;
	    page->queuing |= 0x10;		/* 'unrestricted reordering' */
	    ++modify;
	}

	if( modify )
	{
	    page->pcode &= 0x3f;		/* mask reserved bits */
	    abData[0] = 0;			/* reserved in MODE SELECT */

	    printf("Dumping new mode page %u and header\n", pgno);
	    DumpBuffer(abData, mdlen);
	    rc = AspiModeSelect(bAdp, bTarget, bLUN, 0, abData, mdlen);
	    if( rc != 0 )
	    {
		fprintf(stderr, "\nAspiModeSelect - rc %lu (%#lx)\n", rc, rc);
		DumpBuffer(&strLastSense, sizeof(strLastSense));
		AHSense(abData, &strLastSense),	printf(abData);
		break;				/* skip this page */
	    }
	}
    }
    while( 0 );
    return rc;
}




/*# ----------------------------------------------------------------------
 * ModifyDisconnectPage(void)
 *
 * PARAMETER
 *	(none)
 *
 * RETURNS
 *	APIRET
 *
 * GLOBAL
 *	bAdp,bTarget,bLUN
 *	abData
 *	hsDisconnect
 *
 * DESPRIPTION
 *	Try to check/modify page 2 (disconnect-reconnect).
 *	Things to be modified are coded in 'hsDisconnect'.
 *
 * REMARKS
 */
PRIVATE APIRET
ModifyDisconnectPage(void)
{
    APIRET	rc;
    unsigned const	pgno = 0x02;
    UCHAR		mdlen;			/* mode data length */
    PSCSI_MODEPAGE_2	page;
    int			modify = 0;		/* write new value? */


    printf("---- Mode Page %u\tdisconnect-reconnect page ----\n", pgno);

    do
    {
	rc = AspiModeSense(bAdp, bTarget, bLUN, pgno, 0, abData, 250);
	if( rc != 0 )
	{
	    fprintf(stderr, "\nAspiModeSense - rc %lu (%#lx)\n", rc, rc);
	    DumpBuffer(&strLastSense, sizeof(strLastSense));
	    AHSense(abData, &strLastSense),	printf(abData);
	    break;				/* skip this page */
	}

	page = (PVOID)&abData[sizeof(SCSI_MODEPAGE_HDR)
			     +sizeof(SCSI_MODEPAGE_DESCR)];

	mdlen = abData[0] + 1;		/* 'does not include itself' */
	if( hsFlags.quiet == 0 )
	    DumpBuffer(abData, mdlen);
	if( (page->pcode & 0x7F) != 0x02 )
	{
	    printf("Unknown format!\n");
	    printf("Dumping complete buffer for mode page %u\n", pgno);
	    DumpBuffer(abData, mdlen);
	    break;				/* skip this page */
	}

	printf("Dumping mode page %u (current values)\n", pgno);
	DumpBuffer(page, sizeof(*page));

	if( hsDisconnect.buffer == 1
	    &&  (page->bfull |  page->bempty) != 0 )
	{
	    /* no disconnect because of full/empty buffers */

	    page->bfull = 0;
	    page->bempty = 0;
	    ++modify;
	}
	else if( hsDisconnect.buffer == 2
		 &&  (page->bfull |  page->bempty) == 0 )
	{
	    /* disconnect because of full/empty buffers */

	    page->bfull = 40;			/* ~15% */
	    page->bempty = 40;
	    ++modify;
	}

	if( hsDisconnect.inactive == 1
		 &&  (page->businactive[0] |  page->businactive[1]) != 0 )
	{
	    /* no limit on bus busy time w/o connection */

	    page->businactive[0] = page->businactive[1] = 0;
	    ++modify;
	}
	else if( hsDisconnect.inactive == 2
		 &&  (page->businactive[0] |  page->businactive[1]) == 0 )
	{
	    /* don't keep bus busy w/o connection */

	    page->businactive[0] = 0;
	    page->businactive[1] = 200;		/* LSB */
	    ++modify;
	}

	if( modify )
	{
	    page->pcode &= 0x3f;		/* mask reserved bits */
	    abData[0] = 0;			/* reserved in MODE SELECT */

	    printf("Dumping new mode page %u and header\n", pgno);
	    DumpBuffer(abData, mdlen);
	    rc = AspiModeSelect(bAdp, bTarget, bLUN, 0, abData, mdlen);
	    if( rc != 0 )
	    {
		fprintf(stderr, "\nAspiModeSelect - rc %lu (%#lx)\n", rc, rc);
		DumpBuffer(&strLastSense, sizeof(strLastSense));
		AHSense(abData, &strLastSense),	printf(abData);
		break;				/* skip this page */
	    }
	}
    }
    while( 0 );
    return rc;
}




/*# ----------------------------------------------------------------------
 * ModifySpinning(void)
 *
 * PARAMETER
 *	(none)
 *
 * RETURNS
 *	APIRET
 *
 * GLOBAL
 *	bAdp,bTarget,bLUN
 *	abData
 *	hwSpin
 *
 * DESPRIPTION
 *	Try to modify page 0D (time in hold track state)
 *	Things to be modified are coded in 'hsSpin'.
 *
 * REMARKS
 */
PRIVATE APIRET
ModifySpinning(void)
{
    APIRET	rc;
    unsigned const	pgno = 0x0D;
    UCHAR		mdlen;			/* mode data length */
    PSCSI_MODEPAGE_D	page;
    int			modify = 0;		/* write new value? */


    printf("---- Mode Page %u\tCD-ROM parameters page ----\n", pgno);

    do
    {
	rc = AspiModeSense(bAdp, bTarget, bLUN, pgno, 0, abData, 250);
	if( rc != 0 )
	{
	    fprintf(stderr, "\nAspiModeSense - rc %lu (%#lx)\n", rc, rc);
	    DumpBuffer(&strLastSense, sizeof(strLastSense));
	    AHSense(abData, &strLastSense),	printf(abData);
	    break;				/* skip this page */
	}

	page = (PVOID)&abData[sizeof(SCSI_MODEPAGE_HDR)
			     +sizeof(SCSI_MODEPAGE_DESCR)];

	mdlen = abData[0] + 1;		/* 'does not include itself' */
	if( hsFlags.quiet == 0 )
	    DumpBuffer(abData, mdlen);
	if( (page->pcode & 0x7F) != 0x0D )
	{
	    printf("Unknown format!\n");
	    printf("Dumping complete buffer for mode page %u\n", pgno);
	    DumpBuffer(abData, mdlen);
	    break;				/* skip this page */
	}

	printf("Dumping mode page %u (current values)\n", pgno);
	DumpBuffer(page, sizeof(*page));

	if( hsSpin.timeout != 0 )
	{
	    if( hsSpin.timeout == 1 )
		page->multiplier = 0x04;
	    else if( hsSpin.timeout == 2 )
		page->multiplier = 0x05;
	    else if( hsSpin.timeout <= 4 )
		page->multiplier = 0x06;
	    else if( hsSpin.timeout <= 8 )
		page->multiplier = 0x07;
	    else if( hsSpin.timeout <= 16 )
		page->multiplier = 0x08;
	    else if( hsSpin.timeout <= 32 )
		page->multiplier = 0x09;
	    else if( hsSpin.timeout <= 60 )
		page->multiplier = 0x0A;
	    else if( hsSpin.timeout <= 120 )
		page->multiplier = 0x0B;
	    else if( hsSpin.timeout <= 240 )
		page->multiplier = 0x0C;
	    else if( hsSpin.timeout <= 480 )
		page->multiplier = 0x0D;
	    else if( hsSpin.timeout <= 960 )
		page->multiplier = 0x0E;
	    else if( hsSpin.timeout <= 1920 )
		page->multiplier = 0x0F;
	    ++modify;
	}

	if( modify )
	{
	    page->pcode &= 0x3f;		/* mask reserved bits */
	    abData[0] = 0;			/* reserved in MODE SELECT */

	    printf("Dumping new mode page %u and header\n", pgno);
	    DumpBuffer(abData, mdlen);
	    rc = AspiModeSelect(bAdp, bTarget, bLUN, 0, abData, mdlen);
	    if( rc != 0 )
	    {
		fprintf(stderr, "\nAspiModeSelect - rc %lu (%#lx)\n", rc, rc);
		DumpBuffer(&strLastSense, sizeof(strLastSense));
		AHSense(abData, &strLastSense),	printf(abData);
		break;				/* skip this page */
	    }
	}
    }
    while( 0 );
    return rc;
}






PUBLIC int
main(int argc,char *argv[])
{
    APIRET	rc;
    UCHAR	type;				/* see SCSI spec */

    ExamineArgs(argc, argv);

    rc = AspiOpen(0);
    if( rc != 0 )
    {
	fprintf(stderr, "\nAspiOpen - rc %lu\n", rc);
	return rc;
    }

    do
    {
	rc = AspiGetType(bAdp, bTarget, bLUN, &type);
	if( rc != 0 )
	{
	    fprintf(stderr, "\nAspiGetType - rc %lu (%#lx)\n", rc, rc);
	    break;
	}

	printf("HA %u  Target %u  LUN %u\t\"%s\" (%s)\n",
	       bAdp, bTarget, bLUN,
	       AHInquiryType(type), AHInquiryQual(type));

	if( hsCache.access )
	    rc = ModifyCachePage();
	if( hsControl.access )
	    rc = ModifyControlPage();
	if( hsDisconnect.access )
	    rc = ModifyDisconnectPage();
	if( hsSpin.access )
	    rc = ModifySpinning();
    }
    while( 0 );

    rc = AspiClose();
    if( rc != 0 )
    {
	fprintf(stderr, "\nAspiClose - rc %lu (%#lx)\n", rc, rc);
	return rc;
    }

    return 0;
}
