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
/* Module Name: subscript.c                                                  */
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
#ifndef MQIACF_SELECTOR_TYPE
#define MQIACF_SELECTOR_TYPE 1321L
#define MQSELTYPE_NONE 0L
#define MQSELTYPE_STANDARD 1L
#define MQSELTYPE_EXTENDED 2L 
#endif
extern int noisy;
extern int header;
extern int oneLine; /* v601 -- whether to write the entire DEFINE on oneline */
extern char lineTerm[3]; /* v601 -- line terminator */
extern int platform;
extern int diagnostics;

#ifndef MQPSPROP_COMPAT   /* Solaris doesn't define this? */
#define MQPSPROP_COMPAT   1L
#endif

/* ------------------------------------------------------------------------ */
/*                                                                          */
void ProcessSubscrStringParm(MQCFST *pPCFString, SubscrParms *SubscrParms,
		MQLONG *iCC) {
	switch (pPCFString->Parameter) {
	case MQCACF_SUB_NAME:
		if (diagnostics>10) {
			fprintf(stderr, "(subscript.c) About to set SubscrName\n");
		}
		memset(SubscrParms->SubscrName, '\0', MQ_SUB_NAME_LENGTH);
		memcpy(SubscrParms->SubscrName, pPCFString->String,
				(int)pPCFString->StringLength);
		if (diagnostics>10) {
			fprintf(stderr,
			"(subscript.c) Set SubscrName with length %ld to '%.*s'\n",
			pPCFString->StringLength, (int)pPCFString->StringLength,
			SubscrParms->SubscrName);
		}
		break;
	case MQCA_CREATION_DATE:
		memset(SubscrParms->CreationDate, '\0', MQ_CREATION_DATE_LENGTH);
		memcpy(SubscrParms->CreationDate, pPCFString->String,
				(int)pPCFString->StringLength);
		if (diagnostics>10) {
			fprintf(
			stderr,
			"(subscript.c) Set CreationDate with length %ld to '%.*s'\n",
			pPCFString->StringLength, (int)pPCFString->StringLength,
			SubscrParms->CreationDate);
		}
		break;
	case MQCA_CREATION_TIME:
		memset(SubscrParms->CreationTime, '\0', MQ_CREATION_TIME_LENGTH);
		memcpy(SubscrParms->CreationTime, pPCFString->String,
				(int)pPCFString->StringLength);
		if (diagnostics>10) {
			fprintf(
			stderr,
			"(subscript.c) Set CreationTime with length %ld to '%.*s'\n",
			pPCFString->StringLength, (int)pPCFString->StringLength,
			SubscrParms->CreationTime);
		}
		break;
	case MQCA_ALTERATION_DATE:
		memset(SubscrParms->AlterationDate, '\0', MQ_CREATION_DATE_LENGTH);
		memcpy(SubscrParms->AlterationDate, pPCFString->String,
				(int)pPCFString->StringLength);
		if (diagnostics>10) {
			fprintf(
			stderr,
			"(subscript.c) Set AlterationDate with length %ld to '%.*s'\n",
			pPCFString->StringLength, (int)pPCFString->StringLength,
			SubscrParms->AlterationDate);
		}
		break;
	case MQCA_ALTERATION_TIME:
		memset(SubscrParms->AlterationTime, '\0', MQ_CREATION_TIME_LENGTH);
		memcpy(SubscrParms->AlterationTime, pPCFString->String,
				(int)pPCFString->StringLength);
		if (diagnostics>10) {
			fprintf(
			stderr,
			"(subscript.c) Set AlterationTime with length %ld to '%.*s'\n",
			pPCFString->StringLength, (int)pPCFString->StringLength,
			SubscrParms->AlterationTime);
		}
		break;
	case MQCACF_DESTINATION:
		memset(SubscrParms->Destination, '\0', MQ_Q_NAME_LENGTH);
		memcpy(SubscrParms->Destination, pPCFString->String,
				(int)pPCFString->StringLength);
		if (diagnostics>10) {
			fprintf(stderr,
			"(subscript.c) Set Destination with length %ld to '%.*s'\n",
			pPCFString->StringLength, (int)pPCFString->StringLength,
			SubscrParms->Destination);
		}
		break;
	case MQCACF_DESTINATION_Q_MGR:
		memset(SubscrParms->DestQmgr, '\0', MQ_Q_MGR_NAME_LENGTH);
		memcpy(SubscrParms->DestQmgr, pPCFString->String,
				(int)pPCFString->StringLength);
		if (diagnostics>10) {
			fprintf(
			stderr,
			"(subscript.c) Set DestQmgr with length %ld to '%.*s' from '%.*s'\n",
			pPCFString->StringLength, (int)pPCFString->StringLength,
			SubscrParms->DestQmgr, (int)pPCFString->StringLength,
			pPCFString->String);
		}
		break;
	case MQCACF_APPL_IDENTITY_DATA:
		memset(SubscrParms->PubApplId, '\0', MQ_APPL_IDENTITY_DATA_LENGTH);
		memcpy(SubscrParms->PubApplId, pPCFString->String,
				(int)pPCFString->StringLength);
		if (diagnostics>10) {
			fprintf(stderr,
			"(subscript.c) Set PubApplId with length %ld to '%.*s'\n",
			pPCFString->StringLength, (int)pPCFString->StringLength,
			SubscrParms->PubApplId);
		}
		break;
	case MQCACF_SUB_SELECTOR:
		memset(SubscrParms->SubSelector, '\0', MQ_SELECTOR_LENGTH);
		memcpy(SubscrParms->SubSelector, pPCFString->String,
				(int)pPCFString->StringLength);
		if (diagnostics>10) {
			fprintf(
			stderr,
			"(subscript.c) Set SubSelector with length %ld to '%.*s' \n",
			pPCFString->StringLength, (int)pPCFString->StringLength,
			SubscrParms->SubSelector);
		}
		break;
	case MQCACF_SUB_USER_ID:
		memset(SubscrParms->SubUser, '\0', MQ_USER_ID_LENGTH);
		memcpy(SubscrParms->SubUser, pPCFString->String,
				(int)pPCFString->StringLength);
		if (diagnostics>10) {
			fprintf(stderr,
			"(subscript.c) Set SubUser with length %ld to '%.*s' \n",
			pPCFString->StringLength,
			(int)pPCFString->StringLength, SubscrParms->SubUser);
		}
		break;
	case MQCA_TOPIC_STRING:
		memset(SubscrParms->TopicString, '\0', MQ_TOPIC_STR_LENGTH);
		memcpy(SubscrParms->TopicString, pPCFString->String,
				(int)pPCFString->StringLength);
		if (diagnostics>10) {
			fprintf(
			stderr,
			"(subscript.c) Set TopicString with length %ld to '%.*s' \n",
			pPCFString->StringLength, (int)pPCFString->StringLength,
			SubscrParms->TopicString);
		}
		break;
	case MQCA_TOPIC_NAME:
		memset(SubscrParms->TopicName, '\0', MQ_TOPIC_NAME_LENGTH);
		memcpy(SubscrParms->TopicName, pPCFString->String,
				(int)pPCFString->StringLength);
		if (diagnostics>10) {
			fprintf(stderr,
			"(subscript.c) Set TopicName with length %ld to '%.*s'\n",
			pPCFString->StringLength, (int)pPCFString->StringLength,
			SubscrParms->TopicName);
		}
		break;
	case MQCACF_SUB_USER_DATA:
		memset(SubscrParms->SubUserData, '\0', MQ_USER_DATA_LENGTH);
		memcpy(SubscrParms->SubUserData, pPCFString->String,
				(int)pPCFString->StringLength);
		if (diagnostics>10) {
			fprintf(
			stderr,
			"(subscript.c) Set SubUserData with length %ld to '%.*s' \n",
			pPCFString->StringLength,
			(int)pPCFString->StringLength, SubscrParms->SubUserData);
			fflush(stderr);
		}
		break;
	default:
#ifdef zzzOS		
	case MQCACF_RESPONSE_Q_MGR_NAME: /* zOS things to ignore */
		break;
#endif
		fprintf(stderr, "(subscript.c) Unknown PCF String Parameter: %ld\n",
		pPCFString->Parameter);
		*iCC = WARNING;
		break;

	}
}

void ProcessSubscrIntegerParm(MQCFIN *pPCFInteger, SubscrParms *SubscrParms,
		MQLONG *iCC) {
	switch (pPCFInteger->Parameter) {
	case MQIACF_DESTINATION_CLASS:
		if (diagnostics>10) {
			fprintf(stderr, "(subscript.c) About to set DestClass\n");
		}
		SubscrParms->DestClass = (int)pPCFInteger->Value;
		break;
	case MQIACF_DURABLE_SUBSCRIPTION:
		if (diagnostics>10) {
			fprintf(stderr, "(subscript.c) About to set DurSub\n");
		}
		SubscrParms->DurSub = (int)pPCFInteger->Value;
		break;
	case MQIACF_EXPIRY:
		if (diagnostics>10) {
			fprintf(stderr, "(subscript.c) About to set Expiry\n");
		}
		SubscrParms->Expiry = (int)pPCFInteger->Value;
		break;
	case MQIACF_PUB_PRIORITY:
		if (diagnostics>10) {
			fprintf(stderr, "(subscript.c) About to set PubPriorty\n");
		}
		SubscrParms->PubPriorty = (int)pPCFInteger->Value;
		break;
	case MQIACF_PUBSUB_PROPERTIES:
		if (diagnostics>10) {
			fprintf(stderr, "(subscript.c) About to set PubSubProps\n");
		}
		SubscrParms->PubSubProps = (int)pPCFInteger->Value;
		break;
	case MQIACF_REQUEST_ONLY:
		if (diagnostics>10) {
			fprintf(stderr, "(subscript.c) About to set ReqOnly\n");
		}
		SubscrParms->ReqOnly = (int)pPCFInteger->Value;
		break;
	case MQIACF_SUB_LEVEL:
		if (diagnostics>10) {
			fprintf(stderr, "(subscript.c) About to set SubLevel\n");
		}
		SubscrParms->SubLevel = (int)pPCFInteger->Value;
		break;
	case MQIACF_SUBSCRIPTION_SCOPE:
		if (diagnostics>10) {
			fprintf(stderr, "(subscript.c) About to set SubScope\n");
		}
		SubscrParms->SubScope = (int)pPCFInteger->Value;
		break;
	case MQIACF_SUB_TYPE:
		if (diagnostics>10) {
			fprintf(stderr, "(subscript.c) About to set SubType \n");
		}
		SubscrParms->SubType = (int)pPCFInteger->Value;
		break;
	case MQIACF_SELECTOR_TYPE:
		if (diagnostics>10) {
			fprintf(stderr, "(subscript.c) About to set SubType \n");
		}
		SubscrParms->SubType = (int)pPCFInteger->Value;
		break;
	case MQIACF_VARIABLE_USER_ID:
		if (diagnostics>10) {
			fprintf(stderr, "(subscript.c) About to set VarUser\n");
		}
		SubscrParms->VarUser = (int)pPCFInteger->Value;
		break;
	case MQIACF_WILDCARD_SCHEMA:
		if (diagnostics>10) {
			fprintf(stderr, "(subscript.c) About to set WildcardSchema\n");
		}
		SubscrParms->WildcardSchema =0;
		SubscrParms->WildcardSchema = (int)pPCFInteger->Value;
		break;

	default:
		fprintf(stderr, "(subscript.c) Unknown PCF Integer Parameter: %ld\n",
		pPCFInteger->Parameter);
		*iCC = WARNING;
		break;

	}
}

void ProcessSubscrByteParm(MQCFBS *pPCFByte, SubscrParms *SubscrParms,
		MQLONG *iCC) {
	switch (pPCFByte->Parameter) {
	case MQBACF_DESTINATION_CORREL_ID:
		if (diagnostics>10) {
			fprintf(stderr, "(subscript.c) About to set DestCorrID\n");
			fflush(stderr);
		}
		memset(SubscrParms->DestCorId, '\0', MQ_CORREL_ID_LENGTH);
		memcpy(SubscrParms->DestCorId, pPCFByte->String,
				(int)pPCFByte->StringLength);
		if (diagnostics>10) {
			fprintf(stderr,
			"(subscript.c) Set DestCorId with length %d to '%s'\n",
			(int)pPCFByte->StringLength, SubscrParms->DestCorId);
		}
		break;
	case MQBACF_SUB_ID:
		if (diagnostics>10) {
			fprintf(stderr, "(subscript.c) About to set SubID\n");
			fflush(stderr);
		}
		memset(SubscrParms->SubId, '\0', MQ_CORREL_ID_LENGTH);
		memcpy(SubscrParms->SubId, pPCFByte->String,
				(int)pPCFByte->StringLength);
		if (diagnostics>10) {
			fprintf(stderr, "(subscript.c) Set SubId with length %d to '%s'\n",
			(int)pPCFByte->StringLength, SubscrParms->SubId);
		}
		break;
	case MQBACF_ACCOUNTING_TOKEN:
		if (diagnostics>10) {
			fprintf(stderr, "(subscript.c) About to set AcctToken\n");
			fflush(stderr);
		}
		memset(SubscrParms->PubAcctTkn, '\0', MQ_ACCOUNTING_TOKEN_LENGTH);
		memcpy(SubscrParms->PubAcctTkn, pPCFByte->String,
				(int)pPCFByte->StringLength);
		if (diagnostics>10) {
			fprintf(stderr,
			"(subscript.c) Set PubAcctTkn with length %d to '%s'\n",
			(int)pPCFByte->StringLength, SubscrParms->PubAcctTkn);
		}
		break;
#ifdef zzzOS		
	case MQBACF_RESPONSE_ID: /* zOS things to ignore */
		break;
#endif
	default:
		fprintf(stderr, "(subscript.c) Unknown PCF Byte Parameter: %ld\n",
		pPCFByte->Parameter);
		*iCC = WARNING;
		break;

	}
}

void AddToFileSubscr(SubscrParms *SubscrParms, int QmgrVersion, char *fileName,
		FILE *fp, /* v51 */
		int twolines, /* v601 */
		MQLONG *iCC) {
	char * ParmBuffer; /* Temporary buffer to hold for output to file */
	int options = 0;
	char* work;
	char work2[8]; /* This must be 8 in size to avoid segfaults on linux... */
	int i;

	if ( !(ParmBuffer = (char *) malloc(PBSIZE*2) )) {
		fprintf(stderr, "(subscript.c) Failed to allocate %d bytes \n",
		PBSIZE*2);
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
	MQsprintf_X(ParmBuffer, "DEFINE SUB ('%s') ", SubscrParms->SubscrName, 
	MQ_SUB_NAME_LENGTH, options);
	fputs(ParmBuffer, fp);

	if (!oneLine || twolines) {
		MQsprintf_X(ParmBuffer, "* CRDATE (%s) ", SubscrParms->CreationDate, 
		MQ_CREATION_DATE_LENGTH, options);
		fputs(ParmBuffer, fp);
		MQsprintf_X(ParmBuffer, "* CRTIME (%s) ", SubscrParms->CreationTime, 
		MQ_CREATION_TIME_LENGTH, options);
		fputs(ParmBuffer, fp);
		if (header) {
			MQsprintf_X(ParmBuffer, "* ALTDATE (%s) ",
					SubscrParms->AlterationDate, 
					MQ_CREATION_DATE_LENGTH, options);
			fputs(ParmBuffer, fp);
			MQsprintf_X(ParmBuffer, "* ALTTIME (%s) ",
					SubscrParms->AlterationTime, 
					MQ_CREATION_TIME_LENGTH, options);
			fputs(ParmBuffer, fp);

			switch (SubscrParms->SubType) {
			case MQSUBTYPE_ALL:
				sprintf(ParmBuffer, "* SUBTYPE(ALL) %s", lineTerm);
				fputs(ParmBuffer, fp);
				break;
			case MQSUBTYPE_API:
				sprintf(ParmBuffer, "* SUBTYPE(API) %s", lineTerm);
				fputs(ParmBuffer, fp);
				break;
			case MQSUBTYPE_ADMIN:
				sprintf(ParmBuffer, "* SUBTYPE(ADMIN) %s", lineTerm);
				fputs(ParmBuffer, fp);
				break;
			case MQSUBTYPE_PROXY:
				sprintf(ParmBuffer, "* SUBTYPE(PROXY) %s", lineTerm);
				fputs(ParmBuffer, fp);
				break;
			case MQSUBTYPE_USER:
				sprintf(ParmBuffer, "* SUBTYPE(USER) %s", lineTerm);
				fputs(ParmBuffer, fp);
				break;
			}
			switch (SubscrParms->SelectorType) {
			case MQSELTYPE_NONE:
				sprintf(ParmBuffer, "* SELTYPE(NONE) %s", lineTerm);
				fputs(ParmBuffer, fp);
				break;
			case MQSELTYPE_STANDARD:
				sprintf(ParmBuffer, "* SELTYPE(STANDARD) %s", lineTerm);
				fputs(ParmBuffer, fp);
				break;
			case MQSELTYPE_EXTENDED:
				sprintf(ParmBuffer, "* SELTYPE(EXTENDED) %s", lineTerm);
				fputs(ParmBuffer, fp);
				break;
			}
			work = (char*)malloc((MQ_CORREL_ID_LENGTH+1)*2);
			memset(work, '\0', sizeof(MQ_CORREL_ID_LENGTH*2));
			for (i=0; i<MQ_CORREL_ID_LENGTH; i++) {
				sprintf(work2, "%02X", SubscrParms->DestCorId[i]);
				strcat(work, work2);
			}

			MQsprintf_X(ParmBuffer, "* DESTCORL(%s) ", work, 
			MQ_CORREL_ID_LENGTH*2, options);
			fputs(ParmBuffer, fp);
			free(work);
			switch (SubscrParms->DurSub) {
			case MQSUB_DURABLE_ALL:
				sprintf(ParmBuffer, "* DURABLE(ALL) %s", lineTerm);
				fputs(ParmBuffer, fp);
				break;
			case MQSUB_DURABLE_YES:
				sprintf(ParmBuffer, "* DURABLE(YES) %s", lineTerm);
				fputs(ParmBuffer, fp);
				break;
			case MQSUB_DURABLE_NO:
				sprintf(ParmBuffer, "* DURABLE(NO) %s", lineTerm);
				fputs(ParmBuffer, fp);
				break;
			}
			work = (char*)malloc((MQ_CORREL_ID_LENGTH+1)*2);
			memset(work, '\0', MQ_CORREL_ID_LENGTH*2);
			for (i=0; i<MQ_CORREL_ID_LENGTH; i++) {
				sprintf(work2, "%02X", SubscrParms->SubId[i]);
				strcat(work, work2);
			}

			MQsprintf_X(ParmBuffer, "* SUBID(%s) ", work, 
			MQ_CORREL_ID_LENGTH*2, options);
			fputs(ParmBuffer, fp);
			free(work);
		}
	}

	MQsprintf_X(ParmBuffer, "       DESTQMGR('%s') ", SubscrParms->DestQmgr, 
	MQ_Q_MGR_NAME_LENGTH, options);
	fputs(ParmBuffer, fp);

	if (SubscrParms->DestClass== MQDC_PROVIDED) {
		MQsprintf_X(ParmBuffer, "       DEST('%s') ", SubscrParms->Destination, 
		MQ_Q_NAME_LENGTH, options);
		fputs(ParmBuffer, fp);
		sprintf(ParmBuffer, "       DESTCLAS(PROVIDED) %s", lineTerm);
		fputs(ParmBuffer, fp);

	} else {
		sprintf(ParmBuffer, "       DESTCLAS(MANAGED) %s", lineTerm);
		fputs(ParmBuffer, fp);
		if (!oneLine || twolines) {
			sprintf(ParmBuffer,
					"* DEST is not valid with DESTCLAS(MANAGED) %s", lineTerm);
			MQsprintf_X(ParmBuffer, "* DEST('%s') ", SubscrParms->Destination, 
			MQ_Q_NAME_LENGTH, options);
			fputs(ParmBuffer, fp);
		}
	}

	if (SubscrParms->Expiry == MQEI_UNLIMITED) {
		sprintf(ParmBuffer, "       EXPIRY(UNLIMITED) %s", lineTerm);
		fputs(ParmBuffer, fp);

	} else {
		sprintf(ParmBuffer, "       EXPIRY(%ld) %s", SubscrParms->Expiry,
				lineTerm);
		fputs(ParmBuffer, fp);
	}

	work = (char*)malloc((MQ_ACCOUNTING_TOKEN_LENGTH+1)*2);
	memset(work, '\0', MQ_ACCOUNTING_TOKEN_LENGTH*2);
	for (i=0; i< MQ_ACCOUNTING_TOKEN_LENGTH; i++) {
		sprintf(work2, "%02X", SubscrParms->PubAcctTkn[i]);
		strcat(work, work2);
	}
	MQsprintf_X(ParmBuffer, "       PUBACCT(%s) ", work,
			(MQ_ACCOUNTING_TOKEN_LENGTH*2), options);
	fputs(ParmBuffer, fp);
	free(work);

	MQsprintf_X(ParmBuffer, "       PUBAPPID('%s') ", SubscrParms->PubApplId, 
	MQ_APPL_IDENTITY_DATA_LENGTH, options);
	fputs(ParmBuffer, fp);

	if (SubscrParms->PubPriorty == MQPRI_PRIORITY_AS_PUBLISHED) {
		sprintf(ParmBuffer, "       PUBPRTY(ASPUB) %s", lineTerm);
		fputs(ParmBuffer, fp);

	} else if (SubscrParms->PubPriorty == MQPRI_PRIORITY_AS_Q_DEF) {
		sprintf(ParmBuffer, "       PUBPRTY(ASQDEF) %s", lineTerm);
		fputs(ParmBuffer, fp);
	} else {
		sprintf(ParmBuffer, "       PUBPRTY(%1ld) %s", SubscrParms->PubPriorty,
				lineTerm);
		fputs(ParmBuffer, fp);
	}

	switch (SubscrParms->PubSubProps) {
	case MQPSPROP_NONE:
		sprintf(ParmBuffer, "       PSPROP(NONE) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
	case MQPSPROP_COMPAT:
		sprintf(ParmBuffer, "       PSPROP(COMPAT) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
	case MQPSPROP_MSGPROP:
		sprintf(ParmBuffer, "       PSPROP(MSGPROP) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
	case MQPSPROP_RFH2:
		sprintf(ParmBuffer, "       PSPROP(RFH2) %s", lineTerm);
		fputs(ParmBuffer, fp);
		break;
	}

	if (SubscrParms->ReqOnly == MQRU_PUBLISH_ON_REQUEST) {
		sprintf(ParmBuffer, "       REQONLY(YES) %s", lineTerm);
		fputs(ParmBuffer, fp);
	} else {
		sprintf(ParmBuffer, "       REQONLY(NO) %s", lineTerm);
		fputs(ParmBuffer, fp);
	}

	if (strlen(SubscrParms->SubSelector)!=0) {
		MQsprintf_X(ParmBuffer, "       SELECTOR('%s') ",
				SubscrParms->SubSelector, 
				MQ_SELECTOR_LENGTH, options);
		fputs(ParmBuffer, fp);
	} else {
		sprintf(ParmBuffer, "       SELECTOR('') %s", lineTerm);
		fputs(ParmBuffer, fp);

	}
	sprintf(ParmBuffer, "       SUBLEVEL(%1ld) %s", SubscrParms->SubLevel,
			lineTerm);
	fputs(ParmBuffer, fp);

	if (SubscrParms->SubScope == MQTSCOPE_ALL) {
		sprintf(ParmBuffer, "       SUBSCOPE(ALL) %s", lineTerm);
		fputs(ParmBuffer, fp);
	} else {
		sprintf(ParmBuffer, "       SUBSCOPE(QMGR) %s", lineTerm);
		fputs(ParmBuffer, fp);
	}

	MQsprintf_X(ParmBuffer, "       SUBUSER('%s') ", SubscrParms->SubUser,
			(int)MQ_USER_ID_LENGTH, options);
	fputs(ParmBuffer, fp);

	MQsprintf_X(ParmBuffer, "       TOPICSTR('%s') ", SubscrParms->TopicString, 
	MQ_TOPIC_STR_LENGTH, options);
	fputs(ParmBuffer, fp);

	MQsprintf_X(ParmBuffer, "       TOPICOBJ('%s') ", SubscrParms->TopicName, 
	MQ_TOPIC_NAME_LENGTH, options);
	fputs(ParmBuffer, fp);

	MQsprintf_X(ParmBuffer, "       USERDATA('%s') ", SubscrParms->SubUserData, 
	MQ_USER_DATA_LENGTH, options);
	fputs(ParmBuffer, fp);

	if (SubscrParms->VarUser == MQVU_ANY_USER) {
		sprintf(ParmBuffer, "       VARUSER(ANY) %s", lineTerm);
		fputs(ParmBuffer, fp);
	} else {
		sprintf(ParmBuffer, "       VARUSER(FIXED) %s", lineTerm);
		fputs(ParmBuffer, fp);
	}

	if (SubscrParms->WildcardSchema == MQWS_CHAR) {
		sprintf(ParmBuffer, "       WSCHEMA(CHAR) %s", lineTerm);
		fputs(ParmBuffer, fp);
	} else if (SubscrParms->WildcardSchema == MQWS_TOPIC) {
		sprintf(ParmBuffer, "       WSCHEMA(TOPIC) %s", lineTerm);
		fputs(ParmBuffer, fp);
	}
	sprintf(ParmBuffer, "       REPLACE\n");
	fputs(ParmBuffer, fp);
	if (!oneLine) {
		fputs("\n", fp);
	}

}

#endif

