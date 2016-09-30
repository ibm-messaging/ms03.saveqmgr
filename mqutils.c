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
/*==========================================================================*/
/* Module Name: MQUTILS.C                                                   */
/*  DESCRIPTIVE NAME WebSphere MQ Save Queue Manager Object                  */
/*                  Definitions using PCFs (ms03 supportpac)                 */
/*                                                                           */
/*==========================================================================*/

/* Include standard libraries */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* Include MQSeries headers */
#include <cmqc.h>
#include <cmqcfc.h>
#include <cmqxc.h>

/* Include Application headers */
#include "saveqmgr.h"
#include "mqutils.h"
extern int diagnostics;
extern int noMsgOnTimeout;
extern int oneLine;


MQLONG OpenQ(MQHCONN hConn, MQCHAR48 QMgrName, MQCHAR48 QName,
		MQCHAR48 *DynamicQName, MQLONG OpenOpts, MQLONG *pHOpenQ) {
	MQLONG CompCode = 0;
	MQLONG Reason = 0;
	MQOD ObjDesc = { MQOD_DEFAULT};

	/* ******************************************************************** */
	/* If QName specifies the name of a model queue then we are going to    */
	/* open a dynamic queue, the properties of which are specified by the   */
	/* model queue definition. DynamicQName then represents the name to be  */
	/* given to the dynamic queue. If there is an asterisk anywhere in the  */
	/* first 33 character positions it will be replaced by a generated a    */
	/* unique name. The actual name is returned in place of the model       */
	/* queue used and is stored for future use.                             */
	/* ******************************************************************** */

	memcpy( ObjDesc.ObjectName, QName, MQ_Q_NAME_LENGTH );
	memcpy( ObjDesc.ObjectQMgrName, QMgrName, MQ_Q_MGR_NAME_LENGTH );
	if ( DynamicQName != NULL ) {
		memcpy( ObjDesc.DynamicQName, *DynamicQName, MQ_Q_NAME_LENGTH );
	} /* endif */

	MQOPEN( hConn /* I  : Queue manager handle   */
			, &ObjDesc /* IO : queue attributes       */
			, OpenOpts /* I  :                        */
			, pHOpenQ /*  O : handle to open queue   */
			, &CompCode /*  O : MQCC_OK/Warning/Error       */
			, &Reason /*  O : Reason for above       */
	);

	if (CompCode != MQCC_OK ) {
		fprintf(stderr, "(mqutils.c) MQOPEN failed for %s, CC=%ld RC=%ld\n"
				, QName
				, CompCode
				, Reason
		);
	} /* endif */

	/* --------------------------------------------------------------- */
	/* If we have opened a dynamic queue then the actual name supplied */
	/* has been returned in the object descriptor in place of the      */
	/* supplied model queue name                                       */
	/* --------------------------------------------------------------- */
	if ( DynamicQName != NULL ) {
		memcpy( *DynamicQName, ObjDesc.ObjectName, MQ_Q_NAME_LENGTH );
	} /* endif */

	return( CompCode );

}

MQLONG PutMsg(MQHCONN hConn /* Connection to queue manager            */
, MQCHAR8 MsgFormat /* Format of user data to be put in msg   */
, MQHOBJ hQName /* handle of queue to put the message to  */
, MQCHAR48 QName /* name of queue to put the message to    */
, MQCHAR48 ReplyToQ /* name of q on which to receive replies  */
, MQBYTE *UserMsg /* The user data to be put in the message */
, MQLONG UserMsgLen /*                                        */
, MQCHAR12 UserID
)

{
	MQMD MsgDesc = { MQMD_DEFAULT}; /* Message description        */
	MQPMO PutMsgOpts = {MQPMO_DEFAULT}; /* Controls action of MQPUT   */
	MQLONG CompCode; /* Completion code            */
	MQLONG Reason; /* reason qualifying CompCode */
	int i;

	/********************************/
	/* Set up message header fields */
	/********************************/

	MsgDesc.Persistence = MQPER_NOT_PERSISTENT;
	memset( MsgDesc.ReplyToQMgr, '\0', MQ_Q_MGR_NAME_LENGTH );
	MsgDesc.MsgType = MQMT_REQUEST;
	memcpy( MsgDesc.ReplyToQ, ReplyToQ, MQ_Q_NAME_LENGTH );
	memcpy( MsgDesc.Format, MsgFormat, MQ_FORMAT_LENGTH );

	MsgDesc.Expiry = 1800; /* 3 minutes */

	PutMsgOpts.Options = MQPMO_NO_SYNCPOINT;

	if (diagnostics>10) {
		fprintf(stderr, "\n(mqutils.c) ReplyToQ: %.48s",MsgDesc.ReplyToQ );
	}

	if (diagnostics>10) {
		fprintf(stderr, "\n(mqutils.c) *** Putting to command server ***");
		dumpInHex(UserMsg, UserMsgLen);
	}

	MQPUT( hConn /* I  : Queue manager handle     */
			, hQName /* I  : Queue handle             */
			, &MsgDesc /* IO : Message attributes       */
			, &PutMsgOpts /* IO : Control action of MQPUT  */
			, UserMsgLen /* I  : User message length      */
			, UserMsg /* I  : User message             */
			, &CompCode /*  O : MQCC_OK/Warning/Error         */
			, &Reason /*  O : Reason for above         */
	);

	if (diagnostics>10) {
		fprintf(stderr, "\n  MsgId    : X'");
		for (i = 0; i < MQ_MSG_ID_LENGTH; i++)
		fprintf(stderr, "%02X",MsgDesc.MsgId[i] );
		fprintf(stderr, "'\n");
	}

	if ( CompCode ) {
		fprintf(stderr, "(mqutils.c) MQPUT failed for queue %s. CC=%ld RC=%ld\n"
				, QName
				, CompCode
				, Reason
		);

	} /* endif */

	return (CompCode);

} /* end PutMsg */

MQLONG GetMsg(MQHCONN hConn /* handle of queue manager            */
, MQHOBJ hQName /* handle of queue to read from       */
, MQBYTE **UserMsg /* Input/Output buffer containing msg */
, MQLONG *ReadBufferLen /* Length of supplied buffer          */
, CmdArgParmsPtr MyParms
) {
	MQMD MsgDesc; /* Message descriptor            */
	MQMD MsgDesc_V = { MQMD_DEFAULT}; /* Message descriptor (def values) */
	MQGMO GetMsgOpts = {MQGMO_DEFAULT}; /* Options to control MQGET      */
	MQLONG UserMsgLen; /* Actual length received        */
	MQLONG CompCode; /* Completion code               */
	MQLONG Reason; /* reason qualifying CompCode    */
	int i=0;

	GetMsgOpts.Options = MQGMO_WAIT + MQGMO_CONVERT + MQGMO_FAIL_IF_QUIESCING;

	/* v531a wait less time for last msg if it isn't going to arrive */
	if (noMsgOnTimeout) {
		GetMsgOpts.WaitInterval = MyParms->shortWaitTime;
	} else {
		GetMsgOpts.WaitInterval = MyParms->longWaitTime;
	}
	/* [v520] on Tandem NSK, you MUST use the no syncpoint option on mqget */

#if defined(_OSS_TARGET) || defined(_GUARDIAN_TARGET)
	GetMsgOpts.Options += MQGMO_NO_SYNCPOINT;
#endif

	do {

		/* if the get fails becuase of truncated message, and we're getting from */
		/* a platform with different encoding / ccsid (like zLinux), then we     */
		/* need to reset those fields in the MQMD, so this will keep everything good */
		memcpy(&MsgDesc, &MsgDesc_V, sizeof(MsgDesc));

		if (diagnostics>10) {
			fprintf(stderr, "(mqutils.c) Before MQGET: Options: %ld, Encoding: %ld, CCSID: %ld, WaitInterval: %ld\n",
					GetMsgOpts.Options, MsgDesc.Encoding, MsgDesc.CodedCharSetId, GetMsgOpts.WaitInterval);
		}

		MQGET( hConn /* I  : Queue manager handle              */
				, hQName /* I  : handle to QName to read from      */
				, &MsgDesc /* IO : message attributes                */
				, &GetMsgOpts /* IO : control action of MQGET           */
				, *ReadBufferLen /* I  : length of supplied buffer         */
				, *UserMsg /*  O : User data to be put               */
				, &UserMsgLen /*  O : Actual length of returned message */
				, &CompCode /*  O : MQCC_OK/Warning/Error                  */
				, &Reason /*  O : Reason for above                  */
		);

		if (diagnostics>10) {
			fprintf(stderr, "(mqutils.c) After MQGET: RC: %ld, Options: %ld, Encoding: %ld, CCSID: %ld, Format '%.8s'\n",
					Reason,
					GetMsgOpts.Options,
					MsgDesc.Encoding,
					MsgDesc.CodedCharSetId,
					MsgDesc.Format
			);
		}
		if (diagnostics>10 && !Reason) {
			fprintf(stderr, "(mqutil.c) *** Got message from command server ***");
			fprintf(stderr, "\n  CorrelId : X'");
			for (i = 0; i < MQ_CORREL_ID_LENGTH; i++)
			fprintf(stderr, "%02X",MsgDesc.CorrelId[i] );
			fprintf(stderr,"'\n");
			dumpInHex(*UserMsg, UserMsgLen);
		}

		if (Reason==MQRC_TRUNCATED_MSG_FAILED) {
			if (*UserMsg) free(*UserMsg);
			*ReadBufferLen = UserMsgLen;
			if (!(*UserMsg=malloc(*ReadBufferLen))) {
				fprintf(stderr, "(mqutils.c) Failed to allocate %ld bytes\n",*ReadBufferLen);
				CompCode = MQCC_FAILED;
				Reason = MQRC_ENVIRONMENT_ERROR;
			}/* End if*/
		}/* End if*/
		if (Reason == MQRC_UNIT_OF_WORK_NOT_STARTED) {
/*			Hackery for retrying when we need to specify MQGMO_NO_SYNCPOINT but failed to notice. */
			GetMsgOpts.Options += MQGMO_NO_SYNCPOINT;
			Reason = MQRC_TRUNCATED_MSG_FAILED;  /* this is the hackery.  Reset this so that we redo the get. */
		}

	}while (Reason==MQRC_TRUNCATED_MSG_FAILED); /* End do while */
	
	if ( CompCode ) {
		if (Reason == MQRC_NO_MSG_AVAILABLE) {
			if (! noMsgOnTimeout) {
				fprintf(stderr, "(mqutils.c) No reply msg, make sure command server is started (strmqcsv qmgr-name)\n");
			} else {
				fprintf(stderr, "(mqutils.c) PCF error(s) have been encountered during this run\n");
			}
		} else {
			fprintf(stderr, "(mqutils.c) MQGET failed for reply queue. MQCC=%ld MQRC=%ld\n"
					, CompCode
					, Reason
			);
		}
	} else { /* good comp code */
		if (memcmp(MsgDesc.Format, MQFMT_ADMIN, strlen(MQFMT_ADMIN))) {
			fprintf(stderr, "(mqutils.c) Received message is not of type MQADMIN (received \"%.8s\")\n", MsgDesc.Format);
			CompCode = MQCC_FAILED;
			Reason = MQRC_FORMAT_NOT_SUPPORTED;

		}
	}

	return (CompCode);

}

#if 0
/* ------------------------------------------------------------------------ */
/* MQsprintf is used to write the value of the string attributes to our     */
/* file.                                                                    */
/*                                                                          */
/* The text of the insert is exactly as it was returned by the queue        */
/* manager during processing by the command server. That is, a string       */
/* of characters which indicate the value of a specific attribute, padded   */
/* with blanks. We will change these blanks to nulls.                       */
/*                                                                          */
/* It is possible that the text to be written, for instance a queue         */
/* name, is the maximum length for the object concerned. In this case the   */
/* string is not null terminated so sprintf would not behave as we intend.  */
/* For this reason, MQsprintf assumes only one text insert and takes an     */
/* additional parameter indicating the maximum length the the particular    */
/* attribute being written.                                                 */
/* ------------------------------------------------------------------------ */

/* The "classic" MQsprintf which does it all */

void MQsprintf( char *ParmBuffer
		, char *Format
		, char *Insert
		, int Length
)
{
	MQsprintf_X( ParmBuffer
			, Format
			, Insert
			, Length
			,MQSPRINTF_X_LINE_BREAKS
			+ MQSPRINTF_X_DOUBLE_QUOTES
			+ MQSPRINTF_X_NO_TRAILING_BLANKS
	);
}

#endif

/* [v530] The "new" MQsprintf which accepts options */

void MQsprintf_X(char *ParmBuffer, char *Format, char *Insert, int Length,
		int Options) {
	char *MQParm = NULL;
	char *MQParm2 = NULL;
	int mlen = 0;
	if (diagnostics>10) {
		fprintf(stderr, "(mqutils.c) About to print '%s' with '%.*s'\n",Format,Length,Insert);
		fflush(stderr);
	}	

	/* Allocate a temporary buffer larger than the               */
	/* maximum allowed for this attribute. Initialise to nulls.  */
	/* This will ensure that the buffer is null terminated when  */
	/* subsequently passed to sprintf.                           */

	mlen = 2 * (strlen(Format) + Length);
	if (!(MQParm = (char *)malloc(mlen)) || !(MQParm2 = (char *)malloc(mlen))) {
		fprintf(stderr, "(mqutils.c) Error allocating %d bytes in MQsprintf\n",
				mlen);
		exit(2);
	}
	memset(MQParm, '\0', mlen);
	memset(MQParm2, '\0', mlen);
	if (Options & MQSPRINTF_X_NO_TRAILING_BLANKS) 
		StripTrailingBlanks(MQParm, Insert, Length);
	else 
		strncpy(MQParm, Insert, Length);

	/* Now, if the string is null, make it a single blank */
	/* unless the caller really wanted a null returned */

	if (!(Options & MQSPRINTF_X_NULL_IF_BLANK) && (MQParm[0]=='\0')) 
		MQParm[0] = ' ';

	if (Options & MQSPRINTF_X_DOUBLE_QUOTES) 
		DoubleUpQuotes(MQParm2, MQParm);
	else 
		strcpy(MQParm2, MQParm);
	/* Format the string  */
	sprintf(MQParm, Format, MQParm2);
	
	if (Options & MQSPRINTF_X_LINE_BREAKS)
		InsertNewlines(ParmBuffer, MQParm);
	else
		strcpy(ParmBuffer, MQParm);

	free(MQParm);
	free(MQParm2);
}

/*****************************************************************************/
/* [v512] wms                                                                */
/* This routine converts a string list into a string of the form:            */
/* 'exit1', 'exit2', 'exit3'                                                 */
/*****************************************************************************/

void makeStringFromList(char* targetString, char * sourceString,
		MQLONG stringLength, MQLONG stringCount) {

#define STRENDNL "', +\n          '"
#define STRENDNONL "', '"

	int i = 0;
	char * pT = NULL;
	char * pS = NULL;
	char * j = NULL;
	int moveLen = 0;

	char strend[32]; /* leave some extra room */

	/* v603 -- fix for "oneline" output */
	if (oneLine)
		strcpy(strend, STRENDNONL);
	else
		strcpy(strend, STRENDNL);

	pS = sourceString;
	pT = targetString;

	if (stringCount) {
		strcpy(pT, "'"); /* opening quote */
		for (i = 0; i< stringCount; i++) {
			/* trim trailing blanks */
			for (j = pS + stringLength - 1; j >= pS && *(j)==' '; j--)
				;
			/* move from the source to the target */
			moveLen = j-pS+1;
			strncat(pT, pS, moveLen);
			/* and end the string nicely (also starting the next string) */
			strcat(pT, strend);
			/* point to next element */
			pS+=stringLength;
		}
		strcat(pT, "'"); /* closing quote */
	} else {
		strcpy(pT, "' '");
	}

	/* trim off the ending quotes and comma */
	if (strlen(pT) >= strlen(strend))
		*(pT + strlen(pT) - strlen(strend)) = '\0';

	return;

}

/* [v530] small rtn to Double up single quotes  */

void DoubleUpQuotes(char * MQParm2, char * MQParm) {

	/* Go through the string, */
	/* changing any single quotes to two single quotes */
	/* Copying to MQParm2 as we go along */
	int scounter = 0;
	int tcounter = 0;
	for (scounter = 0; scounter < strlen(MQParm); scounter++) {
		MQParm2[tcounter++] = MQParm[scounter];
		if (MQParm2[tcounter-1]=='\'')
			MQParm2[tcounter++]='\'';
	}
}

/* [v530] Remove trailing blanks */

void StripTrailingBlanks(char * MQParm, char * Insert, int Length) {

	int scounter = Length - 1;
	/* Scan backwards until we find a non-blank character */
	while (scounter >= 0 && Insert[scounter] == ' ')
		scounter--;
	/* Copy the original INSERT string into MQParm buffer */
	while (scounter >= 0) {
		MQParm[scounter] = Insert[scounter];
		scounter--;
	} /* endwhile */

}

/* [v530] */
/* We need to assure that no lines are more than 70 characters  ... */
/* The following loop newlines the string into  */
/* maximum 70 character chunks */
/* Copy into the Return ParmBuffer area as we go along */

void InsertNewlines(char * ParmBuffer, char * MQParm) {

#define LINEBREAKEND   "-\n"
	int curlen = 0;
	int tcounter = 0;
	int scounter = 0;

	for (scounter=0; scounter < strlen(MQParm); scounter++) {
		ParmBuffer[tcounter++] = MQParm[scounter];
		curlen++;
		/* if we are processing a string list, they'll have newlines in them */
		if (ParmBuffer[tcounter-1]=='\n') {
			curlen = 0;
		} else if (!(curlen % (70-strlen(LINEBREAKEND)))
				&& (strlen(&MQParm[scounter]) > strlen(LINEBREAKEND))) {
			ParmBuffer[tcounter] = '\0';
			strcat(ParmBuffer, LINEBREAKEND);
			tcounter += strlen(LINEBREAKEND);
			curlen = 0;
		} /* end if !curlen ... */
	} /* End for*/

	/* Null terminate the buffer we are returing */
	ParmBuffer[tcounter] = '\0';
	/* v601 ... we add the +\n here now to allow for sane oneLine processing */
	strcat(ParmBuffer, "+\n");

}

void toStrMQEVR(char * s, MQLONG v) {
	switch (v) {
	case MQEVR_DISABLED:
		strcpy(s, "DISABLED");
		break;
	case MQEVR_ENABLED:
		strcpy(s, "ENABLED");
		break;
#if defined(zzMQV60)
	case MQEVR_EXCEPTION:
		strcpy(s, "EXCEPTION");
		break;
	case MQEVR_NO_DISPLAY:
		strcpy(s, "NODISPLAY");
		break;
#endif
	default:
		sprintf(s, "%ld", v);
		fprintf(stderr, "(mqutils.c) Unknown MQEVR value %ld\n", v);
	}
}

#if defined(zzMQV60)
void toStrMQMON(char * s, MQLONG v) {
	switch (v) {
	case MQMON_NONE:
		strcpy(s, "NONE");
		break;
	case MQMON_Q_MGR:
		strcpy(s, "QMGR");
		break;
	case MQMON_OFF:
		strcpy(s, "OFF");
		break;
	case MQMON_ON:
		strcpy(s, "ON");
		break;
	case MQMON_LOW:
		strcpy(s, "LOW");
		break;
	case MQMON_MEDIUM:
		strcpy(s, "MEDIUM");
		break;
	case MQMON_HIGH:
		strcpy(s, "HIGH");
		break;
	default:
		sprintf(s, "%ld", v);
		fprintf(stderr, "(mqutils.c) Unknown MON/STAT/ACCT value %ld\n", v);
	}
}

void toStrMQREC(char * s, MQLONG v) {
	switch (v) {
	case MQRECORDING_DISABLED:
		strcpy(s, "DISABLED");
		break;
	case MQRECORDING_Q:
		strcpy(s, "QUEUE");
		break;
	case MQRECORDING_MSG:
		strcpy(s, "MSG");
		break;
	default:
		sprintf(s, "%ld", v);
		fprintf(stderr, "(mqutils.c) Unknown MQREC value %ld\n", v);
	}
}
void toStrMQSVC(char * s, MQLONG v) {
	switch (v) {
	case MQSVC_CONTROL_Q_MGR:
		strcpy(s, "QMGR");
		break;
	case MQSVC_CONTROL_Q_MGR_START:
		strcpy(s, "STARTONLY");
		break;
	case MQSVC_CONTROL_MANUAL:
		strcpy(s, "MANUAL");
		break;
	default:
		sprintf(s, "%ld", v);
		fprintf(stderr, "(mqutils.c) Unknown MQSVC value %ld\n", v);
	}
}
#if defined(zzzOS)
void toStrQSG(char * s, MQLONG v) {
	switch (v) {
	case MQQSGD_ALL:
		strcpy(s, "ALL");
		break;
	case MQQSGD_Q_MGR:
		strcpy(s, "QMGR");
		break;
	case MQQSGD_COPY:
		strcpy(s, "COPY");
		break;
	case MQQSGD_SHARED:
		strcpy(s, "SHARED");
		break;
	case MQQSGD_GROUP:
		strcpy(s, "GROUP");
		break;
	case MQQSGD_PRIVATE:
		strcpy(s, "PRIVATE");
		break;
	case MQQSGD_LIVE:
		strcpy(s, "LIVE");
		break;
	default:
		sprintf(s, "%ld", v);
		fprintf(stderr, "(mqutils.c) Unknown QSG value %ld\n", v);
	}
}

void toStrINDXTYPE(char * s, MQLONG v) {
	switch (v) {
	case MQIT_NONE:
		strcpy(s, "NONE");
		break;
	case MQIT_MSG_ID:
		strcpy(s, "MSGID");
		break;
	case MQIT_CORREL_ID:
		strcpy(s, "CORRELID");
		break;
	case MQIT_MSG_TOKEN:
		strcpy(s, "MSGTOKEN");
		break;
	case MQIT_GROUP_ID:
		strcpy(s, "GROUPID");
		break;
	default:
		sprintf(s, "%ld", v);
		fprintf(stderr, "(mqutils.c) Unknown INDXTYPE value %ld\n", v);
	}

}

void toStrNLType(char * s, MQLONG v) {

	switch (v) {
	case MQNT_NONE:
		strcpy(s, "NONE");
		break;
	case MQNT_Q:
		strcpy(s, "QUEUE");
		break;
	case MQNT_CLUSTER:
		strcpy(s, "CLUSTER");
		break;
	case MQNT_AUTH_INFO:
		strcpy(s, "AUTHINFO");
		break;
	default:
		sprintf(s, "%ld", v);
		fprintf(stderr, "(mqutils.c) Unknown NLType value %ld\n", v);
	}
}
void toStrIGQPA(char * s, MQLONG v) {

	switch (v) {
	case MQIGQPA_DEFAULT:
		strcpy(s, "DEF");
		break;
	case MQIGQPA_CONTEXT:
		strcpy(s, "CTX");
		break;
	case MQIGQPA_ONLY_IGQ:
		strcpy(s, "ONLYIGQ");
		break;
	case MQIGQPA_ALTERNATE_OR_IGQ:
		strcpy(s, "ALTIGQ");
		break;
	default:
		sprintf(s, "%ld", v);
		fprintf(stderr, "(mqutils.c) Unknown IGQ Put Authority value %ld\n", v);
	}
}
void toStrRTT(char * s, MQLONG v) {

	switch (v) {
	case MQRCVTIME_MULTIPLY:
		strcpy(s, "MULTIPLY");
		break;
	case MQRCVTIME_ADD:
		strcpy(s, "ADD");
		break;
	case MQRCVTIME_EQUAL:
		strcpy(s, "EQUAL");
		break;
	default:
		sprintf(s, "%ld", v);
		fprintf(stderr, "(mqutils.c) Unknown ReceiveTimeoutType value %ld\n", v);
	}
}
void toStrANMC(char * s, MQLONG v) {

	switch (v) {
	case MQADOPT_CHECK_ALL:
		strcpy(s, "ALL");
		break;
	case MQADOPT_CHECK_NET_ADDR:
		strcpy(s, "NETADDR");
		break;
	case MQADOPT_CHECK_NONE:
		strcpy(s, "NONE");
		break;
	case MQADOPT_CHECK_Q_MGR_NAME:
		strcpy(s, "QMNAME");
		break;
	default:
		sprintf(s, "%ld", v);
		fprintf(stderr, "(mqutils.c) Unknown AdoptNewMCACheck value %ld\n", v);
	}
}

#endif  /* end of zzzOS */

void toStrMQCLWL(char * s, MQLONG v) {
	switch (v) {
	case MQCLWL_USEQ_ANY:
		strcpy(s, "ANY");
		break;
	case MQCLWL_USEQ_LOCAL:
		strcpy(s, "LOCAL");
		break;
	case MQCLWL_USEQ_AS_Q_MGR:
		strcpy(s, "QMGR");
		break;
	default:
		sprintf(s, "QMGR", v);
		fprintf(stderr,
				"(mqutils.c) Unknown MQCLWL value %ld, setting to \"QMGR\"\n",
				v);
	}

}

void toStrMQComp(char * s, MQLONG v[]) {
	int i=0;

	*s='\0';

	for (i=0; i<COMP_ARRAY_SIZE; i++) {
		if (v[i] == MQCOMPRESS_NOT_AVAILABLE)
			break;
		switch (v[i]) {
		case MQCOMPRESS_NONE:
			strcat(s, "NONE,");
			break;
		case MQCOMPRESS_RLE:
			strcat(s, "RLE,");
			break;
		case MQCOMPRESS_ZLIBFAST:
			strcat(s, "ZLIBFAST,");
			break;
		case MQCOMPRESS_ZLIBHIGH:
			strcat(s, "ZLIBHIGH,");
			break;
		case MQCOMPRESS_SYSTEM:
			strcat(s, "SYSTEM,");
			break;
		case MQCOMPRESS_ANY:
			strcat(s, "ANY,");
			break;
		default:
			sprintf(s, "%ld", v[i]);
			fprintf(stderr, "(mqutils.c) Unknown COMPRESSION value %ld\n", v[i]);
		} /* end switch */
	} /* end for loop */

	/* remove the trailing comma */
	*(s+(strlen(s) ? strlen(s)-1 : 0))='\0';

}
#endif  /* end of if v6 compile */

void toStrMQPLATFORM(char * s, MQLONG v) {
	switch (v) {
	case MQPL_OS2:
		strcpy(s, "* PLATFORM (OS2)");
		break;
	case MQPL_OS400:
		strcpy(s, "OS400");
		break;
	case MQPL_UNIX:
		strcpy(s, "UNIX");
		break;
	case MQPL_WINDOWS_NT:
		strcpy(s, "WINDOWSNT");
		break;
	case MQPL_NSK:
		strcpy(s, "NSK");
		break;
	case MQPL_VMS:
		strcpy(s, "OPENVMS");
		break;
#if defined (MQPL_MVS)
	case MQPL_MVS:
		strcpy(s, "MVS");
		break;
#endif
	default:
		sprintf(s, "%ld", v);
		fprintf(stderr, "(mqutils.c) Unknown PLATFORM value %ld\n", v);
	}

}

void dumpInHex(MQBYTE Buffer[], MQLONG DataLength) {
#define    CHARS_PER_LINE  16  /* Used in formatting the message */
	int ch=0;
	int chars_this_line=0;
	char line_text[CHARS_PER_LINE + 4]; /* allows for up to 3 MBCS bytes overrun */

	fprintf(stderr, " length - %ld bytes ", DataLength);
	do {
		chars_this_line = 0;
		fprintf(stderr, "\n%08X: ", ch);
		while ( (chars_this_line < CHARS_PER_LINE) && (ch < DataLength)) {
			if (chars_this_line % 2 == 0)
				fprintf(stderr, " ");
			fprintf(stderr, "%.02X", (unsigned char)Buffer[ch] );
			line_text[chars_this_line] = isprint(Buffer[ch]) ? Buffer[ch] : '.';
			chars_this_line++;
			ch++;
		}
		/* pad with blanks to format the last line correctly */
		if (chars_this_line < CHARS_PER_LINE) {
			for (; chars_this_line < CHARS_PER_LINE; chars_this_line++) {
				if (chars_this_line % 2 == 0)
					fprintf(stderr, " ");
				fprintf(stderr, "  ");
				line_text[chars_this_line] = ' ';
			}
		}
		line_text[chars_this_line] = '\0';
		fprintf(stderr, " '%s'", line_text);
	} while (ch < DataLength);
	fprintf(stderr, "\n");
}
