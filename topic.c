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
/* Module Name: topic.c                                                      */
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

/* If this isn't a v7 compile, don't bother compiling anything */
#if defined (zzMQV70)
/* ------------------------------------------------------------------------ */
/*                                                                          */
#ifndef MQIA_DURABLE_SUB		/*Solaris doesn't define this? */
#define MQIA_DURABLE_SUB   175L
#endif
extern int noisy;
extern int header;
extern int oneLine; /* v601 -- whether to write the entire DEFINE on oneline */
extern char lineTerm[3]; /* v601 -- line terminator */
extern int platform;

void ProcessTopicStringParm(MQCFST *pPCFString, TopicParms *TopicParms,
		MQLONG *iCC) {

	switch (pPCFString->Parameter) {
		case MQCA_TOPIC_NAME:
		memset(TopicParms->TopicName, '\0', MQ_TOPIC_NAME_LENGTH);
		memcpy(TopicParms->TopicName, pPCFString->String,
				pPCFString->StringLength);
		break;
		case MQCA_TOPIC_STRING:
		memset(TopicParms->TopicString, '\0', MQ_TOPIC_STR_LENGTH);
		memcpy(TopicParms->TopicString, pPCFString->String,
				pPCFString->StringLength);
		break;
		case MQCA_TOPIC_DESC:
		memset(TopicParms->TopicDesc, '\0', MQ_TOPIC_DESC_LENGTH);
		memcpy(TopicParms->TopicDesc, pPCFString->String,
				pPCFString->StringLength);
		break;
		case MQCA_ALTERATION_DATE:
		memset(TopicParms->AlterationDate, '\0', MQ_CREATION_DATE_LENGTH);
		memcpy(TopicParms->AlterationDate, pPCFString->String,
				pPCFString->StringLength);
		break;
		case MQCA_ALTERATION_TIME:
		memset(TopicParms->AlterationTime, ' ', MQ_CREATION_TIME_LENGTH);
		memcpy(TopicParms->AlterationTime, pPCFString->String,
				pPCFString->StringLength);
		break;
		case MQCA_CLUSTER_NAME:
		memset(TopicParms->ClusterName, ' ', MQ_CLUSTER_NAME_LENGTH);
		memcpy(TopicParms->ClusterName, pPCFString->String,
				pPCFString->StringLength);
		break;
		case MQCA_MODEL_DURABLE_Q:
		memset(TopicParms->DurModQName, ' ', MQ_Q_NAME_LENGTH);
		memcpy(TopicParms->DurModQName, pPCFString->String,
				pPCFString->StringLength);
		break;
		case MQCA_MODEL_NON_DURABLE_Q:
		memset(TopicParms->NonDurModQName, ' ', MQ_Q_NAME_LENGTH);
		memcpy(TopicParms->NonDurModQName, pPCFString->String,
				pPCFString->StringLength);
		break;
#ifdef zzzOS		
		case MQBACF_RESPONSE_ID: /* zOS things to ignore */
		break;
		case MQCACF_RESPONSE_Q_MGR_NAME: /* zOS things to ignore */
		break;
#endif
		default:
		fprintf(stderr, "(topic.c) Unknown PCF String Parameter: %ld\n",
				pPCFString->Parameter);
		*iCC = WARNING;
		break;

	}
}

void ProcessTopicIntegerParm(MQCFIN *pPCFInteger, TopicParms *TopicParms,
		MQLONG *iCC) {
	switch (pPCFInteger->Parameter) {
		case MQIA_TOPIC_TYPE:
		TopicParms->TopicType = pPCFInteger->Value;
		break;
		case MQIA_DEF_PRIORITY:
		TopicParms->DefPriority = pPCFInteger->Value;
		break;
		case MQIA_TOPIC_DEF_PERSISTENCE:
		TopicParms->DefPersistence = pPCFInteger->Value;
		break;
		case MQIA_DEF_PUT_RESPONSE_TYPE:
		TopicParms->DefPutResp = pPCFInteger->Value;
		break;
		case MQIA_DURABLE_SUB:
		TopicParms->DurSubs = pPCFInteger->Value;
		break;
		case MQIA_INHIBIT_PUB:
		TopicParms->InhibitPubs = pPCFInteger->Value;
		break;
		case MQIA_INHIBIT_SUB:
		TopicParms->InhibitSubs = pPCFInteger->Value;
		break;
		case MQIA_NPM_DELIVERY:
		TopicParms->NPerMsgDlvr = pPCFInteger->Value;
		break;
		case MQIA_PM_DELIVERY:
		TopicParms->PerMsgDlvr = pPCFInteger->Value;
		break;
		case MQIA_PROXY_SUB:
		TopicParms->ProxySubs = pPCFInteger->Value;
		break;
		case MQIA_PUB_SCOPE:
		TopicParms->PubScope = pPCFInteger->Value;
		break;
		case MQIA_SUB_SCOPE:
		TopicParms->SubScope = pPCFInteger->Value;
		break;
		case MQIA_WILDCARD_OPERATION:
		TopicParms->Wildcard = pPCFInteger->Value;
		break;
#ifdef zzzOS		
		case MQIA_QSG_DISP: 
			TopicParms->qsgDisp= pPCFInteger->Value;
		break;
#endif

		default:
		fprintf(stderr, "(topic.c) Unknown PCF Integer Parameter: %ld\n",
				pPCFInteger->Parameter);
		*iCC = WARNING;
		break;

	}
}

void AddToFileTopic(TopicParms *TopicParms, int QmgrVersion, char *fileName,
		FILE *fp, /* v51 */
		int twolines, /* v601 */
		MQLONG *iCC) {
	char * ParmBuffer; /* Temporary buffer to hold for output to file */
	int options = 0;
	char work[256];

	memset(work, '\0', 256);

	if ( !(ParmBuffer = (char *) malloc(PBSIZE) )) {
		fprintf(stderr, "(topic.c) Failed to allocate %d bytes \n",
				PBSIZE);
		exit(2);
	}

	/* Initialise return codes */
	*iCC = MQCC_OK;

	/* v601 setup options for oneLine operation */
	options = MQSPRINTF_X_DOUBLE_QUOTES + MQSPRINTF_X_NO_TRAILING_BLANKS;
	if (!oneLine)
	options += MQSPRINTF_X_LINE_BREAKS;

	if (twolines)
	fputs("* ", fp);

	/* Append these details to the end of the current output file (v32) */

	MQsprintf_X(ParmBuffer, "DEFINE TOPIC ('%s') ", TopicParms->TopicName,
			MQ_TOPIC_NAME_LENGTH, options);
	fputs(ParmBuffer, fp);
	if (header && (!oneLine || twolines)) {
		MQsprintf_X(ParmBuffer, "* ALTDATE (%s) ", TopicParms->AlterationDate,
				MQ_CREATION_DATE_LENGTH, options);
		fputs(ParmBuffer, fp);
		MQsprintf_X(ParmBuffer, "* ALTTIME (%s) ", TopicParms->AlterationTime,
				MQ_CREATION_TIME_LENGTH, options);
		fputs(ParmBuffer, fp);
	}

	MQsprintf_X(ParmBuffer, "       TOPICSTR('%s') ", TopicParms->TopicString,
			MQ_TOPIC_STR_LENGTH, options+MQSPRINTF_X_NULL_IF_BLANK);
	fputs(ParmBuffer, fp);

	MQsprintf_X(ParmBuffer, "       DESCR('%s') ", TopicParms->TopicDesc,
			MQ_TOPIC_DESC_LENGTH, options);
	fputs(ParmBuffer, fp);

	MQsprintf_X(ParmBuffer, "       CLUSTER('%s') ", TopicParms->ClusterName,
			MQ_CLUSTER_NAME_LENGTH, options);
	fputs(ParmBuffer, fp);

	if (TopicParms->DefPriority == MQPRI_PRIORITY_AS_PARENT) {
		sprintf(ParmBuffer, "       DEFPRTY(ASPARENT) %s", lineTerm);
		fputs(ParmBuffer, fp);

	} else {
		sprintf(ParmBuffer, "       DEFPRTY(%ld) %s", TopicParms->DefPriority,
				lineTerm);
		fputs(ParmBuffer, fp);
	}

	switch (TopicParms->DefPersistence) {
		case MQPER_PERSISTENT:
		sprintf(ParmBuffer, "       DEFPSIST(YES) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
		case MQPER_NOT_PERSISTENT:
		sprintf(ParmBuffer, "       DEFPSIST(NO) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
		case MQPER_PERSISTENCE_AS_PARENT:
		sprintf(ParmBuffer, "       DEFPSIST(ASPARENT) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
	}

	switch (TopicParms->DefPutResp) {
		case MQPRT_ASYNC_RESPONSE:
		sprintf(ParmBuffer, "       DEFPRESP(ASYNC) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
		case MQPRT_RESPONSE_AS_PARENT:
		sprintf(ParmBuffer, "       DEFPRESP(ASPARENT) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
		case MQPRT_SYNC_RESPONSE:
		sprintf(ParmBuffer, "       DEFPRESP(SYNC) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
	}

	MQsprintf_X(ParmBuffer, "       MDURMDL('%s') ", TopicParms->DurModQName,
			MQ_Q_NAME_LENGTH, options);
	fputs(ParmBuffer, fp);

	switch (TopicParms->DurSubs) {
		case MQSUB_DURABLE_ALLOWED:
		sprintf(ParmBuffer, "       DURSUB(YES) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
		case MQSUB_DURABLE_INHIBITED:
		sprintf(ParmBuffer, "       DURSUB(NO) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
		case MQSUB_DURABLE_AS_PARENT:
		sprintf(ParmBuffer, "       DURSUB(ASPARENT) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
	}

	switch (TopicParms->InhibitPubs) {
		case MQTA_PUB_ALLOWED:
		sprintf(ParmBuffer, "       PUB(ENABLED) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
		case MQTA_PUB_INHIBITED:
		sprintf(ParmBuffer, "       PUB(DISABLED) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
		case MQTA_PUB_AS_PARENT:
		sprintf(ParmBuffer, "       PUB(ASPARENT) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
	}

	switch (TopicParms->InhibitSubs) {
		case MQTA_SUB_ALLOWED:
		sprintf(ParmBuffer, "       SUB(ENABLED) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
		case MQTA_SUB_INHIBITED:
		sprintf(ParmBuffer, "       SUB(DISABLED) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
		case MQTA_SUB_AS_PARENT:
		sprintf(ParmBuffer, "       SUB(ASPARENT) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
	}

	MQsprintf_X(ParmBuffer, "       MNDURMDL('%s') ",
			TopicParms->NonDurModQName,
			MQ_Q_NAME_LENGTH, options);
	fputs(ParmBuffer, fp);

	switch (TopicParms->NPerMsgDlvr) {
		case MQDLV_AS_PARENT:
		sprintf(ParmBuffer, "       NPMSGDLV(ASPARENT) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
		case MQDLV_ALL:
		sprintf(ParmBuffer, "       NPMSGDLV(ALL) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
		case MQDLV_ALL_DUR:
		sprintf(ParmBuffer, "       NPMSGDLV(ALLDUR) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
		case MQDLV_ALL_AVAIL:
		sprintf(ParmBuffer, "       NPMSGDLV(ALLAVAIL) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
	}

	switch (TopicParms->PerMsgDlvr) {
		case MQDLV_AS_PARENT:
		sprintf(ParmBuffer, "       PMSGDLV(ASPARENT) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
		case MQDLV_ALL:
		sprintf(ParmBuffer, "       PMSGDLV(ALL) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
		case MQDLV_ALL_DUR:
		sprintf(ParmBuffer, "       PMSGDLV(ALLDUR) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
		case MQDLV_ALL_AVAIL:
		sprintf(ParmBuffer, "       PMSGDLV(ALLAVAIL) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
	}

	switch (TopicParms->ProxySubs) {
		case MQTA_PROXY_SUB_FIRSTUSE:
		sprintf(ParmBuffer, "       PROXYSUB(FIRSTUSE) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
		case MQTA_PROXY_SUB_FORCE:
		sprintf(ParmBuffer, "       PROXYSUB(FORCE) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
	}

	switch (TopicParms->PubScope) {
		case MQSCOPE_ALL:
		sprintf(ParmBuffer, "       PUBSCOPE(ALL) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
		case MQSCOPE_AS_PARENT:
		sprintf(ParmBuffer, "       PUBSCOPE(ASPARENT) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
		case MQSCOPE_QMGR:
		sprintf(ParmBuffer, "       PUBSCOPE(QMGR) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
	}

	switch (TopicParms->SubScope) {
		case MQSCOPE_ALL:
		sprintf(ParmBuffer, "       SUBSCOPE(ALL) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
		case MQSCOPE_AS_PARENT:
		sprintf(ParmBuffer, "       SUBSCOPE(ASPARENT) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
		case MQSCOPE_QMGR:
		sprintf(ParmBuffer, "       SUBSCOPE(QMGR) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
	}

	switch (TopicParms->Wildcard) {
		case MQTA_PASSTHRU:
		sprintf(ParmBuffer, "       WILDCARD(PASSTHRU) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
		case MQTA_BLOCK:
		sprintf(ParmBuffer, "       WILDCARD(BLOCK) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
	}

#if defined(zzzOS)
	if (platform == MQPL_MVS) {
		toStrQSG((char*)&work, TopicParms->qsgDisp);
		sprintf(ParmBuffer, "       QSGDISP(%s) %s", work, lineTerm);
		fputs(ParmBuffer, fp);
	}
#endif

	sprintf(ParmBuffer, "       REPLACE\n");
	fputs(ParmBuffer, fp);
	if (!oneLine) {
		fputs("\n", fp);
	}

}

#endif
