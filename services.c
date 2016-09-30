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
/* Module Name: listener.c                                                   */
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

/* If this isn't v6, don't bother compiling anything */
#if defined (zzMQV60)

/* ------------------------------------------------------------------------ */
/* ProcessServiceStringParm()                                              */
/* ProcessServiceIntegerParm()                                             */
/*                                                                          */
/* These two processes are given a pointer to a block of storage that       */
/* contains the definition of a single attribute relating to an individual  */
/* Listener. The first handles string parameters, the second integer.       */
/* The calling process has already decided which type of parameter this is  */
/* and is responsible for managing the storage. These procedures decide     */
/* which of the possible attributes this one is and extract the value.      */
/*                                                                          */
/* Follows is a list of all attributes possible for a process definition.   */
/*                                                                          */
/* The ProcDefn structure contains entries for each of the attributes.      */
/* ------------------------------------------------------------------------ */

extern int noisy;
extern int header;
extern int oneLine;          /* v601 -- whether to write the entire DEFINE on oneline */
extern char lineTerm[3];     /* v601 -- line terminator */


void ProcessServiceStringParm( MQCFST    *pPCFString
                          , ServiceParms *ServiceDefn
                          , MQLONG       *iCC
                          )
{
   /* Initialise return codes */
   *iCC = MQCC_OK;

   switch ( pPCFString->Parameter ) {
   case MQCA_SERVICE_NAME:
      memset( ServiceDefn->ServiceName
            , ' ', MQ_SERVICE_NAME_LENGTH );
      memcpy( ServiceDefn->ServiceName
            , pPCFString->String, pPCFString->StringLength );
      break;
   case MQCA_SERVICE_DESC:
      memset( ServiceDefn->ServiceDesc
            , ' ', MQ_SERVICE_DESC_LENGTH );
      memcpy( ServiceDefn->ServiceDesc
            , pPCFString->String, pPCFString->StringLength );
      break;
   case MQCA_ALTERATION_DATE:
      memset( ServiceDefn->AlterationDate
            , ' ', MQ_CREATION_DATE_LENGTH );
      memcpy( ServiceDefn->AlterationDate
            , pPCFString->String, pPCFString->StringLength);
      break;
   case MQCA_ALTERATION_TIME:
      memset( ServiceDefn->AlterationTime
            , ' ', MQ_CREATION_TIME_LENGTH );
      memcpy( ServiceDefn->AlterationTime
            , pPCFString->String, pPCFString->StringLength);
      break;
   case MQCA_SERVICE_START_COMMAND:
      memset( ServiceDefn->StartCommand
            , ' ', MQ_SERVICE_COMMAND_LENGTH );
      memcpy( ServiceDefn->StartCommand
            , pPCFString->String, pPCFString->StringLength);
      break;
   case MQCA_SERVICE_START_ARGS:
      memset( ServiceDefn->StartArgs
            , ' ', MQ_SERVICE_ARGS_LENGTH );
      memcpy( ServiceDefn->StartArgs
            , pPCFString->String, pPCFString->StringLength);
      break;
   case MQCA_SERVICE_STOP_COMMAND:
      memset( ServiceDefn->StopCommand
            , ' ', MQ_SERVICE_COMMAND_LENGTH );
      memcpy( ServiceDefn->StopCommand
            , pPCFString->String, pPCFString->StringLength);
      break;
   case MQCA_SERVICE_STOP_ARGS:
      memset( ServiceDefn->StopArgs
            , ' ', MQ_SERVICE_ARGS_LENGTH );
      memcpy( ServiceDefn->StopArgs
            , pPCFString->String, pPCFString->StringLength);
      break;
   case MQCA_STDOUT_DESTINATION:
      memset( ServiceDefn->StdoutDestination
            , ' ', sizeof(ServiceDefn->StdoutDestination) );
      memcpy( ServiceDefn->StdoutDestination
            , pPCFString->String, pPCFString->StringLength);
      break;
   case MQCA_STDERR_DESTINATION:
      memset( ServiceDefn->StderrDestination
            , ' ', sizeof(ServiceDefn->StderrDestination) );
      memcpy( ServiceDefn->StderrDestination
            , pPCFString->String, pPCFString->StringLength);
      break;

   default:
       if(noisy) fprintf(stderr, "(services.c) Unknown PCF String Parameter: %ld\n",
              pPCFString->Parameter);
      *iCC = WARNING;
      break;
   } /* endswitch */

   return;
}


/*************************************************************/
void ProcessServiceIntegerParm( MQCFIN    *pPCFInteger
                          , ServiceParms *ServiceDefn
                           , MQLONG       *iCC
                           )
{
   /* Initialise return codes */
   *iCC = MQCC_OK;

   switch ( pPCFInteger->Parameter ) {
   case MQIA_SERVICE_CONTROL:
        ServiceDefn->Control = pPCFInteger->Value;
      break;
   case MQIA_SERVICE_TYPE:
        ServiceDefn->ServiceType = pPCFInteger->Value;
      break;
   default:
       if(noisy) fprintf(stderr, "(services.c) Unknown PCF Integer Parameter: %ld\n",
              pPCFInteger->Parameter);
      *iCC = WARNING;
      break;
   } /* endswitch */

   return;
}


/* ------------------------------------------------------------------------ */
/*                                                                          */
/* This process takes the attributes of a single process and adds them      */
/* to the end of a file, SAVEQMGR.TST, which can be found in the current    */
/* directory.                                                               */
/*                                                                          */
/* The file is of a format suitable for subsequent input to RUNMQSC.        */
/*                                                                          */
/* ------------------------------------------------------------------------ */
void AddToFileSERVICE(
                       ServiceParms *pServiceDefn,
                       int UserVersion,
                       char * fileName,   /* v32 Name of the output file  */
                        FILE * fp,       /* v51 */
                       int twolines,
                       MQLONG *iCC )
{
   char *  ParmBuffer;   /* Temporary buffer to hold for output to file */
   char work[256];
   int options = 0;

   /* Initialise return codes */


   *iCC = MQCC_OK;

   /* v601 setup options for oneLine operation */ 
   options = MQSPRINTF_X_DOUBLE_QUOTES + MQSPRINTF_X_NO_TRAILING_BLANKS;
   if (!oneLine) options += MQSPRINTF_X_LINE_BREAKS;


   if ( !( ParmBuffer = (char *) malloc (PBSIZE) )) {
     fprintf (stderr, "(services.c) Failed to allocate %d bytes \n", PBSIZE);
     exit(2);
   }


   if (twolines) fputs("* ", fp);

      MQsprintf_X( ParmBuffer
               , "DEFINE SERVICE ('%s') "
               , pServiceDefn->ServiceName
               , MQ_SERVICE_NAME_LENGTH
		   , options
               );
      fputs( ParmBuffer, fp );

      if (header && (!oneLine|| twolines)) {
	MQsprintf_X( ParmBuffer, "* ALTDATE (%s) ",
               pServiceDefn->AlterationDate,
               MQ_CREATION_DATE_LENGTH, options);
	fputs( ParmBuffer, fp);
	MQsprintf_X( ParmBuffer, "* ALTTIME (%s) ",
               pServiceDefn->AlterationTime,
               MQ_CREATION_TIME_LENGTH, options);
	fputs( ParmBuffer, fp);
      }

      MQsprintf_X( ParmBuffer
               , "       DESCR('%s') "
               , pServiceDefn->ServiceDesc
               , MQ_SERVICE_DESC_LENGTH
		   , options
               );
      fputs( ParmBuffer, fp );

      MQsprintf_X( ParmBuffer
               , "       STARTCMD('%s') "
               , pServiceDefn->StartCommand
               , MQ_SERVICE_COMMAND_LENGTH
		   , options
               );
      fputs( ParmBuffer, fp );

      MQsprintf_X( ParmBuffer
               , "       STARTARG('%s') "
               , pServiceDefn->StartArgs
               , MQ_SERVICE_ARGS_LENGTH
		   , options
               );
      fputs( ParmBuffer, fp );

      MQsprintf_X( ParmBuffer
               , "       STOPCMD('%s') "
               , pServiceDefn->StopCommand
               , MQ_SERVICE_COMMAND_LENGTH
		   , options
               );
      fputs( ParmBuffer, fp );

      MQsprintf_X( ParmBuffer
               , "       STOPARG('%s') "
               , pServiceDefn->StopArgs
               , MQ_SERVICE_ARGS_LENGTH
		   , options
               );
      fputs( ParmBuffer, fp );

      MQsprintf_X( ParmBuffer
               , "       STDOUT('%s') "
               , pServiceDefn->StdoutDestination
               , MQ_SERVICE_PATH_LENGTH
		   , options
               );
      fputs( ParmBuffer, fp );

      MQsprintf_X( ParmBuffer
               , "       STDERR('%s') "
               , pServiceDefn->StderrDestination
               , MQ_SERVICE_PATH_LENGTH
		   , options
               );
      fputs( ParmBuffer, fp );


      toStrMQSVC((char*)&work , pServiceDefn->Control);
      sprintf( ParmBuffer, "       CONTROL(%s) %s", work, lineTerm);  
      fputs( ParmBuffer, fp );

      sprintf( ParmBuffer, "       SERVTYPE(%s) %s", 
                pServiceDefn->ServiceType==MQSVC_TYPE_COMMAND?"COMMAND":"SERVER", lineTerm);
      fputs( ParmBuffer, fp );


      fputs( "       REPLACE\n", fp );

      if (!oneLine) {
        fputs("\n", fp);
      }



   free (ParmBuffer);
   return;


}
/* end of don't compile anything */
#endif


