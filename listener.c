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
/* ProcessNamelistStringParm()                                              */
/* ProcessNamelistIntegerParm()                                             */
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


void ProcessListenerStringParm( MQCFST    *pPCFString
                          , ListenerParms *ListenerDefn
                          , MQLONG       *iCC
                          )
{
   /* Initialise return codes */
   *iCC = MQCC_OK;

   switch ( pPCFString->Parameter ) {
   case MQCACH_LISTENER_NAME:
      memset( ListenerDefn->ListenerName
            , ' ', MQ_LISTENER_NAME_LENGTH );
      memcpy( ListenerDefn->ListenerName
            , pPCFString->String, pPCFString->StringLength );
      break;
   case MQCACH_LISTENER_DESC:
      memset( ListenerDefn->ListenerDesc
            , ' ', MQ_LISTENER_DESC_LENGTH );
      memcpy( ListenerDefn->ListenerDesc
            , pPCFString->String, pPCFString->StringLength );
      break;
   case MQCA_ALTERATION_DATE:
      memset( ListenerDefn->AlterationDate
            , ' ', MQ_CREATION_DATE_LENGTH );
      memcpy( ListenerDefn->AlterationDate
            , pPCFString->String, pPCFString->StringLength);
      break;
   case MQCA_ALTERATION_TIME:
      memset( ListenerDefn->AlterationTime
            , ' ', MQ_CREATION_TIME_LENGTH );
      memcpy( ListenerDefn->AlterationTime
            , pPCFString->String, pPCFString->StringLength);
      break;


   case MQCACH_TP_NAME:
      memset( ListenerDefn->TpName
               , ' ', MQ_TP_NAME_LENGTH );
      memcpy( ListenerDefn->TpName
               , pPCFString->String, pPCFString -> StringLength);
      break;
   case MQCACH_LOCAL_NAME:
      memset( ListenerDefn->LocalName
               , ' ', MQ_CONN_NAME_LENGTH );
      memcpy( ListenerDefn->LocalName
               , pPCFString->String, pPCFString -> StringLength);
      break;
   case MQCACH_IP_ADDRESS:
      memset( ListenerDefn->IPAddress
               , ' ', MQ_CONN_NAME_LENGTH);
      memcpy( ListenerDefn->IPAddress
               , pPCFString->String, pPCFString -> StringLength);
      break;


   default:
       if(noisy) fprintf(stderr, "(listener.c) Unknown PCF String Parameter: %ld\n",
              pPCFString->Parameter);
      *iCC = WARNING;
      break;
   } /* endswitch */

   return;
}


/*************************************************************/
void ProcessListenerIntegerParm( MQCFIN    *pPCFInteger
                          , ListenerParms *ListenerDefn
                           , MQLONG       *iCC
                           )
{
   /* Initialise return codes */
   *iCC = MQCC_OK;

   switch ( pPCFInteger->Parameter ) {
   case MQIACH_XMIT_PROTOCOL_TYPE:
      ListenerDefn->TransportType = pPCFInteger->Value;
      break;
   case MQIACH_LISTENER_CONTROL:
      ListenerDefn->Control = pPCFInteger->Value;
      break;
   case MQIACH_BACKLOG:
      ListenerDefn->Backlog = pPCFInteger->Value;
      break;
   case MQIACH_PORT:
      ListenerDefn->Port = pPCFInteger->Value;
      break;
   case MQIACH_SESSION_COUNT:
      ListenerDefn->SessionCount = pPCFInteger->Value;
      break;
   case MQIACH_ADAPTER:
      ListenerDefn->Adapter = pPCFInteger->Value;
      break;
   case MQIACH_COMMAND_COUNT:
      ListenerDefn->CommandCount = pPCFInteger->Value;
      break;
   case MQIACH_SOCKET:
      ListenerDefn->Socket = pPCFInteger->Value;
      break;
   case MQIACH_NAME_COUNT:
      ListenerDefn->NameCount = pPCFInteger->Value;
      break;


   default:
       if(noisy) fprintf(stderr, "(listener.c) Unknown PCF Integer Parameter: %ld\n",
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
void AddToFileLISTENER(
                       ListenerParms *pListenerDefn,
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
     fprintf (stderr, "(listener.c) Failed to allocate %d bytes \n", PBSIZE);
     exit(2);
   }
 
   if (twolines) fputs("* ", fp);

   /* Append these details to the end of the current output file (v32) */

      MQsprintf_X( ParmBuffer
               , "DEFINE LISTENER ('%s') "
               , pListenerDefn->ListenerName
               , MQ_LISTENER_NAME_LENGTH
		   , options
               );
      fputs( ParmBuffer, fp );

      if (header && (!oneLine || twolines)) {
	MQsprintf_X( ParmBuffer, "* ALTDATE (%s) ",
               pListenerDefn->AlterationDate,
               MQ_CREATION_DATE_LENGTH, options);
	fputs( ParmBuffer, fp);
	MQsprintf_X( ParmBuffer, "* ALTTIME (%s) ",
               pListenerDefn->AlterationTime,
               MQ_CREATION_TIME_LENGTH, options);
	fputs( ParmBuffer, fp);
      }

     switch ( pListenerDefn->TransportType ) {
     case MQXPT_LU62:
          sprintf( ParmBuffer, "       TRPTYPE(LU62) %s", lineTerm );
          fputs( ParmBuffer, fp );

          MQsprintf_X( ParmBuffer
               , "       TPNAME('%s') "
               , pListenerDefn->TpName
               , MQ_TP_NAME_LENGTH
		 , options
               );
         fputs( ParmBuffer, fp );
        break;
     case MQXPT_TCP:
        sprintf( ParmBuffer, "       TRPTYPE(TCP) %s", lineTerm );
        fputs( ParmBuffer, fp );

        MQsprintf_X( ParmBuffer
               , "       IPADDR('%s') "
               , pListenerDefn->IPAddress
               , MQ_CONN_NAME_LENGTH
		 , options
               );
        fputs( ParmBuffer, fp );

        sprintf( ParmBuffer, "       PORT(%ld) %s",
                     pListenerDefn->Port, lineTerm );
        fputs( ParmBuffer, fp );

        sprintf( ParmBuffer, "       BACKLOG(%ld) %s",
                     pListenerDefn->Backlog, lineTerm );
        fputs( ParmBuffer, fp );


        break;
     case MQXPT_NETBIOS:
        sprintf( ParmBuffer, "       TRPTYPE(NETBIOS) %s", lineTerm );
        fputs( ParmBuffer, fp );

        MQsprintf_X( ParmBuffer
               , "       LOCLNAME('%s') "
               , pListenerDefn->LocalName
               , MQ_CONN_NAME_LENGTH
		 , options
               );
        fputs( ParmBuffer, fp );

        sprintf( ParmBuffer, "       ADAPTER(%ld) %s",
                     pListenerDefn->Adapter, lineTerm );
        fputs( ParmBuffer, fp );

        sprintf( ParmBuffer, "       COMMANDS(%ld) %s",
                     pListenerDefn->CommandCount, lineTerm );
        fputs( ParmBuffer, fp );

        sprintf( ParmBuffer, "       NTBNAMES(%ld) %s",
                     pListenerDefn->NameCount , lineTerm);
        fputs( ParmBuffer, fp );

        sprintf( ParmBuffer, "       SESSIONS(%ld) %s",
                     pListenerDefn->SessionCount , lineTerm);
        fputs( ParmBuffer, fp );

        break;
     #if defined (MQXPT_SPX)
     case MQXPT_SPX:
        sprintf( ParmBuffer, "       TRPTYPE(SPX) %s", lineTerm );
        fputs( ParmBuffer, fp );
        sprintf( ParmBuffer, "       SOCKET(%ld) %s",
                     pListenerDefn->Socket, lineTerm );
        fputs( ParmBuffer, fp );
        sprintf( ParmBuffer, "       BACKLOG(%ld) %s",
                     pListenerDefn->Backlog, lineTerm );
        fputs( ParmBuffer, fp );


        break;
     #endif

     }  /* end of switch based on trptype */
      MQsprintf_X( ParmBuffer
               , "       DESCR('%s') "
               , pListenerDefn->ListenerDesc
               , MQ_LISTENER_DESC_LENGTH
		 , options
               );
      fputs( ParmBuffer, fp );

      toStrMQSVC((char*)&work , pListenerDefn->Control);
      sprintf( ParmBuffer, "       CONTROL(%s) %s", work, lineTerm);  
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


