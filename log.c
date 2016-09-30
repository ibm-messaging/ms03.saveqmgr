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
/* Module Name: log.c                                                   */
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
/* ProcessLogStringParm()                                              */
/* ProcessLogIntegerParm()                                             */
/*                                                                          */
extern int noisy;
extern int header;
extern int oneLine;          /* v601 -- whether to write the entire DEFINE on oneline */
extern char lineTerm[3];     /* v601 -- line terminator */
extern int platform;

int firstLog = 1;

void ProcessLogStringParm( MQCFST    *pPCFString
                          , LogParms *pLogDefn
                          , MQLONG       *iCC
                          )
{
   /* Initialise return codes */
   *iCC = MQCC_OK;

   switch ( pPCFString->Parameter ) {
   case MQBACF_RESPONSE_ID:          /* zOS things to ignore */
   case MQCACF_RESPONSE_Q_MGR_NAME:  /* zOS things to ignore */
   case MQCACF_SYSP_Q_MGR_TIME :
   case MQCACF_SYSP_Q_MGR_DATE :
   case MQCACF_SYSP_Q_MGR_RBA :
   case MQCACF_SYSP_LOG_RBA :
     break;

   case MQCACF_DATA_SET_NAME :
      memset( pLogDefn->logDSN
            , ' ', 44 );
      memcpy( pLogDefn->logDSN
            , pPCFString->String, pPCFString->StringLength );
     break;

   default:
       if(noisy) fprintf(stderr, "(Log.c) Unknown PCF String Parameter: %ld\n",
              pPCFString->Parameter);
      *iCC = WARNING;
      break;
   } /* endswitch */

   return;
}


/*************************************************************/
void ProcessLogIntegerParm( MQCFIN    *pPCFInteger
                          , LogParms *pLogDefn
                           , MQLONG       *iCC
                           )
{
   /* Initialise return codes */
   *iCC = MQCC_OK;

   switch ( pPCFInteger->Parameter ) {
     /* This stuff gets ignored */
   case MQIACF_SYSP_LOG_USED:
   case MQIACF_SYSP_LOG_SUSPEND:
   case MQIACF_SYSP_OFFLOAD_STATUS:
   case MQIACF_SYSP_TOTAL_LOGS:
   case MQIACF_SYSP_FULL_LOGS:
     break;

     /* this is important stuff */

   case MQIACF_SYSP_TYPE:
       pLogDefn->type  = pPCFInteger->Value;
   break;
   case MQIACF_SYSP_DEALLOC_INTERVAL:
       pLogDefn->deallocInt  = pPCFInteger->Value;
   break;
   case MQIACF_SYSP_MAX_ARCHIVE:
       pLogDefn->maxArchive  = pPCFInteger->Value;
   break;
   case MQIACF_SYSP_MAX_READ_TAPES:
       pLogDefn->maxRTU  = pPCFInteger->Value;
   break;
   case MQIACF_SYSP_IN_BUFFER_SIZE:
       pLogDefn->inBuffer  = pPCFInteger->Value;
   break;
   case MQIACF_SYSP_OUT_BUFFER_SIZE:
       pLogDefn->outBuffer  = pPCFInteger->Value;
   break;
   case MQIACF_SYSP_OUT_BUFFER_COUNT:
       pLogDefn->wrthrsh  = pPCFInteger->Value;
   break;
   case MQIACF_SYSP_ARCHIVE:
       pLogDefn->offload  = pPCFInteger->Value;
   break;
   case MQIACF_SYSP_DUAL_ACTIVE:
       pLogDefn->dualActive  = pPCFInteger->Value;
   break;
   case MQIACF_SYSP_DUAL_ARCHIVE:
       pLogDefn->dualArchive  = pPCFInteger->Value;
   break;
   case MQIACF_SYSP_DUAL_BSDS:
       pLogDefn->dualBSDS  = pPCFInteger->Value;
   break;
   case MQIACF_SYSP_LOG_COPY:
       pLogDefn->logCopy  = pPCFInteger->Value;
   break;
   default:
       if(noisy) fprintf(stderr, "(Log.c) Unknown PCF Integer Parameter: %ld\n",
              pPCFInteger->Parameter);
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
void AddToFileLog(
                       LogParms *pLogDefn,
                       int UserVersion,
                       char * fileName,   /* v32 Name of the output file  */
                        FILE * fp,       /* v51 */
                       int twolines,     /* v601 */ 
                       MQLONG *iCC )
{
   char *  ParmBuffer;   /* Temporary buffer to hold for output to file */
   int options ; /* options for MQsprintf_X */

   /* we don't make output if in oneline mode */
   if (oneLine) {
      fprintf(stderr, "Skipping LOG information in oneline mode\n");
      return;
   }

   if ( !( ParmBuffer = (char *) malloc (PBSIZE) )) {
     fprintf (stderr, "(Log.c) Failed to allocate %d bytes \n", PBSIZE);
     exit(2);
   }

   options = MQSPRINTF_X_DOUBLE_QUOTES + MQSPRINTF_X_NO_TRAILING_BLANKS
             +  MQSPRINTF_X_LINE_BREAKS;

   /* Initialise return codes */
   *iCC = MQCC_OK;

   if (firstLog) {
      firstLog = 0;
      fprintf(fp, "******** Log related definitions ********\n");
   }

   switch (pLogDefn-> type) {
     case MQSYSP_TYPE_INITIAL:

              fprintf( fp, "*(zparm) CSQ6LOGP DEALLCT=%ld,\n", pLogDefn->deallocInt );

              fprintf( fp, "*(zparm)          MAXARCH=%ld,\n", pLogDefn->maxArchive );

              fprintf( fp, "*(zparm)          MAXRTU=%ld,\n", pLogDefn->maxRTU );

              fprintf( fp, "*(zparm)          INBUFF=%ld,\n", pLogDefn->inBuffer );

              fprintf( fp, "*(zparm)          OUTBUFF=%ld,\n", pLogDefn->outBuffer );

              fprintf( fp, "*(zparm)          WRTHRSH=%ld,\n", pLogDefn->wrthrsh );
            
             if (pLogDefn->offload == MQSYSP_YES) {
	       fprintf(fp, "*(zparm)          OFFLOAD=YES,\n" );
             } else {
	       fprintf(fp, "*(zparm)          OFFLOAD=NO,\n" );
             }

             if (pLogDefn->dualActive == MQSYSP_YES) {
	       fprintf(fp, "*(zparm)          TWOACTV=YES,\n" );
             } else {
	       fprintf(fp, "*(zparm)          TWOACTV=NO,\n" );
             }

             if (pLogDefn->dualArchive == MQSYSP_YES) {
	       fprintf(fp, "*(zparm)          TWOARCH=YES,\n" );
             } else {
	       fprintf(fp, "*(zparm)          TWOARCH=NO,\n" );
             }

             if (pLogDefn->dualBSDS == MQSYSP_YES) {
	       fprintf(fp, "*(zparm)          TWOBSDS=YES\n" );
             } else {
	       fprintf(fp, "*(zparm)          TWOBSDS=NO\n" );
             }
           
             fprintf( fp, "\n\n");
 
     break;

     case MQSYSP_TYPE_SET:

             fprintf( fp, "*(12cr) SET LOG +\n"  );

             fprintf( fp, "*(12cr)       DEALLCT(%ld) +\n", pLogDefn->deallocInt);

             fprintf( fp, "*(12cr)       MAXARCH(%ld) +\n", pLogDefn->maxArchive);

             fprintf( fp, "*(12cr)       MAXRTU(%ld) +\n", pLogDefn->maxRTU);

             fprintf( fp, "*(12cr)       WRTHRSH(%ld)\n", pLogDefn->wrthrsh );

             fprintf( fp, "\n\n");

       break;

     case MQSYSP_TYPE_LOG_COPY:

      MQsprintf_X( ParmBuffer
               , "*(cr) DEFINE LOG('%s') "
               , pLogDefn->logDSN
               , 44
	       , options
               );
      fputs( ParmBuffer, fp );
      
      fprintf( fp, "*(cr)       COPY(%ld)\n", pLogDefn->logCopy);
      fprintf( fp, "\n\n");

     break;

     case MQSYSP_TYPE_LOG_STATUS:
       /* Nothing to do here ! */
     break;

   default:
       if(noisy) fprintf(stderr, "(Log.c) Unknown MQIACF_SYSP_TYPE: %ld\n",
              pLogDefn-> type);
      *iCC = WARNING;
      break;
   } /* endswitch */

   free (ParmBuffer);
   return;


}

/* end of don't compile anything */
#endif
