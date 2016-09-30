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
/* Module Name: NAMELIST.c                                                   */
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
/* If this isn't v5.1, don't bother compiling anything */
#if defined (MQ_OBJECT_NAME_LENGTH)


/* ------------------------------------------------------------------------ */
/* ProcessNamelistStringParm()                                              */
/* ProcessNamelistIntegerParm()                                             */
/*                                                                          */
/* These two processes are given a pointer to a block of storage that       */
/* contains the definition of a single attribute relating to an individual  */
/* Namelist. The first handles string parameters, the second integer.       */
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
extern int platform;

void ProcessNamelistStringParm( MQCFST    *pPCFString
                          , NameLParms *NlDefn
                          , MQLONG       *iCC
                          )
{
   /* Initialise return codes */
   *iCC = MQCC_OK;

   switch ( pPCFString->Parameter ) {
   case MQCA_NAMELIST_NAME:
      memset( NlDefn->NamelistName
            , ' ', MQ_NAMELIST_NAME_LENGTH );
      memcpy( NlDefn->NamelistName
            , pPCFString->String, pPCFString->StringLength );
      break;
   case MQCA_NAMELIST_DESC:
      memset( NlDefn->NamelistDesc
            , ' ', MQ_NAMELIST_DESC_LENGTH );
      memcpy( NlDefn->NamelistDesc
            , pPCFString->String, pPCFString->StringLength );
      break;
   case MQCA_ALTERATION_DATE:
      memset( NlDefn->AlterationDate
            , ' ', MQ_CREATION_DATE_LENGTH );
      memcpy( NlDefn->AlterationDate
            , pPCFString->String, pPCFString->StringLength);
      break;
   case MQCA_ALTERATION_TIME:
      memset( NlDefn->AlterationTime
            , ' ', MQ_CREATION_TIME_LENGTH );
      memcpy( NlDefn->AlterationTime
            , pPCFString->String, pPCFString->StringLength);
      break;

#if defined(zzzOS)
      /* v602 zOS attributes */
   case MQBACF_RESPONSE_ID:          /* zOS things to ignore */
     break;
   case MQCACF_RESPONSE_Q_MGR_NAME:  /* zOS things to ignore */
     break;
#endif

   default:
       if(noisy) fprintf(stderr, "(namelist.c) Unknown PCF String Parameter: %ld\n",
              pPCFString->Parameter);
      *iCC = WARNING;
      break;
   } /* endswitch */

   return;
}

/*************************************************************/
void ProcessNamelistStringListParm( MQCFSL    *pPCFString
                          , NameLParms *NlDefn
                          , MQLONG       *iCC
                          )
{
   /* Initialise return codes */
   *iCC = MQCC_OK;

   /* [v512] fixed this up too.... wms */
   switch ( pPCFString->Parameter ) {
   case MQCA_NAMES:
      makeStringFromList( (char*)NlDefn->Names,
                          pPCFString->Strings,
                          pPCFString->StringLength,
                          pPCFString->Count);
   break;
   default:
       if(noisy) fprintf(stderr, "(namelist.c) Unknown PCF String List Parameter: %ld\n",
              pPCFString->Parameter);
      *iCC = WARNING;
      break;
   } /* endswitch */

   return;
}

/*************************************************************/
void ProcessNamelistIntegerParm( MQCFIN    *pPCFInteger
                          , NameLParms *NlDefn
                           , MQLONG       *iCC
                           )
{
   /* Initialise return codes */
   *iCC = MQCC_OK;

   switch ( pPCFInteger->Parameter ) {
     case MQIA_NAME_COUNT:
        NlDefn->NameCount = pPCFInteger->Value;
        break;

#if defined(zzzOS)
      /* v602 zOS attributes */
   case MQIA_QSG_DISP:   /* zOS */
       NlDefn->qsgDisp = pPCFInteger->Value;
     break ;
   case MQIA_NAMELIST_TYPE:   /* zOS */
       NlDefn->nameListType = pPCFInteger->Value;
     break ;


#endif

   default:
       if(noisy) fprintf(stderr, "(namelist.c) Unknown PCF Integer Parameter: %ld\n",
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
void AddToFileNAMELIST(
                       NameLParms *pNlDefn,
                       int UserVersion,
                       char * fileName,   /* v32 Name of the output file  */
                        FILE * fp,       /* v51 */
                       int twolines,     /* v601 */
                       MQLONG *iCC )
{
   char *  ParmBuffer;   /* Temporary buffer to hold for output to file */
   int options = 0;
   char    work[256];

   memset(work, '\0', 256);

   if ( !( ParmBuffer = (char *) malloc (NAMELIST_BUFFER_SIZE) )) {
     fprintf (stderr, "(namelist.c) Failed to allocate %d bytes \n", NAMELIST_BUFFER_SIZE);
     exit(2);
   }

   /* Initialise return codes */
   *iCC = MQCC_OK;

   /* v601 setup options for oneLine operation */ 
   options = MQSPRINTF_X_DOUBLE_QUOTES + MQSPRINTF_X_NO_TRAILING_BLANKS;
   if (!oneLine) options += MQSPRINTF_X_LINE_BREAKS;

   if (twolines) fputs("* ", fp);

   /* Append these details to the end of the current output file (v32) */

      MQsprintf_X( ParmBuffer
               , "DEFINE NAMELIST ('%s') "
               , pNlDefn->NamelistName
               , MQ_NAMELIST_NAME_LENGTH
		   , options
               );
      fputs( ParmBuffer, fp );

      sprintf( ParmBuffer,  "       REPLACE %s", lineTerm );
      fputs(ParmBuffer, fp);

      if (header && (!oneLine || twolines)) {
	MQsprintf_X( ParmBuffer, "* ALTDATE (%s) ",
               pNlDefn->AlterationDate,
               MQ_CREATION_DATE_LENGTH,
               options);
	fputs( ParmBuffer, fp);
	MQsprintf_X( ParmBuffer, "* ALTTIME (%s) ",
               pNlDefn->AlterationTime,
               MQ_CREATION_TIME_LENGTH,
               options);
	fputs( ParmBuffer, fp);
      }


      MQsprintf_X( ParmBuffer
               , "       DESCR('%s') "
               , pNlDefn->NamelistDesc
               , MQ_NAMELIST_DESC_LENGTH
		   , options
               );
      fputs( ParmBuffer, fp );

#if defined(zzzOS)
      if (platform == MQPL_MVS) {
	  toStrQSG((char*)&work, pNlDefn->qsgDisp);
	  sprintf( ParmBuffer, "       QSGDISP(%s) %s", work, lineTerm );
	  fputs( ParmBuffer, fp );

	  toStrNLType((char*)&work, pNlDefn->nameListType);
	  sprintf( ParmBuffer, "       NLTYPE(%s) %s", work, lineTerm );
	  fputs( ParmBuffer, fp );
      }
#endif

      /* [v530] We have to use the new formatter routine here    */
      /* since we changed the way we're handling string lists    */
      MQsprintf_X( ParmBuffer
               , "        NAMES(%s) "
               , (char*) pNlDefn->Names
               , NAMELIST_BUFFER_SIZE
               , MQSPRINTF_X_NONE
               );
      fputs( ParmBuffer, fp );

      fputs("\n", fp);

      if (!oneLine) {
        fputs("\n", fp);
      }

   free (ParmBuffer);
   return;


}
/* end of don't compile anything */
#endif


