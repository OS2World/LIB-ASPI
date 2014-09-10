/*
 * $Source: r:/source/aspi/RCS/dispg.c,v $
 * $Revision $
 * $Date: 1999/08/18 00:17:37 $
 * $Locker:  $
 *
 *	Display mode sense page <x>
 *
 * $Log: dispg.c,v $
 * Revision 1.3  1999/08/18 00:17:37  vitus
 * - updated location of defines.h (moved)
 *
 * Revision 1.2  1997/09/22 02:27:07  vitus
 * uses AH*() routines from library
 *
 * Revision 1.1  1997/09/18 01:34:24  vitus
 * Initial revision
 * ----------------------------------------
 * Sample code to demonstrate use of ASPI Interface.
 */
static char const id[]="$Id: dispg.c,v 1.3 1999/08/18 00:17:37 vitus Exp $";

#include <stdio.h>

#define INCL_DOS
#include <os2.h>

#include "../lib/defines.h"
#include "scsi.h"
#include "srb.h"
#include "aspio.h"



UCHAR	data[5000];





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




PUBLIC int
main(int argc,char *argv[])
{
    APIRET	rc;
    unsigned	ha = -1, target = -1, lun = -1;
    unsigned	pgno = -1;
    UCHAR	type;				/* see SCSI spec */

    if( argc == 5 )
    {
	sscanf(argv[1], " %u", &ha);
	sscanf(argv[2], " %u", &target);
	sscanf(argv[3], " %u", &lun);
	sscanf(argv[4], " %u", &pgno);
    }
    if( ha > 7  ||  target > 7  ||  lun > 7  ||  pgno > 0x3F )
    {
	fprintf(stderr,
		"Invalid parameter\n"
		"usage: dispg <ha> <target> <lun> <pgno>\n");
	return 1;
    }

    printf("Parameter: %u %u %u\n", ha, target, lun);
    rc = AspiOpen(0);
    if( rc != 0 )
    {
	fprintf(stderr, "AspiOpen - rc %lu\n", rc);
	return rc;
    }

    do
    {
	rc = AspiGetType(ha, target, lun, &type);
	if( rc != 0 )
	{
	    fprintf(stderr, "AspiGetType - rc %lu (%#lx)\n", rc, rc);
	    break;
	}

	printf("HA %u  Target %u  LUN %u\t\"%s\" (%s)\n",
	       ha, target, lun,
	       AHInquiryType(type), AHInquiryQual(type));

	rc = AspiModeSense(ha, target, lun, pgno, 0, data, 250);
	if( rc != 0 )
	{
	    fprintf(stderr, "AspiModeSense - rc %lu (%#lx)\n", rc, rc);
	    DumpBuffer(&strLastSense, sizeof(strLastSense));
	    AHSense(data, &strLastSense),	printf(data);
	    break;
	}

	printf("Dumping mode sense buffer\n");
	DumpBuffer(data, data[0]);
    }
    while( 0 );

    rc = AspiClose();
    if( rc != 0 )
    {
	fprintf(stderr, "AspiClose - rc %lu (%#lx)\n", rc, rc);
	return rc;
    }

    return 0;
}
