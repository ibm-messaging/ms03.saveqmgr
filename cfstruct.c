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
/* ProcessCFStructStringParm()                                              */
/* ProcessCFStructIntegerParm()                                             */
/*                                                                          */
extern int noisy;
extern int header;
extern int oneLine;          /* v601 -- whether to write the entire DEFINE on oneline */
extern char lineTerm[3];     /* v601 -- line terminator */
extern int platform;

int firstCFStruct = 1;

void ProcessCFStructStringParm( MQCFST    *pPCFString
                          , CFStructParms *pCFStructDefn
                          , MQLONG       *iCC
                          )
{
   /* Initialise return codes */
   *iCC = MQCC_OK;

   switch ( pPCFString->Parameter ) {
   case MQCA_CF_STRUC_NAME:
      memset( pCFStructDefn->CFName
            , ' ',  MQ_CF_STRUC_NAME_LENGTH);
      memcpy( pCFStructDefn->CFName
            , pPCFString->String, pPCFString->StringLength);
      break;
   case MQCA_CF_STRUC_DESC:
      memset( pCFStructDefn->CFDesc
            , ' ', MQ_CF_STRUC_DESC_LENGTH );
      memcpy( pCFStructDefn->CFDesc
            , pPCFString->String, pPCFString->StringLength);
      break;

   case MQCA_ALTERATION_DATE:
      memset( pCFStructDefn->AlterationDate
            , ' ', MQ_DATE_LENGTH );
      memcpy( pCFStructDefn->AlterationDate
            , pPCFString->String, pPCFString->StringLength);
      break;
   case MQCA_ALTERATION_TIME:
      memset( pCFStructDefn->AlterationTime
            , ' ', MQ_TIME_LENGTH );
      memcpy( pCFStructDefn->AlterationTime
            , pPCFString->String, pPCFString->StringLength);
      break;

   case MQBACF_RESPONSE_ID:          /* zOS things to ignore */
     break;
   case MQCACF_RESPONSE_Q_MGR_NAME:  /* zOS things to ignore */
     break;

   default:
       if(noisy) fprintf(stderr, "(CFStruct.c) Unknown PCF String Parameter: %ld\n",
              pPCFString->Parameter);
      *iCC = WARNING;
      break;
   } /* endswitch */

   return;
}


/*************************************************************/
void ProcessCFStructIntegerParm( MQCFIN    *pPCFInteger
                          , CFStructParms *pCFStructDefn
                           , MQLONG       *iCC
                           )
{
   /* Initialise return codes */
   *iCC = MQCC_OK;

   switch ( pPCFInteger->Parameter ) {
     case MQIA_CF_LEVEL:
        pCFStructDefn->cfLevel = pPCFInteger->Value;
        break;

     case MQIA_CF_RECOVER:
        pCFStructDefn->recovery = pPCFInteger->Value;
        break;

   default:
       if(noisy) fprintf(stderr, "(CFStruct.c) Unknown PCF Integer Parameter: %ld\n",
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
void AddToFileCFStruct(
                       CFStructParms *pCFStructDefn,
                       int UserVersion,
                       char * fileName,   /* v32 Name of the output file  */
                        FILE * fp,       /* v51 */
                       int twolines,     /* v601 */ 
                       MQLONG *iCC )
{
   char *  ParmBuffer;   /* Temporary buffer to hold for output to file */
   int options ; /* options for MQsprintf_X */

   if ( !( ParmBuffer = (char *) malloc (PBSIZE) )) {
     fprintf (stderr, "(CFStruct.c) Failed to allocate %d bytes \n", PBSIZE);
     exit(2);
   }

if (firstCFStruct) {
     firstCFStruct = 0;
     fprintf(fp, "******** CFStruct related definitions ********\n");
   }

   

   /* Initialise return codes */
   *iCC = MQCC_OK;

   /* v601 setup MQsprintf_X to break lines based on oneLine setting */
   options = MQSPRINTF_X_DOUBLE_QUOTES + MQSPRINTF_X_NO_TRAILING_BLANKS;
   if (!oneLine) options += MQSPRINTF_X_LINE_BREAKS;


   if (twolines) fputs("* ", fp); 

      MQsprintf_X( ParmBuffer
               , "*(2cr) DEFINE CFSTRUCT('%s') "
               , pCFStructDefn->CFName
               , MQ_CF_STRUC_NAME_LENGTH
	       , options
               );
      fputs( ParmBuffer, fp );

      if (header && (!oneLine || twolines)) {
	MQsprintf_X( ParmBuffer, "*(2cr)*     ALTDATE (%s) ",
               pCFStructDefn->AlterationDate,
		     MQ_CREATION_DATE_LENGTH,
               options);
	fputs( ParmBuffer, fp);

	MQsprintf_X( ParmBuffer, "*(2cr)*     ALTTIME (%s) ",
               pCFStructDefn->AlterationTime,
               MQ_CREATION_TIME_LENGTH,
               options);
	fputs( ParmBuffer, fp);
      }

      MQsprintf_X( ParmBuffer
               ,           "*(2cr)       DESCR('%s') "
               , pCFStructDefn->CFDesc
               , MQ_CF_STRUC_DESC_LENGTH
	       , options
               );

      sprintf(ParmBuffer,"*(2cr)       CFLEVEL(%ld) %s", pCFStructDefn->cfLevel, lineTerm );
      fputs( ParmBuffer, fp);

      if ( pCFStructDefn->recovery == MQCFR_YES ) {
            sprintf( ParmBuffer, "*(2cr)       RECOVERY(YES) %s" , lineTerm );
       } else {
            sprintf( ParmBuffer, "*(2cr)       RECOVERY(NO) %s" , lineTerm );
       } /* endif */
       fputs( ParmBuffer, fp );

      sprintf( ParmBuffer, "*(2cr)       REPLACE\n" );
      fputs( ParmBuffer, fp );

      if (!oneLine) {
	fputs( "\n", fp );
      }
      

   free (ParmBuffer);
   return;


}

/* end of don't compile anything */
#endif
