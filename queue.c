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
/* Module Name: queue.c                                                      */
/*  DESCRIPTIVE NAME WebSphere MQ Save Queue Manager Object                  */
/*                  Definitions using PCFs (ms03 supportpac)                 */
/*                                                                           */
/*===========================================================================*/

/* Include standard libraries */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Include MQSeries headers */
#include <cmqc.h>
#include <cmqcfc.h>
#include <cmqxc.h>

/* Include Application headers */
#include "saveqmgr.h"
#include "mqutils.h"
/* ------------------------------------------------------------------------ */
/* ProcessQueueStringParm()                                                 */
/* ProcessQueueIntegerParm()                                                */
/*                                                                          */
/* These two processes are given a pointer to a block of storage that       */
/* contains the definition of a single attribute relating to an individual  */
/* queue. The first handles string parameters, the second integer.          */
/* The calling process has already decided which type of parameter this is  */
/* and is responsible for managing the storage. These procedures decide     */
/* which of the possible attributes this one is and extract the value.      */
/*                                                                          */
/* Follows is a list of all attributes possible for the complete            */
/* set of queue types; namely QLOCAL, QALIAS, QREMOTE and QMODEL.           */
/*                                                                          */
/* The parameters are not all relevant for all queue types.                 */
/*                                                                          */
/* The QDefn structure contains entries for each of the attributes,         */
/* only the attributes relevant for this queue type will be meaningful.     */
/* When we later write the file we will ensure that, based on queue type,   */
/* only the relevant fields are used.                                       */
/* ------------------------------------------------------------------------ */
extern int noisy;
extern int permDyn; /* v530 *//* whether to create qlocal from Permdyn */
extern int foundnpmclass; /* v533a */
extern int header;
extern int oneLine; /* v601 -- whether to write the entire DEFINE on oneline */
extern char lineTerm[3]; /* v601 -- line terminator */
extern int platform; /* v602 -- platform to generate code for */

extern pqList qListAnchor; /* point to first qlist element */
extern pqList qListTerm; /* point to last qlist element */

void ProcessQueueStringParm(MQCFST *pPCFString, QParms *QDefn, MQLONG *iCC) {
	/* Initialise return codes */
	*iCC = MQCC_OK;

	switch (pPCFString->Parameter) {
	case MQCA_Q_NAME:
		memset(QDefn->QName, ' ', MQ_Q_NAME_LENGTH);
		memcpy(QDefn->QName, pPCFString->String, pPCFString->StringLength);
		break;
	case MQCA_Q_DESC:
		memset(QDefn->QDesc, ' ', MQ_Q_DESC_LENGTH);
		memcpy(QDefn->QDesc, pPCFString->String, pPCFString->StringLength);
		break;
	case MQCA_PROCESS_NAME:
		memset(QDefn->ProcessName, ' ', MQ_PROCESS_NAME_LENGTH);
		memcpy(QDefn->ProcessName, pPCFString->String, pPCFString->StringLength);
		break;
	case MQCA_BACKOUT_REQ_Q_NAME:
		memset(QDefn->BackoutReqQName, ' ', MQ_Q_NAME_LENGTH);
		memcpy(QDefn->BackoutReqQName, pPCFString->String,
				pPCFString->StringLength);
		break;
	case MQCA_CREATION_DATE:
		memset(QDefn->CreationDate, ' ', MQ_CREATION_DATE_LENGTH);
		memcpy(QDefn->CreationDate, pPCFString->String,
				pPCFString->StringLength);
		break;
	case MQCA_CREATION_TIME:
		memset(QDefn->CreationTime, ' ', MQ_CREATION_TIME_LENGTH);
		memcpy(QDefn->CreationTime, pPCFString->String,
				pPCFString->StringLength);
		break;
	case MQCA_INITIATION_Q_NAME:
		memset(QDefn->InitiationQName, ' ', MQ_Q_NAME_LENGTH);
		memcpy(QDefn->InitiationQName, pPCFString->String,
				pPCFString->StringLength);
		break;
	case MQCA_TRIGGER_DATA:
		memset(QDefn->TriggerData, ' ', MQ_TRIGGER_DATA_LENGTH);
		memcpy(QDefn->TriggerData, pPCFString->String, pPCFString->StringLength);
		break;
	case MQCA_BASE_Q_NAME:
		memset(QDefn->BaseQName, ' ', MQ_Q_NAME_LENGTH);
		memcpy(QDefn->BaseQName, pPCFString->String, pPCFString->StringLength);
		break;
	case MQCA_REMOTE_Q_NAME:
		memset(QDefn->RemoteQName, ' ', MQ_Q_NAME_LENGTH);
		memcpy(QDefn->RemoteQName, pPCFString->String, pPCFString->StringLength);
		break;
	case MQCA_REMOTE_Q_MGR_NAME:
		memset(QDefn->RemoteQMgrName, ' ', MQ_Q_MGR_NAME_LENGTH);
		memcpy(QDefn->RemoteQMgrName, pPCFString->String,
				pPCFString->StringLength);
		break;
	case MQCA_XMIT_Q_NAME:
		memset(QDefn->XmitQName, ' ', MQ_Q_NAME_LENGTH);
		memcpy(QDefn->XmitQName, pPCFString->String, pPCFString->StringLength);
		break;
		/* [v51] begin v51 attributes */
#if defined (MQCA_CLUSTER_NAME)
	case MQCA_CLUSTER_NAME:
		memset(QDefn->ClusterName, ' ', MQ_CLUSTER_NAME_LENGTH);
		memcpy(QDefn->ClusterName, pPCFString->String, pPCFString->StringLength);
		break;
#endif
#if defined (MQCA_CLUSTER_NAMELIST)
	case MQCA_CLUSTER_NAMELIST:
		memset(QDefn->ClusterNameList, ' ', MQ_OBJECT_NAME_LENGTH);
		memcpy(QDefn->ClusterNameList, pPCFString->String,
				pPCFString->StringLength);
		break;
#endif
#if defined (MQCA_ALTERATION_DATE)
	case MQCA_ALTERATION_DATE:
		memset(QDefn->AlterationDate, ' ', MQ_CREATION_DATE_LENGTH);
		memcpy(QDefn->AlterationDate, pPCFString->String,
				pPCFString->StringLength);
		break;
	case MQCA_ALTERATION_TIME:
		memset(QDefn->AlterationTime, ' ', MQ_CREATION_TIME_LENGTH);
		memcpy(QDefn->AlterationTime, pPCFString->String,
				pPCFString->StringLength);
		break;

#endif

#if defined(zzzOS)
		/* v602 zOS attributes */
	case MQBACF_RESPONSE_ID: /* zOS things to ignore */
		break;
	case MQCACF_RESPONSE_Q_MGR_NAME: /* zOS things to ignore */
		break;
	case MQCA_CF_STRUC_NAME: /* zOS */
		memset(QDefn->cfStrucName, ' ', MQ_CF_STRUC_NAME_LENGTH);
		memcpy(QDefn->cfStrucName, pPCFString->String, pPCFString->StringLength);
		break;
	case MQCA_STORAGE_CLASS: /* zOS */
		memset(QDefn->storageClass, ' ', MQ_STORAGE_CLASS_LENGTH);
		memcpy(QDefn->storageClass, pPCFString->String,
				pPCFString->StringLength);
		break;

#endif

	default:
		if (noisy)
			fprintf(stderr, "(queue.c) Unknown PCF String Parameter: %ld\n",
			pPCFString->Parameter);
		*iCC = WARNING;
		break;
	} /* endswitch */

	return;
}

void ProcessQueueStringListParm(MQCFSL *pPCFString, QParms *QDefn, MQLONG *iCC,CmdArgParmsPtr pMyParms) {
#define STREND ", +\n*          "
#define STRENDLEN strlen(STREND)
	/* Initialise return codes */

#if defined(zzzOS)
	int i = 0;
	char * pT= NULL;
	char * pS= NULL;
#endif

	*iCC = MQCC_OK;

	switch (pPCFString->Parameter) {

#if defined(zzzOS)
	case MQCA_TPIPE_NAME:

		pT = QDefn->tpipes;
		pS = pPCFString->Strings;

		if (pPCFString->Count) {
			strcpy(pT, " "); /* opening blank */
			pT++;
			for (i = 0; i<pPCFString->Count; i++) {
				strncat(pT, pS, pPCFString->StringLength);
				if (pMyParms->oneLine==0 && pMyParms->twoLines == 0) {
					strcat(pT, STREND);
				}
				pS+=pPCFString->StringLength;
			}
			strcat(pT, " "); /* closing blank */
		} else {
			strcpy(pT, " ");
		}

		/* trim off the ending quotes and comma */
		if (strlen(pT) >= STRENDLEN)
			*(pT + strlen(pT) - STRENDLEN) = '\0';

		break;
#endif

	default:
		if (noisy)
			fprintf(stderr,
			"(queue.c) Unknown PCF String List Parameter: %ld\n",
			pPCFString->Parameter);
		*iCC = WARNING;
		break;
	} /* endswitch */

	return;
}

void ProcessQueueIntegerParm(MQCFIN *pPCFInteger, QParms *QDefn, MQLONG *iCC) {
	/* Initialise return codes */
	*iCC = MQCC_OK; /*[wms] fix */

	switch (pPCFInteger->Parameter) {
	case MQIA_Q_TYPE:
		QDefn->QType = pPCFInteger->Value;
		break;
	case MQIA_INHIBIT_PUT:
		QDefn->InhibitPut = pPCFInteger->Value;
		break;
	case MQIA_DEF_PRIORITY:
		QDefn->DefPriority = pPCFInteger->Value;
		break;
	case MQIA_DEF_PERSISTENCE:
		QDefn->DefPersistence = pPCFInteger->Value;
		break;
	case MQIA_INHIBIT_GET:
		QDefn->InhibitGet = pPCFInteger->Value;
		break;
	case MQIA_SCOPE:
		QDefn->Scope = pPCFInteger->Value;
		break;
	case MQIA_MAX_Q_DEPTH:
		QDefn->MaxQDepth = pPCFInteger->Value;
		break;
	case MQIA_MAX_MSG_LENGTH:
		QDefn->MaxMsgLength = pPCFInteger->Value;
		break;
	case MQIA_BACKOUT_THRESHOLD:
		QDefn->BackoutThreshold = pPCFInteger->Value;
		break;
	case MQIA_SHAREABILITY:
		QDefn->Shareability = pPCFInteger->Value;
		break;
	case MQIA_DEF_INPUT_OPEN_OPTION:
		QDefn->DefInputOpenOption = pPCFInteger->Value;
		break;
	case MQIA_HARDEN_GET_BACKOUT:
		QDefn->HardenGetBackout = pPCFInteger->Value;
		break;
	case MQIA_MSG_DELIVERY_SEQUENCE:
		QDefn->MsgDeliverySequence = pPCFInteger->Value; /* [wms] fix only */
		break;
	case MQIA_RETENTION_INTERVAL:
		QDefn->RetentionInterval = pPCFInteger->Value;
		break;
	case MQIA_DEFINITION_TYPE:
		QDefn->DefinitionType = pPCFInteger->Value;
		break;
	case MQIA_USAGE:
		QDefn->Usage = pPCFInteger->Value;
		break;
	case MQIA_OPEN_INPUT_COUNT:
		QDefn->OpenInputCount = pPCFInteger->Value;
		break;
	case MQIA_OPEN_OUTPUT_COUNT:
		QDefn->OpenOutputCount = pPCFInteger->Value;
		break;
	case MQIA_CURRENT_Q_DEPTH:
		QDefn->CurrentQDepth = pPCFInteger->Value;
		break;
	case MQIA_TRIGGER_CONTROL:
		QDefn->TriggerControl = pPCFInteger->Value;
		break;
	case MQIA_TRIGGER_TYPE:
		QDefn->TriggerType = pPCFInteger->Value;
		break;
	case MQIA_TRIGGER_MSG_PRIORITY:
		QDefn->TriggerMsgPriority = pPCFInteger->Value;
		break;
	case MQIA_TRIGGER_DEPTH:
		QDefn->TriggerDepth = pPCFInteger->Value;
		break;
	case MQIA_Q_DEPTH_HIGH_LIMIT:
		QDefn->QDepthHighLimit = pPCFInteger->Value;
		break;
	case MQIA_Q_DEPTH_LOW_LIMIT:
		QDefn->QDepthLowLimit = pPCFInteger->Value;
		break;
	case MQIA_Q_DEPTH_MAX_EVENT:
		QDefn->QDepthMaxEvent = pPCFInteger->Value;
		break;
	case MQIA_Q_DEPTH_HIGH_EVENT:
		QDefn->QDepthHighEvent = pPCFInteger->Value;
		break;
	case MQIA_Q_DEPTH_LOW_EVENT:
		QDefn->QDepthLowEvent = pPCFInteger->Value;
		break;
	case MQIA_Q_SERVICE_INTERVAL:
		QDefn->QServiceInterval = pPCFInteger->Value;
		break;
	case MQIA_Q_SERVICE_INTERVAL_EVENT:
		QDefn->QServiceIntervalEvent = pPCFInteger->Value;
		break;
#if defined (MQIA_DIST_LISTS)
		/* [wms] start V5 attributes */
	case MQIA_DIST_LISTS:
		QDefn->DistLists = pPCFInteger->Value;
		break;
		/* [wms] end V5 attributes */
#endif
#if defined (MQIA_DEF_BIND)
		/* [v51] start V51 attributes */
	case MQIA_DEF_BIND:
		QDefn->DefBind = pPCFInteger->Value;
		break;
		/* [v51] end V51 attributes */
#endif

		/* [v533 csd6 adds NPMCLASS */
#ifdef MQIA_NPM_CLASS
	case MQIA_NPM_CLASS:
		QDefn->npmClass = pPCFInteger->Value;
		foundnpmclass=1; /* show the we are at csd 6 */
		break;
#endif
		/* v6 attributes */
#ifdef zzMQV60
	case MQIA_STATISTICS_Q:
		QDefn->StatQ = pPCFInteger->Value;
		break;
	case MQIA_ACCOUNTING_Q:
		QDefn->ActngQ = pPCFInteger->Value;
		break;
	case MQIA_MONITORING_Q:
		QDefn->MonQ = pPCFInteger->Value;
		break;
	case MQIA_CLWL_Q_RANK:
		QDefn->ClwlRank = pPCFInteger->Value;
		break;
	case MQIA_CLWL_Q_PRIORITY:
		QDefn->ClwlPri = pPCFInteger->Value;
		break;
	case MQIA_CLWL_USEQ:
		QDefn->ClwlUseQ = pPCFInteger->Value;
		break;
#endif
#ifdef zzMQV70
	case MQIA_DEF_PUT_RESPONSE_TYPE:
		QDefn->DefPResP = pPCFInteger->Value;
		break;
	case MQIA_DEF_READ_AHEAD:
		QDefn->DefRdAhd = pPCFInteger->Value;
		break;
	case MQIA_PROPERTY_CONTROL:
		QDefn->PropCntl = pPCFInteger->Value;
		break;
	case MQIA_BASE_TYPE:
		QDefn->TargType = pPCFInteger->Value;
		break;
#endif
#if defined(zzzOS)
		/* v602 zOS attributes */
	case MQIA_QSG_DISP: /* zOS */
		QDefn->qsgDisp = pPCFInteger->Value;
		break;
	case MQIA_PAGESET_ID: /* zOS */
		QDefn->pagesetId = pPCFInteger->Value;
		break;
	case MQIA_INDEX_TYPE: /* zOS */
		QDefn->indexType = pPCFInteger->Value;
		break;

#endif

	default:
		if (noisy)
			fprintf(stderr, "(queue.c) Unknown PCF Integer Parameter: %ld\n",
			pPCFInteger->Parameter);
		*iCC = WARNING;
		break;
	} /* endswitch */

	return;
}

/* ------------------------------------------------------------------------- */
/*                                                                           */
/* This process takes the attributes of a single queue and adds them         */
/* to the end of a file, SAVEQMGR.TST, which can be found in the current     */
/* directory.                                                                */
/*                                                                           */
/* The file is of a format suitable for subsequent input to RUNMQSC.         */
/*                                                                           */
/* ------------------------------------------------------------------------- */

int AddToFileQUEUE(QParms * pQDefn, int UserVersion, char * fileName, /* v32 Name of the output file  */
FILE * fp, /* v51 */
int twolines, /* v601 */
MQLONG *iCC) {
	char *ParmBuffer; /* Temporary buffer to hold for output to file */
	char work[256];

	int options;
	int retcode = 0;

	/* Initialise return codes */
	*iCC = MQCC_OK;

	work[0]='\0';

	if ( !(ParmBuffer = (char *) malloc(PBSIZE) )) {
		fprintf(stderr, "(queue.c) Failed to allocate %d bytes \n", PBSIZE);
		exit(2);
	}

	/* okay, we do this so later on OAM can determine what type of queue it has */
	/* which means there's a very good chance this wil never get used .... */
	/* oh well..... */

	if ( !(qListTerm->next = (char*) malloc(sizeof(qList)) )) {
		fprintf(stderr, "(queue.c) Failed to allocate %d bytes \n",
		sizeof(qList));
		exit(2);
	}

	qListTerm = (pqList)qListTerm->next;
	/* set the new area to blanks and zeros */
	memset(qListTerm->qName, ' ', sizeof(qListTerm->qName));
	qListTerm->next = 0;
	/* set the info in the list */
	qListTerm->qType = pQDefn->QType;
	qListTerm->qDefType = pQDefn->DefinitionType;
	memcpy(qListTerm->qName, pQDefn->QName, sizeof(pQDefn->QName));

	/* v601 setup MQsprintf_X to break lines based on oneLine setting */
	options = MQSPRINTF_X_DOUBLE_QUOTES + MQSPRINTF_X_NO_TRAILING_BLANKS;
	if (!oneLine)
		options += MQSPRINTF_X_LINE_BREAKS;

	/* [v530]  Process  all local queues and any predefined queues */
	/* [v530] If the user so indicates, also process PermDyn queues */

	if ( (pQDefn->QType!=MQQT_LOCAL) || /* Process all non-local queues */
	(pQDefn->DefinitionType == MQQDT_PREDEFINED) || (permDyn
			&& (pQDefn->DefinitionType == MQQDT_PERMANENT_DYNAMIC))) {

		/* Append these details to the end of the current output file (v32) */
		/* v51 remove open from here ...   fp = fopen( fileName, OPENMODE ); */

		if (twolines)
			fputs("* ", fp);

		/* ---------------------------------------------------------------- */
		/* Parameters general to all queue types                            */
		/* ---------------------------------------------------------------- */

		switch (pQDefn->QType) {
		case MQQT_LOCAL:
			MQsprintf_X(ParmBuffer, "DEFINE QLOCAL ('%s') ", pQDefn->QName, 
			MQ_Q_NAME_LENGTH, options);

			break;
		case MQQT_ALIAS:
			MQsprintf_X(ParmBuffer, "DEFINE QALIAS ('%s') ", pQDefn->QName, 
			MQ_Q_NAME_LENGTH, options);
			break;
		case MQQT_MODEL:
			MQsprintf_X(ParmBuffer, "DEFINE QMODEL ('%s') ", pQDefn->QName, 
			MQ_Q_NAME_LENGTH, options);
			break;
		case MQQT_REMOTE:
			MQsprintf_X(ParmBuffer, "DEFINE QREMOTE ('%s') ", pQDefn->QName, 
			MQ_Q_NAME_LENGTH, options);
			break;
		default:
			*iCC = WARNING;
			if (noisy)
				fprintf(stderr, "Unknown queue type = %ld \n", pQDefn->QType);
			break;
		} /* endswitch */
		fputs(ParmBuffer, fp);

		if (!oneLine || twolines) {
			if (pQDefn->QType == MQQT_LOCAL || pQDefn->QType == MQQT_MODEL) {
				MQsprintf_X(ParmBuffer, "* CRDATE (%s) ", pQDefn->CreationDate, 
				MQ_CREATION_DATE_LENGTH, options);
				fputs(ParmBuffer, fp);
				MQsprintf_X(ParmBuffer, "* CRTIME (%s) ", pQDefn->CreationTime, 
				MQ_CREATION_TIME_LENGTH, options);
				fputs(ParmBuffer, fp);
			}

#if defined (MQCA_ALTERATION_DATE)
			if (header) {
				MQsprintf_X(ParmBuffer, "* ALTDATE (%s) ",
						pQDefn->AlterationDate, 
						MQ_CREATION_DATE_LENGTH, options);
				fputs(ParmBuffer, fp);
				MQsprintf_X(ParmBuffer, "* ALTTIME (%s) ",
						pQDefn->AlterationTime, 
						MQ_CREATION_TIME_LENGTH, options);
				fputs(ParmBuffer, fp);
			}
#endif
		} /* end of if ! oneline */
		/* [v530] If we're making a local q from a Permdyn one, */
		/* [v530] then make a little note in the descr field */

		if (permDyn && pQDefn->QType == MQQT_LOCAL && pQDefn->DefinitionType
				== MQQDT_PERMANENT_DYNAMIC) {
			MQsprintf_X(ParmBuffer, "       DESCR('(Defn from PermDyn Q)%s') ",
					pQDefn->QDesc, MQ_Q_DESC_LENGTH - 21, options);
		} else {
			MQsprintf_X(ParmBuffer, "       DESCR('%s') ", pQDefn->QDesc, 
			MQ_Q_DESC_LENGTH, options);
		}/* End if*/
		fputs(ParmBuffer, fp);

		if (pQDefn->InhibitPut == MQQA_PUT_ALLOWED) {
			sprintf(ParmBuffer, "       PUT(ENABLED) %s", lineTerm);
			fputs(ParmBuffer, fp);
		} else {
			sprintf(ParmBuffer, "       PUT(DISABLED) %s", lineTerm);
			fputs(ParmBuffer, fp);
		} /* endif */

		sprintf(ParmBuffer, "       DEFPRTY(%ld) %s", pQDefn->DefPriority,
				lineTerm);
		fputs(ParmBuffer, fp);

		if (pQDefn->DefPersistence == MQPER_PERSISTENT) {
			sprintf(ParmBuffer, "       DEFPSIST(YES) %s", lineTerm);
			fputs(ParmBuffer, fp);
		} else {
			sprintf(ParmBuffer, "       DEFPSIST(NO) %s", lineTerm);
			fputs(ParmBuffer, fp);
		} /* endif */
#ifdef zzMQV70
		if (UserVersion >= MQCMDL_LEVEL_700) {
			if (pQDefn->DefPResP == MQPRT_SYNC_RESPONSE) {
				sprintf(ParmBuffer, "       DEFPRESP(SYNC) %s", lineTerm);
				fputs(ParmBuffer, fp);
			} else {
				sprintf(ParmBuffer, "       DEFPRESP(ASYNC) %s", lineTerm);
				fputs(ParmBuffer, fp);
			} /* endif */
		}
#endif
		/* ---------------------------------------------------------------- */
		/* Parameters exclusive to QLOCAL                                   */
		/* ---------------------------------------------------------------- */

		if (pQDefn->QType == MQQT_LOCAL) {
			if (header && (!oneLine || twolines)) {
				sprintf(ParmBuffer, "* CURDEPTH(%ld) %s",
						pQDefn->CurrentQDepth, lineTerm);
				fputs(ParmBuffer, fp);
			}

#ifdef zzMQV60
			if (UserVersion>=MQCMDL_LEVEL_600) {
				toStrMQCLWL((char *)&work, pQDefn->ClwlUseQ);
				sprintf(ParmBuffer, "       CLWLUSEQ(%s) %s", work, lineTerm);
				fputs(ParmBuffer, fp);
			}
#endif

#if defined(zzzOS)
			if (platform==MQPL_MVS && (!oneLine || twolines)) {
				MQsprintf_X(ParmBuffer, "*      TPIPES(%s) ",
						(char*) pQDefn->tpipes , TPIPE_BUFFER_SIZE, options);
				fputs(ParmBuffer, fp);
			}
#endif
		}

		/* ---------------------------------------------------------------- */
		/* Parameters common to QLOCAL, QALIAS and QREMOTE                  */
		/* ---------------------------------------------------------------- */
		/* if (  MQAT_DEFAULT != MQAT_OS400 && ( *//* v510 AvV */
		if (platform != MQPL_OS400 && platform != MQPL_MVS && ( /* v510 AvV */
		pQDefn->QType == MQQT_LOCAL || pQDefn->QType == MQQT_ALIAS
				|| pQDefn->QType == MQQT_REMOTE )) {
			if (pQDefn->Scope == MQSCO_Q_MGR) {
				sprintf(ParmBuffer, "       SCOPE(QMGR) %s", lineTerm);
				fputs(ParmBuffer, fp);
			} else {
				sprintf(ParmBuffer, "       SCOPE(CELL) %s", lineTerm);
				fputs(ParmBuffer, fp);
			} /* endif */

		} /* endif */

		/* ---------------------------------------------------------------- */
		/* Parameters common to QLOCAL, QALIAS and QMODEL                   */
		/* ---------------------------------------------------------------- */
		if (pQDefn->QType == MQQT_LOCAL || pQDefn->QType == MQQT_ALIAS
				|| pQDefn->QType == MQQT_MODEL) {

			if (pQDefn->InhibitGet == MQQA_GET_ALLOWED) {
				sprintf(ParmBuffer, "       GET(ENABLED) %s", lineTerm);
				fputs(ParmBuffer, fp);
			} else {
				sprintf(ParmBuffer, "       GET(DISABLED) %s", lineTerm);
				fputs(ParmBuffer, fp);
			} /* endif */
#ifdef zzMQV70
			if (UserVersion >= MQCMDL_LEVEL_700) {
				switch (pQDefn->PropCntl) {
				case MQPROP_COMPATIBILITY:
					sprintf(ParmBuffer, "       PROPCTL(COMPAT) %s", lineTerm);
					fputs(ParmBuffer, fp);
					break;
				case MQPROP_NONE:
					sprintf(ParmBuffer, "       PROPCTL(NONE) %s", lineTerm);
					fputs(ParmBuffer, fp);
					break;
				case MQPROP_ALL:
					sprintf(ParmBuffer, "       PROPCTL(ALL) %s", lineTerm);
					fputs(ParmBuffer, fp);
					break;
				case MQPROP_FORCE_MQRFH2:
					sprintf(ParmBuffer, "       PROPCTL(FORCE) %s", lineTerm);
					fputs(ParmBuffer, fp);
					break;
				}
				switch (pQDefn->DefRdAhd) {
				case MQREADA_NO:
					sprintf(ParmBuffer, "       DEFREADA(NO) %s", lineTerm);
					fputs(ParmBuffer, fp);
					break;
				case MQREADA_YES:
					sprintf(ParmBuffer, "       DEFREADA(YES) %s", lineTerm);
					fputs(ParmBuffer, fp);
					break;
				case MQREADA_DISABLED:
					sprintf(ParmBuffer, "       DEFREADA(DISABLED) %s",
							lineTerm);
					fputs(ParmBuffer, fp);
					break;
				} /* endswitch */
			}
#endif
		} /* endif */

		/* ---------------------------------------------------------------- */
		/* Parameters exclusive to QALIAS                                   */
		/* ---------------------------------------------------------------- */
		if (pQDefn->QType == MQQT_ALIAS) {
			if (UserVersion < MQCMDL_LEVEL_700) {
				MQsprintf_X(ParmBuffer, "       TARGQ('%s') ",
						pQDefn->BaseQName, 
						MQ_Q_NAME_LENGTH, options);
				fputs(ParmBuffer, fp);
			}
#if defined(zzMQV70)			
			if (UserVersion >= MQCMDL_LEVEL_700) {
				MQsprintf_X(ParmBuffer, "       TARGET('%s') ",
						pQDefn->BaseQName, 
						MQ_Q_NAME_LENGTH, options);
				fputs(ParmBuffer, fp);
				if (pQDefn->TargType == MQOT_Q) {
					sprintf(ParmBuffer, "       TARGTYPE(QUEUE) %s", lineTerm);
					fputs(ParmBuffer, fp);
				} else if (pQDefn->TargType == MQOT_TOPIC) {
					sprintf(ParmBuffer, "       TARGTYPE(TOPIC) %s", lineTerm);
					fputs(ParmBuffer, fp);
				}
			}
#endif
		} /* endif */

		/* ---------------------------------------------------------------- */
		/* Parameters exclusive to QREMOTE                                  */
		/* ---------------------------------------------------------------- */	
		if (pQDefn->QType == MQQT_REMOTE) {

			MQsprintf_X(ParmBuffer, "       XMITQ('%s') ", pQDefn->XmitQName, 
			MQ_Q_NAME_LENGTH, options);
			fputs(ParmBuffer, fp);

			MQsprintf_X(ParmBuffer, "       RNAME('%s') ", pQDefn->RemoteQName, 
			MQ_Q_NAME_LENGTH, options);
			fputs(ParmBuffer, fp);

			MQsprintf_X(ParmBuffer, "       RQMNAME('%s') ",
					pQDefn->RemoteQMgrName, MQ_Q_MGR_NAME_LENGTH, options);
			fputs(ParmBuffer, fp);

		} /* endif */

		/* ---------------------------------------------------------------- */
		/* Parameters exclusive to QMODEL                                   */
		/* ---------------------------------------------------------------- */
		if (pQDefn->QType == MQQT_MODEL) {

			switch (pQDefn->DefinitionType) {
			case MQQDT_PREDEFINED:
				break;
			case MQQDT_PERMANENT_DYNAMIC:
				sprintf(ParmBuffer, "       DEFTYPE(PERMDYN) %s", lineTerm);
				fputs(ParmBuffer, fp);
				break;
			case MQQDT_TEMPORARY_DYNAMIC:
				sprintf(ParmBuffer, "       DEFTYPE(TEMPDYN) %s", lineTerm);
				fputs(ParmBuffer, fp);
				break;
#if defined(zzzOS)
			case MQQDT_SHARED_DYNAMIC:
				sprintf(ParmBuffer, "       DEFTYPE(SHAREDYN) %s", lineTerm);
				fputs(ParmBuffer, fp);
				break;
#endif
			} /* endswitch */

		} /* endif */

		/* ---------------------------------------------------------------- */
		/* Parameters common to QLOCAL and QMODEL                           */
		/* ---------------------------------------------------------------- */
		if (pQDefn->QType == MQQT_LOCAL || pQDefn->QType == MQQT_MODEL) {

			/* [v530] here's a little hack, if we're generating pre 5.3 */
			/* mqsc, maxdepth can't be greater than 640000 */
			if (UserVersion < MQCMDL_LEVEL_530 && pQDefn->MaxQDepth > 640000)
				pQDefn->MaxQDepth = 640000;

			sprintf(ParmBuffer, "       MAXDEPTH(%ld) %s", pQDefn->MaxQDepth,
					lineTerm);
			fputs(ParmBuffer, fp);

			sprintf(ParmBuffer, "       MAXMSGL(%ld) %s", pQDefn->MaxMsgLength,
					lineTerm);
			fputs(ParmBuffer, fp);

			if (pQDefn->Shareability == MQQA_SHAREABLE) {
				sprintf(ParmBuffer, "       SHARE %s", lineTerm);
				fputs(ParmBuffer, fp);
			} else {
				sprintf(ParmBuffer, "       NOSHARE %s", lineTerm);
				fputs(ParmBuffer, fp);
			} /* endif */

			if (pQDefn->DefInputOpenOption == MQOO_INPUT_SHARED) {
				sprintf(ParmBuffer, "       DEFSOPT(SHARED) %s", lineTerm);
				fputs(ParmBuffer, fp);
			} else {
				sprintf(ParmBuffer, "       DEFSOPT(EXCL) %s", lineTerm);
				fputs(ParmBuffer, fp);
			} /* endif */

			if (pQDefn->MsgDeliverySequence == MQMDS_PRIORITY) {
				sprintf(ParmBuffer, "       MSGDLVSQ(PRIORITY) %s", lineTerm);
				fputs(ParmBuffer, fp);
			} else {
				sprintf(ParmBuffer, "       MSGDLVSQ(FIFO) %s", lineTerm);
				fputs(ParmBuffer, fp);
			} /* endif */

			if (pQDefn->HardenGetBackout == MQQA_BACKOUT_HARDENED) {
				sprintf(ParmBuffer, "       HARDENBO %s", lineTerm);
				fputs(ParmBuffer, fp);
			} else {
				sprintf(ParmBuffer, "       NOHARDENBO %s", lineTerm);
				fputs(ParmBuffer, fp);
			} /* endif */

			if (pQDefn->Usage == MQUS_NORMAL) {
				sprintf(ParmBuffer, "       USAGE(NORMAL) %s", lineTerm);
				fputs(ParmBuffer, fp);
			} else {
				sprintf(ParmBuffer, "       USAGE(XMITQ) %s", lineTerm);
				fputs(ParmBuffer, fp);
			} /* endif */

			if (pQDefn->TriggerControl == MQTC_OFF) {
				sprintf(ParmBuffer, "       NOTRIGGER %s", lineTerm);
				fputs(ParmBuffer, fp);
			} else {
				sprintf(ParmBuffer, "       TRIGGER %s", lineTerm);
				fputs(ParmBuffer, fp);
			} /* endif */

			switch (pQDefn->TriggerType) {
			case MQTT_NONE:
				sprintf(ParmBuffer, "       TRIGTYPE(NONE) %s", lineTerm);
				fputs(ParmBuffer, fp);
				break;
			case MQTT_FIRST:
				sprintf(ParmBuffer, "       TRIGTYPE(FIRST) %s", lineTerm);
				fputs(ParmBuffer, fp);
				break;
			case MQTT_EVERY:
				sprintf(ParmBuffer, "       TRIGTYPE(EVERY) %s", lineTerm);
				fputs(ParmBuffer, fp);
				break;
			case MQTT_DEPTH:
				sprintf(ParmBuffer, "       TRIGTYPE(DEPTH) %s", lineTerm);
				fputs(ParmBuffer, fp);
				break;
			} /* endswitch */

			sprintf(ParmBuffer, "       TRIGDPTH(%ld) %s",
					pQDefn->TriggerDepth, lineTerm);
			fputs(ParmBuffer, fp);

			sprintf(ParmBuffer, "       TRIGMPRI(%ld) %s",
					pQDefn->TriggerMsgPriority, lineTerm);
			fputs(ParmBuffer, fp);

			MQsprintf_X(ParmBuffer, "       TRIGDATA('%s') ",
					pQDefn->TriggerData, MQ_TRIGGER_DATA_LENGTH, options);
			fputs(ParmBuffer, fp);

			MQsprintf_X(ParmBuffer, "       PROCESS('%s') ",
					pQDefn->ProcessName, MQ_PROCESS_NAME_LENGTH, options);
			fputs(ParmBuffer, fp);

			MQsprintf_X(ParmBuffer, "       INITQ('%s') ",
					pQDefn->InitiationQName, MQ_Q_NAME_LENGTH, options);
			fputs(ParmBuffer, fp);

			sprintf(ParmBuffer, "       RETINTVL(%ld) %s",
					pQDefn->RetentionInterval, lineTerm);
			fputs(ParmBuffer, fp);

			sprintf(ParmBuffer, "       BOTHRESH(%ld) %s",
					pQDefn->BackoutThreshold, lineTerm);
			fputs(ParmBuffer, fp);

			MQsprintf_X(ParmBuffer, "       BOQNAME('%s') ",
					pQDefn->BackoutReqQName, MQ_Q_NAME_LENGTH, options);
			fputs(ParmBuffer, fp);

			sprintf(ParmBuffer, "       QDEPTHHI(%ld) %s",
					pQDefn->QDepthHighLimit, lineTerm);
			fputs(ParmBuffer, fp);

			sprintf(ParmBuffer, "       QDEPTHLO(%ld) %s",
					pQDefn->QDepthLowLimit, lineTerm);
			fputs(ParmBuffer, fp);

			if (pQDefn->QDepthMaxEvent == MQEVR_ENABLED) {
				sprintf(ParmBuffer, "       QDPMAXEV(ENABLED) %s", lineTerm);
				fputs(ParmBuffer, fp);
			} else {
				sprintf(ParmBuffer, "       QDPMAXEV(DISABLED) %s", lineTerm);
				fputs(ParmBuffer, fp);
			} /* endif */

			if (pQDefn->QDepthHighEvent == MQEVR_ENABLED) {
				sprintf(ParmBuffer, "       QDPHIEV(ENABLED) %s", lineTerm);
				fputs(ParmBuffer, fp);
			} else {
				sprintf(ParmBuffer, "       QDPHIEV(DISABLED) %s", lineTerm);
				fputs(ParmBuffer, fp);
			} /* endif */

			if (pQDefn->QDepthLowEvent == MQEVR_ENABLED) {
				sprintf(ParmBuffer, "       QDPLOEV(ENABLED) %s", lineTerm);
				fputs(ParmBuffer, fp);
			} else {
				sprintf(ParmBuffer, "       QDPLOEV(DISABLED) %s", lineTerm);
				fputs(ParmBuffer, fp);
			} /* endif */

			sprintf(ParmBuffer, "       QSVCINT(%ld) %s",
					pQDefn->QServiceInterval, lineTerm);
			fputs(ParmBuffer, fp);

			switch (pQDefn->QServiceIntervalEvent) {
			case MQQSIE_OK:
				sprintf(ParmBuffer, "       QSVCIEV(OK) %s", lineTerm);
				fputs(ParmBuffer, fp);
				break;
			case MQQSIE_NONE:
				sprintf(ParmBuffer, "       QSVCIEV(NONE) %s", lineTerm);
				fputs(ParmBuffer, fp);
				break;
			case MQQSIE_HIGH:
				sprintf(ParmBuffer, "       QSVCIEV(HIGH) %s", lineTerm);
				fputs(ParmBuffer, fp);
				break;
			} /* endswitch */
#if defined(MQDL_SUPPORTED)
			/* [wms] start V5 attributes */

			if (UserVersion>=MQCMDL_LEVEL_500 && platform != MQPL_MVS) {
				if (pQDefn->DistLists == MQDL_SUPPORTED) {
					sprintf(ParmBuffer, "       DISTL(YES) %s", lineTerm);
					fputs(ParmBuffer, fp);
				} else {
					sprintf(ParmBuffer, "       DISTL(NO) %s", lineTerm);
					fputs(ParmBuffer, fp);
				} /* endif */

			}
			/* [wms]  end V5 attributes */
#endif

			/* v533 csd6 adds npmclass */
#ifdef MQIA_NPM_CLASS
			if (UserVersion>=MQCMDL_LEVEL_530 && foundnpmclass && platform
					!= MQPL_MVS) {
				switch (pQDefn->npmClass) {
				case MQNPM_CLASS_NORMAL:
					sprintf(ParmBuffer, "       NPMCLASS(NORMAL) %s", lineTerm);
					fputs(ParmBuffer, fp);
					break;
				case MQNPM_CLASS_HIGH:
					sprintf(ParmBuffer, "       NPMCLASS(HIGH) %s", lineTerm);
					fputs(ParmBuffer, fp);
					break;
				} /* endswitch */
			} /* end if userversion > 500 */
#endif

#ifdef zzMQV60
			if (UserVersion>=MQCMDL_LEVEL_600) {

				if (platform != MQPL_MVS) {
					toStrMQMON((char*)&work, pQDefn->StatQ);
					sprintf(ParmBuffer, "       STATQ(%s) %s", work, lineTerm);
					fputs(ParmBuffer, fp);
				}

				toStrMQMON((char*)&work, pQDefn->MonQ);
				sprintf(ParmBuffer, "       MONQ(%s) %s", work, lineTerm);
				fputs(ParmBuffer, fp);

				toStrMQMON((char*)&work, pQDefn->ActngQ);
				sprintf(ParmBuffer, "       ACCTQ(%s) %s", work, lineTerm);
				fputs(ParmBuffer, fp);

			}

#endif
#if defined(zzzOS)
			if (platform==MQPL_MVS) {

				MQsprintf_X(ParmBuffer, "       CFSTRUCT('%s') ",
						pQDefn->cfStrucName, MQ_CF_STRUC_NAME_LENGTH, options);
				fputs(ParmBuffer, fp);

				MQsprintf_X(ParmBuffer, "       STGCLASS('%s') ",
						pQDefn->storageClass, MQ_STORAGE_CLASS_LENGTH, options);
				fputs(ParmBuffer, fp);

				toStrINDXTYPE((char*)&work, pQDefn->indexType);
				sprintf(ParmBuffer, "       INDXTYPE(%s) %s", work, lineTerm);
				fputs(ParmBuffer, fp);

				if (!oneLine || twolines) {
					sprintf(ParmBuffer, "*      PSID(%ld) %s",
							pQDefn->pagesetId, lineTerm);
					fputs(ParmBuffer, fp);
				}

			}

#endif

		} /* endif */

		/* ---------------------------------------------------------------- */
		/* [v5] Parms common to QLOCAL and QREMOTE and QALIAS               */
		/* ---------------------------------------------------------------- */
		if (pQDefn->QType == MQQT_LOCAL || pQDefn->QType == MQQT_REMOTE
				|| pQDefn->QType == MQQT_ALIAS) {

#if defined(MQ_CLUSTER_NAME_LENGTH)
			/* [v51] start V51 attributes */

			if (UserVersion>=MQCMDL_LEVEL_510) {

				MQsprintf_X(ParmBuffer, "       CLUSTER('%s') ",
						pQDefn->ClusterName, MQ_CLUSTER_NAME_LENGTH, options);
				fputs(ParmBuffer, fp);
				MQsprintf_X(ParmBuffer, "       CLUSNL('%s') ",
						pQDefn->ClusterNameList, MQ_CLUSTER_NAME_LENGTH,
						options);
				fputs(ParmBuffer, fp);
				if (pQDefn->DefBind == MQBND_BIND_ON_OPEN) {
					sprintf(ParmBuffer, "       DEFBIND(OPEN) %s", lineTerm);
					fputs(ParmBuffer, fp);
				} else {
					sprintf(ParmBuffer, "       DEFBIND(NOTFIXED) %s", lineTerm);
					fputs(ParmBuffer, fp);
				} /* endif */
			} /* end of user want v51 */
#endif

#ifdef zzMQV60
			if (UserVersion>=MQCMDL_LEVEL_600) {

				sprintf(ParmBuffer, "       CLWLRANK(%ld) %s",
						pQDefn->ClwlRank, lineTerm);
				fputs(ParmBuffer, fp);

				sprintf(ParmBuffer, "       CLWLPRTY(%ld) %s", pQDefn->ClwlPri,
						lineTerm);
				fputs(ParmBuffer, fp);
			}
#endif

		} /* end of local remote and alias common defs */

#if defined(zzzOS)
		if (platform == MQPL_MVS) {
			toStrQSG((char*)&work, pQDefn->qsgDisp);
			sprintf(ParmBuffer, "       QSGDISP(%s) %s", work, lineTerm);
			fputs(ParmBuffer, fp);

		}
#endif

		sprintf(ParmBuffer, "       REPLACE\n");
		fputs(ParmBuffer, fp);

		if (!oneLine) {
			fputs("\n", fp);
		}

	} else /* Its a dynmaic queue and we don't want to process it */
	{
		fprintf(stderr, "Skipping dynamic queue %.48s\n", pQDefn->QName);
		retcode = 1;
	} /* endif */

	free(ParmBuffer);
	return retcode;

}

