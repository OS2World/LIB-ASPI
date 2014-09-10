#
# $Source: r:/source/aspi/RCS/makefile,v $
# $Revision: 1.8 $
# $Date: 1999/08/18 02:10:02 $
# $Locker:  $
#
#	Create ASPI Interface Library
#
# $Log: makefile,v $
# Revision 1.8  1999/08/18 02:10:02  vitus
# - added defects.c to library
# - added dispdef.exe to sample applications
#
# Revision 1.7  1999/08/18 00:11:29  vitus
# - updated location of defines.h (moved)
#
# Revision 1.6  1999/08/17 23:51:30  vitus
# - switched to Visual Age for C++
# - added inquiry.c to library
#
# Revision 1.5  1998/11/07 19:45:19  vitus
# - all OBJ created in subdirectory o
# - added creation of plxupd.exe
# - added some modules to support plxupd.exe
#
# Revision 1.4  1998/07/31 00:56:23  vitus
# - added creation of cdspeed.exe
#
# Revision 1.3  1997/09/22 02:30:12  vitus
# added cleanup target (->rcsclean)
# renamed library to aspio.lib
#
# Revision 1.2  1997/09/18 02:03:06  vitus
# more library modules
# more applications
# automatic checkout of modules
#
# Revision 1.1  1997/09/08 01:59:45  vitus
# Initial revision
# ------------------------------------------------
# Sample code to demonstrate use of ASPI Interface.
#
COMPILER *= VAC
DEBUG *= 0

.INCLUDE: ../compiler.mkf

CFLAGS = $(_CFLAGS)


ASPIL_SRC = aspill.c hainq.c devtype.c modesense.c modeselect.c \
	rewind.c mount.c seqread.c inquiry.c defects.c \
	plxflshchk.c plxflshera.c plxflshwr.c \
	ascii.c
ASPIL_INC = srb.h scsi.h aspio.h

APP_SRC = aspiscan.c dispg.c scache.c tape.c

ASPIL_OBJS := o/{$(ASPIL_SRC:s/.c/.obj/)}


o/%.$(OBJ) : %.c '{$(ASPIL_INC)}' '../lib/defines.h'
.IF $(COMPILER) == GNUC
	$(CC2) $(CFLAGS) -o o/$*.$(OBJ) $<
.ELSE
	$(CC2) $(CFLAGS) -Foo/$*.$(OBJ) $<
.ENDIF


all: aspio.$(LIB) aspiscan.exe scache.exe tape.exe dispg.exe dispdef.exe \
	cdspeed.exe plxupd.exe

.REMOVE:
	@echo Clean $<

cleanup:
	rcsclean $(ASPIL_SRC)
	rcsclean $(APP_SRC)
#	rcsclean $(ASPIL_INC)



aspio.$(LIB) .SEQUENTIAL :! $(ASPIL_OBJS)
.IF $(COMPILER) == GNUC
	$(CLIB) r aspio.$(LIB) $?
.ELSE
	$(CLIB) aspio.$(LIB) -+$?;
.ENDIF


aspiscan.exe: o/aspiscan.$(OBJ) aspio.$(LIB)
	$(CL2VIO) $(CFLAGS) $<

dispg.exe: o/dispg.$(OBJ) aspio.$(LIB)
	$(CL2VIO) $(CFLAGS) $<

dispdef.exe: o/dispdef.$(OBJ) aspio.$(LIB)
	$(CL2VIO) $(CFLAGS) $<

scache.exe: o/scache.$(OBJ) aspio.$(LIB)
	$(CL2VIO) $(CFLAGS) $<

tape.exe: o/tape.$(OBJ) aspio.$(LIB)
	$(CL2VIO) $(CFLAGS) $<

cdspeed.exe: o/cdspeed.$(OBJ) aspio.$(LIB)
	$(CL2VIO) $(CFLAGS) $<

plxupd.exe: o/plxupd.$(OBJ) aspio.$(LIB)
	$(CL2VIO) $(CFLAGS) $<
