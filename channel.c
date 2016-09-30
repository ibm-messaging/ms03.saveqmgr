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
/* Module Name: CHANNEL.C                                                    */
/*  DESCRIPTIVE NAME WebSphere MQ Save Queue Manager Object                  */
/*                  Definitions using PCFs (ms03 supportpac)                 */
/*                                                                           */
/*===========================================================================*/

/* Include standard libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Include MQSeries headers */
#include <cmqc.h>
#include <cmqcfc.h>
#include <cmqxc.h>

/* Include Application headers */
#include "saveqmgr.h"
#include "mqutils.h"


/* ------------------------------------------------------------------------ */
/* ProcessChannelStringParm()                                               */
/* ProcessChannelIntegerParm()                                              */
/* ProcessChannelIntegerListParm()                                          */
/* ProcessChannelStringListParm()                                           */
/*                                                                          */
/*                                                                          */
/* The ChlDefn structure contains entries for each of the attributes,       */
/* only the attributes relevant for this channel type will be meaningful.   */
/* When we later write the file we will ensure that, based on channel type, */
/* only the relevant fields are used.                                       */
/* ------------------------------------------------------------------------ */
extern int noisy;
extern int chlReset;
extern int header;
extern int oneLine; /* v601 -- whether to write the entire DEFINE on oneline */
extern char lineTerm[3]; /* v601 -- line terminator */
extern int platform; /* v602 -- platform to generate MQSC for */

void ProcessChannelStringParm(MQCFST *pPCFString, ChlParms *ChlDefn,
		MQLONG *iCC) {
	/* Initialise return codes */
	*iCC = MQCC_OK;

	switch (pPCFString->Parameter) {
	case MQCACH_CHANNEL_NAME:
		memset(ChlDefn->ChannelName, ' ', MQ_CHANNEL_NAME_LENGTH);
		memcpy(ChlDefn->ChannelName, pPCFString->String,
				pPCFString -> StringLength);
		break;
	case MQCACH_MODE_NAME:
		memset(ChlDefn->ModeName, ' ', MQ_MODE_NAME_LENGTH);
		memcpy(ChlDefn->ModeName, pPCFString->String, pPCFString -> StringLength);
		break;
	case MQCACH_TP_NAME:
		memset(ChlDefn->TpName, ' ', MQ_TP_NAME_LENGTH);
		memcpy(ChlDefn->TpName, pPCFString->String, pPCFString -> StringLength);
		break;
	case MQCA_Q_MGR_NAME:
		memset(ChlDefn->QMgrName, ' ', MQ_Q_MGR_NAME_LENGTH);
		memcpy(ChlDefn->QMgrName, pPCFString->String, pPCFString -> StringLength);
		break;
	case MQCACH_XMIT_Q_NAME:
		memset(ChlDefn->XmitQName, ' ', MQ_Q_NAME_LENGTH);
		memcpy(ChlDefn->XmitQName, pPCFString->String,
				pPCFString -> StringLength);
		break;
	case MQCACH_CONNECTION_NAME:
		memset(ChlDefn->ConnectionName, ' ', MQ_CONN_NAME_LENGTH);
		memcpy(ChlDefn->ConnectionName, pPCFString->String,
				pPCFString -> StringLength);
		break;
		/* note that although we capture this attribute, it currently can only */
		/* have a value of blanks, so we never bother to generate a keyword */
	case MQCACH_MCA_NAME:
		memset(ChlDefn->MCAName, ' ', MQ_MCA_NAME_LENGTH);
		memcpy(ChlDefn->MCAName, pPCFString->String, pPCFString -> StringLength);
		break;
	case MQCACH_DESC:
		memset(ChlDefn->ChannelDesc, ' ', MQ_CHANNEL_DESC_LENGTH);
		memcpy(ChlDefn->ChannelDesc, pPCFString->String,
				pPCFString -> StringLength);
		break;
	case MQCACH_SEC_EXIT_NAME:
		memset(ChlDefn->SecurityExit, ' ', MQ_EXIT_NAME_LENGTH);
		memcpy(ChlDefn->SecurityExit, pPCFString->String,
				pPCFString -> StringLength);
		break;
	case MQCACH_MSG_EXIT_NAME:
		memset(ChlDefn->MsgExit, ' ', MQ_TOTAL_EXIT_NAME_LENGTH);
		memcpy(ChlDefn->MsgExit, pPCFString->String, pPCFString -> StringLength);
		ChlDefn->MsgExitListFlag = 0;
		break;
	case MQCACH_SEND_EXIT_NAME:
		memset(ChlDefn->SendExit, ' ', MQ_TOTAL_EXIT_NAME_LENGTH);
		memcpy(ChlDefn->SendExit, pPCFString->String, pPCFString -> StringLength);
		ChlDefn->SendExitListFlag = 0;
		break;
	case MQCACH_RCV_EXIT_NAME:
		memset(ChlDefn->ReceiveExit, ' ', MQ_TOTAL_EXIT_NAME_LENGTH);
		memcpy(ChlDefn->ReceiveExit, pPCFString->String,
				pPCFString -> StringLength);
		ChlDefn->ReceiveExitListFlag = 0;
		break;
	case MQCACH_SEC_EXIT_USER_DATA:
		memset(ChlDefn->SecurityUserData, ' ', MQ_EXIT_DATA_LENGTH);
		memcpy(ChlDefn->SecurityUserData, pPCFString->String,
				pPCFString -> StringLength);
		break;
	case MQCACH_MSG_EXIT_USER_DATA:
		memset(ChlDefn->MsgUserData, ' ', MQ_TOTAL_EXIT_DATA_LENGTH);
		memcpy(ChlDefn->MsgUserData, pPCFString->String,
				pPCFString -> StringLength);
		ChlDefn->MsgUserDataListFlag = 0;
		break;
	case MQCACH_SEND_EXIT_USER_DATA:
		memset(ChlDefn->SendUserData, ' ', MQ_TOTAL_EXIT_DATA_LENGTH);
		memcpy(ChlDefn->SendUserData, pPCFString->String,
				pPCFString -> StringLength);
		ChlDefn->SendUserDataListFlag = 0;
		break;
	case MQCACH_RCV_EXIT_USER_DATA:
		memset(ChlDefn->ReceiveUserData, ' ', MQ_TOTAL_EXIT_DATA_LENGTH);
		memcpy(ChlDefn->ReceiveUserData, pPCFString->String,
				pPCFString -> StringLength);
		ChlDefn->ReceiveUserDataListFlag = 0;
		break;
	case MQCACH_MCA_USER_ID:
		memset(ChlDefn->MCAUserIdentifier, ' ', XZMCAUSERLEN);
		memcpy(ChlDefn->MCAUserIdentifier, pPCFString->String,
				pPCFString -> StringLength);
		break;
	case MQCACH_USER_ID:
		memset(ChlDefn->UserIdentifier, ' ', MQ_USER_ID_LENGTH);
		memcpy(ChlDefn->UserIdentifier, pPCFString->String,
				pPCFString -> StringLength);
		break;
	case MQCACH_PASSWORD:
		memset(ChlDefn->Password, ' ', MQ_PASSWORD_LENGTH);
		memcpy(ChlDefn->Password, pPCFString->String, pPCFString -> StringLength);
		break;
	case MQCACH_MR_EXIT_NAME:

		memset(ChlDefn->MsgRetryExit, ' ', MQ_EXIT_NAME_LENGTH);
		memcpy(ChlDefn->MsgRetryExit, pPCFString->String,
				pPCFString -> StringLength);
		ChlDefn->MsgRetryExitListFlag = 0;

		break;
	case MQCACH_MR_EXIT_USER_DATA:
		memset(ChlDefn->MsgRetryUserData, ' ', MQ_EXIT_DATA_LENGTH);
		memcpy(ChlDefn->MsgRetryUserData, pPCFString->String,
				pPCFString -> StringLength);
		ChlDefn->MsgRetryUserDataListFlag = 0;
		break;
#if defined (MQCA_CLUSTER_NAME)
	case MQCA_CLUSTER_NAME:
		memset(ChlDefn->ClusterName, ' ', MQ_CLUSTER_NAME_LENGTH);
		memcpy(ChlDefn->ClusterName, pPCFString->String,
				pPCFString->StringLength);
		break;
#endif
#if defined (MQCA_CLUSTER_NAMELIST)
	case MQCA_CLUSTER_NAMELIST:
		memset(ChlDefn->ClusterNameList, ' ', MQ_OBJECT_NAME_LENGTH);
		memcpy(ChlDefn->ClusterNameList, pPCFString->String,
				pPCFString->StringLength);
		break;
#endif
#if defined (MQCA_ALTERATION_DATE)
	case MQCA_ALTERATION_DATE:
		memset(ChlDefn->AlterationDate, ' ', MQ_CREATION_DATE_LENGTH);
		memcpy(ChlDefn->AlterationDate, pPCFString->String,
				pPCFString->StringLength);
		break;
	case MQCA_ALTERATION_TIME:
		memset(ChlDefn->AlterationTime, ' ', MQ_CREATION_TIME_LENGTH);
		memcpy(ChlDefn->AlterationTime, pPCFString->String,
				pPCFString->StringLength);
		break;
#endif
#if defined (MQ_LOCAL_ADDRESS_LENGTH)

	case MQCACH_LOCAL_ADDRESS:
		memset(ChlDefn->LocalAddr, ' ', MQ_LOCAL_ADDRESS_LENGTH);
		memcpy(ChlDefn->LocalAddr, pPCFString->String, pPCFString->StringLength);
		break;

	case MQCACH_SSL_CIPHER_SPEC:
		memset(ChlDefn->SslCipher, ' ', MQ_SSL_CIPHER_SPEC_LENGTH);
		memcpy(ChlDefn->SslCipher, pPCFString->String, pPCFString->StringLength);
		break;

	case MQCACH_SSL_PEER_NAME:
		memset(ChlDefn->SslPeer, ' ', MQ_SSL_PEER_NAME_LENGTH);
		memcpy(ChlDefn->SslPeer, pPCFString->String, pPCFString->StringLength);
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
			fprintf(stderr, "(channel.c) Unknown PCF String Parameter: %ld\n",
					pPCFString->Parameter);
		*iCC = WARNING;
		break;
	} /* endswitch */

	return;
}

/********************* [v512] process lists of exits and exit data ***************/
void ProcessChannelStringListParm(MQCFSL *pPCFString, ChlParms *ChlDefn,
		MQLONG *iCC) {
	/* Initialise return codes */
	*iCC = MQCC_OK;

	switch (pPCFString->Parameter) {
	case MQCACH_MSG_EXIT_NAME:
		makeStringFromList(ChlDefn->MsgExit, pPCFString->Strings,
				pPCFString->StringLength, pPCFString->Count);
		ChlDefn->MsgExitListFlag = 1;
		break;
	case MQCACH_SEND_EXIT_NAME:
		makeStringFromList(ChlDefn->SendExit, pPCFString->Strings,
				pPCFString -> StringLength, pPCFString->Count);
		ChlDefn->SendExitListFlag = 1;
		break;
	case MQCACH_RCV_EXIT_NAME:
		makeStringFromList(ChlDefn->ReceiveExit, pPCFString->Strings,
				pPCFString -> StringLength, pPCFString->Count);
		ChlDefn->ReceiveExitListFlag = 1;
		break;
	case MQCACH_MSG_EXIT_USER_DATA:
		makeStringFromList(ChlDefn->MsgUserData, pPCFString->Strings,
				pPCFString -> StringLength, pPCFString->Count);
		ChlDefn->MsgUserDataListFlag = 1;
		break;
	case MQCACH_SEND_EXIT_USER_DATA:
		makeStringFromList(ChlDefn->SendUserData, pPCFString->Strings,
				pPCFString -> StringLength, pPCFString->Count);
		ChlDefn->SendUserDataListFlag = 1;
		break;
	case MQCACH_RCV_EXIT_USER_DATA:
		makeStringFromList(ChlDefn->ReceiveUserData, pPCFString->Strings,
				pPCFString -> StringLength, pPCFString->Count);
		ChlDefn->ReceiveUserDataListFlag = 1;
		break;
	case MQCACH_MR_EXIT_NAME:
		makeStringFromList(ChlDefn->MsgRetryExit, pPCFString->Strings,
				pPCFString -> StringLength, pPCFString->Count);
		ChlDefn->MsgRetryExitListFlag = 1;
		break;
	case MQCACH_MR_EXIT_USER_DATA:
		makeStringFromList(ChlDefn->MsgRetryUserData, pPCFString->Strings,
				pPCFString -> StringLength, pPCFString->Count);
		ChlDefn->MsgRetryUserDataListFlag = 1;
		break;

	default:

		if (noisy)
			fprintf(stderr,
					"(channel.c) Unknown PCF String List Parameter: %ld\n",
					pPCFString->Parameter);
		*iCC = WARNING;
		break;

	} /* endswitch */

	return;
}

void ProcessChannelIntegerParm(MQCFIN *pPCFInteger, ChlParms *ChlDefn,
		MQLONG *iCC) {
	/* Initialise  return codes */
	*iCC = MQCC_OK;

	switch (pPCFInteger->Parameter) {
	case MQIACH_CHANNEL_TYPE:
		ChlDefn->ChannelType = pPCFInteger->Value;
		break;
	case MQIACH_XMIT_PROTOCOL_TYPE:
		ChlDefn->TransportType = pPCFInteger->Value;
		break;
	case MQIACH_BATCH_SIZE:
		ChlDefn->BatchSize = pPCFInteger->Value;
		break;
	case MQIACH_DISC_INTERVAL:
		ChlDefn->DiscInterval = pPCFInteger->Value;
		break;
	case MQIACH_SHORT_RETRY:
		ChlDefn->ShortRetryCount = pPCFInteger->Value;
		break;
	case MQIACH_SHORT_TIMER:
		ChlDefn->ShortRetryInterval = pPCFInteger->Value;
		break;
	case MQIACH_LONG_RETRY:
		ChlDefn->LongRetryCount = pPCFInteger->Value;
		break;
	case MQIACH_LONG_TIMER:
		ChlDefn->LongRetryInterval = pPCFInteger->Value;
		break;
	case MQIACH_DATA_CONVERSION:
		ChlDefn->DataConversion = pPCFInteger->Value;
		break;
	case MQIACH_PUT_AUTHORITY:
		ChlDefn->PutAuthority = pPCFInteger->Value;
		break;
	case MQIACH_SEQUENCE_NUMBER_WRAP:
		ChlDefn->SeqNumberWrap = pPCFInteger->Value;
		break;
	case MQIACH_MAX_MSG_LENGTH:
		ChlDefn->MaxMsgLength = pPCFInteger->Value;
		break;
	case MQIACH_MCA_TYPE:
		ChlDefn->MCAType = pPCFInteger->Value;
		break;
	case MQIACH_MR_COUNT:
		ChlDefn->MsgRetryCount = pPCFInteger->Value;
		break;
	case MQIACH_MR_INTERVAL:
		ChlDefn->MsgRetryInterval = pPCFInteger->Value;
		break;
#if defined (MQIACH_BATCH_INTERVAL)
	case MQIACH_BATCH_INTERVAL:
		ChlDefn->BatchInterval = pPCFInteger->Value;
		break;
#endif
#if defined (MQIACH_HB_INTERVAL)
	case MQIACH_HB_INTERVAL:
		ChlDefn->HeartbeatInterval = pPCFInteger->Value;
		break;
#endif
#if defined (MQIACH_NPM_SPEED)
	case MQIACH_NPM_SPEED:
		ChlDefn->NonPersistentMsgSpeed = pPCFInteger->Value;
		break;
#endif
#if defined (MQIACH_NETWORK_PRIORITY)
	case MQIACH_NETWORK_PRIORITY:
		ChlDefn->NetworkPriority = pPCFInteger->Value;
		break;
#endif
#ifdef MQIACH_INHIBIT_AUTOSTART
		case MQIACH_INHIBIT_AUTOSTART:
		ChlDefn->InhibitAutostart = pPCFInteger->Value;
		break;
#endif
#if defined (MQ_LOCAL_ADDRESS_LENGTH)

	case MQIACH_BATCH_HB:
		ChlDefn->BatchHB = pPCFInteger->Value;
		break;
	case MQIACH_SSL_CLIENT_AUTH:
		ChlDefn->SslCAuth = pPCFInteger->Value;

		break;

#if defined (MQIACH_KEEP_ALIVE_INTERVAL)
	case MQIACH_KEEP_ALIVE_INTERVAL:
		ChlDefn->KAInt = pPCFInteger->Value;
		break;
#endif

#endif

#ifdef zzMQV60
	case MQIA_MONITORING_CHANNEL:
		ChlDefn->MonChl = pPCFInteger->Value;
		break;
	case MQIA_STATISTICS_CHANNEL:
		ChlDefn->StatsChl = pPCFInteger->Value;
		break;
	case MQIACH_CLWL_CHANNEL_RANK:
		ChlDefn->Rank = pPCFInteger->Value;
		break;
	case MQIACH_CLWL_CHANNEL_PRIORITY:
		ChlDefn->Priority = pPCFInteger->Value;
		break;
	case MQIACH_CLWL_CHANNEL_WEIGHT:
		ChlDefn->Weight = pPCFInteger->Value;
		break;

#endif

#ifdef zzMQV70
	case MQIA_PROPERTY_CONTROL:
		ChlDefn->PropCntl = pPCFInteger->Value;
		break;
	case MQIACH_SHARING_CONVERSATIONS:
		ChlDefn->ShareCnv = pPCFInteger->Value;
		break;
	case MQIACH_MAX_INSTANCES:
		ChlDefn->MaxInsts = pPCFInteger->Value;
		break;
	case MQIACH_MAX_INSTS_PER_CLIENT:
		ChlDefn->MaxInstsC = pPCFInteger->Value;
		break;
	case MQIACH_CONNECTION_AFFINITY:
		ChlDefn->Affinity = pPCFInteger->Value;
		break;
	case MQIACH_DEF_CHANNEL_DISP:
		ChlDefn->DefCDsp = pPCFInteger->Value;
		break;
	case MQIACH_CLIENT_CHANNEL_WEIGHT:
		ChlDefn->ClntChlWght = pPCFInteger->Value;
		break;

#endif
#if defined(zzzOS)
		/* v602 zOS attributes */
	case MQIA_QSG_DISP: /* zOS */
		ChlDefn->qsgDisp = pPCFInteger->Value;
		break;
#endif

	default:
		if (noisy)
			fprintf(stderr, "(channel.c) Unknown PCF Integer Parameter: %ld\n",
					pPCFInteger->Parameter);
		*iCC = WARNING;
		break;
	} /* endswitch */

	return;
}

void ProcessChannelIntegerListParm(MQCFIL *pPCFIntegerList, ChlParms *ChlDefn,
		MQLONG *iCC) {
	int i;
	*iCC = MQCC_OK;
	i=0;

	switch (pPCFIntegerList->Parameter) {

#if defined (MQIACH_HDR_COMPRESSION)
	case MQIACH_HDR_COMPRESSION:
		for (i=0; i<COMP_ARRAY_SIZE; i++) {
			ChlDefn->HdrComp[i] = MQCOMPRESS_NOT_AVAILABLE;
		}

		for (i=0; i<pPCFIntegerList->Count && i<COMP_ARRAY_SIZE; i++) {
			ChlDefn->HdrComp[i] = pPCFIntegerList->Values[i];
		}
		break;

	case MQIACH_MSG_COMPRESSION:
		for (i=0; i<COMP_ARRAY_SIZE; i++) {
			ChlDefn->MsgComp[i] = MQCOMPRESS_NOT_AVAILABLE;
		}
		for (i=0; i<pPCFIntegerList->Count && i<COMP_ARRAY_SIZE; i++) {
			ChlDefn->MsgComp[i] = pPCFIntegerList->Values[i];
		}
		break;
#endif

	default:
		if (noisy)
			fprintf(stderr,
					"(channel.c) Unknown PCF Integer List Parameter: %ld\n",
					pPCFIntegerList->Parameter);
		*iCC = WARNING;
		break;
	} /* endswitch */

	return;
}

/* ------------------------------------------------------------------------ */
/*                                                                          */
/* This process takes the attributes of a single channel and adds           */
/* them to the end of a file, SAVEQMGR.TST, which can be found in the       */
/* current directory.                                                       */
/*                                                                          */
/* The file is of a format suitable for subsequent input to RUNMQSC.        */
/*                                                                          */
/* ------------------------------------------------------------------------ */

void AddToFileCHANNEL(ChlParms *pChlDefn, int UserVersion, char * fileName, /* v32 Name of the output file  */
FILE *fp, /* v51 file handle */
int twolines, /* v601 */
MQLONG *iCC) {
	char * ParmBuffer; /* Temporary buffer to hold for output to file */
	int options_exit; /* Option flag for MQsprintf_X (for chained exits) */
	int options; /* Option flag for MQsprintf_X */
	char work[256];
	work[0]='\0';

	/* Initialise return codes */
	*iCC = MQCC_OK;

	if ( !(ParmBuffer = (char *) malloc(PBSIZE) )) {
		fprintf(stderr, "(channel.c) Failed to allocate %d bytes \n", PBSIZE);
		exit(2);
	}

	/* v510 removes open from here ... fp = fopen( fileName, OPENMODE ); */

	/* v601 setup options for oneLine operation */
	options = MQSPRINTF_X_DOUBLE_QUOTES + MQSPRINTF_X_NO_TRAILING_BLANKS;
	if (!oneLine)
		options += MQSPRINTF_X_LINE_BREAKS;

	/* ----------------------------------------------------------------- */
	/* Add ChannelName and Type to file                                  */
	/* If unrecognised type then ignore this defintition, return warning */
	/* ----------------------------------------------------------------- */

	if (twolines)
		fputs("* ", fp);

	switch (pChlDefn->ChannelType) {
	case MQCHT_SENDER:
		MQsprintf_X(ParmBuffer, "DEFINE CHANNEL ('%s') CHLTYPE(SDR) ",
				pChlDefn->ChannelName, MQ_CHANNEL_NAME_LENGTH, options);
		break;
	case MQCHT_SERVER:
		MQsprintf_X(ParmBuffer, "DEFINE CHANNEL ('%s') CHLTYPE(SVR) ",
				pChlDefn->ChannelName, MQ_CHANNEL_NAME_LENGTH, options);
		break;
	case MQCHT_RECEIVER:
		MQsprintf_X(ParmBuffer, "DEFINE CHANNEL ('%s') CHLTYPE(RCVR) ",
				pChlDefn->ChannelName, MQ_CHANNEL_NAME_LENGTH, options);
		break;
	case MQCHT_REQUESTER:
		MQsprintf_X(ParmBuffer, "DEFINE CHANNEL ('%s') CHLTYPE(RQSTR) ",
				pChlDefn->ChannelName, MQ_CHANNEL_NAME_LENGTH, options);
		break;
	case MQCHT_SVRCONN:
		MQsprintf_X(ParmBuffer, "DEFINE CHANNEL ('%s') CHLTYPE(SVRCONN) ",
				pChlDefn->ChannelName, MQ_CHANNEL_NAME_LENGTH, options);
		break;
	case MQCHT_CLNTCONN:
		MQsprintf_X(ParmBuffer, "DEFINE CHANNEL ('%s') CHLTYPE(CLNTCONN) ",
				pChlDefn->ChannelName, MQ_CHANNEL_NAME_LENGTH, options);
		break;
		/* [v51] start v51 attributes */
#if defined(MQCHT_CLUSRCVR)
	case MQCHT_CLUSRCVR:
		if (UserVersion>=MQCMDL_LEVEL_510) {
			MQsprintf_X(ParmBuffer, "DEFINE CHANNEL ('%s') CHLTYPE(CLUSRCVR) ",
					pChlDefn->ChannelName, MQ_CHANNEL_NAME_LENGTH, options);
		} else {
			return; /* user doesn't want V51 stuff */
		}/* End if*/
		break;
#endif

#if defined(MQCHT_CLUSSDR)
	case MQCHT_CLUSSDR:
		if (UserVersion>=MQCMDL_LEVEL_510) {
			MQsprintf_X(ParmBuffer, "DEFINE CHANNEL ('%s') CHLTYPE(CLUSSDR) ",
					pChlDefn->ChannelName, MQ_CHANNEL_NAME_LENGTH, options);
		} else {
			return; /* user doesn't want V51 stuff */
		}/* End if*/
		break;
#endif
		/* [v51] end of V51 channel types */

	default:
		*iCC = WARNING;
		if (noisy)
			fprintf(stderr, "(channel.c) Unknown channel type = %ld \n",
					pChlDefn->ChannelType);
	} /* endswitch */

	fputs(ParmBuffer, fp);

#if defined (MQCA_ALTERATION_DATE)
	if (header && (!oneLine || twolines)) {
		MQsprintf_X(ParmBuffer, "* ALTDATE (%s) ", pChlDefn->AlterationDate, 
		MQ_CREATION_DATE_LENGTH, options);
		fputs(ParmBuffer, fp);
		MQsprintf_X(ParmBuffer, "* ALTTIME (%s) ", pChlDefn->AlterationTime, 
		MQ_CREATION_TIME_LENGTH, options);
		fputs(ParmBuffer, fp);
	}
#endif

	switch (pChlDefn->TransportType) {
	case MQXPT_LU62:
		sprintf(ParmBuffer, "       TRPTYPE(LU62) %s", lineTerm);
		fputs(ParmBuffer, fp);
		if (pChlDefn->ChannelType != MQCHT_RECEIVER && pChlDefn->ChannelType
				!= MQCHT_SVRCONN) {
			MQsprintf_X(ParmBuffer, "       MODENAME('%s') ",
					pChlDefn->ModeName, MQ_MODE_NAME_LENGTH, options);
			fputs(ParmBuffer, fp);
			MQsprintf_X(ParmBuffer, "       TPNAME('%s') ", pChlDefn->TpName, 
			MQ_TP_NAME_LENGTH, options);
			fputs(ParmBuffer, fp);
		}
		break;
	case MQXPT_TCP:
		sprintf(ParmBuffer, "       TRPTYPE(TCP) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
	case MQXPT_NETBIOS:
		sprintf(ParmBuffer, "       TRPTYPE(NETBIOS) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
#if defined (MQXPT_SPX)
	case MQXPT_SPX: /* [WMS] added for unix & dec */
		sprintf(ParmBuffer, "       TRPTYPE(SPX) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
#endif
#if defined (MQXPT_DECNET)
	case MQXPT_DECNET:
		sprintf(ParmBuffer, "       TRPTYPE(DECNET) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
#endif
#if defined (MQXPT_UDP)
	case MQXPT_UDP:
		sprintf(ParmBuffer, "       TRPTYPE(UPD) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
#endif
	} /* endswitch */

	/* ------------------------------------------------------------- */
	/* [v51] Write out options in logical groups, based on cmd level  */
	/* requested by user and channel types.                          */
	/* ------------------------------------------------------------- */

	if (UserVersion>=MQCMDL_LEVEL_500 && (pChlDefn->ChannelType == MQCHT_SERVER
			|| pChlDefn->ChannelType == MQCHT_SENDER
#if defined (MQCHT_CLUSSDR)
			|| pChlDefn->ChannelType == MQCHT_CLUSSDR || pChlDefn->ChannelType
			== MQCHT_CLUSRCVR
#endif
			)) {
		sprintf(ParmBuffer, "       BATCHINT(%ld) %s", pChlDefn->BatchInterval,
				lineTerm);
		fputs(ParmBuffer, fp);

		/* [v530] Batchb for SDR, SVR, CLUSSDR and CLUSSVR */
#if defined (MQIACH_BATCH_HB)
		if (UserVersion>=MQCMDL_LEVEL_530) {
			sprintf(ParmBuffer, "       BATCHHB(%ld) %s", pChlDefn->BatchHB,
					lineTerm);
			fputs(ParmBuffer, fp);
		}
#endif
	}

	if (pChlDefn->ChannelType == MQCHT_SENDER || pChlDefn->ChannelType
			== MQCHT_SERVER || pChlDefn->ChannelType == MQCHT_RECEIVER
			|| pChlDefn->ChannelType == MQCHT_REQUESTER
#if defined (MQCHT_CLUSSDR)
			|| pChlDefn->ChannelType == MQCHT_CLUSSDR || pChlDefn->ChannelType
			== MQCHT_CLUSRCVR
#endif
	) {

		sprintf(ParmBuffer, "       BATCHSZ(%ld) %s", pChlDefn->BatchSize,
				lineTerm);
		fputs(ParmBuffer, fp);
	}

#if defined (MQCHT_CLUSSDR)
	if (UserVersion>=MQCMDL_LEVEL_510 && (pChlDefn->ChannelType
			== MQCHT_CLUSSDR || pChlDefn->ChannelType == MQCHT_CLUSRCVR

	)) {

		MQsprintf_X(ParmBuffer, "       CLUSTER('%s') ", pChlDefn->ClusterName, 
		MQ_CLUSTER_NAME_LENGTH, options);
		fputs(ParmBuffer, fp);
		MQsprintf_X(ParmBuffer, "       CLUSNL('%s') ",
				pChlDefn->ClusterNameList, MQ_CLUSTER_NAME_LENGTH, options);
		fputs(ParmBuffer, fp);

	}
#endif

	if (pChlDefn->ChannelType == MQCHT_SENDER || pChlDefn->ChannelType
			== MQCHT_REQUESTER || pChlDefn->ChannelType == MQCHT_CLNTCONN
			|| pChlDefn->ChannelType == MQCHT_SERVER
#if defined (MQCHT_CLUSSDR)
			|| pChlDefn->ChannelType == MQCHT_CLUSSDR || pChlDefn->ChannelType
			== MQCHT_CLUSRCVR
#endif
	) {

		MQsprintf_X(ParmBuffer, "       CONNAME('%s') ",
				pChlDefn->ConnectionName, MQ_CONN_NAME_LENGTH, options);
		fputs(ParmBuffer, fp);

		/* [v530] Locladdr for SDR, SVR, RQSTR, CLNTCONN, CLUSSDR, CLUSRCVR */
#if defined (MQCACH_LOCAL_ADDRESS)
		if (UserVersion>=MQCMDL_LEVEL_530) {
			MQsprintf_X(ParmBuffer, "       LOCLADDR('%s') ",
					pChlDefn->LocalAddr, MQ_LOCAL_ADDRESS_LENGTH, options);
			fputs(ParmBuffer, fp);
		}
#endif

	}

	if (pChlDefn->ChannelType == MQCHT_SENDER || pChlDefn->ChannelType
			== MQCHT_SERVER
#if defined (MQCHT_CLUSSDR)
			|| pChlDefn->ChannelType == MQCHT_CLUSSDR || pChlDefn->ChannelType
			== MQCHT_CLUSRCVR
#endif
	) {

		if (pChlDefn->DataConversion == MQCDC_NO_SENDER_CONVERSION)
			sprintf(ParmBuffer, "       CONVERT(NO) %s", lineTerm);
		else
			sprintf(ParmBuffer, "       CONVERT(YES) %s", lineTerm);
		fputs(ParmBuffer, fp);

	}

	MQsprintf_X(ParmBuffer, "       DESCR('%s') ", pChlDefn->ChannelDesc, 
	MQ_CHANNEL_DESC_LENGTH, options);
	fputs(ParmBuffer, fp);

	if (pChlDefn->ChannelType == MQCHT_SENDER || pChlDefn->ChannelType
			== MQCHT_SERVER
#if defined (MQCHT_CLUSSDR)
			|| pChlDefn->ChannelType == MQCHT_CLUSSDR || pChlDefn->ChannelType
			== MQCHT_CLUSRCVR
#endif
			|| (pChlDefn->ChannelType == MQCHT_SVRCONN && platform == MQPL_MVS )) {

		sprintf(ParmBuffer, "       DISCINT(%ld) %s", pChlDefn->DiscInterval,
				lineTerm);
		fputs(ParmBuffer, fp);
	}

	if (UserVersion>=MQCMDL_LEVEL_500) {
		sprintf(ParmBuffer, "       HBINT(%ld) %s",
				pChlDefn->HeartbeatInterval, lineTerm);
		fputs(ParmBuffer, fp);
	}

	if (pChlDefn->ChannelType == MQCHT_SENDER || pChlDefn->ChannelType
			== MQCHT_SERVER
#if defined (MQCHT_CLUSSDR)
			|| pChlDefn->ChannelType == MQCHT_CLUSSDR || pChlDefn->ChannelType
			== MQCHT_CLUSRCVR
#endif
	) {

		sprintf(ParmBuffer, "       LONGRTY(%ld) %s", pChlDefn->LongRetryCount,
				lineTerm);
		fputs(ParmBuffer, fp);

		sprintf(ParmBuffer, "       LONGTMR(%ld) %s",
				pChlDefn->LongRetryInterval, lineTerm);
		fputs(ParmBuffer, fp);

		sprintf(ParmBuffer, "       SHORTRTY(%ld) %s",
				pChlDefn->ShortRetryCount, lineTerm);
		fputs(ParmBuffer, fp);

		sprintf(ParmBuffer, "       SHORTTMR(%ld) %s",
				pChlDefn->ShortRetryInterval, lineTerm);
		fputs(ParmBuffer, fp);

	}

	sprintf(ParmBuffer, "       MAXMSGL(%ld) %s", pChlDefn->MaxMsgLength,
			lineTerm);
	fputs(ParmBuffer, fp);

	/* Note: MCANAME can only be set to blanks, so we don't bother to */
	/* write it to the output file .                                  */

	if (platform != MQPL_OS400 && platform != MQPL_MVS && ( /* not the 400 platform */
	pChlDefn->ChannelType == MQCHT_SENDER || pChlDefn->ChannelType
			== MQCHT_SERVER || pChlDefn->ChannelType == MQCHT_REQUESTER
#if defined (MQCHT_CLUSSDR)
			|| pChlDefn->ChannelType == MQCHT_CLUSSDR || pChlDefn->ChannelType
			== MQCHT_CLUSRCVR
#endif
			)) {

		if (pChlDefn->MCAType == MQMCAT_PROCESS)
			sprintf(ParmBuffer, "       MCATYPE(PROCESS) %s", lineTerm);
		else
			sprintf(ParmBuffer, "       MCATYPE(THREAD) %s", lineTerm);
		fputs(ParmBuffer, fp);

	}

	if (pChlDefn->ChannelType != MQCHT_CLNTCONN) {

		MQsprintf_X(ParmBuffer, "       MCAUSER('%s') ",
				pChlDefn->MCAUserIdentifier, XZMCAUSERLEN, options);
		fputs(ParmBuffer, fp);

	}

	if (pChlDefn->ChannelType == MQCHT_RECEIVER || pChlDefn->ChannelType
			== MQCHT_REQUESTER
#if defined (MQCHT_CLUSRCVR)
			|| pChlDefn->ChannelType == MQCHT_CLUSRCVR
#endif
	) {

		/* [v530] Depending on whether we're dealing with a string */
		/* or chained exits, pass the correct flags to MQsprintf_X */
		options_exit = MQSPRINTF_X_NO_TRAILING_BLANKS;
		if (!oneLine)
			options_exit += MQSPRINTF_X_LINE_BREAKS;

		if (!pChlDefn->MsgRetryUserDataListFlag) {
			options_exit += MQSPRINTF_X_DOUBLE_QUOTES;
			MQsprintf_X(ParmBuffer, "       MRDATA('%s') ",
					pChlDefn->MsgRetryUserData, MQ_EXIT_DATA_LENGTH,
					options_exit);
		} else {
			MQsprintf_X(ParmBuffer, "       MRDATA(%s) ",
					pChlDefn->MsgRetryUserData, MQ_TOTAL_EXIT_DATA_LENGTH,
					options_exit);
		}
		fputs(ParmBuffer, fp);

		/* [v530] Depending on whether we're dealing with a string */
		/* or chained exits, pass the correct flags to MQsprintf_X */

		options_exit = MQSPRINTF_X_NO_TRAILING_BLANKS;
		if (!oneLine)
			options_exit += MQSPRINTF_X_LINE_BREAKS;

		if (!pChlDefn->MsgRetryExitListFlag) {
			options_exit += MQSPRINTF_X_DOUBLE_QUOTES;
			MQsprintf_X(ParmBuffer, "       MREXIT('%s') ",
					pChlDefn->MsgRetryExit, MQ_EXIT_NAME_LENGTH, options_exit);
		} else {
			MQsprintf_X(ParmBuffer, "       MREXIT(%s) ",
					pChlDefn->MsgRetryExit, MQ_TOTAL_EXIT_NAME_LENGTH,
					options_exit);
		}
		fputs(ParmBuffer, fp);

		sprintf(ParmBuffer, "       MRRTY(%ld) %s", pChlDefn->MsgRetryCount,
				lineTerm);
		fputs(ParmBuffer, fp);
		sprintf(ParmBuffer, "       MRTMR(%ld) %s", pChlDefn->MsgRetryInterval,
				lineTerm);
		fputs(ParmBuffer, fp);
	}

	if (pChlDefn->ChannelType != MQCHT_CLNTCONN && pChlDefn->ChannelType
			!= MQCHT_SVRCONN) {

		/* [v530] Depending on whether we're dealing with a string */
		/* or chained exits, pass the correct flags to MQsprintf_X */

		options_exit = MQSPRINTF_X_NO_TRAILING_BLANKS;
		if (!oneLine)
			options_exit += MQSPRINTF_X_LINE_BREAKS;

		if (!pChlDefn->MsgUserDataListFlag) {
			options_exit += MQSPRINTF_X_DOUBLE_QUOTES;
			MQsprintf_X(ParmBuffer, "       MSGDATA('%s') ",
					pChlDefn->MsgUserData, MQ_EXIT_DATA_LENGTH, options_exit);
		} else {
			MQsprintf_X(ParmBuffer, "       MSGDATA(%s) ",
					pChlDefn->MsgUserData, MQ_TOTAL_EXIT_DATA_LENGTH,
					options_exit);
		}
		fputs(ParmBuffer, fp);


		/* [v530] Depending on whether we're dealing with a /400   */
		/* or chained exits, pass the correct flags to MQsprintf_X */
		options_exit = 0;
		if (!oneLine)
			options_exit = MQSPRINTF_X_LINE_BREAKS;
		/* if (MQAT_DEFAULT != MQAT_OS400) */
		if (platform != MQPL_OS400)
			options_exit += MQSPRINTF_X_NO_TRAILING_BLANKS;

		if (!pChlDefn->MsgExitListFlag) {
			options_exit += MQSPRINTF_X_DOUBLE_QUOTES;
			MQsprintf_X(ParmBuffer, "       MSGEXIT('%s') ", pChlDefn->MsgExit, 
			MQ_EXIT_NAME_LENGTH, options_exit);
		} else {
			MQsprintf_X(ParmBuffer, "       MSGEXIT(%s) ", pChlDefn->MsgExit, 
			MQ_TOTAL_EXIT_NAME_LENGTH, options_exit);
		}
		fputs(ParmBuffer, fp);

	}

#if defined (MQCHT_CLUSRCVR)
	if (UserVersion>=MQCMDL_LEVEL_510 && (pChlDefn->ChannelType
			== MQCHT_CLUSRCVR )) {
		sprintf(ParmBuffer, "       NETPRTY(%ld) %s",
				pChlDefn->NetworkPriority, lineTerm);
		fputs(ParmBuffer, fp);
	} /* end of channel = cluster */
#endif

	if (UserVersion>=MQCMDL_LEVEL_500 && (pChlDefn->ChannelType == MQCHT_SENDER
			|| pChlDefn->ChannelType == MQCHT_SERVER || pChlDefn->ChannelType
			== MQCHT_RECEIVER || pChlDefn->ChannelType == MQCHT_REQUESTER
#if defined (MQCHT_CLUSSDR)
			|| pChlDefn->ChannelType == MQCHT_CLUSSDR || pChlDefn->ChannelType
			== MQCHT_CLUSRCVR
#endif
			)) {
#if defined (MQNPMS_NORMAL)
		if (pChlDefn->NonPersistentMsgSpeed == MQNPMS_NORMAL)
			sprintf(ParmBuffer, "       NPMSPEED(NORMAL) %s", lineTerm);
		else
			sprintf(ParmBuffer, "       NPMSPEED(FAST) %s", lineTerm);
		fputs(ParmBuffer, fp);
#endif
	}

	/* Sorry, we don't generate passwords, change the #if you want them */
#if 0
	if ( pChlDefn->ChannelType == MQCHT_SENDER
			|| pChlDefn->ChannelType == MQCHT_REQUESTER
			|| pChlDefn->ChannelType == MQCHT_CLNTCONN
			|| pChlDefn->ChannelType == MQCHT_SERVER
#if defined (MQCHT_CLUSSDR)
			|| pChlDefn->ChannelType == MQCHT_CLUSSDR
#endif
	) {
		sprintf( ParmBuffer
				, "       PASSWORD('%s') %s"
				, pChlDefn->Password, lineTerm
		);
		fputs( ParmBuffer, fp );
	}
#endif

	if (pChlDefn->ChannelType == MQCHT_RECEIVER || pChlDefn->ChannelType
			== MQCHT_REQUESTER
	/* doc says svrconn are okay for putaut ... doesn't make sense to me */
	/* and also doesn't work !!                                          */
	/*       || pChlDefn->ChannelType == MQCHT_SVRCONN                     */
#if defined (MQCHT_CLUSRCVR)
			|| pChlDefn->ChannelType == MQCHT_CLUSRCVR
#endif
	) {

		switch (pChlDefn->PutAuthority) {
		case MQPA_DEFAULT:
			sprintf(ParmBuffer, "       PUTAUT(DEF) %s", lineTerm);
			break;
		case MQPA_CONTEXT:
			sprintf(ParmBuffer, "       PUTAUT(CTX) %s", lineTerm);
			break;
#if defined (MQPA_ONLY_MCA)
		case MQPA_ONLY_MCA:
			sprintf(ParmBuffer, "       PUTAUT(ONLYMCA) %s", lineTerm);
			break;
#endif
#if defined (MQPA_ALTERNATE_OR_MCA)
		case MQPA_ALTERNATE_OR_MCA:
			sprintf(ParmBuffer, "       PUTAUT(ALTMCA) %s", lineTerm);
			break;
#endif
		default:
			sprintf(ParmBuffer, "       PUTAUT(%ld?) %s",
					pChlDefn->PutAuthority, lineTerm);
			break;
		}/* End switch*/
		fputs(ParmBuffer, fp);
	}

	if (pChlDefn->ChannelType == MQCHT_CLNTCONN) {

		MQsprintf_X(ParmBuffer, "       QMNAME('%s') ", pChlDefn->QMgrName, 
		MQ_Q_MGR_NAME_LENGTH, options);
		fputs(ParmBuffer, fp);

	}

	/* [v530] Depending on whether we're dealing with a string */
	/* or chained exits, pass the correct flags to MQsprintf_X */

	options_exit = MQSPRINTF_X_NO_TRAILING_BLANKS;
	if (!oneLine)
		options_exit += MQSPRINTF_X_LINE_BREAKS;

	if (!pChlDefn->ReceiveUserDataListFlag) {
		options_exit += MQSPRINTF_X_DOUBLE_QUOTES;
		MQsprintf_X(ParmBuffer, "       RCVDATA('%s') ",
				pChlDefn->ReceiveUserData, MQ_EXIT_DATA_LENGTH, options_exit);
	} else {
		MQsprintf_X(ParmBuffer, "       RCVDATA(%s) ",
				pChlDefn->ReceiveUserData, MQ_TOTAL_EXIT_DATA_LENGTH,
				options_exit);
	}
	fputs(ParmBuffer, fp);

	/* [v530] Depending on whether we're dealing with a /400   */
	/* or chained exits, pass the correct flags to MQsprintf_X */
	options_exit = 0;
	if (!oneLine)
		options_exit = MQSPRINTF_X_LINE_BREAKS;
	/* if (MQAT_DEFAULT != MQAT_OS400) */
	if (platform != MQPL_OS400)
		options_exit += MQSPRINTF_X_NO_TRAILING_BLANKS;

	if (!pChlDefn->ReceiveExitListFlag) {
		options_exit += MQSPRINTF_X_DOUBLE_QUOTES;
		MQsprintf_X(ParmBuffer, "       RCVEXIT('%s') ", pChlDefn->ReceiveExit, 
		MQ_EXIT_NAME_LENGTH, options_exit);
	} else {
		MQsprintf_X(ParmBuffer, "       RCVEXIT(%s) ", pChlDefn->ReceiveExit, 
		MQ_TOTAL_EXIT_NAME_LENGTH, options_exit);
	}
	fputs(ParmBuffer, fp);

	MQsprintf_X(ParmBuffer, "       SCYDATA('%s') ",
			pChlDefn->SecurityUserData, MQ_EXIT_DATA_LENGTH, options);
	fputs(ParmBuffer, fp);

	/* [v530] Depending on whether we're dealing with a /400   */
	/* or chained exits, pass the correct flags to MQsprintf_X */
	options_exit = MQSPRINTF_X_DOUBLE_QUOTES;
	if (!oneLine)
		options_exit += MQSPRINTF_X_LINE_BREAKS;
	/* if (MQAT_DEFAULT != MQAT_OS400) */
	if (platform != MQPL_OS400)
		options_exit += MQSPRINTF_X_NO_TRAILING_BLANKS;

	MQsprintf_X(ParmBuffer, "       SCYEXIT('%s') ", pChlDefn->SecurityExit, 
	MQ_EXIT_NAME_LENGTH, options_exit);
	fputs(ParmBuffer, fp);

	/* [v530] Depending on whether we're dealing with a string */
	/* or chained exits, pass the correct flags to MQsprintf_X */

	options_exit = MQSPRINTF_X_NO_TRAILING_BLANKS;
	if (!oneLine)
		options_exit += MQSPRINTF_X_LINE_BREAKS;

	if (!pChlDefn->SendUserDataListFlag) {
		options_exit += MQSPRINTF_X_DOUBLE_QUOTES;
		MQsprintf_X(ParmBuffer, "       SENDDATA('%s') ",
				pChlDefn->SendUserData, MQ_EXIT_DATA_LENGTH, options_exit);
	} else {
		MQsprintf_X(ParmBuffer, "       SENDDATA(%s) ", pChlDefn->SendUserData, 
		MQ_TOTAL_EXIT_DATA_LENGTH, options_exit);
	}
	fputs(ParmBuffer, fp);

	/* [v530] Depending on whether we're dealing with a /400   */
	/* or chained exits, pass the correct flags to MQsprintf_X */
	options_exit = 0;
	if (!oneLine)
		options_exit = MQSPRINTF_X_LINE_BREAKS;
	/* if (MQAT_DEFAULT != MQAT_OS400) */
	if (platform != MQPL_OS400)
		options_exit += MQSPRINTF_X_NO_TRAILING_BLANKS;

	if (!pChlDefn->SendExitListFlag) {
		options_exit += MQSPRINTF_X_DOUBLE_QUOTES;
		MQsprintf_X(ParmBuffer, "       SENDEXIT('%s') ", pChlDefn->SendExit, 
		MQ_EXIT_NAME_LENGTH, options_exit);
	} else {
		MQsprintf_X(ParmBuffer, "       SENDEXIT(%s) ", pChlDefn->SendExit, 
		MQ_TOTAL_EXIT_NAME_LENGTH, options_exit);
	}
	fputs(ParmBuffer, fp);

	if (pChlDefn->ChannelType == MQCHT_SENDER || pChlDefn->ChannelType
			== MQCHT_SERVER || pChlDefn->ChannelType == MQCHT_RECEIVER
			|| pChlDefn->ChannelType == MQCHT_REQUESTER
#if defined (MQCHT_CLUSSDR)
			|| pChlDefn->ChannelType == MQCHT_CLUSSDR || pChlDefn->ChannelType
			== MQCHT_CLUSRCVR
#endif
	) {

		sprintf(ParmBuffer, "       SEQWRAP(%ld) %s", pChlDefn->SeqNumberWrap,
				lineTerm);
		fputs(ParmBuffer, fp);

	}

	/* if (MQAT_DEFAULT != MQAT_OS400 && ( */
	if (platform != MQPL_OS400 && platform != MQPL_MVS
			&& (pChlDefn->ChannelType == MQCHT_SENDER || pChlDefn->ChannelType
					== MQCHT_REQUESTER || pChlDefn->ChannelType
					== MQCHT_CLNTCONN || pChlDefn->ChannelType == MQCHT_SERVER
#if defined (MQCHT_CLUSSDR)
					|| pChlDefn->ChannelType == MQCHT_CLUSSDR
#endif
			)) {

		MQsprintf_X(ParmBuffer, "       USERID('%s') ",
				pChlDefn->UserIdentifier, MQ_USER_ID_LENGTH, options);
		fputs(ParmBuffer, fp);
	}

	if (pChlDefn->ChannelType == MQCHT_SENDER || pChlDefn->ChannelType
			== MQCHT_SERVER) {

		MQsprintf_X(ParmBuffer, "       XMITQ('%s') ", pChlDefn->XmitQName, 
		MQ_Q_NAME_LENGTH, options);
		fputs(ParmBuffer, fp);
	}

	/* [V520] Include Tandem NSK channel sttribute */
#ifdef MQIACH_INHIBIT_AUTOSTART
	if ( pChlDefn->ChannelType == MQCHT_SERVER
			|| pChlDefn->ChannelType == MQCHT_REQUESTER
			|| pChlDefn->ChannelType == MQCHT_SVRCONN
			|| pChlDefn->ChannelType == MQCHT_RECEIVER
	) {
		if ( pChlDefn->InhibitAutostart == MQCFC_AUTOSTART_ALLOWED) {
			sprintf( ParmBuffer, "       AUTOSTART(ENABLED) %s" , lineTerm );
			fputs( ParmBuffer, fp );
		} else {
			sprintf( ParmBuffer, "       AUTOSTART(DISABLED) %s" , lineTerm );
			fputs( ParmBuffer, fp );
		} /* endif */
	}
#endif

	/* [V530] Include SSL Client Auth Attribute for receiving type chls */
#ifdef MQIACH_SSL_CLIENT_AUTH
	if (pChlDefn->ChannelType == MQCHT_SERVER || pChlDefn->ChannelType
			== MQCHT_REQUESTER || pChlDefn->ChannelType == MQCHT_SVRCONN
			|| pChlDefn->ChannelType == MQCHT_CLUSRCVR || pChlDefn->ChannelType
			== MQCHT_RECEIVER) {
		if (UserVersion >= MQCMDL_LEVEL_530) {
			if (pChlDefn->SslCAuth) {
				sprintf(ParmBuffer, "       SSLCAUTH(OPTIONAL) %s", lineTerm);
			} else {
				sprintf(ParmBuffer, "       SSLCAUTH(REQUIRED) %s", lineTerm);
			} /* endif */
			fputs(ParmBuffer, fp);
		}
	}
#endif

	/* [v530] all channel types get SSLCIPH and SSLPEER */
#if defined (MQCACH_SSL_PEER_NAME)

	if (UserVersion>=MQCMDL_LEVEL_530) {

		MQsprintf_X(ParmBuffer, "       SSLCIPH('%s') ", pChlDefn->SslCipher, 
		MQ_SSL_CIPHER_SPEC_LENGTH, options);
		fputs(ParmBuffer, fp);

		MQsprintf_X(ParmBuffer, "       SSLPEER('%s') ", pChlDefn->SslPeer, 
		MQ_SSL_PEER_NAME_LENGTH, options);
		fputs(ParmBuffer, fp);

	}
#endif

	/* [v531b] csd01 on v5.3 adds KAINT to mqsc */
#if defined (MQIACH_KEEP_ALIVE_INTERVAL)
	if (UserVersion>=MQCMDL_LEVEL_530) {
		if (pChlDefn->KAInt != -999) { /* -999 means we never got it from mqsc */
			if (pChlDefn->KAInt == -1) {
				sprintf(ParmBuffer, "       KAINT(AUTO) %s", lineTerm);
			} else {
				sprintf(ParmBuffer, "       KAINT(%ld) %s", pChlDefn->KAInt,
						lineTerm);
			}
			fputs(ParmBuffer, fp);
			pChlDefn -> KAInt = -999; /* special value to indicate not to set KAINT */
		}
	}
#endif

#ifdef zzMQV60

	if (UserVersion >= MQCMDL_LEVEL_600 && (pChlDefn->ChannelType
			== MQCHT_SENDER || pChlDefn->ChannelType == MQCHT_SERVER
			|| pChlDefn->ChannelType == MQCHT_RECEIVER || pChlDefn->ChannelType
			== MQCHT_REQUESTER || pChlDefn->ChannelType == MQCHT_SVRCONN
			|| pChlDefn->ChannelType == MQCHT_CLUSSDR || pChlDefn->ChannelType
			== MQCHT_CLUSRCVR )) {
		toStrMQMON((char*)&work, pChlDefn->MonChl);
		sprintf(ParmBuffer, "       MONCHL(%s) %s", work, lineTerm);
		fputs(ParmBuffer, fp);

	}
	if (UserVersion >= MQCMDL_LEVEL_600 && platform != MQPL_MVS
			&& (pChlDefn->ChannelType == MQCHT_SENDER || pChlDefn->ChannelType
					== MQCHT_SERVER || pChlDefn->ChannelType == MQCHT_RECEIVER
					|| pChlDefn->ChannelType == MQCHT_REQUESTER
					|| pChlDefn->ChannelType == MQCHT_CLUSSDR
					|| pChlDefn->ChannelType == MQCHT_CLUSRCVR )) {
		toStrMQMON((char*)&work, pChlDefn->StatsChl);
		sprintf(ParmBuffer, "       STATCHL(%s) %s", work, lineTerm);
		fputs(ParmBuffer, fp);

	}

	if (UserVersion >= MQCMDL_LEVEL_600 && (pChlDefn->ChannelType
			== MQCHT_CLUSSDR || pChlDefn->ChannelType == MQCHT_CLUSRCVR )) {
		sprintf(ParmBuffer, "       CLWLPRTY(%ld) %s", pChlDefn->Priority,
				lineTerm);
		fputs(ParmBuffer, fp);

		sprintf(ParmBuffer, "       CLWLRANK(%ld) %s", pChlDefn->Rank, lineTerm);
		fputs(ParmBuffer, fp);

		sprintf(ParmBuffer, "       CLWLWGHT(%ld) %s", pChlDefn->Weight,
				lineTerm);
		fputs(ParmBuffer, fp);

	}

	if (UserVersion >= MQCMDL_LEVEL_600) {
		toStrMQComp((char*)work, pChlDefn->MsgComp);
		sprintf(ParmBuffer, "       COMPMSG(%s) %s", work, lineTerm);
		fputs(ParmBuffer, fp);

		toStrMQComp((char*)work, pChlDefn->HdrComp);
		sprintf(ParmBuffer, "       COMPHDR(%s) %s", work, lineTerm);
		fputs(ParmBuffer, fp);
	}

#endif
#ifdef zzMQV70
if ( UserVersion >= MQCMDL_LEVEL_700) {
	if (pChlDefn->ChannelType == MQCHT_CLUSSDR || pChlDefn->ChannelType
			== MQCHT_CLUSRCVR || pChlDefn->ChannelType == MQCHT_SENDER
			|| pChlDefn->ChannelType == MQCHT_SERVER) {
		switch (pChlDefn->PropCntl) {
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
		}
	}

	if (pChlDefn->ChannelType == MQCHT_CLNTCONN || pChlDefn->ChannelType
			== MQCHT_SVRCONN) {
		sprintf(ParmBuffer, "       SHARECNV(%ld) %s", pChlDefn->ShareCnv,
				lineTerm);
		fputs(ParmBuffer, fp);
	}
	
	if (pChlDefn->ChannelType == MQCHT_SVRCONN) {
		sprintf(ParmBuffer, "       MAXINST(%ld) %s", pChlDefn->MaxInsts,
				lineTerm);
		fputs(ParmBuffer, fp);
		sprintf(ParmBuffer, "       MAXINSTC(%ld) %s", pChlDefn->MaxInstsC,
				lineTerm);
		fputs(ParmBuffer, fp);
	}

	if (pChlDefn->ChannelType == MQCHT_CLNTCONN) {
		switch (pChlDefn->Affinity) {
		case MQCAFTY_PREFERRED:
			sprintf(ParmBuffer, "       AFFINITY(PREFERRED) %s", lineTerm);
			fputs(ParmBuffer, fp);
			break;
		case MQCAFTY_NONE:
			sprintf(ParmBuffer, "       AFFINITY(NONE) %s", lineTerm);
			fputs(ParmBuffer, fp);
			break;
		}
	}

	if (pChlDefn->ChannelType != MQCHT_CLNTCONN &&
		pChlDefn->ChannelType != MQCHT_CLUSRCVR &&
		pChlDefn->ChannelType != MQCHT_CLUSSDR ) {
		switch (pChlDefn->DefCDsp) {
		case MQCHLD_PRIVATE:
			sprintf(ParmBuffer, "       DEFCDISP(PRIVATE) %s", lineTerm);
			fputs(ParmBuffer, fp);
			break;
		case MQCHLD_FIXSHARED:
			sprintf(ParmBuffer, "       DEFCDISP(FIXSHARED) %s", lineTerm);
			fputs(ParmBuffer, fp);
			break;
		case MQCHLD_SHARED:
			sprintf(ParmBuffer, "       DEFCDISP(SHARED) %s", lineTerm);
			fputs(ParmBuffer, fp);
			break;
		}		
	}

	if (pChlDefn->ChannelType == MQCHT_CLNTCONN) {
		sprintf(ParmBuffer, "       CLNTWGHT(%ld) %s", pChlDefn->ClntChlWght,
				lineTerm);
		fputs(ParmBuffer, fp);
	}
}
#endif
#if defined(zzzOS)
	if (platform == MQPL_MVS) {
		toStrQSG((char*)&work, pChlDefn->qsgDisp);
		sprintf(ParmBuffer, "       QSGDISP(%s) %s", work, lineTerm);
		fputs(ParmBuffer, fp);

	}
#endif

	sprintf(ParmBuffer, "       REPLACE\n");
	fputs(ParmBuffer, fp);

	if (!oneLine) {
		fputs("\n", fp);
	}

	/* v512 -- if -R specified, then create "reset" commands */
	/* v603 -- control different channel types via command line switch */
	if ( ( (pChlDefn->ChannelType == MQCHT_SENDER) && (chlReset & CHLR_SENDER) )
			|| ( (pChlDefn->ChannelType == MQCHT_SERVER) && (chlReset
					& CHLR_SERVER ) ) || ( (pChlDefn->ChannelType
			== MQCHT_RECEIVER) && (chlReset & CHLR_RECEIVER ) )
			|| ( (pChlDefn->ChannelType == MQCHT_REQUESTER) && (chlReset
					& CHLR_REQUESTER ) )
#if defined (MQCHT_CLUSSDR)
			|| ( (pChlDefn->ChannelType == MQCHT_CLUSSDR) && (chlReset
					& CHLR_CLUSSDR ) ) || ( (pChlDefn->ChannelType
			== MQCHT_CLUSRCVR) && (chlReset & CHLR_CLUSRCVR ) )
#endif
	) {
		MQsprintf_X(ParmBuffer, "RESET CHANNEL ('%s') SEQNUM(1)\n",
				pChlDefn->ChannelName, MQ_CHANNEL_NAME_LENGTH, 
				MQSPRINTF_X_NO_TRAILING_BLANKS);
		fputs(ParmBuffer, fp);

		if (!oneLine)
			fputs("\n", fp);

	}

	free(ParmBuffer);
	return;
}

