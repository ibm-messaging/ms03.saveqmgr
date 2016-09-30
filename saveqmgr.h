/*===========================================================================*/
/* Module Name: SAVEQMGR.H                                                   */
/*  DESCRIPTIVE NAME WebSphere MQ Save Queue Manager Object                  */
/*                  Definitions using PCFs (ms03 supportpac)                 */
/*                                                                           */
/*===========================================================================*/
#ifndef SAVEQMGRH__ 
#define SAVEQMGRH__
#include <cmqc.h>
#include <cmqcfc.h>
#include <cmqxc.h>
#include "args.h"

#define THISVERSION "SAVEQMGR V6.1.3.1"

/* [v6] We're going to start using zzMQVnn to indicate version numbers and */
/* [v6] have a centrailized place for making that determination */
#if defined(MQIA_STATISTICS_Q)
#define zzMQV60 1L
#endif

#if defined(MQIA_TREE_LIFE_TIME)
#define zzMQV70 1L
#endif

#if defined(MQIACF_SELECTOR_TYPE)
#define zzMQV701 1L
#endif

#if defined MQSCO_DEFAULT
#define USESSL 1
#endif

/* should we compile for zOS ? */
#if defined(MQ_SYSP_SERVICE_LENGTH) && defined(zzMQV60)
#define zzzOS 1L
#endif

#define WAIT_INT     60000           /* 1 minute */
#define WAIT_INT_SHORT     10000     /* v531a */      /* short wait interval */
/* Application return codes */
/* [OvE] On OpenVMS, switch WARNING and OK */

#if MQAT_DEFAULT == MQAT_VMS
#define  WARNING       0  /* we (probably) created output, but there were problems */
#define  OK            1  /* everything worked, no problems encountered */
#define  ERROR         2  /* nothing was created for output */
#else
#define  OK            0  /* everything worked, no problems encountered */
#define  WARNING       1  /* we (probably) created output, but there were problems */
#define  ERROR         2  /* nothing was created for output */
#endif


/* [WMS] Using predefined constants instead of hard coding numbers for */
/* string sizes.                                                       */

/* defining these command levels removes the need for a lot of */
/* conditional testing in the source code                      */

#ifndef MQCMDL_LEVEL_1
#define MQCMDL_LEVEL_1   100L
#endif

#ifndef MQCMDL_LEVEL_101
#define MQCMDL_LEVEL_101 101L
#endif

#ifndef MQCMDL_LEVEL_110
#define MQCMDL_LEVEL_110 110L
#endif

#ifndef MQCMDL_LEVEL_114
#define MQCMDL_LEVEL_114 114L
#endif

#ifndef MQCMDL_LEVEL_120
#define MQCMDL_LEVEL_120 120L
#endif

#ifndef MQCMDL_LEVEL_200
#define MQCMDL_LEVEL_200 200L
#endif

#ifndef MQCMDL_LEVEL_201
#define MQCMDL_LEVEL_201 201L
#endif

#ifndef MQCMDL_LEVEL_210
#define MQCMDL_LEVEL_210 210L
#endif

#ifndef MQCMDL_LEVEL_220
#define MQCMDL_LEVEL_220 220L
#endif

#ifndef MQCMDL_LEVEL_221
#define MQCMDL_LEVEL_221 221L
#endif

#ifndef MQCMDL_LEVEL_320
#define MQCMDL_LEVEL_320 320L
#endif

#ifndef MQCMDL_LEVEL_420
#define MQCMDL_LEVEL_420 420L
#endif

#ifndef MQCMDL_LEVEL_500
#define MQCMDL_LEVEL_500 500L
#endif

#ifndef MQCMDL_LEVEL_510
#define MQCMDL_LEVEL_510 510L
#endif

/* Note, we don't define MQCMDL_LEVEL_520 because there are no */
/* attributes introduced in v5.2 which we can used to determine */
/* the "compiled" level of the program.  Besides, we don't use */
/* it in any other preprocessor directives */

#ifndef MQCMDL_LEVEL_530
#define MQCMDL_LEVEL_530 530L
#endif

#ifndef MQCMDL_LEVEL_600
#define MQCMDL_LEVEL_600 600
#endif
#ifndef MQCMDL_LEVEL_700
#define MQCMDL_LEVEL_700 700L
#endif

/* [v520] Included for Tandem NSK changes */
#ifndef MQ_TOTAL_EXIT_DATA_LENGTH
#define MQ_TOTAL_EXIT_DATA_LENGTH 999L
#endif
#ifndef MQ_TOTAL_EXIT_NAME_LENGTH
#define MQ_TOTAL_EXIT_NAME_LENGTH 999L
#endif

/* V32 special command type to get clntconn channels */
#define XZCMD_INQUIRE_CHANNEL_CLNT MQMT_APPL_FIRST+1

/* V513 get the right length for MCAUserId field */
#if defined(MQ_MAX_MCA_USER_ID_LENGTH)
#define XZMCAUSERLEN MQ_MAX_MCA_USER_ID_LENGTH
#else
#define XZMCAUSERLEN MQ_USER_ID_LENGTH
#endif

/*  Lenght for strings that we don't know the length of ... */
#define ZZSTRLEN 256
/* maximum zOS routing codes */
#define ZZRTNCODES 16
/* maximum trace classes */
#define ZZTRCCLASSES 4

typedef struct QMgrParms {
	MQCHAR QMgrName[MQ_Q_MGR_NAME_LENGTH];
	MQCHAR QMgrDesc[MQ_Q_MGR_DESC_LENGTH];
	MQLONG Platform;
	MQLONG CommandLevel; /*[wms]level 500 indicates V5 etc */
	MQLONG TriggerInterval;
	MQCHAR DeadLetterQName[MQ_Q_NAME_LENGTH];
	MQLONG MaxPriority;
	MQCHAR CommandInputQName[MQ_Q_NAME_LENGTH];
	MQCHAR DefXmitQName[MQ_Q_NAME_LENGTH];
	MQLONG CodedCharSetId;
	MQLONG MaxHandles;
	MQLONG MaxUncommittedMsgs;
	MQLONG MaxMsgLength;
	MQLONG SyncPoint;
	MQLONG AuthorityEvent;
	MQLONG InhibitEvent;
	MQLONG LocalEvent;
	MQLONG RemoteEvent;
	MQLONG StartStopEvent;
	MQLONG PerformanceEvent;

	MQLONG DistLists; /* [wms] V5 */
	MQLONG ChannelAutoDef; /* [wms] V5 */
	MQLONG ChannelAutoDefEvent; /* [wms] V5 */
	MQCHAR ChannelAutoDefExit[MQ_EXIT_NAME_LENGTH]; /* [wms] V5 */

	MQCHAR WorkLoadExit[MQ_EXIT_NAME_LENGTH]; /* [v51] V51 */
	MQCHAR WorkLoadData[MQ_EXIT_DATA_LENGTH]; /* [v51] V51 */
	MQLONG WorkLoadLength; /* [v51] V51 */
	MQCHAR CreationDate[MQ_CREATION_DATE_LENGTH]; /*[v51] V51 */
	MQCHAR CreationTime[MQ_CREATION_TIME_LENGTH]; /*[v51] V51 */
	MQCHAR AlterationDate[MQ_CREATION_DATE_LENGTH]; /*[v51] V51 */
	MQCHAR AlterationTime[MQ_CREATION_TIME_LENGTH]; /*[v51] V51 */
#if defined (MQ_OBJECT_NAME_LENGTH)
MQCHAR RepositoryName[MQ_OBJECT_NAME_LENGTH]; /* [v51] V51 */
MQCHAR RepositoryNameList[MQ_OBJECT_NAME_LENGTH]; /* [v51] V51 */
MQCHAR QMgrId[MQ_Q_MGR_IDENTIFIER_LENGTH]; /*[v51] V51 */
#endif

#if defined (MQ_SSL_KEY_REPOSITORY_LENGTH)
MQCHAR SslCrlNameList[MQ_OBJECT_NAME_LENGTH]; /*[v53] V53 */
MQCHAR SslKeyRepos[MQ_SSL_KEY_REPOSITORY_LENGTH]; /*[v53] V53 */
MQCHAR SslCryptoHW[MQ_SSL_CRYPTO_HARDWARE_LENGTH]; /*[v53] V53 */
#endif

/* [v6] new V6 attributes */
#ifdef zzMQV60
MQLONG StatQ; /* MQIA_STATISTICS_Q */
MQLONG ChannelEvent; /* MQIA_CHANNEL_EVENT */
MQLONG SSLEvent; /* MQIA_SSL_EVENT */
MQLONG SSLFIPSReqd; /* MQIA_SSL_FIPS_REQUIRED */
MQLONG IPAddressVersion; /* MQIA_IP_ADDRESS_VERSION */
MQLONG LoggerEvent; /* MQIA_LOGGER_EVENT */
MQLONG ChinitCtl; /* MQIA_CHINIT_CONTROL */
MQLONG CmdSvrCtl; /* MQIA_CMD_SERVER_CONTROL */
MQLONG StatsMQI; /* MQIA_STATISTICS_MQI */
MQLONG StatsChl; /* MQIA_STATISTICS_CHANNEL */
MQLONG StatsAutoClsr; /* MQIA_STATISTICS_AUTO_CLUSSDR */
MQLONG StatsIntv; /* MQIA_STATISTICS_INTERVAL */
MQLONG ActngMQI; /* MQIA_ACCOUNTING_MQI */
MQLONG ActngQ; /* MQIA_ACCOUNTING_Q */
MQLONG ActngInt; /* MQIA_ACCOUNTING_INTERVAL */
MQLONG ActngConnOver; /* MQIA_ACCOUNTING_CONN_OVERRIDE */
MQLONG TrcRteRecord; /* MQIA_TRACE_ROUTE_RECORDING */
MQLONG ActivityRecord; /* MQIA_ACTIVITY_RECORDING */
MQLONG MonChl; /* MQIA_MONITORING_CHANNEL */
MQLONG MonQ; /* MQIA_MONITORING_Q */
MQLONG MonAutoClsr; /* MQIA_MONITORING_AUTO_CLUSSDR */
MQLONG ClwlMRUChl; /* MQIA_CLWL_MRU_CHANNELS */
MQLONG ClwlUseQ; /* MQIA_CLWL_USEQ */
MQLONG SSLResetCnt; /* MQIA_SSL_RESET_COUNT */
#endif  /* end of v6 attributes */
/* [v7] new V7 attributes */
#ifdef zzMQV70
MQLONG MaxPropsLen; /* MQIA_MAX_PROPERTIES_LENGTH (MAXPROPL) */
MQCHAR ParentQMgr[MQ_Q_MGR_NAME_LENGTH]; /* MQCA_PARENT (PARENT)*/
MQLONG PubSubMode; /* MQIA_PUBSUB_MODE (PSMODE)*/
MQLONG PubSubRtyCnt; /* MQIA_PUBSUB_MAXMSG_RETRY_COUNT (PSRTYCNT) */
MQLONG PubSubNPMsg; /* MQIA_PUBSUB_NP_MSG (PSNPMSG)*/
MQLONG PubSubNPResp; /* MQIA_PUBSUB_NP_RESP (PSNPRES)*/
MQLONG PubSubSyncPt; /* MQIA_PUBSUB_SYNC_PT (PSSYNCPT)*/
MQLONG MkBrInt; /* MQIA_MSG_MARK_BROWSE_INTERVAL (MARKINT) */
MQLONG Treelife; /* MQIA_TREE_LIFE_TIME */
#endif  /* end of v7 attributes */

#if defined(zzzOS)
MQLONG expiryInterval; /*  MQIA_EXPIRY_INTERVAL */
MQLONG configurationEvent; /* MQIA_CONFIGURATION_EVENT */
MQLONG commandEvent; /* MQIA_COMMAND_EVENT */
MQLONG bridgeEvent; /* MQIA_BRIDGE_EVENT */
MQLONG sharedQQmgrName; /* MQIA_SHARED_Q_Q_MGR_NAME */

MQLONG sslTasks; /* MQIA_SSL_TASKS */
MQLONG igq; /* MQIA_INTRA_GROUP_QUEUING */
MQLONG igqPutAuth; /* MQIA_IGQ_PUT_AUTHORITY */
MQLONG chinitAdapters; /* MQIA_CHINIT_ADAPTERS */
MQLONG chinitDispatchers; /* MQIA_CHINIT_DISPATCHERS  */

MQLONG tcpChannels; /* MQIA_TCP_CHANNELS */
MQLONG tcpKeepAlive; /* MQIA_TCP_KEEP_ALIVE */
MQLONG tcpStack; /* MQIA_TCP_STACK_TYPE */
MQLONG outBoundPortMin; /* MQIA_OUTBOUND_PORT_MIN */
MQLONG outBoundPortMax; /* MQIA_OUTBOUND_PORT_MAX */

MQLONG dnsWlm; /* MQIA_DNS_WLM */
MQLONG receiveTimeout; /* MQIA_RECEIVE_TIMEOUT */
MQLONG receiveTimeoutType; /* MQIA_RECEIVE_TIMEOUT_TYPE */
MQLONG receiveTimeoutMin; /* MQIA_RECEIVE_TIMEOUT_MIN */
MQLONG adoptNewMcaCheck; /* MQIA_ADOPTNEWMCA_CHECK */

MQLONG adoptNewMcaType; /* MQIA_ADOPTNEWMCA_TYPE */
MQLONG chinitTraceTableSize; /* MQIA_CHINIT_TRACE_TABLE_SIZE */
MQLONG chinitTraceAutoStart; /* MQIA_CHINIT_TRACE_AUTO_START */
MQLONG lu62Channels; /* MQIA_LU62_CHANNELS */
MQLONG maxChannels; /* MQIA_MAX_CHANNELS */

MQLONG activeChannels; /* MQIA_ACTIVE_CHANNELS */
MQLONG listenerTimer; /* MQIA_LISTENER_TIMER */
MQLONG cpiLevel; /* MQIA_CPI_LEVEL */

MQCHAR igqUserId[MQ_USER_ID_LENGTH]; /* MQCA_IGQ_USER_ID */
MQCHAR tcpName[ZZSTRLEN]; /* MQCA_TCP_NAME */
MQCHAR dnsGroup[ZZSTRLEN]; /* MQCA_DNS_GROUP */
MQCHAR chinitServiceParm[ZZSTRLEN]; /* MQCA_CHINIT_SERVICE_PARM */
MQCHAR luGroupName[ZZSTRLEN]; /* MQCA_LU_GROUP_NAME */

MQCHAR luName[ZZSTRLEN]; /* MQCA_LU_NAME */
MQCHAR luArmSuffix[ZZSTRLEN]; /* MQCA_LU62_ARM_SUFFIX */
MQCHAR qsgName[MQ_QSG_NAME_LENGTH]; /* MQCA_QSG_NAME */
#ifdef zzMQV70
/* zOS v7 specific parms */
MQLONG securityCase; /* MQIA_SECURITY_CASE */
#endif
#endif

} QMgrParms;

typedef struct QParms {
	MQCHAR QName[MQ_Q_NAME_LENGTH];
	MQLONG QType;
	MQCHAR QDesc[MQ_Q_DESC_LENGTH];
	MQLONG InhibitGet;
	MQLONG InhibitPut;
	MQLONG DefPriority;
	MQLONG DefPersistence;
	MQCHAR ProcessName[MQ_PROCESS_NAME_LENGTH];
	MQLONG MaxQDepth;
	MQLONG MaxMsgLength;
	MQLONG BackoutThreshold;
	MQCHAR BackoutReqQName[MQ_Q_NAME_LENGTH];
	MQLONG Shareability;
	MQLONG DefInputOpenOption;
	MQLONG HardenGetBackout;
	MQLONG MsgDeliverySequence;
	MQLONG RetentionInterval;
	MQLONG DefinitionType;
	MQLONG Usage;
	MQLONG OpenInputCount;
	MQLONG OpenOutputCount;
	MQLONG CurrentQDepth;
	MQCHAR CreationDate[MQ_CREATION_DATE_LENGTH];
	MQCHAR CreationTime[MQ_CREATION_TIME_LENGTH];
	MQCHAR InitiationQName[MQ_Q_NAME_LENGTH];
	MQLONG TriggerControl;
	MQLONG TriggerType;
	MQLONG TriggerMsgPriority;
	MQLONG TriggerDepth;
	MQCHAR TriggerData[MQ_TRIGGER_DATA_LENGTH];
	MQCHAR BaseQName[MQ_Q_NAME_LENGTH];
	MQCHAR RemoteQName[MQ_Q_NAME_LENGTH];
	MQCHAR RemoteQMgrName[MQ_Q_NAME_LENGTH];
	MQCHAR XmitQName[MQ_Q_NAME_LENGTH];
	MQLONG Scope;
	MQLONG QDepthHighLimit;
	MQLONG QDepthLowLimit;
	MQLONG QDepthMaxEvent;
	MQLONG QDepthHighEvent;
	MQLONG QDepthLowEvent;
	MQLONG QServiceInterval;
	MQLONG QServiceIntervalEvent;
	MQLONG DistLists; /*[wms] V5 */
	MQCHAR AlterationDate[MQ_CREATION_DATE_LENGTH]; /*[v51] V51 */
	MQCHAR AlterationTime[MQ_CREATION_TIME_LENGTH]; /*[v51] V51 */
#if defined (MQ_OBJECT_NAME_LENGTH)
	MQCHAR ClusterName[MQ_CLUSTER_NAME_LENGTH]; /*[v51] V51 */
	MQCHAR ClusterNameList[MQ_OBJECT_NAME_LENGTH]; /*[v51] V51 */
	MQLONG DefBind; /*[v51] V51 */
#endif
	MQLONG npmClass; /* v533 */
/* v6 attributes */
#if defined(zzMQV60)
MQLONG StatQ; /* MQIA_STATISTICS_Q */
MQLONG ActngQ; /* MQIA_ACCOUNTING_Q */
MQLONG MonQ; /* MQIA_MONITORING_Q */
MQLONG ClwlRank; /* MQIA_CLWL_Q_RANK */
MQLONG ClwlPri; /* MQIA_CLWL_Q_PRIORITY */
MQLONG ClwlUseQ; /* MQIA_CLWL_USEQ */
#endif
#if defined(zzMQV70)
MQLONG DefPResP; /* MQIA_DEF_PUT_RESPONSE_TYPE (184)*/
MQLONG DefRdAhd; /* MQIA_DEF_READ_AHEAD (188)*/
MQLONG PropCntl; /* MQIA_PROPERTY_CONTROL (190)*/
MQLONG TargType; /* MQIA_BASE_TYPE (193) */
#endif

#if defined(zzzOS)
/* zOS stuff */
MQLONG qsgDisp; /* MQIA_QSG_DISP */
MQLONG pagesetId; /* MQIA_PAGESET_ID */
MQLONG indexType; /* MQIA_INDEX_TYPE */
MQCHAR cfStrucName[MQ_CF_STRUC_NAME_LENGTH]; /* MQCA_CF_STRUC_NAME */
MQCHAR storageClass[MQ_STORAGE_CLASS_LENGTH]; /* MQCA_STORAGE_CLASS */
#define TPIPE_BUFFER_SIZE  MQ_TPIPE_NAME_LENGTH*16
MQCHAR tpipes[TPIPE_BUFFER_SIZE]; /* MQCA_TPIPE_NAME */
#endif

} QParms;

typedef struct ChlParms {
	MQCHAR ChannelName[MQ_CHANNEL_NAME_LENGTH];
	MQLONG ChannelType;
	MQLONG TransportType;
	MQCHAR ModeName[MQ_MODE_NAME_LENGTH];
	MQCHAR TpName[MQ_TP_NAME_LENGTH];
	MQCHAR QMgrName[MQ_Q_MGR_NAME_LENGTH];
	MQCHAR XmitQName[MQ_Q_NAME_LENGTH];
	MQCHAR ConnectionName[MQ_CONN_NAME_LENGTH];
	MQCHAR MCAName[MQ_MCA_NAME_LENGTH];
	MQCHAR ChannelDesc[MQ_CHANNEL_DESC_LENGTH];
	MQLONG BatchSize;
	MQLONG DiscInterval;
	MQLONG ShortRetryCount;
	MQLONG ShortRetryInterval;
	MQLONG LongRetryCount;
	MQLONG LongRetryInterval;
	MQLONG DataConversion;
	MQCHAR SecurityExit[MQ_EXIT_NAME_LENGTH];

	/* [v530] Flags to tell if the following attributes are only strings */
	/* or lists. Ie- lists are used for chaining and a */
	/* value of 1 indicates list */
	MQLONG MsgExitListFlag;
	MQLONG SendExitListFlag;
	MQLONG ReceiveExitListFlag;
	MQLONG MsgUserDataListFlag;
	MQLONG SendUserDataListFlag;
	MQLONG ReceiveUserDataListFlag;
	MQLONG MsgRetryUserDataListFlag;
	MQLONG MsgRetryExitListFlag;

	/* [v512] becuase we actually store this as 'exit1','exit2','exit3' */
	/* we double the size to allow for the quotes and commas            */
	MQCHAR MsgExit[MQ_TOTAL_EXIT_NAME_LENGTH*2];
	MQCHAR SendExit[MQ_TOTAL_EXIT_NAME_LENGTH*2];
	MQCHAR ReceiveExit[MQ_TOTAL_EXIT_NAME_LENGTH*2];
	MQCHAR MsgUserData[MQ_TOTAL_EXIT_DATA_LENGTH*2];
	MQCHAR SendUserData[MQ_TOTAL_EXIT_DATA_LENGTH*2];
	MQCHAR ReceiveUserData[MQ_TOTAL_EXIT_DATA_LENGTH*2];
	MQCHAR MsgRetryExit[MQ_TOTAL_EXIT_NAME_LENGTH*2];
	MQCHAR MsgRetryUserData[MQ_TOTAL_EXIT_DATA_LENGTH*2];

	MQLONG PutAuthority;
	MQLONG SeqNumberWrap;
	MQLONG MaxMsgLength;
	MQCHAR SecurityUserData[MQ_EXIT_DATA_LENGTH];
	MQLONG MCAType;
	MQCHAR MCAUserIdentifier[XZMCAUSERLEN];
	MQCHAR UserIdentifier[MQ_USER_ID_LENGTH];
	MQCHAR Password[MQ_PASSWORD_LENGTH];

	MQLONG MsgRetryCount; /*[wms]  */
	MQLONG MsgRetryInterval; /*[wms]  */

	MQLONG BatchInterval; /*[wms]V5*/
	MQLONG HeartbeatInterval; /*[wms]V5*/
	MQLONG NonPersistentMsgSpeed; /*[wms]V5*/

	MQCHAR AlterationDate[MQ_CREATION_DATE_LENGTH]; /*[v51] V51 */
	MQCHAR AlterationTime[MQ_CREATION_TIME_LENGTH]; /*[v51] V51 */
#if defined (MQ_OBJECT_NAME_LENGTH)
	MQCHAR ClusterName[MQ_CLUSTER_NAME_LENGTH]; /*[v51] V51 */
	MQCHAR ClusterNameList[MQ_OBJECT_NAME_LENGTH]; /*[v51] V51 */
	MQLONG NetworkPriority; /* [v51] v51 */
#endif
	MQLONG InhibitAutostart; /* [v520] tandem nsk */

#if defined (MQCACH_LOCAL_ADDRESS)                  /* [v530] attributes */
	MQLONG BatchHB; /* Batch Heartbeat */
	MQLONG SslCAuth; /* Client Auth Required */
	MQCHAR LocalAddr[MQ_LOCAL_ADDRESS_LENGTH];
	MQCHAR SslCipher[MQ_SSL_CIPHER_SPEC_LENGTH];
	MQCHAR SslPeer[MQ_SSL_PEER_NAME_LENGTH];

#endif
	MQLONG KAInt; /* [v531] KAINT */

#ifdef zzMQV60
MQLONG MonChl; /* MQIA_MONITORING_CHANNEL      */
MQLONG StatsChl; /* MQIA_STATISTICS_CHANNEL      */
MQLONG Rank; /* MQIACH_CLWL_CHANNEL_RANK     */
MQLONG Priority; /* MQIACH_CLWL_CHANNEL_PRIORITY */
MQLONG Weight; /* MQIACH_CLWL_CHANNEL_WEIGHT   */
#define COMP_ARRAY_SIZE 16
MQLONG HdrComp[COMP_ARRAY_SIZE]; /* MQIACH_HDR_COMPRESSION */
MQLONG MsgComp[COMP_ARRAY_SIZE]; /* MQIACH_MSG_COMPRESSION */
#endif
#ifdef zzMQV70

MQLONG PropCntl; /* MQIA_PROPERTY_CONTROL (PROPCTL) (190)*/
MQLONG ShareCnv; /* MQIACH_SHARING_CONVERSATIONS (SHARECNV) (1615)*/
MQLONG MaxInsts; /* MQIACH_MAX_INSTANCES (MAXINST) (1618) */
MQLONG MaxInstsC; /* MQIACH_MAX_INSTS_PER_CLIENT (MAXINSTC) (1619) */
MQLONG Affinity; /* MQIACH_CONNECTION_AFFINITY (AFFINITY) (1621) */
MQLONG DefCDsp; /* MQIACH_DEF_CHANNEL_DISP (DEFCDSP) (1614) */
MQLONG ClntChlWght; /*MQIACH_CLIENT_CHANNEL_WEIGHT (CLNTWGHT) (1620) */

#endif

#if defined(zzzOS)
/* zOS stuff */
MQLONG qsgDisp; /* MQIA_QSG_DISP */
#endif

} ChlParms;

typedef struct ProcParms {
	MQCHAR ProcessName[MQ_PROCESS_NAME_LENGTH];
	MQCHAR ProcessDesc[MQ_PROCESS_DESC_LENGTH];
	MQLONG ApplType;
	MQCHAR ApplID[MQ_PROCESS_APPL_ID_LENGTH];
	MQCHAR EnvData[MQ_PROCESS_ENV_DATA_LENGTH];
	MQCHAR UserData[MQ_PROCESS_USER_DATA_LENGTH];

	MQCHAR AlterationDate[MQ_CREATION_DATE_LENGTH]; /*[v51] V51 */
	MQCHAR AlterationTime[MQ_CREATION_TIME_LENGTH]; /*[v51] V51 */

#if defined(zzzOS)
/* zOS stuff */
MQLONG qsgDisp; /* MQIA_QSG_DISP */
#endif

} ProcParms;

#if defined (MQ_OBJECT_NAME_LENGTH)
/* [v512] see the above comment                                   */
#define NAMELIST_BUFFER_SIZE ( MQNC_MAX_NAMELIST_NAME_COUNT*MQ_OBJECT_NAME_LENGTH * 2 )
typedef struct NameLParms {
	MQCHAR NamelistName[MQ_NAMELIST_NAME_LENGTH];
	MQCHAR NamelistDesc[MQ_NAMELIST_DESC_LENGTH];

	MQCHAR AlterationDate[MQ_CREATION_DATE_LENGTH]; /*[v51] V51 */
	MQCHAR AlterationTime[MQ_CREATION_TIME_LENGTH]; /*[v51] V51 */
	MQLONG NameCount;
	MQCHAR Names[NAMELIST_BUFFER_SIZE];

#if defined(zzzOS)
	/* zOS stuff */
	MQLONG qsgDisp; /* MQIA_QSG_DISP */
	MQLONG nameListType;
#endif

}NameLParms;
#endif

/* v530 Authinfo */

#if defined (MQ_AUTH_INFO_NAME_LENGTH)
typedef struct AuthInfoParms {
	MQCHAR AuthInfoName[MQ_AUTH_INFO_NAME_LENGTH];
	MQCHAR AuthInfoDesc[MQ_AUTH_INFO_DESC_LENGTH];
	MQCHAR AlterationDate[MQ_CREATION_DATE_LENGTH]; /*[v51] V51 */
	MQCHAR AlterationTime[MQ_CREATION_TIME_LENGTH]; /*[v51] V51 */
#if defined(zzMQV701)
	MQCHAR OcspURL[MQ_AUTH_INFO_OCSP_URL_LENGTH];
#endif
	MQCHAR ConnName[MQ_AUTH_INFO_CONN_NAME_LENGTH];
	MQCHAR Password[MQ_LDAP_PASSWORD_LENGTH];
	MQLONG AuthInfoType;
	MQCHAR UserName[1024 /*MQ_LDAP_USER_NAME_LENGTH*/];

#if defined(zzzOS)
	/* zOS stuff */
	MQLONG qsgDisp; /* MQIA_QSG_DISP */
#endif

}AuthInfoParms;
#endif

#if defined (zzMQV60)
typedef struct ListenerParms {
	MQCHAR ListenerName[MQ_LISTENER_NAME_LENGTH];
	MQCHAR ListenerDesc[MQ_LISTENER_DESC_LENGTH];
	MQCHAR AlterationDate[MQ_CREATION_DATE_LENGTH];
	MQCHAR AlterationTime[MQ_CREATION_TIME_LENGTH];
	MQCHAR TpName[MQ_TP_NAME_LENGTH];
	MQCHAR LocalName[MQ_CONN_NAME_LENGTH];
	MQCHAR IPAddress[MQ_CONN_NAME_LENGTH];
	MQLONG TransportType;
	MQLONG Control;
	MQLONG Backlog;
	MQLONG Port;
	MQLONG SessionCount;
	MQLONG Adapter;
	MQLONG CommandCount;
	MQLONG Socket;
	MQLONG NameCount;

}ListenerParms;
typedef struct ServiceParms {
	MQCHAR ServiceName[MQ_OBJECT_NAME_LENGTH];
	MQCHAR ServiceDesc[MQ_SERVICE_DESC_LENGTH];
	MQCHAR AlterationDate[MQ_CREATION_DATE_LENGTH];
	MQCHAR AlterationTime[MQ_CREATION_TIME_LENGTH];
	MQCHAR StartCommand[MQ_SERVICE_COMMAND_LENGTH]; /*  MQCA_SERVICE_START_COMMAND */
	MQCHAR StartArgs[MQ_SERVICE_ARGS_LENGTH]; /*  MQCA_SERVICE_START_ARGS */
	MQCHAR StopCommand[MQ_SERVICE_COMMAND_LENGTH]; /*  MQCA_SERVICE_STOP_COMMAND */
	MQCHAR StopArgs[MQ_SERVICE_ARGS_LENGTH]; /*  MQCA_SERVICE_STOP_ARGS */
	MQCHAR StdoutDestination[MQ_SERVICE_PATH_LENGTH]; /*  MQCA_STDOUT_DESTINATION */
	MQCHAR StderrDestination[MQ_SERVICE_PATH_LENGTH]; /*  MQCA_STDERR_DESTINATION */
	MQLONG Control; /* MQIA_SERVICE_CONTROL */
	MQLONG ServiceType; /* MQIA_SERVICE_TYPE */

}ServiceParms;
#endif

#if defined (zzMQV60)
typedef struct OAMParms {
	MQCHAR QMgrName[MQ_Q_MGR_NAME_LENGTH]; /* MQCA_Q_MGR_NAME */
	MQCHAR AuthProfile[MQ_AUTH_PROFILE_NAME_LENGTH]; /* MQCACF_AUTH_PROFILE_NAME */
	MQCHAR AuthEntity[MQ_ENTITY_NAME_LENGTH]; /* MQCACF_ENTITY_NAME */
	MQLONG EntityType; /* MQIACF_ENTITY_TYPE */
	MQLONG ObjectType; /* MQIACF_OBJECT_TYPE */
	MQLONG AuthOptions; /* MQIACF_AUTH_OPTIONS */
#define ZZAUTHLISTSIZE 100
	MQLONG AuthList[ZZAUTHLISTSIZE]; /* MQIACF_AUTHORIZATION_LIST */
}OAMParms;
#endif

#if defined(zzMQV70)

typedef struct TopicParms {
	MQCHAR TopicName[MQ_TOPIC_NAME_LENGTH]; /* MQCA_TOPIC_NAME (TOPIC) (2092) */
	MQCHAR TopicString[MQ_TOPIC_STR_LENGTH]; /* MQCA_TOPIC_STRING (TOPICSTR) (2094)  */
	MQCHAR TopicDesc[MQ_TOPIC_DESC_LENGTH]; /* MQCA_TOPIC_DESC (DESCR) (2093) */
	MQLONG TopicType; /* MQIA_TOPIC_TYPE (TYPE) (208) */
	MQCHAR AlterationDate[MQ_CREATION_DATE_LENGTH]; /* MQCA_ALTERATION_DATE (ALTDATE) (2027) */
	MQCHAR AlterationTime[MQ_CREATION_TIME_LENGTH]; /* MQCA_ALTERATION_TIME (ALTTIME) (2028) */
	MQCHAR ClusterName[MQ_CLUSTER_NAME_LENGTH]; /* MQCA_CLUSTER_NAME (CLUSTER) (2029) */
	MQLONG DefPriority; /* MQIA_DEF_PRIORITY (DEFPRTY) (6) */
	MQLONG DefPersistence; /* MQIA_TOPIC_DEF_PERSISTENCE (DEFPSIST) (185) */
	MQLONG DefPutResp; /* MQIA_DEF_PUT_RESPONSE_TYPE (DEFPRESP) (184) */
	MQCHAR DurModQName[MQ_Q_NAME_LENGTH]; /* MQCA_MODEL_DURABLE_Q (MDURMDL) (2096) */
	MQLONG DurSubs; /* MQIA_DURABLE_SUB (DURSUB) (175) */
	MQLONG InhibitPubs; /* MQIA_INHIBIT_PUB (PUB) (181) */
	MQLONG InhibitSubs; /* MQIA_INHIBIT_SUB (SUB) (182) */
	MQCHAR NonDurModQName[MQ_Q_NAME_LENGTH]; /* MQCA_MODEL_NON_DURABLE_Q (MNDURMDL) (2097) */
	MQLONG NPerMsgDlvr; /* MQIA_NPM_DELIVERY (NPMSGDLV) (196) */
	MQLONG PerMsgDlvr; /* MQIA_PM_DELIVERY (PMSGDLV) (195) */
	MQLONG ProxySubs; /* MQIA_PROXY_SUB (PROXYSUB) (199)*/
	MQLONG PubScope; /* MQIA_PUB_SCOPE (PUBSCOPE) (219) */
	MQLONG SubScope; /* MQIA_SUB_SCOPE (SUBSCOPE) (218) */
	MQLONG Wildcard; /* MQIA_WILDCARD_OPERATION (WILDCARD) (216) */
#if defined(zzzOS)
	/* zOS stuff */
	MQLONG qsgDisp; /* MQIA_QSG_DISP */
#endif
}TopicParms;

typedef struct SubscrParms {
	MQLONG WildcardSchema; /* MQIACF_WILDCARD_SCHEMA (WSCHEMA) (1288) **/
	MQLONG DestClass; /* MQIACF_DESTINATION_CLASS (DESTCLAS) (1273) **/
	MQLONG DurSub; /* MQIACF_DURABLE_SUBSCRIPTION (DURABLE) (1274) **/
	MQLONG Expiry; /* MQIACF_EXPIRY (EXPIRY) (1244) MQEI_UNLIMITED **/
	MQLONG PubPriorty; /* MQIACF_PUB_PRIORITY (PUBPRTY) (1283) **/
	MQLONG PubSubProps; /* MQIACF_PUBSUB_PROPERTIES (PSPROP) (1271) **/
	MQLONG ReqOnly; /* MQIACF_REQUEST_ONLY (REQONLY) (1280) **/
	MQLONG SubLevel; /* MQIACF_SUB_LEVEL (SUBLEVEL) (1307) **/
	MQLONG SubScope; /* MQIACF_SUBSCRIPTION_SCOPE (SUBSCOPE) (1275) **/
	MQLONG SubType; /* MQIACF_SUB_TYPE (SUBTYPE) (1289) **/
	MQLONG VarUser; /* MQIACF_VARIABLE_USER_ID (VARUSER) (1277) **/
	MQCHAR TopicString[MQ_TOPIC_STR_LENGTH]; /* MQCA_TOPIC_STRING (TOPICSTR) (2094)  **/
	MQCHAR SubUser[MQ_USER_ID_LENGTH]; /* MQCACF_SUB_USER_ID (SUBUSER) (3156) **/
	MQCHAR SubscrName[MQ_SUB_NAME_LENGTH]; /* MQCACF_SUB_NAME () (3152)  **/
	MQCHAR CreationDate[MQ_CREATION_DATE_LENGTH]; /* MQCA_CREATION_DATE (CRDATE) (2004) **/
	MQCHAR CreationTime[MQ_CREATION_TIME_LENGTH]; /* MQCA_CREATION_TIME (CRTIME) (2005) **/
	MQCHAR AlterationDate[MQ_CREATION_DATE_LENGTH]; /* MQCA_ALTERATION_DATE (ALTDATE) (2027) **/
	MQCHAR AlterationTime[MQ_CREATION_TIME_LENGTH]; /* MQCA_ALTERATION_TIME (ALTTIME) (2028) **/
	MQCHAR Destination[MQ_Q_NAME_LENGTH]; /* MQCACF_DESTINATION (DEST) (3154) **/
	MQCHAR DestQmgr[MQ_Q_MGR_NAME_LENGTH]; /* MQCACF_DESTINATION_Q_MGR (DESTQMGR) (3153) **/
	MQCHAR PubApplId[MQ_APPL_IDENTITY_DATA_LENGTH]; /* MQCACF_APPL_IDENTITY_DATA (PUBAPPID) (3135) **/
	MQCHAR SubSelector[MQ_SELECTOR_LENGTH]; /* MQCACF_SUB_SELECTOR (SELECTOR) (3160) **/
	MQLONG SelectorType; /*MQIACF_SELECTOR_TYPE (SELTYPE) (1321) **/
	MQCHAR TopicName[MQ_TOPIC_NAME_LENGTH]; /* MQCA_TOPIC_NAME (TOPIC) (2092) **/
	MQCHAR SubUserData[MQ_USER_DATA_LENGTH]; /* MQCACF_SUB_USER_DATA (USERDATA) (3159) **/
	MQBYTE SubId[MQ_CORREL_ID_LENGTH]; /* MQBACF_SUB_ID (SUBID) (7016) **/
	MQBYTE PubAcctTkn[MQ_ACCOUNTING_TOKEN_LENGTH]; /* MQCACF_ACCOUNTING_TOKEN (PUBACCT) (7010) **/
	MQBYTE DestCorId[MQ_CORREL_ID_LENGTH]; /* MQBACF_DESTINATION_CORREL_ID (DESTCORL) (7015) **/
}SubscrParms;

#endif 

#if defined(zzzOS)
typedef struct StorageClassParms {
	MQCHAR StgClassName[MQ_STORAGE_CLASS_LENGTH]; /* MQCA_STORAGE_CLASS */
	MQCHAR StgClassDesc[MQ_STORAGE_CLASS_DESC_LENGTH]; /* MQCA_STORAGE_CLASS_DESC */
	MQCHAR AlterationDate[MQ_CREATION_DATE_LENGTH]; /* MQCA_ALTERATION_DATE */
	MQCHAR AlterationTime[MQ_CREATION_TIME_LENGTH]; /* MQCA_ALTERATION_TIME */
	MQLONG pageSet; /* MQIA_PAGESET_ID */
	MQCHAR passTicketApplication[MQ_PASS_TICKET_APPL_LENGTH]; /* MQCA_PASS_TICKET_APPL */
	MQLONG qsgDisp; /* MQIA_QSG_DISP */
	MQCHAR xcfGroupName[MQ_XCF_GROUP_NAME_LENGTH]; /* MQCA_XCF_GROUP_NAME */
	MQCHAR xcfMemberName[MQ_XCF_MEMBER_NAME_LENGTH]; /* MQCA_XCF_MEMBER_NAME */
}StorageClassParms;

typedef struct CFStructParms {
	MQCHAR CFName[MQ_CF_STRUC_NAME_LENGTH]; /* MQCA_CF_STRUC_NAME */
	MQCHAR CFDesc[MQ_CF_STRUC_DESC_LENGTH]; /* MQCA_CF_STRUC_DESC */
	MQCHAR AlterationDate[MQ_CREATION_DATE_LENGTH]; /* MQCA_ALTERATION_DATE */
	MQCHAR AlterationTime[MQ_CREATION_TIME_LENGTH]; /* MQCA_ALTERATION_TIME */
	MQLONG cfLevel; /* MQIA_CF_LEVEL */
	MQLONG recovery; /* MQIA_CF_RECOVER */
}CFStructParms;

typedef struct SystemParms {
	MQLONG type; /* MQIACF_SYSP_TYPE */
	MQLONG chkPoint; /* MQIACF_SYSP_CHKPOINT_COUNT */
	MQLONG clCache; /* MQIACF_SYSP_CLUSTER_CACHE */
	MQLONG ccsId; /* MQIA_CODED_CHAR_SET_ID */
	MQLONG db2Blobs; /* MQIACF_SYSP_DB2_BLOB_TASKS */
	MQLONG db2Tasks; /* MQIACF_SYSP_DB2_TASKS */
	MQLONG exitInt; /* MQIACF_SYSP_EXIT_INTERVAL */
	MQLONG exitTasks; /* MQIACF_SYSP_EXIT_TASKS */

	MQLONG maxConns; /* MQIACF_SYSP_MAX_CONNS */
	MQLONG maxBconns; /* MQIACF_SYSP_MAX_CONNS_BACK */
	MQLONG maxFconns; /* MQIACF_SYSP_MAX_CONNS_FORE */
	MQLONG otmaInterval; /* MQIACF_SYSP_OTMA_INTERVAL */
	MQLONG qIndexDefer; /* MQIACF_SYSP_Q_INDEX_DEFER */
	MQLONG resLevel; /* MQIACF_SYSP_RESLEVEL_AUDIT */

	MQLONG rtnCodes [ZZRTNCODES]; /* MQIACF_SYSP_ROUTING_CODE */
	MQLONG smfAccounting; /* MQIACF_SYSP_SMF_ACCOUNTING */
	MQLONG smfStats; /* MQIACF_SYSP_SMF_STATS */
	MQLONG smfInterval; /* MQIACF_SYSP_SMF_INTERVAL */
	MQLONG traceClass[ZZTRCCLASSES]; /* MQIACF_SYSP_TRACE_CLASS */
	MQLONG traceSize; /* MQIACF_SYSP_TRACE_SIZE */
	MQLONG wlmInt; /* MQIACF_SYSP_WLM_INTERVAL */
	MQLONG wlmUnit; /* MQIACF_SYSP_WLM_INT_UNITS */

	MQCHAR otmaTpipe[48]; /* MQCACF_SYSP_OTMA_TPIPE_PFX */
	MQCHAR servicearms[MQ_SYSP_SERVICE_LENGTH]; /* MQIACF_SYSP_SERVICE */
	MQCHAR qsgName[MQ_QSG_NAME_LENGTH]; /* MQCA_QSG_NAME */
	MQCHAR otmaDruExit[MQ_EXIT_NAME_LENGTH]; /* MQCACF_SYSP_OTMA_DRU_EXIT */
	MQCHAR otmaGroup[MQ_XCF_GROUP_NAME_LENGTH]; /* MQCACF_SYSP_OTMA_GROUP */
	MQCHAR otmaMember[MQ_XCF_MEMBER_NAME_LENGTH]; /* MQCACF_SYSP_OTMA_MEMBER */
	MQCHAR db2Name[MQ_DB2_NAME_LENGTH]; /* MQCACF_DB2_NAME */
	MQCHAR cmdUser[MQ_USER_ID_LENGTH]; /* MQCACF_SYSP_CMD_USER_ID */
	MQCHAR dsgName[MQ_DSG_NAME_LENGTH]; /* MQCACF_DSG_NAME */

}SystemParms;

typedef struct UsageParms {
	MQLONG type; /* MQIACF_USAGE_TYPE */
	MQLONG psId; /* MQIA_PAGESET_ID */
	MQLONG buffPool; /*   MQIACF_BUFFER_POOL_ID */
	MQLONG expandType; /*  MQIACF_USAGE_EXPAND_TYPE */
	MQLONG totalBuffers; /*  MQIACF_USAGE_TOTAL_BUFFERS */
}UsageParms;

typedef struct LogParms {
	MQLONG type; /* MQIACF_SYSP_TYPE */
	MQLONG deallocInt; /* MQIACF_SYSP_DEALLOC_INTERVAL */
	MQLONG maxArchive; /* MQIACF_SYSP_MAX_ARCHIVE */
	MQLONG maxRTU; /* MQIACF_SYSP_MAX_READ_TAPES */
	MQLONG inBuffer; /* MQIACF_SYSP_IN_BUFFER_SIZE */
	MQLONG outBuffer; /* MQIACF_SYSP_OUT_BUFFER_SIZE */
	MQLONG wrthrsh; /* MQIACF_SYSP_OUT_BUFFER_COUNT */
	MQLONG offload; /* MQIACF_SYSP_ARCHIVE */
	MQLONG dualActive; /* MQIACF_SYSP_DUAL_ACTIVE */
	MQLONG dualArchive; /* MQIACF_SYSP_DUAL_ARCHIVE */
	MQLONG dualBSDS; /* MQIACF_SYSP_DUAL_BSDS */
	MQLONG logCopy; /* MQIACF_SYSP_LOG_COPY */
	MQCHAR logDSN[44]; /* MQCACF_DATA_SET_NAME */
}LogParms;

typedef struct ArchiveParms {

	MQLONG type; /* MQIACF_SYSP_TYPE */
	MQLONG allocUnit; /* MQIACF_SYSP_ALLOC_UNIT:MQSYSP_ALLOC_BLK, MQSYSP_ALLOC_TRK, MQSYSP_ALLOC_CYL (ALCUNIT) */
	MQLONG archRetain; /* MQIACF_SYSP_ARCHIVE_RETAIN (ARCRETN) */
	MQLONG archWtor; /* MQIACF_SYSP_ARCHIVE_WTOR:MQSYSP_YES (ARCWTOR)*/
	MQLONG blkSize; /* MQIACF_SYSP_BLOCK_SIZE (BLKSIZE)*/
	MQLONG catalog; /* MQIACF_SYSP_CATALOG:MQSYSP_YES (CATALOG)*/
	MQLONG compact; /* MQIACF_SYSP_COMPACT:MQSYSP_YES (COMPACT)*/
	MQLONG primaryAlloc; /* MQIACF_SYSP_ALLOC_PRIMARY  (PRIQTY)*/
	MQLONG protect; /* MQIACF_SYSP_PROTECT:MQSYSP_YES (PROTECT)*/
	MQLONG quiesceInterval; /* MQIACF_SYSP_QUIESCE_INTERVAL (QUIESCE)*/
	MQLONG secondaryAlloc; /* MQIACF_SYSP_ALLOC_SECONDARY  (SECQTY)*/
	MQLONG timestamp; /* MQIACF_SYSP_TIMESTAMP:MQSYSP_YES, MQSYSP_EXTENDED(EXT), MQSYSP_NO (TSTAMP)*/

	MQLONG rtnCodes[ZZRTNCODES]; /* MQIACF_SYSP_ROUTING_CODE(FIL[14]) (ARCWRTC)*/

	MQCHAR archUnit1[MQ_ARCHIVE_UNIT_LENGTH]; /* MQCACF_SYSP_ARCHIVE_UNIT1 (UNIT)*/
	MQCHAR archUnit2[MQ_ARCHIVE_UNIT_LENGTH]; /* MQCACF_SYSP_ARCHIVE_UNIT2 (UNIT2)*/
	MQCHAR archPrefix1[MQ_ARCHIVE_PFX_LENGTH]; /* MQCACF_SYSP_ARCHIVE_PFX1 (ARCPFX1)*/
	MQCHAR archPrefix2[MQ_ARCHIVE_PFX_LENGTH]; /* MQCACF_SYSP_ARCHIVE_PFX2 (ARCPFX2)*/

}ArchiveParms;

#endif

MQLONG InquireDefinition(MQHCONN hConn, MQHOBJ hAdminQ, MQCHAR48 ReplyToQ,
		MQLONG SearchType /* QUEUE|CHANNEL|PROCESS|QMGR */
		, MQLONG SearchParm,CmdArgParmsPtr MyParms );

MQLONG ProcessReplyQ(MQHCONN hConn, MQHOBJ hReplyQ, int *pUserVersion,
		int * pQmgrVersion, FILE ** outputfp /* v51 */
		, char * fileName, FILE ** authOutputfp /* v603 */
		, char * authFileName /* v603 */
		, MQLONG ObjectSets, int SysObjs, int overlay /* v534 overlay flag */
		, int twoLines, MQLONG * pcfReason, CmdArgParmsPtr MyParms);

void ReportNoneFound(MQCFH *pPCFHeader);
int printThisObject(char* objectName, int SysObjs, int SO_OBJECT);

/* ****************** */
/* Defined in QUEUE.C */
/* ****************** */

void ProcessQueueStringParm(MQCFST *pPCFString, QParms *QDefn, MQLONG *iCC);

void ProcessQueueStringListParm(MQCFSL *pPCFString, QParms *QDefn, MQLONG *iCC, CmdArgParmsPtr pMyParms);

void ProcessQueueIntegerParm(MQCFIN *pPCFInteger, QParms *QDefn, MQLONG *iCC);

int AddToFileQUEUE(QParms *QDefn, int QmgrVersion, char *fileName,
		FILE *poutputfp, /* v51 */
		int twolines, /* v601 */
		MQLONG *iCC);

/* ******************** */
/* Defined in CHANNEL.C */
/* ******************** */

void ProcessChannelStringParm(MQCFST *pPCFString, ChlParms *ChlDefn,
		MQLONG *iCC);

void ProcessChannelStringListParm(MQCFSL *pPCFString, ChlParms *ChlDefn,
		MQLONG *iCC);

void ProcessChannelIntegerParm(MQCFIN *pPCFInteger, ChlParms *ChlDefn,
		MQLONG *iCC);

void ProcessChannelIntegerListParm(MQCFIL *pPCFIntegerList, ChlParms *ChlDefn,
		MQLONG *iCC);

void AddToFileCHANNEL(ChlParms *ChlDefn, int QmgrVersion, char * fileName,
		FILE *poutputfp, /* v51 */
		int twolines, MQLONG *iCC);

/* ******************** */
/* Defined in PROCESS.C */
/* ******************** */

void
		ProcessProcStringParm(MQCFST *pPCFString, ProcParms *ProcDefn,
				MQLONG *iCC);

void ProcessProcIntegerParm(MQCFIN *pPCFInteger, ProcParms *ProcDefn,
		MQLONG *iCC);

void AddToFilePROCESS(ProcParms *ProcDefn, int QmgrVersion, char *fileName,
		FILE *poutputfp, /* v51 */
		int twolines, /* v601 */
		MQLONG *iCC);

/* ***************** */
/* Defined in QMGR.C */
/* ***************** */

void
		ProcessQMgrStringParm(MQCFST *pPCFString, QMgrParms *QMgrDefn,
				MQLONG *iCC);

void ProcessQMgrIntegerParm(MQCFIN *pPCFInteger, QMgrParms *QMgrDefn,
		MQLONG *iCC);

void AddToFileQMGR(QMgrParms *QMgrDefn, int QmgrVersion, char *fileName,
		FILE *poutputfp, /* v51 */
		int twolines, /* v601 */
		MQLONG *iCC);

#if defined (MQ_OBJECT_NAME_LENGTH)
/* ********************* */
/* Defined in NAMELIST.C */
/* ********************* */

void ProcessNamelistStringParm( MQCFST *pPCFString
		, NameLParms *NlDefn
		, MQLONG *iCC
);

void ProcessNamelistStringListParm( MQCFSL *pPCFString
		, NameLParms *NlDefn
		, MQLONG *iCC
);

void ProcessNamelistIntegerParm( MQCFIN *pPCFInteger
		, NameLParms *NlDefn
		, MQLONG *iCC
);

void AddToFileNAMELIST( NameLParms *NlDefn,
		int QmgrVersion,
		char *fileName,
		FILE *poutputfp, /* v51 */
		int twolines, /* v601 */
		MQLONG *iCC );

#endif

#if defined (MQ_AUTH_INFO_NAME_LENGTH)
/* ********************* */
/* Defined in authinfo.C */
/* ********************* */

void ProcessAuthInfoStringParm( MQCFST *pPCFString
		, AuthInfoParms *pAuthInfoDefn
		, MQLONG *iCC
);

void ProcessAuthInfoIntegerParm( MQCFIN *pPCFInteger
		, AuthInfoParms *pAuthInfoDefn
		, MQLONG *iCC
);

void AddToFileAUTHINFO(
		AuthInfoParms *pAuthInfoDefn,
		int QmgrVersion,
		char *fileName,
		FILE *poutputfp,
		int twolines,
		MQLONG *iCC );

#endif

#ifdef zzMQV60
/* ********************* */
/* Defined in listener.c */
/* ********************* */

void ProcessListenerStringParm( MQCFST *pPCFString
		, ListenerParms *ListenerDefn
		, MQLONG *iCC
);

void ProcessListenerIntegerParm( MQCFIN *pPCFInteger
		, ListenerParms *ListenerDefn
		, MQLONG *iCC
);

void AddToFileLISTENER( ListenerParms *ListenerDefn,
		int QmgrVersion,
		char *fileName,
		FILE *poutputfp,
		int twolines, /* v601 */
		MQLONG *iCC );

/* ********************* */
/* Defined in services.c */
/* ********************* */

void ProcessServiceStringParm( MQCFST *pPCFString
		, ServiceParms *ServiceDefn
		, MQLONG *iCC
);

void ProcessServiceIntegerParm( MQCFIN *pPCFInteger
		, ServiceParms *ServiceDefn
		, MQLONG *iCC
);

void AddToFileSERVICE( ServiceParms *ServiceDefn,
		int QmgrVersion,
		char *fileName,
		FILE *poutputfp,
		int twolines, /* v601 */
		MQLONG *iCC );

/* ****************** */
/* Defined in oam.C */
/* ****************** */

void ProcessOAMStringParm( MQCFST *pPCFString
		, OAMParms *OAMParms
		, MQLONG *iCC
);

void ProcessOAMIntegerParm( MQCFIN *pPCFInteger
		, OAMParms *OAMParms
		, MQLONG *iCC
);

void ProcessOAMIntegerListParm( MQCFIL *pPCFInteger
		, OAMParms *OAMParms
		, MQLONG *iCC
);

void AddToFileOAM(OAMParms *OAMParms,
		int QmgrVersion,
		char *fileName,
		FILE *poutputfp, /* v51 */
		int twolines, /* v601 */
		MQLONG *iCC );

#endif
#ifdef zzMQV70

/* ****************** */
/* Defined in topic.c */
/* ****************** */

void ProcessTopicStringParm( MQCFST *pPCFString
		, TopicParms *TopicParms
		, MQLONG *iCC
);

void ProcessTopicIntegerParm( MQCFIN *pPCFInteger
		, TopicParms *TopicParms
		, MQLONG *iCC
);

void ProcessTopicIntegerListParm( MQCFIL *pPCFInteger
		, TopicParms *TopicParms
		, MQLONG *iCC
);

void AddToFileTopic(TopicParms *TopicParms,
		int QmgrVersion,
		char *fileName,
		FILE *poutputfp, /* v51 */
		int twolines, /* v601 */
		MQLONG *iCC );

/* ****************** */
/* Defined in subscript.c */
/* ****************** */

void ProcessSubscrStringParm( MQCFST *pPCFString
		, SubscrParms *SubscrParms
		, MQLONG *iCC
);

void ProcessSubscrIntegerParm( MQCFIN *pPCFInteger
		, SubscrParms *SubscrParms
		, MQLONG *iCC
);

void ProcessSubscrByteParm(MQCFBS *pPCFByte
		, SubscrParms *SubscrParms
		, MQLONG *iCC);

void AddToFileSubscr(SubscrParms *SubscrParms,
		int QmgrVersion,
		char *fileName,
		FILE *poutputfp, /* v51 */
		int twolines, /* v601 */
		MQLONG *iCC );

#endif 

#if defined(zzzOS)
/* ****************** */
/* Defined in STGCLASS.C */
/* ****************** */

void ProcessStgClassStringParm( MQCFST *pPCFString
		, StorageClassParms *StgClass
		, MQLONG *iCC
);

void ProcessStgClassIntegerParm( MQCFIN *pPCFInteger
		, StorageClassParms *StgClassParms
		, MQLONG *iCC
);

void AddToFileStgClass(StorageClassParms *StgClassParms,
		int QmgrVersion,
		char *fileName,
		FILE *poutputfp, /* v51 */
		int twolines, /* v601 */
		MQLONG *iCC );

/* ****************** */
/* Defined in SYSTEM.C */
/* ****************** */

void ProcessSystemStringParm( MQCFST *pPCFString
		, SystemParms *SystemClass
		, MQLONG *iCC
);

void ProcessSystemIntegerParm( MQCFIN *pPCFInteger
		, SystemParms *SsystemParms
		, MQLONG *iCC
);

void ProcessSystemIntegerListParm( MQCFIL *pPCFInteger
		, SystemParms *SsystemParms
		, MQLONG *iCC
);

void AddToFileSystem(SystemParms *SystemParms,
		int QmgrVersion,
		char *fileName,
		FILE *poutputfp, /* v51 */
		int twolines, /* v601 */
		MQLONG *iCC );

/* ****************** */
/* Defined in usage.C */
/* ****************** */

void ProcessUsageStringParm( MQCFST *pPCFString
		, UsageParms *UsageClass
		, MQLONG *iCC
);

void ProcessUsageIntegerParm( MQCFIN *pPCFInteger
		, UsageParms *SsystemParms
		, MQLONG *iCC
);

void AddToFileUsage(UsageParms *UsageParms,
		int QmgrVersion,
		char *fileName,
		FILE *poutputfp, /* v51 */
		int twolines, /* v601 */
		MQLONG *iCC );

/* ******************    */
/* Defined in CFStruct.C */
/* ******************    */

void ProcessCFStructStringParm( MQCFST *pPCFString
		, CFStructParms *CFStructClass
		, MQLONG *iCC
);

void ProcessCFStructIntegerParm( MQCFIN *pPCFInteger
		, CFStructParms *SsystemParms
		, MQLONG *iCC
);

void AddToFileCFStruct(CFStructParms *CFStructParms,
		int QmgrVersion,
		char *fileName,
		FILE *poutputfp, /* v51 */
		int twolines, /* v601 */
		MQLONG *iCC );

/* ******************    */
/* Defined in Log.C */
/* ******************    */

void ProcessLogStringParm( MQCFST *pPCFString
		, LogParms *LogParms
		, MQLONG *iCC
);

void ProcessLogIntegerParm( MQCFIN *pPCFInteger
		, LogParms *LogParms
		, MQLONG *iCC
);

void AddToFileLog(LogParms *LogParms,
		int QmgrVersion,
		char *fileName,
		FILE *poutputfp, /* v51 */
		int twolines, /* v601 */
		MQLONG *iCC );

/* ******************    */
/* Defined in Archive.C */
/* ******************    */

void ProcessArchiveStringParm( MQCFST *pPCFString
		, ArchiveParms *ArchiveClass
		, MQLONG *iCC
);

void ProcessArchiveIntegerParm( MQCFIN *pPCFInteger
		, ArchiveParms *ArchiveParms
		, MQLONG *iCC
);

void ProcessArchiveIntegerListParm( MQCFIL *pPCFIntegerList
		, ArchiveParms *ArchiveParms
		, MQLONG *iCC
);

void AddToFileArchive(ArchiveParms *ArchiveParms,
		int QmgrVersion,
		char *fileName,
		FILE *poutputfp, /* v51 */
		int twolines, /* v601 */
		MQLONG *iCC );

#endif
#endif /*___SAVEQMGRH*/
