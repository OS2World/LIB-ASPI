/*
 * $Source: r:/source/aspi/RCS/dispdef.c,v $
 * $Revision: 1.2 $
 * $Date: 1999/08/19 00:43:46 $
 * $Locker:  $
 *
 *	Display primary/grown defects.
 *
 * $Log: dispdef.c,v $
 * Revision 1.2  1999/08/19 00:43:46  vitus
 * - more comments
 * - more display output
 *
 * Revision 1.1  1999/08/18 02:00:57  vitus
 * Initial revision
 * ----------------------------------------
 * Sample code to demonstrate use of ASPI Interface.
 */
static char const id[]="$Id: dispdef.c,v 1.2 1999/08/19 00:43:46 vitus Exp $";

#include <stdio.h>

#define INCL_DOS
#include <os2.h>

#include "../lib/defines.h"
#include "scsi.h"
#include "srb.h"
#include "aspio.h"
#pragma pack(1)



PRIVATE UCHAR	data[5000];






PRIVATE ULONG
Swap16(USHORT us)
{
    return MAKEUSHORT(HIUCHAR(us), LOUCHAR(us));
}


PRIVATE ULONG
Swap32(ULONG ul)
{
    /* Toshiba: 2* ex */
    return MAKEULONG(MAKEUSHORT(HIUCHAR(HIUSHORT(ul)),
				LOUCHAR(HIUSHORT(ul))),
		     MAKEUSHORT(HIUCHAR(LOUSHORT(ul)),
				LOUCHAR(LOUSHORT(ul))));
}




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
 * DisplayDefects(data,descr)
 *
 * PARAMETER
 *	data		defect data from device
 *	descr		kind of defects (ASCIIZ)
 *
 * RETURNS
 *	(nothing)
 *
 * DESCRIPTION
 *	Displays defects in human readable form.
 *
 * REMARKS
 */
PRIVATE void
DisplayDefects(PUCHAR data,PCSZ descr)
{
    ULONG	i;


    printf("Defect list header: ");
    DumpBuffer(data, 4);


    if( (data[1] & 0x07) == 0 )
    {
	ULONG const	cnt = Swap16(*(PUSHORT)&data[2])/4;
	PULONG		p = (PVOID)&data[4];

	printf("%lu %s in 'block format'\n", cnt, descr);
	for( i = 0; i < cnt; ++i, ++p )
	    printf("%lu. defect block:\t%lu\n", i, Swap32(p[i]));
    }
    else if( (data[1] & 0x07) == 4 )
    {
	ULONG const	cnt = Swap16(*(PUSHORT)&data[2])/8;
	struct _INDEX {
	    UCHAR	cyl[3];
	    UCHAR	head;
	    UCHAR	bc[4];
	} * index = (PVOID)&data[4];

	printf("%lu %s in 'index format'\n", cnt, descr);
	for( i = 0; i < cnt; ++i, ++index )
	{
	    ULONG	ul1 = MAKEULONG(MAKEUSHORT(index[i].cyl[2],index[i].cyl[1]),
					MAKEUSHORT(index[i].cyl[0],0));
	    ULONG	ul2 = Swap32(*(PULONG)index[i].bc);

	    printf("%lu. defect:\tCylinder %lu, Head %u, Bytes %lu\n",
		   i, ul1, index[i].head, ul2);
	}
    }
    else if( (data[1] & 0x07) == 5 )
    {
	ULONG const	cnt = Swap16(*(PUSHORT)&data[2])/8;
	struct _PHYS {
	    UCHAR	cyl[3];
	    UCHAR	head;
	    UCHAR	sec[4];
	} * index = (PVOID)&data[4];

	printf("%lu %s in 'physical sector format'\n", cnt, descr);
	for( i = 0; i < cnt; ++i, ++index )
	{
	    ULONG	ul1 = Swap32((*(PULONG)index[i].cyl) & 0x00FFFFFF) >> 8;
	    ULONG	ul2 = Swap32(*(PULONG)index[i].sec);

	    printf("%lu. defect:\tCylinder %lu, Head %u, Sector %lu\n",
		   i, ul1, index[i].head, ul2);
	}
    }
    else
    {
	printf("%s in unknown format, dumping:\n", descr);
	DumpBuffer(data, Swap16(*(PUSHORT)&data[2]));
    }
    return;
}




PUBLIC int
main(int argc,char *argv[])
{
    APIRET	rc;
    unsigned	ha = -1, target = -1, lun = -1;
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
		"Invalid parameters\n"
		"usage: dispdef <ha> <target> <lun>\n");
	return 1;
    }


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
	    fprintf(stderr, "AspiGetType(%u,%u,%u) - rc %lu (%#lx)\n",
		    ha, target, lun, rc, rc);
	    break;
	}

	printf("HA %u  Target %u  LUN %u\t\"%s\" (%s)\n",
	       ha, target, lun,
	       AHInquiryType(type), AHInquiryQual(type));


	printf("Getting primary defects (not critical)...\n");
	rc = AspiDefectData(ha, target, lun, 0, 0, data, sizeof(data));
	if( rc != 0 )
	{
	    fprintf(stderr, "AspiDefectData - rc %lu (%#lx)\n", rc, rc);
	    DumpBuffer(&strLastSense, sizeof(strLastSense));
	    AHSense(data, &strLastSense),	printf(data);
	    break;
	}

	DisplayDefects(data, "primary defects");



	printf("Getting grown defects (keep an eye on this data)...\n");
	rc = AspiDefectData(ha, target, lun, 1, 0, data, sizeof(data));
	if( rc != 0 )
	{
	    fprintf(stderr, "AspiDefectData - rc %lu (%#lx)\n", rc, rc);
	    DumpBuffer(&strLastSense, sizeof(strLastSense));
	    AHSense(data, &strLastSense),	printf(data);
	    break;
	}

	DisplayDefects(data, "grown defects");
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
