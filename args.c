/*===========================================================================*/
/*              This supportpac is maintained by Jeff Lowrey 	             */
/* 				Wayne M. Schutz is maintainer emeritus.	  					 */
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
/* Module Name: args.c                                                       */
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

#include "args.h"
#include "saveqmgr.h"
extern int oneLine; /* v601 -- whether to write the entire DEFINE on oneline */
int ProcessArgs(int argc, char *argv[], CmdArgParmsPtr MyParms, int *ParmCount,
		int isClient) {
	int inValidParm = 0;
#if defined (ZZOS400PARMS)
	inValidParm = ProcessISeriesArgs(argc,argv, MyParms, ParmCount );
#else
	inValidParm = ProcessMainArgs(argc, argv, MyParms, ParmCount, isClient);
#endif
	return inValidParm;
}

int ProcessMainArgs(int argc, char *argv[], CmdArgParmsPtr MyParms,
		int *ParmCount, int isClient) {

	int inValidParm = 0;
	int platform = 0; /* platform to generate code for */
	char* found;
	/* QM QU NL PR AU LS SV TP SB */
	char
			sysObjParms[77] =
					"QMGR,QUEUE,NAMELIST,CHANNEL,PROCESS,AUTHINFO,LISTENER,SERVICE,TOPIC,SUB,NONE\0";

	InitArgs(MyParms);
	/* Get our executable name for the help display */
	strcpy(MyParms->myName, argv[0]);

	while (argc> *ParmCount && !inValidParm) {

		if (MyParms->diagnostics >20) {
			fprintf(stderr,
			"(args.c) current arg is %s at %d of %d and invalidParm is %d\n",
			argv[*ParmCount], *ParmCount, argc, inValidParm);
		}
		/* was a qmgr name specified ? */
		if ((memcmp(argv[*ParmCount], LOCAL_QMGR_SHORT_NAME,
				strlen(LOCAL_QMGR_SHORT_NAME) ) == 0) || (memcmp(
				argv[*ParmCount], LOCAL_QMGR_LONG_NAME,
				strlen(LOCAL_QMGR_LONG_NAME) ) == 0)) {

			/* Is there no space between the switch and the parameter value? */
			if ((strlen(argv[*ParmCount])== strlen(LOCAL_QMGR_SHORT_NAME))
					||(strlen(argv[*ParmCount])== strlen(LOCAL_QMGR_LONG_NAME))) {
				if (*ParmCount < argc-1) {
					strncpy(MyParms->LclQMgrName, argv[*ParmCount+1], 
					MQ_Q_MGR_NAME_LENGTH);
					*ParmCount += 2;
					if (MyParms->diagnostics>20) {
						fprintf(
						stderr,
						"(args.c) Processing %s flag, setting Local Queue Manager to %s\n",LOCAL_QMGR_SHORT_NAME,MyParms->LclQMgrName);
					}
				} else {
					inValidParm = 1;
				}
			} else {
				fprintf(stderr,"(args.c) no space after qmgr switch\n");
				if (memcmp(argv[*ParmCount], 
				LOCAL_QMGR_LONG_NAME, strlen(LOCAL_QMGR_LONG_NAME)) == 0) {
					strncpy(MyParms->LclQMgrName, argv[*ParmCount]
							+strlen(LOCAL_QMGR_LONG_NAME), 
					MQ_Q_MGR_NAME_LENGTH);
					*ParmCount += 1;
				} else {
					strncpy(MyParms->LclQMgrName, argv[*ParmCount]
							+strlen(LOCAL_QMGR_SHORT_NAME), 
					MQ_Q_MGR_NAME_LENGTH);
					*ParmCount += 1;
				}
				if (MyParms->diagnostics>20) {
					fprintf(
					stderr,
					"(args.c) Processing %s flag, setting Local Queue Manager to %s\n",LOCAL_QMGR_SHORT_NAME,MyParms->LclQMgrName);
				}
			}

			/* was a remote qmgr name specified ? */
		} else if ((memcmp(argv[*ParmCount], REMOTE_QMGR_SHORT_NAME,
				strlen(REMOTE_QMGR_SHORT_NAME) ) == 0) || (memcmp(
				argv[*ParmCount], REMOTE_QMGR_LONG_NAME,
				strlen(REMOTE_QMGR_LONG_NAME) ) == 0)) {

			/* test to see if there is a blank after the flag */
			if (strlen(argv[*ParmCount]) == strlen(REMOTE_QMGR_SHORT_NAME)
					|| strlen(argv[*ParmCount])
							== strlen(REMOTE_QMGR_LONG_NAME)) {
				if (*ParmCount < argc-1) {
					strncpy(MyParms->RmtQMgrName, argv[*ParmCount+1], 
					MQ_Q_MGR_NAME_LENGTH);
					*ParmCount += 2;
					if (MyParms->diagnostics>20) {
						fprintf(
						stderr,
						"(args.c) Processing %s flag, setting Remote Queue Manager to %s\n",REMOTE_QMGR_SHORT_NAME,MyParms->RmtQMgrName);
					}
				} else {
					inValidParm = 1;
				}
			} else {
				if (memcmp(argv[*ParmCount], 
				REMOTE_QMGR_LONG_NAME, strlen(REMOTE_QMGR_LONG_NAME)) == 0) {
					strncpy(MyParms->RmtQMgrName, argv[*ParmCount]
							+strlen(REMOTE_QMGR_LONG_NAME), 
					MQ_Q_MGR_NAME_LENGTH);
					*ParmCount += 1;
				} else {
					strncpy(MyParms->RmtQMgrName, argv[*ParmCount]
							+strlen(REMOTE_QMGR_SHORT_NAME), 
					MQ_Q_MGR_NAME_LENGTH);
					*ParmCount += 1;
				}
				if (MyParms->diagnostics>20) {
					fprintf(
					stderr,
					"(args.c) Processing %s flag, setting Remote Queue Manager to %s\n",REMOTE_QMGR_SHORT_NAME,MyParms->RmtQMgrName);
				}
			}
			/* was a "generate this version mqsc" specified? */
		} else if (memcmp(argv[*ParmCount], COMSVRQ_SHORT_NAME,
				strlen(COMSVRQ_SHORT_NAME)) == 0 || memcmp(argv[*ParmCount], 
		COMSVRQ_LONG_NAME, strlen(COMSVRQ_LONG_NAME)) == 0) {
			/* test to see if there is a blank after the flag */
			if (strlen(argv[*ParmCount]) != strlen(COMSVRQ_SHORT_NAME)
					&& strlen(argv[*ParmCount]) != strlen(COMSVRQ_LONG_NAME)) {
				if (memcmp(argv[*ParmCount], COMSVRQ_LONG_NAME,
						strlen(COMSVRQ_LONG_NAME)) == 0) {
					strncpy(MyParms->CmdSvrQName, argv[*ParmCount]
							+strlen(COMSVRQ_LONG_NAME),
							sizeof(MyParms->CmdSvrQName));
					*ParmCount += 1;
				} else {
					strncpy(MyParms->CmdSvrQName, argv[*ParmCount]
							+strlen(COMSVRQ_SHORT_NAME),
							sizeof(MyParms->CmdSvrQName));
					*ParmCount += 1;
				}
				if (MyParms->diagnostics>20) {
					fprintf(
					stderr,
					"(args.c) Processing %s flag, setting Command Server Queue to %s\n",COMSVRQ_SHORT_NAME,MyParms->CmdSvrQName);
				}
			} else {
				if (*ParmCount < argc-1) {
					strncpy(MyParms->CmdSvrQName, argv[*ParmCount+1],
							sizeof(MyParms->CmdSvrQName));
					*ParmCount += 2;
					if (MyParms->diagnostics>20) {
						fprintf(
						stderr,
						"(args.c) Processing %s flag, setting Command Server Queue to %s\n",COMSVRQ_SHORT_NAME,MyParms->CmdSvrQName);
					}
				} else {
					inValidParm = 1;
				}
			}
			/* was a channel send exit specified ? */
		} else if ((memcmp(argv[*ParmCount], VERSION_SHORT_NAME,
				strlen(VERSION_SHORT_NAME)) == 0 ) || (memcmp(argv[*ParmCount], 
		VERSION_LONG_NAME, strlen(VERSION_LONG_NAME)) == 0 )) {

			/* test to see if there is a blank after the flag */
			if ((strlen(argv[*ParmCount]) != strlen(VERSION_SHORT_NAME))
					&& (strlen(argv[*ParmCount]) != strlen(VERSION_LONG_NAME))) {
				if (memcmp(argv[*ParmCount], VERSION_LONG_NAME,
						strlen(VERSION_LONG_NAME)) == 0) {
					MyParms->UserVersion = atoi(argv[*ParmCount]
							+strlen(VERSION_LONG_NAME));
					strcpy(MyParms->StrVersion, argv[*ParmCount]
							+strlen(VERSION_LONG_NAME));
					*ParmCount += 1;
				} else {
					MyParms->UserVersion = atoi(argv[*ParmCount]
							+strlen(VERSION_SHORT_NAME));
					strcpy(MyParms->StrVersion, argv[*ParmCount]
							+strlen(VERSION_SHORT_NAME));
					*ParmCount += 1;
				}
				if (MyParms->diagnostics>20) {
					fprintf(
					stderr,
					"(args.c) Processing %s flag, setting DEPRECATED Version to %s\n",VERSION_SHORT_NAME,MyParms->StrVersion);
				}
			} else {
				if (*ParmCount < argc-1) {
					MyParms->UserVersion = atoi(argv[*ParmCount+1]);
					strcpy(MyParms->StrVersion, argv[*ParmCount+1]); /* v531a ... check user version */
					*ParmCount += 2;
					if (MyParms->diagnostics>20) {
						fprintf(
						stderr,
						"(args.c) Processing %s flag, setting DEPRECATED Version to %s\n",VERSION_SHORT_NAME,MyParms->StrVersion);
					}
				} else {
					inValidParm = 1;
				}
			}
			if (!MyParms->UserVersion)
				MyParms->UserVersion = -1; /* v531a ... check user version */
			switch (MyParms->UserVersion) {
			case 0:
				/* user didn't specify a level , leave alone for now */
				break;
			case 2:
			case 20:
				MyParms->UserVersion = MQCMDL_LEVEL_200;
				break;
				/* don't allow these to be specified if not compiled for this level */
#if defined (MQDL_NOT_SUPPORTED)
			case 5:
			case 50:
				MyParms->UserVersion = MQCMDL_LEVEL_500;
				break;
#endif
				/* don't allow these to be specified if not compiled for this level */
#if defined (MQOO_BIND_ON_OPEN)
			case 51:
			case 510:
				MyParms->UserVersion = MQCMDL_LEVEL_510;
				break;
#endif
				/* don't allow these to be specified if not compiled for this level */
#if defined (MQCMDL_LEVEL_520)
			case 52:
			case 520:
				MyParms->UserVersion = MQCMDL_LEVEL_520;
				break;
#endif
#if defined (MQCMD_INQUIRE_AUTH_INFO)
			case 53:
			case 530:
				MyParms->UserVersion = MQCMDL_LEVEL_530;
				break;
#endif
#if defined (MQOT_SERVICE)
			case 6:
			case 600:
				MyParms->UserVersion = MQCMDL_LEVEL_600;
				break;
#endif
#if defined (MQOT_TOPIC)
			case 7:
			case 700:
				MyParms->UserVersion = MQCMDL_LEVEL_700;
				break;
			case 701:
				MyParms->UserVersion = MQCMDL_LEVEL_701;
				break;
#endif
			default: /* v531a give better error msg */
				fprintf(stderr,
				"Invalid invocation, Invalid Version Number: \"%s\"\n",
				MyParms->StrVersion);
				if (isClient) {
					fprintf(stderr, USAGE, MyParms->myName, USAGE2);
				} else {
					fprintf(stderr, USAGE, MyParms->myName, USAGE3);
				}
				fprintf(stderr, " \n\n %s\n\n", THISVERSION);
				exit( ERROR);
				break;
			}/* End switch*/
			if (MyParms->diagnostics>20) {
				fprintf(
				stderr,
				"(args.c) Processing %s flag, setting Numeric Version to %d\n",VERSION_SHORT_NAME,MyParms->UserVersion);
			}
			/* was an output filename flag and optional name specified? */
		} else if ( (memcmp(argv[*ParmCount], OUTPUT_FILE_SHORT_NAME,
				strlen(OUTPUT_FILE_SHORT_NAME) ) == 0 ) || (memcmp(
				argv[*ParmCount], OUTPUT_FILE_LONG_NAME,
				strlen(OUTPUT_FILE_LONG_NAME) ) == 0 ) || (memcmp(
				argv[*ParmCount], OUTPUT_FILE_NH_SHORT_NAME,
				strlen(OUTPUT_FILE_NH_SHORT_NAME) ) == 0 ) || (memcmp(
				argv[*ParmCount], OUTPUT_FILE_NH_LONG_NAME,
				strlen(OUTPUT_FILE_NH_LONG_NAME) ) == 0 )) {
			/* v513 */
			if ( (memcmp(argv[*ParmCount], OUTPUT_FILE_NH_SHORT_NAME,
					strlen(OUTPUT_FILE_NH_SHORT_NAME) ) == 0 )|| (memcmp(
					argv[*ParmCount], OUTPUT_FILE_NH_LONG_NAME,
					strlen(OUTPUT_FILE_NH_LONG_NAME) ) == 0 )) {
				MyParms->header=0; /* v513 */
				if (MyParms->diagnostics>20) {
					fprintf(
					stderr,
					"(args.c) Processing %s flag, setting no header to true \n",OUTPUT_FILE_NH_SHORT_NAME);
				}
			}
			MyParms->fileName[0] = '\0'; /* V3.2 user wants unique output queue names */
			/* test to see if there is a blank after the flag */
			if (strlen(argv[*ParmCount]) != strlen(OUTPUT_FILE_LONG_NAME)
					&& strlen(argv[*ParmCount])
							!= strlen(OUTPUT_FILE_SHORT_NAME)
					&& strlen(argv[*ParmCount])
							!= strlen(OUTPUT_FILE_NH_SHORT_NAME)
					&& strlen(argv[*ParmCount])
							!= strlen(OUTPUT_FILE_NH_LONG_NAME)) {
				if (memcmp(argv[*ParmCount], OUTPUT_FILE_LONG_NAME,
						strlen(OUTPUT_FILE_LONG_NAME)) == 0) {
					strncpy(MyParms->fileName, argv[*ParmCount]
							+strlen(OUTPUT_FILE_LONG_NAME), FILENAMESIZE);
				} else if (memcmp(argv[*ParmCount], OUTPUT_FILE_NH_LONG_NAME,
						strlen(OUTPUT_FILE_NH_LONG_NAME)) == 0) {
					strncpy(MyParms->fileName, argv[*ParmCount]
							+strlen(OUTPUT_FILE_NH_LONG_NAME), FILENAMESIZE);
				} else {
					strncpy(MyParms->fileName, argv[*ParmCount]+2, FILENAMESIZE);
				};
				if (MyParms->diagnostics>20) {
					fprintf(
					stderr,
					"(args.c) Processing %s flag, setting Output File Name to %s\n",OUTPUT_FILE_SHORT_NAME,MyParms->fileName);
				}
			} else {
				if ((*ParmCount+1) < argc && argv[*ParmCount+1][0] != '-') { /* is the next token not a switch ? */
					/* yes.. then the next token is a filename */
					*ParmCount += 1;
					strncpy(MyParms->fileName, argv[*ParmCount], 
					FILENAMESIZE);
					if (MyParms->diagnostics>20) {
						fprintf(
						stderr,
						"(args.c) Processing %s flag, setting Output File Name to %s\n",OUTPUT_FILE_SHORT_NAME,MyParms->fileName);
					}
				}; /* End if next char is not a switch */
			}
			*ParmCount += 1;

#if defined(zzMQV60)
			/* support the creation of authorization records */
		} else if (memcmp(argv[*ParmCount], SAVE_SEC_SHORT_NAME,
				strlen(SAVE_SEC_SHORT_NAME) ) == 0 || memcmp(argv[*ParmCount], 
		SAVE_SEC_NH_SHORT_NAME, strlen(SAVE_SEC_NH_SHORT_NAME) ) == 0
				|| memcmp(argv[*ParmCount], 
				SAVE_SEC_LONG_NAME, strlen(SAVE_SEC_LONG_NAME) ) == 0
				|| memcmp(argv[*ParmCount], SAVE_SEC_NH_LONG_NAME,
						strlen(SAVE_SEC_NH_LONG_NAME) ) == 0) {
			if (memcmp(argv[*ParmCount], SAVE_SEC_NH_SHORT_NAME,
					strlen(SAVE_SEC_NH_SHORT_NAME)) == 0 || memcmp(
					argv[*ParmCount], SAVE_SEC_NH_LONG_NAME,
					strlen(SAVE_SEC_NH_LONG_NAME) ) == 0) {
				MyParms->authHeader=0;
				if (MyParms->diagnostics>20) {
					fprintf(
					stderr,
					"(args.c) Processing %s flag, setting No Header to true\n",SAVE_SEC_NH_SHORT_NAME);
				}
			}
			MyParms->authFileName[0] = '\0';
			MyParms->makeSecurityDefs = 1;
			if (MyParms->diagnostics>20) {
				fprintf(
				stderr,
				"(args.c) Processing %s flag, setting Save Security to true\n",SAVE_SEC_NH_SHORT_NAME);
			}
			/* test to see if there is a blank after the flag */
			if (strlen(argv[*ParmCount]) != strlen(SAVE_SEC_LONG_NAME)
					&& strlen(argv[*ParmCount]) != strlen(SAVE_SEC_SHORT_NAME)
					&& strlen(argv[*ParmCount])
							!= strlen(SAVE_SEC_NH_SHORT_NAME)
					&& strlen(argv[*ParmCount])
							!= strlen(SAVE_SEC_NH_LONG_NAME)) {
				if (memcmp(argv[*ParmCount], SAVE_SEC_LONG_NAME,
						strlen(SAVE_SEC_LONG_NAME)) == 0) {
					strncpy(MyParms->authFileName, argv[*ParmCount]
							+strlen(SAVE_SEC_LONG_NAME), FILENAMESIZE);
				} else if (memcmp(argv[*ParmCount], SAVE_SEC_NH_LONG_NAME,
						strlen(SAVE_SEC_NH_LONG_NAME)) == 0) {
					strncpy(MyParms->authFileName, argv[*ParmCount]
							+strlen(SAVE_SEC_NH_LONG_NAME), FILENAMESIZE);
				} else {
					strncpy(MyParms->authFileName, argv[*ParmCount]+2, 
					FILENAMESIZE);
				};
				if (MyParms->diagnostics>20) {
					fprintf(
					stderr,
					"(args.c) Processing %s flag, setting Security File Name to %s\n",SAVE_SEC_SHORT_NAME,MyParms->authFileName);
				}
			} else {
				if ((*ParmCount+1) < argc && argv[*ParmCount+1][0] != '-') { /* is the next token not a switch ? */

					/* yes.. then the next token is a filename */
					*ParmCount += 1;
					strncpy(MyParms->authFileName, argv[*ParmCount], 
					FILENAMESIZE);
					*ParmCount += 1;
					if (MyParms->diagnostics>20) {
						fprintf(
						stderr,
						"(args.c) Processing %s flag, setting Security File Name to %s\n",SAVE_SEC_SHORT_NAME,MyParms->authFileName);
					}
				} else {
					if (MyParms->diagnostics>20) {
						fprintf(
						stderr,
						"(args.c) Processing %s flag, setting Security File Name to %s\n",SAVE_SEC_SHORT_NAME,DEFAULTOUTPUTFILEAUT);
					}
				*ParmCount += 1;
				}
			}
#endif
			/* does the user want the output to stdout */
		} else if (memcmp(argv[*ParmCount], STDOUT_SHORT_NAME,
				strlen(STDOUT_SHORT_NAME)) == 0 || memcmp(argv[*ParmCount], 
		STDOUT_LONG_NAME, strlen(STDOUT_LONG_NAME)) == 0) {
			/*             outputfp = std_out;    user wants stdout option */
			MyParms->std_out = 1;
			if (MyParms->diagnostics>20) {
				fprintf(
				stderr,
				"(args.c) Processing %s flag, setting flag to use standard output \n",STDOUT_SHORT_NAME);
			}
			*ParmCount += 1;
		} else if (memcmp(argv[*ParmCount], NO_SYSOBJS_SHORT_NAME,
				strlen(NO_SYSOBJS_SHORT_NAME)) == 0 || memcmp(argv[*ParmCount], 
		NO_SYSOBJS_LONG_NAME, NO_SYSOBJS_LONG_LN) == 0) {
			/* does the user want SYSTEM objects supressed? */
			if (MyParms->diagnostics> 20) {
				fprintf(
				stderr,
				"(args.c) Processing  flag %s with length %d\n",
				argv[*ParmCount], strlen(argv[*ParmCount]));
			}
			MyParms->SysObjs = SO_NONE; /* user does NOT want SYSTEM objects */
			/* Is there no space between the switch and the parameter value? */
			if ((strlen(argv[*ParmCount]) != strlen(NO_SYSOBJS_SHORT_NAME))
					&& (strlen(argv[*ParmCount]) != NO_SYSOBJS_LONG_LN)) {
				if (memcmp(argv[*ParmCount], 
				NO_SYSOBJS_LONG_NAME, NO_SYSOBJS_LONG_LN) == 0) {
					if (MyParms->diagnostics>2) {
						fprintf(
						stderr,
						"(args.c) Processing  flag option value %s\n",
						argv[*ParmCount]+NO_SYSOBJS_LONG_LN);
					}
					strncpy(sysObjParms, argv[*ParmCount] +NO_SYSOBJS_LONG_LN,
							sizeof(sysObjParms));

				} else {
					if (MyParms->diagnostics>20) {
						fprintf(
						stderr,
						"(args.c) Processing  flag option value %s\n",
						argv[*ParmCount]+2);
					}
					strncpy(sysObjParms, argv[*ParmCount]+2,
							sizeof(sysObjParms));
				}
				*ParmCount += 1;
			}
			/* is the next token a switch or there are no more tokens? */
			else if (((*ParmCount+1) < argc && argv[*ParmCount+1][0] == '-')
					||(*ParmCount+1 >=argc)) {
				if (MyParms->diagnostics>20) {
					fprintf(stderr,
					"(args.c) Processing sysObjs flag with no option value \n");
				}
				MyParms->SysObjs = SO_NONE; /* user does NOT want SYSTEM objects */
				sysObjParms[0]='\0';
				*ParmCount += 1;
			} else if ((*ParmCount+1)< argc) {
				if (MyParms->diagnostics>20) {
					fprintf(
					stderr,
					"(args.c) Processing sysObjs flag with option value after space %s, argc %d ParmCount %d\n",
					argv[*ParmCount+1], argc, *ParmCount);
				}
				strncpy(sysObjParms, argv[*ParmCount+1], sizeof(sysObjParms));
				*ParmCount += 2;
			}
			/*process sysObjParms
			 strtok QMGR,QUEUE,NAMELIST,PROCESS,AUTHINFO,LISTENER,SERVICE,TOPIC,SUB,NONE */
			found = strtok(sysObjParms, ",");
			while (found != NULL) {

				toUpperStr(found);
				if (MyParms->diagnostics>20) {
					fprintf(stderr, "(args.c) tokenFound is %s\n", found);
				}
				if (!strcmp(found, "QMGR")) {
					if (MyParms->diagnostics>20) {
						fprintf(
						stderr,
						"(args.c) setting SO_QMGR flag current value=%d ",
						MyParms->SysObjs);
					}
					MyParms->SysObjs |= SO_QMGR;
					if (MyParms->diagnostics>20) {
						fprintf(stderr, " new value is %d\n", MyParms->SysObjs);
					}
				} else if (!strcmp(found, "QUEUE")) {
					MyParms->SysObjs |= SO_QUEUE;
				} else if (!strcmp(found, "NAMELIST")) {
					MyParms->SysObjs |= SO_NAMELIST;
				} else if (!strcmp(found, "PROCESS")) {
					MyParms->SysObjs |= SO_PROCESS;
				} else if (!strcmp(found, "AUTHINFO")) {
					MyParms->SysObjs |= SO_AUTHINFO;
				} else if (!strcmp(found, "LISTENER")) {
					MyParms->SysObjs |= SO_LISTENER;
				} else if (!strcmp(found, "CHANNEL")) {
					MyParms->SysObjs |= SO_CHANNEL;
				} else if (!strcmp(found, "SERVICE")) {
					MyParms->SysObjs |= SO_SERVICE;
				} else if (!strcmp(found, "TOPIC")) {
					MyParms->SysObjs |= SO_TOPIC;
				} else if (!strcmp(found, "SUB")) {
					MyParms->SysObjs |= SO_SUB;
				} else if (!strcmp(found, "NONE")) {
					MyParms->SysObjs = SO_NONE;
				}
				found = strtok(NULL, ",");
			}
			if (MyParms->SysObjs & SO_NONE)
				MyParms->SysObjs= SO_NONE;
			if (MyParms->diagnostics>20) {
				fprintf(stderr, "(args.c)  flag has value %d\n",
				MyParms->SysObjs);
				fprintf(stderr,
				"(args.c)  flag AND SO_NONE is %d\n",
				(MyParms->SysObjs & SO_NONE));
				fprintf(stderr,
				"(args.c)  flag AND SO_QMGR is %d\n",
				(MyParms->SysObjs & SO_QMGR));
			}
		} else if (memcmp(argv[*ParmCount], ONLY_QMGR_LONG_NAME,
				strlen(ONLY_QMGR_LONG_NAME)) == 0) {
			MyParms->SysObjs = SO_QMGR;
			if (MyParms->diagnostics>20) {
				fprintf(
				stderr,
				"(args.c) Processing --qm flag, setting SysObjs %d to SO_QMGR %d\n",
				MyParms->SysObjs, SO_QMGR);
			}
			*ParmCount += 1;
			/* does the user want DIAG messages */
		} else if (memcmp(argv[*ParmCount], DIAG_SHORT_NAME,
				strlen(DIAG_SHORT_NAME)) == 0 || memcmp(argv[*ParmCount], 
		DIAG_LONG_NAME, strlen(DIAG_LONG_NAME)) == 0) {
			MyParms->diagnostics=1; /* defaul to 1 if not specified */
			/* test to see if there is a blank after the flag */

			if (strlen(argv[*ParmCount]) != strlen(DIAG_SHORT_NAME)
					&& strlen(argv[*ParmCount]) != strlen(DIAG_LONG_NAME)) {
				MyParms->diagnostics = atoi(argv[*ParmCount]+2);
			}
			*ParmCount += 1;
			if (*ParmCount < argc && argv[*ParmCount][0] != '-') { /* is the next token not a switch ? */
				/* yes.. then the next token is a diagnostic level */
				MyParms->diagnostics = atoi(argv[*ParmCount]);
				if (MyParms->diagnostics>20) {
					fprintf(
					stderr,
					"(args.c) Processing %s flag, setting Diagnostic level to %d\n",SAVE_SEC_SHORT_NAME,MyParms->authFileName);
				}
				*ParmCount += 1;
			} /* End if next char is not a switch */

			/* was a conname specified? */
		} else if (memcmp(argv[*ParmCount], HOSTNAME_SHORT_NAME,
				strlen(HOSTNAME_SHORT_NAME)) == 0 || memcmp(argv[*ParmCount], 
		HOSTNAME_LONG_NAME, strlen(HOSTNAME_LONG_NAME)) == 0) {

			if (!isClient) {
				inValidParm = 1;
				break;
			}
			/* test to see if there is a blank after the flag */
			if (strlen(argv[*ParmCount]) != strlen(HOSTNAME_SHORT_NAME)
					&& strlen(argv[*ParmCount]) != strlen(HOSTNAME_LONG_NAME)) {
				if (memcmp(argv[*ParmCount], HOSTNAME_LONG_NAME,
						strlen(HOSTNAME_LONG_NAME)) == 0) {
					strncpy(MyParms->argCONNAME, argv[*ParmCount]
							+strlen(HOSTNAME_LONG_NAME), 
					MQ_CONN_NAME_LENGTH);
					*ParmCount += 1;
				} else {
					strncpy(MyParms->argCONNAME, argv[*ParmCount]+2, 
					MQ_CONN_NAME_LENGTH);
					*ParmCount += 1;
				}
				if (MyParms->diagnostics>20) {
					fprintf(
					stderr,
					"(args.c) Processing %s flag, setting Host Name to %s\n",HOSTNAME_SHORT_NAME,MyParms->argCONNAME);
				}
			} else {
				if (*ParmCount < argc-1) {
					strncpy(MyParms->argCONNAME, argv[*ParmCount+1], 
					MQ_CONN_NAME_LENGTH);
					*ParmCount += 2;
					if (MyParms->diagnostics>20) {
						fprintf(
						stderr,
						"(args.c) Processing %s flag, setting Host Name to %s\n",HOSTNAME_SHORT_NAME,MyParms->argCONNAME);
					}
				} else {
					inValidParm = 1;
				}
			}
			MyParms->flagCONNAME++;

			/* was a channel name specified? */
		} else if (memcmp(argv[*ParmCount], CHANNEL_SHORT_NAME,
				strlen(CHANNEL_SHORT_NAME)) == 0 || memcmp(argv[*ParmCount], 
		CHANNEL_LONG_NAME, strlen(CHANNEL_LONG_NAME)) == 0) {

			if (!isClient) {
				inValidParm = 1;
				break;
			}
			/* test to see if there is a blank after the flag */
			if (strlen(argv[*ParmCount]) != strlen(CHANNEL_SHORT_NAME)
					&& strlen(argv[*ParmCount]) != strlen(CHANNEL_LONG_NAME)) {
				if (memcmp(argv[*ParmCount], CHANNEL_LONG_NAME,
						strlen(CHANNEL_LONG_NAME)) == 0) {
					strncpy(MyParms->argSVRCONN, argv[*ParmCount]
							+strlen(CHANNEL_LONG_NAME), MQ_CHANNEL_NAME_LENGTH);
					*ParmCount += 1;
				} else {
					strncpy(MyParms->argSVRCONN, argv[*ParmCount]+2, 
					MQ_CHANNEL_NAME_LENGTH);
					*ParmCount += 1;
				}
				if (MyParms->diagnostics>20) {
					fprintf(
					stderr,
					"(args.c) Processing %s flag, setting Channel Name to %s\n",CHANNEL_SHORT_NAME,MyParms->argSVRCONN);
				}
			} else {
				if (*ParmCount < argc-1) {
					strncpy(MyParms->argSVRCONN, argv[*ParmCount+1], 
					MQ_CHANNEL_NAME_LENGTH);
					*ParmCount += 2;
					if (MyParms->diagnostics>20) {
						fprintf(
						stderr,
						"(args.c) Processing %s flag, setting Channel Name to %s\n",CHANNEL_SHORT_NAME,MyParms->argSVRCONN);
					}
				} else {
					inValidParm = 1;
				}
			}
			MyParms->flagCONNAME++;

#if defined USESSL
			/* was an SSL key repository specified? */
		} else if (memcmp(argv[*ParmCount], KEYREPOS_SHORT_NAME,
				strlen(KEYREPOS_SHORT_NAME)) == 0 || memcmp(argv[*ParmCount], 
		KEYREPOS_LONG_NAME, strlen(KEYREPOS_LONG_NAME)) == 0) {
			if (!isClient) {
				inValidParm = 1;
				if (MyParms->diagnostics>20) {
					fprintf(
					stderr,
					"(args.c) Processing -k flag, failed because we are not a client build.");
				}
				break;
			}
			/* test to see if there is a blank after the flag */
			if (strlen(argv[*ParmCount]) != strlen(KEYREPOS_SHORT_NAME)
					&& strlen(argv[*ParmCount]) != strlen(KEYREPOS_LONG_NAME)) {

				if (memcmp(argv[*ParmCount], KEYREPOS_LONG_NAME,
						strlen(KEYREPOS_LONG_NAME)) == 0) {
					strncpy(MyParms->SSL_options.KeyRepository,
							argv[*ParmCount]+sizeof(KEYREPOS_LONG_NAME),
							sizeof(MyParms->SSL_options.KeyRepository));
				} else {
					strncpy(MyParms->SSL_options.KeyRepository,
							argv[*ParmCount]+2,
							sizeof(MyParms->SSL_options.KeyRepository));
				}
				if (MyParms->diagnostics>20) {
					fprintf(
					stderr,
					"(args.c) Processing -k flag, setting KeyRepository to %s\n",MyParms->SSL_options.KeyRepository);
				}
				*ParmCount += 1;
			} else {
				if (*ParmCount < argc-1) {
					strncpy(MyParms->SSL_options.KeyRepository,
							argv[*ParmCount+1],
							sizeof(MyParms->SSL_options.KeyRepository));
					if (MyParms->diagnostics>20) {
						fprintf(
						stderr,
						"(args.c) Processing -k flag, setting KeyRepository to %s\n",MyParms->SSL_options.KeyRepository);
					}
					*ParmCount += 2;
				} else {
					inValidParm = 1;
				}
			}
			MyParms->flagSSL++;

			/* was a CipherSpec specified ? */
		} else if (memcmp(argv[*ParmCount], SSL_CIPHER_SHORT_NAME,
				strlen(SSL_CIPHER_SHORT_NAME)) == 0 || memcmp(argv[*ParmCount], 
		SSL_CIPHER_LONG_NAME, strlen(SSL_CIPHER_LONG_NAME)) == 0) {
			if (!isClient) {
				inValidParm = 1;
				if (MyParms->diagnostics>20) {
					fprintf(
					stderr,
					"(args.c) Processing -C flag failed because we are not a client build.\n");
				}
				break;
			}
			/* test to see if there is a blank after the flag */
			if (strlen(argv[*ParmCount]) != strlen(SSL_CIPHER_SHORT_NAME)
					&& strlen(argv[*ParmCount]) != strlen(SSL_CIPHER_LONG_NAME)) {
				if (memcmp(argv[*ParmCount], SSL_CIPHER_LONG_NAME,
						strlen(SSL_CIPHER_LONG_NAME)) == 0) {
					strncpy(MyParms->ClientConn.SSLCipherSpec, argv[*ParmCount]
							+sizeof(SSL_CIPHER_LONG_NAME),
							sizeof(MyParms->SSL_options.KeyRepository));
				} else {
					strncpy(MyParms->ClientConn.SSLCipherSpec, argv[*ParmCount]
							+2, sizeof(MyParms->ClientConn.SSLCipherSpec));
				}
				if (MyParms->diagnostics>20) {
					fprintf(
					stderr,
					"(args.c) Processing -C flag, setting SSL Cipher to %s\n",MyParms->ClientConn.SSLCipherSpec);
				}
				*ParmCount += 1;
			} else {
				if (*ParmCount < argc-1) {
					strncpy(MyParms->ClientConn.SSLCipherSpec,
							argv[*ParmCount+1],
							sizeof(MyParms->ClientConn.SSLCipherSpec));
					if (MyParms->diagnostics>20) {
						fprintf(
						stderr,
						"(args.c) Processing -C flag, setting SSL Cipher to %s\n",MyParms->ClientConn.SSLCipherSpec);
					}
					*ParmCount += 2;
				} else {
					inValidParm = 1;
				}
			}
			MyParms->flagSSL++;
#endif  /* of use SSL */

			/* was a channel security exit specified ? */
		} else if (memcmp(argv[*ParmCount], CHANNEL_EXIT_Y_SHORT_NAME,
				strlen(CHANNEL_EXIT_Y_SHORT_NAME)) == 0 || memcmp(
				argv[*ParmCount], CHANNEL_EXIT_Y_LONG_NAME,
				strlen(CHANNEL_EXIT_Y_LONG_NAME)) == 0) {

			if (!isClient) {
				inValidParm = 1;
				break;
			}
			/* test to see if there is a blank after the flag */
			if (strlen(argv[*ParmCount]) != strlen(CHANNEL_EXIT_Y_SHORT_NAME)
					&& strlen(argv[*ParmCount])
							!= strlen(CHANNEL_EXIT_Y_LONG_NAME)) {
				if (memcmp(argv[*ParmCount], CHANNEL_EXIT_Y_LONG_NAME,
						strlen(CHANNEL_EXIT_Y_LONG_NAME)) == 0) {
					strncpy(MyParms->ClientConn.SecurityExit, argv[*ParmCount]
							+strlen(CHANNEL_EXIT_Y_LONG_NAME),
							sizeof(MyParms->ClientConn.SecurityExit));
					*ParmCount += 1;
				} else {
					strncpy(MyParms->ClientConn.SecurityExit, argv[*ParmCount]
							+3, sizeof(MyParms->ClientConn.SecurityExit));

					*ParmCount += 1;
				}
				if (MyParms->diagnostics>20) {
					fprintf(
					stderr,
					"(args.c) Processing %s flag, setting Channel Security Exit to %s\n",CHANNEL_EXIT_Y_SHORT_NAME,MyParms->ClientConn.SecurityExit);
				}
			} else {
				if (*ParmCount < argc-1) {
					strncpy(MyParms->ClientConn.SecurityExit,
							argv[*ParmCount+1],
							sizeof(MyParms->ClientConn.SecurityExit));
					*ParmCount += 2;
					if (MyParms->diagnostics>20) {
						fprintf(
						stderr,
						"(args.c) Processing %s flag, setting Channel Security Exit to %s\n",CHANNEL_EXIT_Y_SHORT_NAME,MyParms->ClientConn.SecurityExit);
					}
				} else {
					inValidParm = 1;
				}
			}
			/* was a channel send exit specified ? */
		} else if (memcmp(argv[*ParmCount], CHANNEL_EXIT_S_SHORT_NAME,
				strlen(CHANNEL_EXIT_S_SHORT_NAME)) == 0 || memcmp(
				argv[*ParmCount], CHANNEL_EXIT_S_LONG_NAME,
				strlen(CHANNEL_EXIT_S_LONG_NAME)) == 0) {
			if (!isClient) {
				inValidParm = 1;
				break;
			}
			/* test to see if there is a blank after the flag */
			if (strlen(argv[*ParmCount]) != strlen(CHANNEL_EXIT_S_SHORT_NAME)
					&& strlen(argv[*ParmCount])
							!= strlen(CHANNEL_EXIT_S_LONG_NAME)) {
				if (memcmp(argv[*ParmCount], CHANNEL_EXIT_S_LONG_NAME,
						strlen(CHANNEL_EXIT_S_LONG_NAME)) == 0) {
					strncpy(MyParms->ClientConn.SendExit, argv[*ParmCount]
							+strlen(CHANNEL_EXIT_S_LONG_NAME),
							sizeof(MyParms->ClientConn.SendExit));
					*ParmCount += 1;
				} else {
					strncpy(MyParms->ClientConn.SendExit, argv[*ParmCount] +3,
							sizeof(MyParms->ClientConn.SendExit));
					*ParmCount += 1;
					if (MyParms->diagnostics>20) {
						fprintf(
						stderr,
						"(args.c) Processing %s flag, setting Channel Send Exit to %s\n",CHANNEL_EXIT_S_SHORT_NAME,MyParms->ClientConn.SendExit);
					}
				}
			} else {
				if (*ParmCount < argc-1) {
					strncpy(MyParms->ClientConn.SendExit, argv[*ParmCount+1],
							sizeof(MyParms->ClientConn.SendExit));
					*ParmCount += 2;
					if (MyParms->diagnostics>20) {
						fprintf(
						stderr,
						"(args.c) Processing %s flag, setting Channel Send Exit to %s\n",CHANNEL_EXIT_S_SHORT_NAME,MyParms->ClientConn.SendExit);
					}
				} else {
					inValidParm = 1;
				}
			}
			/* was a channel receive exit specified ? */
		} else if (memcmp(argv[*ParmCount], CHANNEL_EXIT_R_SHORT_NAME,
				strlen(CHANNEL_EXIT_R_SHORT_NAME)) == 0 || memcmp(
				argv[*ParmCount], CHANNEL_EXIT_R_LONG_NAME,
				strlen(CHANNEL_EXIT_R_LONG_NAME)) == 0) {

			if (!isClient) {
				inValidParm = 1;
				break;
			}
			/* Is there no space between the switch and the parameter value? */
			if ((strlen(argv[*ParmCount]) != strlen(CHANNEL_EXIT_R_SHORT_NAME))
					&& (strlen(argv[*ParmCount])
							!= strlen(CHANNEL_EXIT_R_LONG_NAME))) {
				if (memcmp(argv[*ParmCount], 
				CHANNEL_EXIT_R_LONG_NAME, strlen(CHANNEL_EXIT_R_LONG_NAME))
						== 0) {
					strncpy(MyParms->ClientConn.ReceiveExit, argv[*ParmCount]
							+strlen(CHANNEL_EXIT_R_LONG_NAME),
							sizeof(MyParms->ClientConn.ReceiveExit));
					*ParmCount += 1;
				} else {
					strncpy(MyParms->ClientConn.ReceiveExit,
							argv[*ParmCount]+3,
							sizeof(MyParms->ClientConn.ReceiveExit));
					*ParmCount += 1;
				}
				if (MyParms->diagnostics>20) {
					fprintf(
					stderr,
					"(args.c) Processing %s flag, setting Channel Receive Exit to %s\n",CHANNEL_EXIT_R_SHORT_NAME,MyParms->ClientConn.ReceiveExit);
				}
			} else {
				if (*ParmCount < argc-1) {
					strncpy(MyParms->ClientConn.ReceiveExit,
							argv[*ParmCount+1],
							sizeof(MyParms->ClientConn.ReceiveExit));
					*ParmCount += 2;
					if (MyParms->diagnostics>20) {
						fprintf(
						stderr,
						"(args.c) Processing %s flag, setting Channel Receive Exit to %s\n",CHANNEL_EXIT_R_SHORT_NAME,MyParms->ClientConn.ReceiveExit);
					}
				} else {
					inValidParm = 1;
				}
			}

			/* was a channel security data specified ? */
		} else if (memcmp(argv[*ParmCount], CHANNEL_DATA_Y_SHORT_NAME,
				strlen(CHANNEL_DATA_Y_SHORT_NAME)) == 0 || memcmp(
				argv[*ParmCount], CHANNEL_DATA_Y_LONG_NAME,
				strlen(CHANNEL_DATA_Y_LONG_NAME)) == 0) {

			if (!isClient) {
				inValidParm = 1;
				break;
			}
			/* test to see if there is a blank after the flag */
			if (strlen(argv[*ParmCount]) != strlen(CHANNEL_DATA_Y_SHORT_NAME)
					&& strlen(argv[*ParmCount])
							!= strlen(CHANNEL_DATA_Y_LONG_NAME)) {
				if (memcmp(argv[*ParmCount], CHANNEL_DATA_Y_LONG_NAME,
						strlen(CHANNEL_DATA_Y_LONG_NAME)) == 0) {
					strncpy(MyParms->ClientConn.SecurityUserData,
							argv[*ParmCount] +strlen(CHANNEL_DATA_Y_LONG_NAME),
							sizeof(MyParms->ClientConn.SecurityUserData));
					*ParmCount += 1;
				} else {
					strncpy(MyParms->ClientConn.SecurityUserData,
							argv[*ParmCount] +3,
							sizeof(MyParms->ClientConn.SecurityUserData));
					*ParmCount += 1;
				}
				if (MyParms->diagnostics>20) {
					fprintf(
					stderr,
					"(args.c) Processing %s flag, setting Channel Security User Data to %s\n",CHANNEL_DATA_Y_SHORT_NAME,MyParms->ClientConn.SecurityUserData);
				}
			} else {
				if (*ParmCount < argc-1) {
					strncpy(MyParms->ClientConn.SecurityUserData,
							argv[*ParmCount+1],
							sizeof(MyParms->ClientConn.SecurityUserData));
					*ParmCount += 2;
					if (MyParms->diagnostics>20) {
						fprintf(
						stderr,
						"(args.c) Processing %s flag, setting Channel Security User Data to %s\n",CHANNEL_DATA_Y_SHORT_NAME,MyParms->ClientConn.SecurityUserData);
					}
				} else {
					inValidParm = 1;
				}
			}
			/* was a channel send data specified ? */
		} else if (memcmp(argv[*ParmCount], CHANNEL_DATA_S_SHORT_NAME,
				strlen(CHANNEL_DATA_S_SHORT_NAME)) == 0 || memcmp(
				argv[*ParmCount], CHANNEL_DATA_S_LONG_NAME,
				strlen(CHANNEL_DATA_S_LONG_NAME)) == 0) {
			if (!isClient) {
				inValidParm = 1;
				break;
			}
			/* test to see if there is a blank after the flag */
			if (strlen(argv[*ParmCount]) != strlen(CHANNEL_DATA_S_SHORT_NAME)
					&& strlen(argv[*ParmCount])
							!= strlen(CHANNEL_DATA_S_LONG_NAME)) {
				if (memcmp(argv[*ParmCount], CHANNEL_DATA_S_LONG_NAME,
						strlen(CHANNEL_DATA_S_LONG_NAME)) == 0) {
					strncpy(MyParms->ClientConn.SendUserData, argv[*ParmCount]
							+strlen(CHANNEL_DATA_S_LONG_NAME),
							sizeof(MyParms->ClientConn.SendUserData));
					*ParmCount += 1;
				} else {
					strncpy(MyParms->ClientConn.SendUserData, argv[*ParmCount]
							+3, sizeof(MyParms->ClientConn.SendUserData));
					*ParmCount += 1;
				}
				if (MyParms->diagnostics>20) {
					fprintf(stderr,"(args.c) Processing %s flag, setting Channel Send User Data to %s\n", CHANNEL_DATA_S_SHORT_NAME, MyParms->ClientConn.SendUserData);
				}
			} else {
				if (*ParmCount < argc-1) {
					strncpy(MyParms->ClientConn.SendUserData,
							argv[*ParmCount+1],
							sizeof(MyParms->ClientConn.SendUserData));
					*ParmCount += 2;
					if (MyParms->diagnostics>20) {
						fprintf(stderr,"(args.c) Processing %s flag, setting Channel Send User Data to %s\n",
						CHANNEL_DATA_S_SHORT_NAME, MyParms->ClientConn.SendUserData);
					}
				} else {
					inValidParm = 1;
				}
			}
			/* was a channel receive data specified ? */
		} else if (memcmp(argv[*ParmCount], CHANNEL_DATA_R_SHORT_NAME,
				strlen(CHANNEL_DATA_R_SHORT_NAME)) == 0 || memcmp(
				argv[*ParmCount], CHANNEL_DATA_R_LONG_NAME,
				strlen(CHANNEL_DATA_R_LONG_NAME)) == 0) {
			if (!isClient) {
				inValidParm = 1;
				break;
			}
			/* test to see if there is a blank after the flag */
			if (strlen(argv[*ParmCount]) != strlen(CHANNEL_DATA_R_SHORT_NAME)
					&& strlen(argv[*ParmCount])
							!= strlen(CHANNEL_DATA_R_LONG_NAME)) {
				if (memcmp(argv[*ParmCount], CHANNEL_DATA_R_LONG_NAME,
						strlen(CHANNEL_DATA_R_LONG_NAME)) == 0) {
					strncpy(MyParms->ClientConn.ReceiveUserData,
							argv[*ParmCount]+strlen(CHANNEL_DATA_R_LONG_NAME),
							sizeof(MyParms->ClientConn.ReceiveUserData));
					*ParmCount += 1;
				} else {
					strncpy(MyParms->ClientConn.ReceiveUserData,
							argv[*ParmCount]+3,
							sizeof(MyParms->ClientConn.ReceiveUserData));
					*ParmCount += 1;
				}
				if (MyParms->diagnostics>20) {
					fprintf(stderr,"(args.c) Processing %s flag, setting Channel Receive User Data to %s\n",
					CHANNEL_DATA_R_SHORT_NAME, MyParms->ClientConn.ReceiveUserData);
				}
			} else {
				if (*ParmCount < argc-1) {
					strncpy(MyParms->ClientConn.ReceiveUserData,
							argv[*ParmCount+1],
							sizeof(MyParms->ClientConn.ReceiveUserData));
					*ParmCount += 2;
					if (MyParms->diagnostics>20) {
						fprintf(stderr,"(args.c) Processing %s flag, setting Channel Receive User Data to %s\n",
						CHANNEL_DATA_R_SHORT_NAME, MyParms->ClientConn.ReceiveUserData);
					}
				} else {
					inValidParm = 1;
				}
			}
		} else if (memcmp(argv[*ParmCount], REPLACE_OUTPUT_SHORT_NAME,
				strlen(REPLACE_OUTPUT_SHORT_NAME)) == 0 || memcmp(
				argv[*ParmCount], REPLACE_OUTPUT_LONG_NAME,
				strlen(REPLACE_OUTPUT_LONG_NAME)) == 0) {
			*ParmCount += 1;
			MyParms->overlay = 1;
			if (MyParms->diagnostics>20) {
				fprintf(stderr,"(args.c) Processing %s flag, setting Replace Output File to true\n",
				REPLACE_OUTPUT_SHORT_NAME);
			}
		} else if (memcmp(argv[*ParmCount], RESET_CHANNELS_SHORT_NAME,
				strlen(RESET_CHANNELS_SHORT_NAME)) == 0 || memcmp(
				argv[*ParmCount], RESET_CHANNELS_LONG_NAME,
				strlen(RESET_CHANNELS_LONG_NAME)) == 0) {
			int i;
			strcpy(MyParms->reset_Switches, "svrqcd");

			/* test to see if there is a blank after the flag */
			if (strlen(argv[*ParmCount]) != strlen(RESET_CHANNELS_SHORT_NAME)
					&& strlen(argv[*ParmCount])
							!= strlen(RESET_CHANNELS_LONG_NAME) ) {
				if (memcmp(argv[*ParmCount], RESET_CHANNELS_LONG_NAME,
						strlen(RESET_CHANNELS_LONG_NAME)) == 0) {
					strncpy(MyParms->reset_Switches, argv[*ParmCount]
							+(strlen(RESET_CHANNELS_LONG_NAME)),
							sizeof(MyParms->reset_Switches));
					if (MyParms->diagnostics>20) {
						fprintf(stderr,"(args.c) Processing %s flag, setting Reset Channels to %s\n",
						RESET_CHANNELS_SHORT_NAME, MyParms->reset_Switches);
					}
					*ParmCount += 1;
				} else {
					strncpy(MyParms->reset_Switches, argv[*ParmCount]+2,
							sizeof(MyParms->reset_Switches));
					if (MyParms->diagnostics>20) {
						fprintf(stderr,"(args.c) Processing %s flag, setting Reset Channels to %s\n",
						RESET_CHANNELS_SHORT_NAME, MyParms->reset_Switches);
					}
					*ParmCount += 1;
				}
			} else if (*ParmCount+1 < argc && argv[*ParmCount+1][0] != '-') { /* is the next token not a switch ? */
				/* yes.. then the next token is a selection string */
				strncpy(MyParms->reset_Switches, argv[*ParmCount+1],
						sizeof(MyParms->reset_Switches));
				if (MyParms->diagnostics>20) {
					fprintf(stderr,"(args.c) Processing %s flag, setting Reset Channels to %s\n",
					RESET_CHANNELS_SHORT_NAME, MyParms->reset_Switches);
				}
				*ParmCount += 2;
			} /* End if next char is not a switch */else {
				if (MyParms->diagnostics>20) {
					fprintf(stderr,"(args.c) Processing %s flag, setting Reset Channels to %s\n",
					RESET_CHANNELS_SHORT_NAME, MyParms->reset_Switches);
				}
				*ParmCount += 1;
			}

			for (i = 0; i<strlen(MyParms->reset_Switches); i++) {
				switch (MyParms->reset_Switches[i]) {
				case 's':
					MyParms->chlReset |= CHLR_SENDER;
					break;
				case 'v':
					MyParms->chlReset |= CHLR_SERVER;
					break;
				case 'r':
					MyParms->chlReset |= CHLR_RECEIVER;
					break;
				case 'q':
					MyParms->chlReset |= CHLR_REQUESTER;
					break;
				case 'c':
					MyParms->chlReset |= CHLR_CLUSSDR;
					break;
				case 'd':
					MyParms->chlReset |= CHLR_CLUSRCVR;
					break;

				default:
					fprintf(stderr, "Invalid invocation: -R switch: %c\n",
					MyParms->reset_Switches[i]);
					if (isClient) {
						fprintf(stderr, USAGE, MyParms->myName, USAGE2);
					} else {
						fprintf(stderr, USAGE, MyParms->myName, USAGE3);
					}
					fprintf(stderr, " \n\n %s\n\n", THISVERSION);
					exit( ERROR);
				}

			}
			if (MyParms->diagnostics>20) {
				fprintf(stderr,"(args.c) Processed %s flag, setting Channel Reset Flags to to %d\n",
				RESET_CHANNELS_SHORT_NAME, MyParms->chlReset);
			}

		} else if (memcmp(argv[*ParmCount], QUIET_SHORT_NAME,
				strlen(QUIET_SHORT_NAME)) == 0 || memcmp(argv[*ParmCount], 
		QUIET_LONG_NAME, strlen(QUIET_LONG_NAME)) == 0) {
			MyParms->noisy = 0; /* user does NOT want warning msgs   */
			if (MyParms->diagnostics>20) {
				fprintf(stderr,"(args.c) Processing %s flag, setting noisy to false\n",
				QUIET_SHORT_NAME);
			}
			*ParmCount += 1;
		} else if (memcmp(argv[*ParmCount], HELP_SHORT_NAME,
				strlen(HELP_SHORT_NAME)) == 0 || memcmp(argv[*ParmCount], 
		HELP_SHORT_NAME_ALT1, strlen(HELP_SHORT_NAME_ALT1)) == 0 || memcmp(
				argv[*ParmCount], HELP_SHORT_NAME_ALT2,
				strlen(HELP_SHORT_NAME_ALT2)) == 0 || memcmp(argv[*ParmCount], 
		HELP_LONG_NAME, strlen(HELP_LONG_NAME)) == 0) {
			if (isClient) {
				fprintf(stderr, USAGE, MyParms->myName, USAGE2);
			} else {
				fprintf(stderr, USAGE, MyParms->myName, USAGE3);
			}
			fprintf(stderr, " \n\n %s\n\n", THISVERSION);
			exit( ERROR);
		} else if (memcmp(argv[*ParmCount], PERMDYN_SHORT_NAME,
				strlen(PERMDYN_SHORT_NAME)) == 0 || memcmp(argv[*ParmCount], 
		PERMDYN_LONG_NAME, strlen(PERMDYN_LONG_NAME)) == 0) {
			MyParms->permDyn = 1; /* v530 user wants qlocals created from permdyn */
			if (MyParms->diagnostics>20) {
				fprintf(stderr,"(args.c) Processing %s flag, setting Create Permanent Dyanmic queues to true\n",
				PERMDYN_SHORT_NAME);
			}
			*ParmCount += 1;
		} else if (memcmp(argv[*ParmCount], IGNORE_ERRORS_SHORT_NAME,
				strlen(IGNORE_ERRORS_SHORT_NAME)) == 0 || memcmp(
				argv[*ParmCount], IGNORE_ERRORS_LONG_NAME,
				strlen(IGNORE_ERRORS_LONG_NAME)) == 0) {
			MyParms->ignoreDamaged = 1; /* v530 user wants to ignore damaged objects */
			if (MyParms->diagnostics>20) {
				fprintf(stderr,"(args.c) Processing %s flag, setting Ignore Damaged queues to true\n",
				IGNORE_ERRORS_SHORT_NAME);
			}
			*ParmCount += 1;
		} else if (memcmp(argv[*ParmCount], ONE_LINE_SHORT_NAME,
				strlen(ONE_LINE_SHORT_NAME)) == 0 || memcmp(argv[*ParmCount], 
		ONE_LINE_LONG_NAME, strlen(ONE_LINE_LONG_NAME)) == 0) {
			MyParms->oneLine = 1; /* v601 user wants output on one line */
			oneLine = 1;
			strcpy(MyParms->lineTerm, "  ");
			if (MyParms->diagnostics>20) {
				fprintf(stderr,"(args.c) Processing %s flag, setting One Line to true\n",
				ONE_LINE_SHORT_NAME);
			}
			*ParmCount += 1;
		} else if (memcmp(argv[*ParmCount], TWO_LINES_SHORT_NAME,
				strlen(TWO_LINES_SHORT_NAME)) == 0 || memcmp(argv[*ParmCount], 
		TWO_LINES_LONG_NAME, strlen(TWO_LINES_LONG_NAME)) == 0) {
			MyParms->oneLine = 1; /* v601 user wants output on two lines */
			MyParms->twoLines = 1;
			strcpy(MyParms->lineTerm, "  ");
			if (MyParms->diagnostics>20) {
				fprintf(stderr,"(args.c) Processing %s flag, setting Two Lines to true\n",
				TWO_LINES_SHORT_NAME);
			}
			*ParmCount += 1;
			/* sorry, this isn't documented as I don't expect it to be needed */
		} else if (memcmp(argv[*ParmCount], 
		OUTPUT_STYLE_MQSC_LONG_NAME, strlen(OUTPUT_STYLE_MQSC_LONG_NAME)) == 0) {
			/*output style is not used in any current or future release of saveqmgr. */
			MyParms->outputStyle = OUTPUT_STYLE_MQSC;
			*ParmCount += 1;
		} else if (memcmp(argv[*ParmCount], OUTPUT_STYLE_XML_LONG_NAME,
				strlen(OUTPUT_STYLE_XML_LONG_NAME)) == 0) {
			MyParms->outputStyle = OUTPUT_STYLE_XML;
			*ParmCount += 1;
#if defined (zzzOS)

		} else if (memcmp(argv[*ParmCount], PLATFORM_SHORT_NAME,
				strlen(PLATFORM_SHORT_NAME)) == 0 || memcmp(argv[*ParmCount], 
		PLATFORM_LONG_NAME, strlen(PLATFORM_LONG_NAME)) == 0) {
			platform = MQPL_UNIX; /* v602 zOS qmgr stuff */
			/* test to see if there is a blank after the flag */
			if (strlen(argv[*ParmCount]) != strlen(PLATFORM_SHORT_NAME)
					&& strlen(argv[*ParmCount]) != strlen(PLATFORM_LONG_NAME)) {
				if (memcmp(argv[*ParmCount], PLATFORM_LONG_NAME,
						strlen(PLATFORM_LONG_NAME)) == 0) {
					platform
							= atoi(argv[*ParmCount]+strlen(PLATFORM_LONG_NAME));
				} else {
					platform = atoi(argv[*ParmCount]+2);
				}
				if (MyParms->diagnostics>20) {
					fprintf(stderr,"(args.c) Processing %s flag, setting platform to %d\n",
					PLATFORM_SHORT_NAME,platform);
				}
			}
			*ParmCount += 1;
			if (*ParmCount < argc && argv[*ParmCount][0] != '-') { /* is the next token not a switch ? */
				/* yes.. then the next token is a diagnostic level */
				platform = atoi(argv[*ParmCount]);
				*ParmCount += 1;
				if (MyParms->diagnostics>20) {
					fprintf(stderr,"(args.c) Processing %s flag, setting platform to %d\n",
					PLATFORM_SHORT_NAME,platform);
				}
			} /* End if next char is not a switch */
#endif
		} else if (memcmp(argv[*ParmCount], SHORT_WAITTIME_SHORT_NAME,
				strlen(SHORT_WAITTIME_SHORT_NAME)) == 0 || memcmp(
				argv[*ParmCount], 
				SHORT_WAITTIME_LONG_NAME, strlen(SHORT_WAITTIME_LONG_NAME))== 0) {
			if (strlen(argv[*ParmCount]) == strlen(SHORT_WAITTIME_SHORT_NAME)
					|| strlen(argv[*ParmCount])
							== strlen(SHORT_WAITTIME_LONG_NAME)) {
				*ParmCount += 1;
				if (*ParmCount < argc && argv[*ParmCount][0] != '-') { /* is the next token not a switch ? */
					MyParms->shortWaitTime = atoi(argv[*ParmCount]);
				} /* End if next char is not a switch */
			} else if (strlen(argv[*ParmCount])
					> strlen(SHORT_WAITTIME_LONG_NAME)) {
				MyParms->shortWaitTime = atoi(argv[*ParmCount]
						+strlen(SHORT_WAITTIME_LONG_NAME));
			} else {
				MyParms->shortWaitTime = atoi(argv[*ParmCount]
						+strlen(SHORT_WAITTIME_SHORT_NAME));
			}
			if (MyParms->diagnostics>20) {
				fprintf(stderr,"(args.c) Processing %s flag, setting Short Wait time to %d\n",
				SHORT_WAITTIME_SHORT_NAME,MyParms->shortWaitTime);
			}
			*ParmCount += 1;

		} else if (memcmp(argv[*ParmCount], WAITTIME_SHORT_NAME,
				strlen(WAITTIME_SHORT_NAME)) == 0 || memcmp(argv[*ParmCount], 
		WAITTIME_LONG_NAME, strlen(WAITTIME_LONG_NAME))== 0) {
			if (strlen(argv[*ParmCount]) == strlen(WAITTIME_SHORT_NAME)
					|| strlen(argv[*ParmCount]) == strlen(WAITTIME_LONG_NAME)) {
				*ParmCount += 1;
				if (*ParmCount < argc && argv[*ParmCount][0] != '-') { /* is the next token not a switch ? */
					/* yes.. then the next token is a diagnostic level */
					MyParms->longWaitTime = atoi(argv[*ParmCount]) *1000;
				} /* End if next char is not a switch */
			} else if (strlen(argv[*ParmCount])> strlen(WAITTIME_LONG_NAME)) {
				MyParms->longWaitTime = atoi(argv[*ParmCount]
						+strlen(WAITTIME_LONG_NAME)) *1000;
			} else {
				MyParms->longWaitTime = atoi(argv[*ParmCount]
						+strlen(WAITTIME_SHORT_NAME)) *1000;
			}
			if (MyParms->diagnostics>20) {
				fprintf(stderr,"(args.c) Processing %s flag, setting Long Wait time to %d\n",
				SHORT_WAITTIME_SHORT_NAME,MyParms->longWaitTime);
			}
			*ParmCount += 1;

		} else { /* invalid parameter */
			inValidParm=1;
		} /* endif */
	}
	/* endwhile */
	return inValidParm;
}

int ProcessISeriesArgs(int argc, char *argv[], CmdArgParmsPtr MyParms,
		int *ParmCount) {
	int inValidParm = 0;
	char work[12]; /* small work area (for platform name) */

	InitArgs(MyParms);

	/* check to see enough args */

	if (argc < OS400PARMCNT) {
		fprintf(stderr, "(saveqmgr.c) Incorrect number of parameters.\n");
		fprintf(stderr, "(saveqmgr.c) Should be %d, received %d\n\n",
		OS400PARMCNT, argc);
		inValidParm=1;
		exit(ERROR);
	}
	memset(MyParms->reset_Switches, '\0', sizeof(MyParms->reset_Switches));
	memset(MyParms->fileName, '\0', sizeof(MyParms->fileName));
	memset(MyParms->authFileName, '\0', sizeof(MyParms->authFileName));

	strncpy(work, argv[1], 3); /* big problem if arg2 begins with # */
	fprintf(stderr, "Input Parms: VERSION: \"%.4s\" \n", work);
	MyParms->UserVersion = atoi(work);	
	strcpy(MyParms->StrVersion, work); /* v531a ... check user version */

	fprintf(stderr, "\t LCLQMGRNAM: \"%.48s\" \n", argv[2]);
	strncpy(MyParms->LclQMgrName, argv[2], MQ_Q_MGR_NAME_LENGTH);

	fprintf(stderr, "\t RMTQMGRNAM: \"%.48s\" \n", argv[3]);
	strncpy(MyParms->RmtQMgrName, argv[3], MQ_Q_MGR_NAME_LENGTH);
	
	strncpy(MyParms->fileName, argv[4], FILENAMESIZE);
	fprintf(stderr, "\t FILENAME: \"%.33s\" \n", MyParms->fileName);

	fprintf(stderr, "\t OUTPUT: \"%.4s\" \n", argv[5]);
	if (!memcmp(argv[5], "*REPLACE", 4))
		MyParms->overlay = 1;

	fprintf(stderr, "\t RDEFSYS: \"%.4s\" \n", argv[6]);
	if (!memcmp(argv[6], "*NO", 3))
		MyParms->SysObjs = 0;

	fprintf(stderr, "\t MAKEAUTH: \"%.4s\" \n", argv[7]);
	if (!memcmp(argv[7], "*YES", 4))
		MyParms->makeSecurityDefs = 1;

	strncpy(MyParms->authFileName, argv[8], FILENAMESIZE);
	fprintf(stderr, "\t AUTHFN: \"%.33s\" \n", MyParms->authFileName);

	fprintf(stderr, "\t PERMDYN: \"%.4s\" \n", argv[9]);
	if (!memcmp(argv[9], "*YES", 4))
		MyParms->permDyn = 1;

	fprintf(stderr, "\t IGNDAMAGED: \"%.4s\" \n", argv[10]);
	if (!memcmp(argv[10], "*YES", 4))
		MyParms->ignoreDamaged = 1;

	if (argc > 11 ) {
		strncpy(work, argv[11], 3); /* big problem if arg2 begins with # */
		fprintf(stderr, "Input Parms: DIAGNOSTICS: \"%.4s\" \n", work);
		MyParms->diagnostics = atoi(work);	
	}
	
	return inValidParm;
}

void InitArgs(CmdArgParmsPtr MyParms) {
	/* Process input parameter list                                           */

	MQCNO Connect_options= { MQCNO_DEFAULT};/* MQCONNX options                 */
	MQCD ClientConn= {MQCD_CLIENT_CONN_DEFAULT}; /* Client connection channel       */

	MQSCO SSL_options= {MQSCO_DEFAULT}; /* Client SSL options */
	/* Set queue manager names to defaults */
	memset( MyParms->LclQMgrName, '\0', sizeof( MyParms->LclQMgrName ) );
	memset( MyParms->RmtQMgrName, '\0', sizeof( MyParms->RmtQMgrName ) );
	memset( MyParms->userID, '\0', sizeof( MyParms->userID) );
	strcpy(MyParms->CmdSvrQName, DEFAULTCMDSVRQ );

	memset( MyParms->argCONNAME, '\0', sizeof( MyParms->argCONNAME) );
	memset( MyParms->argSVRCONN, '\0', sizeof( MyParms->argSVRCONN) );
	memset( MyParms->fileName, '\0', sizeof( MyParms->fileName) );

	strncpy(MyParms->fileName, DEFAULTOUTPUTFILE,sizeof(DEFAULTOUTPUTFILE) ); /* V3.2 assume old output name */
	strcpy(MyParms->argCONNAME,"localhost"); /* Client Conn IP Address or name  */
	strcpy(MyParms->argSVRCONN,"SYSTEM.DEF.SVRCONN"); /* Client Conn SVRCONN name        */
	strcpy(MyParms->lineTerm, "+\n");
	MyParms->flagCONNAME = 0; /* Did user give us a CONNAME?  */
	MyParms->flagSSL=0; /* should we use ssl ? */
	MyParms->oneLine = 0;
	MyParms->twoLines = 0;
	MyParms->SysObjs = SO_ALL;
	MyParms->makeSecurityDefs = 0;
	MyParms->diagnostics = 0;
	MyParms->noisy = 1;
	MyParms->chlReset = 0;
	MyParms->header = 1;
	MyParms->authHeader = 1;
	MyParms->permDyn = 0;
	MyParms->ignoreDamaged=0;
	MyParms->noMsgOnTimeout=0;
	MyParms->platform = 0;
	MyParms->overlay=0;
	MyParms->qsgQmgr=0;
	MyParms->UserVersion=0;
	MyParms->outputStyle = OUTPUT_STYLE_MQSC;
	MyParms->Connect_options = Connect_options;
	MyParms->ClientConn = ClientConn;
	MyParms->std_out = 0;
	MyParms->SSL_options = SSL_options;
	MyParms->shortWaitTime = WAIT_INT_SHORT;
	MyParms->longWaitTime = WAIT_INT;
}

int toUpperStr(char* myStr) {
	unsigned int i;
	for (i=0; i<strlen(myStr); i++)
		myStr[i]=toupper(myStr[i]);
	return i;
}
