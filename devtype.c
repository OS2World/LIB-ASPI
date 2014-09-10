/*
 * $Source: r:/source/aspi/RCS/devtype.c,v $
 * $Revision: 1.4 $
 * $Date: 1999/08/18 00:13:41 $
 * $Locker:  $
 *
 *	ASPI Interface Library, GET DEVICE TYPE
 *
 * $Log: devtype.c,v $
 * Revision 1.4  1999/08/18 00:13:41  vitus
 * - updated location of defines.h (moved)
 * - changed function comments to new layout
 *
 * Revision 1.3  1997/09/22 02:25:37  vitus
 * commented
 *
 * Revision 1.2  1997/09/18 01:58:51  vitus
 * changed to new header file names
 *
 * Revision 1.1  1997/09/08 02:02:26  vitus
 * Initial revision
 * ----------------------------------------
 * Sample code to demonstrate use of ASPI Interface.
 */
static char const id[]="$Id: devtype.c,v 1.4 1999/08/18 00:13:41 vitus Exp $";

#include <string.h>

#define INCL_DOS
#include <os2.h>

#include "../lib/defines.h"
#include "scsi.h"
#include "srb.h"
#include "aspio.h"





/*# ----------------------------------------------------------------------
 * AspiGetType(ha,target,lun,type)
 *
 * PARAMETER
 *	ha,target,lun	addresses device to query
 *	type		return type here
 *
 * RETURNS
 *	ASPIRET
 *
 * GLOBAL
 *	(none)
 *
 * DESPRIPTION
 *	Uses ASPI to query device type of ha,target,lun and
 *	returns that type to caller.
 *
 * REMARKS
 */
PUBLIC APIRET _System
AspiGetType(UCHAR ha,UCHAR target,UCHAR lun,PUCHAR type)
{
    APIRET		rc;
    ASPI_SRB_DEVICE_TYPE srb;

    memset(&srb, 0, sizeof(srb));
    srb.SRBHdr.CommandCode = ASPI_CMD_GET_DEVICE_TYPE;
    srb.SRBHdr.AdapterIndex  = ha;
    srb.SRBHdr.ASPIReqFlags = 0;		/* no flags set */
    srb.DeviceTargetID = target;
    srb.DeviceTargetLUN = lun;

    rc = AspiSendSRB(&srb.SRBHdr, sizeof(srb));

    if( rc == 0 )
    {
	if( srb.SRBHdr.ASPIStatus != ASPI_STATUS_NO_ERROR )
	    rc = 0xF0000000 | srb.SRBHdr.ASPIStatus;
	else
	    *type = srb.DeviceType;
    }
    return rc;
}
