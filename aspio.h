/*
 * $Source: r:/source/aspi/RCS/aspio.h,v $
 * $Revision: 1.7 $
 * $Date: 1999/08/18 02:10:24 $
 * $Locker:  $
 *
 *	Describes complete ASPI Interface Library.
 *
 * $Log: aspio.h,v $
 * Revision 1.7  1999/08/18 02:10:24  vitus
 * - AspiDefectData()
 *
 * Revision 1.6  1999/08/18 00:21:52  vitus
 * - added AspiInquiry()
 *
 * Revision 1.5  1998/11/07 19:46:08  vitus
 * - added AspiPlxFlash* routines
 *
 * Revision 1.4  1997/09/22 02:30:58  vitus
 * commented
 * removed ASPI Router depend values
 *
 * Revision 1.3  1997/09/21 03:57:02  vitus
 * added mode select
 * added routines from ascii.c
 *
 * Revision 1.2  1997/09/17 01:34:17  vitus
 * added 'strLastSense'
 * added more 'Aspi*' routines
 *
 * Revision 1.1  1997/09/08 02:01:50  vitus
 * Initial revision
 * ----------------------------------------
 * Sample code to demonstrate use of ASPI Interface.
 */



extern SCSI_REQSENSE_DATA	strLastSense;


/*
 * Open/Close ASPI interface.  Use parameter to AspiOpen()
 * to select interface (currently only 0: aspirout.sys).
 */
extern APIRET _System	AspiOpen(ULONG reserved);
extern APIRET _System	AspiClose(void);

/*
 * Lowest level, see ASPI spec.
 */
extern APIRET _System	AspiSendSRB(PASPI_SRB_HEADER srb,ULONG srbsize);

extern APIRET _System	AspiHAInq(UCHAR ha,PASPI_SRB_INQUIRY srb);
extern APIRET _System	AspiGetType(UCHAR ha,UCHAR target,UCHAR lun,PUCHAR type);


/*
 * higher level: SCSI related stuff.
 * Return value will be
 * APIRET		if lower than 0x10000 (OS/2 errors codes are
 *			really 16bit to keep them compatible with OS/2 1.x).
 *			Please keep in mind that device driver errors are
 *			returned as negative numbers (16bit).
 * ASPI Error		if MSB == 0xF0, see ASPI spec.
 * Host Adapter	Error	if MSB == 0xF1, see ASPI spec.
 * Target Status	if MSB == 0xF2, see SCSI spec.  'hsLastSense' will
 *			probably be valid.
 */
extern APIRET _System	AspiInquiry(UCHAR ha,UCHAR target,UCHAR lun,
				    PVOID buf,ULONG bufsiz);
extern APIRET _System	AspiModeSense(UCHAR ha,UCHAR target,UCHAR lun,
				      UCHAR page,UCHAR pcontrol,
				      PVOID buf,ULONG bufsiz);
extern APIRET _System	AspiModeSelect(UCHAR ha,UCHAR target,UCHAR lun,
				       UCHAR save,PVOID buf,ULONG bufsiz);

extern APIRET _System	AspiMount(UCHAR ha,UCHAR target,UCHAR lun,UCHAR mount);
extern APIRET _System	AspiRewind(UCHAR ha,UCHAR target,UCHAR lun);
extern APIRET _System	AspiReadSeq(UCHAR ha,UCHAR target,UCHAR lun,
				    UCHAR fixed,ULONG count,
				    PVOID buf,ULONG bufsiz);
extern APIRET _System	AspiDefectData(UCHAR ha,UCHAR target,UCHAR lun,
				       UCHAR pg,UCHAR format,
				       PVOID buf,ULONG bufsiz);


/* Restricted code from aspio.lib --> */

extern APIRET _System	AspiPlxFlashCheck(UCHAR ha,UCHAR target,UCHAR lun,
					  PVOID buf,ULONG bufsiz);
extern APIRET _System	AspiPlxFlashErase(UCHAR ha,UCHAR target,UCHAR lun,
					  UCHAR mode,UCHAR sector);
extern APIRET _System	AspiPlxFlashWrite(UCHAR ha,UCHAR target,UCHAR lun,
					  UCHAR mode,ULONG offset,
					  PVOID buf,ULONG bufsiz);

/* <-- */


/*
 * Small helpers
 */
extern char const *	AHInquiryType(UCHAR type);
extern char const *	AHInquiryQual(UCHAR qual);
extern void		AHSense(char * const buf,PSCSI_REQSENSE_DATA const sense);

