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
/* ProcessUsageStringParm()                                              */
/* ProcessUsageIntegerParm()                                             */
/*                                                                          */
extern int noisy;
extern int header;
extern int oneLine; /* v601 -- whether to write the entire DEFINE on oneline */
extern char lineTerm[3]; /* v601 -- line terminator */
extern int platform;

int firstUsage = 1;

void ProcessUsageStringParm( MQCFST *pPCFString
		, UsageParms *pUsageDefn
		, MQLONG *iCC
)
{
	/* Initialise return codes */
	*iCC = MQCC_OK;

	switch ( pPCFString->Parameter ) {
		case MQBACF_RESPONSE_ID: /* zOS things to ignore */
		break;
		case MQCACF_RESPONSE_Q_MGR_NAME: /* zOS things to ignore */
		break;

		default:
		if(noisy) fprintf(stderr, "(Usage.c) Unknown PCF String Parameter: %ld\n",
				pPCFString->Parameter);
		*iCC = WARNING;
		break;
	} /* endswitch */

	return;
}

/*************************************************************/
void ProcessUsageIntegerParm( MQCFIN *pPCFInteger
		, UsageParms *pUsageDefn
		, MQLONG *iCC
)
{
	/* Initialise return codes */
	*iCC = MQCC_OK;

	switch ( pPCFInteger->Parameter ) {
		case MQIACF_PAGESET_STATUS :
		case MQIACF_USAGE_TOTAL_PAGES:
		case MQIACF_USAGE_UNUSED_PAGES:
		case MQIACF_USAGE_PERSIST_PAGES:
		case MQIACF_USAGE_NONPERSIST_PAGES:
		case MQIACF_USAGE_EXPAND_COUNT:
		break;
		case MQIA_PAGESET_ID:
		pUsageDefn->psId = pPCFInteger->Value;
		break;
		case MQIACF_USAGE_TYPE:
		pUsageDefn->type = pPCFInteger->Value;
		break;
		case MQIACF_BUFFER_POOL_ID:
		pUsageDefn->buffPool = pPCFInteger->Value;
		break;
		case MQIACF_USAGE_EXPAND_TYPE:
		pUsageDefn->expandType = pPCFInteger->Value;
		break;
		case MQIACF_USAGE_TOTAL_BUFFERS:
		pUsageDefn->totalBuffers = pPCFInteger->Value;
		break;

		default:
		if(noisy) fprintf(stderr, "(Usage.c) Unknown PCF Integer Parameter: %ld\n",
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
void AddToFileUsage(
		UsageParms *pUsageDefn,
		int UserVersion,
		char * fileName, /* v32 Name of the output file  */
		FILE * fp, /* v51 */
		int twolines, /* v601 */
		MQLONG *iCC )
{
	char * ParmBuffer; /* Temporary buffer to hold for output to file */

	/* we don't make output if in oneline mode */
	if (oneLine) {
		fprintf(stderr, "Skipping USAGE information in oneline mode\n");
		return;
	}

	if ( !( ParmBuffer = (char *) malloc (PBSIZE) )) {
		fprintf (stderr, "(Usage.c) Failed to allocate %d bytes \n", PBSIZE);
		exit(2);
	}

	/* Initialise return codes */
	*iCC = MQCC_OK;

	if (firstUsage) {
		firstUsage = 0;
		fprintf(fp, "******** Usage related definitions ********\n");
	}

	switch (pUsageDefn-> type) {
		case MQIACF_USAGE_PAGESET:
		fprintf(fp, "*(1cr) DEFINE PSID(%ld) +\n*(1cr)       BUFFPOOL(%ld) +\n",
				pUsageDefn->psId,
				pUsageDefn->buffPool);

		switch(pUsageDefn->expandType) {
			case MQUSAGE_EXPAND_NONE:
			fprintf(fp, "*(1cr)       EXPAND(NONE)\n");
			break;
			case MQUSAGE_EXPAND_USER:
			fprintf(fp, "*(1cr)       EXPAND(USER)\n");
			break;
			case MQUSAGE_EXPAND_SYSTEM:
			fprintf(fp, "*(1cr)       EXPAND(SYSTEM)\n");
			break;
		}
		fputs( "\n", fp);

		break;
		case MQIACF_USAGE_BUFFER_POOL:
		fprintf(fp, "*(1) DEFINE BUFFPOOL(%ld) +\n*(1)       BUFFERS(%ld)\n\n",
				pUsageDefn->buffPool,
				pUsageDefn->totalBuffers);

		break;
		case MQIACF_USAGE_DATA_SET:
		break;
		default:
		if(noisy) fprintf(stderr, "(Usage.c) Unknown MQIACF_USAGE_TYPE: %ld\n",
				pUsageDefn-> type);
		*iCC = WARNING;
		break;

	}

	free (ParmBuffer);
	return;

}

/* end of don't compile anything */
#endif
