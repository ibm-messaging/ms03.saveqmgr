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
/* Module Name: PROCESS.C                                                    */
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

/* Include Application headers */
#include "saveqmgr.h"
#include "mqutils.h"

/* ------------------------------------------------------------------------ */
/* ProcessProcStringParm()                                                  */
/* ProcessProcIntegerParm()                                                 */
/*                                                                          */
/* These two processes are given a pointer to a block of storage that       */
/* contains the definition of a single attribute relating to an individual  */
/* process. The first handles string parameters, the second integer.        */
/* The calling process has already decided which type of parameter this is  */
/* and is responsible for managing the storage. These procedures decide     */
/* which of the possible attributes this one is and extract the value.      */
/*                                                                          */
/* Follows is a list of all attributes possible for a process definition.   */
/*                                                                          */
/* The ProcDefn structure contains entries for each of the attributes.      */
/* ------------------------------------------------------------------------ */
extern int noisy;
extern int header;
extern int oneLine; /* v601 -- whether to write the entire DEFINE on oneline */
extern char lineTerm[3]; /* v601 -- line terminator */
extern int platform;

void ProcessProcStringParm(MQCFST *pPCFString, ProcParms *ProcDefn, MQLONG *iCC) {
	/* Initialise return codes */
	*iCC = MQCC_OK;

	switch (pPCFString->Parameter) {
	case MQCA_PROCESS_NAME:
		memset(ProcDefn->ProcessName, ' ', MQ_PROCESS_NAME_LENGTH);
		memcpy(ProcDefn->ProcessName, pPCFString->String,
				pPCFString->StringLength);
		break;
	case MQCA_PROCESS_DESC:
		memset(ProcDefn->ProcessDesc, ' ', MQ_PROCESS_DESC_LENGTH);
		memcpy(ProcDefn->ProcessDesc, pPCFString->String,
				pPCFString->StringLength);
		break;
	case MQCA_APPL_ID:
		memset(ProcDefn->ApplID, ' ', MQ_PROCESS_APPL_ID_LENGTH);
		memcpy(ProcDefn->ApplID, pPCFString->String, pPCFString->StringLength);
		break;
	case MQCA_ENV_DATA:
		memset(ProcDefn->EnvData, ' ', MQ_PROCESS_ENV_DATA_LENGTH);
		memcpy(ProcDefn->EnvData, pPCFString->String, pPCFString->StringLength);
		break;
	case MQCA_USER_DATA:
		memset(ProcDefn->UserData, ' ', MQ_PROCESS_USER_DATA_LENGTH);
		memcpy(ProcDefn->UserData, pPCFString->String, pPCFString->StringLength);
		break;

		/* [v51] begin v51 attributes */
#if defined (MQCA_ALTERATION_DATE)
	case MQCA_ALTERATION_DATE:
		memset(ProcDefn->AlterationDate, ' ', MQ_CREATION_DATE_LENGTH);
		memcpy(ProcDefn->AlterationDate, pPCFString->String,
				pPCFString->StringLength);
		break;
	case MQCA_ALTERATION_TIME:
		memset(ProcDefn->AlterationTime, ' ', MQ_CREATION_TIME_LENGTH);
		memcpy(ProcDefn->AlterationTime, pPCFString->String,
				pPCFString->StringLength);
		break;
#endif
		/* [v51] end v51 attributes */

#if defined(zzzOS)
		/* v602 zOS attributes */
	case MQBACF_RESPONSE_ID: /* zOS things to ignore */
		break;
	case MQCACF_RESPONSE_Q_MGR_NAME: /* zOS things to ignore */
		break;
#endif

	default:
		if (noisy)
			fprintf(stderr, "(process.c) Unknown PCF String Parameter: %ld\n",
			pPCFString->Parameter);
		*iCC = WARNING;
		break;
	} /* endswitch */

	return;
}

void ProcessProcIntegerParm(MQCFIN *pPCFInteger, ProcParms *ProcDefn,
		MQLONG *iCC) {
	/* Initialise return codes */
	*iCC = MQCC_OK;

	switch (pPCFInteger->Parameter) {
	case MQIA_APPL_TYPE:
		ProcDefn->ApplType = pPCFInteger->Value;
		break;

#if defined(zzzOS)
		/* v602 zOS attributes */
	case MQIA_QSG_DISP: /* zOS */
		ProcDefn->qsgDisp = pPCFInteger->Value;
		break;
#endif

	default:
		if (noisy)
			fprintf(stderr, "(process.c) Unknown PCF Integer Parameter: %ld\n",
			pPCFInteger->Parameter);
		*iCC = WARNING;
		break;
	} /* endswitch */

	return;
}

/* ------------------------------------------------------------------------ */
/*                                                                          */
/* This process takes the attributes of a single process and adds them      */
/* to the end of a file, SAVEQMGR.TST, which can be found in the current    */
/* directory.                                                               */
/*                                                                          */
/* The file is of a format suitable for subsequent input to RUNMQSC.        */
/*                                                                          */
/* ------------------------------------------------------------------------ */
void AddToFilePROCESS(ProcParms *pProcDefn, int UserVersion, char * fileName, /* v32 Name of the output file  */
FILE * fp, /* v51 */
int twolines, MQLONG *iCC) {
	char * ParmBuffer; /* Temporary buffer to hold for output to file */
	int options = 0;
	char work[256];

	memset(work, '\0', 256);

	if ( !(ParmBuffer = (char *) malloc(PBSIZE) )) {
		fprintf(stderr, "(process.c) Failed to allocate %d bytes \n", PBSIZE);
		exit(2);
	}

	/* Initialise return codes */
	*iCC = MQCC_OK;

	/* v601 setup options for oneLine operation */
	options = MQSPRINTF_X_DOUBLE_QUOTES + MQSPRINTF_X_NO_TRAILING_BLANKS;
	if (!oneLine)
		options += MQSPRINTF_X_LINE_BREAKS;

	/* Append these details to the end of the current output file (v32) */
	/* v51 removes open from here ... fp = fopen( fileName, OPENMODE ); */

	if (twolines)
		fputs("* ", fp);

	MQsprintf_X(ParmBuffer, "DEFINE PROCESS ('%s') ", pProcDefn->ProcessName,
			MQ_PROCESS_NAME_LENGTH, options);
	fputs(ParmBuffer, fp);

#if defined (MQCA_ALTERATION_DATE)
	if (header && (!oneLine||twolines)) {
		MQsprintf_X(ParmBuffer, "* ALTDATE (%s) ", pProcDefn->AlterationDate, 
		MQ_CREATION_DATE_LENGTH, options);
		fputs(ParmBuffer, fp);
		MQsprintf_X(ParmBuffer, "* ALTTIME (%s) ", pProcDefn->AlterationTime, 
		MQ_CREATION_TIME_LENGTH, options);
		fputs(ParmBuffer, fp);
	}
#endif

	MQsprintf_X(ParmBuffer, "       DESCR('%s') ", pProcDefn->ProcessDesc,
			MQ_PROCESS_DESC_LENGTH, options);
	fputs(ParmBuffer, fp);

	switch (pProcDefn->ApplType) {
	case MQAT_OS400:
		sprintf(ParmBuffer, "       APPLTYPE(OS400) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
	case MQAT_OS2:
		sprintf(ParmBuffer, "       APPLTYPE(OS2) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
	case MQAT_DOS:
		sprintf(ParmBuffer, "       APPLTYPE(DOS) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
	case MQAT_WINDOWS:
		sprintf(ParmBuffer, "       APPLTYPE(WINDOWS) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
		/* [WMS] AIX is now UNIX */
		/*      case MQAT_AIX:                                        */
		/*       sprintf( ParmBuffer, "       APPLTYPE(AIX) " ); */
		/*       fputs( ParmBuffer, fp );                           */
		/*       break;                                             */

	case MQAT_CICS:
		sprintf(ParmBuffer, "       APPLTYPE(CICS) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
		/* [WMS] adding "new" application types */
	case MQAT_QMGR:
		sprintf(ParmBuffer, "       APPLTYPE(QMGR) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
	case MQAT_IMS:
		sprintf(ParmBuffer, "       APPLTYPE(IMS) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
	case MQAT_CICS_VSE:
		sprintf(ParmBuffer, "       APPLTYPE(CICSVSE) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
	case MQAT_UNIX:
		sprintf(ParmBuffer, "       APPLTYPE(UNIX) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;

		/*   [OvE] On VMS the APPLTYPE parameter is not accepted:         */
		/*         APPLTYPE(OpenVMS) is filled in by default when omitted */
		/* v513 - [wms] new version now accepts "OpenVMS" as appltype     */

#if defined (MQAT_VMS)
	case MQAT_VMS:
		sprintf(ParmBuffer, "       APPLTYPE(OpenVMS) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
#endif
#if defined (MQAT_VOS)
	case MQAT_VOS:
		sprintf(ParmBuffer, "       APPLTYPE(VOS) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
#endif
#if defined (MQAT_GUARDIAN)
	case MQAT_GUARDIAN:
		sprintf(ParmBuffer, "       APPLTYPE(NSK) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
#endif
#if defined (MQAT_IMS_BRIDGE)
	case MQAT_IMS_BRIDGE:
		sprintf(ParmBuffer, "       APPLTYPE(IMS_BRIDGE) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
#endif
#if defined (MQAT_WINDOWS_NT)
	case MQAT_WINDOWS_NT:
		sprintf(ParmBuffer, "       APPLTYPE(WINDOWSNT) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
#endif
#if defined (MQAT_XCF)
	case MQAT_XCF:
		sprintf(ParmBuffer, "       APPLTYPE(XCF) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
#endif
	default:
		/* User defined application type */
		sprintf(ParmBuffer, "       APPLTYPE(%ld) %s", pProcDefn->ApplType,
				lineTerm);
		fputs(ParmBuffer, fp);
		break;
	} /* endswitch */

	MQsprintf_X(ParmBuffer, "       APPLICID('%s') ", pProcDefn->ApplID,
			MQ_PROCESS_APPL_ID_LENGTH, options);
	fputs(ParmBuffer, fp);

	MQsprintf_X(ParmBuffer, "       USERDATA('%s') ", pProcDefn->UserData,
			MQ_PROCESS_USER_DATA_LENGTH, options);
	fputs(ParmBuffer, fp);

	MQsprintf_X(ParmBuffer, "       ENVRDATA('%s') ", pProcDefn->EnvData,
			MQ_PROCESS_ENV_DATA_LENGTH, options);
	fputs(ParmBuffer, fp);

#if defined(zzzOS)
	if (platform == MQPL_MVS) {
		toStrQSG((char*)&work, pProcDefn->qsgDisp);
		sprintf(ParmBuffer, "       QSGDISP(%s) %s", work, lineTerm);
		fputs(ParmBuffer, fp);

	}
#endif

	fputs("       REPLACE\n", fp);

	if (!oneLine) {
		fputs("\n", fp);
	}

	/* v51 removes close from here ... fclose(fp); */

	free(ParmBuffer);
	return;

}

