# This Makefile makes the saveqmgr executable on VAX-VMS (ms03)
#
MQML =
# MQM library directory
MQMI = mqs_include:
# Option file that should be used
OPTFILE = saveqmgr
OPTF = $(OPTFILE).opt

# CC defines the compiler. Set to "cc" for ANSI compliant C compiler.
CC = cc

# Set CFLAGS - the compiler options.
#CFLAGS = /decc /warnings=(enable=all, disable=(misalgndstrct, misalgndmem, \
#         strctpadding, caserange, switchlong, ignorecallval, dollarid,     \
#         truncintasn, truncintcast)) /include_directory=$(MQMI)
CFLAGS = /decc /warnings=(enable=all, disable=(misalgndstrct, \
switchlong, ignorecallval))/include_directory=$(MQMI)


# Shareable library to include
LIBS = sys$library:mqm/share

# List of object files that build the executable

all : saveqmgr.exe
        @ !

saveqmgr.obj : saveqmgr.c saveqmgr.h
channel.obj  : channel.c saveqmgr.h
mqutils.obj  : mqutils.c saveqmgr.h
process.obj  : process.c saveqmgr.h
namelist.obj : namelist.c saveqmgr.h
qmgr.obj     : qmgr.c saveqmgr.h
queue.obj    : queue.c saveqmgr.h
authinfo.obj    : authinfo.c saveqmgr.h
subscript.obj	: subscript.c saveqmgr.h
topic.obj	: topic.c saveqmgr.h

saveqmgr.exe : saveqmgr.obj channel.obj mqutils.obj process.obj namelist.obj qmgr.obj queue.obj authinfo.obj topic.obj subscript.obj
        Open/Write Tmp $(OPTF)
        Write Tmp "$(MQML)$(LIBS)"
        Close Tmp
	$(LINK) $(LINKFLAGS) $(MMS$SOURCE_LIST),$(OPTFILE)/options

