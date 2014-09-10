/*
 * $Source: r:/source/aspi/RCS/inquiry.c,v $
 * $Revision: 1.2 $
 * $Date: 1999/08/18 00:13:23 $
 * $Locker:  $
 *
 *	ASPI Interface Library, INQUIRY (all device types)
 *
 * $Log: inquiry.c,v $
 * Revision 1.2  1999/08/18 00:13:23  vitus
 * - updated location of defines.h (moved)
 * - changed function comments to new layout
 *
 * Revision 1.1  1998/11/07 20:32:15  vitus
 * Initial revision
 * ----------------------------------------
 * Sample code to demonstrate use of ASPI Interface.
 */
static char const id[]="$Id: inquiry.c,v 1.2 1999/08/18 00:13:23 vitus Exp $";

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
 * AspiInquiry(ha,target,lun,buf,bufsiz)
 *
 * PARAMETER
 *	ha,target,lun	addresses device
 *	buf		data will be placed here
 *	bufsiz		size of 'buf'
 *
 * RETURNS
 *	APIRET
 *
 * GLOBAL
 *	hsLastSense	updated
 *
 * DESPRIPTION
 *	Requests information about a SCSI device.
 *
 * REMARKS
 */
PUBLIC APIRET _System
AspiInquiry(UCHAR ha,UCHAR target,UCHAR lun,PVOID buf,ULONG bufsiz)
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
    srb->SRBHdr.ASPIReqFlags = ASPI_REQFLAG_DIR_TO_HOST | ASPI_REQFLAG_POST_ENABLE;

    srb->DeviceTargetID = target;
    srb->DeviceTargetLUN = lun;
    srb->DataXferLen = bufsiz;
    srb->pDataBuffer = buf;
    srb->SenseDataLen = sizeof(SCSI_REQSENSE_DATA);

    srb->CDBLen = 6;
    srb->CDB[0] = SCSI_INQUIRY;
    srb->CDB[1] = (lun << 5);
    srb->CDB[2] = 0;
    srb->CDB[3] = 0;
    srb->CDB[4] = (bufsiz > 255 ? 255 : bufsiz);
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
	memcpy(&strLastSense, &srb->CDB[10], sizeof(SCSI_REQSENSE_DATA));
    }
    free(srb);
    return rc;
}
