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
/* Module Name: archive.c                                                    */
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
/* ProcessArchiveStringParm()                                              */
/* ProcessArchiveIntegerParm()                                             */
/*                                                                          */
extern int noisy;
extern int header;
extern int oneLine;          /* v601 -- whether to write the entire DEFINE on oneline */
extern char lineTerm[3];     /* v601 -- line terminator */
extern int platform;

int firstArchive = 1;

void ProcessArchiveStringParm( MQCFST    *pPCFString
                          , ArchiveParms *pArchiveDefn
                          , MQLONG       *iCC
                          )
{
   /* Initialise return codes */
   *iCC = MQCC_OK;

   switch ( pPCFString->Parameter ) {
   case MQBACF_RESPONSE_ID:          /* zOS things to ignore */
   case MQCACF_RESPONSE_Q_MGR_NAME:  /* zOS things to ignore */
   case MQCACF_DATA_SET_NAME:
   case MQCACF_SYSP_LOG_CORREL_ID:
   case MQCACF_SYSP_UNIT_VOLSER:
   break;

   case  MQCACF_SYSP_ARCHIVE_UNIT1:
      memset( pArchiveDefn->archUnit1
            , ' ',  MQ_ARCHIVE_UNIT_LENGTH);
      memcpy( pArchiveDefn->archUnit1
            , pPCFString->String, pPCFString->StringLength );
     break;
   case  MQCACF_SYSP_ARCHIVE_UNIT2:
      memset( pArchiveDefn->archUnit2
            , ' ',  MQ_ARCHIVE_UNIT_LENGTH);
      memcpy( pArchiveDefn->archUnit2
            , pPCFString->String, pPCFString->StringLength );
     break;
   case  MQCACF_SYSP_ARCHIVE_PFX1:
      memset( pArchiveDefn->archPrefix1
            , ' ',  MQ_ARCHIVE_PFX_LENGTH);
      memcpy( pArchiveDefn->archPrefix1
            , pPCFString->String, pPCFString->StringLength );
     break;
   case  MQCACF_SYSP_ARCHIVE_PFX2:
      memset( pArchiveDefn->archPrefix2
            , ' ',  MQ_ARCHIVE_PFX_LENGTH);
      memcpy( pArchiveDefn->archPrefix2
            , pPCFString->String, pPCFString->StringLength );
     break;

   default:
       if(noisy) fprintf(stderr, "(Archive.c) Unknown PCF String Parameter: %ld\n",
              pPCFString->Parameter);
      *iCC = WARNING;
      break;
   } /* endswitch */

   return;
}


/*************************************************************/
void ProcessArchiveIntegerParm( MQCFIN    *pPCFInteger
                          , ArchiveParms *pArchiveDefn
                           , MQLONG       *iCC
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
       pArchiveDefn->type  = pPCFInteger->Value;
   break;
   case MQIACF_SYSP_ALLOC_UNIT:
       pArchiveDefn->allocUnit  = pPCFInteger->Value;
   break;
   case MQIACF_SYSP_ARCHIVE_RETAIN :
       pArchiveDefn->archRetain  = pPCFInteger->Value;
   break;
   case MQIACF_SYSP_ARCHIVE_WTOR:
       pArchiveDefn->archWtor  = pPCFInteger->Value;
   break;
   case MQIACF_SYSP_BLOCK_SIZE:
       pArchiveDefn->blkSize  = pPCFInteger->Value;
   break;
   case MQIACF_SYSP_CATALOG:
       pArchiveDefn->catalog  = pPCFInteger->Value;
   break;
   case MQIACF_SYSP_COMPACT:
       pArchiveDefn->compact  = pPCFInteger->Value;
   break;
   case MQIACF_SYSP_ALLOC_PRIMARY:
       pArchiveDefn->primaryAlloc  = pPCFInteger->Value;
   break;
   case MQIACF_SYSP_PROTECT:
       pArchiveDefn->protect  = pPCFInteger->Value;
   break;
   case MQIACF_SYSP_QUIESCE_INTERVAL:
       pArchiveDefn->quiesceInterval  = pPCFInteger->Value;
   break;
   case MQIACF_SYSP_ALLOC_SECONDARY:
       pArchiveDefn->secondaryAlloc  = pPCFInteger->Value;
   break;
   case MQIACF_SYSP_TIMESTAMP:
       pArchiveDefn->timestamp   = pPCFInteger->Value;
   break;
   default:
       if(noisy) fprintf(stderr, "(Archive.c) Unknown PCF Integer Parameter: %ld\n",
              pPCFInteger->Parameter);
      *iCC = WARNING;
      break;
   } /* endswitch */

   return;
}

void ProcessArchiveIntegerListParm( MQCFIL    *pPCFIntegerList
                          , ArchiveParms *pArchiveDefn
                          , MQLONG       *iCC
                          )
{
  int i;
   *iCC = MQCC_OK;
   i=0;

   switch ( pPCFIntegerList->Parameter ) {


   case MQIACF_SYSP_ROUTING_CODE:
     for (i=0; i<ZZRTNCODES; i++) {
       pArchiveDefn->rtnCodes[i] =  -1; 
     }

     for (i=0; i<pPCFIntegerList->Count && i<ZZRTNCODES; i++) {
       pArchiveDefn->rtnCodes[i] = pPCFIntegerList->Values[i];
     }
     break;

   default:
      if(noisy) fprintf(stderr, "(archive.c) Unknown PCF Integer List Parameter: %ld\n",
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
void AddToFileArchive(
                       ArchiveParms *pArchiveDefn,
                       int UserVersion,
                       char * fileName,   /* v32 Name of the output file  */
                        FILE * fp,       /* v51 */
                       int twolines,     /* v601 */ 
                       MQLONG *iCC )
{
   char *  ParmBuffer;   /* Temporary buffer to hold for output to file */
   int options ; /* options for MQsprintf_X */
   MQCHAR work[256];
   MQCHAR work2[8];
   int i=0;
 

   /* we don't make output if in oneline mode */
   if (oneLine) {
      fprintf(stderr, "Skipping ARCHIVE information in oneline mode\n");
      return;
   }

   if ( !( ParmBuffer = (char *) malloc (PBSIZE) )) {
     fprintf (stderr, "(Archive.c) Failed to allocate %d bytes \n", PBSIZE);
     exit(2);
   }

   options = MQSPRINTF_X_NO_TRAILING_BLANKS;

   /* Initialise return codes */
   *iCC = MQCC_OK;

   if (firstArchive) {
     firstArchive = 0;
     fprintf(fp, "******** Archive related definitions ********\n");
   }

   switch (pArchiveDefn-> type) {
     case MQSYSP_TYPE_INITIAL:

            switch (pArchiveDefn->allocUnit) {
 	     case MQSYSP_ALLOC_BLK:
	         fprintf(fp, "*(zparm) CSQ6ARVP ALCUNIT=BLK,\n");
	       break;
               case MQSYSP_ALLOC_TRK:
		 fprintf(fp, "*(zparm) CSQ6ARVP ALCUNIT=TRK,\n");
	       break;
	       case MQSYSP_ALLOC_CYL:
   	         fprintf(fp, "*(zparm) CSQ6ARVP ALCUNIT=CYL,\n");
	       break;
             }
             fprintf( fp, "*(zparm)          ARCRETN=%ld,\n", pArchiveDefn->archRetain );

             fprintf( fp, "*(zparm)          BLKSIZE=%ld,\n", pArchiveDefn->blkSize );

             fprintf( fp, "*(zparm)          PRIQTY=%ld,\n", pArchiveDefn->primaryAlloc );

             fprintf( fp, "*(zparm)          QUIESCE=%ld,\n", pArchiveDefn->quiesceInterval );

             fprintf( fp, "*(zparm)          SECQTY=%ld,\n", pArchiveDefn->secondaryAlloc );

             if (pArchiveDefn->catalog == MQSYSP_YES) {
	       fprintf( fp, "*(zparm)          CATALOG=YES,\n" );
             } else {
	       fprintf( fp, "*(zparm)          CATALOG=NO,\n" );
             }
             if (pArchiveDefn->compact == MQSYSP_YES) {
	       fprintf( fp, "*(zparm)          COMPACT=YES,\n" );
             } else {
	       fprintf( fp, "*(zparm)          COMPACT=NO,\n" );
             }
             if (pArchiveDefn->protect == MQSYSP_YES) {
	       fprintf( fp, "*(zparm)          PROTECT=YES,\n" );
             } else {
	       fprintf( fp, "*(zparm)          PROTECT=NO,\n" );
             }
             if (pArchiveDefn->archWtor == MQSYSP_YES) {
	       fprintf( fp, "*(zparm)          ARCWTOR=YES,\n" );
             } else {
	       fprintf( fp, "*(zparm)          ARCWTOR=NO,\n" );
             }

	     MQsprintf_X( ParmBuffer
			  , "*(zparm)          UNIT=%s,\n"
			  , pArchiveDefn->archUnit1
			  , MQ_ARCHIVE_UNIT_LENGTH
			  , options
			  );
	     fputs( ParmBuffer, fp );


	     MQsprintf_X( ParmBuffer
			  , "*(zparm)          UNIT2=%s,\n"
			  , pArchiveDefn->archUnit2
			  , MQ_ARCHIVE_UNIT_LENGTH
			  , options
			  );
	     fputs( ParmBuffer, fp );


	     MQsprintf_X( ParmBuffer
			  , "*(zparm)          ARCPFX1=%s,\n"
			  , pArchiveDefn->archPrefix1
			  , MQ_ARCHIVE_PFX_LENGTH
			  , options
			  );
	     fputs( ParmBuffer, fp );

	     MQsprintf_X( ParmBuffer
			  , "*(zparm)          ARCPFX2=%s,\n"
			  , pArchiveDefn->archPrefix2
			  , MQ_ARCHIVE_PFX_LENGTH
			  , options
			  );
	     fputs( ParmBuffer, fp );

             memset(work,'\0',sizeof(work));
             for(i=0;i<ZZRTNCODES;i++) {
	       if (pArchiveDefn->rtnCodes[i]!=-1) {
                 sprintf(work2, "%ld,", pArchiveDefn->rtnCodes[i]);
                 strcat(work,work2);
               }
             }
             if(strlen(work)) work[strlen(work)-1]='\0';
	     MQsprintf_X( ParmBuffer
			  , "*(zparm)          ARCWRTC=(%s),\n"
			  , work
			  , sizeof(work)
			  , options
			  );
	     fputs( ParmBuffer, fp );

              switch (pArchiveDefn->timestamp) {
 	     case MQSYSP_YES:
	         fprintf(fp, "*(zparm)          TSTAMP=YES\n");
	       break;
               case MQSYSP_NO:
		 fprintf(fp, "*(zparm)          TSTAMP=NO\n");
	       break;
	       case MQSYSP_EXTENDED:
   	         fprintf(fp, "*(zparm)          TSTAMP=EXT\n");
	       break;
             }

  
             fprintf( fp, "\n\n");
 
     break;

     case MQSYSP_TYPE_SET:

             fprintf(fp, "*(12cr) SET ARCHIVE +\n"  );

            switch (pArchiveDefn->allocUnit) {
 	     case MQSYSP_ALLOC_BLK:
	         fprintf(fp, "*(12cr)       ALCUNIT(BLK) +\n");
	       break;
               case MQSYSP_ALLOC_TRK:
		 fprintf(fp, "*(12cr)       ALCUNIT(TRK) +\n");
	       break;
	       case MQSYSP_ALLOC_CYL:
   	         fprintf(fp, "*(12cr)       ALCUNIT(CYL) +\n");
	       break;
             }


             fprintf( fp, "*(12cr)        ARCRETN(%ld) +\n", pArchiveDefn->archRetain );

             fprintf( fp, "*(12cr)        BLKSIZE(%ld) +\n", pArchiveDefn->blkSize );

             fprintf( fp, "*(12cr)        PRIQTY(%ld) +\n", pArchiveDefn->primaryAlloc );

             fprintf( fp, "*(12cr)        QUIESCE(%ld) +\n", pArchiveDefn->quiesceInterval );

             fprintf( fp, "*(12cr)        SECQTY(%ld) +\n", pArchiveDefn->secondaryAlloc );

             if (pArchiveDefn->catalog == MQSYSP_YES) {
	       fprintf( fp, "*(12cr)       CATALOG(YES) +\n" );
             } else {
	       fprintf( fp, "*(12cr)       CATALOG(NO) +\n" );
             }
             if (pArchiveDefn->compact == MQSYSP_YES) {
	       fprintf( fp, "*(12cr)       COMPACT(YES) +\n" );
             } else {
	       fprintf( fp, "*(12cr)       COMPACT(NO) +\n" );
             }
             if (pArchiveDefn->protect == MQSYSP_YES) {
	       fprintf( fp, "*(12cr)       PROTECT(YES) +\n" );
             } else {
	       fprintf( fp, "*(12cr)       PROTECT(NO) +\n" );
             }
             if (pArchiveDefn->archWtor == MQSYSP_YES) {
	       fprintf( fp, "*(12cr)       ARCWTOR(YES) +\n" );
             } else {
	       fprintf( fp, "*(12cr)       ARCWTOR(NO) +\n" );
             }

	     MQsprintf_X( ParmBuffer
			  ,           "*(12cr)       UNIT(%s) +\n"
			  , pArchiveDefn->archUnit1
			  , MQ_ARCHIVE_UNIT_LENGTH
			  , options
			  );
	     fputs( ParmBuffer, fp );


	     MQsprintf_X( ParmBuffer
			  ,           "*(12cr)       UNIT2(%s) +\n"
			  , pArchiveDefn->archUnit2
			  , MQ_ARCHIVE_UNIT_LENGTH
			  , options
			  );
	     fputs( ParmBuffer, fp );


	     MQsprintf_X( ParmBuffer
			  ,           "*(12cr)       ARCPFX1(%s) +\n"
			  , pArchiveDefn->archPrefix1
			  , MQ_ARCHIVE_PFX_LENGTH
			  , options
			  );
	     fputs( ParmBuffer, fp );

	     MQsprintf_X( ParmBuffer
			  ,           "*(12cr)       ARCPFX2(%s) +\n"
			  , pArchiveDefn->archPrefix2
			  , MQ_ARCHIVE_PFX_LENGTH
			  , options
			  );
	     fputs( ParmBuffer, fp );

             memset(work,'\0',sizeof(work));
             for(i=0;i<14;i++) {
	       if (pArchiveDefn->rtnCodes[i]!=-1) {
                 sprintf(work2, "%ld,", pArchiveDefn->rtnCodes[i]);
                 strcat(work,work2);
               }
             }
             if(strlen(work)) work[strlen(work)-1]='\0';
	     MQsprintf_X( ParmBuffer
			  ,           "*(12cr)       ARCWRTC(%s) +\n"
			  , work
			  , sizeof(work)
			  , options
			  );
	     fputs( ParmBuffer, fp );

             switch (pArchiveDefn->timestamp) {
	       case MQSYSP_YES:
 	         fprintf(fp, "*(12cr)       TSTAMP(YES)\n");
	       break;
	       case MQSYSP_NO:
	         fprintf(fp, "*(12cr)       TSTAMP(NO)\n");
	       break;
	       case MQSYSP_EXTENDED:
	         fprintf(fp, "*(12cr)       TSTAMP(EXT)\n");
	       break;
             }

             fprintf( fp, "\n\n");

       break;

     case MQSYSP_TYPE_ARCHIVE_TAPE:
       /* Nothing to do here ! */
     break;

   default:
       if(noisy) fprintf(stderr, "(Archive.c) Unknown MQIACF_SYSP_TYPE: %ld\n",
              pArchiveDefn-> type);
      *iCC = WARNING;
      break;
   } /* endswitch */

   free (ParmBuffer);
   return;


}

/* end of don't compile anything */
#endif
