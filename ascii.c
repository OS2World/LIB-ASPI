/*
 * $Source: r:/source/aspi/RCS/ascii.c,v $
 * $Revision: 1.3 $
 * $Date: 1999/08/18 00:17:03 $
 * $Locker:  $
 *
 *	Translate some SCSI related values to ASCIIZ.
 *
 * $Log: ascii.c,v $
 * Revision 1.3  1999/08/18 00:17:03  vitus
 * - updated location of defines.h (moved)
 * - changed function comments to new layout
 *
 * Revision 1.2  1997/09/22 02:23:18  vitus
 * documented
 *
 * Revision 1.1  1997/09/21 03:53:43  vitus
 * Initial revision
 * ----------------------------------------
 * Sample code to demonstrate use of ASPI Interface.
 */
static char const id[]="$Id: ascii.c,v 1.3 1999/08/18 00:17:03 vitus Exp $";

#include <stdio.h>

#define INCL_DOS
#include <os2.h>

#include "../lib/defines.h"
#include "scsi.h"
#include "srb.h"
#include "aspio.h"



PRIVATE char *apszDevType[] = {
    "direct-access device",
    "sequential-access device",
    "printer device",
    "processor device",
    "write-once device",
    "CD-ROM device",
    "scanner device",
    "optical memory device",
    "medium changer device",
    "communication device"
};

PRIVATE char *apszDevQual[] = {
    "connected",
    "not connected",
    "???",
    "can't connect anything"
};





/*# ----------------------------------------------------------------------
 * AHInquiryType(type)
 *
 * PARAMETER
 *	type		from INQUIRY data
 *
 * RETURNS
 *	pointer to const string
 *
 * GLOBAL
 *	apszDevType[]
 *
 * DESPRIPTION
 *	Translates SCSI type to ASCII description.
 *
 * REMARKS
 */
PUBLIC char const *
AHInquiryType(UCHAR type)
{
    type &= 0x1F;
    if( type < sizeof(apszDevType) / sizeof(apszDevType[0]) )
	return apszDevType[type];
    else if( type == 0x1F )
	return "no device";
    return "???";
}




/*# ----------------------------------------------------------------------
 * AHInquiryType(type)
 *
 * PARAMETER
 *	type		from INQUIRY data
 *
 * RETURNS
 *	pointer to const string
 *
 * GLOBAL
 *	apszDevQual[]
 *
 * DESPRIPTION
 *	Translates SCSI type qualifier to ASCII description.
 *
 * REMARKS
 */
PUBLIC char const *
AHInquiryQual(UCHAR qual)
{
    qual = (qual >> 5) & 0x07;
    if( qual < sizeof(apszDevQual) / sizeof(apszDevQual[0]) )
	return apszDevQual[qual];
    return "???";
}




/*# ----------------------------------------------------------------------
 * AHSense(buffer,sense)
 *
 * PARAMETER
 *	buffer		to fill
 *	sense		sense data
 *
 * RETURNS
 *	(nothing)
 *
 * GLOBAL
 *	(none)
 *
 * DESPRIPTION
 *	Translates REQSENSE data to ASCII.  Multiline.
 *
 * REMARKS
 */
PUBLIC void
AHSense(char * const buffer,PSCSI_REQSENSE_DATA const sense)
{
    sprintf(buffer,
	    "error code %02x\n"
	    "sense key %02x, ILI %d, EOM %d, FM %d\n"
	    "additional sense code %02x\n"
	    "additional sense code qualifier %02x\n",
	    (sense->ErrCode_Valid & SCSI_ERRCODE_MASK),
	    (sense->SenseKey & SCSI_SENSEKEY_MASK),
	    (sense->SenseKey & SCSI_INCORRECT_LEN) ? 1 : 0,
	    (sense->SenseKey & SCSI_SENSE_ENDOFMEDIUM) ? 1 : 0,
	    (sense->SenseKey & SCSI_SENSE_FM) ? 1 : 0,
	    sense->AddSenseCode,
	    sense->AddSenseCodeQual);
}
