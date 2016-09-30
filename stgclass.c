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
/* Module Name: stgclass.c                                                   */
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
/* ProcessStgClassStringParm()                                              */
/* ProcessStgClassIntegerParm()                                             */
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

void ProcessStgClassStringParm( MQCFST *pPCFString
		, StorageClassParms *pStgClassDefn
		, MQLONG *iCC
)
{
	/* Initialise return codes */
	*iCC = MQCC_OK;

	switch ( pPCFString->Parameter ) {
		case MQCA_STORAGE_CLASS:
		memset( pStgClassDefn->StgClassName
				, ' ', MQ_STORAGE_CLASS_LENGTH );
		memcpy( pStgClassDefn->StgClassName
				, pPCFString->String, pPCFString->StringLength );
		break;
		case MQCA_STORAGE_CLASS_DESC:
		memset( pStgClassDefn->StgClassDesc
				, ' ', MQ_STORAGE_CLASS_DESC_LENGTH );
		memcpy( pStgClassDefn->StgClassDesc
				, pPCFString->String, pPCFString->StringLength );
		break;
		case MQCA_ALTERATION_DATE:
		memset( pStgClassDefn->AlterationDate
				, ' ', MQ_CREATION_DATE_LENGTH );
		memcpy( pStgClassDefn->AlterationDate
				, pPCFString->String, pPCFString->StringLength);
		break;
		case MQCA_ALTERATION_TIME:
		memset( pStgClassDefn->AlterationTime
				, ' ', MQ_CREATION_TIME_LENGTH );
		memcpy( pStgClassDefn->AlterationTime
				, pPCFString->String, pPCFString->StringLength);
		break;
		case MQCA_PASS_TICKET_APPL:
		memset( pStgClassDefn->passTicketApplication
				, ' ', MQ_PASS_TICKET_APPL_LENGTH );
		memcpy( pStgClassDefn->passTicketApplication
				, pPCFString->String, pPCFString->StringLength);
		break;
		case MQCA_XCF_GROUP_NAME:
		memset( pStgClassDefn->xcfGroupName
				, ' ', MQ_XCF_GROUP_NAME_LENGTH);
		memcpy( pStgClassDefn->xcfGroupName
				, pPCFString->String, pPCFString->StringLength);
		break;
		case MQCA_XCF_MEMBER_NAME:
		memset( pStgClassDefn->xcfMemberName
				, ' ', MQ_XCF_MEMBER_NAME_LENGTH );
		memcpy( pStgClassDefn->xcfMemberName
				, pPCFString->String, pPCFString->StringLength);
		break;

		case MQBACF_RESPONSE_ID: /* zOS things to ignore */
		break;
		case MQCACF_RESPONSE_Q_MGR_NAME: /* zOS things to ignore */
		break;

		default:
		if(noisy) fprintf(stderr, "(stgclass.c) Unknown PCF String Parameter: %ld\n",
				pPCFString->Parameter);
		*iCC = WARNING;
		break;
	} /* endswitch */

	return;
}

/*************************************************************/
void ProcessStgClassIntegerParm( MQCFIN *pPCFInteger
		, StorageClassParms *pStgClassDefn
		, MQLONG *iCC
)
{
	/* Initialise return codes */
	*iCC = MQCC_OK;

	switch ( pPCFInteger->Parameter ) {
		case MQIA_PAGESET_ID:
		pStgClassDefn->pageSet = pPCFInteger->Value;
		break;
		case MQIA_QSG_DISP :
		pStgClassDefn->qsgDisp = pPCFInteger->Value;
		break;
		default:
		if(noisy) fprintf(stderr, "(stgclass.c) Unknown PCF Integer Parameter: %ld\n",
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
void AddToFileStgClass(
		StorageClassParms *pStgClassDefn,
		int UserVersion,
		char * fileName, /* v32 Name of the output file  */
		FILE * fp, /* v51 */
		int twolines, /* v601 */
		MQLONG *iCC )
{
	char * ParmBuffer; /* Temporary buffer to hold for output to file */
	int options; /* options for MQsprintf_X */
	char work[256];

	if ( !( ParmBuffer = (char *) malloc (PBSIZE) )) {
		fprintf (stderr, "(stgclass.c) Failed to allocate %d bytes \n", PBSIZE);
		exit(2);
	}

	/* Initialise return codes */
	*iCC = MQCC_OK;

	/* v601 setup MQsprintf_X to break lines based on oneLine setting */
	options = MQSPRINTF_X_DOUBLE_QUOTES + MQSPRINTF_X_NO_TRAILING_BLANKS;
	if (!oneLine) options += MQSPRINTF_X_LINE_BREAKS;

	/* Append these details to the end of the current output file (v32) */

	if (twolines) fputs("* ", fp);

	MQsprintf_X( ParmBuffer
			, "DEFINE STGCLASS('%s') "
			, pStgClassDefn->StgClassName
			, MQ_STORAGE_CLASS_LENGTH
			, options
	);
	fputs( ParmBuffer, fp );

	if (header && (!oneLine || twolines)) {
		MQsprintf_X( ParmBuffer, "* ALTDATE (%s) ",
				pStgClassDefn->AlterationDate,
				MQ_DATE_LENGTH,
				options);
		fputs( ParmBuffer, fp);

		MQsprintf_X( ParmBuffer, "* ALTTIME (%s) ",
				pStgClassDefn->AlterationTime,
				MQ_TIME_LENGTH,
				options);
		fputs( ParmBuffer, fp);
	}

	MQsprintf_X( ParmBuffer
			, "       DESCR('%s') "
			, pStgClassDefn->StgClassDesc
			, MQ_STORAGE_CLASS_DESC_LENGTH
			, options
	);
	fputs( ParmBuffer, fp );

	sprintf( ParmBuffer, "       PSID(%ld) %s", pStgClassDefn->pageSet , lineTerm );
	fputs( ParmBuffer, fp );

	MQsprintf_X( ParmBuffer, "       PASSTKTA ('%s') "
			, pStgClassDefn->passTicketApplication
			, MQ_PASS_TICKET_APPL_LENGTH
			, options);
	fputs( ParmBuffer, fp);
	MQsprintf_X( ParmBuffer, "       XCFGNAME ('%s') "
			, pStgClassDefn->xcfGroupName
			, MQ_XCF_GROUP_NAME_LENGTH
			, options);
	fputs( ParmBuffer, fp);
	MQsprintf_X( ParmBuffer, "       XCFMNAME ('%s') "
			, pStgClassDefn->xcfMemberName
			, MQ_XCF_MEMBER_NAME_LENGTH
			, options);
	fputs( ParmBuffer, fp);

	toStrQSG((char*)&work, pStgClassDefn->qsgDisp);
	sprintf( ParmBuffer, "       QSGDISP(%s) %s", work, lineTerm );
	fputs( ParmBuffer, fp );

	sprintf( ParmBuffer, "       REPLACE\n" );
	fputs( ParmBuffer, fp );

	if (!oneLine) {
		fputs( "\n", fp );
	}

	free (ParmBuffer);
	return;

}

/* end of don't compile anything */
#endif
