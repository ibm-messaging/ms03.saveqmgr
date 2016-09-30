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
/*                                                                           */
/*  See the read.me file for change history                                  */
/*                                                                           */
/*===========================================================================*/
/* Module Name: saveqmgr.c                                                   */
/*  DESCRIPTIVE NAME WebSphere MQ Save Queue Manager Object                  */
/*                  Definitions using PCFs (ms03 supportpac)                 */
/*                                                                           */
/*===========================================================================*/
/*                                                                           */
/* This is a program to inquire of a queue manager about its                 */
/* own attributes and those of all the objects, (queues, channels and        */
/* processes), defined to it. The inquiry may be made of a local or remote   */
/* queue manager.                                                            */
/*                                                                           */
/*===========================================================================*/

/* Include standard libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Include WebSphere MQ headers */
#include <cmqc.h>
#include <cmqcfc.h>
#include <cmqxc.h>

#if defined USEMQCNX
#define USEMQCONNX
#endif
/* Include Application headers */
#include "args.h"
#include "saveqmgr.h"
#include "mqutils.h"

int diagnostics = 0; /* determines whether to issue some diags around mq ops */
int noisy = 1; /* whether or not to put bad PCF attr messages */
int chlReset = 0; /* determines whether or not to issue RESET CHANNEL cmds */
int header = 1; /* whether to create header in output file *//* v513 */
int authHeader = 1; /* whether to create header in oam output file */
int permDyn = 0; /* v530 *//* whether to create qlocal from Permdyn */
int ignoreDamaged = 0; /* v530 *//* whether to ignore damaged obj messages */
int noMsgOnTimeout = 0; /* v531a *//* whether to give an error msg on timeout */
int foundDamagedObject = 0; /* v531a */
int foundnpmclass = 0; /* v533a */
int SysObjs = 0; /* [v51].. create "system" objects */
int oneLine = 0; /* [v601].. create each DEFINE on one line */
char lineTerm[3]; /* v601 -- line terminator */
int platform = 0; /* platform to generate code for */
int qsgQmgr = 0; /* v602 ..is this a qsg qmgr ? (set in qmgr.c) */
pqList qListAnchor=0; /* point to first qlist element */
pqList qListTerm=(pqList)&qListAnchor; /* point to last qlist element */

int main(int argc, char *argv[]) {
	int parmCount = 1;
	CmdArgParms myParmStruc;
	CmdArgParmsPtr pMyParms = &myParmStruc;

	MQCHAR48 ReplyToQ; /* DynamicQName for this run       */

	MQHCONN hConn=MQHC_UNUSABLE_HCONN; /* handle to connected queue mgr   */
	MQLONG CompCode=0; /* MQ API completion code          */
	MQLONG Reason=0; /* Reason qualifying above         */
	MQLONG pcfReason=0; /* PCF Reason return code       */

	MQHOBJ hAdminQ=MQHO_UNUSABLE_HOBJ; /* handle to output queue          */
	MQHOBJ hReplyQ=MQHO_UNUSABLE_HOBJ; /* handle to input queue           */

	int ParmCount = 0;
	int QmgrVersion = 0; /* version number we detected */
	/*	int UserVersion = 0; *//* version number we detected */

	MQLONG ObjectSets=0; /* number of sets of objects to process */

	MQLONG RetCode = 0; /* Return code from this program */

	/*** v532 drive mqconnx for client version of supportpac ***/
#if defined USEMQCONNX
	MQCNO Connect_options = { MQCNO_DEFAULT}; /* MQCONNX options                 */
	int isClient = 1;
#else
	int isClient = 0;
#endif

	char connVerb[8]= "";
	int inValidParm=0;

	char work[12]; /* small work area (for platform name) */

	/*filename constructed from queue manager name or "saveqmgr.tst" */

	/* file handle for output MQSC file */
	FILE *outputfp = 0;
	/* file handle for output OAM file */
	FILE *authOutputfp = 0;
	int useSSL;

	/* variables for handling -o option */
	int i=0;

	/* default is to put +nl at the end of each line */
	strcpy(lineTerm, "+\n");

	/* ********************************************************************** */
	/* Get a connection to the queue manager and open the                     */
	/* required application queues.                                           */
	/*                                                                        */
	/* SYSTEM.ADMIN.COMMAND.QUEUE is the output queue for this application    */
	/* and the default command queue for the queue manager. Messages placed   */
	/* on this queue must be of a format suitable for processing by the       */
	/* supplied WebSphere MQ command server, STRMQCSV.                        */
	/*                                                                        */
	/* SAVEQMGR.* is the prefix for the name of the dynamic queue to which    */
	/* the command server will send the messages generated in response to     */
	/* our request. This will be based on the definition of the queue         */
	/* SYSTEM.DEFAULT.MODEL.QUEUE                                             */
	/* ********************************************************************** */

	/* A little hack to print which version of WebSphere MQ we were */
	/* compiled for .... */
#if defined(MQCMDL_LEVEL_701)
	fprintf(stderr, "Compiled for Websphere MQ V7.0.1.0 on %s\n", __DATE__);
#elif defined(MQOT_TOPIC)	
	fprintf(stderr, "Compiled for Websphere MQ V7.0 on %s\n", __DATE__);
#elif defined (MQOT_SERVICE)
	fprintf (stderr, "Compiled for Websphere MQ V6.0 on %s\n",__DATE__);
#elif defined  (MQCMD_INQUIRE_AUTH_INFO)
	fprintf (stderr, "Compiled for Websphere MQ V5.3 on %s\n",__DATE__);
#elif defined (MQCMDL_LEVEL_520)
	fprintf (stderr, "Compiled for MQSeries V5.2 on %s\n",__DATE__);
#elif defined (MQOO_BIND_ON_OPEN)
	fprintf (stderr, "Compiled for MQSeries V5.1 on %s\n",__DATE__);
#elif defined (MQDL_NOT_SUPPORTED)
	fprintf (stderr, "Compiled for MQSeries V5.0 on %s\n",__DATE__);
#else
	fprintf(stderr, "Compiled for MQSeries V2 on %s\n", __DATE__);
#endif
	i=0;
	/* Process input parameter list                                           */
	inValidParm = ProcessArgs(argc,argv, pMyParms,&parmCount,isClient );
	/*	fprintf(stderr,"Parm Count = %d\n", parmCount); */
#if defined MQSCO_DEFAULT
	if (diagnostics) {
		fprintf(stderr, "(saveqmgr.c) executing with SSL support enabled\n");
	}
	useSSL = 0;
#endif
	if(inValidParm) {
		/* invalid parameter */
		fprintf(stderr, "Invalid invocation, option: \"%s\"\n", argv[parmCount] );
		if (isClient) {
			fprintf(stderr, USAGE, pMyParms->myName,USAGE2 );
		} else {
			fprintf(stderr, USAGE, pMyParms->myName,USAGE3 );
		}
		fprintf(stderr, " \n\n %s\n\n", THISVERSION);
		exit( ERROR );
	}
	diagnostics = pMyParms->diagnostics;
	if (diagnostics) {
		fprintf(stderr, "(saveqmgr.c) executing with diagnostic level %d\n", diagnostics);
	}
	header = pMyParms->header;
	if (diagnostics>20) {
		fprintf(stderr, "(saveqmgr.c) executing with header %d\n", header);
	}
	authHeader = pMyParms->authHeader;
	if (pMyParms->std_out != 0 ) outputfp = stdout; /* user wants stdout option */
	chlReset = pMyParms->chlReset;
	SysObjs = pMyParms->SysObjs;
	if (diagnostics>2) {
		fprintf(stderr, "(saveqmgr.c) executing with SysObjs = %d\n", SysObjs );
	}
	noisy = pMyParms->noisy;
	permDyn = pMyParms->permDyn;
	ignoreDamaged = pMyParms->ignoreDamaged;
	oneLine = pMyParms->oneLine;
	strcpy(lineTerm, pMyParms->lineTerm);
	platform = pMyParms->platform;
	qsgQmgr = pMyParms->qsgQmgr;
	/*	UserVersion = pMyParms->UserVersion; */

	/** v532 use MQCONNX if USEMQCONNX set */
	fprintf(stderr, "%s\n", THISVERSION);

#if defined USEMQCONNX
	/******************************************************************/
	/*                                                                */
	/*   Initialise the client channel definition if required         */
	/*                                                                */
	/******************************************************************/
	if (pMyParms->flagCONNAME> 0)
	{
		strncpy(pMyParms->ClientConn.ConnectionName,
				pMyParms->argCONNAME,
				MQ_CONN_NAME_LENGTH);

		strncpy(pMyParms->ClientConn.ChannelName,
				pMyParms->argSVRCONN,
				MQ_CHANNEL_NAME_LENGTH);

		/* Point the MQCNO to the client connection definition */
		Connect_options.ClientConnPtr = &(pMyParms->ClientConn);

		fprintf(stderr, "Using the server connection channel \"%s\"\n",
				pMyParms->argSVRCONN);
		fprintf(stderr, "\ton connection name \"%s\".\n", pMyParms->argCONNAME);
		if (pMyParms->ClientConn.SecurityExit[0]) fprintf(stderr, "\tsecurity exit \"%.128s\"\n", pMyParms->ClientConn.SecurityExit);
		if (pMyParms->ClientConn.SecurityUserData[0]) fprintf(stderr, "\tsecurity user data \"%.32s\"\n", pMyParms->ClientConn.SecurityUserData);
		if (pMyParms->ClientConn.SendExit[0]) fprintf(stderr, "\tsend exit \"%.128s\"\n", pMyParms->ClientConn.SendExit);
		if (pMyParms->ClientConn.SendUserData[0]) fprintf(stderr, "\tsend user data \"%.32s\"\n", pMyParms->ClientConn.SendUserData);
		if (pMyParms->ClientConn.ReceiveExit[0]) fprintf(stderr, "\treceive exit \"%.128s\"\n", pMyParms->ClientConn.ReceiveExit);
		if (pMyParms->ClientConn.ReceiveUserData[0]) fprintf(stderr, "\treceive user data \"%.32s\"\n", pMyParms->ClientConn.ReceiveUserData);

		/* Client connection fields are in the version 2 part of the  */
		/* MQCNO so we must set the version number to 2 or they will  */
		/* be ignored, for SSL we must use version 4                 */

		if (pMyParms->flagSSL) {
#if defined USESSL 
			pMyParms->ClientConn.Version = MQCD_VERSION_7;
			Connect_options.Version = MQCNO_VERSION_4;
			Connect_options.SSLConfigPtr = &(pMyParms->SSL_options);

			fprintf(stderr, "\tSSL Cipher = %.32s\n", pMyParms->ClientConn.SSLCipherSpec);
			fprintf(stderr, "\tSSL KeyRepository = %.256s\n", pMyParms->SSL_options.KeyRepository);
#endif
		} else {
			Connect_options.Version = MQCNO_VERSION_2;
		}

	}
	else
	{
		fprintf(stderr, "With no client connection information specified.\n");
	}

	/******************************************************************/
	/*                                                                */
	/*   Connect to queue manager                                     */
	/*                                                                */
	/******************************************************************/
	strcpy(connVerb, "MQCONNX");
	MQCONNX(pMyParms->LclQMgrName, /* queue manager                  */
			&Connect_options, /* options for connection         */
			&hConn, /* connection handle              */
			&CompCode, /* completion code                */
			&Reason); /* reason code                    */

#else
	strcpy(connVerb, "MQCONN");
	MQCONN( pMyParms->LclQMgrName /* I  : use default queue manager  */
			, &hConn /*  O : queue manager handle       */
			, &CompCode /*  O : Completion code            */
			, &Reason /*  O : Reason qualifying CompCode */
	);

#endif  /* v532 */

	/* v3.2 check for failed, since os/400 can return a warning .... */

	if ( CompCode == MQCC_FAILED ) {
		fprintf(stderr, "%s failed for %s, CC=%ld RC=%ld\n"
				, connVerb
				, pMyParms->LclQMgrName
				, CompCode
				, Reason
		);
		exit( ERROR );
	} /* endif */

	CompCode = OpenQ( hConn
			, pMyParms->RmtQMgrName
			, pMyParms->CmdSvrQName
			, NULL
			, MQOO_OUTPUT
			, &hAdminQ
	);

	if (CompCode!=MQCC_FAILED) {
		strcpy( ReplyToQ, "SAVEQMGR.*" );

		CompCode = OpenQ( hConn
				, pMyParms->LclQMgrName
				, "SYSTEM.DEFAULT.MODEL.QUEUE\0"
				, &ReplyToQ
				, MQOO_INPUT_EXCLUSIVE
				, &hReplyQ
		);
	}/* End if*/

	if (CompCode==MQCC_FAILED) {
		MQDISC ( &hConn,
				&CompCode,
				&Reason
		);
		exit ( ERROR );

	}

	/* ******************************************************************** */
	/* QUEUE MANAGER                                                        */
	/* First, get the command level of the queue manager. We need this to   */
	/* tell us what further objects (such as namelists) we can query.       */
	/* ******************************************************************** */

	fprintf(stderr, "Requesting attributes of the queue manager...\n" );

	RetCode = InquireDefinition( hConn /* QMgr connection handle    */
			, hAdminQ /* Handle to open output Q   */
			, ReplyToQ /* Name of queue to reply to */
			, MQCMD_INQUIRE_Q_MGR /* Command to execute        */
			, 0 /* Attribute to search on    */
			, pMyParms /*command args */
	);
	if (RetCode!=ERROR) {
		RetCode = ProcessReplyQ( hConn /* QMgr connection handle    */
				, hReplyQ /* Handle to open input Q    */
				, &pMyParms->UserVersion /* Command level user is requesting */
				, &QmgrVersion /* Command level we detected        */
				, &outputfp /* [v51]output file handle */
				, pMyParms->fileName /* v32 Name of the output file  */
				, &authOutputfp /* [v51]output file handle */
				, pMyParms->authFileName /* v32 Name of the output file  */
				, 1 /* number of reply sets to process */
				, SysObjs /* system object flag [v51]  */
				, pMyParms->overlay /* v534 overlay flag */
				, pMyParms->twoLines /* v601 */
				, &pcfReason
				, pMyParms
		);

#if defined(zzzOS)
		/* if we got a MQRCCF_CFH_VERSION_ERROR, this is probably becuase we are connected to a zOS */
		/* qmgr, so set the platform to that and try again                       */
		if (pcfReason == MQRCCF_CFH_VERSION_ERROR) {
			if (diagnostics>10) {
				fprintf(stderr, "(saveqmgr.c) pcfReason = %ld\n", pcfReason);
			}
			platform = MQPL_MVS;
			RetCode = InquireDefinition( hConn /* QMgr connection handle    */
					, hAdminQ /* Handle to open output Q   */
					, ReplyToQ /* Name of queue to reply to */
					, MQCMD_INQUIRE_Q_MGR /* Command to execute        */
					, 0 /* Attribute to search on    */
					, pMyParms /*command args */
			);
			RetCode = ProcessReplyQ( hConn /* QMgr connection handle    */
					, hReplyQ /* Handle to open input Q    */
					, &pMyParms->UserVersion /* Command level user is requesting */
					, &QmgrVersion /* Command level we detected        */
					, &outputfp /* [v51]output file handle */
					, pMyParms->fileName /* v32 Name of the output file  */
					, &authOutputfp /* [v51]output file handle */
					, pMyParms->authFileName /* v32 Name of the output file  */
					, 1 /* number of reply sets to process */
					, SysObjs /* system object flag [v51]  */
					, pMyParms->overlay /* v534 overlay flag */
					, pMyParms->twoLines /* v601 */
					, &pcfReason
					, pMyParms
			);
		}
#endif

	}/* End if*/

	/* ******************************************************************** */
	/* Other objects:                                                       */
	/* Now we can request all the other objects that are appropriate for    */
	/* this level of the queue manager.  For example, mqv5 doesn't          */
	/* support listener objects                                             */
	/* ******************************************************************** */

	if (RetCode!=ERROR) {
		toStrMQPLATFORM((char*)&work, platform);
		fprintf(stderr, "Generating code for platform %s\n", work);
	}

	ObjectSets = 0; /* number of sets requested   */

#if defined (MQCMD_INQUIRE_AUTH_INFO)

	if (QmgrVersion >= MQCMDL_LEVEL_530 && /* We're V5.3 or better */
			pMyParms->UserVersion >= MQCMDL_LEVEL_530 /* and user wants authinfo */
	) {

		/* ******************************************************************** */
		/* [v53] Authinfo Objects                                                       */
		/* ******************************************************************** */

		if (RetCode!=ERROR) {
			ObjectSets++;
			fprintf(stderr, "Requesting attributes of all authinfo objects...\n" );
			RetCode = InquireDefinition( hConn /* QMgr connection handle    */
					, hAdminQ /* Handle to open output Q   */
					, ReplyToQ /* Name of queue to reply to */
					, MQCMD_INQUIRE_AUTH_INFO /* Command to execute        */
					, MQCA_AUTH_INFO_NAME /* Attribute to search on    */
					, pMyParms /*command args */
			);
		}/* End if retcode != ERROR */

	}
#endif

	if (RetCode!=ERROR) {
		ObjectSets++;
		fprintf(stderr, "Requesting attributes of all queues...\n" );
		RetCode = InquireDefinition( hConn /* QMgr connection handle    */
				, hAdminQ /* Handle to open output Q   */
				, ReplyToQ /* Name of queue to reply to */
				, MQCMD_INQUIRE_Q /* Command to execute        */
				, MQCA_Q_NAME /* Attribute to search on    */
				, pMyParms /*command args */
		);
	}/* End if retcode != ERROR */

	if (RetCode!=ERROR) {
		ObjectSets++;
		fprintf(stderr, "Requesting attributes of all channels...\n" );
		RetCode = InquireDefinition( hConn /* QMgr connection handle    */
				, hAdminQ /* Handle to open output Q   */
				, ReplyToQ /* Name of queue to reply to */
				, MQCMD_INQUIRE_CHANNEL /* Command to execute        */
				, MQCACH_CHANNEL_NAME /* Attribute to search on    */
				, pMyParms /*command args */
		);
	}/* End if retcode != ERROR */

	/* Special note.  Contrary to the documentation I have, it appears that */
	/* V5.1 of MQSeries includes client-conn channels when you ask for all  */
	/* channels.  Since we asked for all channel types above, we will not do */
	/* it again here ...                                                    */

	if (QmgrVersion < MQCMDL_LEVEL_510 && /* see BIG note above            */
			/*   MQAT_DEFAULT != MQAT_OS400 ) { *//* Os/400 doesn't support clients*/
			platform != MQPL_OS400 ) { /* Os/400 doesn't support clients*/

		if (RetCode!=ERROR) {
			ObjectSets++;
			RetCode = InquireDefinition( hConn /* QMgr connection handle    */
					, hAdminQ /* Handle to open output Q   */
					, ReplyToQ /* Name of queue to reply to */
					, XZCMD_INQUIRE_CHANNEL_CLNT /* Command to execute        */
					, MQCACH_CHANNEL_NAME /* Attribute to search on    */
					, pMyParms /*command args */
			);
		}/* End if retcode != ERROR */
	}

	if (RetCode!=ERROR) {
		ObjectSets++;
		fprintf(stderr, "Requesting attributes of all processes...\n" );
		RetCode = InquireDefinition( hConn /* QMgr connection handle    */
				, hAdminQ /* Handle to open output Q   */
				, ReplyToQ /* Name of queue to reply to */
				, MQCMD_INQUIRE_PROCESS /* Command to execute        */
				, MQCA_PROCESS_NAME /* Attribute to search on    */
				, pMyParms /*command args */
		);
	}/* End if retcode != ERROR */

#if defined (MQCMD_INQUIRE_NAMELIST)

	if (QmgrVersion >= MQCMDL_LEVEL_510 && /* We're V5.1 or better */
			pMyParms->UserVersion >= MQCMDL_LEVEL_510 /* and user wants namelists */
	) {

		/* ******************************************************************** */
		/* [v51] Namelists                                                       */
		/* ******************************************************************** */

		if (RetCode!=ERROR) {
			ObjectSets++;
			fprintf(stderr, "Requesting attributes of all namelists...\n" );
			RetCode = InquireDefinition( hConn /* QMgr connection handle    */
					, hAdminQ /* Handle to open output Q   */
					, ReplyToQ /* Name of queue to reply to */
					, MQCMD_INQUIRE_NAMELIST /* Command to execute        */
					, MQCA_NAMELIST_NAME /* Attribute to search on    */
					, pMyParms /*command args */
			);
		}/* End if retcode != ERROR */
	}
#endif

	/*** v6  Inquire on Listeners and services ***/
#if defined (zzMQV60)

	if (QmgrVersion >= MQCMDL_LEVEL_600 && /* We're V6.0 or better */
			pMyParms->UserVersion >= MQCMDL_LEVEL_600 && /* and user wants namelists */
			platform != MQPL_MVS /* and its not zOS */
	) {

		/* ******************************************************************** */
		/*  Listeners                                                           */
		/* ******************************************************************** */

		if (RetCode!=ERROR) {
			ObjectSets++;
			fprintf(stderr, "Requesting attributes of all listeners...\n" );
			RetCode = InquireDefinition( hConn /* QMgr connection handle    */
					, hAdminQ /* Handle to open output Q   */
					, ReplyToQ /* Name of queue to reply to */
					, MQCMD_INQUIRE_LISTENER /* Command to execute        */
					, MQCACH_LISTENER_NAME /* Attribute to search on    */
					, pMyParms /*command args */
			);
		}/* End if retcode != ERROR */

		/* ******************************************************************** */
		/*  Services                                                           */
		/* ******************************************************************** */

		if (RetCode!=ERROR) {
			ObjectSets++;
			fprintf(stderr, "Requesting attributes of all services...\n" );
			RetCode = InquireDefinition( hConn /* QMgr connection handle    */
					, hAdminQ /* Handle to open output Q   */
					, ReplyToQ /* Name of queue to reply to */
					, MQCMD_INQUIRE_SERVICE /* Command to execute        */
					, MQCA_SERVICE_NAME /* Attribute to search on    */
					, pMyParms /*command args */
			);
		}/* End if retcode != ERROR */

	}

	/* ******************************************************************** */
	/*  Authority Classes                                                   */
	/*  if the target qmgrs is NOT MVS, and at least V6                     */
	/*  OR the target qmgr is at least v7, all platforms                    */
	/* ******************************************************************** */
	if (pMyParms->makeSecurityDefs) {
		if (( QmgrVersion >= MQCMDL_LEVEL_600 && (
								platform==MQPL_UNIX ||
								platform==MQPL_WINDOWS_NT ||
								platform==MQPL_OS400 )) || (QmgrVersion >= MQCMDL_LEVEL_700 && platform!=MQPL_MVS)) {
			if (RetCode!=ERROR) {
				ObjectSets++;
				fprintf(stderr, "Requesting attributes of all OAM classes...\n" );
				RetCode = InquireDefinition( hConn /* QMgr connection handle    */
						, hAdminQ /* Handle to open output Q   */
						, ReplyToQ /* Name of queue to reply to */
						, MQCMD_INQUIRE_AUTH_RECS /* Command to execute        */
						, 0 /* Attribute to search on    */
						, pMyParms /*command args */
				);
			}
		} else {
			fprintf(stderr, "(saveqmgr.c) -z flag specified but target ");
			fprintf(stderr, "qmgr is wrong platform or < v6, flag is ignored\n");
			RetCode=WARNING;

		}

	}
#endif  /* of zzMQV60 */
#if defined(zzMQV70)
	/*Complete:  Need a runtime check for this as well, so we do not inquire against a non v7 qmgr.*/
	if (QmgrVersion >= MQCMDL_LEVEL_700) { /* We're V7.0 or better */
		if (RetCode!=ERROR) {
			ObjectSets++;
			fprintf(stderr, "Requesting attributes of all topics...\n" );
			RetCode = InquireDefinition( hConn /* QMgr connection handle    */
					, hAdminQ /* Handle to open output Q   */
					, ReplyToQ /* Name of queue to reply to */
					, MQCMD_INQUIRE_TOPIC /* Command to execute        */
					, MQCA_TOPIC_NAME /* Attribute to search on    */
					, pMyParms /*command args */
			);
		}/* End if retcode != ERROR */
		if (RetCode!=ERROR) {
			ObjectSets++;
			fprintf(stderr, "Requesting attributes of all subscriptions...\n" );
			RetCode = InquireDefinition( hConn /* QMgr connection handle    */
					, hAdminQ /* Handle to open output Q   */
					, ReplyToQ /* Name of queue to reply to */
					, MQCMD_INQUIRE_SUBSCRIPTION /* Command to execute        */
					, MQCACF_SUB_NAME /* Attribute to search on    */
					, pMyParms /*command args */
			);
		}/* End if retcode != ERROR */
	}
#endif
#if defined(zzzOS)
	if (platform==MQPL_MVS) {

		/* ******************************************************************** */
		/*  Storage Classes                                                     */
		/* ******************************************************************** */

		if (RetCode!=ERROR) {
			ObjectSets++;
			fprintf(stderr, "Requesting attributes of all storage classes...\n" );
			RetCode = InquireDefinition( hConn /* QMgr connection handle    */
					, hAdminQ /* Handle to open output Q   */
					, ReplyToQ /* Name of queue to reply to */
					, MQCMD_INQUIRE_STG_CLASS /* Command to execute        */
					, MQCA_STORAGE_CLASS /* Attribute to search on    */
					, pMyParms /*command args */
			);
		}

		/* ******************************************************************** */
		/*  LOG data sets                                                     */
		/* ******************************************************************** */

		if (RetCode!=ERROR) {
			ObjectSets++;
			fprintf(stderr, "Requesting attributes of all logs...\n" );
			RetCode = InquireDefinition( hConn /* QMgr connection handle    */
					, hAdminQ /* Handle to open output Q   */
					, ReplyToQ /* Name of queue to reply to */
					, MQCMD_INQUIRE_LOG /* Command to execute        */
					, 0 /* Attribute to search on    */
					, pMyParms /*command args */
			);
		}

		/* ******************************************************************** */
		/*  Archive Information                                                 */
		/* ******************************************************************** */

		if (RetCode!=ERROR) {
			ObjectSets++;
			fprintf(stderr, "Requesting attributes of archives...\n" );
			RetCode = InquireDefinition( hConn /* QMgr connection handle    */
					, hAdminQ /* Handle to open output Q   */
					, ReplyToQ /* Name of queue to reply to */
					, MQCMD_INQUIRE_ARCHIVE /* Command to execute        */
					, 0 /* Attribute to search on    */
					, pMyParms /*command args */
			);
		}

		/* ******************************************************************** */
		/*  Inquire Usage                                                       */
		/* ******************************************************************** */

		if (RetCode!=ERROR) {
			ObjectSets++;
			fprintf(stderr, "Requesting Usage Information (MQCMD_INQUIRE_USAGE)...\n" );
			RetCode = InquireDefinition( hConn /* QMgr connection handle    */
					, hAdminQ /* Handle to open output Q   */
					, ReplyToQ /* Name of queue to reply to */
					, MQCMD_INQUIRE_USAGE /* Command to execute        */
					, 0 /* Attribute to search on    */
					, pMyParms /*command args */
			);
		}
		/* ******************************************************************** */
		/*  Inquire System                                                      */
		/* ******************************************************************** */

		if (RetCode!=ERROR) {
			ObjectSets++;
			fprintf(stderr, "Requesting System Information (MQCMD_INQUIRE_SYSTEM)...\n" );
			RetCode = InquireDefinition( hConn /* QMgr connection handle    */
					, hAdminQ /* Handle to open output Q   */
					, ReplyToQ /* Name of queue to reply to */
					, MQCMD_INQUIRE_SYSTEM /* Command to execute        */
					, 0 /* Attribute to search on    */
					, pMyParms /*command args */
			);
		}
		/* ******************************************************************** */
		/*  CF Structure                                                        */
		/* ******************************************************************** */

		if (RetCode!=ERROR && qsgQmgr) {
			ObjectSets++;
			fprintf(stderr, "Requesting attributes of all CF Structures...\n" );
			RetCode = InquireDefinition( hConn /* QMgr connection handle    */
					, hAdminQ /* Handle to open output Q   */
					, ReplyToQ /* Name of queue to reply to */
					, MQCMD_INQUIRE_CF_STRUC /* Command to execute        */
					, MQCA_CF_STRUC_NAME /* Attribute to search on    */
					, pMyParms /*command args */
			);
		}

	}

#endif  /* of zzzOs */

	/* ******************************************************************** */
	/* Now that we have requested all the required object attributes,       */
	/* call ProcessReplyQ to handle the responses.  ObjectSets tell us      */
	/* how many sets of replies there are.                                  */
	/* ******************************************************************** */

	if (RetCode!=ERROR) {
		RetCode = ProcessReplyQ( hConn /* QMgr connection handle    */
				, hReplyQ /* Handle to open input Q    */
				, &pMyParms->UserVersion /* Command level user is requesting */
				, &QmgrVersion /* Command level we detected        */
				, &outputfp /* [v51]output file handle */
				, pMyParms->fileName /* v32 Name of the output file  */
				, &authOutputfp
				, pMyParms->authFileName
				, ObjectSets /* number of sets of replies to process */
				, SysObjs /* system object flag [v51]  */
				, pMyParms->overlay /* v534 overlay flag */
				, pMyParms->twoLines /* v601 two lines flag */
				, &pcfReason
				, pMyParms
		);
	}

	/* ******************************************************************** */
	/* Close down in an orderly manner.                                     */
	/*                                                                      */
	/* No need to test for these calls failing, the result is the same!     */
	/* ******************************************************************** */
	MQCLOSE( hConn /* Queue manager handle      */
			, &hAdminQ /* Handle to queue           */
			, MQCO_NONE /* Parameters on close       */
			, &CompCode /* Completion code           */
			, &Reason /* Reason code               */
	);

	MQCLOSE( hConn /* Queue manager handle      */
			, &hReplyQ /* Handle to queue           */
			, MQCO_NONE /* Parameters on close       */
			, &CompCode /* Completion code           */
			, &Reason /* Reason code               */
	);

	/* [v51] close output file */
	if (outputfp) fclose (outputfp);

	/* [v603] close output file */
	if (authOutputfp) fclose (authOutputfp);

	if (hConn!=MQHC_UNUSABLE_HCONN) {
		MQDISC( &hConn
				, &CompCode
				, &Reason
		);
	}/* End if*/

	/* we do this so we can return the correct "status" based on the platfrm */
	/* saveqmgr.h defines the actual return codes by platform */
	switch (RetCode) {
		case (MQCC_OK): return (OK);
		case (MQCC_WARNING): return (WARNING);
		case (MQCC_FAILED): return (ERROR);
		default: return(RetCode);
	}
} /* end MAIN */

/* ************************************************************************ */
/* InquireDefinition()                                                      */
/*                                                                          */
/* This is a procedure to inquire of the queue manager about the objects    */
/* defined to it.                                                           */
/*                                                                          */
/* SearchType indicates the type of object for which information is         */
/* required. Objects can either be queues, channels, processes or the       */
/* queue manager itself.                                                    */
/*                                                                          */
/* SearchParm indicates the parameter on which the request is based. A      */
/* SearchString specifies a 'mask' for the search. So, for example, a       */
/* SearchParm of MQCA_Q_NAME and a SearchString of "*" will request         */
/* information about all queues.                                            */
/*                                                                          */
/* SearchParmLen indicates the maximum length of a string of this type,     */
/* SearchStringLen indicates the length of the search mask. Note that       */
/* NULL characters in the SearchString are treated as valid. The            */
/* SearchString should, therefore, be padded with blanks (' ').             */
/*                                                                          */
/*                                                                          */
/* The actual request consists of a Request Header and a parameter block    */
/* used to specify the context for the search. The header and the parameter */
/* block follow each other in a contiguous buffer which is pointed  to by   */
/* the variable pAdminMsg. This entire buffer is then put to  the queue.    */
/*                                                                          */
/* The command server, (use STRMQCSV to start it), processes the            */
/* SYSTEM.ADMIN.COMMAND.QUEUE and puts a reply on the application           */
/* ReplyToQ for each defined queue.                                         */
/* ************************************************************************ */
MQLONG InquireDefinition(MQHCONN hConn, MQHOBJ hAdminQ, MQCHAR48 ReplyToQ,
		MQLONG SearchType /* QUEUE|CHANNEL|PROCESS|QMGR */
		, MQLONG SearchParm, CmdArgParmsPtr MyParms) {
	MQLONG AdminMsgLen; /* Length of user message buffer   */
	MQBYTE *pAdminMsg; /* Ptr to outbound data buffer     */
	MQCFH *pPCFHeader; /* Ptr to PCF header structure     */
	MQCFST *pPCFString; /* Ptr to PCF string parm block    */
	MQCFIN *pPCFInteger; /* v32 ptr to chltype param for clntconn */
	MQCFIL *pPCFIntegerList; /* v32 ptr to chltype param for clntconn */
	MQCHAR CmdSvrQName[MQ_Q_MGR_NAME_LENGTH+1];
	MQCHAR12 UserID;
#if defined(zzzOS)||defined(zzMQV70)
	MQCFBS *pPCFByte; /* Ptr to PCF byte string parm block */
#endif
	MQLONG RetCode = 0; /* Return code from function */
	/* ----------------------------------------------------------------------- */
	/* The search for queue manager attributes is slightly different to that   */
	/* for other objects, in that there is only one.                           */
	/* The PCF header is the only piece of the request message required in     */
	/* this case.                                                              */
	/*                                                                         */
	/* When requesting attributes for queues, channels or processes a          */
	/* parameter block follows the header in which you specify the context     */
	/* of the request. That is, you specify the parameter to base the          */
	/* request on, and a mask to search against. In this example we want       */
	/* all attributes for all objects so we base our search on object name     */
	/* and use the '*' wildcard to indicate a generic search.                  */
	/*                                                                         */
	/* This procedure will eventually perform an MQPUT to the                  */
	/* SYSTEM.ADMIN.COMMAND.QUEUE.  The request outlined above will be         */
	/* encapsulated in a single message occupying a contiguous block of        */
	/* storage. To help with manipulation of this buffer we will use           */
	/* several pointers into the buffer.                                       */
	/*                                                                         */
	/* pAdminMsg points to the start of the entire message buffer.             */
	/* This is a pointer to a flat block of storage allocated to be large      */
	/* enough to contain the entire request.                                   */
	/*                                                                         */
	/* pPCFHeader points to a structure of type MQCFH, a command format        */
	/* header. It is points to the same address as pAdminMsg and               */
	/* effectively remaps the first piece of our message buffer. It is         */
	/* used to indicate the type of command we wish to execute and the         */
	/* number of parameter blocks following in the message buffer.             */
	/*                                                                         */
	/* pPCFString points to a structure of type MQCFST, a command format       */
	/* string. It points into the message buffer at an address immediately     */
	/* following the header described above. It is used to map the following   */
	/* bytes onto a PCF string parameter block which in this case is used      */
	/* to indicate the name of the object we want details about, * indicating  */
	/* all.                                                                    */
	/*                                                                         */
	/* Note that this example is a generic search for all attributes of        */
	/* all objects known to the queue manager. By using different,             */
	/* or more, parameter blocks in the request header it is possible          */
	/* to narrow the search.                                                   */
	/* ----------------------------------------------------------------------- */
	if (SearchParm == 0) {
		/* Inquire of the queue manager (or log) attributes. It is only necessary */
		/* to complete the request header in this case.                  */

		/* Set the length for the message buffer */
		AdminMsgLen = MQCFH_STRUC_LENGTH;

		/* Allocate storage for the message buffer and set a pointer to */
		/* its start address.                                           */
		pAdminMsg = (MQBYTE *)malloc(AdminMsgLen);
		memset(pAdminMsg, '\0', AdminMsgLen);

		/* pPCFHeader is set equal to pAdminMsg in order to provide a */
		/* structure to the newly allocated block of storage. We can  */
		/* then create a request header of the correct format in the  */
		/* message buffer.                                            */
		pPCFHeader = (MQCFH *)pAdminMsg;

		/* Setup request header */
		pPCFHeader->StrucLength = MQCFH_STRUC_LENGTH;
		pPCFHeader->Type = MQCFT_COMMAND;
		pPCFHeader->Command = SearchType;
		pPCFHeader->Version = MQCFH_VERSION_1;
		pPCFHeader->MsgSeqNumber = MQCFC_LAST;
		pPCFHeader->Control = MQCFC_LAST;

#if defined (zzzOS)
		if (platform == MQPL_MVS) {
			pPCFHeader->Type = MQCFT_COMMAND_XR; /* zOS */
			pPCFHeader->Version = MQCFH_VERSION_3; /* zOS */
		}
#endif
		pPCFHeader->ParameterCount = 0;

	} else {
		/* Inquire of queue, channel or process attributes; exact one to */
		/* be indicated by SearchType.                                   */

		/* Set the length for the message buffer */
		AdminMsgLen = MQCFH_STRUC_LENGTH + MQCFST_STRUC_LENGTH_FIXED + 4;

		/* Allocate storage for the message buffer and set a pointer to */
		/* its start address.                                           */
		pAdminMsg = (MQBYTE *)malloc(AdminMsgLen);

		memset(pAdminMsg, '\0', AdminMsgLen);

		/* pPCFHeader is set equal to pAdminMsg in order to provide a */
		/* structure to the newly allocated block of storage. We can  */
		/* then create a request header of the correct format in the  */
		/* message buffer.                                            */
		pPCFHeader = (MQCFH *)pAdminMsg;

		/* pPCFString is set to point into the message buffer immediately */
		/* after the request header. This allows us to specify a context  */
		/* for the request in the required format.                        */
		pPCFString = (MQCFST *)(pAdminMsg + MQCFH_STRUC_LENGTH );

		/* Setup request header */
		pPCFHeader->Type = MQCFT_COMMAND;
		pPCFHeader->StrucLength = MQCFH_STRUC_LENGTH;
		pPCFHeader->Version = MQCFH_VERSION_1;
		pPCFHeader->Command = SearchType;
		pPCFHeader->MsgSeqNumber = MQCFC_LAST;
		pPCFHeader->Control = MQCFC_LAST;
		pPCFHeader->ParameterCount = 1;

		/* Setup parameter block */
		pPCFString->Type = MQCFT_STRING;
		pPCFString->StrucLength = MQCFST_STRUC_LENGTH_FIXED + 4;
		pPCFString->Parameter = SearchParm;
		pPCFString->CodedCharSetId = MQCCSI_DEFAULT;
		pPCFString->StringLength = 1;
		memset(pPCFString->String, '\0', 4);
		strcpy(pPCFString->String, "*");

#if defined (zzzOS)

		if (platform == MQPL_MVS) {
			pPCFHeader->Type = MQCFT_COMMAND_XR; /* zOS */
			pPCFHeader->Version = MQCFH_VERSION_3; /* zOS */
		}
#endif

	} /* endif */

	/* v32 More additions for clntconn channels */
	/* V32 add integer attribute chltype to query   */
	if (SearchType == XZCMD_INQUIRE_CHANNEL_CLNT) {
		/* V32 change some previously set values */
		AdminMsgLen += MQCFIN_STRUC_LENGTH;
		pAdminMsg = (MQBYTE *) realloc( (void *) pAdminMsg, AdminMsgLen);
		pPCFHeader = (MQCFH *)pAdminMsg;

		/*  V32  memset( pAdminMsg, '\0', AdminMsgLen ); */
		pPCFHeader->Command = MQCMD_INQUIRE_CHANNEL;
		pPCFHeader->ParameterCount = 2;

		/* V32 fill in values for clntconn channel type */
		pPCFInteger = (MQCFIN *)(pAdminMsg + MQCFH_STRUC_LENGTH
				+ MQCFST_STRUC_LENGTH_FIXED + 4 );

		pPCFInteger->Type = MQCFT_INTEGER;
		pPCFInteger->StrucLength = MQCFIN_STRUC_LENGTH;
		pPCFInteger->Parameter = MQIACH_CHANNEL_TYPE;
		pPCFInteger->Value = MQCHT_CLNTCONN;
	}

	/* V32 End Clntconn addition */

#if defined(zzMQV60)
	/* v603 Special handling for Authority Objects */

	if (SearchType == MQCMD_INQUIRE_AUTH_RECS) {

		/*  change some previously set values */
		AdminMsgLen = MQCFH_STRUC_LENGTH + 2*MQCFIN_STRUC_LENGTH +
		/* needs to be fullword */
		MQCFST_STRUC_LENGTH_FIXED + MQCFIL_STRUC_LENGTH_FIXED + 8;

		pAdminMsg = (MQBYTE *) realloc( (void *) pAdminMsg, AdminMsgLen);
		pPCFHeader = (MQCFH *)pAdminMsg;

		pPCFHeader->ParameterCount = 4;

		/* set the Options field */
		pPCFInteger = (MQCFIN *)(pAdminMsg + MQCFH_STRUC_LENGTH);
		pPCFInteger->Type = MQCFT_INTEGER;
		pPCFInteger->StrucLength = MQCFIN_STRUC_LENGTH;
		pPCFInteger->Parameter = MQIACF_AUTH_OPTIONS;
		pPCFInteger->Value
				= MQAUTHOPT_ENTITY_EXPLICIT+MQAUTHOPT_NAME_ALL_MATCHING;
		/* set the ObjectType field */
		pPCFInteger = (MQCFIN *)(pAdminMsg + MQCFH_STRUC_LENGTH
				+ MQCFIN_STRUC_LENGTH);
		pPCFInteger->Type = MQCFT_INTEGER;
		pPCFInteger->StrucLength = MQCFIN_STRUC_LENGTH;
		pPCFInteger->Parameter = MQIACF_OBJECT_TYPE;
		pPCFInteger->Value = MQOT_ALL;
		/* set the Profile Name field */
		pPCFString = (MQCFST *)(pAdminMsg + MQCFH_STRUC_LENGTH + 2
				* MQCFIN_STRUC_LENGTH);
		pPCFString->Type = MQCFT_STRING;
		pPCFString->StrucLength = MQCFST_STRUC_LENGTH_FIXED + 4;
		pPCFString->Parameter = MQCACF_AUTH_PROFILE_NAME;
		pPCFString->CodedCharSetId = MQCCSI_DEFAULT;
		pPCFString->StringLength = 1;
		memset(pPCFString->String, '\0', 4);
		/* indicate required attributes */
		pPCFIntegerList = (MQCFIL *)(pAdminMsg + MQCFH_STRUC_LENGTH + 2
				* MQCFIN_STRUC_LENGTH + MQCFST_STRUC_LENGTH_FIXED + 4);
		pPCFIntegerList->Type = MQCFT_INTEGER_LIST;
		pPCFIntegerList->StrucLength = MQCFIL_STRUC_LENGTH_FIXED + 4;
		pPCFIntegerList->Parameter = MQIACF_AUTH_PROFILE_ATTRS;
		pPCFIntegerList->Count = 1;
		pPCFIntegerList->Values[0] = MQIACF_ALL;

	}

	/* V603 end of authority objects */
#endif

	if (MyParms->CmdSvrQName[0]!='\0') {

	} else {
	}
	/* Put the newly created message to the input queue for the */
	/* WebSphere MQ command server                              */
	RetCode = PutMsg(hConn, MQFMT_ADMIN, hAdminQ,
			"SYSTEM.ADMIN.COMMAND.QUEUE\0", ReplyToQ, (MQBYTE *)pAdminMsg ,
			AdminMsgLen, UserID);

	/* Free the storage allocated to the message buffer */
	if (pAdminMsg)
		free(pAdminMsg);

	return (RetCode);

}

/* ************************************************************************ */
/* ProcessReplyQ()                                                          */
/*                                                                          */
/* The WebSphere MQ command server has sent replies to our previous request */
/* to the applications ReplyToQ. We are passed a handle to the open         */
/* queue, hReplyQ.                                                          */
/*                                                                          */
/* Command is used to indicate the request that was made of the command     */
/* server in order that we can process the replies appropriately.           */
/*                                                                          */
/* There will be one message on the ReplyToQ for each defined object of     */
/* the requested type. The header of each message will indicate whether     */
/* or not it is the last reply. This procedure will run until all replies   */
/* have been received from the command server.                              */
/*                                                                          */
/* An individual Reply message consists of a header followed by a           */
/* number a parameters, the exact number, type and order will depend        */
/* upon the type of request.                                                */
/*                                                                          */
/* ************************************************************************ */
MQLONG ProcessReplyQ(MQHCONN hConn, MQHOBJ hReplyQ, int *pUserVersion,
		int * pQmgrVersion, FILE **poutputfp /* v3.3 pointer to pointer to the  output file  */
		, char * fileName /* v32 Name of the output file  */
		, FILE **pauthOutputfp /* v3.3 pointer to pointer to the  output file  */
		, char * authFileName /* v32 Name of the output file  */
		, MQLONG ObjectSets /* number of sets of objects to get */
		, int SysObjs /* [v51] System Object Flags */
		, int overlay /* v534 output file overlay flag */
		, int twoLines, MQLONG *pPcfReason /* pcf Reason code */
		, CmdArgParmsPtr pMyParms) {
	MQLONG AdminMsgLen=0; /* Length of user message buffer   */
	MQBYTE *pAdminMsg=NULL; /* Ptr to outbound data buffer     */
	MQCFH *pPCFHeader=NULL; /* Ptr to PCF header structure     */
	MQCFST *pPCFString=NULL; /* Ptr to PCF string parm block    */
#if defined(zzzOS)||defined(zzMQV70)
	MQCFBS *pPCFByte=NULL; /* Ptr to PCF string parm block    */
#endif
	MQCFSL *pPCFStringList=NULL;/* Ptr to PCF string parm block    */
	MQCFIL *pPCFIntegerList=NULL;/* Ptr to PCF integer parm block    */
	MQCFIN *pPCFInteger=NULL; /* Ptr to PCF integer parm block   */
	MQLONG *pPCFType=NULL; /* Type field of PCF message parm  */

	QParms QDefn; /*                                 */
	ChlParms ChlDefn; /*                                 */
	ProcParms ProcDefn; /*                                 */
	QMgrParms QMgrDefn; /*                                 */

#if defined (MQCMD_INQUIRE_NAMELIST)
	NameLParms *pNlDefn=NULL; /* Namelist [v51] pointer            */
#endif

	/* v53 - authinfo object */
#if defined (MQCMD_INQUIRE_AUTH_INFO)
	AuthInfoParms AuthInfoDefn; /* Authinfo [v53] structure */
#endif

#if defined(zzMQV60)
	ListenerParms ListenerDefn; /* Listener Attributes */
	ServiceParms ServiceDefn; /* Service Attributes */
	OAMParms OAMDefn; /* oam Attributes */
#endif
#if defined(zzMQV70)
	TopicParms TopicDefn; /* Topic Attributes */
	SubscrParms *pSubscriptDefn=NULL; /* Subscription Attributes */
#endif
#if defined(zzzOS)
	StorageClassParms StgClassDefn; /* Storage Class Attributes */
	LogParms LogDefn; /* Log Attributes */
	CFStructParms CFStructDefn; /* CF Structure Attributes */
	UsageParms UsageDefn; /* Usage Attributes */
	SystemParms SystemDefn; /* System Attributes */
	ArchiveParms ArchiveDefn; /* Archive Attributes */
#endif

	short Index=0; /* Loop counter                    */
	int i=0; /* V32 loop control */

	char tempfileName[FILENAMESIZE+1]; /* tempory to build filename */

	int firstChannel =2;
	int firstQueue =2;
	int firstNamelist =2;
	int firstProcess =2;
	int firstQMgr =2;
	int firstAuthInfo =2; /* v530 authinfo */
#if defined(zzMQV60)
	int firstListener =2;
	int firstService =2;
	int firstOAM =2;
#endif
#if defined(zzMQV70)
	int firstTopic =2;
	int firstSubscript=2;
#endif
#if defined(zzzOS)
	int firstStgClass=1;
	int firstUsage=1;
	int firstCFStruct=1;
	int firstSystem=1;
	int firstLog=1;
	int firstArchive=1;
#endif  
	int skipNext = 0;
	MQLONG RetCode= MQCC_OK;
	int hasSysName = 0;

	char ObjectName[49]; /* v531a */

	/* ********************************************************************** */
	/*                                                                        */
	/* The message will be retrieved into a buffer pointed to by              */
	/* pAdminMsg. This buffer is to have been allocated to be large           */
	/* enough to hold all the parameters for a object type we are             */
	/* expecting, indicated by the input parameter 'Command'.                 */
	/*                                                                        */
	/* pPCFHeader is then allocated to point also to the beginning of         */
	/* the buffer and is used to access the PCF header structure. The         */
	/* header contains several fields. The one we are specifically            */
	/* interested in is the ParameterCount. This tells us how many            */
	/* parameters follow the header in the message buffer. There is           */
	/* one parameter for each local queue attribute known by the              */
	/* queue manager.                                                         */
	/*                                                                        */
	/* At this point we do not know the order or type of each parameter       */
	/* block in the buffer, the first MQLONG of each block defines its        */
	/* type; they may be parameter blocks containing either strings or        */
	/* integers.                                                              */
	/*                                                                        */
	/* pPCFType is used initially to point to the first byte beyond the       */
	/* known parameter block. Initially then, it points to the first byte     */
	/* after the PCF header. Subsequently it is incremented by the length     */
	/* of the identified parameter block and therefore points at the          */
	/* next. Looking at the value of the data pointed to by pPCFType we       */
	/* can decide how to process the next group of bytes, either as a         */
	/* string, or an integer.                                                 */
	/*                                                                        */
	/* In this way we parse the message buffer extracting the values of       */
	/* each of the parameters we are interested in.                           */
	/*                                                                        */
	/* ********************************************************************** */

	/* clear out all fields in the storage arrays */
	memset( &QDefn, '\0', sizeof(QDefn));
	memset( &ChlDefn, '\0', sizeof(ChlDefn));

#ifdef zzMQV60
	ChlDefn.HdrComp[0]= MQCOMPRESS_NONE;
	ChlDefn.MsgComp[0]= MQCOMPRESS_NONE;

	for (i=1; i<COMP_ARRAY_SIZE; i++) {
		ChlDefn.HdrComp[i]= MQCOMPRESS_NOT_AVAILABLE;
		ChlDefn.MsgComp[i]= MQCOMPRESS_NOT_AVAILABLE;
	}
	ChlDefn.Weight = 50;
#endif

	ChlDefn.KAInt = -999; /* special value to indicate not to set KAINT */
	memset( &ProcDefn, '\0', sizeof(ProcDefn));
	memset( &QMgrDefn, '\0', sizeof(QMgrDefn));

#ifdef zzMQV60
	QMgrDefn.ClwlMRUChl = 999999999;
	QMgrDefn.StatsIntv = 1800;
	QMgrDefn.ActngInt = 1800;

#endif

#if defined (MQCMD_INQUIRE_AUTH_INFO)
	memset( &AuthInfoDefn, '\0', sizeof(AuthInfoDefn)); /* v530 */
#endif

#if defined (MQCMD_INQUIRE_NAMELIST)

	/* [v51] becuase namelist take a lot of storage (256*48 bytes */
	/* we'll  malloc the storage instead of getting from stack   */

	if (!(pNlDefn = malloc(sizeof(NameLParms)))) {
		fprintf(stderr, "Failure to allocate storage for namelist!\n");
		return (ERROR);
	}
#endif

#if defined(zzMQV60)
	memset( &ListenerDefn, '\0', sizeof(ListenerDefn));
	memset( &ServiceDefn, '\0', sizeof(ServiceDefn));
#endif
#ifdef zzMQV70
	memset( &TopicDefn, '\0', sizeof(TopicDefn));
	if (!(pSubscriptDefn = malloc(sizeof(SubscrParms)))) {
		fprintf(stderr, "Failure to allocate storage for subscriptions!\n");
		return (ERROR);
	}
	/* consider mallocing SubscriptDefn instead of taking from stack... */
#endif
	/* AdminMsgLen is to be set to the length of the expected reply       */
	/* message. If INT_BUFSZE turns out to be too small, GetMsg will      */
	/* automatically allocate a larger buffer for us                      */

	AdminMsgLen = INT_BUFSZE;
	if (!(pAdminMsg = (MQBYTE *)malloc(AdminMsgLen))) {
		fprintf(stderr, "Failure to allocate %ld bytes for input buffer!\n",
		AdminMsgLen);
		return (ERROR);
	}

	/* BIG do while loop */
	do {
		RetCode = GetMsg(hConn /* Queue manager handle */
		, hReplyQ /* Get queue handle     */
		, (MQBYTE **)&pAdminMsg /* pointer to message area*/
		, &AdminMsgLen, pMyParms);

		if (RetCode!=MQCC_OK)
			break;

		/* Point to Header */
		pPCFHeader = (MQCFH *)pAdminMsg;

		if (diagnostics>10) {
			fprintf(stderr, "(saveqmgr.c) processing a response:\n");
			fprintf(stderr, "\tType = %ld\n", pPCFHeader -> Type);
			fprintf(stderr, "\tStrucLength = %ld\n", pPCFHeader -> StrucLength);
			fprintf(stderr, "\tVersion = %ld\n", pPCFHeader -> Version);
			fprintf(stderr, "\tCommand = %ld\n", pPCFHeader -> Command);
			fprintf(stderr, "\tMsgSeqNumber = %ld\n", pPCFHeader -> MsgSeqNumber);
			fprintf(stderr, "\tControl = %ld\n", pPCFHeader -> Control);
			fprintf(stderr, "\tCompCode = %ld\n", pPCFHeader -> CompCode);
			fprintf(stderr, "\tReason = %ld\n", pPCFHeader -> Reason);
			fprintf(stderr, "\tParameterCount = %ld\n",
			pPCFHeader -> ParameterCount);
			fprintf(stderr, "\t(ObjectSets) = %ld\n", ObjectSets);
		}

		*pPcfReason = pPCFHeader -> Reason; /* set the reasno for the mainline */

		/* Ignore summary records from zOS, we never asked for them !*/
#if defined (zzzOS)
		if (pPCFHeader->Type != MQCFT_XR_SUMMARY) {
#endif
			if (pPCFHeader->Reason) {
				/* v513 ... some platforms return an error if no client channels */
				/* exist, so we'll put out a warning message instead of the      */
				/* more generic pcf error message.                               */

				if (pPCFHeader->Reason==MQRCCF_CHANNEL_NOT_FOUND) {
					if (noisy)
						fprintf(stderr, "No client channels found\n");
				} else

				if (pPCFHeader->Reason==MQRC_OBJECT_DAMAGED && ignoreDamaged) {
					memset(ObjectName, '\0', 49);

					/* v531a attempt to print damaged object*/
					if (pPCFHeader -> ParameterCount) {
						pPCFType = (MQLONG *)(pAdminMsg + MQCFH_STRUC_LENGTH);
						if (*pPCFType == MQCFT_STRING) {
							pPCFString = (MQCFST *)pPCFType;
							memcpy(ObjectName, pPCFString->String,
									pPCFString->StringLength);
							fprintf(stderr, "Damaged Object name = %s\n",
							ObjectName);
						}
					}

					fprintf(stderr,
					"Ignoring 'damaged object' PCF error messsage\n");
					noMsgOnTimeout=1; /* v531a */
					if (*poutputfp) {
						fprintf(*poutputfp, "* ===== \n");
						fprintf(*poutputfp,
								"* ==>>> Ignoring 'damaged object' PCF error messsage\n");
						fprintf(*poutputfp,
								"* ==>>> Damaged Object name is: %s\n",
								ObjectName);
						fprintf(*poutputfp, "* ===== \n");
					}
					RetCode = WARNING;
				} else

				/* v530a */
				/* okay...here's another hack, it appears that if you have NO process definitions */
				/* the cmd server returns a pcf message with MQRC_UNKNOWN_OBJECT_NAME!            */
				/* I haven't tested it, but its probably the case with other object types as well */
				/* so we'll add this to our list of ignorable error codes                         */

				if (pPCFHeader->Reason==MQRC_UNKNOWN_OBJECT_NAME
						&& ignoreDamaged) {
					fprintf(stderr,
					"Ignoring 'unknown object' PCF error messsage\n");
					noMsgOnTimeout=1; /* v531a */
					if (*poutputfp) {
						fprintf(*poutputfp, "* ===== \n");
						fprintf(*poutputfp,
								"* ==>>> Ignoring 'unknown object' PCF error messsage\n");
						fprintf(*poutputfp, "* ===== \n");
					}
					RetCode = WARNING;
				} else if (pPCFHeader->Reason==MQRCCF_COMMAND_FAILED
						&& ignoreDamaged) {

					fprintf(stderr,
					"Ignoring 'command failed' PCF error messsage\n");
					noMsgOnTimeout=1; /* v531a */
					if (*poutputfp) {
						fprintf(*poutputfp, "* ===== \n");
						fprintf(*poutputfp,
								"* ==>>> Ignoring 'command failed' PCF error messsage\n");
						fprintf(*poutputfp, "* ===== \n");
					}
					RetCode = WARNING;
				}
#ifdef zzMQV70 
				else if (pPCFHeader->Reason==MQRCCF_PUBSUB_INHIBITED
						|| pPCFHeader->Reason == MQRC_PUBSUB_INHIBITED) {
					/*v6.1.2 .. ignore PUBSUB INHIBITED because customer must have disabled PSMODE */
					/* and therefore not want subscription objects or other such. */
					skipNext = 1;
					RetCode = MQCC_OK;
					fprintf(stderr,
					"Some objects not backed up - PSMODE on Queue Manager set to disabled or PubSub is otherwise inihibited.\n");
					if (*poutputfp)
						fprintf(
								*poutputfp,
								"* === > Some objects not backed up - PSMODE on Queue Manager set to disabled or PubSub is otherwise inihibited.\n");
				}
#endif
#ifdef zzMQV60
				else /*v6.1.1 .. report but ignore MQRCCF_NONE_FOUND */
				if (pPCFHeader->Reason==MQRCCF_NONE_FOUND || pPCFHeader->Reason
						== MQRC_UNKNOWN_OBJECT_NAME) {
					ReportNoneFound(pPCFHeader);
					skipNext = 1;
					RetCode = MQCC_OK;
				}
#endif
				else if (pPCFHeader->Reason==MQRCCF_COMMAND_FAILED && skipNext) {
					skipNext = 0;
					RetCode = MQCC_OK;
				} else
				/* other than channel not found error */
				/* for MVS, we expect this condition the first time thru this code */
				if (pPCFHeader -> Reason != MQRCCF_CFH_VERSION_ERROR
						|| platform == MQPL_MVS) {

					fprintf(stderr, "Got bad PCF response message\n");
					noMsgOnTimeout=1;
					fprintf(stderr, "\tType = %ld\n", pPCFHeader -> Type);
					fprintf(stderr, "\tStrucLength = %ld\n",
					pPCFHeader -> StrucLength);
					fprintf(stderr, "\tVersion = %ld\n", pPCFHeader -> Version);
					fprintf(stderr, "\tCommand = %ld\n", pPCFHeader -> Command);
					fprintf(stderr, "\tMsgSeqNumber = %ld\n",
					pPCFHeader -> MsgSeqNumber);
					fprintf(stderr, "\tControl = %ld\n", pPCFHeader -> Control);
					fprintf(stderr, "\tCompCode = %ld\n", pPCFHeader -> CompCode);
					fprintf(stderr, "\tReason = %ld\n", pPCFHeader -> Reason);
					fprintf(stderr, "\tParameterCount = %ld\n",
					pPCFHeader -> ParameterCount);

					if (pPCFHeader -> ParameterCount) {
						pPCFType = (MQLONG *)(pAdminMsg + MQCFH_STRUC_LENGTH);

						switch ( *pPCFType) {
						case MQCFT_INTEGER:
							pPCFInteger = (MQCFIN *)pPCFType;
							fprintf(stderr, "\t\tParameter = %ld\n",
							pPCFInteger->Parameter);
							fprintf(stderr, "\t\tValue = %ld\n",
							pPCFInteger->Value);
							break;

						case MQCFT_STRING:
							pPCFString = (MQCFST *)pPCFType;
							fprintf(stderr, "\t\tParameter = %ld\n",
							pPCFString->Parameter);
							fprintf(stderr, "\t\tCodedCharSetId = %ld\n",
							pPCFString->CodedCharSetId);
							fprintf(stderr, "\t\tStringLength = %ld\n",
							pPCFString->StringLength);
							if ((int)pPCFString->StringLength)
								fprintf(stderr, "\t\tString = %.*s\n",
								(int)pPCFString->StringLength,
								pPCFString->String);
							break;

#if defined (zzzOS)||defined(zzMQV70)
						case MQCFT_BYTE_STRING:
							pPCFByte = (MQCFBS *)pPCFType;
							fprintf(stderr, "\t\tParameter = %ld\n",
							pPCFByte->Parameter);
							fprintf(stderr, "\t\tStringLength = %ld\n",
							pPCFByte->StringLength);
							if ((int)pPCFByte->StringLength)
								fprintf(stderr, "\t\tString = %.*s\n\n\n",
								(int)pPCFByte->StringLength,
								pPCFByte->String);
							break;
#endif

						default:
							fprintf(stderr, "\tParameter Type= %ld\n",
							*pPCFType);
						} /* end of switch */
					} /* end of if stmt */

					/* v531a attempt to print damaged object*/

					if (pPCFHeader->Reason==MQRC_OBJECT_DAMAGED
							&& pPCFHeader -> ParameterCount) {
						pPCFType = (MQLONG *)(pAdminMsg + MQCFH_STRUC_LENGTH);
						if (*pPCFType == MQCFT_STRING) {
							pPCFString = (MQCFST *)pPCFType;
							memset(ObjectName, '\0', 49);
							memcpy(ObjectName, pPCFString->String,
									pPCFString->StringLength);
							fprintf(stderr, "Damaged Object name = %s\n",
							ObjectName);
						}
					}

					fprintf(stderr,
					"Processing Aborted, output file may be incomplete\n");
					if (*poutputfp)
						fprintf( *poutputfp,
								"* === > Processing Aborted, output file may be incomplete\n");
					RetCode = ERROR;
				} /* end of else other than channel not found error */

				/**** Okay, we have a good PCF response message, lets process it */

			} else {
				if (diagnostics> 10) {
					/*					if (pPCFHeader->Command == MQCMD_INQUIRE_SUBSCRIPTION) { */
					fprintf(stderr,
					"(saveqmgr.c) Reporting PCF Message Structure for 'good' message: \n");
					if (pPCFHeader -> ParameterCount) {
						pPCFType = (MQLONG *)(pAdminMsg + MQCFH_STRUC_LENGTH);
						Index = 1;
						while (RetCode != ERROR && Index
								<= pPCFHeader->ParameterCount) {

							switch ( *pPCFType) {
							case MQCFT_INTEGER:
								fprintf(stderr, "\t****Integer\n");
								pPCFInteger = (MQCFIN *)pPCFType;
								fprintf(stderr, "\t\tParameter = %ld\n",
								pPCFInteger->Parameter);
								fprintf(stderr, "\t\tValue = %ld\n",
								pPCFInteger->Value);
								fprintf(stderr, "\t\tLength = %ld\n",
								pPCFInteger->StrucLength);
								pPCFType = (MQLONG *)( (MQBYTE *)pPCFType
										+ pPCFInteger->StrucLength );
								break;

							case MQCFT_STRING:
								fprintf(stderr, "\t****Char String\n");
								pPCFString = (MQCFST *)pPCFType;
								fprintf(stderr, "\t\tParameter = %ld\n",
								pPCFString->Parameter);
								fprintf(stderr,
								"\t\tCodedCharSetId = %ld\n",
								pPCFString->CodedCharSetId);
								fprintf(stderr, "\t\tStringLength = %ld\n",
								pPCFString->StringLength);
								if ((int)pPCFString->StringLength) {
									int q;
									fprintf(stderr,
									"\t\tString = '%.*s' '",
									(int)pPCFString->StringLength,
									pPCFString->String);
									for (q=0; q<pPCFString->StringLength; q++)
										fprintf(
										stderr,
										"%.02X",
										(unsigned char)(pPCFString->String)[q]);
									fprintf(stderr, "'\n");
								}
								pPCFType = (MQLONG *)( (MQBYTE *)pPCFType
										+ pPCFString->StrucLength );
								break;

#if defined (zzzOS)||defined(zzMQV70)
							case MQCFT_BYTE_STRING:
								pPCFByte = (MQCFBS *)pPCFType;
								fprintf(stderr, "\t****Byte String\n");
								fprintf(stderr, "\t\tParameter = %ld\n",
								pPCFByte->Parameter);
								fprintf(stderr, "\t\tStringLength = %d\n",
								(int)pPCFByte->StringLength);
								if ((int)pPCFByte->StringLength) {
									int q;
									fprintf(stderr,
									"\t\tString = '%.*s' '",
									(int)pPCFByte->StringLength,
									pPCFByte->String);
									for (q=0; q<pPCFByte->StringLength; q++)
										fprintf(
										stderr,
										"%.02X",
										(unsigned char)(pPCFByte->String)[q]);
									fprintf(stderr, "'\n");
								}
								fprintf(stderr, "\t\tLength = %ld\n",
								pPCFByte->StrucLength);
								pPCFType = (MQLONG *)( (MQBYTE *)pPCFType
										+ pPCFByte->StrucLength );
								break;
#endif								
							case MQCFT_STRING_LIST:
								pPCFStringList = (MQCFSL *)pPCFType;
								fprintf(stderr, "\t****StringList\n");
								fprintf(stderr, "\t\tParameter = %ld\n",
								pPCFStringList->Parameter);
								fprintf(stderr, "\t\tStringLength = %ld\n",
								pPCFStringList->StringLength);
								fprintf(stderr, "\t\tCount = %ld\n",
								pPCFStringList->Count);
								if ((int)pPCFStringList->Count)
									fprintf(stderr,
									"\t\tStrings = '%.*s'\n",
									(int)pPCFStringList->StringLength,
									pPCFStringList->Strings);

								pPCFType = (MQLONG *)( (MQBYTE *)pPCFType
										+ pPCFStringList->StrucLength );
								break;

							default:
								fprintf(stderr, "\tParameter Type= %ld\n",
								*pPCFType);
								fprintf(
								stderr,
								"\tLength = %ld\n",
								((MQCFIN *)pPCFType)->StrucLength);
								/*									pPCFType = (MQLONG *)( (MQBYTE *)pPCFType */
								/*											+ ((MQCFBS *)pPCFType)->StrucLength ); */
								break;
							} /* end of switch */
							Index++;
						}
						/*						}  end of if stmt */
					}
				}
				/* Point to first parameter */
				pPCFType = (MQLONG *)(pAdminMsg + MQCFH_STRUC_LENGTH);

				Index = 1;
				RetCode = MQCC_OK;
				while (RetCode != ERROR && Index <= pPCFHeader->ParameterCount) {
					/* Establish the type of each parameter and allocate  */
					/* a pointer of the correct type to reference it.     */

					switch ( *pPCFType) {

					/*************************** Integer ********************/
					case MQCFT_INTEGER:
						pPCFInteger = (MQCFIN *)pPCFType;
						if (diagnostics > 10)
							fprintf(stderr,
							"(saveqmgr.c) About to set an integer value\n");

						switch (pPCFHeader->Command) {

						case MQCMD_INQUIRE_Q_MGR:
							ProcessQMgrIntegerParm(pPCFInteger, &QMgrDefn,
									&RetCode);
							/* [wms] at this time, we have the cmd level of the qmgr */
							*pQmgrVersion = QMgrDefn.CommandLevel;
							if (diagnostics>10)
								fprintf(stderr,
								"(saveqmgr.c) Detected Queue Manager Version = %d\n",*pQmgrVersion);
							break;

						case MQCMD_INQUIRE_Q:
							ProcessQueueIntegerParm(pPCFInteger, &QDefn,
									&RetCode);
							break;

						case MQCMD_INQUIRE_CHANNEL:
							ProcessChannelIntegerParm(pPCFInteger, &ChlDefn,
									&RetCode);
							break;

						case MQCMD_INQUIRE_PROCESS:
							ProcessProcIntegerParm(pPCFInteger, &ProcDefn,
									&RetCode);
							break;

							/* [v51] namelist support */
#if defined (MQCMD_INQUIRE_NAMELIST)
						case MQCMD_INQUIRE_NAMELIST:
							ProcessNamelistIntegerParm(pPCFInteger, pNlDefn,
									&RetCode);
							break;
#endif

							/* [v53] authinfo support */
#if defined (MQCMD_INQUIRE_AUTH_INFO)
						case MQCMD_INQUIRE_AUTH_INFO:
							ProcessAuthInfoIntegerParm(pPCFInteger,
									&AuthInfoDefn, &RetCode);
							break;
#endif

							/* [v6] listener and services object  support */
#if defined (MQCMD_INQUIRE_LISTENER)
						case MQCMD_INQUIRE_LISTENER:
							ProcessListenerIntegerParm(pPCFInteger,
									&ListenerDefn, &RetCode);
							break;
#endif

#if defined (MQCMD_INQUIRE_SERVICE)
						case MQCMD_INQUIRE_SERVICE:
							ProcessServiceIntegerParm(pPCFInteger,
									&ServiceDefn, &RetCode);
							break;
#endif
#if defined (MQCMD_INQUIRE_AUTH_RECS)
						case MQCMD_INQUIRE_AUTH_RECS:
							ProcessOAMIntegerParm(pPCFInteger, &OAMDefn,
									&RetCode);
							break;
#endif

#if defined(zzzOS) 

						case MQCMD_INQUIRE_CF_STRUC:
							ProcessCFStructIntegerParm(pPCFInteger,
									&CFStructDefn, &RetCode);
							break;
						case MQCMD_INQUIRE_LOG:
							ProcessLogIntegerParm(pPCFInteger, &LogDefn,
									&RetCode);
							break;
						case MQCMD_INQUIRE_STG_CLASS:
							ProcessStgClassIntegerParm(pPCFInteger,
									&StgClassDefn, &RetCode);
							break;
						case MQCMD_INQUIRE_USAGE:
							ProcessUsageIntegerParm(pPCFInteger, &UsageDefn,
									&RetCode);
							break;
						case MQCMD_INQUIRE_SYSTEM:
							ProcessSystemIntegerParm(pPCFInteger, &SystemDefn,
									&RetCode);
							break;
						case MQCMD_INQUIRE_ARCHIVE:
							ProcessArchiveIntegerParm(pPCFInteger,
									&ArchiveDefn, &RetCode);
							break;

#endif
#ifdef zzMQV70
						case MQCMD_INQUIRE_TOPIC:
							ProcessTopicIntegerParm(pPCFInteger, &TopicDefn,
									&RetCode);
							break;
						case MQCMD_INQUIRE_SUBSCRIPTION:
							ProcessSubscrIntegerParm(pPCFInteger,
									pSubscriptDefn, &RetCode);
							break;
#endif						
						default:
							if (noisy)
								fprintf(
								stderr,
								"(saveqmgr.c) Unknown CFT_INTEGER Command Received %ld\n",
								pPCFHeader->Command);
							RetCode = WARNING;

						} /* endswitch */

						/* Increment the pointer to the next parameter by the */
						/* length of the current parm.                        */
						pPCFType = (MQLONG *)( (MQBYTE *)pPCFType
								+ pPCFInteger->StrucLength );
						break;

						/*************************** String ********************/
					case MQCFT_STRING:
#if defined (zzzOS)
					case MQCFT_BYTE_STRING:
#endif
						pPCFString = (MQCFST *)pPCFType;
						if (diagnostics > 10) {
							if (*pPCFType == MQCFT_BYTE_STRING) {
								fprintf(stderr,
								"(saveqmgr.c) About to set a byte value\n");
							} else {
								fprintf(stderr,
								"(saveqmgr.c) About to set a string value\n");
							}
						}

						switch (pPCFHeader->Command) {

						case MQCMD_INQUIRE_Q_MGR:
							ProcessQMgrStringParm(pPCFString, &QMgrDefn,
									&RetCode);
							break;

						case MQCMD_INQUIRE_Q:
							ProcessQueueStringParm(pPCFString, &QDefn, &RetCode);
							break;

						case MQCMD_INQUIRE_CHANNEL:
							ProcessChannelStringParm(pPCFString, &ChlDefn,
									&RetCode);
							break;

						case MQCMD_INQUIRE_PROCESS:
							ProcessProcStringParm(pPCFString, &ProcDefn,
									&RetCode);
							break;

							/* [v51] namelist support */
#if defined (MQCMD_INQUIRE_NAMELIST)
						case MQCMD_INQUIRE_NAMELIST:
							ProcessNamelistStringParm(pPCFString, pNlDefn,
									&RetCode);
							break;
#endif

							/* [v53] authinfo support */
#if defined (MQCMD_INQUIRE_AUTH_INFO)
						case MQCMD_INQUIRE_AUTH_INFO:
							ProcessAuthInfoStringParm(pPCFString,
									&AuthInfoDefn, &RetCode);
							break;
#endif

							/* [v6] listener and services object  support */
#if defined (MQCMD_INQUIRE_LISTENER)
						case MQCMD_INQUIRE_LISTENER:
							ProcessListenerStringParm(pPCFString,
									&ListenerDefn, &RetCode);
							break;
#endif

#if defined (MQCMD_INQUIRE_SERVICE)
						case MQCMD_INQUIRE_SERVICE:
							ProcessServiceStringParm(pPCFString, &ServiceDefn,
									&RetCode);
							break;
#endif
#if defined (MQCMD_INQUIRE_AUTH_RECS)
						case MQCMD_INQUIRE_AUTH_RECS:
							ProcessOAMStringParm(pPCFString, &OAMDefn, &RetCode);
							break;
#endif

#if defined(zzzOS) 

						case MQCMD_INQUIRE_CF_STRUC:
							ProcessCFStructStringParm(pPCFString,
									&CFStructDefn, &RetCode);
							break;
						case MQCMD_INQUIRE_LOG:
							ProcessLogStringParm(pPCFString, &LogDefn, &RetCode);
							break;
						case MQCMD_INQUIRE_STG_CLASS:
							ProcessStgClassStringParm(pPCFString,
									&StgClassDefn, &RetCode);
							break;
						case MQCMD_INQUIRE_USAGE:
							ProcessUsageStringParm(pPCFString, &UsageDefn,
									&RetCode);
							break;
						case MQCMD_INQUIRE_SYSTEM:
							ProcessSystemStringParm(pPCFString, &SystemDefn,
									&RetCode);
							break;
						case MQCMD_INQUIRE_ARCHIVE:
							ProcessArchiveStringParm(pPCFString, &ArchiveDefn,
									&RetCode);
							break;

#endif
#ifdef zzMQV70
						case MQCMD_INQUIRE_TOPIC:
							ProcessTopicStringParm(pPCFString, &TopicDefn,
									&RetCode);
							break;
						case MQCMD_INQUIRE_SUBSCRIPTION:
							if (*pPCFType == MQCFT_BYTE_STRING) {
								pPCFByte = (MQCFBS *)pPCFType;
								ProcessSubscrByteParm(pPCFByte, pSubscriptDefn,
										&RetCode);
							} else {
								ProcessSubscrStringParm(pPCFString,
										pSubscriptDefn, &RetCode);
							}
							break;
#endif						

						default:
							if (noisy) {
								if (*pPCFType == MQCFT_STRING) {
									fprintf(
									stderr,
									"(saveqmgr.c) Unknown CFT_STRING Command Received %ld\n",
									pPCFHeader->Command);
								}
#ifdef zzMQV70
								else {
									fprintf(
									stderr,
									"(saveqmgr.c) Unknown CFT_BYTE Command Received %ld\n",
									pPCFHeader->Command);
								}
#endif
							}
							RetCode = WARNING;

						} /* endswitch */

						/* Increment the pointer to the next parameter by the */
						/* length of the current parm.                        */
						if (*pPCFType == MQCFT_STRING) {
							if (diagnostics > 10)
								fprintf(
								stderr,
								"(saveqmgr.c) incrementing pPCFType for String value with strucLength %ld\n" ,
								pPCFString->StrucLength);
							fflush(stderr);
							pPCFType = (MQLONG *)( (MQBYTE *)pPCFType
									+ pPCFString->StrucLength );
						} else {
#ifdef zzMQV70
							pPCFByte = (MQCFBS *)pPCFType;
							if (diagnostics > 10)
								fprintf(
								stderr,
								"(saveqmgr.c) incrementing pPCFType for Byte value with strucLength %ld\n",
								pPCFByte->StrucLength);
							fflush(stderr);
							pPCFType = (MQLONG *)( (MQBYTE *)pPCFType
									+ pPCFByte->StrucLength );
#endif
						}

						break;
						/*************************** Integer List ********************/
					case MQCFT_INTEGER_LIST:
						if (diagnostics > 10)
							fprintf(stderr,
							"(saveqmgr.c) About to set an integer list value\n");

						pPCFIntegerList = (MQCFIL *)pPCFType;

						switch (pPCFHeader->Command) {

						case MQCMD_INQUIRE_CHANNEL:
							ProcessChannelIntegerListParm(pPCFIntegerList,
									&ChlDefn, &RetCode);
							break;
#if defined(zzzOS)
						case MQCMD_INQUIRE_ARCHIVE:
							ProcessArchiveIntegerListParm(pPCFIntegerList,
									&ArchiveDefn, &RetCode);
							break;

						case MQCMD_INQUIRE_SYSTEM:
							ProcessSystemIntegerListParm(pPCFIntegerList,
									&SystemDefn, &RetCode);
							break;
						case MQCMD_INQUIRE_AUTH_RECS:
							ProcessOAMIntegerListParm(pPCFIntegerList,
									&OAMDefn, &RetCode);
							break;
#endif

						default:
							if (noisy)
								fprintf(
								stderr,
								"(saveqmgr.c) Unknown CFT_INTEGER_LIST Command Received %ld\n",
								pPCFHeader->Command);
							RetCode = WARNING;

							/* Increment the pointer to the next parameter by the */
							/* length of the current parm.                        */
						}

						pPCFType = (MQLONG *)( (MQBYTE *)pPCFType
								+ pPCFIntegerList->StrucLength );

						break;
						/*************************** String List ********************/
					case MQCFT_STRING_LIST:
						if (diagnostics > 10)
							fprintf(stderr,
							"(saveqmgr.c) About to set a string list value\n");

						pPCFStringList = (MQCFSL *)pPCFType;

						switch (pPCFHeader->Command) {

						/* [v51] namelist support */
#if defined (MQCMD_INQUIRE_NAMELIST)
						case MQCMD_INQUIRE_NAMELIST:
							ProcessNamelistStringListParm(pPCFStringList,
									pNlDefn, &RetCode);
							break;
#endif

							/* [v512] include string list for channel exits */
						case MQCMD_INQUIRE_CHANNEL:
							ProcessChannelStringListParm(pPCFStringList,
									&ChlDefn, &RetCode);
							break;

						case MQCMD_INQUIRE_Q:
							ProcessQueueStringListParm(pPCFStringList, &QDefn,
									&RetCode, pMyParms);
							break;

						default:
							if (noisy)
								fprintf(
								stderr,
								"(saveqmgr.c) Unknown CFT_STRING_LIST Command Received %ld\n",
								pPCFHeader->Command);
							RetCode = WARNING;
							/* Increment the pointer to the next parameter by the */
							/* length of the current parm.                        */
						}

						pPCFType = (MQLONG *)( (MQBYTE *)pPCFType
								+ pPCFStringList->StrucLength );

						break;

						/*************************** Unknown ********************/
					default:
						/* Parameter block of unrecognised type                 */
						/* Something has gone wrong!                            */
						/* Reply message may be corrupt or may be of unexpected */
						/*  format. Either way, continuing may give disasterous */
						/*  results.                                            */
						/* So, terminate program.                               */
						fprintf(stderr,
						"(saveqmgr.c) Error: Received parameter block is of an unrecognised type\n");
						fprintf(stderr, "Type = %ld.\n", *pPCFType);
						fprintf(stderr, "Command = %ld.\n", pPCFHeader->Command);
						RetCode = ERROR;
						break;

					} /* endswitch */

					Index++;

				} /* endwhile */

				/* ********************************************************* */
				/* Message parsed, append to output file                     */
				/* ********************************************************* */

				/* V3.2 if filename is not set, set it to the qmgr name + .mqs    */

				if (!fileName[0]) {
					memset(tempfileName, '\0', FILENAMESIZE+1);
					memcpy(tempfileName, QMgrDefn.QMgrName, 
					MQ_Q_MGR_NAME_LENGTH);
					/* replace blanks with nulls and mangle periods and slashes */
					for (i=0; i < MQ_Q_MGR_NAME_LENGTH; i++) {
						if (tempfileName[i] == ' ')
							tempfileName[i] = '\0';
						if (tempfileName[i] == '.')
							tempfileName[i] = '!';
						if (tempfileName[i] == '/')
							tempfileName[i] = '&';
					} /* end for ... */

					/* Now build the real filename */
					strcpy(fileName, DEFAULTOUTPUTPREFIX);
					strcat(fileName, tempfileName);
					strcat(fileName, DEFAULTOUTPUTPOSTFIX);

				}/* End if !fileName ... */

				/* we'll construct a filename for the setmqaut commands here */
				/* knowing that we might not use it.  It only gets opened if */
				/* we get auth responses and need to write a file            */

				if (!authFileName[0]) {
					memset(tempfileName, '\0', FILENAMESIZE+1);
					memcpy(tempfileName, QMgrDefn.QMgrName, 
					MQ_Q_MGR_NAME_LENGTH);
					/* replace blanks with nulls and mangle periods and slashes */
					for (i=0; i < MQ_Q_MGR_NAME_LENGTH; i++) {
						if (tempfileName[i] == ' ')
							tempfileName[i] = '\0';
						if (tempfileName[i] == '.')
							tempfileName[i] = '!';
						if (tempfileName[i] == '/')
							tempfileName[i] = '&';
					} /* end for ... */

					/* Now build the real filename */
					strcpy(authFileName, DEFAULTOUTPUTPREFIXAUT);
					strcat(authFileName, tempfileName);
					strcat(authFileName, DEFAULTOUTPUTPOSTFIXAUT);

				}/* End if !fileName ... */

				/* v603 ... the setmqaut file must be a different file than the mqsc file */
				if (!strcmp(fileName, authFileName)) {
					fprintf(
					stderr,
					"(saveqmgr.c) OAM filename (%s) must not be the same as ",
					authFileName);
					fprintf(stderr, "MQSC filename (%s), setting to \"%s\"\n",
					fileName, DEFAULTOUTPUTFILEAUT);
					strcpy(authFileName, DEFAULTOUTPUTFILEAUT);
				}

				/* open file here, once... (if not stdout and not already open) v3.3 */
				if (!*poutputfp) {
					if ((*poutputfp = fopen(fileName, overlay ? OPENMODER
							: OPENMODEA)) == NULL) {
						char buf[1024];
						sprintf(buf, "error opening %s", fileName);
						perror(buf);
						RetCode = ERROR;
						break;
					}
				}

				/* [v51]we are using stdout, so fixup filename */
				if (*poutputfp==stdout)
					strcpy(fileName, "<stdout>");

				/* based on wether or not the user requested, we'll    */
				/* send the correct qmgr level to the subroutines      */
				if (!(*pUserVersion))
					*pUserVersion = *pQmgrVersion;
				if (diagnostics > 10)
					fprintf(stderr,"User Version is = %d Qmgr Version is = %d\n",*pUserVersion,*pQmgrVersion);

				/* v32 fileName is the name of the output file  */
				switch (pPCFHeader->Command) {

				/* [v51] namelist support */

#if defined (MQCMD_INQUIRE_NAMELIST)
				case MQCMD_INQUIRE_NAMELIST:
					/* Only write if name is NOT "SYSTEM." or user wants them */
					if (printThisObject(pNlDefn->NamelistName, SysObjs, 
					SO_NAMELIST)) {
						/*					if ((memcmp(pNlDefn->NamelistName, "SYSTEM.", 7)!=0)
						 || ((!(SysObjs & SO_NONE)) && (SysObjs
						 & SO_NAMELIST) )) {
						 */
						if (firstNamelist) {
							if (diagnostics > 2)
								fprintf(stderr,"SO_NAMELIST is = %d SysObjs Flag is = %d SO_NAMELIST AND SysObjs is %d\n",SO_NAMELIST, SysObjs, (SysObjs&SO_NAMELIST));
							firstNamelist = 0;
							fprintf(stderr,
							"Writing Namelist definitions to %.*s.\n",
							FILENAMESIZE, fileName);
						}/* End if*/
						AddToFileNAMELIST(pNlDefn, *pUserVersion, fileName,
								*poutputfp, 0, &RetCode);
						if (twoLines) {
							AddToFileNAMELIST(pNlDefn, *pUserVersion, fileName,
									*poutputfp, 1, &RetCode);
						}

					} else {
						if (firstNamelist> 1) {
							fputs("** (Skipping SYSTEM Namelists) **\n",
									*poutputfp);
							firstNamelist--;
						}
					}

					break;
#endif

				case MQCMD_INQUIRE_Q:
					/* Only write if name is NOT "SYSTEM." or user wants them */

					if (firstQueue>1) {
						firstQueue = 0;
						fprintf(stderr,
						"Writing Queue definitions to %.*s.\n",
						FILENAMESIZE, fileName);

						if ( (!(SysObjs & SO_ALL) || (SysObjs & SO_QUEUE) ))
							fputs("** (Skipping SYSTEM Queues) **\n",
									*poutputfp);
					}/* End if*/
					if (printThisObject(QDefn.QName, SysObjs, SO_QUEUE)) {
						/*					if (memcmp(QDefn.QName, "SYSTEM.", 7) || ((!(SysObjs
						 & SO_NONE)) && (SysObjs & SO_QUEUE))) {*/
						AddToFileQUEUE( &QDefn, *pUserVersion, fileName,
								*poutputfp, 0, &RetCode);
						if (twoLines) {
							AddToFileQUEUE( &QDefn, *pUserVersion, fileName,
									*poutputfp, 1, &RetCode);
						}
					}
					break;

				case MQCMD_INQUIRE_CHANNEL:
					/* Only write if name is NOT "SYSTEM." or user wants them */
					if (printThisObject(ChlDefn.ChannelName, SysObjs, 
					SO_CHANNEL)) {
						/*					if (memcmp(ChlDefn.ChannelName, "SYSTEM.", 7)
						 || ((!(SysObjs & SO_NONE))
						 && (SysObjs & SO_CHANNEL))) {*/
						if (firstChannel) {
							firstChannel = 0;
							fprintf(stderr,
							"Writing Channel definitions to %.*s.\n",
							FILENAMESIZE, fileName);
						}/* End if*/
						AddToFileCHANNEL(&ChlDefn, *pUserVersion, fileName,
								*poutputfp, 0, &RetCode);

						if (twoLines) {
							AddToFileCHANNEL(&ChlDefn, *pUserVersion,
									fileName, *poutputfp, 1, &RetCode);

						}
					} else {
						if (firstChannel> 1) {
							fputs("** (Skipping SYSTEM channels) **\n",
									*poutputfp);
							firstChannel--;
						}
					}
					break;

				case MQCMD_INQUIRE_PROCESS:
					/* Only write if name is NOT "SYSTEM." or user wants them */
					if (printThisObject(ProcDefn.ProcessName, SysObjs, 
					SO_PROCESS)) {
						/*					if (memcmp(ProcDefn.ProcessName, "SYSTEM.", 7)
						 || ((!(SysObjs & SO_NONE))
						 && (SysObjs & SO_PROCESS) )) { */
						if (firstProcess) {
							firstProcess = 0;
							fprintf(stderr,
							"Writing Process definitions to %.*s.\n",
							FILENAMESIZE, fileName);
						}/* End if*/
						AddToFilePROCESS( &ProcDefn, *pUserVersion, fileName,
								*poutputfp, 0, &RetCode);
						if (twoLines) {
							AddToFilePROCESS( &ProcDefn, *pUserVersion,
									fileName, *poutputfp, 1, &RetCode);
						}
					} else {
						if (firstProcess> 1) {
							fputs("** (Skipping SYSTEM Process objects) **\n",
									*poutputfp);
							firstProcess --;
						}
					}
					break;

				case MQCMD_INQUIRE_Q_MGR:
					if (firstQMgr) {
						firstQMgr = 0;
						fprintf(stderr,
						"Writing Queue Manager definition to %.*s.\n",
						FILENAMESIZE, fileName);
					}/* End if*/
					if (twoLines) {
						AddToFileQMGR( &QMgrDefn, *pUserVersion, fileName,
								*poutputfp, 1, &RetCode);
					}
					AddToFileQMGR( &QMgrDefn, *pUserVersion, fileName,
							*poutputfp, 0, &RetCode);
					break;

					/* [v53] authinfo support */

#if defined (MQCMD_INQUIRE_AUTH_INFO)
				case MQCMD_INQUIRE_AUTH_INFO:
					/* Only write if name is NOT "SYSTEM." or user wants them */
					if (printThisObject(AuthInfoDefn.AuthInfoName, SysObjs, 
					SO_AUTHINFO)) {
						/*					if (memcmp(AuthInfoDefn.AuthInfoName, "SYSTEM.", 7)
						 || ((!(SysObjs & SO_NONE)) && (SysObjs
						 & SO_AUTHINFO) )) { */
						if (firstAuthInfo) {
							firstAuthInfo = 0;
							fprintf(stderr,
							"Writing AuthInfo definitions to %.*s.\n",
							FILENAMESIZE, fileName);
						}/* End if*/
						AddToFileAUTHINFO( &AuthInfoDefn, *pUserVersion,
								fileName, *poutputfp, 0, &RetCode);
						if (twoLines) {
							AddToFileAUTHINFO( &AuthInfoDefn, *pUserVersion,
									fileName, *poutputfp, 1, &RetCode);

						}
					} else {
						if (firstAuthInfo> 1) {
							fputs("** (Skipping SYSTEM AuthInfo objects) **\n",
									*poutputfp);
							firstAuthInfo --;
						}
					}/* End if*/
					break;
#endif

#if defined (zzMQV60)
				case MQCMD_INQUIRE_LISTENER:
					/* Only write if name is NOT "SYSTEM." or user wants them */
					if (printThisObject(ListenerDefn.ListenerName, SysObjs, 
					SO_LISTENER)) {
						/*					if (memcmp(ListenerDefn.ListenerName, "SYSTEM.", 7)
						 || ((!(SysObjs & SO_NONE)) && (SysObjs
						 & SO_LISTENER) )) { */
						if (firstListener) {
							firstListener = 0;
							fprintf(stderr,
							"Writing Listener definitions to %.*s.\n",
							FILENAMESIZE, fileName);
						}/* End if*/
						AddToFileLISTENER( &ListenerDefn, *pUserVersion,
								fileName, *poutputfp, 0, &RetCode);
						if (twoLines) {
							AddToFileLISTENER( &ListenerDefn, *pUserVersion,
									fileName, *poutputfp, 1, &RetCode);

						}
					} else {
						if (firstListener> 1) {
							fputs("** (Skipping SYSTEM Listener objects) **\n",
									*poutputfp);
							firstListener --;
						}
					}/* End if*/
					break;
				case MQCMD_INQUIRE_SERVICE:
					/* Only write if name is NOT "SYSTEM." or user wants them */
					if (printThisObject(ServiceDefn.ServiceName, SysObjs, 
					SO_SERVICE)) {
						/*					if (memcmp(ServiceDefn.ServiceName, "SYSTEM.", 7)
						 || ((!(SysObjs & SO_NONE))
						 && (SysObjs & SO_SERVICE) )) { */
						if (firstService) {
							firstService = 0;
							fprintf(stderr,
							"Writing Service definitions to %.*s.\n",
							FILENAMESIZE, fileName);
						}/* End if*/
						AddToFileSERVICE( &ServiceDefn, *pUserVersion,
								fileName, *poutputfp, 0, &RetCode);
						if (twoLines) {
							AddToFileSERVICE( &ServiceDefn, *pUserVersion,
									fileName, *poutputfp, 1, &RetCode);

						}
					} else {
						if (firstService> 1) {
							fputs("** (Skipping SYSTEM Service objects) **\n",
									*poutputfp);
							firstService--;
						}
					}/* End if*/
					break;
				case MQCMD_INQUIRE_AUTH_RECS:

					/* if the setmqaut file isn't open, now is the time to do it ... */
					if (!*pauthOutputfp) {
						if ((*pauthOutputfp = fopen(authFileName,
								overlay ? OPENMODER : OPENMODEA)) == NULL) {
							char buf[1024];
							sprintf(buf, "error opening %s", authFileName);
							perror(buf);
							RetCode = ERROR;
							break;
						}
					}

					if (firstOAM) {
						firstOAM = 0;
						fprintf(stderr, "Writing OAM definitions to %.*s.\n",
						FILENAMESIZE, authFileName);
					}/* End if*/
					AddToFileOAM( &OAMDefn, *pUserVersion, authFileName,
							*pauthOutputfp, 0, &RetCode);
					break;

#endif
#ifdef zzMQV70
				case MQCMD_INQUIRE_TOPIC:

					/* Only write if name is NOT "SYSTEM." or user wants them */
					if (printThisObject(TopicDefn.TopicName, SysObjs, SO_TOPIC)) {
						/*					if (memcmp(TopicDefn.TopicName, "SYSTEM.", 7)
						 || ((!(SysObjs & SO_NONE)) && (SysObjs & SO_TOPIC))) { */
						if (firstTopic) {
							firstTopic = 0;
							fprintf(stderr,
							"Writing Topic definitions to %.*s.\n",
							FILENAMESIZE, fileName);
						}/* End if*/
						AddToFileTopic( &TopicDefn, *pUserVersion, fileName,
								*poutputfp, 0, &RetCode);
						if (twoLines) {
							AddToFileTopic( &TopicDefn, *pUserVersion,
									fileName, *poutputfp, 1, &RetCode);
						}
					} else {
						if (firstTopic> 1) {
							fputs("** (Skipping SYSTEM Topic objects) **\n",
									*poutputfp);
							firstTopic--;
						}
					}/* End if*/
					fflush(*poutputfp);
					break;
				case MQCMD_INQUIRE_SUBSCRIPTION:
					fflush(*poutputfp);
					/* Only write if name is NOT "SYSTEM." or user wants them */
					if (printThisObject(pSubscriptDefn->SubscrName, SysObjs, 
					SO_SUB)) {
						/*					if (memcmp(SubscriptDefn.SubscrName, "SYSTEM.", 7)
						 || ((!(SysObjs & SO_NONE)) && (SysObjs & SO_SUB))) { */
						if (firstSubscript) {
							firstSubscript = 0;
							fprintf(
							stderr,
							"Writing Subscription definitions to %.*s.\n",
							FILENAMESIZE, fileName);
						}/* End if*/
						AddToFileSubscr(pSubscriptDefn, *pUserVersion,
								fileName, *poutputfp, 0, &RetCode);
						if (twoLines) {
							AddToFileSubscr(pSubscriptDefn, *pUserVersion,
									fileName, *poutputfp, 1, &RetCode);
						}
					} else {
						if (firstSubscript>1) {
							fputs(
									"** (Skipping SYSTEM Subscription objects) **\n",
									*poutputfp);
							firstSubscript--;
						}
					}/* End if*/
					break;

#endif

#if defined(zzzOS)
				case MQCMD_INQUIRE_CF_STRUC:
					if (firstCFStruct) {
						firstCFStruct = 0;
						fprintf(stderr,
						"Writing CF Structure definitions to %.*s.\n",
						FILENAMESIZE, fileName);
					}/* End if*/
					AddToFileCFStruct( &CFStructDefn, *pUserVersion, fileName,
							*poutputfp, 0, &RetCode);
					break;
				case MQCMD_INQUIRE_LOG:
					if (firstLog) {
						firstLog = 0;
						fprintf(stderr, "Writing Log definitions to %.*s.\n",
						FILENAMESIZE, fileName);
					}/* End if*/
					AddToFileLog( &LogDefn, *pUserVersion, fileName,
							*poutputfp, 0, &RetCode);
					break;
				case MQCMD_INQUIRE_STG_CLASS:
					if (firstStgClass) {
						firstStgClass = 0;
						fprintf(stderr,
						"Writing Storage Class definitions to %.*s.\n",
						FILENAMESIZE, fileName);
					}/* End if*/
					AddToFileStgClass( &StgClassDefn, *pUserVersion, fileName,
							*poutputfp, 0, &RetCode);
					break;
				case MQCMD_INQUIRE_USAGE:
					if (firstUsage) {
						firstUsage = 0;
						fprintf(stderr, "Writing Usage definitions to %.*s.\n",
						FILENAMESIZE, fileName);
					}/* End if*/
					AddToFileUsage( &UsageDefn, *pUserVersion, fileName,
							*poutputfp, 0, &RetCode);
					break;

				case MQCMD_INQUIRE_SYSTEM:
					if (firstSystem) {
						firstSystem = 0;
						fprintf(stderr,
						"Writing System definitions to %.*s.\n",
						FILENAMESIZE, fileName);
					}/* End if*/
					AddToFileSystem( &SystemDefn, *pUserVersion, fileName,
							*poutputfp, 0, &RetCode);
					break;

				case MQCMD_INQUIRE_ARCHIVE:
					if (firstArchive) {
						firstArchive = 0;
						fprintf(stderr,
						"Writing Archive definitions to %.*s.\n",
						FILENAMESIZE, fileName);
					}/* End if*/
					AddToFileArchive( &ArchiveDefn, *pUserVersion, fileName,
							*poutputfp, 0, &RetCode);
					break;

#endif

				} /* endswitch */

				if (RetCode==ERROR) {
					fprintf(stderr, "Error writing to ouput file %s\n",
					fileName);
				}/* End if*/

				/* ********************************************************* */
				/* Finished processing the current message, do the next one. */
				/* ********************************************************* */

				/* ********************************************************* */
				/* If we hit a "last message" message, reduce the number of  */
				/* objectsets by 1 until none are left                       */
				/* ********************************************************* */

			} /* end of else pcf rc is okay */

#if defined (zzzOS)
		} /* end of check for summary pcf message  */
#endif

		/* if this is the last message in the set, then decrement */
		/* the count of the number of sets we must process */
		if (pPCFHeader->Control == MQCFC_LAST) {
			ObjectSets--;
		}

	} while (RetCode != ERROR && ObjectSets); /* enddo */

#if defined (zzMQV60)
	/* if we've been writing oam commands, then call one last */
	/* time to write any "ending" commands needed */
	if (*pauthOutputfp) {
		AddToFileOAM( 0, *pUserVersion, authFileName, *pauthOutputfp, 0,
				&RetCode);

	}
#endif

	if (pAdminMsg)
		free(pAdminMsg);

#if defined (MQCMD_INQUIRE_NAMELIST)
	if (pNlDefn)
		free(pNlDefn);
#endif

	/* v531a... if we have the ignore damaged flag on, and there was an error, */
	/* then we will return a WARNING level error */
	if (RetCode != MQCC_OK && ignoreDamaged && noMsgOnTimeout)
		RetCode = WARNING;

	return (RetCode);
}

int printThisObject(char* objectName, int SysObjs, int SO_OBJECT) {
	int returnVal;
	returnVal = ((memcmp(objectName, "SYSTEM.", 7)) || (SysObjs & SO_ALL)
			|| (SysObjs & SO_OBJECT) );
	if (diagnostics > 2) {
		fprintf(stderr,"(saveqmgr.c) printThisObject objectName is %0.40s SysObjs is %d SO_OBJECT is %d \n",objectName,SysObjs,SO_OBJECT);
		fprintf(stderr,"(saveqmgr.c) printThisObject sysNameTest is %d SO_ALL test is %d SO_OBJECT test is %d \n",memcmp(objectName, "SYSTEM.", 7),(SysObjs & SO_ALL),(SysObjs & SO_OBJECT));
		fprintf(stderr,"(saveqmgr.c) printThisObject returnVal is %d \n",returnVal);
	}
	return returnVal;
}

void ReportNoneFound(MQCFH *pPCFHeader) {
	char* objName;
	switch (pPCFHeader -> Command) {

#ifdef MQCMD_INQUIRE_Q_MGR
	case MQCMD_INQUIRE_Q_MGR:
		objName = "QMGR";
		break;
#endif
#ifdef MQCMD_INQUIRE_AUTH_INFO
	case MQCMD_INQUIRE_AUTH_INFO:
		objName = "AUTHINFO";
		break;
#endif
#ifdef MQCMD_INQUIRE_Q
	case MQCMD_INQUIRE_Q:
		objName = "QUEUE";
		break;
#endif
#ifdef MQCMD_INQUIRE_CHANNEL
	case MQCMD_INQUIRE_CHANNEL:
		objName = "CHANNEL";
		break;
#endif
#ifdef MQCMD_INQUIRE_PROCESS
	case MQCMD_INQUIRE_PROCESS:
		objName = "PROCESS";
		break;
#endif
#ifdef MQCMD_INQUIRE_NAMELIST
	case MQCMD_INQUIRE_NAMELIST:
		objName = "NAMELIST";
		break;
#endif
#ifdef MQCMD_INQUIRE_LISTENER
	case MQCMD_INQUIRE_LISTENER:
		objName = "LISTENER";
		break;
#endif
#ifdef MQCMD_INQUIRE_SERVICE
	case MQCMD_INQUIRE_SERVICE:
		objName = "SERVICE";
		break;
#endif
#ifdef MQCMD_INQUIRE_AUTH_RECS
	case MQCMD_INQUIRE_AUTH_RECS:
		objName = "AUTHRECS";
		break;
#endif
#ifdef MQCMD_INQUIRE_TOPIC
	case MQCMD_INQUIRE_TOPIC:
		objName = "TOPIC";
		break;
#endif
#ifdef MQCMD_INQUIRE_SUBSCRIPTION
	case MQCMD_INQUIRE_SUBSCRIPTION:
		objName = "SUBSCRIPTION";
		break;
#endif
#ifdef MQCMD_INQUIRE_STG_CLASS
	case MQCMD_INQUIRE_STG_CLASS:
		objName = "STGCLASS";
		break;
#endif
#ifdef MQCMD_INQUIRE_LOG
	case MQCMD_INQUIRE_LOG:
		objName = "LOG";
		break;
#endif
#ifdef MQCMD_INQUIRE_ARCHIVE
	case MQCMD_INQUIRE_ARCHIVE:
		objName = "ARCHIVE";
		break;
#endif
#ifdef MQCMD_INQUIRE_USAGE
	case MQCMD_INQUIRE_USAGE:
		objName = "USAGE";
		break;
#endif
#ifdef MQCMD_INQUIRE_SYSTEM
	case MQCMD_INQUIRE_SYSTEM:
		objName = "SYSTEM";
		break;
#endif
#ifdef MQCMD_INQUIRE_CF_STRUC
	case MQCMD_INQUIRE_CF_STRUC:
		objName = "CFSTRUCT";
		break;
#endif
	}
	fprintf(stderr, "No %s objects found\n", objName);
}
