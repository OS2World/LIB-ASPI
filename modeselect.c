/*
 * $Source: r:/source/aspi/RCS/modeselect.c,v $
 * $Revision: 1.2 $
 * $Date: 1999/08/18 00:16:42 $
 * $Locker:  $
 *
 *	ASPI Interface Library, MODE SELECT
 *
 * $Log: modeselect.c,v $
 * Revision 1.2  1999/08/18 00:16:42  vitus
 * - updated location of defines.h (moved)
 * - changed function comments to new layout
 *
 * Revision 1.1  1997/09/22 02:25:05  vitus
 * Initial revision
 * ----------------------------------------
 * Sample code to demonstrate use of ASPI Interface.
 */
static char const id[]="$Id: modeselect.c,v 1.2 1999/08/18 00:16:42 vitus Exp $";

#include <assert.h>
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
 * AspiModeSelect(ha,target,lun,save,buf,bufsiz)
 *
 * PARAMETER
 *	ha,target,lun	addresses device
 *	save		save pages permanently?
 *	buf,bufsiz	mode pages plus header
 *
 * RETURNS
 *	APIRET
 *
 * GLOBAL
 *	strLastSense		updated
 *
 * DESPRIPTION
 *	Uses MODE SELECT(6 or 10) to change one or more mode pages
 *	on specified device.  Pages may be saved, refer to MODE
 *	SENSE(6 or 10) documentation.
 *
 * REMARKS
 *	See SCSI spec for format of mode pages.
 */
PUBLIC APIRET _System
AspiModeSelect(UCHAR ha,UCHAR target,UCHAR lun,
	       UCHAR save,PVOID buf,ULONG bufsiz)
{
    APIRET		rc;
    size_t const	srbsize = sizeof(ASPI_SRB_EXECUTE_IO)
				+ (bufsiz > 255 ? 10 : 6)
				+ sizeof(SCSI_REQSENSE_DATA);
    PASPI_SRB_EXECUTE_IO srb = malloc(srbsize);

    if( srb == NULL )
	return ERROR_NOT_ENOUGH_MEMORY;
    memset(srb, 0, srbsize);

    srb->SRBHdr.CommandCode = ASPI_CMD_EXECUTE_IO;
    srb->SRBHdr.AdapterIndex = ha;
    srb->SRBHdr.ASPIReqFlags = ASPI_REQFLAG_DIR_TO_TARGET | ASPI_REQFLAG_POST_ENABLE;

    srb->DeviceTargetID = target;
    srb->DeviceTargetLUN = lun;
    srb->DataXferLen = bufsiz;
    srb->pDataBuffer = buf;
    srb->SenseDataLen = sizeof(SCSI_REQSENSE_DATA);

    assert( save <= 0x01 );
    if( bufsiz > 255 )
    {
	srb->CDBLen = 10;
	srb->CDB[0] = SCSI_MODE_SELECT_10;
	srb->CDB[1] = (lun << 5) | 0x10 | save;
	srb->CDB[2] = 0;
	srb->CDB[3] = 0;
	srb->CDB[4] = 0;
	srb->CDB[5] = 0;
	srb->CDB[6] = 0;
	srb->CDB[7] = HIUCHAR((USHORT)bufsiz);
	srb->CDB[8] = LOUCHAR((USHORT)bufsiz);
	srb->CDB[9] = 0;
    }
    else
    {
	srb->CDBLen = 6;
	srb->CDB[0] = SCSI_MODE_SELECT;
	srb->CDB[1] = (lun << 5) | 0x10 | save;
	srb->CDB[2] = 0;
	srb->CDB[3] = 0;
	srb->CDB[4] = (UCHAR)bufsiz;
	srb->CDB[5] = 0;
    }

    rc = AspiSendSRB(&srb->SRBHdr, srbsize);

    if( rc == 0 )
    {
	if( srb->TargetStatus != ASPI_TSTATUS_NO_ERROR )
	    rc = 0xF2000000 | srb->TargetStatus;
	else if( srb->HostStatus != ASPI_HSTATUS_NO_ERROR )
	    rc = 0xF1000000 | srb->HostStatus;
	else if( srb->SRBHdr.ASPIStatus != ASPI_STATUS_NO_ERROR )
	    rc = 0xF0000000 | srb->SRBHdr.ASPIStatus;
	memcpy(&strLastSense,
	       &srb->CDB[bufsiz > 255 ? 10 : 6],
	       sizeof(SCSI_REQSENSE_DATA));
    }
    free(srb);
    return rc;
}
