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
/* Module Name: QMGR.C                                                       */
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

/* move these includes up for Solaris */
#include <sys/types.h>
#include  <time.h>
/* INCLUDE FOR TIME FUNCTION */
#if (MQAT_DEFAULT != MQAT_OS400)
#include <sys/timeb.h>
#endif

/* Include Application headers */
#include "saveqmgr.h"
#include "args.h"
#include "mqutils.h"

/* ------------------------------------------------------------------------ */
/* ProcessQMgrStringParm()                                                  */
/* ProcessQMgrIntegerParm()                                                 */
/*                                                                          */
/* These two processes are given a pointer to a block of storage that       */
/* contains the definition of a single attribute relating to the            */
/* queue manager. The first handles string parameters, the second integer.  */
/* The calling process has already decided which type of parameter this is  */
/* and is responsible for managing the storage. These procedures decide     */
/* which of the possible attributes this one is and extract the value.      */
/*                                                                          */
/* Follows is a list of all attributes possible for a queue manager.        */
/*                                                                          */
/* The QMgrDefn structure contains entries for each of the attributes.      */
/* ------------------------------------------------------------------------ */
extern int noisy;
extern int diagnostics;
extern int header; /* v513 */
extern int SysObjs; /* v6 */
extern int oneLine; /* v601 -- whether to write the entire DEFINE on oneline */
extern char lineTerm[3]; /* v601 -- line terminator */
extern int platform; /* v602 -- platform to generate MQSC for */
extern int qsgQmgr; /* v602 ..is this a qsg qmgr ? (set in qmgr.c) */
int firstTime = 1; /* so we only put out one banner */

void ProcessQMgrStringParm(MQCFST *pPCFString, QMgrParms *QMgrDefn, MQLONG *iCC) {
	/* Initialise return codes */
	*iCC = MQCC_OK;

	switch (pPCFString->Parameter) {
	case MQCA_Q_MGR_NAME:
		memset(QMgrDefn->QMgrName, ' ', MQ_Q_MGR_NAME_LENGTH);
		memcpy(QMgrDefn->QMgrName, pPCFString->String, pPCFString->StringLength);
		break;
	case MQCA_Q_MGR_DESC:
		memset(QMgrDefn->QMgrDesc, ' ', MQ_Q_MGR_DESC_LENGTH);
		memcpy(QMgrDefn->QMgrDesc, pPCFString->String, pPCFString->StringLength);
		break;
	case MQCA_DEAD_LETTER_Q_NAME:
		memset(QMgrDefn->DeadLetterQName, ' ', MQ_Q_NAME_LENGTH);
		memcpy(QMgrDefn->DeadLetterQName, pPCFString->String,
				pPCFString->StringLength);
		break;
	case MQCA_COMMAND_INPUT_Q_NAME:
		memset(QMgrDefn->CommandInputQName, ' ', MQ_Q_NAME_LENGTH);
		memcpy(QMgrDefn->CommandInputQName, pPCFString->String,
				pPCFString->StringLength);
		break;
	case MQCA_DEF_XMIT_Q_NAME:
		memset(QMgrDefn->DefXmitQName, ' ', MQ_Q_NAME_LENGTH);
		memcpy(QMgrDefn->DefXmitQName, pPCFString->String,
				pPCFString->StringLength);
		break;
#if defined (MQCA_CHANNEL_AUTO_DEF_EXIT)
	case MQCA_CHANNEL_AUTO_DEF_EXIT:
		memset(QMgrDefn->ChannelAutoDefExit, ' ', MQ_EXIT_NAME_LENGTH);
		memcpy(QMgrDefn->ChannelAutoDefExit, pPCFString->String,
				pPCFString->StringLength);
		break;
#endif
#if defined (MQCA_CLUSTER_WORKLOAD_EXIT)
	case MQCA_CLUSTER_WORKLOAD_EXIT:
		memset(QMgrDefn->WorkLoadExit, ' ', MQ_EXIT_NAME_LENGTH);
		memcpy(QMgrDefn->WorkLoadExit, pPCFString->String,
				pPCFString->StringLength);
		break;
#endif
#if defined (MQCA_CLUSTER_WORKLOAD_DATA)
	case MQCA_CLUSTER_WORKLOAD_DATA:
		memset(QMgrDefn->WorkLoadData, ' ', MQ_EXIT_DATA_LENGTH);
		memcpy(QMgrDefn->WorkLoadData, pPCFString->String,
				pPCFString->StringLength);
		break;
#endif
#if defined (MQCA_REPOSITORY_NAME)
	case MQCA_REPOSITORY_NAME:
		memset(QMgrDefn->RepositoryName, ' ', MQ_OBJECT_NAME_LENGTH);
		memcpy(QMgrDefn->RepositoryName, pPCFString->String,
				pPCFString->StringLength);
		break;
#endif
#if defined (MQCA_REPOSITORY_NAMELIST)
	case MQCA_REPOSITORY_NAMELIST:
		memset(QMgrDefn->RepositoryNameList, ' ', MQ_OBJECT_NAME_LENGTH);
		memcpy(QMgrDefn->RepositoryNameList, pPCFString->String,
				pPCFString->StringLength);
		break;
#endif
#if defined (MQCA_ALTERATION_DATE)
	case MQCA_CREATION_DATE:
		memset(QMgrDefn->CreationDate, ' ', MQ_CREATION_DATE_LENGTH);
		memcpy(QMgrDefn->CreationDate, pPCFString->String,
				pPCFString->StringLength);
		break;
	case MQCA_CREATION_TIME:
		memset(QMgrDefn->CreationTime, ' ', MQ_CREATION_TIME_LENGTH);
		memcpy(QMgrDefn->CreationTime, pPCFString->String,
				pPCFString->StringLength);
		break;
	case MQCA_ALTERATION_DATE:
		memset(QMgrDefn->AlterationDate, ' ', MQ_CREATION_DATE_LENGTH);
		memcpy(QMgrDefn->AlterationDate, pPCFString->String,
				pPCFString->StringLength);
		break;
	case MQCA_ALTERATION_TIME:
		memset(QMgrDefn->AlterationTime, ' ', MQ_CREATION_TIME_LENGTH);
		memcpy(QMgrDefn->AlterationTime, pPCFString->String,
				pPCFString->StringLength);
		break;
#endif
#if defined (MQCA_Q_MGR_IDENTIFIER)
	case MQCA_Q_MGR_IDENTIFIER:
		memset(QMgrDefn->QMgrId, ' ', MQ_Q_MGR_IDENTIFIER_LENGTH);
		memcpy(QMgrDefn->QMgrId, pPCFString->String, pPCFString->StringLength);
		break;
#endif
#if defined (MQCA_SSL_CRL_NAMELIST)
	case MQCA_SSL_CRL_NAMELIST:
		memset(QMgrDefn->SslCrlNameList, ' ', MQ_OBJECT_NAME_LENGTH);
		memcpy(QMgrDefn->SslCrlNameList, pPCFString->String,
				pPCFString->StringLength);
		break;
	case MQCA_SSL_KEY_REPOSITORY:
		memset(QMgrDefn->SslKeyRepos, ' ', MQ_SSL_KEY_REPOSITORY_LENGTH);
		memcpy(QMgrDefn->SslKeyRepos, pPCFString->String,
				pPCFString->StringLength);
		break;
	case MQCA_SSL_CRYPTO_HARDWARE:
		memset(QMgrDefn->SslCryptoHW, ' ', MQ_SSL_CRYPTO_HARDWARE_LENGTH);
		memcpy(QMgrDefn->SslCryptoHW, pPCFString->String,
				pPCFString->StringLength);
		break;
#endif

#if defined(zzMQV70)      
	case MQCA_PARENT:
		memset(QMgrDefn->ParentQMgr, ' ', MQ_Q_MGR_NAME_LENGTH);
		memcpy(QMgrDefn->ParentQMgr, pPCFString->String,
				pPCFString->StringLength);
		break;

#endif

#if defined(zzzOS)
		/* v602 zOS attributes */
	case MQBACF_RESPONSE_ID: /* zOS things to ignore */
		break;
	case MQCACF_RESPONSE_Q_MGR_NAME: /* zOS things to ignore */
		break;

	case MQCA_IGQ_USER_ID:
		memset(QMgrDefn->igqUserId, ' ', MQ_USER_ID_LENGTH);
		memcpy(QMgrDefn->igqUserId, pPCFString->String,
				pPCFString->StringLength);
		break;
	case MQCA_TCP_NAME:
		memset(QMgrDefn->tcpName, ' ', ZZSTRLEN);
		memcpy(QMgrDefn->tcpName, pPCFString->String, pPCFString->StringLength);
		break;
	case MQCA_DNS_GROUP:
		memset(QMgrDefn->dnsGroup, ' ', ZZSTRLEN);
		memcpy(QMgrDefn->dnsGroup, pPCFString->String, pPCFString->StringLength);
		break;
	case MQCA_CHINIT_SERVICE_PARM:
		memset(QMgrDefn->chinitServiceParm, ' ', ZZSTRLEN);
		memcpy(QMgrDefn->chinitServiceParm, pPCFString->String,
				pPCFString->StringLength);
		break;
	case MQCA_LU_GROUP_NAME:
		memset(QMgrDefn->luGroupName, ' ', ZZSTRLEN);
		memcpy(QMgrDefn->luGroupName, pPCFString->String,
				pPCFString->StringLength);
		break;
	case MQCA_LU_NAME:
		memset(QMgrDefn->luName, ' ', ZZSTRLEN);
		memcpy(QMgrDefn->luName, pPCFString->String, pPCFString->StringLength);
		break;
	case MQCA_LU62_ARM_SUFFIX:
		memset(QMgrDefn->luArmSuffix, ' ', ZZSTRLEN);
		memcpy(QMgrDefn->luArmSuffix, pPCFString->String,
				pPCFString->StringLength);
		break;
	case MQCA_QSG_NAME:
		memset(QMgrDefn->qsgName, ' ', MQ_QSG_NAME_LENGTH);
		memcpy(QMgrDefn->qsgName, pPCFString->String, pPCFString->StringLength);
		if (QMgrDefn->qsgName[0] != ' ') /* Are we in a QSG? */
			qsgQmgr = 1; /* Yes */
		break;

#endif  /* zzzOS parms */

	default:
		if (noisy)
			fprintf(stderr, "(qmgr.c) Unknown PCF String Parameter: %ld\n",
			pPCFString->Parameter);
		*iCC = WARNING;
		break;
	} /* endswitch */

	return;
}

void ProcessQMgrIntegerParm(MQCFIN *pPCFInteger, QMgrParms *QMgrDefn,
		MQLONG *iCC) {
	/* Initialise Completion and Reason Codes */
	*iCC = MQCC_OK;

	switch (pPCFInteger->Parameter) {
	case MQIA_PLATFORM:
		QMgrDefn->Platform = pPCFInteger->Value;
		/* if the user hasn't specified a platform option, then take */
		/* it from the qmgr itself */
		if (platform == 0) {
			platform = pPCFInteger->Value;
		}
		break;
	case MQIA_COMMAND_LEVEL:
		QMgrDefn->CommandLevel = pPCFInteger->Value;
		break;
	case MQIA_TRIGGER_INTERVAL:
		QMgrDefn->TriggerInterval = pPCFInteger->Value;
		break;
	case MQIA_MAX_PRIORITY:
		QMgrDefn->MaxPriority = pPCFInteger->Value;
		break;
	case MQIA_CODED_CHAR_SET_ID:
		QMgrDefn->CodedCharSetId = pPCFInteger->Value;
		break;
	case MQIA_MAX_HANDLES:
		QMgrDefn->MaxHandles = pPCFInteger->Value;
		break;
	case MQIA_MAX_MSG_LENGTH:
		QMgrDefn->MaxMsgLength = pPCFInteger->Value;
		break;
	case MQIA_MAX_UNCOMMITTED_MSGS:
		QMgrDefn->MaxUncommittedMsgs = pPCFInteger->Value;
		break;
	case MQIA_SYNCPOINT:
		QMgrDefn->SyncPoint = pPCFInteger->Value;
		break;
	case MQIA_AUTHORITY_EVENT:
		QMgrDefn->AuthorityEvent = pPCFInteger->Value;
		break;
	case MQIA_INHIBIT_EVENT:
		QMgrDefn->InhibitEvent = pPCFInteger->Value;
		break;
	case MQIA_LOCAL_EVENT:
		QMgrDefn->LocalEvent = pPCFInteger->Value;
		break;
	case MQIA_REMOTE_EVENT:
		QMgrDefn->RemoteEvent = pPCFInteger->Value;
		break;
	case MQIA_START_STOP_EVENT:
		QMgrDefn->StartStopEvent = pPCFInteger->Value;
		break;
	case MQIA_PERFORMANCE_EVENT:
		QMgrDefn->PerformanceEvent = pPCFInteger->Value;
		break;
#if defined (MQIA_CHANNEL_AUTO_DEF)
	case MQIA_CHANNEL_AUTO_DEF:
		QMgrDefn->ChannelAutoDef = pPCFInteger->Value;
		break;
#endif
#if defined (MQIA_CHANNEL_AUTO_DEF_EVENT)
	case MQIA_CHANNEL_AUTO_DEF_EVENT:
		QMgrDefn->ChannelAutoDefEvent = pPCFInteger->Value;
		break;
#endif
#if defined (MQIA_DIST_LISTS)
	case MQIA_DIST_LISTS:
		QMgrDefn->DistLists = pPCFInteger->Value;
		break;
#endif
#if defined (MQIA_CLUSTER_WORKLOAD_LENGTH)
	case MQIA_CLUSTER_WORKLOAD_LENGTH:
		QMgrDefn->WorkLoadLength = pPCFInteger->Value;
		break;
#endif
#ifdef zzMQV60
	case MQIA_STATISTICS_Q:
		QMgrDefn->StatQ = pPCFInteger->Value;
		break;
	case MQIA_CHANNEL_EVENT:
		QMgrDefn->ChannelEvent = pPCFInteger->Value;
		break;
	case MQIA_SSL_EVENT:
		QMgrDefn->SSLEvent = pPCFInteger->Value;
		break;
	case MQIA_SSL_FIPS_REQUIRED:
		QMgrDefn->SSLFIPSReqd = pPCFInteger->Value;
		break;
	case MQIA_IP_ADDRESS_VERSION:
		QMgrDefn->IPAddressVersion = pPCFInteger->Value;
		break;
	case MQIA_LOGGER_EVENT:
		QMgrDefn->LoggerEvent = pPCFInteger->Value;
		break;
	case MQIA_CHINIT_CONTROL:
		QMgrDefn->ChinitCtl = pPCFInteger->Value;
		break;
	case MQIA_CMD_SERVER_CONTROL:
		QMgrDefn->CmdSvrCtl = pPCFInteger->Value;
		break;
	case MQIA_STATISTICS_MQI:
		QMgrDefn->StatsMQI = pPCFInteger->Value;
		break;
	case MQIA_STATISTICS_CHANNEL:
		QMgrDefn->StatsChl = pPCFInteger->Value;
		break;
	case MQIA_STATISTICS_AUTO_CLUSSDR:
		QMgrDefn->StatsAutoClsr = pPCFInteger->Value;
		break;
	case MQIA_STATISTICS_INTERVAL:
		QMgrDefn->StatsIntv = pPCFInteger->Value;
		break;
	case MQIA_ACCOUNTING_MQI:
		QMgrDefn->ActngMQI = pPCFInteger->Value;
		break;
	case MQIA_ACCOUNTING_Q:
		QMgrDefn->ActngQ = pPCFInteger->Value;
		break;
	case MQIA_ACCOUNTING_INTERVAL:
		QMgrDefn->ActngInt = pPCFInteger->Value;
		break;
	case MQIA_ACCOUNTING_CONN_OVERRIDE:
		QMgrDefn->ActngConnOver = pPCFInteger->Value;
		break;
	case MQIA_TRACE_ROUTE_RECORDING:
		QMgrDefn->TrcRteRecord = pPCFInteger->Value;
		break;
	case MQIA_ACTIVITY_RECORDING:
		QMgrDefn->ActivityRecord = pPCFInteger->Value;
		break;
	case MQIA_MONITORING_CHANNEL:
		QMgrDefn->MonChl = pPCFInteger->Value;
		break;
	case MQIA_MONITORING_Q:
		QMgrDefn->MonQ = pPCFInteger->Value;
		break;
	case MQIA_MONITORING_AUTO_CLUSSDR:
		QMgrDefn->MonAutoClsr = pPCFInteger->Value;
		break;
	case MQIA_CLWL_MRU_CHANNELS:
		QMgrDefn->ClwlMRUChl = pPCFInteger->Value;
		break;
	case MQIA_CLWL_USEQ:
		QMgrDefn->ClwlUseQ = pPCFInteger->Value;
		break;
	case MQIA_SSL_RESET_COUNT:
		QMgrDefn->SSLResetCnt = pPCFInteger->Value;
		break;
#endif  /* v60 */

#if defined(zzMQV70)      
	case MQIA_MAX_PROPERTIES_LENGTH:
		QMgrDefn->MaxPropsLen = pPCFInteger->Value;
		break;
	case MQIA_PUBSUB_MODE:
		QMgrDefn->PubSubMode = pPCFInteger->Value;
		break;
	case MQIA_PUBSUB_MAXMSG_RETRY_COUNT:
		QMgrDefn->PubSubRtyCnt = pPCFInteger->Value;
		break;
	case MQIA_PUBSUB_NP_MSG:
		QMgrDefn->PubSubNPMsg = pPCFInteger->Value;
		break;
	case MQIA_PUBSUB_NP_RESP:
		QMgrDefn->PubSubNPResp = pPCFInteger->Value;
		break;
	case MQIA_PUBSUB_SYNC_PT:
		QMgrDefn->PubSubSyncPt = pPCFInteger->Value;
		break;
	case MQIA_MSG_MARK_BROWSE_INTERVAL:
		QMgrDefn->MkBrInt = pPCFInteger->Value;
		break;
	case MQIA_TREE_LIFE_TIME:
		QMgrDefn->Treelife = pPCFInteger->Value;
		break;
#endif /* v70 */

#if defined(zzzOS)
	case MQIA_EXPIRY_INTERVAL:
		QMgrDefn-> expiryInterval = pPCFInteger->Value;
		break;
	case MQIA_CONFIGURATION_EVENT:
		QMgrDefn-> configurationEvent = pPCFInteger->Value;
		break;
	case MQIA_COMMAND_EVENT:
		QMgrDefn-> commandEvent = pPCFInteger->Value;
		break;
	case MQIA_BRIDGE_EVENT:
		QMgrDefn-> bridgeEvent = pPCFInteger->Value;
		break;
	case MQIA_SHARED_Q_Q_MGR_NAME:
		QMgrDefn-> sharedQQmgrName = pPCFInteger->Value;
		break;
	case MQIA_SSL_TASKS:
		QMgrDefn->sslTasks = pPCFInteger->Value;
		break;
	case MQIA_INTRA_GROUP_QUEUING:
		QMgrDefn->igq = pPCFInteger->Value;
		break;
	case MQIA_IGQ_PUT_AUTHORITY:
		QMgrDefn->igqPutAuth = pPCFInteger->Value;
		break;
	case MQIA_CHINIT_ADAPTERS:
		QMgrDefn->chinitAdapters = pPCFInteger->Value;
		break;
	case MQIA_CHINIT_DISPATCHERS:
		QMgrDefn->chinitDispatchers = pPCFInteger->Value;
		break;
	case MQIA_TCP_CHANNELS:
		QMgrDefn->tcpChannels = pPCFInteger->Value;
		break;
	case MQIA_TCP_KEEP_ALIVE:
		QMgrDefn->tcpKeepAlive = pPCFInteger->Value;
		break;
	case MQIA_TCP_STACK_TYPE:
		QMgrDefn->tcpStack = pPCFInteger->Value;
		break;
	case MQIA_OUTBOUND_PORT_MIN:
		QMgrDefn->outBoundPortMin = pPCFInteger->Value;
		break;
	case MQIA_OUTBOUND_PORT_MAX:
		QMgrDefn->outBoundPortMax = pPCFInteger->Value;
		break;
	case MQIA_DNS_WLM:
		QMgrDefn->dnsWlm = pPCFInteger->Value;
		break;
	case MQIA_RECEIVE_TIMEOUT:
		QMgrDefn->receiveTimeout = pPCFInteger->Value;
		break;
	case MQIA_RECEIVE_TIMEOUT_TYPE:
		QMgrDefn->receiveTimeoutType = pPCFInteger->Value;
		break;
	case MQIA_RECEIVE_TIMEOUT_MIN:
		QMgrDefn->receiveTimeoutMin = pPCFInteger->Value;
		break;
	case MQIA_ADOPTNEWMCA_CHECK:
		QMgrDefn->adoptNewMcaCheck = pPCFInteger->Value;
		break;
	case MQIA_ADOPTNEWMCA_TYPE:
		QMgrDefn->adoptNewMcaType = pPCFInteger->Value;
		break;
	case MQIA_CHINIT_TRACE_TABLE_SIZE:
		QMgrDefn->chinitTraceTableSize = pPCFInteger->Value;
		break;
	case MQIA_CHINIT_TRACE_AUTO_START:
		QMgrDefn->chinitTraceAutoStart = pPCFInteger->Value;
		break;
	case MQIA_LU62_CHANNELS:
		QMgrDefn->lu62Channels = pPCFInteger->Value;
		break;
	case MQIA_MAX_CHANNELS:
		QMgrDefn->maxChannels = pPCFInteger->Value;
		break;
	case MQIA_ACTIVE_CHANNELS:
		QMgrDefn->activeChannels = pPCFInteger->Value;
		break;
	case MQIA_LISTENER_TIMER:
		QMgrDefn->listenerTimer = pPCFInteger->Value;
		break;
	case MQIA_CPI_LEVEL:
		QMgrDefn->cpiLevel = pPCFInteger->Value;
		break;
#ifdef zzMQV70
	case MQIA_SECURITY_CASE:
		QMgrDefn->securityCase = pPCFInteger->Value;
		break;
#endif
#endif  /* zzzOS */
	default:
		if (noisy)
			fprintf(stderr, "(qmgr.c) Unknown PCF Integer Parameter: %ld\n",
			pPCFInteger->Parameter);
		*iCC = WARNING;
		break;
	} /* endswitch */

}

/* ------------------------------------------------------------------------ */
/*                                                                          */
/* This process takes the attributes of the local queue manager and adds    */
/* them to the end of the output file.                                      */
/*                                                                          */
/* The file is of a format suitable for subsequent input to RUNMQSC.        */
/*                                                                          */
/* ------------------------------------------------------------------------ */
void AddToFileQMGR(QMgrParms *pQMgrDefn, /* Structure containing attrs */
int UserVersion, /* Indicates which version of mqsc */
char * fileName, /* name of output file */
FILE * fp, /* file handle */
int twolines, /* v601 */
MQLONG *iCC) /* return code */
{
	char * ParmBuffer; /* Temporary buffer to hold for output to file */
	time_t ltime;
	struct tm *today;
	char work[256];
	int options; /* Option flag for MQsprintf_X */

	/* Initialise Completion and Reason Codes */
	*iCC = MQCC_OK;

	/* v601 setup options for oneLine operation */
	options = MQSPRINTF_X_DOUBLE_QUOTES + MQSPRINTF_X_NO_TRAILING_BLANKS;
	if (!oneLine)
		options += MQSPRINTF_X_LINE_BREAKS;

	if ( !(ParmBuffer = (char *) malloc(PBSIZE) )) {
		fprintf(stderr, "(qmgr.c) Failed to allocate %d bytes \n", PBSIZE);
		exit(2);
	}

	{
		int ver = UserVersion / 100;
		int rel = (UserVersion - (ver * 100)) / 10;
		int mod = UserVersion - (ver * 100) - (rel * 10);
		fprintf(stderr,
		"Generating attributes for Websphere MQ Release %d.%d.%d\n",
		ver, rel, mod);
	}
	/* does the user want a nice header ? */

	if (header && firstTime) {
		firstTime = 0;
		work[0] = '\0';
		if (oneLine) {
			if (twolines)
				strcpy(work, " using the TWOLINE (-2) option");
			else
				strcpy(work, " using the ONELINE (-1) option");
		} else {
			fputs("*\n", fp);
		}
		time( &ltime);
		today = localtime( &ltime);

		fprintf(
				fp,
				"* This file generated by %s on %.4d-%.2d-%.2d at %.2d.%.2d.%.2d hours%s.\n", 
				THISVERSION, (today->tm_year) + 1900, today -> tm_mon+1,
				today -> tm_mday, today -> tm_hour, today -> tm_min,
				today -> tm_sec, work);
	}
	if (!oneLine || twolines) {

		MQsprintf_X(ParmBuffer, "*\n* QMNAME (%s) ", pQMgrDefn->QMgrName, 
		MQ_Q_MGR_NAME_LENGTH, options);
		fputs(ParmBuffer, fp);
	}
	/*  [v6] okay, if the user doesn't want system objects, then we shouldn't */
	/*  [v6] be altering the qmgr, since that is a system object              */
	if (diagnostics>2) 
		fprintf(stderr, "Printing qmgr info is %d\n", ( ((SysObjs & SO_ALL) ) || (SysObjs & SO_QMGR)));
	if ( (SysObjs & SO_ALL) || (SysObjs & SO_QMGR)) {
		if (!oneLine || twolines) {
			

#if defined(MQ_Q_MGR_IDENTIFIER_LENGTH)
			MQsprintf_X(ParmBuffer, "* CRDATE (%s) ", pQMgrDefn->CreationDate, 
			MQ_CREATION_DATE_LENGTH, options);
			fputs(ParmBuffer, fp);
			MQsprintf_X(ParmBuffer, "* CRTIME (%s) ", pQMgrDefn->CreationTime, 
			MQ_CREATION_TIME_LENGTH, options);
			fputs(ParmBuffer, fp);

			if (header) {
				MQsprintf_X(ParmBuffer, "* ALTDATE (%s) ",
						pQMgrDefn->AlterationDate, 
						MQ_CREATION_DATE_LENGTH, options);
				fputs(ParmBuffer, fp);
				MQsprintf_X(ParmBuffer, "* ALTTIME (%s) ",
						pQMgrDefn->AlterationTime, 
						MQ_CREATION_TIME_LENGTH, options);
				fputs(ParmBuffer, fp);
			}
			MQsprintf_X(ParmBuffer, "* QMID (%s) ", pQMgrDefn->QMgrId, 
			MQ_Q_MGR_IDENTIFIER_LENGTH, options);
			fputs(ParmBuffer, fp);
#endif

			sprintf(ParmBuffer, "* CMDLEVEL (%ld) %s", pQMgrDefn->CommandLevel,
					lineTerm);
			fputs(ParmBuffer, fp);

#if defined (MQIA_DIST_LISTS)
			if (pQMgrDefn->DistLists == MQDL_SUPPORTED)
				sprintf(ParmBuffer, "* DISTL (YES) %s", lineTerm);
			else
				sprintf(ParmBuffer, "* DISTL (NO) %s", lineTerm);
			fputs(ParmBuffer, fp);
#endif

			sprintf(ParmBuffer, "* MAXPRTY (%ld) %s", pQMgrDefn->MaxPriority,
					lineTerm);
			fputs(ParmBuffer, fp);

			toStrMQPLATFORM((char*)&work, pQMgrDefn->Platform);
			sprintf(ParmBuffer, "* PLATFORM (%s) %s", work, lineTerm);
			fputs(ParmBuffer, fp);

#if defined(MQSP_AVAILABLE)
			if (pQMgrDefn->SyncPoint == MQSP_AVAILABLE) {
				sprintf(ParmBuffer, "* SYNCPT %s", lineTerm);
				fputs(ParmBuffer, fp);
			}
#endif

			MQsprintf_X(ParmBuffer, "* COMMANDQ (%s) ",
					pQMgrDefn->CommandInputQName, MQ_Q_NAME_LENGTH, options);
			fputs(ParmBuffer, fp);

#if defined(zzzOS)
			if (platform==MQPL_MVS) {

				sprintf(ParmBuffer, "* CPILEVEL (%ld) %s", pQMgrDefn->cpiLevel,
						lineTerm);
				fputs(ParmBuffer, fp);

				MQsprintf_X(ParmBuffer, "* CHISERVP('%s') ",
						pQMgrDefn->chinitServiceParm, ZZSTRLEN, options);
				fputs(ParmBuffer, fp);
				MQsprintf_X(ParmBuffer, "* QSGNAME('%s') ", pQMgrDefn->qsgName, 
				MQ_QSG_NAME_LENGTH, options);
				fputs(ParmBuffer, fp);

			} /* end of platform = zos */

#endif /* zzzOS */

			if (!oneLine)
				fputs("\n", fp);

		} /* end of !oneLine */

		sprintf(ParmBuffer, "ALTER  QMGR %s", lineTerm);
		fputs(ParmBuffer, fp);

		toStrMQEVR((char*)&work, pQMgrDefn->AuthorityEvent);
		sprintf(ParmBuffer, "       AUTHOREV(%s) %s", work, lineTerm);
		fputs(ParmBuffer, fp);

		MQsprintf_X(ParmBuffer, "       DEADQ('%s') ",
				pQMgrDefn->DeadLetterQName, MQ_Q_NAME_LENGTH, options);
		fputs(ParmBuffer, fp);

		MQsprintf_X(ParmBuffer, "       DEFXMITQ('%s') ",
				pQMgrDefn->DefXmitQName, MQ_Q_NAME_LENGTH, options);
		fputs(ParmBuffer, fp);

		MQsprintf_X(ParmBuffer, "       DESCR('%s') ", pQMgrDefn->QMgrDesc, 
		MQ_Q_MGR_DESC_LENGTH, options);
		fputs(ParmBuffer, fp);

		toStrMQEVR((char*)&work, pQMgrDefn->InhibitEvent);
		sprintf(ParmBuffer, "       INHIBTEV(%s) %s", work, lineTerm);
		fputs(ParmBuffer, fp);

		toStrMQEVR((char*)&work, pQMgrDefn->LocalEvent);
		sprintf(ParmBuffer, "       LOCALEV(%s) %s", work, lineTerm);
		fputs(ParmBuffer, fp);

		sprintf(ParmBuffer, "       MAXHANDS(%ld) %s", pQMgrDefn->MaxHandles,
				lineTerm);
		fputs(ParmBuffer, fp);

		sprintf(ParmBuffer, "       MAXUMSGS(%ld) %s",
				pQMgrDefn->MaxUncommittedMsgs, lineTerm);
		fputs(ParmBuffer, fp);

		toStrMQEVR((char*)&work, pQMgrDefn->PerformanceEvent);
		sprintf(ParmBuffer, "       PERFMEV(%s) %s", work, lineTerm);
		fputs(ParmBuffer, fp);

		toStrMQEVR((char*)&work, pQMgrDefn->RemoteEvent);
		sprintf(ParmBuffer, "       REMOTEEV(%s) %s", work, lineTerm);
		fputs(ParmBuffer, fp);

		toStrMQEVR((char*)&work, pQMgrDefn->StartStopEvent);
		sprintf(ParmBuffer, "       STRSTPEV(%s) %s", work, lineTerm);
		fputs(ParmBuffer, fp);

		sprintf(ParmBuffer, "       TRIGINT(%ld) %s",
				pQMgrDefn->TriggerInterval, lineTerm);
		fputs(ParmBuffer, fp);

		/* start V5 attributes */
#if defined (MQCHAD_DISABLED)
		if (UserVersion>=MQCMDL_LEVEL_500) {
			if (platform!=MQPL_MVS) {
				sprintf(ParmBuffer, "       MAXMSGL(%ld) %s",
						pQMgrDefn->MaxMsgLength, lineTerm);
			} else {
				sprintf(ParmBuffer, "*      MAXMSGL(%ld) %s",
						pQMgrDefn->MaxMsgLength, lineTerm);
			}
			fputs(ParmBuffer, fp);

			if (platform!=MQPL_MVS) {
				toStrMQEVR((char*)&work, pQMgrDefn->ChannelAutoDef);
				sprintf(ParmBuffer, "       CHAD(%s) %s", work, lineTerm);
				fputs(ParmBuffer, fp);

				toStrMQEVR((char*)&work, pQMgrDefn->ChannelAutoDefEvent);
				sprintf(ParmBuffer, "       CHADEV(%s) %s", work, lineTerm);
				fputs(ParmBuffer, fp);
			}

			MQsprintf_X(ParmBuffer, "       CHADEXIT('%s') ",
					pQMgrDefn->ChannelAutoDefExit, MQ_EXIT_NAME_LENGTH, options);
			fputs(ParmBuffer, fp);
		}

		if (UserVersion >= MQCMDL_LEVEL_500 && platform != MQPL_MVS)
			sprintf(ParmBuffer, "       CCSID(%ld) %s",
					pQMgrDefn->CodedCharSetId, lineTerm);
		else
			sprintf(ParmBuffer, "*      CCSID(%ld) %s",
					pQMgrDefn->CodedCharSetId, lineTerm);

		fputs(ParmBuffer, fp);

#endif

		/* start V5.1 attributes */
#if defined (MQ_OBJECT_NAME_LENGTH)
		if (UserVersion>=MQCMDL_LEVEL_510) {
			MQsprintf_X(ParmBuffer, "       CLWLEXIT('%s') ",
					pQMgrDefn->WorkLoadExit, MQ_EXIT_NAME_LENGTH, options);
			fputs(ParmBuffer, fp);
			MQsprintf_X(ParmBuffer, "       CLWLDATA('%s') ",
					pQMgrDefn->WorkLoadData, MQ_EXIT_DATA_LENGTH, options);
			fputs(ParmBuffer, fp);
			MQsprintf_X(ParmBuffer, "       REPOS('%s') ",
					pQMgrDefn->RepositoryName, MQ_OBJECT_NAME_LENGTH, options);
			fputs(ParmBuffer, fp);
			MQsprintf_X(ParmBuffer, "       REPOSNL('%s') ",
					pQMgrDefn->RepositoryNameList, MQ_OBJECT_NAME_LENGTH,
					options);
			fputs(ParmBuffer, fp);
			sprintf(ParmBuffer, "       CLWLLEN(%ld) %s",
					pQMgrDefn->WorkLoadLength, lineTerm);
			fputs(ParmBuffer, fp);

		}
#endif
		/* start of mq v5.3 attributes */
#if defined (MQCMD_INQUIRE_AUTH_INFO)
		if (UserVersion>=MQCMDL_LEVEL_530) {
			MQsprintf_X(ParmBuffer, "       SSLCRLNL ('%s') ",
					pQMgrDefn->SslCrlNameList, MQ_OBJECT_NAME_LENGTH, options);
			fputs(ParmBuffer, fp);
			/**                                **/
			/** v530a, fixed length of SSLKEYR **/
			/**                                **/
			MQsprintf_X(ParmBuffer, "       SSLKEYR ('%s') ",
					pQMgrDefn->SslKeyRepos, MQ_SSL_KEY_REPOSITORY_LENGTH,
					options);
			fputs(ParmBuffer, fp);
			/* HW Crypto is only supported on Unix */
			/* if (MQAT_DEFAULT == MQAT_UNIX ) { */
			if (platform == MQPL_UNIX) {
				MQsprintf_X(ParmBuffer, "       SSLCRYP ('%s') ",
						pQMgrDefn->SslCryptoHW, MQ_SSL_CRYPTO_HARDWARE_LENGTH,
						options);
				fputs(ParmBuffer, fp);
			}

		}
#endif
		/* start of mq v6 attributes */
#if defined (zzMQV60)
		if (UserVersion>=MQCMDL_LEVEL_600) {

			toStrMQEVR((char*)&work, pQMgrDefn->ChannelEvent);
			sprintf(ParmBuffer, "       CHLEV(%s) %s", work, lineTerm);
			fputs(ParmBuffer, fp);

			toStrMQEVR((char*)&work, pQMgrDefn->SSLEvent);
			sprintf(ParmBuffer, "       SSLEV(%s) %s", work, lineTerm);
			fputs(ParmBuffer, fp);

			/* os/400 doesnt support sslfips */
			/* if (MQAT_DEFAULT != MQAT_OS400 ) { */
			if (platform != MQPL_OS400 && platform != MQPL_MVS) {
				sprintf(ParmBuffer, "       SSLFIPS(%s) %s",
						pQMgrDefn->SSLFIPSReqd ? "YES" : "NO", lineTerm);
				fputs(ParmBuffer, fp);
			}

			sprintf(ParmBuffer, "       IPADDRV(%s) %s",
					pQMgrDefn->IPAddressVersion==MQIPADDR_IPV4 ? "IPV4"
							: "IPV6", lineTerm);
			fputs(ParmBuffer, fp);

			if (platform!=MQPL_MVS) {
				toStrMQEVR((char*)&work, pQMgrDefn->LoggerEvent);
				sprintf(ParmBuffer, "       LOGGEREV(%s) %s", work, lineTerm);
				fputs(ParmBuffer, fp);

				toStrMQSVC((char*)&work , pQMgrDefn->ChinitCtl);
				sprintf(ParmBuffer, "       SCHINIT(%s) %s", work, lineTerm);
				fputs(ParmBuffer, fp);

				toStrMQSVC((char*)&work , pQMgrDefn->CmdSvrCtl);
				sprintf(ParmBuffer, "       SCMDSERV(%s) %s", work, lineTerm);
				fputs(ParmBuffer, fp);

				toStrMQMON((char*)&work, pQMgrDefn->StatQ);
				sprintf(ParmBuffer, "       STATQ(%s) %s", work, lineTerm);
				fputs(ParmBuffer, fp);

				toStrMQMON((char*)&work, pQMgrDefn->StatsMQI);
				sprintf(ParmBuffer, "       STATMQI(%s) %s", work, lineTerm);
				fputs(ParmBuffer, fp);

				toStrMQMON((char*)&work, pQMgrDefn->StatsChl);
				sprintf(ParmBuffer, "       STATCHL(%s) %s", work, lineTerm);
				fputs(ParmBuffer, fp);

				toStrMQMON((char*)&work, pQMgrDefn->StatsAutoClsr);
				sprintf(ParmBuffer, "       STATACLS(%s) %s", work, lineTerm);
				fputs(ParmBuffer, fp);

				sprintf(ParmBuffer, "       STATINT(%ld) %s", /*int*/
				pQMgrDefn->StatsIntv, lineTerm);
				fputs(ParmBuffer, fp);
			}

			toStrMQMON((char*)&work, pQMgrDefn->ActngQ);
			sprintf(ParmBuffer, "       ACCTQ(%s) %s", work, lineTerm);
			fputs(ParmBuffer, fp);

			if (platform != MQPL_MVS) {
				toStrMQMON((char*)&work, pQMgrDefn->ActngMQI);
				sprintf(ParmBuffer, "       ACCTMQI(%s) %s", work, lineTerm);
				fputs(ParmBuffer, fp);

				sprintf(ParmBuffer, "       ACCTINT(%ld) %s", /*int*/
				pQMgrDefn->ActngInt, lineTerm);
				fputs(ParmBuffer, fp);

				toStrMQEVR((char*)&work, pQMgrDefn->ActngConnOver);
				sprintf(ParmBuffer, "       ACCTCONO(%s) %s", work, lineTerm);
				fputs(ParmBuffer, fp);
			}

			toStrMQREC((char*)&work, pQMgrDefn->TrcRteRecord);
			sprintf(ParmBuffer, "       ROUTEREC(%s) %s", work, lineTerm);
			fputs(ParmBuffer, fp);

			toStrMQREC((char*)&work, pQMgrDefn->ActivityRecord);
			sprintf(ParmBuffer, "       ACTIVREC(%s) %s", work, lineTerm);
			fputs(ParmBuffer, fp);

			toStrMQMON((char*)&work, pQMgrDefn->MonChl);
			sprintf(ParmBuffer, "       MONCHL(%s) %s", work, lineTerm);
			fputs(ParmBuffer, fp);

			toStrMQMON((char*)&work, pQMgrDefn->MonQ);
			sprintf(ParmBuffer, "       MONQ(%s) %s", work, lineTerm);
			fputs(ParmBuffer, fp);

			toStrMQMON((char*)&work, pQMgrDefn->MonAutoClsr);
			sprintf(ParmBuffer, "       MONACLS(%s) %s", work, lineTerm);

			fputs(ParmBuffer, fp);
			sprintf(ParmBuffer, "       CLWLMRUC(%ld) %s", /*int*/
			pQMgrDefn->ClwlMRUChl, lineTerm);
			fputs(ParmBuffer, fp);

			toStrMQCLWL((char *)&work, pQMgrDefn->ClwlUseQ);
			sprintf(ParmBuffer, "       CLWLUSEQ(%s) %s", work, lineTerm);
			fputs(ParmBuffer, fp);

			sprintf(ParmBuffer, "       SSLRKEYC(%ld) %s", /*int*/
			pQMgrDefn->SSLResetCnt, lineTerm);
			fputs(ParmBuffer, fp);

		} /* end of if MQVer >= 600 */
#endif

#if defined(zzMQV70)
		if (UserVersion >= MQCMDL_LEVEL_700) {
			if (pQMgrDefn->MkBrInt> 0) {
				sprintf(ParmBuffer, "       MARKINT(%ld) %s", /*int*/
				pQMgrDefn->MkBrInt, lineTerm);
				fputs(ParmBuffer, fp);
			} else {
				sprintf(ParmBuffer, "       MARKINT(NOLIMIT) %s", lineTerm);
				fputs(ParmBuffer, fp);
			}

			if (pQMgrDefn->MaxPropsLen> 0) {
				sprintf(ParmBuffer, "       MAXPROPL(%ld) %s",
						pQMgrDefn->MaxPropsLen, lineTerm);
				fputs(ParmBuffer, fp);
			} else {
				sprintf(ParmBuffer, "       MAXPROPL(NOLIMIT) %s", lineTerm);
				fputs(ParmBuffer, fp);
			}

			MQsprintf_X(ParmBuffer, "       PARENT('%s') ",
					pQMgrDefn->ParentQMgr, 
					MQ_Q_MGR_NAME_LENGTH, options);
			fputs(ParmBuffer, fp);

			switch (pQMgrDefn->PubSubMode) {
			case MQPSM_COMPAT:
				sprintf(ParmBuffer, "       PSMODE(COMPAT) %s", lineTerm);
				fputs(ParmBuffer, fp);
				break;
			case MQPSM_DISABLED:
				sprintf(ParmBuffer, "       PSMODE(DISABLED) %s", lineTerm);
				fputs(ParmBuffer, fp);
				break;
			case MQPSM_ENABLED:
				sprintf(ParmBuffer, "       PSMODE(ENABLED) %s", lineTerm);
				fputs(ParmBuffer, fp);
				break;
			}

			sprintf(ParmBuffer, "       PSRTYCNT(%ld) %s",
					pQMgrDefn->PubSubRtyCnt, lineTerm);
			fputs(ParmBuffer, fp);

			switch (pQMgrDefn->PubSubNPMsg) {
			case MQUNDELIVERED_DISCARD:
				sprintf(ParmBuffer, "       PSNPMSG(DISCARD) %s", lineTerm);
				fputs(ParmBuffer, fp);
				break;
			case MQUNDELIVERED_KEEP:
				sprintf(ParmBuffer, "       PSNPMSG(KEEP) %s", lineTerm);
				fputs(ParmBuffer, fp);
				break;
			}

			switch (pQMgrDefn->PubSubNPResp) {
			case MQUNDELIVERED_NORMAL:
				sprintf(ParmBuffer, "       PSNPRES(NORMAL) %s", lineTerm);
				fputs(ParmBuffer, fp);
				break;
			case MQUNDELIVERED_SAFE:
				sprintf(ParmBuffer, "       PSNPRES(SAFE) %s", lineTerm);
				fputs(ParmBuffer, fp);
				break;
			case MQUNDELIVERED_DISCARD:
				sprintf(ParmBuffer, "       PSNPRES(DISCARD) %s", lineTerm);
				fputs(ParmBuffer, fp);
				break;
			case MQUNDELIVERED_KEEP:
				sprintf(ParmBuffer, "       PSNPRES(KEEP) %s", lineTerm);
				fputs(ParmBuffer, fp);
				break;
			}

			/* MQLONG PubSubSyncPt;  MQIA_PUBSUB_SYNC_PT (PSSYNCPT)*/
			switch (pQMgrDefn->PubSubSyncPt) {
			case MQSYNCPOINT_YES:
				sprintf(ParmBuffer, "       PSSYNCPT(YES) %s", lineTerm);
				fputs(ParmBuffer, fp);
				break;
			case MQSYNCPOINT_IFPER:
				sprintf(ParmBuffer, "       PSSYNCPT(IFPER) %s", lineTerm);
				fputs(ParmBuffer, fp);
				break;
			}

			sprintf(ParmBuffer, "       TREELIFE(%ld) %s", pQMgrDefn->Treelife,
					lineTerm);
			fputs(ParmBuffer, fp);
		}
#endif

#if defined(zzzOS)
		if (platform==MQPL_MVS) {

			if (pQMgrDefn->expiryInterval==MQEXPI_OFF) {
				sprintf(ParmBuffer, "       EXPRYINT(OFF) %s", lineTerm);
			} else {
				sprintf(ParmBuffer, "       EXPRYINT(%ld) %s",
						pQMgrDefn->expiryInterval, lineTerm);
			}
			fputs(ParmBuffer, fp);

			toStrMQEVR((char*)&work, pQMgrDefn->configurationEvent);
			sprintf(ParmBuffer, "       CONFIGEV(%s) %s", work, lineTerm);
			fputs(ParmBuffer, fp);

			toStrMQEVR((char*)&work, pQMgrDefn->commandEvent);
			sprintf(ParmBuffer, "       CMDEV(%s) %s", work, lineTerm);
			fputs(ParmBuffer, fp);

			toStrMQEVR((char*)&work, pQMgrDefn->bridgeEvent);
			sprintf(ParmBuffer, "       BRIDGEEV(%s) %s", work, lineTerm);
			fputs(ParmBuffer, fp);

			if (pQMgrDefn->sharedQQmgrName==MQSQQM_USE) {
				sprintf(ParmBuffer, "       SQQMNAME(USE) %s", lineTerm);
			} else {
				sprintf(ParmBuffer, "       SQQMNAME(IGNORE) %s", lineTerm);
			}
			fputs(ParmBuffer, fp);

			sprintf(ParmBuffer, "       SSLTASKS(%ld) %s", pQMgrDefn->sslTasks,
					lineTerm);
			fputs(ParmBuffer, fp);

			if (pQMgrDefn->igq==MQIGQ_ENABLED) {
				sprintf(ParmBuffer, "       IGQ(ENABLED) %s", lineTerm);
			} else {
				sprintf(ParmBuffer, "       IGQ(DISABLED)  %s", lineTerm);
			}
			fputs(ParmBuffer, fp);

			toStrIGQPA((char*)&work, pQMgrDefn->igqPutAuth);
			sprintf(ParmBuffer, "       IGQAUT(%s) %s", work, lineTerm);
			fputs(ParmBuffer, fp);

			sprintf(ParmBuffer, "       CHIADAPS(%ld) %s",
					pQMgrDefn->chinitAdapters, lineTerm);
			fputs(ParmBuffer, fp);

			sprintf(ParmBuffer, "       CHIDISPS(%ld) %s",
					pQMgrDefn->chinitDispatchers, lineTerm);
			fputs(ParmBuffer, fp);

			sprintf(ParmBuffer, "       TCPCHL(%ld) %s",
					pQMgrDefn->tcpChannels, lineTerm);
			fputs(ParmBuffer, fp);

			if (pQMgrDefn->tcpKeepAlive==MQTCPKEEP_YES) {
				sprintf(ParmBuffer, "       TCPKEEP(YES) %s", lineTerm);
			} else {
				sprintf(ParmBuffer, "       TCPKEEP(NO)  %s", lineTerm);
			}
			fputs(ParmBuffer, fp);

			if (pQMgrDefn->tcpStack==MQTCPSTACK_SINGLE) {
				sprintf(ParmBuffer, "       TCPSTACK(SINGLE) %s", lineTerm);
			} else {
				sprintf(ParmBuffer, "       TCPSTACK(MULTIPLE)  %s", lineTerm);
			}
			fputs(ParmBuffer, fp);

			sprintf(ParmBuffer, "       LSTRTMR(%ld) %s",
					pQMgrDefn->listenerTimer, lineTerm);
			fputs(ParmBuffer, fp);

			if (MQTRAXSTR_YES==pQMgrDefn->chinitTraceAutoStart) {
				sprintf(ParmBuffer, "       TRAXSTR(YES) %s", lineTerm);
			} else {
				sprintf(ParmBuffer, "       TRAXSTR(NO) %s", lineTerm);
			}
			fputs(ParmBuffer, fp);

			sprintf(ParmBuffer, "       TRAXTBL(%ld) %s",
					pQMgrDefn->chinitTraceTableSize, lineTerm);
			fputs(ParmBuffer, fp);

			sprintf(ParmBuffer, "       OPORTMIN(%ld) %s",
					pQMgrDefn->outBoundPortMin, lineTerm);
			fputs(ParmBuffer, fp);

			sprintf(ParmBuffer, "       OPORTMAX(%ld) %s",
					pQMgrDefn->outBoundPortMax, lineTerm);
			fputs(ParmBuffer, fp);

			sprintf(ParmBuffer, "       MAXCHL(%ld) %s",
					pQMgrDefn->maxChannels, lineTerm);
			fputs(ParmBuffer, fp);

			sprintf(ParmBuffer, "       ACTCHL(%ld) %s",
					pQMgrDefn->activeChannels, lineTerm);
			fputs(ParmBuffer, fp);

			sprintf(ParmBuffer, "       LU62CHL(%ld) %s",
					pQMgrDefn->lu62Channels, lineTerm);
			fputs(ParmBuffer, fp);

			MQsprintf_X(ParmBuffer, "       LUGROUP('%s') ",
					pQMgrDefn->luGroupName, ZZSTRLEN, options);
			fputs(ParmBuffer, fp);
			MQsprintf_X(ParmBuffer, "       LUNAME('%s') ", pQMgrDefn->luName, 
			ZZSTRLEN, options);
			fputs(ParmBuffer, fp);
			MQsprintf_X(ParmBuffer, "       LU62ARM('%s') ",
					pQMgrDefn->luArmSuffix, ZZSTRLEN, options);
			fputs(ParmBuffer, fp);

			if (pQMgrDefn->dnsWlm==MQDNSWLM_YES) {
				sprintf(ParmBuffer, "       DNSWLM(YES) %s", lineTerm);
			} else {
				sprintf(ParmBuffer, "       DNSWLM(NO)  %s", lineTerm);
			}
			fputs(ParmBuffer, fp);

			sprintf(ParmBuffer, "       RCVTIME(%ld) %s",
					pQMgrDefn->receiveTimeout, lineTerm);
			fputs(ParmBuffer, fp);
			sprintf(ParmBuffer, "       RCVTMIN(%ld) %s",
					pQMgrDefn->receiveTimeoutMin, lineTerm);
			fputs(ParmBuffer, fp);

			toStrRTT((char*)&work, pQMgrDefn->receiveTimeoutType);
			sprintf(ParmBuffer, "       RCVTTYPE(%s) %s", work, lineTerm);
			fputs(ParmBuffer, fp);

			toStrANMC((char*)&work, pQMgrDefn->adoptNewMcaCheck);
			sprintf(ParmBuffer, "       ADOPTCHK(%s) %s", work, lineTerm);
			fputs(ParmBuffer, fp);

			if (pQMgrDefn->adoptNewMcaType==MQADOPT_TYPE_ALL) {
				sprintf(ParmBuffer, "       ADOPTMCA(ALL) %s", lineTerm);
			} else {
				sprintf(ParmBuffer, "       ADOPTMCA(NO)  %s", lineTerm);
			}
			fputs(ParmBuffer, fp);

			MQsprintf_X(ParmBuffer, "       IGQUSER ('%s') ",
					pQMgrDefn->igqUserId, MQ_USER_ID_LENGTH, options);
			fputs(ParmBuffer, fp);
			MQsprintf_X(ParmBuffer, "       TCPNAME ('%s') ",
					pQMgrDefn->tcpName, ZZSTRLEN, options);
			fputs(ParmBuffer, fp);
			MQsprintf_X(ParmBuffer, "       DNSGROUP ('%s') ",
					pQMgrDefn->dnsGroup, ZZSTRLEN, options);
			fputs(ParmBuffer, fp);
#if defined(zzMQV70)
			if (UserVersion >= MQCMDL_LEVEL_700) {
				if (pQMgrDefn->securityCase == MQSCYC_UPPER) {
					sprintf(ParmBuffer, "       SCYCASE(UPPER) %s", lineTerm);
				} else if (pQMgrDefn->securityCase == MQSCYC_MIXED) {
					sprintf(ParmBuffer, "       SCYCASE(MIXED) %s", lineTerm);
				}
				fputs(ParmBuffer, fp);
			}
#endif
		} /* end of platform = mvs */
#endif   /* zzzOS */ 

		sprintf(ParmBuffer, "       FORCE\n");
		fputs(ParmBuffer, fp);

	} else { /* end of user DOES want system objects */

		sprintf(ParmBuffer, "** (Skipping QMGR Object) **\n");
		fputs(ParmBuffer, fp);

	}

	if (!oneLine) {
		fputs("\n", fp);
	}

	free(ParmBuffer);
	return;

}

