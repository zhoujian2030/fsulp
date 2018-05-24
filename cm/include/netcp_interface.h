
#ifndef __NETCP_DRV_H
#define __NETCP_DRV_H

typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned short uint16_t;
typedef short int16_t;
typedef unsigned int uint32_t;
typedef int int32_t;
typedef unsigned long long uint64_t;

typedef uint32_t   Qmss_QueueHnd;

typedef enum
{
	OAM_SEND_L1_CFG_REQ=0,
	OAM_SEND_L2_CFG_REQ,	//to mac
	OAM_SEND_CMAC_CFG_REQ,	//to pdcp
	OAM_RECS_L1_CFG_RSP,
	OAM_RECS_L1_LOG,
	OAM_RECS_L2_CFG_RSP,
	OAM_RECS_L2_LOG,
	OAM_RECS_CMAC_CFG_RSP,
	OAM_RECS_CMAC_LOG,

	L3_SEND_L2_CFG_REQUEST,
	L3_SEND_CMAC_CFG_REQUEST,	//to rlc
	L3_RECS_L2_CFG_RSP,
	L3_RECS_CMAC_CFG_RSP,

	L2_SEND_L1_CONFIG,
	L2_SEND_L1_DATA,
	L2_SEND_L3_DATA_OR_CONFIG,
	L2_SEND_CMAC_BUF_REP,
	L2_SEND_CMAC_UL_MAC_CE,
	L2_SEND_OAM_DATA_OR_REPLY,
	L2_RECS_L1_DATA,
	L2_RECS_L1_ERRINDICATION,
	L2_RECS_OAM_CONFIG,
	L2_RECS_L3_DATA_OR_CONFIG,
	L2_RECS_L3_CMAC_HARQ_ACK,
	L2_RECS_L3_CMAC_SCH_RESULT,

	CMAC_SEND_L1_DCI,
	CMAC_SEND_L1_CFG,
	CMAC_SEND_L2_SCH_RESULT,
	CMAC_SEND_L2_HARQ_ACK,
	CMAC_SEND_L3_CFG_RSP,
	CMAC_SEND_OAM_CFG_RSP,
	CMAC_RECS_L1_UL_CQI,
	CMAC_RECS_L1_CFG_RSP,
	CMAC_RECS_L2_BUF_REP,
	CMAC_RECS_L2_UL_MAC_CE,
	CMAC_RECS_OAM_CFG_REQ,
	CMAC_RECS_L3_CFG_REQ,

	L1_SEND_OAM_DATAUP_OR_REPLY,
	L1_SEND_CMAC_CQIUP,
	L1_SEND_CMAC_REPLY,
	L1_SEND_L2_DATAUP,
	L1_SEND_L2_ERRINDICATION,
	LOG_SEND_L1_LOG,
	LOG_SEND_L2_LOG,
	LOG_SEND_CMAC_LOG,
	L1_RECS_OAM_CONFIG,
	L1_RECS_L2_DATA,
	L1_RECS_L2_CONFIG,
	L1_RECS_CMAC_CONFIG,
	L1_RECS_CMAC_DCI,
}Qmss_HANDLE;

typedef enum
{
    	QMSS_DESC_REGION_START_INDEX = 0,
    	AIF2_DESC_REGION_START_INDEX = 4,
    	FFTC_DESC_REGION_START_INDEX = 5,
      	PA_DESC_REGION_START_INDEX = 6,
     	BCP_DESC_REGION_START_INDEX = 7,
}Qmss_DescStartRegion;

/*描述符所属核的区分*/
typedef enum
{
	DESC_MEM_L1,
	DESC_MEM_L2,
	DESC_MEM_CMAC,
	DESC_MEM_ARM
}Qmss_DescMemType;
/*QMSS 模块类型枚举*/
typedef enum
{
    TxQue_Type_L1=0,
    TxQue_Type_L2,
    TxQue_Type_L3,
    TxQue_Type_CMAC,
    TxQue_Type_OAM
}Qmss_TxQueType;
/*QMSS LOG模块类型枚举*/
typedef enum
{
    TxQue_Type_L1_LOG=0,
    TxQue_Type_L2_LOG,
    TxQue_Type_CMAC_LOG
}Qmss_TxQueLogType;
///////////////////////////////////////////L1对列类型///////////////////////////////////////////
/*QMSS L1发送空闲队列类型枚举*/
typedef enum
{
	TX_L1_TO_L2_DATAUP,
	TX_L1_TO_L2_ERRINDICATION,
	TX_L1_TO_CMAC_CQIUP,
	TX_L1_TO_CMAC_REPLY,
	TX_L1_TO_OAM_DATAUP_OR_REPLY
}Qmss_L1_TranType;
/*QMSS L1接收队列类型枚举*/
typedef enum
{
     RX_L1_FROM_L2_CONFIG,
     RX_L1_FROM_L2_DATA,
     RX_L1_FROM_CMAC_DCI,
     RX_L1_FROM_CMAC_CONFIG,
     RX_L1_FROM_OAM_CONFIG
}Qmss_L1_RecvType;
///////////////////////////////////////////L2对列类型///////////////////////////////////////////
/*QMSS L2发送空闲队列类型枚举*/
typedef enum
{
	 TX_L2_TO_L1_CONFIG,
	 TX_L2_TO_L1_DATA,
	 TX_L2_TO_L3_DATA_OR_CONFIG,
	 TX_L2_TO_CMAC_BUF_REP,  /* L2给CMAC的缓存报告队列 */
	 TX_L2_TO_CMAC_UL_MAC_CE,/* DMAC给CMAC的上行MAC控制元素队列 */
	 TX_L2_TO_OAM_DATA_OR_REPLY
}Qmss_L2_TranType;
/*QMSS L2接收队列类型枚举*/
typedef enum
{
	 RX_L2_FROM_L1_DATA,
	 RX_L2_FROM_L1_ERRINDICATION,
	 RX_L2_FROM_L3_DATA_OR_CONFIG,
	 RX_L2_FROM_CMAC_SCH_RESULT, /* CMAC给L2的调度结果，包括公共信道，UE级的RLC及DMAC调度结果以及DL-CFG参数 */
	 RX_L2_FROM_CMAC_HARQ_ACK,   /* CMAC给L2的HARQ结果，指示DMAC删除HARQ BUFFER，以及重传调度结果和DL-DCI及DL-CFG参数 */
	 RX_L2_FROM_OAM_CONFIG
}Qmss_L2_RecvType;

/////////////////////////////////////////////L3对列类型/////////////////////////////////////////
/*QMSS L3发送空闲队列类型枚举*/
typedef enum
{
	 TX_L3_TO_L2_CFG_REQ,
	 TX_L3_TO_CMAC_CFG_REQ,

}Qmss_L3_TranType;
/*QMSS L3接收队列类型枚举*/
typedef enum
{
	 RX_L3_FROM_L2_CFG_RSP,
	 RX_L3_FROM_CMAC_CFG_RSP,
}Qmss_L3_RecvType;
/////////////////////////////////////////////CMAC对列类型////////////////////////////////////////
/*QMSS CMAC发送空闲队列类型枚举*/
typedef enum
{
	 TX_CMAC_TO_L1_DCI,       /* 下行发送上行配置消息以及DCI0 */
	 TX_CMAC_TO_L1_CFG,       /* 下行发送给L1的小区及UE级的配置消息 */
	 TX_CMAC_TO_L2_SCH_RESULT,/* CMAC给L2的调度结果，包括公共信道，UE级的RLC及DMAC调度结果以及DL-CFG参数 */
	 TX_CMAC_TO_L2_HARQ_ACK,  /* CMAC给L2的HARQ结果，指示DMAC删除HARQ BUFFER，以及重传调度结果和DL-DCI及DL-CFG参数 */
	 TX_CMAC_TO_L3_CFG_RSP,   /* CMAC给L3的配置响应队列 */
	 TX_CMAC_TO_OAM_CFG_RSP   /* CMAC组OAM的配置响应队列 */
}Qmss_CMAC_TranType;

/*QMSS CMAC接收队列类型枚举*/
typedef enum
{
	 RX_CMAC_FROM_L1_UL_CQI,     /* 上行CQI接收队列 */
	 RX_CMAC_FROM_L1_CFG_RSP,    /* L1给CMAC的配置响应及错误指示 */
	 RX_CMAC_FROM_L2_BUF_REP,   /*  L2给CMAC的缓存报告队列 */
	 RX_CMAC_FROM_L2_UL_MAC_CE, /* DMAC给CMAC的上行MAC控制元素队列 */
	 RX_CMAC_FROM_L3_CFG_REQ,   /* L3给CMAC的配置请求队列 */
	 RX_CMAC_FROM_OAM_CFG_REQ,  /* OAM给CMAC的配置请求队列 */
}Qmss_CMAC_RecvType;

/////////////////////////////////////////////////OAM对列类型///////////////////////////////////////////
/*QMSS OAM发送空闲队列类型枚举*/
typedef enum
{
	 TX_OAM_TO_L1_CFG_REQ,
	 TX_OAM_TO_L2_CFG_REQ,
	 TX_OAM_TO_CMAC_CFG_REQ
}Qmss_OAM_TranType;
/*QMSS OAM接收队列类型枚举*/
typedef enum
{
	 RX_OAM_FROM_L1_CFG_RSP,
	 RX_OAM_FROM_L2_CFG_RSP,
	 RX_OAM_FROM_CMAC_CFG_RSP,
	 RX_OAM_FROM_L1_LOG,
	 RX_OAM_FROM_L2_LOG,
	 RX_OAM_FROM_CMAC_LOG
}Qmss_OAM_RecvType;


/**************NETCP******************/

/**************PA******************/
#define TRUE		  1
#define FAULSE       0
#define PA_L2  2
#define PA_L3  3

#define PA_CFG_FLOW         25
#define PA_DATA_FLOW       26

/* PA规则中协议相关的取值 */
#define pa_IPV4      4

/******************* paRouteInfo_t dest 成员的取值 ******************************/
#define  pa_DEST_DISCARD  3  /**< Packet is discarded */

#define  pa_DEST_CONTINUE_PARSE_LUT1  4 /**< Packet remains in PA sub-system for more parsing and LUT1 classification */

#define  pa_DEST_CONTINUE_PARSE_LUT2  5  /**< Packet remains in PA sub-system for more parsing and LUT2 classification */

#define  pa_DEST_HOST   6   /**< Packet is routed to host */

#define  pa_DEST_EMAC   7   /**< Packet is routed to  EMAC */

#define  pa_DEST_SASS   8   /**< Packet is routed to SA */

#define  pa_DEST_SRIO   9   /**< Packet is routed to SRIO */

/******************* paRouteInfo_t queue 成员的取值 ******************************/
#define QUEUE_PASS_TO_SA1         646
#define QUEUE_PASS_TO_SA2         647
#define QUEUE_PASS_TO_GBE         648


/******************* paRouteInfo_t customtype 成员的取值 ******************************/

#define  pa_CUSTOM_TYPE_NONE   0                  /* 自动指定 */

#define  pa_CUSTOM_TYPE_LUT1   1                  /* 路由到LUT1 */

#define  pa_CUSTOM_TYPE_LUT2   2                  /* 路由到LUT2 */

#define  pa_EMAC_PORT_NOT_SPECIFIED   0           /* 自动指定 */

#define  pa_EMAC_PORT_0               1           /* 路由到EMAC 0 */

#define  pa_EMAC_PORT_1               2           /* 路由到EMAC1 */

/******************* paRouteInfo_t pktType_emacCtrl 成员的取值 ******************************/

#define pa_EMAC_CTRL_PORT_MASK            0x0F 
/**
 *  @def  pa_EMAC_CTRL_CRC_DISABLE
 *        Control Info -- 0:EMAC port computes and inserts CRC
 *                        1:EMAC port does not generate CRC  
 */
#define pa_EMAC_CTRL_CRC_DISABLE          0x80 


/**************ETHINFO  结构体信息 ***********************/

#define pa_MAC_ADDR_SIZE       6 
typedef unsigned char paMacAddr_t[pa_MAC_ADDR_SIZE];

typedef struct  {
  paMacAddr_t        src;           /**< Source MAC addresss  */
  paMacAddr_t        dst;           /**< Destination MAC address */
  uint16_t           vlan;          /**< VLAN tag VID field, 12 lsbs  */
  uint16_t           ethertype;     /**< Ethertype field. */
  uint32_t           mplsTag;       /**< MPLS tag. Only the outer tag is examined */
  uint16_t           inport;        /**< Input EMAC port number as specified by @ref paEmacPort */
} paEthInfo_t;


/**************IPINFO  结构体信息 *************************/

#define pa_IPV6_ADDR_SIZE      16 
typedef unsigned char paIpv6Addr_t[pa_IPV6_ADDR_SIZE];

#define pa_IPV4_ADDR_SIZE      4
typedef unsigned char paIpv4Addr_t[pa_IPV4_ADDR_SIZE];

typedef union  {

  paIpv6Addr_t  ipv6;   /**< IPv6 address */
  paIpv4Addr_t  ipv4;   /**< IPv4 address */
  
} paIpAddr_t;

typedef struct  {

  paIpAddr_t  src;       /**<  Source IP address */
  paIpAddr_t  dst;       /**<  Destination IP address */
  uint32_t    spi;       /**<  ESP or AH header Security Parameters Index */
  uint32_t    flow;      /**<  IPv6 flow label in 20 lsbs */
  int         ipType;    /**<  @ref IpValues */
  uint16_t    greProto;  /**<  GRE protocol field */
  uint8_t     proto;     /**<  IP Protocol (IPv4) / Next Header (IPv6) */
  uint8_t     tos;       /**<  IP Type of Service (IPv4) / Traffic class (IPv6) */
  uint16_t    tosCare;   /**<  TRUE if the tos value is used for matching */
  uint16_t    sctpPort;  /**<  SCTP Destination Port */
} paIpInfo_t;



/******************ROUTEINFO  结构体信息*****************************/

typedef struct {

  uint16_t    ctrlBitfield;    /**< Routing control information as defined at @ref routeCtrlInfo */
  int         dest;            /**< Packet destination as defined at @ref pktDest */
  uint8_t     pktType_emacCtrl;/**<  For destination SRIO, specify the 5-bit packet type toward SRIO 
                                     For destination HOST, EMAC, specify the EMAC control @ref emcOutputCtrlBits to the network */
  uint8_t     flowId;          /**< For host, SA or SRIO destinations, specifies return free descriptor setup */
  uint16_t    queue;           /**< For host, SA or SRIO destinations, specifies the dest queue */
  uint32_t    swInfo0;         /**< Placed in SwInfo0 for packets to host or SA; Placed in the PS Info for packets to SRIO*/
  uint32_t    swInfo1;         /**< Placed in SwInfo1 for packets to the SA; Placed in the PS Info for packets to SRIO */
  uint16_t    multiRouteIndex; /**< Multi-route index. It is valid in the from-network direction only */
} paCmdNextRoute_t;

typedef struct {

  uint16_t startOffset;   /**<  Byte location, from SOP, where the checksum calculation begins */
  uint16_t lengthBytes;   /**<  Number of bytes covered by the checksum. Must be even */
  uint16_t resultOffset;  /**<  Byte offset, from startOffset, to place the resulting checksum */
  uint16_t initialSum;    /**<  Initial value of the checksum */
  uint16_t negative0;     /**<  If TRUE, a computed value of 0 is written as -0 */

} paTxChksum_t;

typedef struct {

  uint16_t    ctrlBitfield;    /**< CRC operation control information as defined at @ref crcOpCtrlInfo */
  uint16_t    startOffset;     /**< Byte location, from SOP/Protocol Header, where the CRC computation begins 
                                    if frame type is not specified
                                    Byte location, from SOP/Protocol header, where the specific frame header begins
                                    if frame type is specified
                                    In to-network direction: offset from SOP
                                    In from-network direction: offset from the current parsed header 
                                    */
  uint16_t    len;             /**< Number of bytes covered by the CRC computation 
                                    valid only if pa_CRC_OP_PAYLOAD_LENGTH_IN_HEADER is clear */
  uint16_t    lenOffset;       /**< Payload length field offset in the custom header */
  uint16_t    lenMask;         /**< Payload length field mask */
  uint16_t    lenAdjust;       /**< Payload length adjustment: valid only if pa_CRC_OP_PAYLOAD_LENGTH_IN_HEADER is set */
  uint16_t    crcOffset;       /**< Offset from SOP/Protocol Header to the CRC field 
                                    In to-network direction: offset from SOP
                                    In from-network direction: offset from the current parsed header */
  uint16_t    frameType;       /**< Frame type @ref crcFrameTypes, vaild if pa_CRC_OP_CRC_FRAME_TYPE is set */                                      
} paCmdCrcOp_t;

typedef struct {

  uint16_t    ctrlBitfield;    /**< Copy operation control information as defined at @ref copyCtrlInfo */
  uint16_t    srcOffset;       /**< Offset from the start of current protocol header for the data copy to begin */
  uint16_t    destOffset;      /**< Offset from the top of the PSInfo for the data to be copied to */
  uint16_t    numBytes;        /**< Number of bytes to be copied */   
} paCmdCopy_t;

typedef struct {

  uint16_t   ctrlBitfield;      /**<  Patch operation control information as defined at @ref patchCtrlInfo */
  uint16_t   nPatchBytes;       /**<  The number of bytes to be patched */
  uint16_t   totalPatchSize;    /**<  The number of patch bytes in the patch command, must be >= to nPatchBytes and a multiple of 4 bytes */
  uint16_t   offset;            /**<  Offset from the start of the packet for the patch to begin in the to-network direction 
                                      Offset from the start of the current header for the patch to begin in the from-network direction */
  uint8_t    *patchData;        /**<  Pointer to the patch data */

} paPatchInfo_t;

typedef struct  {
    uint16_t  offset;    /**< The offset to where the SA packet parsing starts */
    uint16_t  len;       /**< The total length of the protocal payload to be processed by SA */
    uint32_t  supData;   /**< Optional supplement data such as the 32-bit CountC for some 3GPP operation modes */
} paPayloadInfo_t;

typedef struct {

  uint16_t    index;        /**< Command Set Index */
} paCmdSet_t;

typedef struct {

  uint16_t    index;        /**<  Multi-route set Index */
} paCmdMultiRoute_t;

typedef struct  {
    uint16_t  destQueue; /**< Host queue for the tx timestamp reporting packet */
    uint16_t  flowId;    /**< CPPI flow which instructs how link-buffer queues are used for sending tx timestamp reporting packets. */
    uint32_t  swInfo0;   /**< 32 bit value returned in the descriptor as swInfo0 which can be used as event identifier */
} paCmdTxTimestamp_t;

typedef struct  {
    uint16_t  ipOffset; /**< Offset to the IP header. */
    uint16_t  mtuSize;  /**< Size of the maximum transmission unit (>= 68) */
} paCmdIpFrag_t;

typedef struct {
  uint16_t    index;        /**< User-defined statistics index */
} paCmdUsrStats_t;

typedef struct {
  uint16_t    setIndex;          /**< Commad Set Index */
  uint16_t    statsIndex;        /**< User-defined statistics index */
} paCmdSetUsrStats_t;

typedef struct {

  uint8_t    msgLenSize;    /**<  Size of message length field in bytes (@note only 2-byte message length is supported) */
  uint8_t    offset;        /**<  Offset from the start of the packet to the message length field */ 
  uint16_t   msgLen;        /**<  Message length excluding the IP header and payload length */

} paPatchMsgLenInfo_t;

typedef struct {

  uint16_t errorBitfield;   /**<  Packet Error information as defined at @ref pktErrInfo */
  uint8_t  dest;            /**<  Packet destination as defined at @ref pktDest */
  uint8_t  flowId;          /**<  For host destination, specifies CPPI flow which defines free queues are used for receiving packets */
  uint16_t queue;           /**<  For host destination, specifies the destination queue */
  uint32_t swInfo0;         /**<  Placed in SwInfo0 for packets to host */
} paCmdVerifyPktErr_t;

typedef struct {
  uint16_t    cmd;         /**< Specify the PA command code as defined at @ref paCmdCode */
  union {
    paCmdNextRoute_t   route;    /**< Specify nextRoute command specific parameters */
    paTxChksum_t       chksum;   /**< Specify Tx Checksum command specific parameters */
    paCmdCrcOp_t       crcOp;    /**< Specify CRC operation command specific parameters */
    paCmdCopy_t        copy;     /**< Specify Copy command specific parameters */
    paPatchInfo_t      patch;    /**< Specify Patch command specific parameters */
    paPayloadInfo_t    payload;  /**< Specify the payload information required by SA */
    paCmdSet_t         cmdSet;   /**< Specify Command Set command specific parameters */
    paCmdMultiRoute_t  mRoute;   /**< Specify Multi-route command specific parameters */
    paCmdTxTimestamp_t txTs;     /**< Specify Report Tx Timestamp command specific parameters */
    paCmdIpFrag_t      ipFrag;   /**< Specify IP fragmentation command specific parameters */
    paCmdUsrStats_t    usrStats; /**< Specify User-defined Statistics command specific parameters */
    paCmdSetUsrStats_t cmdSetUsrStats; /**< Specify Command Set and User-defined Statistics command specific parameters */
    paPatchMsgLenInfo_t patchMsgLen;   /**< Specify Patch Message Length command specific parameters */
    paCmdVerifyPktErr_t verifyPktErr;  /**< Specify Packet error Verification command specific parameters */
  }params;                      /**< Contain the command specific parameters */

} paCmdInfo_t;

typedef struct  {

  int      dest;                  /**<  Packet destination as defined at @ref pktDest */
  uint8_t  flowId;                /**<  For host, SA or SRIO destinations, specifies CPPI flow which defines free queues are used for receiving packets */
  uint16_t queue;                 /**<  For host, SA or SRIO destinations, specifies the destination queue */
  int      mRouteIndex;           /**<  For host, Multi-queue routing index (0 to (@ref pa_MAX_MULTI_ROUTE_SETS - 1)) 
                                        or @ref pa_NO_MULTI_ROUTE if multi routing not used */
  uint32_t swInfo0;               /**<  Placed in SwInfo0 for packets to host or SA; Placed in the PS Info for packets to SRIO */
  uint32_t swInfo1;               /**<  Placed in SwInfo1 for packets to the SA; Placed in the PS Info for packets to SRIO */
  int      customType;            /**<  For CONTINUE_PARSE_LUT1/LUT2 only, specifies the custom type as defined at @ref customType */
  uint8_t  customIndex;           /**<  For CONTINUE_PARSE_LUT1/LUT2 only, specifies the custom classification entry index */                                
  uint8_t  pktType_emacCtrl;      /**<  For destination SRIO, specify the 5-bit packet type toward SRIO 
                                        For destination HOST, EMAC, specify the EMAC control @ref emcOutputCtrlBits to the network */
  paCmdInfo_t *pCmd;              /**<  Pointer to the Command info to be executed prior to the packet forwarding. 
                                        NULL: no commads 
                                        @note only the following commands are supported within paRouteInfo_t 
                                              - pa_CMD_PATCH_DATA (up to two bytes only) (LUT2 only)
                                              - pa_CMD_CMDSET
                                              - pa_CMD_USR_STATS
                                              - pa_CMD_CMDSET_AND_USR_STATS 
                                   */                                                                                    

} paRouteInfo_t;

typedef struct {
    /** Descriptor type, packet type, protocol specific region location, packet length */
    uint32_t          descInfo;  
    /** Source tag, Destination tag */
    uint32_t          tagInfo;
    /** EPIB present, PS valid word count, error flags, PS flags, return policy, return push policy, 
     * packet return QM number, packet return queue number */
    uint32_t          packetInfo;
    /** Number of valid data bytes in the buffer */
    uint32_t          buffLen;
    /** Byte aligned memory address of the buffer associated with this descriptor */
    uint32_t          buffPtr;
    /** 32-bit word aligned memory address of the next buffer descriptor */
    uint32_t          nextBDPtr;       
    /** Completion tag, original buffer size */
    uint32_t          origBufferLen;
    /** Original buffer pointer */
    uint32_t          origBuffPtr;
    /** Optional EPIB word0 */
    uint32_t          timeStamp;
    /** Optional EPIB word1 */
    uint32_t          softwareInfo0;
    /** Optional EPIB word2 */
    uint32_t          softwareInfo1;
    /** Optional EPIB word3 */
    uint32_t          softwareInfo2;
    /** Optional protocol specific data */
    uint32_t          psData; 
}Cppi_HostDesc;

/***************************************** 网络侧需要获取的队列信息，包括队列号和该队列的使用情况***************************************/
typedef struct 
{
	/* 队列号*/
	uint32_t queue;
	/* 该队列的使用情况，使用中或者空闲，空闲的队列即可被网络侧调用,flag = 0 表示该队列可用，flag = 1表示该队列在使用中*/	
	int flag;
}QueTable_t;

typedef struct
{
    /** Queue manager number */
    int32_t qMgr;        
    /** Queue number within Queue Manager */
    int32_t qNum;
}Qmss_Queue;

typedef struct
{
    uint32_t		chanIndex;	//通道号
    uint32_t  		swInfo[3];	//EBIP
    uint32_t 		rxSpiValue;	//Rx SPI值
}Sa_ChanRxSwInfo_t;

/**************************************************************/
/*                     GBE 模块                               					   */
/*************************************************************/
typedef enum 
{
    ALE_ENTRYTYPE_FREE = 0,
    ALE_ENTRYTYPE_ADDRESS,
    ALE_ENTRYTYPE_VLAN,
    ALE_ENTRYTYPE_VLANADDRESS,
    ALE_ENTRYTYPE_ALL
} ale_entry_type_t;

typedef enum 
{
	MODE_THIN_AP = 0,
	MODE_WIRELESS_AP,
}operation_mode_t;

typedef struct 
{
	unsigned short vlanid_wan_and_wlan;
	unsigned short vlanid_wan_and_host;
	unsigned short vlanid_debug;
	unsigned short vlanid_port0;
	unsigned short vlanid_port1;
	unsigned short vlanid_port2;
}vlan_param_t;

typedef enum
{
	CMD_ALE_NO_BYPASS = 0,
	CMD_ALE_BYPASS = 1,
}cmd_ale_control_t;

typedef enum
{
	CMD_ALE_CTRL_DISABLE = 0,
	CMD_ALE_CTRL_ENABLE = 1,
}cmd_ale_enable_t;

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
  *                                         NETCP模块接口                                               *
  ********************************************************************/
extern int Init_Netcp(void);
extern const char *  Netcp_GetVersion(void);
extern int getGwIp(char *gwIp);
extern int getGwMac(char *gwMac);
extern int postNegoipToKernel(unsigned char * negoip);
/**
 *  @描述: 设置带外IP，建立SCTP时所设置的IP将不被带入option域
     @param[in]       privateIp       需要设置的带外IP，如"192.168.197.241"

     @返回值: 0表示成功,负数表示失败
 **/
extern int setSctpPrivateIp(unsigned char *privateIp);

/*********************************************************************
  *                                          QMSS模块接口                                               *
  ********************************************************************/
extern Qmss_Queue Qmss_getQueueNumber(Qmss_QueueHnd hnd);
extern int Qmss_RecsData(Qmss_HANDLE hnd,void *addr);
extern int Qmss_SendData(Qmss_HANDLE hnd,void *buffer,unsigned int packetLen);
extern int Qmss_GetQueueEntryCount(Qmss_HANDLE hnd);
extern int Qmss_SetEnable(char enable);

/*********************************************************************
  *                                          PA模块接口                                                    *
  ********************************************************************/
  extern int Pa_ClrPaStats(void);

/*********************************************************************
  *                                          SA模块接口                                                   *
  ********************************************************************/
extern int Sa_Init(void);
extern int Sa_DownloadFw(void);
extern int Sa_CreateChan_Dm_Hmac256(int16_t chnum, uint8_t *key, uint16_t key_len);
extern int Sa_GetKey_Hmac256(int16_t chnum, uint8_t *buf, uint32_t buf_len, uint8_t *buf_out);
extern int Sa_CloseChan(uint16_t chnum);

/**************************************************************/
/*                     GBE 模块                               					   */
/*************************************************************/
extern int GbE_Switch_ALE_InitBasic(void);
extern int GbE_Switch_ALE_SetMode(operation_mode_t mode, vlan_param_t vlan_param);
extern int GbE_Switch_ALE_GetEntryInfo(ale_entry_type_t entry_type, unsigned short from, unsigned short to);
extern int GbE_Switch_Set_ALE_Bypass(cmd_ale_control_t cmd);
extern int GbE_Switch_Set_ALE_Enable(cmd_ale_enable_t cmd);


/*****************************************************************
  *                                    用于调试的接口                                      *
  *****************************************************************/
extern void* 		Qmss_queuePop(Qmss_QueueHnd hnd);
extern inline void 	Qmss_queuePushDescSize(Qmss_QueueHnd hnd,void *descAddr,unsigned int descSize);
extern int  		Qmss_getQueueEntryCount_Private(Qmss_QueueHnd hnd);

extern void 		getDescNumOfQue(void);

extern uint32_t GbE_Switch_ALE_GetIDVER(void);
extern int32_t GbE_Switch_ALE_GetFreeEntryCount(void);
extern int32_t GbE_Switch_ALE_GetEntryType(uint32_t index);
extern int32_t GbE_Switch_ALE_GetUnicastType(uint32_t index);
extern int32_t GbE_Switch_ALE_GetEntry(uint32_t index,uint32_t *p_tblw0,uint32_t *p_tblw1,uint32_t *p_tblw2);
extern int32_t GbE_Switch_ALE_ClearEntryTable(uint32_t index);
extern int32_t GbE_Switch_ALE_ClearAllTables(void);
extern int32_t GbE_Switch_ALE_SetAgeOutNow(void);

#ifdef __cplusplus
};
#endif

#endif /* End of #ifndef __NETCP_DRV_H */

