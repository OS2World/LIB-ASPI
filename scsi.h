/*
 * $Source: r:/source/aspi/RCS/scsi.h,v $
 * $Revision: 1.5 $
 * $Date: 1999/08/18 02:13:02 $
 * $Locker:  $
 *
 *	Describes SCSI structures.  Created from DDK header with
 *	same name, so IBM copyrights may be active?
 *	Expanded to contain more opcodes, mode pages, ...
 *
 * $Log: scsi.h,v $
 * Revision 1.5  1999/08/18 02:13:02  vitus
 * - added SCSI_READ_DEFECT_DATA_*
 *
 * Revision 1.4  1998/03/03 03:13:17  vitus
 * added SCSI_MODEPAGE_D typedef
 *
 * Revision 1.3  1997/09/22 02:33:18  vitus
 * added typedefs for some mode pages
 *
 * Revision 1.2  1997/09/21 03:52:09  vitus
 * added sample interesting mode pages (2,8,10)
 *
 * Revision 1.1  1997/09/17 02:17:48  vitus
 * Initial revision
 * ----------------------------------------
 * Sample code to demonstrate use of ASPI Interface.
 */



/* Typedefs to resolve forward references */

#if !defined(__IBMC__)  &&  !defined(__EMX__)
typedef struct _SCSIReqSenseData	SCSI_REQSENSE_DATA;
typedef struct _SCSIReqSenseData FAR *	PSCSI_REQSENSE_DATA;
typedef struct _SCSIReqSenseData *	NPSCSI_REQSENSE_DATA;

typedef struct _SCSICDB6	SCSICDB6;
typedef struct _SCSICDB6 FAR *	PSCSICDB6;
typedef struct _SCSICDB6 *	NPSCSICDB6;

typedef struct _SCSICDB10	SCSICDB10;
typedef struct _SCSICDB10 FAR *	PSCSICDB10;
typedef struct _SCSICDB10 *	NPSCSICDB10;

typedef struct _SCSICDB12	SCSICDB12;
typedef struct _SCSICDB12 FAR *	PSCSICDB12;
typedef struct _SCSICDB12 *	NPSCSICDB12;

typedef struct  _SCSIStatusBlock	SCSI_STATUS_BLOCK;
typedef struct  _SCSIStatusBlock FAR *	PSCSI_STATUS_BLOCK;
typedef struct  _SCSIStatusBlock *	NPSCSI_STATUS_BLOCK;

typedef struct  _SCSIInqData		SCSI_INQDATA;
typedef struct  _SCSIInqData FAR *	PSCSI_INQDATA;
typedef struct  _SCSIInqData *		NPSCSI_INQDATA;
#endif

/*
**    General SCSI definitions
*/


/*
**  SCSI Operation Codes
*/

#define SCSI_TEST_UNIT_READY	0x00    /* Test Unit Ready */
#define SCSI_REZERO_UNIT	0x01    /* CD-ROM rezero unit */
#define SCSI_REWIND		0x01    /* Tape Rewind */
#define SCSI_REQUEST_SENSE	0x03    /* Request Sense Command */
#define SCSI_READ_BLK_LIMITS	0x05    /* Read Block Limits */
#define SCSI_REQ_AUX_SENSE	0x06    /* Request Auxiliary Sense */
#define SCSI_REASSIGN_BLOCKS	0x07    /* Reassign Blocks */
#define SCSI_READ_6		0x08    /* SCSI 6 byte Read */
#define SCSI_WRITE_6		0x0A    /* SCSI 6 byte Write */
#define SCSI_SEEK_6		0x0B    /* SCSI 6 byte Seek */
#define SCSI_WRITE_FILEMARKS	0x10    /* Tape Write Filemarks */
#define SCSI_SPACE		0x11    /* Tape Space */
#define SCSI_INQUIRY		0x12    /* Inquiry command */
#define SCSI_RECOVER_BUFFER	0x14    /* Tape Recover Buffer */
#define SCSI_MODE_SELECT	0x15    /* Mode Select */
#define SCSI_RESERVE_UNIT	0x16    /* Tape Reserve Unit */
#define SCSI_RELEASE_UNIT	0x17    /* Tape Release Unit */
#define SCSI_ERASE		0x19    /* Tape Erase */
#define SCSI_MODE_SENSE		0x1A    /* Mode Sense */
#define SCSI_START_STOP_UNIT	0x1B    /* Start/Stop Unit */
#define SCSI_LOAD_UNLOAD	0x1B    /* Tape Load/Unload Media */
#define SCSI_SEND_DIAGNOSTIC	0x1D    /* Send Diagnostic */
#define SCSI_LOCK_UNLOCK	0x1E    /* Lock/Unlock drive door */
#define SCSI_READ_CAPACITY	0x25    /* Read Capacity */
#define SCSI_READ_10		0x28    /* SCSI 10 byte Read */
#define SCSI_WRITE_10		0x2A    /* SCSI 10 byte Write */
#define SCSI_SEEK_10		0x2B    /* SCSI 10 byte Seek */
#define SCSI_LOCATE		0x2B    /* Tape Locate */
#define SCSI_WRITE_VERIFY_10	0x2E    /* SCSI 10 byte Write w/Verify */
#define SCSI_VERIFY_10		0x2F    /* SCSI 10 byte Verify */
#define SCSI_PREFETCH		0x34    /* Prefetch */
#define SCSI_READ_DEFECT_DATA	0x37	/* Primary or Grown Defects */
#define SCSI_READ_SUB_CHAN	0x42    /* Read Sub-Channel (CD-ROM) */
#define SCSI_READ_TOC		0x43    /* Read Table of Contents */
#define SCSI_READ_HEADER	0x44    /* Read Header */
#define SCSI_PLAY_AUDIO_10	0x45    /* Play Audio 10 byte */
#define SCSI_PLAY_MSF		0x47    /* Play Audio - MSF format */
#define SCSI_PLAY_TRACK_INDEX	0x48    /* Play Audio - Track Index */
#define SCSI_PLAY_TRACK_REL	0x49    /* Play Audio - Track Relative */
#define SCSI_PAUSE_RESUME	0x4B    /* Pause/Resume Audio Play */
#define SCSI_MODE_SELECT_10	0x55    /* Mode Select */
#define SCSI_MODE_SENSE_10	0x5A	/* SCSI 10 Mode Sense */
#define SCSI_PLAY_AUDIO_12	0xA5    /* Play Audio - 12 byte */
#define SCSI_READ_12		0xA8    /* SCSI 12 byte read */
#define SCSI_PLAY_TRACK_REL_12	0xA9	/* Play Audio - Trace Relative 12 */
#define SCSI_READ_DEFECT_DATA_12 0xB7	/* Primary or Grown Defects */




/*
**  SCSI Command Descriptor Block
*/



typedef struct _SCSICDB6 { /* CDB6 - 6 byte Command Descriptor Block*/

    UCHAR	Opcode;			/* CDB Operation Code           */
    UCHAR	Lun_MsbLBA;		/* SCSI LUN & 5 MSB bits of LBA */
    UCHAR	MidLBA;			/* SCSI MID byte of LBA */
    UCHAR	LsbLBA;			/* SCSI LBA byte of LBA */
    UCHAR	XferLen;		/* SCSI Xfer length, Alloc length */
    UCHAR	Control;		/* Control byte */

} SCSICDB6,FAR *PSCSICDB6, *NPSCSICDB6;



typedef struct _SCSICDB10 { /* CDB10 - 10 byte Command Descriptor Block*/

    UCHAR	Opcode;			/* CDB Operation Code */
    UCHAR	Lun;			/* SCSI */
    UCHAR	LBA[4];			/* SCSI LBA MSB->LSB */
    UCHAR	Res;			/* reserved byte */
    UCHAR	XferLen[2];		/* SCSI Xfer length (MSB first) */
    UCHAR	Control;		/* Control byte */

} SCSICDB10,FAR *PSCSICDB10, *NPSCSICDB10;


typedef struct _SCSICDB12 { /* CDB12 - 12 byte Command Descriptor Block*/

    UCHAR	Opcode;			/* CDB Operation Code */
    UCHAR	Lun;			/* SCSI */
    UCHAR	LBA[4];			/* SCSI LBA MSB->LSB */
    UCHAR	XferLen[4];		/* SCSI Xfer length (MSB first) */
    UCHAR	Res;			/* reserved byte */
    UCHAR	Control;		/* Control byte */

} SCSICDB12, FAR *PSCSICDB12, *NPSCSICDB12;


/*
** SCSI Status byte codes
*/


#define SCSI_STAT_GOOD		0x00	/* Good status */
#define SCSI_STAT_CHECKCOND	0x02	/* SCSI Check Condition */
#define SCSI_STAT_CONDMET	0x04	/* Condition Met */
#define SCSI_STAT_BUSY		0x08	/* Target busy status */
#define SCSI_STAT_INTER		0x10	/* Intermediate status */
#define SCSI_STAT_INTERCONDMET	0x14	/* Intermediate condition met */
#define SCSI_STAT_RESCONFLICT	0x18	/* Reservation conflict */
#define SCSI_STAT_CMDTERM	0x22	/* Command Terminated */
#define SCSI_STAT_QUEUEFULL	0x28	/* Queue Full */



/*
**  Request Sense Data format
*/

typedef struct  _SCSIReqSenseData { /* REQSEN */

    UCHAR	ErrCode_Valid;		/* Error Code & Valid bit */
    UCHAR	SegNum;			/* Segment Number */
    UCHAR	SenseKey;		/* Sense Key,ILI,EOM, FM */
    UCHAR	INFO[4];		/* information field */
    UCHAR	AddLen;			/* additional length */
    UCHAR	CmdInfo[4];		/* command-specific info */
    UCHAR	AddSenseCode;		/* additional sense code */
    UCHAR	AddSenseCodeQual;	/* additional sense code qualifier */
    UCHAR	FieldRepUnitCode;	/* field replaceable unit code */
    UCHAR	KeySpecific[3];		/* Sense-key specific */

} SCSI_REQSENSE_DATA, FAR *PSCSI_REQSENSE_DATA, *NPSCSI_REQSENSE_DATA;



/*
**  Sense Data bit masks
*/


/* Byte 0 of sense data */


#define SCSI_ERRCODE_MASK	0x7F	/* Error Code */
#define SCSI_VALID_MASK		0x80	/* Information field valid bit */



/* Byte 2 of sense data */


#define SCSI_SENSEKEY_MASK	0xF	/* Sense key */
#define SCSI_INCORRECT_LEN	0x20	/* Incorrect lenght indicator */
#define SCSI_SENSE_ENDOFMEDIUM	0x40	/* End-of-medium bit */
#define SCSI_SENSE_FM		0x80	/* filemark bit */



/*
**  Sense Key definitions
*/


#define SCSI_SK_NOSENSE		0x00	/* No sense */
#define SCSI_SK_RECERR		0x01	/* Recovered Error */
#define SCSI_SK_NOTRDY		0x02	/* Not Ready Error */
#define SCSI_SK_MEDIUMERR	0x03	/* Medium Error */
#define SCSI_SK_HARDWAREERR	0x04	/* HardWare Error */
#define SCSI_SK_ILLEGALREQ	0x05	/* Illegal Request */
#define SCSI_SK_UNITATTN	0x06	/* Unit Attention */
#define SCSI_SK_DATAPROTECT	0x07	/* Data Protect Error */
#define SCSI_SK_BLANKCHK	0x08	/* Blank Check */
#define SCSI_SK_COPYABORT	0x0A	/* Copy Aborted */
#define SCSI_SK_ABORTEDCMD	0x0B	/* Aborted Command */
#define SCSI_SK_EQUAL		0x0C	/* Equal Comparison satisfied */
#define SCSI_SK_VOLOVERFLOW	0x0D	/* Volume Overflow */
#define SCSI_SK_MISCOMPARE	0x0E	/* Miscompare */


/*
**  Additional Sense Code definitions
*/


#define ASC_NO_SEEK_COMPLETE		0x02
#define ASC_UNIT_NOT_READY		0x04
#define ASC_UNIT_COMM_FAIL		0x08
#define ASC_SERVO_FAIL			0x09
#define ASC_UNRECOVERED_ERROR		0x11
#define ASC_NO_ADDRESS_MARK		0x12
#define ASC_SEEK_POSITIONING_ERROR	0x15
#define ASC_DATA_SYNC_ERROR		0x16
#define ASC_INVALID_COMMAND_OPCODE	0x20
#define ASC_ILLEGAL_LBA			0x21
#define ASC_INVALID_FIELD		0x24
#define ASC_UNIT_NOT_SUPPORTED		0x25
#define ASC_INVALID_PARMLIST		0x26
#define ASC_MEDIUM_CHANGED		0x28
#define ASC_RESET			0x29
#define ASC_INCOMPATIBLE_CARTRIDGE	0x30
#define ASC_INTERNAL_TARGET_FAIL	0x44
#define ASC_TOC_RECOVERY_FAIL		0x57
#define ASC_END_USER_AREA		0x63
#define ASC_ILLEGAL_MODE_FOR_TRACK	0x64

#define ASCV_NOT_AUDIO_TRACK		0x88
#define ASCV_NOT_DATA_TRACK		0x89
#define ASCV_NOT_AUDIO_PLAY_STATE	0x8A


/*
** SCSI IORB StatusBlock definition
*/

#define SCSI_DIAGINFO_LEN	8

typedef struct  _SCSIStatusBlock { /* STATBLK */

    USHORT	Flags;			/* Status block flags */
    USHORT	AdapterErrorCode;	/* Translated Adapter Error */
    UCHAR	TargetStatus;		/* SCSI status codes */
    ULONG	ResidualLength;		/* Residual Length */
    UCHAR	AdapterDiagInfo[SCSI_DIAGINFO_LEN]; /* Raw adapter status */
    USHORT	ReqSenseLen;		/* amount of RS data requested */
    PSCSI_REQSENSE_DATA	SenseData;	/* pointer to Req Sense Data */

} SCSI_STATUS_BLOCK, FAR *PSCSI_STATUS_BLOCK, *NPSCSI_STATUS_BLOCK;

/* Status Block flags definitions */

#define STATUS_SENSEDATA_VALID	0x0001	/* Sense Data Valid */
#define STATUS_RESIDUAL_VALID	0x0002	/* Residual Byte Count Valid */
#define STATUS_DIAGINFO_VALID	0x0004	/* Diagnostic Information Valid */


/*
**  Inquiry Data format
*/

typedef struct  _SCSIInqData { /* INQ */

    UCHAR	DevType;		/* Periph Qualifier & Periph Dev Type */
    UCHAR	RMB_TypeMod;		/* rem media bit & Dev Type Modifier */
    UCHAR	Vers;			/* ISO, ECMA, & ANSI versions */
    UCHAR	RDF;			/* AEN, TRMIOP, & response data format */
    UCHAR	AddLen;			/* length of additional data */
    UCHAR	Res1;			/* reserved */
    UCHAR	Res2;			/* reserved */
    UCHAR	Flags;			/* RelADr,Wbus32,Wbus16,Sync,etc. */
    UCHAR	VendorID[8];		/* Vendor Identification */
    UCHAR	ProductID[16];		/* Product Identification */
    UCHAR	ProductRev[4];		/* Product Revision */

} SCSI_INQDATA, FAR *PSCSI_INQDATA, *NPSCSI_INQDATA;


/*  Inquiry byte 0 masks */


#define SCSI_DEVTYPE		0x1F	/* Peripheral Device Type */
#define SCSI_PERIPHQUAL		0xE0	/* Peripheral Qualifier */


/*  Inquiry byte 1 mask */

#define SCSI_REMOVABLE_MEDIA	0x80	/* Removable Media bit (1=removable) */


/*  Peripheral Device Type definitions */


#define SCSI_DASD		0x00	/* Direct-access Device */
#define SCSI_SEQACESS		0x01	/* Sequential-access device */
#define SCSI_PRINTER		0x02	/* Printer device */
#define SCSI_PROCESSOR		0x03	/* Processor device */
#define SCSI_WRITEONCE		0x04	/* Write-once device */
#define SCSI_CDROM		0x05	/* CD-ROM device */
#define SCSI_SCANNER		0x06	/* Scanner device */
#define SCSI_OPTICAL		0x07	/* Optical memory device */
#define SCSI_MEDCHGR		0x08	/* Medium changer device */
#define SCSI_COMM		0x09	/* Communications device */
#define SCSI_NODEV		0x1F	/* Unknown or no device type */



/*
** Inquiry flag definitions (Inq data byte 7)
*/

#define SCSI_INQ_RELADR		0x80	/* device supports relative addressing */
#define SCSI_INQ_WBUS32		0x40	/* device supports 32 bit data xfers */
#define SCSI_INQ_WBUS16		0x20	/* device supports 16 bit data xfers */
#define SCSI_INQ_SYNC		0x10	/* device supports synchronous xfer */
#define SCSI_INQ_LINKED		0x08	/* device supports linked commands */
#define SCSI_INQ_CMDQUEUE	0x02	/* device supports command queueing */
#define SCSI_INQ_SFTRE		0x01	/* device supports soft resets */



/*
 * Some mode pages
 */

typedef struct _SCSI_MODEPAGE_HDR {

    UCHAR	len;
    UCHAR	medium;
    UCHAR	specific;
    UCHAR	descrlen;

} SCSI_MODEPAGE_HDR, * PSCSI_MODEPAGE_HDR;


typedef struct _SCSI_MODEPAGE_DESCR {

    UCHAR	density;
    UCHAR	blocks[3];
    UCHAR	reserved;
    UCHAR	blklen[3];

} SCSI_MODEPAGE_DESCR, * PSCSI_MODEPAGE_DESCR;



typedef struct _SCSI_MODEPAGE_2 {		/* disconnect-reconnect */

    UCHAR	pcode;				/* <0:5> page code
						   <7>   page is savable */
    UCHAR	plength;			/* excluding this field */
    UCHAR	bfull;				/* reads: x/256 before device
						   reconnects */
    UCHAR	bempty;				/* write: x/256 before device
						   reconnects */
    UCHAR	businactive[2];			/* max time in 100us */
    UCHAR	disconnect[2];			/* max time in 100us */
    UCHAR	connect[2];			/* max time in 100us */
    UCHAR	burst[2];			/* max sectors per data burst */
    UCHAR	dtdc;				/* disable/enable disconnect
						   in data transfers */
    UCHAR	reserved[3];

} SCSI_MODEPAGE_2, * PSCSI_MODEPAGE_2;


typedef struct _SCSI_MODEPAGE_8 {		/* caching */

    UCHAR	pcode;				/* s.a. */
    UCHAR	plength;			/* s.a. */
    UCHAR	cache;				/* <0>  read cache disable
						   <2>  write cache enable */
    UCHAR	priority;			/* pre-fetch priority */
    UCHAR	disprefetch[2];			/* disable pre-fetch on long
						   transfers */
    UCHAR	minprefetch[2];			/* minimum pre-fetch done */
    UCHAR	maxprefetch[2];			/* maximum pre-fetch done */
    UCHAR	prefetchceiling[2];		/* maximum pre-fetch done */

} SCSI_MODEPAGE_8, * PSCSI_MODEPAGE_8;


typedef struct _SCSI_MODEPAGE_A {		/* control */

    UCHAR	pcode;				/* s.a. */
    UCHAR	plength;			/* s.a. */
    UCHAR	reserved1;
    UCHAR	queuing;			/* <0>   disable tagged queuing
						   <4:7> queue algorithm */
    UCHAR	events;				/* enable asynchr. events */
    UCHAR	reserved2;
    UCHAR	aenholdoff[2];			/* related to asynchr. events */

} SCSI_MODEPAGE_A, * PSCSI_MODEPAGE_A;


typedef struct _SCSI_MODEPAGE_D {		/* CD-ROM */

    UCHAR	pcode;				/* s.a. */
    UCHAR	plength;			/* s.a. */
    UCHAR	reserved1;
    UCHAR	multiplier;			/* timeout code (see SCSI-II) */
    UCHAR	s_units[2];			/* ??? */
    UCHAR	f_units[2];			/* ??? */

} SCSI_MODEPAGE_D, * PSCSI_MODEPAGE_D;

