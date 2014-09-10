/*
 * $Source: r:/source/aspi/RCS/defects.c,v $
 * $Revision: 1.1 $
 * $Date: 1999/08/18 02:01:08 $
 * $Locker:  $
 *
 *	ASPI Interface Library, READ DEFECT DATA
 *
 * $Log: defects.c,v $
 * Revision 1.1  1999/08/18 02:01:08  vitus
 * Initial revision
 *
 * ----------------------------------------
 * Sample code to demonstrate use of ASPI Interface.
 */
static char const id[]="$Id: defects.c,v 1.1 1999/08/18 02:01:08 vitus Exp $";

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
 * AspiDefectData(ha,target,lun,pg,format,buf,bufsiz)
 *
 * PARAMETER
 *	ha,target,lun	addresses device
 *	pg		primary defects (0) or grown defects (1)
 *	format		preferred format of list
 *	buf,bufsiz	defects plus header
 *
 * RETURNS
 *	APIRET
 *
 * GLOBAL
 *	strLastSense		updated
 *
 * DESPRIPTION
 *	Uses READ DEFECT DATA (10 or 12) to read all defects.
 *
 * REMARKS
 *	See SCSI spec for possible formats of defect lists.
 */
PUBLIC APIRET _System
AspiDefectData(UCHAR ha,UCHAR target,UCHAR lun,
	       UCHAR pg,UCHAR format,PVOID buf,ULONG bufsiz)
{
    APIRET		rc;
    size_t const	srbsize = sizeof(ASPI_SRB_EXECUTE_IO)
				+ (bufsiz > 65535 ? 12 : 10)
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

    assert( pg <= 1 );
    assert( format <= 0x07 );
    if( bufsiz > 65535 )
    {
	srb->CDBLen = 12;
	srb->CDB[0] = SCSI_READ_DEFECT_DATA_12;
	srb->CDB[1] = (lun << 5) | (pg == 0 ? 0x10 : 0x08) | format;
	srb->CDB[2] = 0;
	srb->CDB[3] = 0;
	srb->CDB[4] = 0;
	srb->CDB[5] = 0;
	srb->CDB[6] = HIUCHAR(HIUSHORT(bufsiz));
	srb->CDB[7] = LOUCHAR(HIUSHORT(bufsiz));
	srb->CDB[8] = HIUCHAR(LOUSHORT(bufsiz));
	srb->CDB[9] = LOUCHAR(LOUSHORT(bufsiz));
	srb->CDB[10] = 0;
	srb->CDB[11] = 0;
    }
    else
    {
	srb->CDBLen = 10;
	srb->CDB[0] = SCSI_READ_DEFECT_DATA;
	srb->CDB[1] = (lun << 5) | 0x00;
	srb->CDB[2] = (pg == 0 ? 0x10 : 0x08) | format;
	srb->CDB[3] = 0;
	srb->CDB[4] = 0;
	srb->CDB[5] = 0;
	srb->CDB[6] = 0;
	srb->CDB[7] = HIUCHAR(LOUSHORT(bufsiz));
	srb->CDB[8] = LOUCHAR(LOUSHORT(bufsiz));
	srb->CDB[9] = 0;
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
