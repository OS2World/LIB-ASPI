/*
 * $Source: r:/source/aspi/RCS/tape.c,v $
 * $Revision $
 * $Date: 1999/08/18 00:19:30 $
 * $Locker:  $
 *
 *	Play with tape device.
 *
 * $Log: tape.c,v $
 * Revision 1.3  1999/08/18 00:19:30  vitus
 * - updated location of defines.h (moved)
 *
 * Revision 1.2  1997/09/22 02:27:32  vitus
 * uses AH*() routines from library
 *
 * Revision 1.1  1997/09/18 01:39:43  vitus
 * Initial revision
 * ----------------------------------------
 * Sample code to demonstrate use of ASPI Interface.
 */
static char const id[]="$Id: tape.c,v 1.3 1999/08/18 00:19:30 vitus Exp $";

#include <stdio.h>
#include <string.h>

#define INCL_DOS
#include <os2.h>

#include "../lib/defines.h"
#include "scsi.h"
#include "srb.h"
#include "aspio.h"



UCHAR	buffer[5000];




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
    APIRET		rc;
    unsigned		ha = -1, target = -1, lun = -1;
    UCHAR	type;				/* see SCSI spec */

    if( argc == 4 )
    {
	sscanf(argv[1], " %u", &ha);
	sscanf(argv[2], " %u", &target);
	sscanf(argv[3], " %u", &lun);
    }
    if( ha > 7  ||  target > 7  ||  lun > 7 )
    {
	fprintf(stderr,
		"Invalid parameter\n"
		"usage: tape <ha> <target> <lun>\n");
	return 1;
    }

    printf("Parameter: %u %u %u\n", ha, target, lun);
    rc = AspiOpen(0);
    if( rc != 0 )
    {
	fprintf(stderr, "AspiOpen - rc %lu (%#lx)\n", rc, rc);
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

	rc = AspiMount(ha, target, lun, 1);
	if( rc != 0 )
	{
	    fprintf(stderr, "AspiMount - rc %lu (%#lx)\n", rc, rc);
	    DumpBuffer(&strLastSense, sizeof(strLastSense));
	    break;
	}

	rc = AspiRewind(ha, target, lun);
	if( rc != 0 )
	{
	    fprintf(stderr, "AspiRewind - rc %lu (%#lx)\n", rc, rc);
	    DumpBuffer(&strLastSense, sizeof(strLastSense));
	    break;
	}

	memset(buffer, 0, sizeof(buffer));
	rc = AspiReadSeq(ha, target, lun, 1, 1, buffer, sizeof(buffer));
	if( rc != 0 )
	{
	    fprintf(stderr, "AspiReadSeq - rc %lu (%#lx)\n", rc, rc);
	    DumpBuffer(&strLastSense, sizeof(strLastSense));
	}
	else
	{
	    printf("Dumping read buffer\n");
	    DumpBuffer(buffer, 512);
	}

	rc = AspiMount(ha, target, lun, 0);
	if( rc != 0 )
	{
	    fprintf(stderr, "AspiMount - rc %lu (%#lx)\n", rc, rc);
	    DumpBuffer(&strLastSense, sizeof(strLastSense));
	    break;
	}
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
