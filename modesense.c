/*
 * $Source: r:/source/aspi/RCS/modesense.c,v $
 * $Revision: 1.4 $
 * $Date: 1999/08/18 00:13:03 $
 * $Locker:  $
 *
 *	ASPI Interface Library, MODE SENSE
 *
 * $Log: modesense.c,v $
 * Revision 1.4  1999/08/18 00:13:03  vitus
 * - updated location of defines.h (moved)
 * - changed function comments to new layout
 *
 * Revision 1.3  1997/09/22 02:25:22  vitus
 * commented
 *
 * Revision 1.2  1997/09/18 01:35:32  vitus
 * allocate srb from heap
 * mode sense (6 or 10) depending on data size
 *
 * Revision 1.1  1997/09/08 02:56:05  vitus
 * Initial revision
 * ----------------------------------------
 * Sample code to demonstrate use of ASPI Interface.
 */
static char const id[]="$Id: modesense.c,v 1.4 1999/08/18 00:13:03 vitus Exp $";

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
 * AspiModeSense(ha,target,lun,page,which,buf,bufsiz)
 *
 * PARAMETER
 *	ha,target,lun	addresses device
 *	page		mode page to read
 *	which		0 = current values
 *	buf,bufsiz	mode pages plus header
 *
 * RETURNS
 *	APIRET
 *
 * GLOBAL
 *	strLastSense		updated
 *
 * DESPRIPTION
 *	Uses MODE SENSE(6 or 10) to read one or all mode pages.
 *
 * REMARKS
 *	See SCSI spec for format of mode pages.
 */
PUBLIC APIRET _System
AspiModeSense(UCHAR ha,UCHAR target,UCHAR lun,
	      UCHAR page,UCHAR which,PVOID buf,ULONG bufsiz)
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
    srb->SRBHdr.ASPIReqFlags = ASPI_REQFLAG_DIR_TO_HOST | ASPI_REQFLAG_POST_ENABLE;

    srb->DeviceTargetID = target;
    srb->DeviceTargetLUN = lun;
    srb->DataXferLen = bufsiz;
    srb->pDataBuffer = buf;
    srb->SenseDataLen = sizeof(SCSI_REQSENSE_DATA);

    assert( page <= 0x3F );
    assert( which <= 0x03 );
    if( bufsiz > 255 )
    {
	srb->CDBLen = 10;
	srb->CDB[0] = SCSI_MODE_SENSE_10;
	srb->CDB[1] = (lun << 5) | 0x00;
	srb->CDB[2] = (which << 6) | page;
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
	srb->CDB[0] = SCSI_MODE_SENSE;
	srb->CDB[1] = (lun << 5) | 0x00;
	srb->CDB[2] = (which << 6) | page;
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
