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
/* Module Name: authinfo.c                                                   */
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

/* If this isn't v5.3, don't bother compiling anything */
#if defined (MQ_AUTH_INFO_NAME_LENGTH)

/* ------------------------------------------------------------------------ */
/* ProcessNamelistStringParm()                                              */
/* ProcessNamelistIntegerParm()                                             */
/*                                                                          */
/* These two processes are given a a block of storage that                  */
/* contains the definition of a single attribute relating to an individual  */
/* authinfo. The first handles string parameters, the second integer.       */
/* The calling process has already decided which type of parameter this is  */
/* and is responsible for managing the storage. These procedures decide     */
/* which of the possible attributes this one is and extract the value.      */
/*                                                                          */
/* Follows is a list of all attributes possible for a process definition.   */
/*                                                                          */
/* The AuthInfoDefn structure contains entries for each of the attributes.  */
/* ------------------------------------------------------------------------ */

extern int noisy;
extern int header;
extern int oneLine; /* v601 -- whether to write the entire DEFINE on oneline */
extern char lineTerm[3]; /* v601 -- line terminator */
extern int platform;

void ProcessAuthInfoStringParm(MQCFST *pPCFString,
		AuthInfoParms *pAuthInfoDefn, MQLONG *iCC) {
	/* Initialise return codes */
	*iCC = MQCC_OK;

	switch (pPCFString->Parameter) {
	case MQCA_AUTH_INFO_NAME:
		memset(pAuthInfoDefn->AuthInfoName, ' ', MQ_AUTH_INFO_NAME_LENGTH);
		memcpy(pAuthInfoDefn->AuthInfoName, pPCFString->String,
				pPCFString->StringLength);
		break;
	case MQCA_AUTH_INFO_DESC:
		memset(pAuthInfoDefn->AuthInfoDesc, ' ', MQ_AUTH_INFO_DESC_LENGTH);
		memcpy(pAuthInfoDefn->AuthInfoDesc, pPCFString->String,
				pPCFString->StringLength);
		break;
	case MQCA_ALTERATION_DATE:
		memset(pAuthInfoDefn->AlterationDate, ' ', MQ_CREATION_DATE_LENGTH);
		memcpy(pAuthInfoDefn->AlterationDate, pPCFString->String,
				pPCFString->StringLength);
		break;
	case MQCA_ALTERATION_TIME:
		memset(pAuthInfoDefn->AlterationTime, ' ', MQ_CREATION_TIME_LENGTH);
		memcpy(pAuthInfoDefn->AlterationTime, pPCFString->String,
				pPCFString->StringLength);
		break;
	case MQCA_AUTH_INFO_CONN_NAME:
		memset(pAuthInfoDefn->ConnName, ' ', MQ_CONN_NAME_LENGTH);
		memcpy(pAuthInfoDefn->ConnName, pPCFString->String,
				pPCFString->StringLength);
		break;
	case MQCA_LDAP_PASSWORD:
		memset(pAuthInfoDefn->Password, ' ', MQ_LDAP_PASSWORD_LENGTH);
		memcpy(pAuthInfoDefn->Password, pPCFString->String,
				pPCFString->StringLength);
		break;
	case MQCA_LDAP_USER_NAME:
		memset(pAuthInfoDefn->UserName, ' ', sizeof(pAuthInfoDefn->UserName));
		memcpy(pAuthInfoDefn->UserName, pPCFString->String,
				pPCFString->StringLength);
		break;
#if defined(zzMQV701)
	case MQCA_AUTH_INFO_OCSP_URL:
		memset(pAuthInfoDefn->OcspURL, ' ', sizeof(pAuthInfoDefn->OcspURL));
		memcpy(pAuthInfoDefn->OcspURL, pPCFString->String,
				pPCFString->StringLength);
		break;

#endif

#if defined(zzzOS)
		/* v602 zOS attributes */
	case MQBACF_RESPONSE_ID: /* zOS things to ignore */
		break;
	case MQCACF_RESPONSE_Q_MGR_NAME: /* zOS things to ignore */
		break;
#endif

	default:
		if (noisy)
			fprintf(stderr, "(authinfo.c) Unknown PCF String Parameter: %ld\n",
			pPCFString->Parameter);
		*iCC = WARNING;
		break;
	} /* endswitch */

	return;
}

/*************************************************************/
void ProcessAuthInfoIntegerParm(MQCFIN *pPCFInteger,
		AuthInfoParms *pAuthInfoDefn, MQLONG *iCC) {
	/* Initialise return codes */
	*iCC = MQCC_OK;

	switch (pPCFInteger->Parameter) {
	case MQIA_AUTH_INFO_TYPE:
		pAuthInfoDefn->AuthInfoType = pPCFInteger->Value;
		break;

#if defined(zzzOS)
		/* v602 zOS attributes */
	case MQIA_QSG_DISP: /* zOS */
		pAuthInfoDefn->qsgDisp = pPCFInteger->Value;
		break;
#endif

	default:
		if (noisy)
			fprintf(stderr, "(authinfo.c) Unknown PCF Integer Parameter: %ld\n",
			pPCFInteger->Parameter);
		*iCC = WARNING;
		break;
	} /* endswitch */

	return;
}

/* ------------------------------------------------------------------------ */
/*                                                                          */
/* This process takes the attributes of a single authinfo and adds them     */
/* to the end of a file, SAVEQMGR.TST, which can be found in the current    */
/* directory.                                                               */
/*                                                                          */
/* The file is of a format suitable for subsequent input to RUNMQSC.        */
/*                                                                          */
/* ------------------------------------------------------------------------ */
void AddToFileAUTHINFO(AuthInfoParms *pAuthInfoDefn, int UserVersion,
		char * fileName, /* v32 Name of the output file  */
		FILE * fp, /* v51 */
		int twolines, /* v601 */
		MQLONG *iCC) {
	char * ParmBuffer; /* Temporary buffer to hold for output to file */
	int options; /* options for MQsprintf_X */
	char work[256];

	memset(work, '\0', 256);

	if ( !(ParmBuffer = (char *) malloc(PBSIZE) )) {
		fprintf(stderr, "(authinfo.c) Failed to allocate %d bytes \n", PBSIZE);
		exit(2);
	}

	/* Initialise return codes */
	*iCC = MQCC_OK;

	/* v601 setup MQsprintf_X to break lines based on oneLine setting */
	options = MQSPRINTF_X_DOUBLE_QUOTES + MQSPRINTF_X_NO_TRAILING_BLANKS;
	if (!oneLine)
		options += MQSPRINTF_X_LINE_BREAKS;

	/* Append these details to the end of the current output file (v32) */

	if (twolines)
		fputs("* ", fp);

	MQsprintf_X(ParmBuffer, "DEFINE AUTHINFO('%s') ",
			pAuthInfoDefn->AuthInfoName, MQ_AUTH_INFO_NAME_LENGTH, options);
	fputs(ParmBuffer, fp);

	if (header && (!oneLine || twolines)) {
		MQsprintf_X(ParmBuffer, "* ALTDATE (%s) ",
				pAuthInfoDefn->AlterationDate, 
				MQ_CREATION_DATE_LENGTH, options);
		fputs(ParmBuffer, fp);

		MQsprintf_X(ParmBuffer, "* ALTTIME (%s) ",
				pAuthInfoDefn->AlterationTime, 
				MQ_CREATION_TIME_LENGTH, options);
		fputs(ParmBuffer, fp);
	}

	/* For now, this suffices.  If More AuthTypes are introduced */
	/* We'll have to get more sophisicated                       */

	if (pAuthInfoDefn->AuthInfoType == MQAIT_CRL_LDAP) {
		sprintf(ParmBuffer, "       AUTHTYPE(CRLLDAP) %s", lineTerm);
		fputs(ParmBuffer, fp);

		/* v533, this is a hack, if the conname is blank, then mqsc gripes */
		/* in V6 this should be fixed, so we don't need this (in theory) if we are */
		/* genning v6 code */

		if (pAuthInfoDefn->ConnName[0]==' ' && UserVersion < MQCMDL_LEVEL_600) {
			memcpy(pAuthInfoDefn->ConnName, "none", 4);
			if (!oneLine) {
				sprintf(ParmBuffer,
						"** saveqmgr is setting conname to none to prevent mqsc errors!**\n");
				fputs(ParmBuffer, fp);
			}
		}

		MQsprintf_X(ParmBuffer, "       CONNAME('%s') ",
				(char*) pAuthInfoDefn->ConnName , 
				MQ_AUTH_INFO_CONN_NAME_LENGTH, options);
		fputs(ParmBuffer, fp);

		if (!oneLine || twolines) {
			MQsprintf_X(ParmBuffer, "**     LDAPPWD('%s') ",
					(char*) pAuthInfoDefn->Password , MQ_LDAP_PASSWORD_LENGTH,
					options);
			fputs(ParmBuffer, fp);
		}

		MQsprintf_X(ParmBuffer, "       LDAPUSER('%s') ",
				(char*) pAuthInfoDefn->UserName ,
				sizeof(pAuthInfoDefn->UserName), options);
		fputs(ParmBuffer, fp);
	}
#if defined(zzMQV701)
	else if (pAuthInfoDefn->AuthInfoType == MQAIT_OCSP) {
		sprintf(ParmBuffer, "       AUTHTYPE(OCSP) %s", lineTerm);
		fputs(ParmBuffer, fp);
		MQsprintf_X(ParmBuffer, "       OCSPURL('%s') ",
				(char*) pAuthInfoDefn->OcspURL , 
				MQ_AUTH_INFO_OCSP_URL_LENGTH, options);
		fputs(ParmBuffer, fp);

	}
#endif 
	MQsprintf_X(ParmBuffer, "       DESCR('%s') ", pAuthInfoDefn->AuthInfoDesc, 
	MQ_AUTH_INFO_DESC_LENGTH, options);
	fputs(ParmBuffer, fp);
#if defined(zzzOS)
	if (platform == MQPL_MVS) {
		toStrQSG((char*)&work, pAuthInfoDefn->qsgDisp);
		sprintf(ParmBuffer, "       QSGDISP(%s) %s", work, lineTerm);
		fputs(ParmBuffer, fp);

	}
#endif

	sprintf(ParmBuffer, "        REPLACE\n");
	fputs(ParmBuffer, fp);

	if (!oneLine) {
		fputs("\n", fp);
	}

	free(ParmBuffer);
	return;

}

/* end of don't compile anything */
#endif
