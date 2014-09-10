/*
 * $Source: R:/source/aspi/RCS/srb.h,v $
 * $Revision: 1.3 $
 * $Date: 1997/09/22 02:32:05 $
 * $Author: vitus $
 *
 * Describes ASPI structures, created from DDK header with
 * same name.
 * Changed for ASPI Router access (buffer management).
 *
 * $Log: srb.h,v $
 * Revision 1.3  1997/09/22 02:32:05  vitus
 * modified comments
 *
 * Revision 1.2  1997/09/17 01:30:38  vitus
 * beautified, added offset comments
 *
 * Revision 1.1  1997/09/08 02:01:04  vitus
 * Initial revision
 * ----------------------------------------
 * Sample code to demonstrate use of ASPI Interface.
 */

#ifndef _ASPI_H
#define _ASPI_H
#pragma pack(1)

#define ASPI_CMD_LIMIT    6

/*
 * ASPI SRB Header
 */

typedef struct _ASPI_SRBHDR {			/* ASHD */

    UCHAR	CommandCode;			/* 00: */
    UCHAR	ASPIStatus;			/* 01: */
    UCHAR	AdapterIndex;			/* 02: */
    UCHAR	ASPIReqFlags;			/* 03: */

} ASPI_SRB_HEADER, *PASPI_SRB_HEADER;


/*
 * Command Values in ASPI_SRB_HEADER->CommandCode
 */

#define ASPI_CMD_ADAPTER_INQUIRY	0x00
#define ASPI_CMD_GET_DEVICE_TYPE	0x01
#define ASPI_CMD_EXECUTE_IO		0x02
#define ASPI_CMD_ABORT_IO		0x03
#define ASPI_CMD_RESET_DEVICE		0x04
#define ASPI_CMD_SET_ADAPTER_PARMS	0x05

/*
 * ASPI Status Values in ASPI_SRB_HEADER->ASPIStatus
 */

#define ASPI_STATUS_IN_PROGRESS		0x00
#define ASPI_STATUS_NO_ERROR		0x01
#define ASPI_STATUS_ABORTED		0x02
#define ASPI_STATUS_ERROR		0x04
#define ASPI_STATUS_INVALID_COMMAND     0x80
#define ASPI_STATUS_INVALID_ADAPTER	0x81
#define ASPI_STATUS_INVALID_TARGET	0x82

/*
 * ASPI Flags in ASPI_SRB_HEADER->ASPIReqFlags
 */

#define ASPI_REQFLAG_POST_ENABLE	0x01
#define ASPI_REQFLAG_LINKED_SRB		0x02
#define ASPI_REQFLAG_RESIDUAL		0x04

#define ASPI_REQFLAG_DIRECTION_BITS	0x18
#define ASPI_REQFLAG_DIR_TO_HOST	0x08
#define ASPI_REQFLAG_DIR_TO_TARGET	0x10
#define ASPI_REQFLAG_DIR_NO_DATA_XFER	0x18

#define ASPI_REQFLAG_SG_ENABLE		0x20




/*
 * ASPI ADAPTER INQUIRY SRB            (for ASPI_CMD_ADAPTER_INQUIRY)
 */

typedef struct _ASPI_SRB_ADAPTER_INQUIRY {	/* ASAI */

    ASPI_SRB_HEADER	SRBHdr;
    UCHAR		Reserved_1[4];
    UCHAR		AdapterCount;
    UCHAR		AdapterTargetID;
    UCHAR		ManagerName[16];
    UCHAR		AdapterName[16];
    UCHAR		AdapterParms[16];

/*
 * New fields for an EXTENDED adapter inquiry                            
 */

    USHORT		AdapterFeatures;                   
    USHORT		MaximumSGList;                     
    ULONG		MaximumCDBTransfer;                

} ASPI_SRB_INQUIRY, *NPASPI_SRB_INQUIRY, FAR *PASPI_SRB_INQUIRY;


/*
 * Number of bytes in the extended fields -         
 */

#define MAX_EXTENDED_COUNT	0x08                            

/*
 * Features in ASPI_SRB_ADAPTER_INQUIRY->AdapterFeatures
 */

#define ASPI_SCATTER_GATHER	0x01
#define ASPI_RESIDUAL_BYTE	0x02




/*
 * ASPI GET DEVICE TYPE SRB		(for ASPI_CMD_DEVICE_TYPE)
 */

typedef struct _ASPI_SRB_DEVICE_TYPE {		/* ASDT */

    ASPI_SRB_HEADER	SRBHdr;
    UCHAR		Reserved_1[4];
    UCHAR		DeviceTargetID;
    UCHAR		DeviceTargetLUN;
    UCHAR		DeviceType;

} ASPI_SRB_DEVICE_TYPE, *PASPI_SRB_DEVICE_TYPE;




/*
 * ASPI EXECUTE IO SRB			(for ASPI_CMD_EXECUTE_IO)
 */

typedef struct _ASPI_SRB_EXECUTE_IO {		/* ASEI */

    ASPI_SRB_HEADER	SRBHdr;
    UCHAR		Reserved_0[2];		/* 04: S/G list */
    UCHAR		Reserved_1[2];		/* 06: */
    UCHAR		DeviceTargetID;		/* 08: */
    UCHAR		DeviceTargetLUN;	/* 09: */
    ULONG		DataXferLen;		/* 0A: */
    UCHAR		SenseDataLen;		/* 0E: */
    void *		pDataBuffer;		/* 0F: */
    void *		pNxtSRB;		/* 13: don't use it! */
    UCHAR		CDBLen;			/* 17: */
    UCHAR		HostStatus;		/* 18: */
    UCHAR		TargetStatus;		/* 19: */
    ULONG		RM_PostAddress;		/* 1A: not called! */
    USHORT		RM_DataSeg;		/* 1E: */
    ULONG		PM_PostAddress;		/* 20: not called! */
    USHORT		PM_DataSeg;		/* 24: */
    ULONG		ppSRB;			/* 26: */
    UCHAR		ASPIWorkSpace[22];	/* 2A: */
    UCHAR		CDB[1];			/* 40: */

} ASPI_SRB_EXECUTE_IO, *NPASPI_SRB_EXECUTE_IO, FAR *PASPI_SRB_EXECUTE_IO;


/*
 * Status returned in ASPI_SRB_EXECUTE_IO->HostStatus
 */

#define ASPI_HSTATUS_NO_ERROR		0x00
#define ASPI_HSTATUS_SELECTION_TIMEOUT	0x11
#define ASPI_HSTATUS_DATA_OVERRUN	0x12
#define ASPI_HSTATUS_BUS_FREE		0x13
#define ASPI_HSTATUS_BUS_PHASE_ERROR	0x14
#define ASPI_HSTATUS_BAD_SGLIST		0x1A              

/*
 * Status returned in ASPI_SRB_EXECUTE_IO->TargetStatus
 */

#define ASPI_TSTATUS_NO_ERROR		0x00
#define ASPI_TSTATUS_CHECK_CONDITION	0x02
#define ASPI_TSTATUS_BUSY		0x08
#define ASPI_TSTATUS_RESERV_CONFLICT	0x18




/*
 * ASPI ABORT IO SRB			(for ASPI_CMD_ABORT_IO)
 */

typedef struct _ASPI_SRB_ABORT_IO {		/* ASAB */

    ASPI_SRB_HEADER	SRBHdr;
    UCHAR		Reserved_1[4];
    UCHAR		ppSRB;

} ASPI_SRB_ABORT_IO, *NPASPI_SRB_ABORT_IO, FAR *PASPI_SRB_ABORT_IO;




/*
 * ASPI RESET DEVICE SRB		(for ASPI_RESET_DEVICE)
 */

typedef struct _ASPI_SRB_RESET_DEVICE {		/* ASRD */

    ASPI_SRB_HEADER	SRBHdr;
    UCHAR		Reserved_1[4];
    UCHAR		DeviceTargetID;
    UCHAR		DeviceTargetLUN;
    UCHAR		Reserved_2[14];
    UCHAR		HostStatus;
    UCHAR		TargetStatus;
    ULONG		RM_PostAddress;		/* not called! */
    USHORT		RM_DataSeg;
    ULONG		PM_PostAddress;		/* not called! */
    USHORT		PM_DataSeg;
    UCHAR		ASPIWorkSpace[22];

} ASPI_SRB_RESET_DEVICE, *PASPI_SRB_RESET_DEVICE;




/*
 * ASPI SET HOST ADAPTER PARAMETERS	(for ASPI_CMD_SET_ADAPTER_PARMS)
 */

typedef struct _ASPI_SRB_ADAPTER_PARMS {	/* ASAP */

    ASPI_SRB_HEADER	SRBHdr;
    UCHAR		Reserved_1[4];
    UCHAR		AdapterParms[16];

}  ASPI_SRB_ADAPTER_PARMS, *PASPI_SRB_ADAPTER_PARMS;


#pragma pack()
#endif /* !def _ASPI_H */
