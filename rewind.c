/*
 * $Source: r:/source/aspi/RCS/rewind.c,v $
 * $Revision: 1.4 $
 * $Date: 1999/08/18 00:12:41 $
 * $Locker:  $
 *
 *	ASPI Interface Library, REWIND
 *
 * $Log: rewind.c,v $
 * Revision 1.4  1999/08/18 00:12:41  vitus
 * - updated location of defines.h (moved)
 * - changed function comments to new layout
 *
 * Revision 1.3  1997/09/22 02:24:49  vitus
 * commented
 *
 * Revision 1.2  1997/09/18 02:01:21  vitus
 * changed to new header file names
 *
 * Revision 1.1  1997/09/12 00:01:28  vitus
 * Initial revision
 * ----------------------------------------
 * Sample code to demonstrate use of ASPI Interface.
 */
static char const id[]="$Id: rewind.c,v 1.4 1999/08/18 00:12:41 vitus Exp $";

#include <string.h>
#include <stdlib.h>

#define INCL_DOS
#define INCL_ERRORS
#include <os2.h>

#include "../lib/defines.h"
#include "scsi.h"
#include "srb.h"
#include "aspio.h"






/*# ----------------------------------------------------------------------
 * AspiRewind(ha,target,lun)
 *
 * PARAMETER
 *	ha,target,lun	addresses device
 *
 * RETURNS	
 *	APIRET
 *
 * GLOBAL
 *	hsLastSense	updated
 *
 * DESPRIPTION
 *	Rewinds medium in sequential devices.
 *
 * REMARKS
 */
PUBLIC APIRET _System
AspiRewind(UCHAR ha,UCHAR target,UCHAR lun)
{
    APIRET		rc;
    size_t const	srbsize = sizeof(ASPI_SRB_EXECUTE_IO)
				+ 6 + sizeof(SCSI_REQSENSE_DATA);
    PASPI_SRB_EXECUTE_IO srb = malloc(srbsize);

    if( srb == NULL )
	return ERROR_NOT_ENOUGH_MEMORY;
    memset(srb, 0, srbsize);

    srb->SRBHdr.CommandCode = ASPI_CMD_EXECUTE_IO;
    srb->SRBHdr.AdapterIndex = ha;
    srb->SRBHdr.ASPIReqFlags = ASPI_REQFLAG_DIR_NO_DATA_XFER | ASPI_REQFLAG_POST_ENABLE;

    srb->DeviceTargetID = target;
    srb->DeviceTargetLUN = lun;
    srb->DataXferLen = 0;
    srb->pDataBuffer = NULL;
    srb->SenseDataLen = sizeof(SCSI_REQSENSE_DATA);

    srb->CDBLen = 6;
    srb->CDB[0] = SCSI_REWIND;
    srb->CDB[1] = (lun << 5);
    srb->CDB[2] = 0;
    srb->CDB[3] = 0;
    srb->CDB[4] = 0;
    srb->CDB[5] = 0;

    rc = AspiSendSRB(&srb->SRBHdr, srbsize);

    if( rc == 0 )
    {
	if( srb->TargetStatus != ASPI_TSTATUS_NO_ERROR )
	    rc = 0xF2000000 | srb->TargetStatus;
	else if( srb->HostStatus != ASPI_HSTATUS_NO_ERROR )
	    rc = 0xF1000000 | srb->HostStatus;
	else if( srb->SRBHdr.ASPIStatus != ASPI_STATUS_NO_ERROR )
	    rc = 0xF0000000 | srb->SRBHdr.ASPIStatus;
	memcpy(&strLastSense, &srb->CDB[6], sizeof(SCSI_REQSENSE_DATA));
    }
    free(srb);
    return rc;
}
