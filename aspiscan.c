/*
 * $Source: r:/source/aspi/RCS/aspiscan.c,v $
 * $Revision: 1.4 $
 * $Date: 1999/08/18 00:11:47 $
 * $Locker:  $
 *
 *	Test ASPI Interface Library,
 *	aspiscan-like utility.
 *
 * $Log: aspiscan.c,v $
 * Revision 1.4  1999/08/18 00:11:47  vitus
 * - updated location of defines.h (moved)
 *
 * Revision 1.3  1997/09/22 02:27:51  vitus
 * uses AH*() routines from library
 *
 * Revision 1.2  1997/09/18 01:57:40  vitus
 * changed to new header file names
 *
 * Revision 1.1  1997/09/08 01:59:32  vitus
 * Initial revision
 * ----------------------------------------
 * Sample code to demonstrate use of ASPI Interface.
 */
static char const id[]="$Id: aspiscan.c,v 1.4 1999/08/18 00:11:47 vitus Exp $";

#include <stdio.h>

#define INCL_DOS
#include <os2.h>

#include "../lib/defines.h"
#include "scsi.h"
#include "srb.h"
#include "aspio.h"




PUBLIC int
main(void)
{
    APIRET		rc;
    ASPI_SRB_INQUIRY	srb;
    UCHAR		hacnt = 0xFF;
    UCHAR		ha, target, lun;

    rc = AspiOpen(0);
    if( rc != 0 )
    {
	fprintf(stderr, "AspiOpen - rc %lu\n", rc);
	return rc;
    }

    for( ha = 0; ha < hacnt; ++ha )
    {
	rc = AspiHAInq(0, &srb);
	if( rc != 0 )
	{
	    fprintf(stderr, "AspiHAInq(%u,) - rc %lu (%#lx)\n", ha, rc, rc);
	    break;
	}

	if( ha == 0 )
	{
	    printf("AdapterCount:\t%u\n", srb.AdapterCount);
	    hacnt = srb.AdapterCount;
	}

	printf("\n========== Adapter %u ==========\n", ha);
	printf("Adapter ID:\t%u\n", srb.AdapterTargetID);
	printf("Manager Name:\t\"%s\"\n", srb.ManagerName);
	printf("Adapter Name:\t\"%s\"\n", srb.AdapterName);
	printf("Adapter Param.:\t\"%s\"\n", srb.AdapterParms);
	printf("Adapter Features:\t%#x\n", srb.AdapterFeatures);
	printf("Adapter SG Cnt:\t\t%u\n", srb.MaximumSGList);
	printf("Adapter Transfer:\t%lu\n", srb.MaximumCDBTransfer);

	for( target = 0; target <= 7; ++target )
	{
	    for( lun = 0; lun <= 7; ++lun )
	    {
		UCHAR	type;			/* see SCSI spec */

		rc = AspiGetType(ha, target, lun, &type);
		if( rc != 0 )
		    break;

		printf("HA %u  Target %u  LUN %u\t\"%s\" (%s)\n",
		       ha, target, lun,
		       AHInquiryType(type), AHInquiryQual(type));
	    }
	}

    }

    rc = AspiClose();
    if( rc != 0 )
    {
	fprintf(stderr, "AspiClose - rc %lu (%#lx)\n", rc, rc);
	return rc;
    }

    return 0;
}
