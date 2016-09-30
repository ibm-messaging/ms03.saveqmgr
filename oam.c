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
/* Module Name: oam.c                                                        */
/*  DESCRIPTIVE NAME WebSphere MQ Save Queue Manager Object                  */
/*                  Definitions using PCFs (ms03 supportpac)                 */
/*                                                                           */
/*===========================================================================*/

/* Include standard libraries */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Include MQSeries headers */
#include <cmqc.h>
#include <cmqcfc.h>
#include <cmqxc.h>
#include <cmqzc.h>

/* move these includes up for Solaris */
#include <sys/types.h>
#include  <time.h>
/* INCLUDE FOR TIME FUNCTION */
#if (MQAT_DEFAULT != MQAT_OS400)
#include <sys/timeb.h>
#endif

/* Include Application headers */
#include "saveqmgr.h"
#include "mqutils.h"

/* If this isn't a v6 compile, don't bother compiling anything */
#if defined (zzMQV60)

/* ------------------------------------------------------------------------ */
/*                                                                          */
extern int noisy;
extern int authHeader;
extern int oneLine;/* v601 -- whether to write the entire DEFINE on oneline */
extern char lineTerm[3]; /* v601 -- line terminator */
extern int platform;
extern pqList qListAnchor;
extern int permDyn;

char* fBuffer;
/* will use for sprintf/etc to buffer entire setmqaut cmd, and only write if good */
#ifdef zzMQV70
#ifndef MQOT_TOPIC		
#define MQOT_TOPIC 8L
#endif
#endif

int firstOAM = 1;

void fromClassToInstance(MQLONG ObjectType, char *work);
void ProcessOAMStringParm(MQCFST *pPCFString, OAMParms *pOAMDefn, MQLONG *iCC);

void fromClassToInstance(MQLONG ObjectType, char *work) {

	switch (ObjectType) {
	case MQOT_Q:
		strcpy(work, "SYSTEM.DEFAULT.LOCAL.QUEUE");
		break;
	case MQOT_PROCESS:
		strcpy(work, "SYSTEM.DEFAULT.PROCESS");
		break;
	case MQOT_NAMELIST:
		strcpy(work, "SYSTEM.DEFAULT.NAMELIST");
		break;
	case MQOT_AUTH_INFO:
		strcpy(work, "SYSTEM.DEFAULT.AUTHINFO.CRLLDAP");
		break;
	case MQOT_SERVICE:
		strcpy(work, "SYSTEM.DEFAULT.SERVICE");
		break;
	case MQOT_LISTENER:
		strcpy(work, "SYSTEM.DEFAULT.LISTENER.TCP");
		break;
	case MQOT_CHANNEL:
		strcpy(work, "SYSTEM.DEF.SENDER");
		break;
	case MQOT_CLNTCONN_CHANNEL:
		strcpy(work, "SYSTEM.DEF.CLNTCONN");
		break;
#ifdef zzMQV70
	case MQOT_TOPIC:
		strcpy(work, "SYSTEM.DEF.TOPIC");
		break;
#endif
	default:
		strcpy(work, "SYSTEM.DEFAULT.??????");
	}

}

void ProcessOAMStringParm(MQCFST *pPCFString, OAMParms *pOAMDefn, MQLONG *iCC) {
	/* Initialise return codes */
	*iCC = MQCC_OK;

	switch (pPCFString->Parameter) {

	case MQCA_Q_MGR_NAME:
		memset(pOAMDefn->QMgrName, ' ', MQ_Q_MGR_NAME_LENGTH);
		memcpy(pOAMDefn->QMgrName, pPCFString->String, pPCFString->StringLength);
		break;

	case MQCACF_AUTH_PROFILE_NAME:
		memset(pOAMDefn->AuthProfile, ' ', MQ_AUTH_PROFILE_NAME_LENGTH);
		memcpy(pOAMDefn->AuthProfile, pPCFString->String,
				pPCFString->StringLength);
		break;

	case MQCACF_ENTITY_NAME:
		memset(pOAMDefn->AuthEntity, ' ', MQ_ENTITY_NAME_LENGTH);
		memcpy(pOAMDefn->AuthEntity, pPCFString->String,
				pPCFString->StringLength);
		break;

	default:
		if (noisy)
			fprintf(stderr, "(oam.c) Unknown PCF String Parameter: %ld\n",
			pPCFString->Parameter);
		*iCC = WARNING;
		break;
	} /* endswitch */

	return;
}

/*************************************************************/
void ProcessOAMIntegerParm(MQCFIN *pPCFInteger, OAMParms *pOAMDefn, MQLONG *iCC) {
	/* Initialise return codes */
	*iCC = MQCC_OK;

	switch (pPCFInteger->Parameter) {

	case MQIACF_ENTITY_TYPE:
		pOAMDefn->EntityType = pPCFInteger->Value;
		break;

	case MQIACF_OBJECT_TYPE:
		pOAMDefn->ObjectType = pPCFInteger->Value;
		break;

	case MQIACF_AUTH_OPTIONS:
		pOAMDefn->AuthOptions = pPCFInteger->Value;
		break;

	default:
		if (noisy)
			fprintf(stderr,
			"(oam.c) Unknown PCF Integer Parameter: %ld\n",
			pPCFInteger->Parameter);
		*iCC = WARNING;
		break;
	} /* endswitch */

	return;
}

void ProcessOAMIntegerListParm(MQCFIL *pPCFIntegerList, OAMParms *pOAMDefn,
		MQLONG *iCC) {
	int i = 0;
	*iCC = MQCC_OK;

	switch (pPCFIntegerList->Parameter) {

	case MQIACF_AUTHORIZATION_LIST:
		for (i=0; i<ZZAUTHLISTSIZE; i++) {
			pOAMDefn->AuthList[i] = -1;
		}

		for (i=0; i<pPCFIntegerList->Count && i<ZZAUTHLISTSIZE; i++) {
			pOAMDefn->AuthList[i] = pPCFIntegerList->Values[i];
		}
		break;

	default:
		if (noisy)
			fprintf(stderr,
			"(oam.c) Unknown PCF Integer List Parameter: %ld\n",
			pPCFIntegerList->Parameter);
		*iCC = WARNING;
		break;
	} /* endswitch */

	return;
}

/* ------------------------------------------------------------------------ */
/*                                                                          */
/* This process takes the attributes of a single log and adds them     */
/* to the end of a file, SAVEQMGR.TST, which can be found in the current    */
/* directory.                                                               */
/*                                                                          */
/* The file is of a format suitable for subsequent input to RUNMQSC.        */
/*                                                                          */
/* ------------------------------------------------------------------------ */
void AddToFileOAM(OAMParms *pOAMDefn, int UserVersion, char * fileName, /* v32 Name of the output file  */
FILE * fp, /* v51 */
int twolines, /* v601 */
MQLONG *iCC) {
	char * ParmBuffer; /* Temporary buffer to hold for output to file */
	int options; /* options for MQsprintf_X */
	time_t ltime;
	struct tm *today;

	int i=0;
	char work[256];
	int writeDef=1;
	pqList qPtr;

	/* Initialise return codes */

	*iCC = MQCC_OK;

	/* This is sort of a hack, if the pointer is null, that means */
	/* we're done, and for the /400 platform we need to write a   */
	/* "endpgm" command */

	if (!pOAMDefn) {

		if (platform==MQPL_OS400) {
			fputs("ENDPGM\n", fp);

		}

		return;

	}

	if ( !(ParmBuffer = (char *) malloc(PBSIZE) )) {
		fprintf(stderr, "(oam.c) Failed to allocate %d bytes \n", PBSIZE);
		exit(2);
	}

	options = MQSPRINTF_X_NO_TRAILING_BLANKS + MQSPRINTF_X_NULL_IF_BLANK;

	/* if a queue, find the queue name in the linked list */
	/* and decide whether or not */
	/* to write it.  If we don't find the name in the list, */
	/* then it could be a profile  */
	/* with a ** or % in it */
	/* if we do find the name, then we ignore temp dyn */
	/* and perm dyn unless -p was specified */

	/* JRL 01/05/2010 Reverted to 6.0.3 release behavior */
	if (pOAMDefn->ObjectType==MQOT_Q) {
		for (qPtr = qListAnchor; qPtr!= 0; qPtr = (pqList)qPtr->next) {
			/* do the names match ? */
			if (!memcmp(qPtr->qName, pOAMDefn->AuthProfile, sizeof(qPtr->qName))) {
				if (qPtr->qType == MQQT_LOCAL && (qPtr->qDefType
						== MQQDT_TEMPORARY_DYNAMIC || (qPtr->qDefType
						== MQQDT_PERMANENT_DYNAMIC && !permDyn)))
					writeDef=0;
				break;
			}
		}
	}

	if (writeDef) {

		if (platform != MQPL_OS400) {

			/******************************************************************/
			/************ making unix & windows  definitions ******************/
			/******************************************************************/

			if (authHeader && firstOAM) {
				firstOAM = 0;
				time( &ltime);
				today = localtime( &ltime);

				if (platform==MQPL_WINDOWS_NT) {
					strcpy(
							work,
							"rem \
\nrem This file generated by %s on %.4d-%.2d-%.2d at %.2d.%.2d.%.2d hours.\
\nrem\n");
				} else {
					strcpy(
							work,
							"#\n# This file generated by %s on %.4d-%.2d-%.2d at %.2d.%.2d.%.2d hours.\
\n#\n");
				}

				fprintf(fp, work, 
				THISVERSION, (today->tm_year) + 1900, today -> tm_mon+1,
						today -> tm_mday, today -> tm_hour, today -> tm_min,
						today -> tm_sec);
			}

			MQsprintf_X(ParmBuffer, "setmqaut -m %s ", pOAMDefn->QMgrName, 
			MQ_Q_MGR_NAME_LENGTH, options);
			fputs(ParmBuffer, fp);

			/* The command server returns @class (uppercase in windows) */
			/* for the +crt authority as a profile name.                */
			/* Its safer to convert that to the default object name for */
			/* that object type and use that when giving the permission */

			/* for qmgrs objects, we never give the name */
			if (pOAMDefn->ObjectType != MQOT_Q_MGR) {

				if (!(memcmp(&pOAMDefn->AuthProfile[0], "@class ", 7)
						&& memcmp(&pOAMDefn->AuthProfile[0], "@CLASS ", 7))) {

					/* get the default object name for this object Type */
					fromClassToInstance(pOAMDefn->ObjectType, (char*)&work[0]);
					if (platform!=MQPL_UNIX) {
						MQsprintf_X(ParmBuffer, "-n %s ", work, 
						MQ_AUTH_PROFILE_NAME_LENGTH, options);
						/* quote the profile name in unix */
					} else {
						MQsprintf_X(ParmBuffer, "-n '%s' ", work, 
						MQ_AUTH_PROFILE_NAME_LENGTH, options);
					}
					/* for windows, no quotes are allowed */
				} else if (platform!=MQPL_UNIX) {
					MQsprintf_X(ParmBuffer, "-n %s ", pOAMDefn->AuthProfile, 
					MQ_AUTH_PROFILE_NAME_LENGTH, options);
					/* quote the profile name in unix */
				} else {
					MQsprintf_X(ParmBuffer, "-n '%s' ", pOAMDefn->AuthProfile, 
					MQ_AUTH_PROFILE_NAME_LENGTH, options);
				}
				fputs(ParmBuffer, fp);
			}

			switch (pOAMDefn->ObjectType) {
			case MQOT_AUTH_INFO:
				fputs("-t authinfo ", fp);
				break;
			case MQOT_CHANNEL:
				fputs("-t channel ", fp);
				break;
			case MQOT_CLNTCONN_CHANNEL:
				fputs("-t clntconn ", fp);
				break;
			case MQOT_LISTENER:
				fputs("-t listener ", fp);
				break;
			case MQOT_NAMELIST:
				fputs("-t namelist ", fp);
				break;
			case MQOT_PROCESS:
				fputs("-t process ", fp);
				break;
			case MQOT_Q:
				fputs("-t queue ", fp);
				break;
			case MQOT_Q_MGR:
				fputs("-t qmgr ", fp);
				break;
			case MQOT_SERVICE:
				fputs("-t service ", fp);
				break;
#ifdef zzMQV70
			case MQOT_TOPIC:
				fputs("-t topic ", fp);
				break;
#endif
			default:
				sprintf(work, "-t ?%ld? ", pOAMDefn->ObjectType);
				fputs(work, fp);
				fprintf(stderr,
				"(oam.c) Unknown ObjectType %ld\n", pOAMDefn->ObjectType);
			}

			/* for windows, group@domain -> group */

			if (platform==MQPL_WINDOWS_NT && pOAMDefn->EntityType
					== MQZAET_GROUP && strstr((char*)&(pOAMDefn->AuthEntity),
					"@")) {
				memset(work, '\0', sizeof(work));
				/* move the group bit (before the @) */
				memcpy(work, (char*)&(pOAMDefn->AuthEntity), (strstr(
						(char*)&(pOAMDefn->AuthEntity), "@")
						- (char*)&pOAMDefn->AuthEntity));
				/* now reassemble */
				strcpy((char*)&(pOAMDefn->AuthEntity), work);
			}

			switch (pOAMDefn->EntityType) {
			case MQZAET_PRINCIPAL:
				MQsprintf_X(ParmBuffer, "-p %s ", pOAMDefn->AuthEntity, 
				MQ_ENTITY_NAME_LENGTH, options);
				break;
			case MQZAET_GROUP:
				MQsprintf_X(ParmBuffer, "-g %s ", pOAMDefn->AuthEntity, 
				MQ_ENTITY_NAME_LENGTH, options);
				break;
			case MQZAET_UNKNOWN:
			default:
				sprintf(ParmBuffer,
						" (unknown entity type(%ld) for entity %.64s) ",
						pOAMDefn->EntityType, pOAMDefn->AuthEntity);
				fprintf(stderr,
				"(oam.c) Unknown EntityType %ld for entity %.64s\n",
				pOAMDefn->EntityType, pOAMDefn->AuthEntity);
			}

			fputs(ParmBuffer, fp);

			for (i=0; i<ZZAUTHLISTSIZE; i++) {
				if (pOAMDefn->AuthList[i]==-1)
					break;

				switch (pOAMDefn->AuthList[i]) {

				case MQAUTH_NONE:
					fputs("+none ", fp);
					break;

				case MQAUTH_ALT_USER_AUTHORITY:
					fputs("+altusr ", fp);
					break;
				case MQAUTH_CONNECT:
					fputs("+connect ", fp);
					break;

				case MQAUTH_BROWSE:
					fputs("+browse ", fp);
					break;
				case MQAUTH_INPUT:
					fputs("+get ", fp);
					break;
				case MQAUTH_INQUIRE:
					fputs("+inq ", fp);
					break;
				case MQAUTH_PASS_ALL_CONTEXT:
					fputs("+passall ", fp);
					break;
				case MQAUTH_PASS_IDENTITY_CONTEXT:
					fputs("+passid ", fp);
					break;
				case MQAUTH_OUTPUT:
					fputs("+put ", fp);
					break;
				case MQAUTH_SET:
					fputs("+set ", fp);
					break;
				case MQAUTH_SET_ALL_CONTEXT:
					fputs("+setall ", fp);
					break;
				case MQAUTH_SET_IDENTITY_CONTEXT:
					fputs("+setid ", fp);
					break;

				case MQAUTH_CHANGE:
					fputs("+chg ", fp);
					break;
				case MQAUTH_CLEAR:
					fputs("+clr ", fp);
					break;
				case MQAUTH_CREATE:
					fputs("+crt ", fp);
					break;
				case MQAUTH_DELETE:
					fputs("+dlt ", fp);
					break;
				case MQAUTH_DISPLAY:
					fputs("+dsp ", fp);
					break;

				case MQAUTH_CONTROL:
					fputs("+ctrl ", fp);
					break;
				case MQAUTH_CONTROL_EXTENDED:
					fputs("+ctrlx ", fp);
					break;
#ifdef zzMQV70
				case MQAUTH_PUBLISH:
					fputs("+pub ", fp);
					break;
				case MQAUTH_SUBSCRIBE:
					fputs("+sub ", fp);
					break;
				case MQAUTH_RESUME:
					fputs("+resume ", fp);
					break;
#endif
#ifdef zzMQV701
				case MQAUTH_SYSTEM:
					fputs("+system ", fp);
					break;
#endif
/*Don't forget to change the iSeries block as well. */
				default:
					sprintf(work, "+?%ld? ", pOAMDefn->AuthList[i]);
					fputs(work, fp);
					fprintf(stderr,
					"(oam.c) Unknown Authorization %ld\n", pOAMDefn->AuthList[i]);
				}
			}

			fputs("\n", fp);

			/******************************************************************/
			/******************* making os400 definitions *********************/
			/******************************************************************/

		} else {

			if (firstOAM) {
				firstOAM = 0;
				fputs("PGM\n", fp);

			}

			/* there are no client channels on the /400 */

			if (MQOT_CLNTCONN_CHANNEL!=pOAMDefn->ObjectType) {

				MQsprintf_X(ParmBuffer, "GRTMQMAUT MQMNAME('%s') ",
						pOAMDefn->QMgrName, 
						MQ_Q_MGR_NAME_LENGTH, options);

				fputs(ParmBuffer, fp);

				if (!oneLine)
					fputs("+ \n     ", fp);

				/* If a qmgr object, then use the qmgr name */

				if (pOAMDefn->ObjectType == MQOT_Q_MGR) {
					MQsprintf_X(ParmBuffer, "OBJ('%s') ", pOAMDefn->QMgrName, 
					MQ_AUTH_PROFILE_NAME_LENGTH, options);

					/* see comment above about changing @class to default objs */

				} else if (!(memcmp(&pOAMDefn->AuthProfile[0], "&class", 6))) {

					fromClassToInstance(pOAMDefn->ObjectType, (char*)&work[0]);
					MQsprintf_X(ParmBuffer, "OBJ('%s') ", work, 
					MQ_AUTH_PROFILE_NAME_LENGTH, options);

				} else {
					MQsprintf_X(ParmBuffer, "OBJ('%s') ",
							pOAMDefn->AuthProfile, 
							MQ_AUTH_PROFILE_NAME_LENGTH, options);

				}
				fputs(ParmBuffer, fp);

				if (!oneLine)
					fputs("+ \n     ", fp);

				switch (pOAMDefn->ObjectType) {
				case MQOT_AUTH_INFO:
					fputs("OBJTYPE(*AUTHINFO) ", fp);
					break;
				case MQOT_CHANNEL:
					fputs("OBJTYPE(*CHL) ", fp);
					break;
				case MQOT_CLNTCONN_CHANNEL:
					fputs("OBJTYPE(*CLTCN) ", fp);
					break;
				case MQOT_LISTENER:
					fputs("OBJTYPE(*LSR) ", fp);
					break;
				case MQOT_NAMELIST:
					fputs("OBJTYPE(*NMLIST) ", fp);
					break;
				case MQOT_PROCESS:
					fputs("OBJTYPE(*PRC) ", fp);
					break;
				case MQOT_Q:
					if (qPtr) {
						switch (qPtr->qType) {
						case MQQT_LOCAL:
							fputs("OBJTYPE(*LCLQ) ", fp);
							break;
						case MQQT_ALIAS:
							fputs("OBJTYPE(*ALSQ) ", fp);
							break;
						case MQQT_MODEL:
							fputs("OBJTYPE(*MDLQ) ", fp);
							break;
						case MQQT_REMOTE:
							fputs("OBJTYPE(*RMTQ) ", fp);
							break;
						default:
							sprintf(work, "OBJTYPE(? ?%ld? ?) ",
									pOAMDefn->ObjectType);
							fputs(work, fp);
						}
						break;
					} else {
						fputs("OBJTYPE(*Q) ", fp);
						break;
					}
				case MQOT_Q_MGR:
					fputs("OBJTYPE(*MQM) ", fp);
					break;
				case MQOT_SERVICE:
					fputs("OBJTYPE(*SVC) ", fp);
					break;
#ifdef zzMQV70
				case MQOT_TOPIC:
					fputs("OBJTYPE(*TOPIC) ", fp);
					break;
#endif
				default:
					sprintf(work, "OBJTYPE(?%ld?) ", pOAMDefn->ObjectType);
					fputs(work, fp);
					fprintf(stderr,
					"(oam.c) Unknown ObjectType %ld\n", pOAMDefn->ObjectType);
				}

				MQsprintf_X(ParmBuffer, "USER(%s) ", pOAMDefn->AuthEntity, 
				MQ_ENTITY_NAME_LENGTH, options);

				fputs(ParmBuffer, fp);

				if (!oneLine)
					fputs("+ \n     ", fp);

				fputs("AUT(", fp);

				for (i=0; i<ZZAUTHLISTSIZE; i++) {

					if (pOAMDefn->AuthList[i]==-1)
						break;

					/* linebreak after 8 objects to keep lines < 80 bytes */

					if (i> 0 && i%8 == 0 && !oneLine)
						fputs("+ \n     ", fp);

					switch (pOAMDefn->AuthList[i]) {

					case MQAUTH_NONE:
						fputs("*NONE ", fp);
						break;

					case MQAUTH_ALT_USER_AUTHORITY:
						fputs("*ALTUSR ", fp);
						break;
					case MQAUTH_CONNECT:
						fputs("*CONNECT ", fp);
						break;

					case MQAUTH_BROWSE:
						fputs("*BROWSE ", fp);
						break;
					case MQAUTH_INPUT:
						fputs("*GET ", fp);
						break;
					case MQAUTH_INQUIRE:
						fputs("*INQ ", fp);
						break;
					case MQAUTH_PASS_ALL_CONTEXT:
						fputs("*PASSALL ", fp);
						break;
					case MQAUTH_PASS_IDENTITY_CONTEXT:
						fputs("*PASSID ", fp);
						break;
					case MQAUTH_OUTPUT:
						fputs("*PUT ", fp);
						break;
					case MQAUTH_SET:
						fputs("*SET ", fp);
						break;
					case MQAUTH_SET_ALL_CONTEXT:
						fputs("*SETALL ", fp);
						break;
					case MQAUTH_SET_IDENTITY_CONTEXT:
						fputs("*SETID ", fp);
						break;

					case MQAUTH_CHANGE:
						fputs("*ADMCHG ", fp);
						break;
					case MQAUTH_CLEAR:
						fputs("*ADMCLR ", fp);
						break;
					case MQAUTH_CREATE:
						fputs("*ADMCRT ", fp);
						break;
					case MQAUTH_DELETE:
						fputs("*ADMDLT ", fp);
						break;
					case MQAUTH_DISPLAY:
						fputs("*ADMDSP ", fp);
						break;

					case MQAUTH_CONTROL:
						fputs("*CTRL ", fp);
						break;
					case MQAUTH_CONTROL_EXTENDED:
						fputs("*CTRLX ", fp);
						break;
#ifdef zzMQV70
					case MQAUTH_PUBLISH:
						fputs("*PUB ", fp);
						break;
					case MQAUTH_SUBSCRIBE:
						fputs("*SUB ", fp);
						break;
					case MQAUTH_RESUME:
						fputs("*RESUME ", fp);
						break;
#endif
#ifdef zzMQV701
					case MQAUTH_SYSTEM:
						fputs("*SYSTEM ", fp);
						break;
#endif
					default:
						sprintf(work, "*?%ld? ", pOAMDefn->AuthList[i]);
						fputs(work, fp);
						fprintf(stderr,
						"(oam.c) Unknown Authorization %ld\n", pOAMDefn->AuthList[i]);
					}
				}

				fputs(")\n", fp);

			} /* end of os400 code */

		} /* end of not clntconn */

	} /* end of writedef */

	free(ParmBuffer);
	return;

}

/* end of don't compile anything */
#endif
