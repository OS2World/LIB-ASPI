/*
 * $Source: r:/source/aspi/RCS/seqread.c,v $
 * $Revision: 1.3 $
 * $Date: 1999/08/18 00:14:55 $
 * $Locker:  $
 *
 *	ASPI Interface Library, READ (sequential device types)
 *
 * $Log: seqread.c,v $
 * Revision 1.3  1999/08/18 00:14:55  vitus
 * - updated location of defines.h (moved)
 * - changed function comments to new layout
 *
 * Revision 1.2  1997/09/22 02:23:53  vitus
 * documented
 *
 * Revision 1.1  1997/09/18 01:37:25  vitus
 * Initial revision
 * ----------------------------------------
 * Sample code to demonstrate use of ASPI Interface.
 */
static char const id[]="$Id: seqread.c,v 1.3 1999/08/18 00:14:55 vitus Exp $";

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
 * AspiReadSeq(ha,target,lun,fixed,count,buf,bufsiz)
 *
 * PARAMETER
 *	ha,target,lun	addresses device
 *	fixed		use block not byte count
 *	count		block or byte count
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
 *	Reads from sequential devices.  Be comfortable with filemarks to
 *	uses this funktion successfully.
 *
 * REMARKS
 */
PUBLIC APIRET _System
AspiReadSeq(UCHAR ha,UCHAR target,UCHAR lun,UCHAR fixed,ULONG count,
	    PVOID buf,ULONG bufsiz)
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
    srb->CDB[0] = SCSI_READ_6;
    srb->CDB[1] = (lun << 5) | (fixed ? 1 : 0);
    srb->CDB[2] = LOUCHAR(HIUSHORT(count));
    srb->CDB[3] = HIUCHAR(LOUSHORT(count));
    srb->CDB[4] = LOUCHAR(LOUSHORT(count));
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
