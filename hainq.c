/*
 * $Source: r:/source/aspi/RCS/hainq.c,v $
 * $Revision: 1.4 $
 * $Date: 1999/08/18 00:14:22 $
 * $Locker:  $
 *
 *	ASPI Interface Library, HOST ADAPTER INQUIRY
 *
 * $Log: hainq.c,v $
 * Revision 1.4  1999/08/18 00:14:22  vitus
 * - updated location of defines.h (moved)
 * - changed function comments to new layout
 *
 * Revision 1.3  1997/09/22 02:25:49  vitus
 * commented
 *
 * Revision 1.2  1997/09/18 01:59:34  vitus
 * changed to new header file names
 *
 * Revision 1.1  1997/09/08 02:02:58  vitus
 * Initial revision
 * ----------------------------------------
 * Sample code to demonstrate use of ASPI Interface.
 */
static char const id[]="$Id: hainq.c,v 1.4 1999/08/18 00:14:22 vitus Exp $";

#include <string.h>

#define INCL_DOS
#include <os2.h>

#include "../lib/defines.h"
#include "scsi.h"
#include "srb.h"
#include "aspio.h"






/*# ----------------------------------------------------------------------
 * AspiHAInq(ha,srb)
 *
 * PARAMETER
 *	ha	host adapter index
 *	srb	see ASPI spec
 *
 * RETURNS
 *	APIRET
 *
 * GLOBAL
 *	(none)
 *
 * DESPRIPTION
 *	Issues ADAPTER INQUIRY SRB to ASPI interface.  As this
 *	calls returns more information than is feasable
 *	to return via stack parameters the complete SRB
 *	is parameter.
 *
 * REMARKS
 */
PUBLIC APIRET _System
AspiHAInq(UCHAR ha,PASPI_SRB_INQUIRY srb)
{
    APIRET	rc;

    memset(srb, 0, sizeof(*srb));
    srb->SRBHdr.CommandCode = ASPI_CMD_ADAPTER_INQUIRY;
    srb->SRBHdr.AdapterIndex  = ha;
    srb->SRBHdr.ASPIReqFlags = 0;		/* no flags set */

    rc = AspiSendSRB(&srb->SRBHdr, sizeof(*srb));

    if( rc == 0 )
    {
	if( srb->SRBHdr.ASPIStatus != ASPI_STATUS_NO_ERROR )
	    rc = 0xF0000000 | srb->SRBHdr.ASPIStatus;
    }

    return rc;
}
