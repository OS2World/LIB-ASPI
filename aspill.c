/*
 * $Source: r:/source/aspi/RCS/aspill.c,v $
 * $Revision: 1.4 $
 * $Date: 1999/08/18 00:16:26 $
 * $Locker:  $
 *
 *	ASPI Interface Library,
 *	low-level routines to access ASPI Router driver.
 *
 *	Open, Close
 *
 * $Log: aspill.c,v $
 * Revision 1.4  1999/08/18 00:16:26  vitus
 * - updated location of defines.h (moved)
 * - changed function comments to new layout
 *
 * Revision 1.3  1997/09/22 02:28:26  vitus
 * commented
 *
 * Revision 1.2  1997/09/18 01:44:01  vitus
 * copies I/O buffer to/from locked ASPIRouter buffer
 * added public sense data structure
 *
 * Revision 1.1  1997/09/08 02:01:33  vitus
 * Initial revision
 * ----------------------------------------
 * Sample code to demonstrate use of ASPI Interface.
 */
static char const id[]="$Id: aspill.c,v 1.4 1999/08/18 00:16:26 vitus Exp $";

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define INCL_DOS
#define INCL_DOSFILEMGR
#define INCL_DOSDEVICES
#define INCL_DOSDEVIOCTL
#define INCL_DOSSEMAPHORES
#define INCL_ERRORS
#include <os2.h>

#include "../lib/defines.h"
#include "scsi.h"
#include "srb.h"
#include "aspio.h"

#define BUFFER_SIZE	(62*1024u)


#if !defined(min)				/* EMX doesn't supply it */
# define min(a,b)	((a)<(b)?(a):(b))
#endif


/* Definition to access aspirout.sys */

#define IOCTL_ASPIR_CATEGORY	0x92		/* a user-defined category */
#define ASPIR_START_SRB		0x02
#define ASPIR_REGISTER_SEMA	0x03
#define ASPIR_REGISTER_BUFFER	0x04



PUBLIC SCSI_REQSENSE_DATA	strLastSense;


PRIVATE HFILE	hdAspi = 0;
PRIVATE HEV	semAspi = 0;
PRIVATE PVOID	pAspiBuffer = NULL;






/*# ----------------------------------------------------------------------
 * AspiOpen(reserved)
 *
 * PARAMETER
 *	reserved	0
 *
 * RETURNS
 *	APIRET
 *
 * GLOBAL
 *	hdAspi, semAspi, pAspiBuffer
 *
 * DESPRIPTION
 *	Opens selected ASPI interface (currently only 0: aspirout.sys).
 *
 * REMARKS
 */
PUBLIC APIRET _System
AspiOpen(ULONG reserved)
{
    APIRET	rc;
    ULONG	action;
    ULONG	cbPara;
    ULONG	cbData;
    USHORT	rcDriver;

    if( reserved != 0 )
	return ERROR_INVALID_PARAMETER;		/* only 0 supported */

    do
    {
	/* Open driver */

	rc = DosOpen("ASPIROU$",
		     &hdAspi,
		     &action,
		     0,
		     0,
		     OPEN_ACTION_FAIL_IF_NEW|OPEN_ACTION_OPEN_IF_EXISTS,
		     OPEN_FLAGS_FAIL_ON_ERROR|OPEN_FLAGS_NOINHERIT
		     |OPEN_SHARE_DENYREADWRITE|OPEN_ACCESS_READWRITE,
		     NULL);
	if( rc != 0 )
	    break;

	/* Create event semaphore */

	rc = DosCreateEventSem(NULL,
			       &semAspi,
			       DC_SEM_SHARED,
			       FALSE);

	if( rc )
	    break;


	/* Pass semaphore handle to driver */

	cbPara = sizeof(semAspi);
	cbData = sizeof(rcDriver);
	rc = DosDevIOCtl(hdAspi,
			 IOCTL_ASPIR_CATEGORY,
			 ASPIR_REGISTER_SEMA,
			 &semAspi,
			 cbPara,
			 &cbPara,
			 &rcDriver,
			 cbData,
			 &cbData);
	if( rc != 0  ||  rcDriver != 0 )
	{
	    if( rc == 0 )
		rc = rcDriver;
	    break;
	}


	/* Allocate buffer and pass to ASPI Router */

	rc = DosAllocMem(&pAspiBuffer,
			 BUFFER_SIZE,
			 OBJ_TILE | PAG_READ
			 | PAG_WRITE | PAG_COMMIT);
	if( rc != 0 )
	    break;

	cbPara = sizeof(pAspiBuffer);
	cbData = sizeof(rcDriver);
	rc = DosDevIOCtl(hdAspi,
			 IOCTL_ASPIR_CATEGORY,
			 ASPIR_REGISTER_BUFFER,
			 pAspiBuffer,
			 cbPara,
			 &cbPara,
			 &rcDriver,
			 cbData,
			 &cbData);
	if( rc != 0  ||  rcDriver != 0 )
	{
	    if( rc == 0 )
		rc = rcDriver;
	    break;
	}
    }
    while( 0 );

    if( rc != 0 )
    {
	/* Error, cleanup */

	if( semAspi != 0 )
	    DosCloseEventSem(semAspi);
	if( hdAspi != 0 )
	    DosClose(hdAspi);
	if( pAspiBuffer != NULL )
	{
	    DosFreeMem(pAspiBuffer);
	    pAspiBuffer = NULL;
	}
    }
    return rc;
}




/*# ----------------------------------------------------------------------
 * AspiClose(void)
 *
 * PARAMETER
 *	(none)
 *
 * RETURNS
 *	APIRET
 *
 * GLOBAL
 *	hdAspi, semAspi, pAspiBuffer
 *
 * DESPRIPTION
 *	Closes current ASPI interface.  Don't forget to call as it
 *	may be important to do the cleanup.
 *
 * REMARKS
 *	With aspirout.sys it _is_ important to call AspiClose(): otherwise
 *	the used semaphore might never be freed.
 */
PUBLIC APIRET _System
AspiClose(void)
{
    APIRET	rc;
    APIRET	rcClose;

    /* Close semaphore */

    rc = DosCloseEventSem(semAspi);

    /* Close driver */

    rcClose = DosClose(hdAspi);

    /* Free memory */

    DosFreeMem(pAspiBuffer);
    pAspiBuffer = NULL;

    return (rc == 0 ? rcClose : rc);
}




/*# ----------------------------------------------------------------------
 * AspiSendSRB(srb,srbsize)
 *
 * PARAMETER
 *	srb		see ASPI spec
 *	srbsize		size of SRB
 *
 * RETURNS
 *	APIRET
 *
 * GLOBAL
 *	hdAspi, semAspi, pAspiBuffer
 *
 * DESPRIPTION
 *	Passes SRB to ASPI interface and waits until SRB is completed.
 *	Does any buffer management that may be needed.
 *
 * REMARKS
 */
PUBLIC APIRET _System
AspiSendSRB(PASPI_SRB_HEADER srb,ULONG srbsize)
{
    APIRET	rc;
    ULONG	cnt;
    ULONG	cbPara = srbsize;
    ULONG	cbData = srbsize;
    PVOID	data = NULL;
    ULONG	datalen = 0;

    srb->ASPIStatus = 0;

    /* Copy output data to global buffer known by ASPI interface. */

    if( srb->CommandCode == ASPI_CMD_EXECUTE_IO )
    {
	data = ((PASPI_SRB_EXECUTE_IO)srb)->pDataBuffer;
	datalen = min(BUFFER_SIZE, ((PASPI_SRB_EXECUTE_IO)srb)->DataXferLen);

	if( (srb->ASPIReqFlags
	     & ASPI_REQFLAG_DIRECTION_BITS) == ASPI_REQFLAG_DIR_TO_TARGET )
	    memcpy(pAspiBuffer, data, datalen);
    }

    /* Pass SRB to device driver. */

    rc = DosDevIOCtl(hdAspi,
		     IOCTL_ASPIR_CATEGORY,
		     ASPIR_START_SRB,
		     srb,
		     cbPara,
		     &cbPara,
		     srb,
		     cbData,
		     &cbData);
    if( rc != 0 )
	return rc;

    /* Wait for 'semAspi' to be posted by device driver. */

    if( (srb->ASPIReqFlags & ASPI_REQFLAG_POST_ENABLE) )
    {
	do
	{
	    rc = DosWaitEventSem(semAspi, 1000ul);
	}
	while( srb->ASPIStatus == 0 );
	if( rc == 0 )
	    rc = DosResetEventSem(semAspi, &cnt);
    }

    /* Copy input data to global buffer known by ASPI interface. */

    if( srb->CommandCode == ASPI_CMD_EXECUTE_IO
	&& datalen != 0
	&&  (srb->ASPIReqFlags
	     & ASPI_REQFLAG_DIRECTION_BITS) == ASPI_REQFLAG_DIR_TO_HOST )
	memcpy(data, pAspiBuffer, datalen);

    return rc;
}
