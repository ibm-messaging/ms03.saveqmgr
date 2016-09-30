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
/* Module Name: System.c                                                     */
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

/* If this isn't a zOS compile, don't bother compiling anything */
#if defined (zzzOS)

/* ------------------------------------------------------------------------ */
/* ProcessSystemStringParm()                                              */
/* ProcessSystemIntegerParm()                                             */
/*                                                                          */
extern int noisy;
extern int header;
extern int oneLine; /* v601 -- whether to write the entire DEFINE on oneline */
extern char lineTerm[3]; /* v601 -- line terminator */
extern int platform;

int firstSystem = 1;

void ProcessSystemStringParm( MQCFST *pPCFString
		, SystemParms *pSystemDefn
		, MQLONG *iCC
)
{
	/* Initialise return codes */
	*iCC = MQCC_OK;

	switch ( pPCFString->Parameter ) {
		case MQBACF_RESPONSE_ID: /* zOS things to ignore */
		case MQCACF_RESPONSE_Q_MGR_NAME: /* zOS things to ignore */
		break;

		case MQCACF_SYSP_OTMA_TPIPE_PFX :
		memset( pSystemDefn->otmaTpipe
				, ' ', 48);
		memcpy( pSystemDefn->otmaTpipe
				, pPCFString->String, pPCFString->StringLength );
		break;
		case MQCACF_SYSP_SERVICE:
		memset( pSystemDefn->servicearms
				, ' ', MQ_SYSP_SERVICE_LENGTH);
		memcpy( pSystemDefn->servicearms
				, pPCFString->String, pPCFString->StringLength );
		break;
		case MQCA_QSG_NAME:
		memset( pSystemDefn->qsgName
				, ' ', MQ_QSG_NAME_LENGTH);
		memcpy( pSystemDefn->qsgName
				, pPCFString->String, pPCFString->StringLength );
		break;
		case MQCACF_SYSP_OTMA_DRU_EXIT:
		memset( pSystemDefn->otmaDruExit
				, ' ', MQ_EXIT_NAME_LENGTH);
		memcpy( pSystemDefn->otmaDruExit
				, pPCFString->String, pPCFString->StringLength );
		break;
		case MQCACF_SYSP_OTMA_GROUP:
		memset( pSystemDefn->otmaGroup
				, ' ', MQ_XCF_GROUP_NAME_LENGTH);
		memcpy( pSystemDefn->otmaGroup
				, pPCFString->String, pPCFString->StringLength );
		break;
		case MQCACF_SYSP_OTMA_MEMBER:
		memset( pSystemDefn->otmaMember
				, ' ', MQ_XCF_MEMBER_NAME_LENGTH);
		memcpy( pSystemDefn->otmaMember
				, pPCFString->String, pPCFString->StringLength );
		break;
		case MQCACF_DB2_NAME:
		memset( pSystemDefn->db2Name
				, ' ', MQ_DB2_NAME_LENGTH);
		memcpy( pSystemDefn->db2Name
				, pPCFString->String, pPCFString->StringLength );
		break;
		case MQCACF_SYSP_CMD_USER_ID:
		memset( pSystemDefn->cmdUser
				, ' ', MQ_USER_ID_LENGTH);
		memcpy( pSystemDefn->cmdUser
				, pPCFString->String, pPCFString->StringLength );
		break;
		case MQCACF_DSG_NAME:
		memset( pSystemDefn->dsgName
				, ' ', MQ_DSG_NAME_LENGTH);
		memcpy( pSystemDefn->dsgName
				, pPCFString->String, pPCFString->StringLength );
		break;

		default:
		if(noisy) fprintf(stderr, "(System.c) Unknown PCF String Parameter: %ld\n",
				pPCFString->Parameter);
		*iCC = WARNING;
		break;
	} /* endswitch */

	return;
}

/*************************************************************/
void ProcessSystemIntegerParm( MQCFIN *pPCFInteger
		, SystemParms *pSystemDefn
		, MQLONG *iCC
)
{
	/* Initialise return codes */
	*iCC = MQCC_OK;

	switch ( pPCFInteger->Parameter ) {
		/* This stuff gets ignored */
		case MQIACF_SYSP_UNIT_ADDRESS:
		case MQIACF_SYSP_UNIT_STATUS:
		break;
		/* this is important stuff */

		case MQIACF_SYSP_TYPE:
		pSystemDefn->type = pPCFInteger->Value;
		break;
		case MQIACF_SYSP_CHKPOINT_COUNT:
		pSystemDefn->chkPoint = pPCFInteger->Value;
		break;
		case MQIACF_SYSP_CLUSTER_CACHE:
		pSystemDefn->clCache = pPCFInteger->Value;
		break;
		case MQIA_CODED_CHAR_SET_ID:
		pSystemDefn->ccsId = pPCFInteger->Value;
		break;
		case MQIACF_SYSP_DB2_BLOB_TASKS:
		pSystemDefn->db2Blobs = pPCFInteger->Value;
		break;
		case MQIACF_SYSP_DB2_TASKS:
		pSystemDefn->db2Tasks = pPCFInteger->Value;
		break;
		case MQIACF_SYSP_EXIT_INTERVAL:
		pSystemDefn->exitInt = pPCFInteger->Value;
		break;
		case MQIACF_SYSP_EXIT_TASKS:
		pSystemDefn->exitTasks = pPCFInteger->Value;
		break;
		case MQIACF_SYSP_MAX_CONNS:
		pSystemDefn->maxConns = pPCFInteger->Value;
		break;
		case MQIACF_SYSP_MAX_CONNS_BACK:
		pSystemDefn->maxBconns = pPCFInteger->Value;
		break;
		case MQIACF_SYSP_MAX_CONNS_FORE:
		pSystemDefn->maxFconns = pPCFInteger->Value;
		break;
		case MQIACF_SYSP_OTMA_INTERVAL:
		pSystemDefn->otmaInterval = pPCFInteger->Value;
		break;
		case MQIACF_SYSP_Q_INDEX_DEFER:
		pSystemDefn->qIndexDefer = pPCFInteger->Value;
		break;
		case MQIACF_SYSP_RESLEVEL_AUDIT:
		pSystemDefn->resLevel = pPCFInteger->Value;
		break;
		case MQIACF_SYSP_SMF_ACCOUNTING:
		pSystemDefn->smfAccounting = pPCFInteger->Value;
		break;
		case MQIACF_SYSP_SMF_STATS:
		pSystemDefn->smfStats = pPCFInteger->Value;
		break;
		case MQIACF_SYSP_SMF_INTERVAL:
		pSystemDefn->smfInterval = pPCFInteger->Value;
		break;
		case MQIACF_SYSP_TRACE_SIZE:
		pSystemDefn->traceSize = pPCFInteger->Value;
		break;
		case MQIACF_SYSP_WLM_INTERVAL:
		pSystemDefn->wlmInt = pPCFInteger->Value;
		break;
		case MQIACF_SYSP_WLM_INT_UNITS:
		pSystemDefn->wlmUnit = pPCFInteger->Value;
		break;

		default:
		if(noisy) fprintf(stderr, "(System.c) Unknown PCF Integer Parameter: %ld\n",
				pPCFInteger->Parameter);
		*iCC = WARNING;
		break;
	} /* endswitch */

	return;
}

void ProcessSystemIntegerListParm( MQCFIL *pPCFIntegerList
		, SystemParms *pSystemDefn
		, MQLONG *iCC
)
{
	int i = 0;
	*iCC = MQCC_OK;

	switch ( pPCFIntegerList->Parameter ) {

		case MQIACF_SYSP_ROUTING_CODE:
		for (i=0; i<ZZRTNCODES; i++) {
			pSystemDefn->rtnCodes[i] = -1;
		}

		for (i=0; i<pPCFIntegerList->Count && i<ZZRTNCODES; i++) {
			pSystemDefn->rtnCodes[i] = pPCFIntegerList->Values[i];
		}
		break;

		case MQIACF_SYSP_TRACE_CLASS:
		for (i=0; i<ZZTRCCLASSES; i++) {
			pSystemDefn->traceClass[i] = -1;
		}

		for (i=0; i<pPCFIntegerList->Count && i<ZZTRCCLASSES; i++) {
			pSystemDefn->traceClass[i] = pPCFIntegerList->Values[i];
		}
		break;

		default:
		if(noisy) fprintf(stderr, "(system.c) Unknown PCF Integer List Parameter: %ld\n",
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
void AddToFileSystem(
		SystemParms *pSystemDefn,
		int UserVersion,
		char * fileName, /* v32 Name of the output file  */
		FILE * fp, /* v51 */
		int twolines, /* v601 */
		MQLONG *iCC )
{
	char * ParmBuffer; /* Temporary buffer to hold for output to file */
	int options; /* options for MQsprintf_X */
	MQCHAR work[256];
	MQCHAR work2[8];
	int i=0;

	/* we don't make output if in oneline mode */
	if (oneLine) {
		fprintf(stderr, "Skipping SYSTEM information in oneline mode\n");
		return;
	}

	if ( !( ParmBuffer = (char *) malloc (PBSIZE) )) {
		fprintf (stderr, "(System.c) Failed to allocate %d bytes \n", PBSIZE);
		exit(2);
	}

	options = MQSPRINTF_X_NO_TRAILING_BLANKS + MQSPRINTF_X_NULL_IF_BLANK;

	/* Initialise return codes */
	*iCC = MQCC_OK;

	if (firstSystem) {
		firstSystem = 0;
		fprintf(fp, "******** System related definitions ********\n");
	}

	switch (pSystemDefn-> type) {
		case MQSYSP_TYPE_INITIAL:

		fprintf( fp, "*(zparm) CSQ6SYSP LOGLOAD=%ld,\n", pSystemDefn->chkPoint );
		fprintf( fp, "*(zparm)          QMCCSID=%ld,\n", pSystemDefn->ccsId );
		fprintf( fp, "*(zparm)          EXITLIM=%ld,\n", pSystemDefn->exitInt );
		fprintf( fp, "*(zparm)          EXITTCB=%ld,\n", pSystemDefn->exitTasks );
		fprintf( fp, "*(zparm)          CTHREAD=%ld,\n", pSystemDefn->maxConns );
		fprintf( fp, "*(zparm)          IDBACK=%ld,\n", pSystemDefn->maxBconns );
		fprintf( fp, "*(zparm)          IDFORE=%ld,\n", pSystemDefn->maxFconns );
		fprintf( fp, "*(zparm)          STATIME=%ld,\n", pSystemDefn->smfInterval );
		fprintf( fp, "*(zparm)          TRACTBL=%ld,\n", pSystemDefn->traceSize );
		fprintf( fp, "*(zparm)          WLMTIME=%ld,\n", pSystemDefn->wlmInt );

		MQsprintf_X( ParmBuffer ,
				"*(zparm)          SERVICE=%s,\n"
				, pSystemDefn->servicearms , MQ_SYSP_SERVICE_LENGTH
				, options );
		fputs( ParmBuffer, fp );
		MQsprintf_X( ParmBuffer ,
				"*(zparm)          CMDUSER=%s,\n"
				, pSystemDefn->cmdUser, MQ_USER_ID_LENGTH
				, options );
		fputs( ParmBuffer, fp );

		/* this should be conditional based on the qsgQmgr global */
		MQsprintf_X( ParmBuffer ,
				"*(zparm)          QSGDATA=(%s,"
				, pSystemDefn->qsgName, MQ_QSG_NAME_LENGTH
				, options );
		fputs( ParmBuffer, fp );

		MQsprintf_X( ParmBuffer , "%s,"
				, pSystemDefn->dsgName, MQ_DSG_NAME_LENGTH
				, options );
		fputs( ParmBuffer, fp );

		MQsprintf_X( ParmBuffer , "%s"
				, pSystemDefn->db2Name, MQ_DB2_NAME_LENGTH
				, options );
		fputs( ParmBuffer, fp );

		fprintf(fp , "%ld)\n" , pSystemDefn->db2Tasks);

		MQsprintf_X( ParmBuffer ,
				"*(zparm)          OTMACON=(%s,"
				, pSystemDefn->otmaGroup, MQ_XCF_GROUP_NAME_LENGTH
				, options );
		fputs( ParmBuffer, fp );

		MQsprintf_X( ParmBuffer , "%s,"
				, pSystemDefn->otmaMember, MQ_XCF_MEMBER_NAME_LENGTH
				, options );
		fputs( ParmBuffer, fp );

		MQsprintf_X( ParmBuffer , "%s,"
				, pSystemDefn->otmaDruExit, MQ_EXIT_NAME_LENGTH
				, options );
		fputs( ParmBuffer, fp );

		fprintf(fp , "%ld," , pSystemDefn->otmaInterval);

		MQsprintf_X( ParmBuffer , "%s)\n"
				, pSystemDefn->otmaTpipe, 48
				, options );
		fputs( ParmBuffer, fp );

		memset(work,'\0',sizeof(work));
		for(i=0;i<ZZRTNCODES;i++) {
			if (pSystemDefn->rtnCodes[i]!=-1) {
				sprintf(work2, "%ld,", pSystemDefn->rtnCodes[i]);
				strcat(work,work2);
			}
		}
		if(strlen(work)) work[strlen(work)-1]='\0';
		MQsprintf_X( ParmBuffer
				, "*(zparm)          ROUTCDE=(%s),\n"
				, work
				, sizeof(work)
				, options
		);
		fputs( ParmBuffer, fp );

		if(pSystemDefn->traceClass[0] == -1) {
			fprintf(fp, "*(zparm)          TRACSTR=NO,\n");
		} else
		if(pSystemDefn->traceClass[0] == 1 &&
				pSystemDefn->traceClass[1] == -1) {
			fprintf(fp, "*(zparm)          TRACSTR=YES,\n");
		} else
		{
			memset(work,'\0',sizeof(work));
			for(i=0;i<ZZRTNCODES;i++) {
				if (pSystemDefn->traceClass[i]!=-1) {
					sprintf(work2, "%ld,", pSystemDefn->traceClass[i]);
					strcat(work,work2);
				}
			}
			if(strlen(work)) work[strlen(work)-1]='\0';
			MQsprintf_X( ParmBuffer
					, "*(zparm)          TRACSTR=(%s),\n"
					, work
					, sizeof(work)
					, options
			);
			fputs( ParmBuffer, fp );
		}

		switch (pSystemDefn->wlmUnit) {
			case MQTIME_UNIT_SECS:
			fprintf(fp, "*(zparm)          WLMTIMU=SECS,\n");
			break;
			case MQTIME_UNIT_MINS:
			fprintf(fp, "*(zparm)          WLMTIMU=MINS,\n");
			break;
		}

		if(pSystemDefn->qIndexDefer==MQSYSP_YES) {
			fprintf(fp, "*(zparm)          QINDXBLD=NOWAIT,\n");
		} else {
			fprintf(fp, "*(zparm)          QINDXBLD=WAIT,\n");
		}

		if(pSystemDefn->resLevel==MQSYSP_YES) {
			fprintf(fp, "*(zparm)          RESAUDIT=YES,\n");
		} else {
			fprintf(fp, "*(zparm)          RESAUDIT=NO,\n");
		}
		if(pSystemDefn->smfAccounting==MQSYSP_YES) {
			fprintf(fp, "*(zparm)          SMFACCT=YES,\n");
		} else {
			fprintf(fp, "*(zparm)          SMFACCT=NO,\n");
		}
		if(pSystemDefn->smfStats==MQSYSP_YES) {
			fprintf(fp, "*(zparm)          SMFSTAT=YES,\n");
		} else {
			fprintf(fp, "*(zparm)          SMFSTAT=NO,\n");
		}
		if(pSystemDefn->clCache==MQCLCT_STATIC) {
			fprintf(fp, "*(zparm)          CLCACHE=STATIC\n");
		} else {
			fprintf(fp, "*(zparm)          CLCACHE=DYNAMIC\n");
		}

		fprintf( fp, "\n\n");

		break;

		case MQSYSP_TYPE_SET:

		fprintf(fp, "*(12cr) SET SYSTEM +\n" );

		fprintf( fp, "*(12cr)       CTHREAD(%ld) +\n", pSystemDefn->maxConns );
		fprintf( fp, "*(12cr)       IDBACK(%ld) +\n", pSystemDefn->maxBconns );
		fprintf( fp, "*(12cr)       IDFORE(%ld) +\n", pSystemDefn->maxFconns );
		fprintf( fp, "*(12cr)       LOGLOAD(%ld) +\n", pSystemDefn->chkPoint );
		MQsprintf_X( ParmBuffer , "*(12cr)       SERVICE(%s) +\n"
				, pSystemDefn->servicearms , MQ_SYSP_SERVICE_LENGTH
				, options );
		fputs( ParmBuffer, fp );
		fprintf( fp, "*(12cr)       STATIME(%ld) +\n", pSystemDefn->smfInterval );
		fprintf( fp, "*(12cr)       TRACTBL(%ld)\n", pSystemDefn->traceSize );

		fprintf( fp, "\n\n");

		break;

		default:
		if(noisy) fprintf(stderr, "(System.c) Unknown MQIACF_SYSP_TYPE: %ld\n",
				pSystemDefn-> type);
		*iCC = WARNING;
		break;
	} /* endswitch */

	free (ParmBuffer);
	return;

}

/* end of don't compile anything */
#endif
