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
#ifndef MQUTILS__
#define MQUTILS__
/* Include standard libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Include WebSphere MQ headers */
#include <cmqc.h>
#include <cmqcfc.h>
#include <cmqxc.h>

#include "args.h"
/* v6.1.2 this file entirely new, enhanced argument processing */


/* ******************** */
/* Defined in MQUTILS.C */
/* ******************** */
void StripTrailingBlanks(char *, char *, int);
void DoubleUpQuotes(char *, char *);
void InsertNewlines(char *, char *);
void dumpInHex(MQBYTE Buffer[], MQLONG DataLength);

void MQParmCpy(char *target, char *source, int length);

void MQsprintf(char *ParmBuffer, char *Format, char *Insert, int Length);

/* [V530] Switches for MQSprintf_X */
#define MQSPRINTF_X_NONE                0x00
#define MQSPRINTF_X_NO_TRAILING_BLANKS  0x01
#define MQSPRINTF_X_DOUBLE_QUOTES       0x02
#define MQSPRINTF_X_LINE_BREAKS         0x04
#define MQSPRINTF_X_NULL_IF_BLANK       0x08

/* [v5.3.0] Extended MQSprintf which is controlled via switches */
void MQsprintf_X(char *ParmBuffer, char *Format, char *Insert, int Length,
		int Options);

MQLONG PutMsg(MQHCONN hConn /* Connection to queue manager            */
, MQCHAR8 MsgFormat /* Format of user data to be put in msg   */
, MQHOBJ hQName /* handle of queue to put the message to  */
, MQCHAR48 QName /* name of queue to put the message to    */
, MQCHAR48 ReplyToQ /* name of q on which to receive replies  */
, MQBYTE *UserMsg /* The user data to be put in the message */
, MQLONG UserMsgLen /*                                        */
, MQCHAR12 UserID
);

MQLONG GetMsg(MQHCONN hConn /* handle of queue manager            */
, MQHOBJ hQName /* handle of queue to read from       */
, MQBYTE **UserMsg /* Input/Output buffer containing msg */
, MQLONG *ReadBufferLen /* Length of supplied buffer          */
, CmdArgParmsPtr MyParms
);

MQLONG OpenQ(MQHCONN hConn, MQCHAR48 QMgrName, MQCHAR48 QName,
		MQCHAR48 *DynamicQName, MQLONG OpenOpts, MQLONG *CompCode);

void makeStringFromList(char *, char *, MQLONG, MQLONG);

/* v6 .. various routines to return a string related to a #define */
void toStrMQMON(char * s, MQLONG v);
void toStrMQEVR(char * s, MQLONG v);
void toStrMQREC(char * s, MQLONG v);
void toStrMQSVC(char * s, MQLONG v);
void toStrMQCLWL(char * s, MQLONG v);
void toStrMQComp(char * s, MQLONG v[]);
void toStrMQPLATFORM(char * s, MQLONG v);
#if defined(zzzOS)
void toStrQSG(char * s, MQLONG v );
void toStrINDXTYPE(char * s, MQLONG v );
void toStrNLType(char * s, MQLONG v );
void toStrIGQPA(char * s, MQLONG v );
void toStrRTT(char * s, MQLONG v );
void toStrANMC(char * s, MQLONG v );
#endif
/* Some constants for our own use .... */
#define INT_BUFSZE    4096            /* 4 K       */


#define PBSIZE 1025  /* Size of PrintBuffer */

/* v603 --- channel RESET command selection flags */
/* s (SENDER) v (SEVER) r (RECEIVER) q (REQUESTER) c (CLUSSDR) d (CLUSRCVR) */

#define CHLR_SENDER     1
#define CHLR_SERVER     2
#define CHLR_RECEIVER   4
#define CHLR_REQUESTER  8
#define CHLR_CLUSSDR   16
#define CHLR_CLUSRCVR  32

typedef struct qList {
	char *next; /* ptr to next list item */
	MQLONG qType; /* type of queue, local remote ... */
	MQLONG qDefType; /* predefined, perm, dyn */
	MQCHAR qName[MQ_Q_NAME_LENGTH]; /* qname */
} qList, *pqList;

#endif /*MQUTILS__*/
