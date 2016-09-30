/*===========================================================================*/
/*              This supportpac is maintained by Jeff Lowrey	             */
/*              Wayne M. Schutz is maintainer emeritus			             */
/*                                                                           */
/*              Chris Petty is the original author.                          */
/*                                                                           */
/*(C)Copyright IBM Corporation 1995, 1997, 1998, 1999, 2000,                 */
/*(C)Copyright IBM Corporation 2002, 2003, 2004, 2005, 2006                  */
/*(C)Copyright IBM Corporation 2007, 2008, 2009 							 */
/*                                                                           */
/*===========================================================================*/
#ifndef ARGS__
#define ARGS__
/* Include standard libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Include WebSphere MQ headers */
#include <cmqc.h>
#include <cmqcfc.h>
#include <cmqxc.h>

/* v6.1.1 this file entirely new, enhanced argument processing */

#define USAGE \
"\nUsage is:\n %s [options], where [options] are one or more of the following optional switches\n" \
" \n" \
" --help | -h | -?  : gives help (this)\n" \
" --localQMgr | -m lqmgr : is the name of the local qmgr to connect (MQCONN)\n" \
" --remoteQmgr | -r rqmgr : is the name of the remote qmgr (XMITQ name)\n" \
" --outputFile | -f [file] : allows the output file to be named, \n" \
"    if -f is not specified, the output file defaults to SAVEQMGR.MQSC\n" \
"    if -f is specified without a filename, it is named \"qmgrname\".MQSC.\n" \
" --outputFileNoHeader | -F [file] : behaves the same as -f except no header line is created in the output file\n"\
"      (and CURDEPTH, ALTDATE and  ALTTIME are not reported)\n" \
" --replaceOutput | -o : specifies to replace (overwrite) the output file\n" \
"    if the -o flag is not specified, then the default is to append if the file exists\n" \
"    (on iSeries, *REPLACE,*APPEND must be specified after the -o flag)\n" \
" --stdout | -c : directs output to STDOUT and overrides use of -f flag\n" \
" --noSystemObjects | -s [list] : suppress creation of objects that begin with \"SYSTEM*\"\n" \
"       list is a comma separated selection from \n" \
"            QMGR,QUEUE,NAMELIST,CHANNEL,PROCESS,AUTHINFO,LISTENER,SERVICE,TOPIC,SUB,NONE\n" \
"       Specifying NONE is equivalent to specifying -s without any options and will override all other choices\n" \
"       This is an inclusion list, System objects of the types listed *will* be reported in the output file.\n" \
" --qm : A shortcut option that is exactly equivlaent to --noSystemObjects QMGR.\n" \
"       The ALTER QMGR command will be produced, all other system objects will be skipped.\n" \
" --resetChannels | -R [flags] : create RESET CHANNEL commands\n" \
"    flags can be any combination of the follow to create RESET for that channel type:\n"\
"    s (SENDER) v (SERVER) r (RECEIVER) q (REQUESTER) c (CLUSSDR) d (CLUSRCVR)\n"\
"    the default is to create RESET commands for all the above channel types\n"\
" --permDyn | -p : Create 'local queue' definitions from PERMDYN queues.\n" \
" --ignoreErrors | -i : Ignore 'pcf' error codes 'damaged object' 'unknown object'.\n" \
"    Sets RetCode to WARNING if this error encountered.\n" \
" --quiet | -q : Quiet mode, do not issue warning about unknown PCF attributes\n" \
" --oneLine | -1 : generate entire DEFINE command on one line (which does NOT include ALTDATE etc)\n" \
" --twoLines | -2 : on a 2nd line, generate the DEFINE as a comment (which DOES include ALTDATE etc)\n" \
" --saveSecurity | -z [file] : create 'security definitions (like amqoamd -s)' in file 'file'(only >=V6)\n" \
"           if not specified, defaults to \"qmgrname\".AUT \n" \
" --saveSecurityNoHeader | -Z [file]: same as -z only without the header line\n" \
" --commandServerQ | -cs [queue]: the name of the Command Server Input queue, defaults to SYSTEM.ADMIN.COMMAND.QUEUE\n" \
" --waitTime | -w [seconds]: the period of time to wait for a response from the command server, defaults to 60 seconds\n" \
" --version | -v version : determines which version of MQSC to generate\n" \
"      and can be '2','5','51','52','53','6', or '7' \n" \
"    Note: see read.me for more information BEFORE you use this flag.\n" \
"    The default is to generate mqsc at the version of the connected queue manager \n" \
"%s"    /* this %s is replaced by USAGE2 value */

#define USAGE2 \
"\n If either the -a or -x switch is used, then MQCONNX will be used for the client connection\n" \
" --hostname | -a host : is the address of the host for a client connection (default is \"localhost\")\n" \
" --channel | -x channel : is the SVRCONN name for a client connection (default is \"SYSTEM.DEF.SVRCONN\")\n" \
" --sslCipher | -C cipher spec : indicates to use an SSL cipher spec for this client connection\n" \
" --keyRepos | -k key repository : name of key respository directory if using SSL for client connection\n" \
" --channelExitX | -eX channel exit: X = 'y' for security, 's' for send, 'r' for receive\n" \
" --channelDataX | -dX \"channel exit data\": X = 'y' for security, 's' for send, 'r' for receive.\n"

#define USAGE3 "\n"

#define OS400PARMCNT 11
/* os400 needs a special open mode */

#define ZZSTRLEN 256

#if MQAT_DEFAULT == MQAT_OS400
#define FILENAMESIZE 33 /* v51 AvV Lib/File(Member) -> 10/10(10) */
#define OPENMODEA "a lrecl=128"
#define OPENMODER "w lrecl=128"
#define DEFAULTOUTPUTFILE "*CURLIB/QMQSC(SAVEQMGR)"
#define DEFAULTOUTPUTFILEAUT "*CURLIB/QMQSCAUT(SAVEQMGR)"
#define DEFAULTOUTPUTPREFIX "*CURLIB/QMQSC("
#define DEFAULTOUTPUTPOSTFIX ")"
#define DEFAULTOUTPUTPREFIXAUT "*CURLIB/QMAUT("
#define DEFAULTOUTPUTPOSTFIXAUT ")"
#define ZZOS400PARMS   /* indicate os/400 style parms */
#else
#define FILENAMESIZE ZZSTRLEN
#define OPENMODEA "a"
#define OPENMODER "w"
#define DEFAULTOUTPUTFILE "SAVEQMGR.MQSC"
#define DEFAULTOUTPUTFILEAUT "SAVEQMGR.AUT"
#define DEFAULTOUTPUTPREFIX ""
#define DEFAULTOUTPUTPOSTFIX ".MQSC"
#define DEFAULTOUTPUTPREFIXAUT ""
#define DEFAULTOUTPUTPOSTFIXAUT ".AUT"
#endif
#define DEFAULTCMDSVRQ "SYSTEM.ADMIN.COMMAND.QUEUE                     "
#define LOCAL_QMGR_LONG_NAME  "--localQMgr"
#define LOCAL_QMGR_SHORT_NAME   "-m"
#define REMOTE_QMGR_LONG_NAME   "--remoteQMgr"
#define REMOTE_QMGR_SHORT_NAME   "-r"
#define HELP_LONG_NAME   "--help"
#define HELP_SHORT_NAME   "-h"
#define HELP_SHORT_NAME_ALT1   "-?"
#define HELP_SHORT_NAME_ALT2   "?"
#define VERSION_SHORT_NAME   "-v"
#define VERSION_LONG_NAME   "--version"
#define OUTPUT_FILE_LONG_NAME   "--outputFile"
#define OUTPUT_FILE_SHORT_NAME   "-f"
#define OUTPUT_FILE_NH_LONG_NAME   "--outputFileNoHeader"
#define OUTPUT_FILE_NH_SHORT_NAME   "-F"
#define REPLACE_OUTPUT_LONG_NAME   "--replaceOutput"
#define REPLACE_OUTPUT_SHORT_NAME   "-o"
#define STDOUT_LONG_NAME   "--stdout"
#define STDOUT_SHORT_NAME   "-c"
#define NO_SYSOBJS_LONG_LN 17
#define NO_SYSOBJS_LONG_NAME   "--noSystemObjects"
#define NO_SYSOBJS_SHORT_NAME   "-s"
#define ONLY_QMGR_LONG_NAME   "--qm"
#define RESET_CHANNELS_LONG_NAME   "--resetChannels"
#define RESET_CHANNELS_SHORT_NAME   "-R"
#define PERMDYN_LONG_NAME   "--permDyn"
#define PERMDYN_SHORT_NAME   "-p"
#define IGNORE_ERRORS_LONG_NAME   "--ignoreErrors"
#define IGNORE_ERRORS_SHORT_NAME   "-i"
#define QUIET_LONG_NAME   "--quiet"
#define QUIET_SHORT_NAME   "-q"
#define ONE_LINE_LONG_NAME   "--oneLine"
#define ONE_LINE_SHORT_NAME   "-1"
#define TWO_LINES_LONG_NAME   "--twoLines"
#define TWO_LINES_SHORT_NAME   "-2"
#define SAVE_SEC_LONG_NAME   "--saveSecurity"
#define SAVE_SEC_SHORT_NAME   "-z"
#define SAVE_SEC_NH_LONG_NAME   "--saveSecurityNoHeader"
#define SAVE_SEC_NH_SHORT_NAME   "-Z"
#define HOSTNAME_LONG_NAME   "--hostname"
#define HOSTNAME_SHORT_NAME   "-a"
#define CHANNEL_LONG_NAME   "--channel"
#define CHANNEL_SHORT_NAME   "-x"
#define SSL_CIPHER_LONG_NAME   "--sslCipher"
#define SSL_CIPHER_SHORT_NAME   "-C"
#define KEYREPOS_LONG_NAME   "--keyRepos"
#define KEYREPOS_SHORT_NAME   "-k"
#define COMSVRQ_LONG_NAME   "--commandServerQ"
#define COMSVRQ_SHORT_NAME   "-cs"
#define CHANNEL_EXIT_Y_LONG_NAME   "--channelExity"
#define CHANNEL_EXIT_S_LONG_NAME   "--channelExits"
#define CHANNEL_EXIT_R_LONG_NAME   "--channelExitr"
#define CHANNEL_EXIT_Y_SHORT_NAME   "-ey"
#define CHANNEL_EXIT_S_SHORT_NAME   "-es"
#define CHANNEL_EXIT_R_SHORT_NAME   "-er"
#define CHANNEL_DATA_Y_LONG_NAME   "--channelDatay"
#define CHANNEL_DATA_S_LONG_NAME   "--channelDatas"
#define CHANNEL_DATA_R_LONG_NAME   "--channelDatar"
#define CHANNEL_DATA_Y_SHORT_NAME   "-dy"
#define CHANNEL_DATA_S_SHORT_NAME   "-ds"
#define CHANNEL_DATA_R_SHORT_NAME   "-dr"
#define WAITTIME_SHORT_NAME "-w"
#define WAITTIME_LONG_NAME "--waitTime"
#define SHORT_WAITTIME_SHORT_NAME "-ws"
#define SHORT_WAITTIME_LONG_NAME "--waitTimeShort"
#define DIAG_LONG_NAME   "--diagnostics"
#define DIAG_SHORT_NAME   "-D"
#define USER_ID_LONG_NAME "--userId"
#define USER_ID_SHORT_NAME "-u"
#define PLATFORM_SHORT_NAME "-y"
#define PLATFORM_LONG_NAME "--platform"
#define OUTPUT_STYLE_XML_LONG_NAME "--xml"
#define OUTPUT_STYLE_MQSC_LONG_NAME "--mqsc"

#define OUTPUT_STYLE_MQSC 0
#define OUTPUT_STYLE_XML 1

/* v603 --- channel RESET command selection flags */
/* s (SENDER) v (SEVER) r (RECEIVER) q (REQUESTER) c (CLUSSDR) d (CLUSRCVR) */

#define CHLR_SENDER   	  1
#define CHLR_SERVER   	  2
#define CHLR_RECEIVER 	  4
#define CHLR_REQUESTER	  8
#define CHLR_CLUSSDR   	 16
#define CHLR_CLUSRCVR  	 32

#define SO_NONE	      	  0
#define SO_ALL			  1
#define SO_QMGR	      	  2
#define SO_QUEUE	  	  4
#define SO_NAMELIST	  	  8
#define SO_PROCESS	   	 16
#define SO_AUTHINFO	   	 32
#define SO_LISTENER	   	 64
#define SO_SERVICE	  	128
#define SO_TOPIC	  	256
#define SO_SUB	      	512
#define SO_CHANNEL		1024

typedef struct {
	MQCHAR LclQMgrName[MQ_Q_MGR_NAME_LENGTH+1];
	MQCHAR RmtQMgrName[MQ_Q_MGR_NAME_LENGTH+1];
	MQCHAR CmdSvrQName[MQ_Q_NAME_LENGTH+1];
	int UserVersion; /* version number user requested */
	char StrVersion[256]; /* v531a for error message */
	/*filename constructed from queue manager name or "saveqmgr.tst" */
	char fileName[FILENAMESIZE+1];
	char authFileName[FILENAMESIZE+1];
	MQCHAR48 ReplyToQ; /* DynamicQName for this run       */

	/*** v532 drive mqconnx for client version of supportpac ***/

	MQCHAR argCONNAME[MQ_CONN_NAME_LENGTH+1]; /* Client Conn IP Address or name  */
	MQCHAR argSVRCONN[MQ_CHANNEL_NAME_LENGTH+1]; /* Client Conn SVRCONN name        */
	int flagCONNAME; /* Did user give us a CONNAME?  */
	int flagSSL; /* should we use ssl ? */

	int oneLine; /* [v601].. create each DEFINE on one line */
	int twoLines; /* v601 whether or not to write a second line as a comment */
	int SysObjs; /* [v51].. create "system" objects */
	int makeSecurityDefs; /* make security definitions */
	char myName[256];
	int diagnostics; /* determines whether to issue some diags around mq ops */
	int noisy; /* whether or not to put bad PCF attr messages */
	int chlReset; /* determines whether or not to issue RESET CHANNEL cmds */
	int header; /* whether to create header in output file */ /* v513 */
	int authHeader; /* whether to create header in oam output file */
	int permDyn; /* v530 */ /* whether to create qlocal from Permdyn */
	int ignoreDamaged; /* v530 */ /* whether to ignore damaged obj messages */
	int noMsgOnTimeout; /* v531a */ /* whether to give an error msg on timeout */
	/* variables for handling -o option */
	int overlay;
	/* working storage for -R command */
	int qsgQmgr; /* v602 ..is this a qsg qmgr ? (set in qmgr.c) */
	int platform; /* platform to generate code for */
	int std_out;
	char reset_Switches[8];
	char lineTerm[3]; /* v601 -- line terminator */
	int outputStyle; /* 0 for MQSC, 1 for XML, 2 for something else */
	MQCNO Connect_options;/* MQCONNX options                 */
	MQCD ClientConn; /* Client connection channel       */

	MQSCO SSL_options; /* Client SSL options */
	int shortWaitTime; 
	int longWaitTime;
	MQCHAR12 userID;
} CmdArgParms;

typedef CmdArgParms* CmdArgParmsPtr;

int ProcessArgs(int argc, char *argv[], CmdArgParmsPtr MyParms, int *parmCount,int isClient);
int ProcessMainArgs(int argc, char *argv[], CmdArgParmsPtr MyParms,
		int *parmCount,int isClient);
int ProcessISeriesArgs(int argc, char *argv[], CmdArgParmsPtr MyParms,
		int *parmCount);

void InitArgs(CmdArgParmsPtr MyParms);
int toUpperStr(char* myStr);

#endif /*ARGS__*/
