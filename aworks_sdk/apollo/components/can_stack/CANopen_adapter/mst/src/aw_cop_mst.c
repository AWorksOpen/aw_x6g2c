/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief AWBus CANopen Master type implementation (lite)
 *
 * \internal
 * \par modification history:
 * - 1.02 19-12-02  anu, modify multi inst
 * - 1.02 15-12-02  cae, first implementation
 * \endinternal
 */

#include "apollo.h"
#include "aw_delay.h"
#include "aw_sem.h"
#include "aw_cop_mst.h"
#include "mst/inc/aw_cop_cfg.h"
#include "copinc.h"
#include "ccmmain.h"
#include "objdicts_mst/obdcfg.h"

#define __COP_MST_LOCK(lock)             /* 锁 */
#define __COP_MST_UNLOCK(lock)           /* 解锁 */

#define __COP_MST_DLY(d)    aw_mdelay(d) /* 系统延时 */

#define __COP_MST_FRAME_TYPE_NMT     0   /* 帧类型为网络管理 */
#define __COP_MST_FRAME_TYPE_PDO     1   /* 帧类型为过程数据对象 */
#define __COP_MST_FRAME_TYPE_SDO     2   /* 帧类型为服务数据对象 */

#define __COP_MST_STATUS_START             1   /* 启动主站 */
#define __COP_MST_STATUS_STOP              2   /* 停止主站 */
#define __COP_MST_STATUS_PRE_OPERATIONAL   128 /* 主站 进入预工作状态  */
#define __COP_MST_STATUS_RESET             129 /* 复位主站 */
#define __COP_MST_STATUS_RESET_CONNECTION  130 /* 复位主站连接 */

/******************************************************************************/
tCopKernel PUBLIC AppCbRxPdo    \
    (CCM_DECL_INSTANCE_HDL_ uint16_t wPdoCommuIndex_p);
tCopKernel PUBLIC AppCbTxPdo    \
    (CCM_DECL_INSTANCE_HDL_ uint16_t wPdoCommuIndex_p);
tCopKernel PUBLIC AppCbNmtEvent \
    (CCM_DECL_INSTANCE_HDL_ tNmtEvent NmtEvent_p);

/*******************************************************************************
  PDO  队列结构体定义
*******************************************************************************/
/* CANopen数据报文帧结构体声明 */
typedef struct CopFrameHead {
    uint8_t  head;            /* 起始字                        */
    uint32_t timestamp;       /* 时间标志量                */
    uint8_t  type;            /* 对象报文的类型         */
    uint8_t  device;          /* CAN 通道号(1 ~ 4) */
    uint8_t  node_id;         /* 节点ID            */
    uint16_t attrib;          /* 帧数据的属性             */
    uint16_t index;           /* 服务数据索引             */
    uint16_t subindex;        /* 服务数据子索引         */
    uint8_t  info;            /* 该帧数据的长度         */
    uint8_t  data[8];         /* 报文数据                     */
} tCopFrameHead;

#define __COP_MST_FRAME_SIZE (sizeof(tCopFrameHead))
#define __COP_MST_PDO_QUEUE_SIZE (__COP_MST_FRAME_SIZE * 10)

typedef struct PdoQueueInfo {
    uint8_t   bNodeID;
    uint16_t  wPdoID;
    uint8_t  *pbPdoQueue;

    struct PdoQueueInfo *pNext;
} tPdoQueueList;

typedef struct PdoFrame {
    uint8_t bData[__COP_MST_FRAME_SIZE];
} tPdoFrame;

typedef struct PdoQueueHead {
    tPdoFrame  *pOut;
    tPdoFrame  *pIn;
    tPdoFrame  *pEnd;
    uint8_t     bFull;
    tPdoFrame   Buff[1];
} tPdoQueueHead;

/*******************************************************************************
   PDO queue return value
*******************************************************************************/
#define __COP_MST_QUEUE_OK                 0           /*  操作成功                    */
#define __COP_MST_QUEUE_NOK                1           /*  操作失败                    */
#define __COP_MST_QUEUE_EMPTY              4           /*  无数据                         */
#define __COP_MST_QUEUE_FULL               8           /*  队列满                         */

/*******************************************************************************
   App Instance Infomation
 ******************************************************************************/
typedef struct AppInstanceInfo {
    CONST tCcmInitParam ROM*    m_pInitParam;
    CONST tVarParam ROM*        m_pVarTab;
    uint16_t                    m_wVarTabCount;
    tPdoParam ROM*              m_pPdoTab;
    uint16_t                    m_wPdoTabCount;
    uint8_t ROM*                m_pSlaveTab;
    uint16_t                    m_wSlaveTabCount;
    tSdocParam ROM*             m_pSdoClientTab;
    tNmtmSlaveParam ROM*        m_pDefaultLifeguardParameter;
    tHbcProdParam ROM*          m_pHeartbeatProducerParam;

    tSdocTransferParam MEM      m_SdoClientTransferParam;
    uint16_t MEM                m_wSdocData;
    uint32_t                    m_dwSdoTransData;

#if ((CCM_MODULE_INTEGRATION & CCM_MODULE_LSSM) != 0)
    uint8_t                     m_bProcessStep;
    uint8_t                     m_bBdiIndex;
    uint8_t                     m_bNewNodeId;
    tTime                       m_LssMstTime;
#endif

    tCopInstanceHdl MEM         m_bInstanceHandle;
    aw_bool_t                   m_bIsUsed;

    uint8_t                     m_bNodeId;
    uint8_t                     m_bState;
    uint8_t                     m_bSlaveCount;
    uint8_t                     m_bSdoCount;

    uint32_t
        m_bPdoQueue[PDO_MAX_RXPDO_IN_OBD][(__COP_MST_PDO_QUEUE_SIZE + 3) / 4];
        
    tPdoQueueList   m_PdoQChain[PDO_MAX_RXPDO_IN_OBD];
    tPdoQueueList  *m_pPdoQChainFree;
    tPdoQueueList  *m_pPdoQChainHead;

    aw_cop_mst_node_info_t      node_list[AW_COP_MAX_SLAVES];
    aw_cop_mst_pfn_pdo_recv_t   pfn_pdo_recv_callback;
    aw_cop_mst_pfn_emcc_event_t pfn_emcc_event_callback;

} tAppInstanceInfo;

typedef aw_cop_data_t tAppInstanceData;

tCcmInitParam    MEM  CcmInitParam_g[COP_MAX_INSTANCES];
tAppInstanceInfo MEM  aAppInstanceInfo_g[COP_MAX_INSTANCES];
tAppInstanceData     *pAppInstanceData_g[COP_MAX_INSTANCES];
aw_can_baud_param_t   aBrtTable_g[COP_MAX_INSTANCES][AW_COP_BRT_LIST_CNT];
BYTE                  aCanChn_g[COP_MAX_INSTANCES];

AW_MUTEX_DECL_STATIC(__g_cop_comm_mutex);

/*******************************************************************************
                                                                声明
 ******************************************************************************/
tCopKernel PUBLIC ObdInitRam  (void MEM* pInitParam_p);

static tCopKernel __emcc_event_callback (
    CCM_DECL_INSTANCE_HDL_ uint8_t bNodeId_p, tEmcParam MEM* pEmcParam_p);

/******************************************************************************/
uint8_t PdoQueueCreate (void *pvBuf, uint32_t dwSize)
{
    tPdoQueueHead *puqQueue;

    uint32_t  dwMaxData;

    if (pvBuf && dwSize >=
        ((uint32_t)(((tPdoQueueHead *)0)->Buff) + __COP_MST_FRAME_SIZE)) {

        puqQueue  = (tPdoQueueHead *)pvBuf;
        dwMaxData = (uint32_t)((dwSize - __COP_MST_FRAME_SIZE -
                    (uint32_t)(((tPdoQueueHead *)0)->Buff)) / sizeof(uint8_t));

        puqQueue->pEnd  = puqQueue->Buff + dwMaxData / __COP_MST_FRAME_SIZE + 1;
        puqQueue->pOut  = puqQueue->Buff;
        puqQueue->pIn   = puqQueue->Buff;
        puqQueue->bFull = 0;
        return __COP_MST_QUEUE_OK;
    } else {
        return __COP_MST_QUEUE_NOK;
    }
}

/******************************************************************************/
uint8_t PdoQueueRead (uint8_t *pbRet, void *pvBuf, uint8_t bLenth)
{
    uint8_t        i;
    tPdoQueueHead *puqQueue = (tPdoQueueHead *)pvBuf;

    if ((puqQueue->pOut != puqQueue->pIn) || (1 == puqQueue->bFull)) {

        for (i = 0; (i < __COP_MST_FRAME_SIZE) && (i < bLenth); i++) {
            *(pbRet + i) = puqQueue->pOut->bData[i];
        }
        if ((puqQueue->pOut + 1) < puqQueue->pEnd) {
            puqQueue->pOut++;
        } else {
            puqQueue->pOut = puqQueue->Buff;
        }
        puqQueue->bFull = 0;
        return __COP_MST_QUEUE_OK;
    }
    return __COP_MST_QUEUE_EMPTY;
}

/******************************************************************************/
uint8_t PdoQueueWrite (void *pvBuf, uint8_t* pbData, uint8_t bLenth)
{
    uint8_t        i;
    tPdoQueueHead *puqQueue = (tPdoQueueHead *)pvBuf;

    if (1 == puqQueue->bFull) {
        if ((puqQueue->pOut + 1) >= puqQueue->pEnd) {
            puqQueue->pOut = puqQueue->Buff;
        } else {
            puqQueue->pOut++;
        }
    }
    for (i = 0; (i < __COP_MST_FRAME_SIZE) && (i < bLenth); i++) {
        puqQueue->pIn->bData[i] = *(pbData + i);
    }
    if ((puqQueue->pIn +1) >= puqQueue->pEnd) {
        puqQueue->pIn = puqQueue->Buff;
    } else {
        puqQueue->pIn++;
    }
    if (puqQueue->pIn == puqQueue->pOut) {
        puqQueue->bFull = 1;

        return __COP_MST_QUEUE_FULL;
    }
    return __COP_MST_QUEUE_OK;
}

/******************************************************************************/
static uint8_t __cop_app_inst_get (CCM_DECL_INSTANCE_HDL)
{
    int i = 0;

#if (COP_MAX_INSTANCES > 1)
    for (i = 0; i < AW_NELEMENTS(aAppInstanceInfo_g); i++) {
        if (aAppInstanceInfo_g[i].m_bIsUsed == AW_TRUE &&
            aAppInstanceInfo_g[i].m_bInstanceHandle == InstanceHandle) {
            break;
        }
    }

    if (i > AW_NELEMENTS(aAppInstanceInfo_g)) {
        i = CCM_ILLINSTANCE_HDL;
    }
#endif

    return i;
}

/******************************************************************************/
static void SdoReadTransBytes (CCM_DECL_INSTANCE_HDL_
                               tSdocCbFinishParam MEM* pParamList_p)
{
    uint8_t inst;

    tAppInstanceInfo MEM* pAppInst = NULL;

#if (COP_MAX_INSTANCES > 1)
    inst = __cop_app_inst_get(CCM_INSTANCE_HDL);
    if (inst == CCM_ILLINSTANCE_HDL) {
        return;
    }

    pAppInst = &aAppInstanceInfo_g[inst];
#else
    pAppInst = &aAppInstanceInfo_g[0];
#endif

    if (kSdocTransferFinished != pParamList_p->m_SdocState) {
        __COP_MST_LOCK();
        pAppInst->m_dwSdoTransData = 0;
        __COP_MST_UNLOCK();
        return;
    }

    __COP_MST_LOCK();
#if (COP_MAX_INSTANCES > 1)
    SdocGetTransmittedBytes(&pAppInst->m_bInstanceHandle,
                            pParamList_p->m_wClientIndex,
                            &pAppInst->m_dwSdoTransData);
#else
    SdocGetTransmittedBytes(pParamList_p->m_wClientIndex,
                           &pAppInst->m_dwSdoTransData);
#endif
    __COP_MST_UNLOCK();
}

/******************************************************************************/
static tCopKernel RunSdoTransfer (uint8_t         inst,
                                  tSdocType       SdocType_p,
                                  uint16_t        wIndex_p,
                                  uint8_t         bSubindex_p,
                                  void GENERIC*   pData_p,
                                  tObdSize        ObdSize_p,
                                  uint32_t        wait_time)
{
    tCopKernel Ret;

    tSdocState MEM SdocState;
    uint32_t   MEM dwAbortCode;

    tAppInstanceInfo MEM* pAppInst;

    pAppInst = &aAppInstanceInfo_g[inst];

    /* fill out SDO client transfer structure*/
    pAppInst->m_SdoClientTransferParam.m_wClientIndex  = 0x1280;
    pAppInst->m_SdoClientTransferParam.m_wObjIndex     = wIndex_p;
    pAppInst->m_SdoClientTransferParam.m_bObjSubIndex  = bSubindex_p;
    pAppInst->m_SdoClientTransferParam.m_pData         = pData_p;
    pAppInst->m_SdoClientTransferParam.m_DataLen       = ObdSize_p;
    pAppInst->m_SdoClientTransferParam.m_TimeOut       = 0;
    pAppInst->m_SdoClientTransferParam.m_SdocType      = SdocType_p;
    pAppInst->m_SdoClientTransferParam.m_pfnCbFinished = NULL;

    if ((SdocType_p == kSdocTypeSegUp) ||
        (SdocType_p == kSdocTypeBlockUp)){
        pAppInst->m_SdoClientTransferParam.m_pfnCbFinished = SdoReadTransBytes;
    } else {
        pAppInst->m_SdoClientTransferParam.m_pfnCbFinished = NULL;
    }

    /* start SDO transfer*/
#if (COP_MAX_INSTANCES > 1)
    Ret = CcmSdocStartTransfer (pAppInst->m_bInstanceHandle,
                                &pAppInst->m_SdoClientTransferParam);
#else
    Ret = CcmSdocStartTransfer (&pAppInst->m_SdoClientTransferParam);
#endif

    if (Ret != kCopSuccessful) {

        goto Exit;
    }

    /* loop until transfer is not running */
    do
    {
        // todo 考虑此处调用 CcmProcess 使其消息能快速传输出去，
        // -否则用户处理CcmProcess任务延迟大了，可能会导致失败
        __COP_MST_DLY(10);
#if (COP_MAX_INSTANCES > 1)
        Ret = CcmSdocGetState (pAppInst->m_bInstanceHandle,
                               0x1280,
                               &SdocState,
                               &dwAbortCode);
#else
        Ret = CcmSdocGetState (0x1280,
                               &SdocState,
                               &dwAbortCode);
#endif
        if (Ret != kCopSuccessful) {
            goto Exit;
        }
        wait_time -= 10;
        if ((wait_time - 10) <= 0) {
            Ret = kCopSdocBusy;
            break;
        }

    } while (SdocState == kSdocTransferRunning);

    // check state of SDO transfer
    switch (SdocState) {

    case kSdocTransferTxAborted:
        PRINTF2 ("(%d) SDO client has sent abort 0x%lX\n",
                inst, dwAbortCode);
        Ret = (tCopKernel) kCopUserSDOAbort;
        break;

    case kSdocTransferRxAborted:
        PRINTF2 ("(%d) SDO server has sent abort 0x%lX\n",
                inst, dwAbortCode);
        Ret = (tCopKernel) kCopUserSDOAbort;
        break;

    case kSdocTransferFinished:
        PRINTF1 ("(%d) SDO transfer has successfuly completed\n",
                inst);
        break;
    }

Exit:
    return Ret;
}

/******************************************************************************/
tCopKernel PUBLIC AppCbRxPdo (CCM_DECL_INSTANCE_HDL_ uint16_t wPdoCommuIndex_p)
{
    uint8_t   inst;
    uint8_t   i   = 0;
    uint32_t  ret = TRUE;
    tCobParam cob_param;

    tCopFrameHead     cop_frame;
    tPdoQueueList    *pPdoQueue;
    tAppInstanceInfo *pAppInst;
    tAppInstanceData *pAppInstData;

    aw_cop_pdo_msg_t  pdo_msg;

#if (COP_MAX_INSTANCES > 1)
    inst = __cop_app_inst_get(CCM_INSTANCE_HDL);
    if (inst == CCM_ILLINSTANCE_HDL) {
        return kCopInvalidInstanceParam;
    }

    pAppInst = &aAppInstanceInfo_g[inst];
    pAppInstData = pAppInstanceData_g[inst];
#else
    pAppInst = &aAppInstanceInfo_g[0];
    pAppInstData = pAppInstanceData_g[0];
#endif
    pPdoQueue = (tPdoQueueList *)pAppInst->m_pPdoQChainHead;

    PRINTF1 ("RX-PDO %d event\n", (uint16_t)wPdoCommuIndex_p & 0x1FF);

    memset(&cop_frame, 0, sizeof(cop_frame));
    memset(&pdo_msg, 0, sizeof(pdo_msg));

    /* search PDO table */
    for (i = 0; i < pAppInst->m_wPdoTabCount; i++) {
        if (pAppInst->m_pPdoTab[i].m_wPdoCommuIndex == wPdoCommuIndex_p){
            break;
        }
    }

    for (cob_param.m_CobHdl = 0;
         cob_param.m_CobHdl < COB_MAX_RX_COB_ENTRIES;
         cob_param.m_CobHdl++) {
#if (COP_MAX_INSTANCES > 1)
        CcmCobGetParam(
            pAppInst->m_bInstanceHandle, (tCobParam MEM*) &cob_param);
#else
        CcmCobGetParam((tCobParam MEM*)&cob_param);
#endif
        if (pAppInst->m_pPdoTab[i].m_dwCanId == cob_param.m_CobId){
            break;
        }
    }

    /* set frame infomation */
    cop_frame.head      = 0x0e;
    cop_frame.timestamp = 0;
    cop_frame.type      = __COP_MST_FRAME_TYPE_PDO;
    cop_frame.device    = 0x00;
    cop_frame.node_id   = (uint8_t)pAppInst->m_pPdoTab[i].m_bNodeID;
    cop_frame.attrib    = (uint16_t)pAppInst->m_pPdoTab[i].m_dwCanId;
    cop_frame.index     = wPdoCommuIndex_p;
    cop_frame.subindex  = 0x00;
    cop_frame.info      = cob_param.m_bSize;

    memcpy(cop_frame.data, pAppInstData->p_digit_out_8bit_tab, cop_frame.info);

    if (pAppInst->pfn_pdo_recv_callback != NULL) {
        pdo_msg.node_id = pAppInst->m_pPdoTab[i].m_bNodeID;
        pdo_msg.pdo_id  = pAppInst->m_pPdoTab[i].m_dwCanId;
        pdo_msg.pdo_len = cob_param.m_bSize;

        memcpy(pdo_msg.pdo_data,
               pAppInstData->p_digit_out_8bit_tab,
               cob_param.m_bSize);

        ret = pAppInst->pfn_pdo_recv_callback(&pdo_msg);
    }

    if (ret != FALSE) {

        while(NULL != pPdoQueue) {
            if ((pPdoQueue->wPdoID  == cop_frame.attrib ) &&
                (pPdoQueue->bNodeID == cop_frame.node_id)) {
                break;
            }
            pPdoQueue = pPdoQueue->pNext;
        }

        if((pPdoQueue != NULL)&&(pPdoQueue->pbPdoQueue != NULL)) {
            PdoQueueWrite(pPdoQueue->pbPdoQueue,
                          (uint8_t*)&cop_frame,
                          __COP_MST_FRAME_SIZE);
        }
    }

    return kCopSuccessful;
}

/******************************************************************************/
tCopKernel PUBLIC AppCbTxPdo (CCM_DECL_INSTANCE_HDL_ uint16_t wPdoCommuIndex_p)
{
    PRINTF1 ("TX-PDO %d event\n", (uint16_t)wPdoCommuIndex_p & 0x1FF);
    return kCopSuccessful;
}

/******************************************************************************/
tCopKernel PUBLIC AppCbNmtEvent (CCM_DECL_INSTANCE_HDL_ tNmtEvent NmtEvent_p)
{
    uint8_t     inst;
    tCopKernel  ret = kCopSuccessful;
    tAppInstanceInfo MEM* pAppInst;

#if (COP_MAX_INSTANCES > 1)
    inst = __cop_app_inst_get(CCM_INSTANCE_HDL);
    if (inst == CCM_ILLINSTANCE_HDL) {
        return kCopInvalidInstanceParam;
    }

    pAppInst = &aAppInstanceInfo_g[inst];

#else
    pAppInst = &aAppInstanceInfo_g[0];
#endif
    /* which event is called */
    switch (NmtEvent_p) {

    case kNmtEvEnterInitialising:

        /* init CANopen master functionality */
        PRINTF0 ("init NMT master...\n");
        ret = CcmInitNmtm (CCM_INSTANCE_HDL);
        if (ret != kCopSuccessful) {
            goto Exit;
        }

        /* define all NMT slaves which has to be controlled by NMT master*/
        PRINTF0 ("define all NMT slaves...\n");
        ret = CcmDefineSlaveTab (CCM_INSTANCE_HDL_
                                (uint8_t GENERIC*)pAppInst->m_pSlaveTab,
                                 pAppInst->m_wSlaveTabCount);

        if (ret != kCopSuccessful) {
            goto Exit;
        }

        /* init Heartbeat Consumer functionality */
        PRINTF0 ("init Heartbeat Consumer...\n");

        ret = CcmInitHbc (CCM_INSTANCE_HDL);

        if (ret != kCopSuccessful) {
            goto Exit;
        }

        /* init Emergency Consumer functionality */
        PRINTF0 ("init Emergency Consumer...\n");

        ret = CcmInitEmccEx (CCM_INSTANCE_HDL_ __emcc_event_callback);
        if (ret != kCopSuccessful) {
            goto Exit;
        }

        /* define all variables for CANopen  */
        PRINTF0 ("define all process variables...\n");

        ret = CcmDefineVarTab (CCM_INSTANCE_HDL_
                              (tVarParam GENERIC*)pAppInst->m_pVarTab,
                               pAppInst->m_wVarTabCount);

        if (ret != kCopSuccessful) {
            goto Exit;
        }
        break;

    // -------------------------------------------------------------------------
    // All application obejcts (0x2000-0x9FFF) has been reseted.
    case kNmtEvResetNode:
        break;

    // -------------------------------------------------------------------------
    // All communication obejcts (0x1000-0x1FFF) has been reseted.
    // Now all PDOs and SDOs has to be defined.
    case kNmtEvResetCommunication:

        PRINTF0 ("define all PDO communication and mappimg parameter...\n");

        ret = CcmDefinePdoTab (CCM_INSTANCE_HDL_
                              (tPdoParam GENERIC*)pAppInst->m_pPdoTab,
                               pAppInst->m_wPdoTabCount);

        if (ret != kCopSuccessful) {
            goto Exit;
        }

        PRINTF0 ("define all Emergency Producer...\n");

        ret = CcmEmccDefineProducerTab(CCM_INSTANCE_HDL_
                                      (uint8_t GENERIC*)pAppInst->m_pSlaveTab,
                                       pAppInst->m_wSlaveTabCount);

        if (ret != kCopSuccessful){
            goto Exit;
        }
#if (COP_MAX_INSTANCES > 1)
        ret = CcmConfigEmcp (
            CCM_INSTANCE_HDL_
            0x80 + CcmInitParam_g[inst].m_bInitNodeId);
#else
        ret = CcmConfigEmcp (CCM_INSTANCE_HDL_
                             0x80 + CcmInitParam_g[0].m_bInitNodeId);
#endif
        if (ret != kCopSuccessful) {
            goto Exit;
        }
        break;

    // -------------------------------------------------------------------------
    // Node is changed to state PRE-OPERATIONAL.
    // All SDOs are active.
    case kNmtEvEnterPreOperational:
        break;

    // -------------------------------------------------------------------------
    // Node is changed to state OPERATIONAL.
    // All SDOs and PDOs are active.
    case kNmtEvEnterOperational:
        break;

    // -------------------------------------------------------------------------
    // Node is changed to state STOPPED.
    // No SDOs and PDOs are active.
    case kNmtEvEnterStopped:
        break;

    // -------------------------------------------------------------------------
    // others
    default:
        break;

    }

Exit:
    if (ret != kCopSuccessful) {
        PRINTF1 ("ERROR: AppCbNmtEvent(): ret = 0x%X\n", (uint16_t) ret);
    }
    return ret;
}

/*******************************************************************************
** Function name:           CalDays
** Descriptions:            计算相对天数
** input parameters:        since_time: 开始时间
**                          dwCurTime:  结束时间
** output parameters:       none
** Returned value:          dwdays:     相对天数
**                            -1：         参数错误
** Created by:              cae
** Created Date:            2009-3-19
*******************************************************************************/
static int __cal_days (aw_cop_time_t *p_since_time, aw_cop_time_t *p_cur_time)
{
    uint32_t days  = 0;
    uint32_t i = 0;
    uint32_t j = 0;

    /*
     * check parameter
     */
    if((p_since_time->month == 0)  ||
       (p_since_time->month > 12)  ||
       (p_since_time->day   == 0)  ||
       (p_cur_time->month   == 0)  ||
       (p_cur_time->month   > 12)  ||
       (p_cur_time->day     == 0)) {
        return -1;
    }

    if(p_cur_time->year < p_since_time->year) {

        return -1;

    } else if(p_cur_time->year == p_since_time->year) {
        if(p_cur_time->month < p_since_time->month) {
            return -1;
        } else if (p_cur_time->month == p_since_time->month) {
            if (p_cur_time->day < p_since_time->day) {
                return -1;
            } else {
                switch (p_cur_time->month) {
                case 1:
                case 3:
                case 5:
                case 7:
                case 8:
                case 10:
                case 12:
                    if ((p_since_time->day > 31) ||
                        (p_cur_time->day   > 31)) {
                        return -1;
                    }
                    break;
                case 4:
                case 6:
                case 9:
                case 11:
                    if ((p_since_time->day > 30) ||
                        (p_cur_time->day   > 30)) {
                        return -1;
                    }
                    break;

                case 2:
                    if (((p_since_time->year % 4   == 0)  &&
                         (p_since_time->year % 100 != 0)) ||
                         (p_since_time->year % 400 == 0)) {
                        if ((p_since_time->day > 29)||
                            (p_cur_time->day   > 29)) {
                            return -1;
                        }
                    } else {
                        if ((p_since_time->day > 28)||
                            (p_cur_time->day   > 28)) {
                            return -1;
                        }
                    }
                    break;
                }
                return (p_cur_time->day - p_since_time->day);
            }
        }
    }

    /*
     * cal. days of since year
     */
    j = (p_cur_time->year > p_since_time->year) ? 12 : p_cur_time->month;

    for (i = p_since_time->month; i <= j; i++) {

        switch (i) {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            if (i == p_since_time->month) {
                if(p_since_time->day > 31) {
                    return -1;
                }
            }
            if ((p_cur_time->year == p_since_time->year) &&
                (i == p_cur_time->month)){
            } else {
                days += 31;
            }

            break;

        case 4:
        case 6:
        case 9:
        case 11:
            if (i == p_since_time->month) {
                if(p_since_time->day > 30) {
                    return -1;
                }
            }
            if ((p_cur_time->year == p_since_time->year) &&
                (i == p_cur_time->month)){

            } else {
                days += 30;
            }
            break;

        case 2:
            if(((p_since_time->year % 4   == 0)  &&
                (p_since_time->year % 100 != 0)) ||
                (p_since_time->year % 400 == 0)) {
                if(i == p_since_time->month) {
                    if(p_since_time->day > 29) {
                        return -1;
                    }
                }

                if((p_cur_time->year == p_since_time->year) &&
                   (i == p_cur_time->month)){

                } else {
                    days += 29;
                }

            } else {
                if(i == p_since_time->month) {
                    if(p_since_time->day > 28) {
                        return -1;
                    }
                }
                if((p_cur_time->year == p_since_time->year) &&
                   (i == p_cur_time->month)){
                } else {
                    days += 28;
                }
            }
            break;

        default:
            return -1;
        }
    }
    days -= (p_since_time->day - 1);

    /*
     * cal. days of current year
     */
    for (i = (p_cur_time->year == p_since_time->year)? p_cur_time->month : 1;
         i <= p_cur_time->month;
         i++) {
        switch (i) {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            if (i == p_cur_time->month) {
                if(p_cur_time->day > 31) {
                    return -1;
                }
                days += (p_cur_time->day-1);
                break;
            }
            days += 31;
            break;

        case 4:
        case 6:
        case 9:
        case 11:
            if (i == p_cur_time->month) {
                if(p_cur_time->day > 30) {
                    return -1;
                }
                days += (p_cur_time->day-1);
                break;
            }
            days += 30;
            break;

        case 2:
            if (((p_cur_time->year % 4   == 0)  &&
                 (p_cur_time->year % 100 != 0)) ||
                 (p_cur_time->year % 400 == 0)) {
                if (i == p_cur_time->month) {
                    if(p_cur_time->day > 29) {
                        return -1;
                    }
                    days += (p_cur_time->day - 1);
                    break;
                }
                days += 29;
            } else {
                if (i == p_cur_time->month) {
                    if(p_cur_time->day > 28) {
                        return -1;
                    }
                    days += (p_cur_time->day - 1);
                    break;
                }
                days += 28;
            }
            break;

        default:
            return -1;
        }
    }

    /*
     * sum. all days during these years
     */
    for(i = p_since_time->year + 1 ; i < p_cur_time->year ; i++ ){
        if (((i % 4 == 0) && (i % 100 != 0)) || (i % 400 == 0)){
            days += 366;
        } else {
            days += 365;
        }
    }

    return days;
}

/******************************************************************************/
static tCopKernel __emcc_event_callback (
    CCM_DECL_INSTANCE_HDL_ uint8_t bNodeId_p, tEmcParam MEM* pEmcParam_p)
{
    uint8_t inst;

    tAppInstanceInfo MEM* pAppInst;

    aw_cop_emcy_msg_t emcy_msg;

#if (COP_MAX_INSTANCES > 1)
    inst = __cop_app_inst_get(CCM_INSTANCE_HDL);
    if (inst == CCM_ILLINSTANCE_HDL) {
        return kCopInvalidInstanceParam;
    }

    pAppInst = &aAppInstanceInfo_g[inst];

#else
    pAppInst = &aAppInstanceInfo_g[0];
#endif

    if (pAppInst != NULL && pAppInst->pfn_emcc_event_callback != NULL) {

        emcy_msg.node_id = bNodeId_p;
        emcy_msg.err_code = pEmcParam_p->m_wEmcyErrorCode;
        emcy_msg.err_reg_code = pEmcParam_p->m_bErrorRegister;
        COP_MEMCPY(emcy_msg.err_specific,
                   pEmcParam_p->m_abManufactErrorCode,
                   sizeof(emcy_msg.err_specific));

        pAppInst->pfn_emcc_event_callback(&emcy_msg);
    }

    return kCopSuccessful;
}

/******************************************************************************/
static tCdrvBaudIndex cop_baud_index_get (aw_cop_baud_t baud)
{
    tCdrvBaudIndex index;

    switch(baud) {
    case AW_COP_BAUD_1M:   index = kBdi1MBaud;   break;
    case AW_COP_BAUD_800K: index = kBdi800kBaud; break;
    case AW_COP_BAUD_500K: index = kBdi500kBaud; break;
    case AW_COP_BAUD_250K: index = kBdi250kBaud; break;
    case AW_COP_BAUD_125K: index = kBdi125kBaud; break;
    case AW_COP_BAUD_100K: index = kBdi100kBaud; break;
    case AW_COP_BAUD_50K:  index = kBdi50kBaud;  break;
    case AW_COP_BAUD_20K:  index = kBdi20kBaud;  break;
    case AW_COP_BAUD_10K:  index = kBdi10kBaud;  break;
    default:               index = kBdi10kBaud;  break;
    }

    return index;
}

/******************************************************************************/
aw_cop_err_t aw_cop_mst_init (uint8_t inst, aw_cop_mst_info_t *p_info)
{
    uint32_t   i = 0;
    uint32_t   cop_version = 0;
    tCopKernel ret;

    tAppInstanceInfo MEM* pAppInst;
    tAppInstanceData MEM* pAppInstData;

    static aw_bool_t cop_comm_init_flag = FALSE;

    tCcmInitCall ccm_init_flag;

    if (inst >= COP_MAX_INSTANCES) {
        return -AW_COP_ERR_NOTEXIST;
    }

    if ((p_info->node_id == AW_COP_NODE_ID_INVAILD) ||
        (p_info->node_id >  AW_COP_NODE_ID_MAX)) {
           return -AW_COP_ERR_INVALIDPARAM;
    }

    /*
     * get current software version of CANopen stack
     */
    cop_version = CopGetStackVersion ();

    PRINTF3 ("*************************************************************\n"
            "*              CANopen master demo for aworks               *\n"
            "*************************************************************\n"
            "* Version: %d.%d.%d,  23.04.03                                *\n"
            "*                                                           *\n"
            "* Guangzhou ZLGMCU Development Co.,Ltd.                     *\n"
            "*************************************************************\n\n",
            (uint16_t) COP_GET_MAJOR_VERSION   (cop_version),
            (uint16_t) COP_GET_MINOR_VERSION   (cop_version),
            (uint16_t) COP_GET_RELEASE_VERSION (cop_version));


    /*
     * initiat global variables
     */
    pAppInst = &aAppInstanceInfo_g[inst];
    pAppInstData = pAppInstanceData_g[inst];

    COP_MEMSET(pAppInstData->p_slave_tab, 0, pAppInstData->slave_tab_size);
    COP_MEMSET(pAppInstData->p_sdo_client_tab, 0,
               pAppInstData->sdo_client_tab_size);
    COP_MEMSET(pAppInstData->p_hbt_producer_param,
               0,
               pAppInstData->hbt_producer_param_size);
    COP_MEMSET(pAppInstData->p_digit_in_8bit_tab,
               0,
               pAppInstData->digit_in_8bit_tab_size);
    COP_MEMSET(pAppInstData->p_digit_in_16bit_tab,
               0,
               pAppInstData->digit_in_16bit_tab_size);
    COP_MEMSET(pAppInstData->p_digit_out_8bit_tab,
               0,
               pAppInstData->digit_out_8bit_tab_size);
    COP_MEMSET(pAppInstData->p_digit_out_16bit_tab,
               0,
               pAppInstData->digit_out_16bit_tab_size);
    COP_MEMSET(pAppInstData->p_analog_in_16bit_tab,
               0,
               pAppInstData->analog_in_16bit_tab_size);
    COP_MEMSET(pAppInstData->p_analog_in_32bit_tab,
               0,
               pAppInstData->analog_in_32bit_tab_size);
    COP_MEMSET(pAppInstData->p_analog_out_16bit_tab,
               0,
               pAppInstData->analog_out_16bit_tab_size);
    COP_MEMSET(pAppInstData->p_analog_out_32bit_tab,
               0,
               pAppInstData->analog_out_32bit_tab_size);

    pAppInst->m_pVarTab = pAppInstData->p_var_tab;
    pAppInst->m_wVarTabCount =
        pAppInstData->var_tab_size / sizeof(tVarParam);

    pAppInst->m_pPdoTab = pAppInstData->p_pdo_tab;
    pAppInst->m_wPdoTabCount =
        pAppInstData->pdo_tab_size / sizeof(tPdoParam);

    pAppInst->m_pSlaveTab = pAppInstData->p_slave_tab;
    pAppInst->m_wSlaveTabCount =
        pAppInstData->slave_tab_size / sizeof(uint8_t);

    pAppInst->m_pSdoClientTab =
        pAppInstData->p_sdo_client_tab;
    pAppInst->m_pHeartbeatProducerParam =
        pAppInstData->p_hbt_producer_param;
    pAppInst->m_pDefaultLifeguardParameter =
        pAppInstData->p_lifeguard_default_param;

    pAppInst->m_bNodeId        = p_info->node_id;
    pAppInst->m_bSlaveCount    = 0;
    pAppInst->m_bSdoCount      = 0;
    pAppInst->m_pPdoQChainFree = &pAppInst->m_PdoQChain[0];
    pAppInst->m_pPdoQChainHead = NULL;
    COP_MEMSET(pAppInst->node_list, 0, sizeof(pAppInst->node_list));

    pAppInst->pfn_pdo_recv_callback = p_info->pfn_pdo_recv_callback;
    pAppInst->pfn_emcc_event_callback = p_info->pfn_emcc_event_callback;

    /*
     * initiat pdo queue
     */
    for (i = 0; i < PDO_MAX_RXPDO_IN_OBD; i++) {
        if (__COP_MST_QUEUE_OK != PdoQueueCreate(&pAppInst->m_bPdoQueue[i][0],
                                                 __COP_MST_PDO_QUEUE_SIZE)) {
            CcmInitParam_g[inst].m_bInitNodeId = 0;
            return -AW_COP_ERR_QUEUECREATE;
        }
        pAppInst->m_PdoQChain[i].bNodeID    = 0;
        pAppInst->m_PdoQChain[i].wPdoID     = 0;
        pAppInst->m_PdoQChain[i].pbPdoQueue =
            (uint8_t *)&pAppInst->m_bPdoQueue[i][0];

        if (i != PDO_MAX_RXPDO_IN_OBD - 1) {
            pAppInst->m_PdoQChain[i].pNext = &pAppInst->m_PdoQChain[i + 1];
        } else {
            pAppInst->m_PdoQChain[i].pNext = NULL;
        }
    }

    CcmInitParam_g[inst].m_BaudIndex     = cop_baud_index_get(p_info->baudrate);
    CcmInitParam_g[inst].m_bInitNodeId   = p_info->node_id;
    CcmInitParam_g[inst].m_pBdiTable     = aBrtTable_g[inst];
    CcmInitParam_g[inst].m_wSizeOfBdiTab = sizeof(aBrtTable_g[inst]);
    CcmInitParam_g[inst].m_dwAmr         = 0xFFFFFFFFL;
    CcmInitParam_g[inst].m_dwAcr         = 0x00000000L;

    CcmInitParam_g[inst].m_HwParam.m_ZeroBasedCanNo.m_bCanNo = aCanChn_g[inst];

    CcmInitParam_g[inst].m_fpCanInterruptEnable = TgtEnableCanInterrupt;
    CcmInitParam_g[inst].m_fpNmtEventCallback   = AppCbNmtEvent;
    CcmInitParam_g[inst].m_fpObdInitRam         = ObdInitRam;

    AW_MUTEX_LOCK(__g_cop_comm_mutex, AW_SEM_WAIT_FOREVER);
#if (COP_MAX_INSTANCES > 1)
    if (!cop_comm_init_flag) {
        cop_comm_init_flag = TRUE;
        ccm_init_flag = kCcmFirstInstance;
    } else {
        ccm_init_flag = kCcmAdditionalInstance;
    }
    ret = CcmInitCANopen(&pAppInst->m_bInstanceHandle,
                         &CcmInitParam_g[inst],
                         ccm_init_flag);

#else
    ret = CcmInitCANopen(&CcmInitParam_g[inst], kCcmFirstInstance);
#endif
    AW_MUTEX_UNLOCK(__g_cop_comm_mutex);

    if (ret != kCopSuccessful) {

        return -AW_COP_ERR_LOADLIB;
    }

    pAppInst ->m_bIsUsed = AW_TRUE;

    return AW_COP_ERR_NO_ERROR;
}

/******************************************************************************/
aw_cop_err_t aw_cop_mst_deinit (uint8_t inst)
{
    uint32_t i;

    tAppInstanceInfo MEM* pAppInst;

    if (inst >= COP_MAX_INSTANCES) {
        return -AW_COP_ERR_NOTEXIST;
    }

    pAppInst = &aAppInstanceInfo_g[inst];

    if (pAppInst->m_bIsUsed == AW_TRUE){
#if (COP_MAX_INSTANCES > 1)
        CcmShutDownCANopen(pAppInst->m_bInstanceHandle);
#else
        CcmShutDownCANopen();
#endif
    }
    
    COP_MEMSET(&CcmInitParam_g[inst], 0, sizeof(tCcmInitParam));

    COP_MEMSET((void*)pAppInst->m_bPdoQueue,
                0,
                sizeof(pAppInst->m_bPdoQueue));

    COP_MEMSET(pAppInst->m_pSlaveTab,
               0,
               pAppInst->m_wSlaveTabCount);

    COP_MEMSET(pAppInst->m_pSdoClientTab,
               0,
               pAppInst->m_wSlaveTabCount * sizeof(tSdocParam));

    COP_MEMSET(pAppInst->m_pHeartbeatProducerParam,
               0,
               pAppInst->m_wSlaveTabCount * sizeof(tHbcProdParam));

    PRINTF0 ("*************************************************************\n"
            "*              CANopen master demo closed                   *\n"
            "*************************************************************\n");
    /*
     *    注销PDO
     */
    for (i = 0; i < pAppInst->m_wPdoTabCount; i++) {
        pAppInst->m_pPdoTab[i].m_dwCanId |= 0x80000000;
    }

    pAppInst->m_bState = __COP_MST_STATUS_STOP;
    pAppInst ->m_bIsUsed = AW_FALSE;

    return AW_COP_ERR_NO_ERROR;
}

/******************************************************************************/
aw_cop_err_t aw_cop_mst_start (uint8_t inst)
{
    tCopKernel      ret;
    uint32_t        i;
    uint16_t        hbc_prod_time;
    tNmtmSlaveParam LifeGuardPara;

    tAppInstanceInfo MEM* pAppInst;
    tAppInstanceData MEM* pAppInstData;

    if (inst >= COP_MAX_INSTANCES) {
        return -AW_COP_ERR_NOTEXIST;
    }
    pAppInst = &aAppInstanceInfo_g[inst];
    pAppInstData = pAppInstanceData_g[inst];

    PRINTF0 ("connect this node to network...\n");
#if (COP_MAX_INSTANCES > 1)
    CcmConnectToNet(pAppInst->m_bInstanceHandle);
#else
    CcmConnectToNet();
#endif

    if (pAppInst->m_bState == __COP_MST_STATUS_START) {
#if (COP_MAX_INSTANCES > 1)
        CcmShutDownCANopen(pAppInst->m_bInstanceHandle);
#else
        CcmShutDownCANopen();
#endif
    }

    /*
     * NMT state machine is completely started now.
     * CANopen process is running
     */
    PRINTF0 ("\nCANopen process is running.\n");
    PRINTF0 ("\nset network to state OPERATIONAL...\n");

   /*
    * switch all nodes in state "OPERATIONAL"
    */
#if (COP_MAX_INSTANCES > 1)
    ret = CcmSendNmtCommand(
        pAppInst->m_bInstanceHandle, 0x00, kNmtCommStartRemoteNode);
#else
    ret = CcmSendNmtCommand(0x00, kNmtCommStartRemoteNode);
#endif

    if (ret != kCopSuccessful) {
       goto Error;
    }

    for(i = 0;
        (pAppInst->node_list[i].node_id != AW_COP_NODE_ID_INVAILD) &&
        (i < AW_COP_MAX_SLAVES);
        i++) {

        /*
         * define sdo client tab
         */
#if (COP_MAX_INSTANCES > 1)
        ret = CcmSdocDefineClientTab(pAppInst->m_bInstanceHandle,
                                     &pAppInst->m_pSdoClientTab[i],
                                     1);
#else
        ret = CcmSdocDefineClientTab(&pAppInst->m_pSdoClientTab[i], 1);
#endif

        if(ret != kCopSuccessful) {
            return -AW_COP_ERR_INVALIDPARAM;
        }

        switch(pAppInst->node_list[i].check_mode) {
        
        case AW_COP_MST_NODE_CHECK_WAY_GUARDING:

            /*
             *  set slave node life-guard parameter
             */
            if(pAppInst->node_list[i].check_period == 0) {
                AmiSetWord(
                    &pAppInst->m_wSdocData,
                    pAppInst->m_pDefaultLifeguardParameter->m_wLifeguardTime +
                    100);
            } else {
                AmiSetWord(&pAppInst->m_wSdocData,
                            pAppInst->node_list[i].check_period + 100);

                LifeGuardPara.m_wLifeguardTime =
                    pAppInst->node_list[i].check_period;
            }
        
            ret = RunSdoTransfer(inst,
                                 kSdocTypeSegDown,
                                 0x100C,
                                 0x00,
                                 &pAppInst->m_wSdocData,
                                 sizeof(uint16_t),
                                 1000);
            if (ret != kCopSuccessful){
                break;
            }

            /*
             *  configure life time factor of CANopen slave node
             */
            if (pAppInst->node_list[i].retry_factor == 0){
                AmiSetByte(
                    &pAppInst->m_wSdocData,
                    pAppInstData->\
                    p_lifeguard_default_param->m_bLifeguardFactor);
            } else {
                AmiSetByte (&pAppInst->m_wSdocData,
                            pAppInst->node_list[i].retry_factor);
                LifeGuardPara.m_bLifeguardFactor =
                    pAppInst->node_list[i].retry_factor;
            }

            ret = RunSdoTransfer(inst,
                                 kSdocTypeSegDown,
                                 0x100D,
                                 0x00,
                                 &pAppInst->m_wSdocData,
                                 sizeof(uint8_t),
                                 1000);
            if (ret != kCopSuccessful){
                break;
            }

            if((pAppInst->node_list[i].retry_factor != 0) &&
               (pAppInst->node_list[i].check_period != 0)) {
#if (COP_MAX_INSTANCES > 1)

                CcmConfigLgm(pAppInst->m_bInstanceHandle,
                            (uint8_t)pAppInst->node_list[i].node_id,
                            (tNmtmSlaveParam GENERIC*)&LifeGuardPara);
            } else {
                CcmConfigLgm(pAppInst->m_bInstanceHandle,
                            (uint8_t)pAppInst->node_list[i].node_id,
                             pAppInst->m_pDefaultLifeguardParameter);

#else
                CcmConfigLgm((uint8_t)pAppInst->node_list[i].node_id,
                             (tNmtmSlaveParam GENERIC*)&LifeGuardPara);
            } else {
                CcmConfigLgm((uint8_t)pAppInst->node_list[i].node_id,
                    pAppInst->m_pDefaultLifeguardParameter);
#endif
            }

            break;
            
        case AW_COP_MST_NODE_CHECK_WAY_HEARTBEAT:
            /*
             *  configure life guard time of CANopen slave node
             */
            hbc_prod_time =
                (pAppInst->m_pHeartbeatProducerParam->wHeartbeatTime_p * 9) /
                10;
            AmiSetWord(&pAppInst->m_wSdocData, hbc_prod_time);
            ret = RunSdoTransfer(inst,
                                 kSdocTypeSegDown,
                                 0x1017,
                                 0x00,
                                 &pAppInst->m_wSdocData,
                                 sizeof(hbc_prod_time),
                                 1000);

            if (ret != kCopSuccessful){
                break;
            }
#if (COP_MAX_INSTANCES > 1)
            CcmHbcDefineProducerTab(pAppInst->m_bInstanceHandle,
                                    &pAppInst->m_pHeartbeatProducerParam[i],
                                    1);
#else
            CcmHbcDefineProducerTab(&pAppInst->m_pHeartbeatProducerParam[i], 1);
#endif
            break;

        default:
            break; 
        }        
    }
    pAppInst->m_bState = __COP_MST_STATUS_START;
    return AW_COP_ERR_NO_ERROR;

Error:
    pAppInst->m_bState = __COP_MST_STATUS_STOP;
    return -AW_COP_ERR_STARTDEV;
}

/******************************************************************************/
aw_cop_err_t aw_cop_mst_stop (uint8_t inst)
{
    uint32_t i;

    tAppInstanceInfo MEM* pAppInst;

    if (inst >= COP_MAX_INSTANCES) {
        return -AW_COP_ERR_NOTEXIST;
    }
    pAppInst = &aAppInstanceInfo_g[inst];

    if (pAppInst->m_bState != __COP_MST_STATUS_START) {
        return -AW_COP_ERR_NOTSTARTED;
    }
#if (COP_MAX_INSTANCES > 1)
    CcmShutDownCANopen(pAppInst->m_bInstanceHandle);
#else
    CcmShutDownCANopen();
#endif

    /*
     *    注销PDO
     */
    for (i = 0; i < pAppInst->m_wPdoTabCount; i++){
        pAppInst->m_pPdoTab[i].m_bNodeID  = 0;
        pAppInst->m_pPdoTab[i].m_dwCanId |= 0x80000000;
    }
    
    COP_MEMSET(pAppInst->m_pSlaveTab, 0, pAppInst->m_wSlaveTabCount);

    pAppInst->m_bState = __COP_MST_STATUS_STOP;

    return AW_COP_ERR_NO_ERROR;
}

/******************************************************************************/
aw_cop_err_t aw_cop_mst_add_node (uint8_t inst, aw_cop_mst_node_info_t *p_info)
{
    uint8_t i;

    tAppInstanceInfo MEM* pAppInst;

    if (inst >= COP_MAX_INSTANCES) {
        return -AW_COP_ERR_NOTEXIST;
    }

    if (p_info == NULL             ||
        p_info->retry_factor > 0xff) {

        return -AW_COP_ERR_INVALIDPARAM;
    }

    if ((p_info->node_id == AW_COP_NODE_ID_INVAILD) ||
        (p_info->node_id >  AW_COP_NODE_ID_MAX)) {
           return -AW_COP_ERR_INVALIDPARAM;
    }

    switch(p_info->check_mode) {
        case AW_COP_MST_NODE_CHECK_WAY_GUARDING:
        case AW_COP_MST_NODE_CHECK_WAY_HEARTBEAT:
        case AW_COP_MST_NODE_CHECK_WAY_DISABLE:
            break;
        default:
            return -AW_COP_ERR_INVALIDPARAM;
    }

    pAppInst = &aAppInstanceInfo_g[inst];
    /*
     *  check slave node id
     */
    for (i = 0; i < pAppInst->m_wSlaveTabCount; i++) {
        if (pAppInst->m_pSlaveTab[i] == p_info->node_id){
            return -AW_COP_ERR_SLAVEEXIST;
        }
        if (0 == pAppInst->m_pSlaveTab[i]){
            pAppInst->m_pSlaveTab[i] = p_info->node_id;
            break;
        }
    }

    if (i < pAppInst->m_wSlaveTabCount){
            
        __COP_MST_LOCK();
        
        pAppInst->m_pSdoClientTab[i].m_bValidFlags   = kSdocValidAllParams;
        pAppInst->m_pSdoClientTab[i].m_TimeOut       = 10000;
        pAppInst->m_pSdoClientTab[i].m_wClientIndex  = 0x1280;
        pAppInst->m_pSdoClientTab[i].m_dwRxCanId     = 0x580 + p_info->node_id;
        pAppInst->m_pSdoClientTab[i].m_dwTxCanId     = 0x600 + p_info->node_id;
        pAppInst->m_pSdoClientTab[i].m_bServerNodeId = p_info->node_id;
        
        pAppInst->m_pHeartbeatProducerParam->wHeartbeatTime_p  =
            (uint16_t)p_info->check_period;
        pAppInst->m_pHeartbeatProducerParam->bProducerNodeId_p =
            p_info->node_id;
        pAppInst->m_pHeartbeatProducerParam->bSubindex_p       = i + 1;

        pAppInst->node_list[i].node_id    =  p_info->node_id;
        pAppInst->node_list[i].check_mode =  p_info->check_mode;
        pAppInst->node_list[i].check_period = (uint16_t)p_info->check_period;
        pAppInst->node_list[i].retry_factor = (uint8_t)p_info->retry_factor;

        pAppInst->m_bSlaveCount = i + 1;
        pAppInst->m_bSdoCount   = i + 1;
        pAppInst->node_list[pAppInst->m_bSlaveCount].node_id   = 0;

        __COP_MST_UNLOCK();

        return AW_COP_ERR_NO_ERROR;
    }
       
    return -AW_COP_ERR_SLAVETABFUL;
 }

/******************************************************************************/
aw_cop_err_t aw_cop_mst_remove_node (uint8_t inst, uint8_t node_id)
{
    uint8_t i;

    tAppInstanceInfo MEM* pAppInst;

    if (inst >= COP_MAX_INSTANCES) {
        return -AW_COP_ERR_NOTEXIST;
    }

    if (node_id == AW_COP_NODE_ID_INVAILD || node_id > AW_COP_NODE_ID_MAX) {
        return -AW_COP_ERR_INVALIDPARAM;
    }
    pAppInst = &aAppInstanceInfo_g[inst];

    for (i = 0; i < pAppInst->m_wSlaveTabCount; i++) {
        if (node_id == pAppInst->m_pSlaveTab[i]){
            pAppInst->m_pSlaveTab[i] = 0;
            break;
        }                
    }
    
    if (i < pAppInst->m_wSlaveTabCount){
        __COP_MST_LOCK();

        for (;(pAppInst->m_pSlaveTab[i+1] != 0) &&
              (i < pAppInst->m_wSlaveTabCount);  i++) {

            pAppInst->m_pSlaveTab[i] =
                    pAppInst->m_pSlaveTab[i+1];

            COP_MEMCPY(&pAppInst->node_list[i],
                       &pAppInst->node_list[i+1],
                       sizeof(aw_cop_mst_node_info_t));

            COP_MEMCPY(&pAppInst->m_pSdoClientTab[i],
                       &pAppInst->m_pSdoClientTab[i+1],
                       sizeof(tSdocParam));

            COP_MEMCPY(&pAppInst->m_pHeartbeatProducerParam[i],
                       &pAppInst->m_pHeartbeatProducerParam[i+1],
                       sizeof(tHbcProdParam));

        }
        
        pAppInst->m_pSlaveTab[i] = 0;

        COP_MEMSET(&pAppInst->m_pSdoClientTab[i],
                   0,
                   sizeof(tSdocParam));

        COP_MEMSET(&pAppInst->m_pHeartbeatProducerParam[i],
                   0,
                   sizeof(tHbcProdParam));

        COP_MEMSET(&pAppInst->node_list[i],
                   0,
                   sizeof(aw_cop_mst_node_info_t));

        pAppInst->m_bSlaveCount  = i;
        pAppInst->m_bSdoCount    = i;

        __COP_MST_UNLOCK();

        return AW_COP_ERR_NO_ERROR;
    }
    
    for (i = 0; i < pAppInst->m_wPdoTabCount; i++){
        pAppInst->m_pPdoTab[i].m_bNodeID  = 0;
        pAppInst->m_pPdoTab[i].m_dwCanId |= 0x80000000;
    }
    return -AW_COP_ERR_SLAVENOTEXIST;

}

/******************************************************************************/
aw_cop_err_t aw_cop_mst_sdo_upload (uint8_t   inst,
                                    uint8_t   node_id,
                                    uint16_t  index,
                                    uint8_t   subindex,
                                    uint8_t  *p_data,
                                    uint32_t *p_length,
                                    uint32_t  wait_time)
{
    uint8_t i;

    tCopKernel ret;

    tAppInstanceInfo MEM* pAppInst;

    if (inst >= COP_MAX_INSTANCES) {
        return -AW_COP_ERR_NOTEXIST;
    }
    if (node_id == AW_COP_NODE_ID_INVAILD || node_id > AW_COP_NODE_ID_MAX) {
        return -AW_COP_ERR_INVALIDPARAM;
    }
    pAppInst = &aAppInstanceInfo_g[inst];

    if (pAppInst->m_bState != __COP_MST_STATUS_START) {
        return -AW_COP_ERR_NOTSTARTED;
    }

    if (p_data   == NULL  ||
       *p_length == 0     ){
        return -AW_COP_ERR_INVALIDPARAM;
    }

    if ((wait_time == 0) || (wait_time > (0xffffffff/10))) {
        return -AW_COP_ERR_INVALIDPARAM;
    }

    /*
     * check sdo id
     */
    for (i = 0; i < pAppInst->m_bSdoCount; i++){
        if (node_id == pAppInst->m_pSdoClientTab[i].m_dwTxCanId - 0x600) {
            break;
        }  
    }

    if(i == pAppInst->m_bSdoCount) {
        return -AW_COP_ERR_INVALIDPARAM;
    }

    pAppInst->m_pSdoClientTab[i].m_TimeOut = wait_time * 10;
    /*
     *  define SDO clients for CANopen slave node 
     */
#if (COP_MAX_INSTANCES > 1)
    ret = CcmSdocDefineClientTab(pAppInst->m_bInstanceHandle,
                                 pAppInst->m_pSdoClientTab,
                                 1);
#else
    ret = CcmSdocDefineClientTab(pAppInst->m_pSdoClientTab, 1);
#endif

    if (ret != kCopSuccessful){
        return -AW_COP_ERR_INVALIDPARAM;
    }
    /*
     * Send SDO in segment uploard mode
     */
    ret = RunSdoTransfer(inst,
                        (*p_length > 28)?kSdocTypeBlockUp:kSdocTypeSegUp,
                        (uint16_t)(index),
                        (uint8_t)(subindex),
                        (void GENERIC*)p_data,
                        *p_length,
                         wait_time);

    if (ret == kCopUserSDOAbort) {
#if (COP_MAX_INSTANCES > 1)
        ret = CcmSdocDefineClientTab(pAppInst->m_bInstanceHandle,
                                     pAppInst->m_pSdoClientTab,
                                     1);
#else
        ret = CcmSdocDefineClientTab(pAppInst->m_pSdoClientTab, 1);
#endif

        if (ret != kCopSuccessful){
            return -AW_COP_ERR_INVALIDPARAM;
        }
        ret = RunSdoTransfer(inst,
                             kSdocTypeSegUp,
                            (uint16_t)(index),
                            (uint8_t)(subindex),
                            (void GENERIC*)p_data,
                            *p_length,
                             wait_time);
    }
    if (ret != kCopSuccessful){
        return -AW_COP_ERR_SDOABORT;
    }

    __COP_MST_LOCK();
    *p_length = pAppInst->m_dwSdoTransData;
    pAppInst->m_dwSdoTransData = 0;
    __COP_MST_UNLOCK();
    
    return AW_COP_ERR_NO_ERROR;
}

/******************************************************************************/
aw_cop_err_t aw_cop_mst_sdo_dwonload (uint8_t    inst,
                                      uint8_t    node_id,
                                      uint16_t   index,
                                      uint8_t    subindex,
                                      uint8_t   *p_data,
                                      uint32_t   length,
                                      uint32_t   wait_time)
{
    uint8_t i = 0;

    tCopKernel ret = kCopSuccessful;
    tAppInstanceInfo MEM* pAppInst;

    if (inst >= COP_MAX_INSTANCES) {
        return -AW_COP_ERR_NOTEXIST;
    }

    if (node_id == AW_COP_NODE_ID_INVAILD || node_id > AW_COP_NODE_ID_MAX) {
        return -AW_COP_ERR_INVALIDPARAM;
    }

    if (p_data == NULL ||
        length == 0     ) {
        return -AW_COP_ERR_INVALIDPARAM;
    }

    if ((wait_time == 0) || (wait_time > (0xffffffff/10))) {
        return -AW_COP_ERR_INVALIDPARAM;
    }

    pAppInst = &aAppInstanceInfo_g[inst];

    if (pAppInst->m_bState != __COP_MST_STATUS_START) {
        return -AW_COP_ERR_NOTSTARTED;
    }

    for (i=0; i < pAppInst->m_bSdoCount; i++) {
        if (node_id == pAppInst->m_pSdoClientTab[i].m_dwTxCanId - 0x600) {
            break;
        }  
    }

    if(i == pAppInst->m_bSdoCount) {
        return -AW_COP_ERR_INVALIDPARAM;
    }
    pAppInst->m_pSdoClientTab[i].m_TimeOut = wait_time * 10;

    /*
     *  define SDO clients for CANopen slave node 
     */
#if (COP_MAX_INSTANCES > 1)
    ret = CcmSdocDefineClientTab(
        pAppInst->m_bInstanceHandle,
        (tSdocParam GENERIC*)&pAppInst->m_pSdoClientTab[i], 1);
#else
    ret = CcmSdocDefineClientTab(
        (tSdocParam GENERIC*)&pAppInst->m_pSdoClientTab[i], 1);
#endif

    if (ret != kCopSuccessful){
        return -AW_COP_ERR_INVALIDPARAM;
    }

    /*
     * Send SDO in segment uploard mode
     */
    ret = RunSdoTransfer(inst,
                        (length > 21)? kSdocTypeBlockDown:kSdocTypeSegDown,
                        (uint16_t)(index),
                        (uint8_t)(subindex),
                        (void GENERIC*)p_data,
                        length,
                        wait_time);

    if (ret == kCopUserSDOAbort) {
#if (COP_MAX_INSTANCES > 1)
        ret = CcmSdocDefineClientTab(
            pAppInst->m_bInstanceHandle,
            (tSdocParam*)&pAppInst->m_pSdoClientTab[i], 1);

#else
        ret = CcmSdocDefineClientTab(
            (tSdocParam*)&pAppInst->m_pSdoClientTab[i], 1);
#endif

        if (ret != kCopSuccessful){
            return -AW_COP_ERR_INVALIDPARAM;
        }
        ret = RunSdoTransfer (inst,
                              kSdocTypeSegDown,
                              (uint16_t)(index),
                              (uint8_t)(subindex),
                              (void GENERIC*)p_data,
                              length,
                              wait_time);
    }
    if (ret != kCopSuccessful){
        return -AW_COP_ERR_INVALIDPARAM;
    }
    return AW_COP_ERR_NO_ERROR;

}

/******************************************************************************/
aw_cop_err_t aw_cop_mst_input_pdo_install (
    uint8_t inst, uint8_t node_id, uint32_t pdo_id)
{
    uint8_t i = 0;
    uint8_t j = 0;

    tCopKernel ret = kCopSuccessful;

    tAppInstanceInfo MEM* pAppInst;

    tPdoQueueList *pPdoQueue;

    if (inst >= COP_MAX_INSTANCES) {
        return -AW_COP_ERR_NOTEXIST;
    }

    if (node_id == AW_COP_NODE_ID_INVAILD || node_id > AW_COP_NODE_ID_MAX) {
        return -AW_COP_ERR_INVALIDPARAM;
    }

    pAppInst = &aAppInstanceInfo_g[inst];

//    if (pAppInst->m_bState != __COP_MST_STATUS_START) {
//        return -AW_COP_ERR_NOTSTARTED;
//    }

    pPdoQueue = pAppInst->m_pPdoQChainHead;

    switch(pdo_id & 0x780) {
    case 0x180:
    case 0x280:
    case 0x380:
    case 0x480:
        break;
    /*
     * pdo_type which does not falls into above values is considered illegal
     */
    default:
        return -AW_COP_ERR_INVALIDPARAM;
    }

    j = pAppInst->m_wPdoTabCount - 4;
    for (i = 0; i < j; i++){
        if (pAppInst->m_pPdoTab[i].m_bNodeID == 0){
            break;
        }
    }

    if(i == j) {
        return -AW_COP_ERR_PDOTABFULL;
    }

    pAppInst->m_pPdoTab[i].m_bNodeID    = (uint8_t)(node_id & 0x7F);
    pAppInst->m_pPdoTab[i].m_dwCanId    = (pdo_id & 0x700007FF);
    pAppInst->m_pPdoTab[i].m_bValidFlag = kVarValidAll;

#if (COP_MAX_INSTANCES > 1)
    ret = CcmDefinePdoTab (
        pAppInst->m_bInstanceHandle, &pAppInst->m_pPdoTab[i], 1);
#else
    ret = CcmDefinePdoTab (&pAppInst->m_pPdoTab[i], 1);
#endif

    if (ret != kCopSuccessful) {
        return -AW_COP_ERR_INTERNAL;
    }

    if (pAppInst->m_pPdoQChainFree == NULL) {
        return -AW_COP_ERR_INTERNAL;
    }
    pAppInst->m_pPdoQChainFree->bNodeID = pAppInst->m_pPdoTab[i].m_bNodeID;
    pAppInst->m_pPdoQChainFree->wPdoID  = pAppInst->m_pPdoTab[i].m_dwCanId;
    while ((NULL != pPdoQueue)&&
           (NULL != pPdoQueue->pNext)) {
        pPdoQueue = pPdoQueue->pNext;
    }
    if(NULL == pAppInst->m_pPdoQChainHead) {
        pPdoQueue = pAppInst->m_pPdoQChainFree;
        pAppInst->m_pPdoQChainHead = pPdoQueue;
    } else {
        pPdoQueue->pNext = pAppInst->m_pPdoQChainFree;
        pPdoQueue        = pPdoQueue->pNext;
    }
    pAppInst->m_pPdoQChainFree = pAppInst->m_pPdoQChainFree->pNext;
    pPdoQueue->pNext = NULL;

    return AW_COP_ERR_NO_ERROR;
}

/******************************************************************************/
aw_cop_err_t aw_cop_mst_input_pdo_remove (
    uint8_t inst, uint8_t node_id, uint32_t pdo_id)
{
    uint8_t i = 0;
    uint8_t j = 0;

    tAppInstanceInfo MEM* pAppInst;

    tCopKernel ret = kCopSuccessful;

    if (inst >= COP_MAX_INSTANCES) {
        return -AW_COP_ERR_NOTEXIST;
    }

    if (node_id == AW_COP_NODE_ID_INVAILD || node_id > AW_COP_NODE_ID_MAX) {
        return -AW_COP_ERR_INVALIDPARAM;
    }

    pAppInst = &aAppInstanceInfo_g[inst];

    if (CcmInitParam_g[inst].m_bInitNodeId == 0) {
        return -AW_COP_ERR_NOTOPEN;
    }

    if (pAppInst->m_bState != __COP_MST_STATUS_START) {
        return -AW_COP_ERR_NOTSTARTED;
    }

    tPdoQueueList *pPdoQueue     = pAppInst->m_pPdoQChainHead;
    tPdoQueueList *pPdoQFreeTemp = NULL;
    tPdoQueueList *pPdoQPre      = NULL;


    switch(pdo_id & 0x780) {

    case 0x180:
    case 0x280:
    case 0x380:
    case 0x480:
        break;
    /*
     * pdo_type which does not falls into above values is considered illegal
     */
    default:
        return -AW_COP_ERR_INVALIDPARAM;
    }

    j = pAppInst->m_wPdoTabCount - 4;
    for(i = 0; i < j; i++){
        if((pAppInst->m_pPdoTab[i].m_dwCanId ==  pdo_id) &&
           (pAppInst->m_pPdoTab[i].m_bNodeID == (node_id & 0x7f))){
            break;
        }
    }

    if(i == j) {
        return -AW_COP_ERR_INVALIDPARAM;
    }

    pAppInst->m_pPdoTab[i].m_bNodeID  = 0;
    pAppInst->m_pPdoTab[i].m_dwCanId |= 0x80000000;
#if (COP_MAX_INSTANCES > 1)
    ret = CcmDefinePdoTab (pAppInst->m_bInstanceHandle,
                           &pAppInst->m_pPdoTab[i], 1);
#else
    ret = CcmDefinePdoTab (&pAppInst->m_pPdoTab[i], 1);
#endif

    if (ret != kCopSuccessful) {
        return -AW_COP_ERR_INTERNAL;
    }

    while (pPdoQueue != NULL) {
        if (pPdoQueue->wPdoID  == pdo_id) {
            pPdoQueue->bNodeID =  0;
            pPdoQueue->wPdoID  =  0;
            if (NULL == pPdoQPre) {
                pAppInst->m_pPdoQChainHead = pPdoQueue->pNext;
            } else {
                pPdoQPre->pNext = pPdoQueue->pNext;
            }
            pPdoQFreeTemp = pAppInst->m_pPdoQChainFree;
            pAppInst->m_pPdoQChainFree = pPdoQueue;
            pAppInst->m_pPdoQChainFree->pNext = pPdoQFreeTemp;
            break;
        }
        pPdoQPre  = pPdoQueue;
        pPdoQueue = pPdoQueue->pNext;
    }
    return AW_COP_ERR_NO_ERROR;
}

/******************************************************************************/
aw_cop_err_t aw_cop_mst_output_pdo_install (
    uint8_t inst, uint8_t node_id, uint32_t pdo_id)
{
    return AW_COP_ERR_NO_ERROR;
}

/******************************************************************************/
aw_cop_err_t aw_cop_mst_output_pdo_remove (
    uint8_t inst, uint8_t node_id, uint32_t pdo_id)
{
    return AW_COP_ERR_NO_ERROR;
}

/******************************************************************************/
aw_cop_err_t aw_cop_mst_input_pdo_get (uint8_t   inst,
                                       uint8_t   node_id,
                                       uint32_t  pdo_id,
                                       uint8_t  *p_data,
                                       uint32_t *p_length,
                                       uint32_t  wait_time)
{
    uint8_t  ret;

    tPdoQueueList   *pPdoQueue;
    tCopFrameHead    pdobuf;

    tAppInstanceInfo MEM* pAppInst;

    if (inst >= COP_MAX_INSTANCES) {
        return -AW_COP_ERR_NOTEXIST;
    }

    if (node_id == AW_COP_NODE_ID_INVAILD || node_id > AW_COP_NODE_ID_MAX) {
        return -AW_COP_ERR_INVALIDPARAM;
    }

    pAppInst  = &aAppInstanceInfo_g[inst];
    pPdoQueue = pAppInst->m_pPdoQChainHead;

    if (CcmInitParam_g[inst].m_bInitNodeId == 0) {
        return -AW_COP_ERR_NOTOPEN;
    }

    if (pAppInst->m_bState != __COP_MST_STATUS_START) {
        return -AW_COP_ERR_NOTSTARTED;
    }

    if (p_data   == NULL ||
        p_length == NULL ) {
        return -AW_COP_ERR_INVALIDPARAM;
    }

    switch(pdo_id & 0x780) {

    case 0x180:
    case 0x280:
    case 0x380:
    case 0x480:
        break;
    /*
     * pdo_type which does not falls into above values is considered illegal
     */
    default:
        return -AW_COP_ERR_INVALIDPARAM;
    }

    while (NULL != pPdoQueue) {
        if ((pPdoQueue->wPdoID  == pdo_id ) &&
            (pPdoQueue->bNodeID == node_id)) {
            break;
        }
        pPdoQueue = pPdoQueue->pNext;
    }

    if (NULL == pPdoQueue) {
        return -AW_COP_ERR_PDONOTREGISTER;
    }

reread:
    ret = PdoQueueRead((uint8_t*)&pdobuf,
                       pPdoQueue->pbPdoQueue,
                       __COP_MST_FRAME_SIZE);

    if (__COP_MST_QUEUE_EMPTY == ret) {
        if (0 == wait_time) {
            __COP_MST_DLY(10);
            goto reread;
        } else {
            while (wait_time) {
                ret = PdoQueueRead((uint8_t*)&pdobuf,
                                    pPdoQueue->pbPdoQueue,
                                    __COP_MST_FRAME_SIZE);
                if (__COP_MST_QUEUE_EMPTY == ret) {
                    wait_time --;
                    __COP_MST_DLY(1);
                } else {
                    goto readed;
                }
            }
            return -AW_COP_ERR_NODATA;
        }
    }

readed:
    if ((pdobuf.node_id == (node_id & 0x7f      ))  &&
        (pdobuf.attrib  == (pdo_id  & 0x700007FF))) {
        *p_length = (pdobuf.info < *p_length) ? pdobuf.info : *p_length;
        memcpy(p_data, pdobuf.data, *p_length);
        return AW_COP_ERR_NO_ERROR;
    }

    return -AW_COP_ERR_NODATA;
}

/******************************************************************************/
aw_cop_err_t aw_cop_mst_output_pdo_set (uint8_t   inst,
                                        uint8_t   node_id,
                                        uint32_t  pdo_id,
                                        uint8_t  *p_data,
                                        uint8_t   length)
{
    uint8_t i = 0;
    uint8_t j = 0;

    tCopKernel ret = kCopSuccessful;

    tAppInstanceInfo MEM* pAppInst;
    tAppInstanceData MEM* pAppInstData;

    if (CcmInitParam_g[inst].m_bInitNodeId == 0) {
        return -AW_COP_ERR_NOTOPEN;
    }

    if (node_id == AW_COP_NODE_ID_INVAILD || node_id > AW_COP_NODE_ID_MAX) {
        return -AW_COP_ERR_INVALIDPARAM;
    }

    pAppInst = &aAppInstanceInfo_g[inst];
    pAppInstData = pAppInstanceData_g[inst];

    if (pAppInst->m_bState != __COP_MST_STATUS_START) {
        return -AW_COP_ERR_NOTSTARTED;
    }

    if (p_data == NULL ||
        length == 0    ||
        length >  8 )  {
        return -AW_COP_ERR_INVALIDPARAM;
    }

    switch(pdo_id & 0x780) {
    case 0x200:
    case 0x300:
    case 0x400:
    case 0x500:
        break;
    /*
     * pdo_type which does not falls into above values is considered illegal
     */
    default:
        return -AW_COP_ERR_INVALIDPARAM;
    }

    j = pAppInst->m_wPdoTabCount;

    for (i = 0; i < j; i++) {

        if ((pAppInst->m_pPdoTab[i].m_dwCanId & ~0xC000007f) ==
                (pdo_id & ~0xC000007f)) {
            break;
        }
    }
    if (i == j) {
        return -AW_COP_ERR_INTERNAL;
    }
    
    /* 
     * Set PDO trans. parameter
     */
    pAppInst->m_pPdoTab[i].m_bValidFlag = kPdoValidAll;
    pAppInst->m_pPdoTab[i].m_bPdoType   = kPdoTypeSend;
    pAppInst->m_pPdoTab[i].m_bTxTyp     = 255;
               
    if(length > PDO_MAX_MAPPENTRIES) {
        length = PDO_MAX_MAPPENTRIES;
    }

    if((pAppInst->m_pPdoTab[i].m_bNrOfEntries != length) ||
       (pAppInst->m_pPdoTab[i].m_dwCanId      != pdo_id)) {
        
        pAppInst->m_pPdoTab[i].m_dwCanId      = (pdo_id & 0x700007FF);
        pAppInst->m_pPdoTab[i].m_bNrOfEntries = length;
#if (COP_MAX_INSTANCES > 1)
        ret = CcmDefinePdoTab(pAppInst->m_bInstanceHandle,
                              &pAppInst->m_pPdoTab[i], 1);
#else
        ret = CcmDefinePdoTab(&pAppInst->m_pPdoTab[i], 1);
#endif
        
        if (ret != kCopSuccessful){
            return -AW_COP_ERR_INTERNAL;
        }   
    }    

    /*
     * to prevent CCM module process collision,following process
     * should be protected by entering critical section
     */
    for(j = 0; j < length; j++) {
        pAppInstData->p_digit_in_8bit_tab[j] = (uint8_t)*(p_data + j);
    }

#if (COP_MAX_INSTANCES > 1)
    ret = CcmSendPdo(pAppInst->m_bInstanceHandle,
              (uint16_t)(pAppInst->m_pPdoTab[i].m_wPdoCommuIndex));
#else

    ret = CcmSendPdo((uint16_t)(pAppInst->m_pPdoTab[i].m_wPdoCommuIndex));
#endif

    if (ret != kCopSuccessful){
        return -AW_COP_ERR_INVALIDPARAM;
    }
    return AW_COP_ERR_NO_ERROR;
}

/******************************************************************************/
aw_cop_err_t aw_cop_mst_remote_pdo_send (
    uint8_t inst, uint8_t node_id, uint32_t pdo_id)
{
    uint8_t i = 0;
    uint8_t j = 0;

    tCopKernel ret = kCopSuccessful;

    tAppInstanceInfo MEM* pAppInst;

    if (CcmInitParam_g[inst].m_bInitNodeId == 0) {
        return -AW_COP_ERR_NOTOPEN;
    }

    if (node_id == AW_COP_NODE_ID_INVAILD || node_id > AW_COP_NODE_ID_MAX) {
        return -AW_COP_ERR_INVALIDPARAM;
    }

    pAppInst = &aAppInstanceInfo_g[inst];

    if (pAppInst->m_bState != __COP_MST_STATUS_START) {
        return -AW_COP_ERR_NOTSTARTED;
    }

    switch(pdo_id & 0x780) {

    case 0x180:
    case 0x280:
    case 0x380:
    case 0x480:
        break;
    /*
     * pdo_type which does not falls into above values is considered illegal
     */
    default:
        return -AW_COP_ERR_INVALIDPARAM;
    }

    j = pAppInst->m_wPdoTabCount - 4;

    for (i = 0; i < j; i++) {
        if ((pAppInst->m_pPdoTab[i].m_bNodeID == (node_id & 0x7f)) &&
           ((pAppInst->m_pPdoTab[i].m_dwCanId & ~0xC000007f) ==
                   (pdo_id & ~0xC000007f))) {
            break;
        }
    }

    if (i == j) {
        return -AW_COP_ERR_PDONOTREGISTER;
    }
    
    /* 
     * Set PDO trans. parameter
     */
    pAppInst->m_pPdoTab[i].m_bValidFlag   = kPdoValidAll;
    pAppInst->m_pPdoTab[i].m_bPdoType     = kPdoTypeSend;
    pAppInst->m_pPdoTab[i].m_bTxTyp       = 255;
    pAppInst->m_pPdoTab[i].m_bNrOfEntries = 0;
    pAppInst->m_pPdoTab[i].m_dwCanId      = (pdo_id & 0x700007FF);
#if (COP_MAX_INSTANCES > 1)
    ret = CcmDefinePdoTab(pAppInst->m_bInstanceHandle,
            (tPdoParam GENERIC*)&pAppInst->m_pPdoTab[i], 1);

#else
    ret = CcmDefinePdoTab((tPdoParam GENERIC*)&pAppInst->m_pPdoTab[i], 1);
#endif
    
    if (ret != kCopSuccessful){
        return -AW_COP_ERR_INTERNAL;
    }   
       
    /*
     * to prevent CCM module process collision,following process
     * should be protected by entering critical section
     */
#if (COP_MAX_INSTANCES > 1)
    ret = CcmSendPdo(pAppInst->m_bInstanceHandle,
            (uint16_t)(pAppInst->m_pPdoTab[i].m_wPdoCommuIndex));
#else
    ret = CcmSendPdo((uint16_t)(pAppInst->m_pPdoTab[i].m_wPdoCommuIndex));
#endif
    
    if (ret != kCopSuccessful){
        return -AW_COP_ERR_SENDFAILED;
    }

    return AW_COP_ERR_NO_ERROR;
}

/******************************************************************************/
aw_cop_err_t aw_cop_mst_node_status_set (
    uint8_t inst, uint8_t node_id, aw_cop_mst_node_status_t status)
{
    uint32_t ret;

    tAppInstanceInfo MEM* pAppInst;

    if (inst >= COP_MAX_INSTANCES) {
        return -AW_COP_ERR_NOTEXIST;
    }

    if (node_id == AW_COP_NODE_ID_INVAILD || node_id > AW_COP_NODE_ID_MAX) {
        return -AW_COP_ERR_INVALIDPARAM;
    }

    pAppInst = &aAppInstanceInfo_g[inst];

    if (pAppInst->m_bState != __COP_MST_STATUS_START) {
        return -AW_COP_ERR_NOTSTARTED;
    }

    switch (status) {

    case AW_COP_MST_NODE_STATUS_CMD_START:
    case AW_COP_MST_NODE_STATUS_CMD_STOP:
    case AW_COP_MST_NODE_STATUS_CMD_PRE_OPERATIONAL:
    case AW_COP_MST_NODE_STATUS_CMD_RESET:
    case AW_COP_MST_NODE_STATUS_CMD_RESET_CONNECTION:
        break;
    default:
        return -AW_COP_ERR_INVALIDPARAM;
    }

#if (COP_MAX_INSTANCES > 1)
    ret = CcmSendNmtCommand (pAppInst->m_bInstanceHandle,
                            (uint8_t)node_id,
                            (tNmtCommand)status);
#else
    ret = CcmSendNmtCommand ((uint8_t)node_id, (tNmtCommand)status);
#endif

    if (ret != kCopSuccessful){
        return -AW_COP_ERR_INTERNAL;
    }

    return AW_COP_ERR_NO_ERROR;
}

/******************************************************************************/
aw_cop_err_t aw_cop_mst_node_status_get (
    uint8_t inst, uint8_t node_id, aw_cop_mst_node_status_t *p_status)
{
    uint32_t        ret;
    static uint8_t  ucStateToggle = 0;
    tNmtmSlaveInfo  SlaveInfo;
    tNodeState      node_state;

    tAppInstanceInfo MEM* pAppInst;

    if (inst >= COP_MAX_INSTANCES) {
        return -AW_COP_ERR_NOTEXIST;
    }

    if (node_id == AW_COP_NODE_ID_INVAILD || node_id > AW_COP_NODE_ID_MAX) {
        return -AW_COP_ERR_INVALIDPARAM;
    }

    pAppInst = &aAppInstanceInfo_g[inst];

    if (pAppInst->m_bState != __COP_MST_STATUS_START) {
        return -AW_COP_ERR_NOTSTARTED;
    }

    //CcmTriggerNodeGuard((uint8_t)node_id);
#if (COP_MAX_INSTANCES > 1)
    ret = CcmNmtmGetSlaveInfo(
        pAppInst->m_bInstanceHandle, (uint8_t)node_id, &SlaveInfo);
#else
    ret = CcmNmtmGetSlaveInfo(
        (uint8_t)node_id, &SlaveInfo);
#endif

    if(ret != kCopSuccessful) {
        return -AW_COP_ERR_GETSTATUS;
    }

    if (0 == (SlaveInfo.m_bSlaveState & kNmtmStateLgActive)) {
       __COP_MST_DLY(10);
    }

#if (COP_MAX_INSTANCES > 1)
    ret = CcmNmtmGetSlaveInfo(
        pAppInst->m_bInstanceHandle, (uint8_t)node_id, &SlaveInfo);
#else
    ret = CcmNmtmGetSlaveInfo((uint8_t)node_id, &SlaveInfo);
#endif

    if (ret != kCopSuccessful){
        return -AW_COP_ERR_GETSTATUS;
    }

    /* lifeguard active */
    if ((SlaveInfo.m_bSlaveState & kNmtmStateLgActive) ||
        (SlaveInfo.m_bSlaveState & kNmtmStateNgActive) ||
        (SlaveInfo.m_bSlaveState & kNmtmUpdate      )) {

        if((SlaveInfo.m_bSlaveState & kNmtmLgReceived) ||
           (SlaveInfo.m_bSlaveState & kNmtmUpdate    ) ||
           (ucStateToggle != (SlaveInfo.m_bSlaveState & kNmtmStateToggleBit))) {
            ucStateToggle = SlaveInfo.m_bSlaveState & kNmtmStateToggleBit;
            *p_status = SlaveInfo.m_bSlaveNodeState;
            return AW_COP_ERR_NO_ERROR;
        }
        *p_status = AW_COP_MST_NODE_STATUS_DISCONNECT;
        return AW_COP_ERR_NO_ERROR;
    }

    /* heartbeat active */
#if (COP_MAX_INSTANCES > 1)
    ret = CcmHbcGetNodeState(
        pAppInst->m_bInstanceHandle, (uint8_t)node_id, &node_state);
#else
    ret = CcmHbcGetNodeState((uint8_t)node_id, &node_state);
#endif

    if (ret != kCopSuccessful) {
        return  -AW_COP_ERR_GETSTATUS;
    }

    *p_status = node_state;
    return AW_COP_ERR_NO_ERROR;
}

/******************************************************************************/
aw_cop_err_t aw_cop_mst_sync_prodcer_cfg (
    uint8_t inst, uint32_t sync_id, uint16_t cyc_time)
{
    uint32_t ret;

    tAppInstanceInfo MEM* pAppInst;

    if (inst >= COP_MAX_INSTANCES) {
        return -AW_COP_ERR_NOTEXIST;
    }
    pAppInst = &aAppInstanceInfo_g[inst];
    if (pAppInst->m_bState != __COP_MST_STATUS_START) {
        return -AW_COP_ERR_NOTSTARTED;
    }

    if (AW_COP_ID_SYNC_CLEAR == sync_id) {
        cyc_time = 0;
    }

#if (COP_MAX_INSTANCES > 1)
    ret = CcmConfigSyncProducer(
        pAppInst->m_bInstanceHandle, sync_id, cyc_time * 1000);
#else
    ret = CcmConfigSyncProducer(sync_id, cyc_time * 1000);
#endif

    if (ret != kCopSuccessful){
        return -AW_COP_ERR_INTERNAL;
    }

    return AW_COP_ERR_NO_ERROR;
}

/******************************************************************************/
aw_cop_err_t aw_cop_mst_timestamp_send (uint8_t inst, aw_cop_time_t *p_time)
{
    uint32_t      ret;
    INT           result;

    tTimeOfDay    time_stamp;
    aw_cop_time_t since_time = {1984, 1 , 1};

    tAppInstanceInfo MEM* pAppInst;

    if (inst >= COP_MAX_INSTANCES) {
        return -AW_COP_ERR_NOTEXIST;
    }
    pAppInst = &aAppInstanceInfo_g[inst];

    if (pAppInst->m_bState != __COP_MST_STATUS_START) {
        return -AW_COP_ERR_NOTSTARTED;
    }

    if((p_time->year        < 1984) ||
       (p_time->hour        > 23)   ||
       (p_time->minute      > 59)   ||
       (p_time->second      > 59)   ||
       (p_time->millisecond > 999)) {
        return -AW_COP_ERR_INVALIDPARAM;
    }

    result = __cal_days(&since_time, p_time);
    if (result < 0) {
        return -AW_COP_ERR_INVALIDPARAM;
    }
    time_stamp.m_wDays = (uint16_t)result;
    time_stamp.m_dwMs  = p_time->hour   * 3600000 +
                         p_time->minute * 60000   +
                         p_time->second * 1000    +
                         p_time->millisecond;
    time_stamp.m_dwMs &= 0x0FFFFFFF;

#if (COP_MAX_INSTANCES > 1)
    ret = CcmTspSend(
        pAppInst->m_bInstanceHandle, AW_COP_ID_TIME_STAMP, &time_stamp);
#else
    ret = CcmTspSend(AW_COP_ID_TIME_STAMP, &time_stamp);
#endif

    if (ret != kCopSuccessful){
        return -AW_COP_ERR_INVALIDPARAM;
    }

    return AW_COP_ERR_NO_ERROR;
}

/******************************************************************************/
void aw_cop_mst_process (uint8_t inst)
{
    if (inst >= COP_MAX_INSTANCES) {
        return;
    }
    
#if (COP_MAX_INSTANCES > 1)
    CcmProcess(aAppInstanceInfo_g[inst].m_bInstanceHandle);
#else
    CcmProcess();
#endif
}

/******************************************************************************/
aw_err_t aw_cop_init (
    int                         inst_cnt,
    const aw_hwtimer_handle_t   p_hwtimer,
    aw_cop_data_t              **p_inst_data,
    const aw_cop_brt_list_t    *p_chn_brt_list)
{
    int      i, j;
    aw_err_t ret;

    if (p_hwtimer == NULL || p_inst_data == NULL || p_chn_brt_list == NULL) {
        ret = -AW_EINVAL;
        goto exit;
    }

    if (inst_cnt != AW_NELEMENTS(pAppInstanceData_g)) {
        ret = -AW_EINVAL;
        goto exit;
    }

    aw_hwtimer_enable(p_hwtimer, 10000);

    for (i = 0; i < inst_cnt; i++) {
        if (p_inst_data[i] == NULL) {
            ret = -AW_EINVAL;
            goto exit;
        }
        pAppInstanceData_g[i] = p_inst_data[i];
        aCanChn_g[i] = p_chn_brt_list[i].chn;

        for (j = 0; j < AW_COP_BRT_LIST_CNT; j++) {
            aBrtTable_g[i][cop_baud_index_get(p_chn_brt_list[i].brt[j].val)] =
                p_chn_brt_list[i].brt[j].param;
        }
    }

    AW_MUTEX_INIT(__g_cop_comm_mutex, AW_SEM_Q_FIFO);

exit:
    return ret;
}



