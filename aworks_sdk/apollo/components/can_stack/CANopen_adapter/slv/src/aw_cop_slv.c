

#include "apollo.h"
#include "aw_sem.h"
#include "aw_can.h"
#include "aw_cop_slv.h"
#include "slv/inc/aw_cop_cfg.h"

#include "copinc.h"
#include "ccmmain.h"
#include "objdicts_slv/obdcfg.h"
#include "objdef.h"
#include "ccmdyn.h"

//---------------------------------------------------------------------------
// 类型定义
//---------------------------------------------------------------------------
typedef struct __cop_inst_info {

    tCopInstanceHdl      inst_handle_num;
    aw_bool_t            is_used;
    aw_bool_t            is_dyn_od_build;

    tProcessImageDscrpt  pi_dscrpt;

    uint8_t             *p_seg_cont;
    uint8_t              seg_cont_size;

    aw_bool_t            dyn_od_init;

} __cop_inst_info_t;

//---------------------------------------------------------------------------
// 全局变量定义
//---------------------------------------------------------------------------

static tCcmInitParam       __g_cop_ccm_init_param[COP_MAX_INSTANCES];
static __cop_inst_info_t   __g_cop_inst_info[COP_MAX_INSTANCES];
static BYTE                __g_cop_can_chn[COP_MAX_INSTANCES];
static aw_can_baud_param_t
__g_cop_brt_tab[COP_MAX_INSTANCES][AW_COP_BRT_LIST_CNT];

AW_MUTEX_DECL_STATIC(__g_cop_comm_mutex);

//---------------------------------------------------------------------------
// 申明
//---------------------------------------------------------------------------

// This function is the entry point for your object dictionary. It is defined
// in OBJDICT.C by define OBD_INIT_RAM_NAME. Use this function name to define
// this function prototype here. If you want to use more than one CANopen
// instances then the function name of each object dictionary has to differ.
tCopKernel PUBLIC ObdInitRam  (void MEM* pInitParam_p);


/******************************************************************************/
static uint8_t __cop_inst_get (CCM_DECL_INSTANCE_HDL)
{
    int i = 0;

#if (COP_MAX_INSTANCES > 1)
    for (i = 0; i < AW_NELEMENTS(__g_cop_inst_info); i++) {
        if (__g_cop_inst_info[i].is_used == AW_TRUE &&
            __g_cop_inst_info[i].inst_handle_num == InstanceHandle) {
            break;
        }
    }

    if (i > AW_NELEMENTS(__g_cop_inst_info)) {
        i = CCM_ILLINSTANCE_HDL;
    }
#endif

    return i;
}

//---------------------------------------------------------------------------
//
// Function:    __cop_build_dyn_od()
//
// Description: function builds the dynamic OD
//
// Parameters:  p_cop_inst
//
// Returns:     tCopKernel = error code
//                  kCopSuccessful
//                  from other functions to configure CANopen modules
//
//---------------------------------------------------------------------------
tCopKernel __cop_build_dyn_od (__cop_inst_info_t *p_cop_inst)
{
    tCopKernel ret = kCopSuccessful;

#if (COP_MAX_INSTANCES > 1)
    // build the dynamic part of the OD
    ret = DynBuildOd (p_cop_inst->inst_handle_num,
        (BYTE FAR*)p_cop_inst->p_seg_cont, &p_cop_inst->pi_dscrpt);
#else
    // build the dynamic part of the OD
        ret = DynBuildOd (
            (BYTE FAR*)p_cop_inst->p_seg_cont, &p_cop_inst->pi_dscrpt);
#endif

    TRACE1 ("AppCbAccessDynOd(): DynBuildOd() returned 0x%X\n", ret);

    return ret;
}


//---------------------------------------------------------------------------
//
// Function:    __cop_nmt_event_cb()
//
// Description: callback function for NMT events
//              It will be called if NMT state will be changed while
//              initializing CANopen stack or after receiving NMT command
//              (CANopen slave) or sending NMT command including own node
//              (CANopen master).
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              NmtEvent_p              = NMT event which is ocuured
//
// returns:     tCopKernel = error code
//                  kCopSuccessful
//                  from other functions to configure CANopen modules
//
//              NOTE: This error code will be reported in error callback
//                    function CcmCbError() within CcmMain.c using
//                    kCopNmtStateError.
//
//---------------------------------------------------------------------------
tCopKernel PUBLIC __cop_nmt_event_cb (
    CCM_DECL_INSTANCE_HDL_ tNmtEvent nmt_event)
{
    tCopKernel ret = kCopSuccessful;
    uint8_t    inst;

//    __cop_inst_info_t *p_cop_inst;

    inst = __cop_inst_get(CCM_INSTANCE_HDL);

//    p_cop_inst = &__g_cop_inst_info[inst];

    // which event is called?
    switch (nmt_event) {

    // -------------------------------------------------------------------------
    // after Power On initialize all variables
    // Now all variables has to be defined.
    case kNmtEvEnterInitialising:

        PRINTF0 ("init life guard slave...\n");

        // init life guard slave
        ret = CcmInitLgs(CCM_INSTANCE_HDL);
        if (ret != kCopSuccessful) {
            goto Exit;
        }

        /*
         * initialize support for Load/Restore (objects 0x1010 and 0x1011)
         * NOTE: If you do call this function, then do not call CcmDefinePdoTab()
         * and CcmConfigEmcp() in case kNmtEvResetCommunication in this way.
         * Because object data will be read from EEPROM at first and then
         * this callback function will be called with kNmtEvResetCommunication.
         * Please try to use Default Values of OD for PDO and EMCY or check in
         * case kNmtEvResetCommunication the signature in EEPROM.
         * USE CcmStoreCheckArchivState() to check!
         */
#ifndef COP_KIT
#if CCM_USE_STORE_RESTORE == TRUE
        ret = CcmInitStore (CCM_INSTANCE_HDL);
        if (ret != kCopSuccessful) {
            PRINTF0("CcmInitStore failed");
            goto Exit;
        }
#endif
#endif
//        PRINTF0 ("define all process variables...\n");
//
//        // define all variables for CANopen
//        ret = CcmDefineVarTab (CCM_INSTANCE_HDL_
//            (tVarParam GENERIC*) &aVarTab_g[0], sizeof (aVarTab_g) / sizeof (tVarParam));
//        if (ret != kCopSuccessful) {
//            goto Exit;
//        }
        break;

    // -------------------------------------------------------------------------
    // All application obejcts (0x2000-0x9FFF) has been reseted.
    case kNmtEvResetNode:
        break;

    // -------------------------------------------------------------------------
    // All communication obejcts (0x1000-0x1FFF) has been reseted.
    // Now all PDOs and SDOs has to be defined.
    case kNmtEvResetCommunication:

//#ifndef COP_KIT
//         check if there are valid data from non-volatile memory
//         in this case we should not overwrite PDO and EMCP settings
//        ret = CcmStoreCheckArchivState (CCM_INSTANCE_HDL_ kObdPartGen);
//        if (ret != kCopSuccessful)
//#endif
        {
//            PRINTF0 ("Define all communication and mappimg parameter\n");
//
//            // define all PDOs for CANopen
//            ret = CcmDefinePdoTab (CCM_INSTANCE_HDL_
//                (tPdoParam GENERIC*) &aPdoTab_g[0],
//                sizeof (aPdoTab_g) / sizeof (tPdoParam));
//
//            if (ret != kCopSuccessful) {
//                goto Exit;
//            }

            PRINTF0 ("All communication and mappimg parameter are defined\n");

            // configure emergency COB ID
            ret = CcmConfigEmcp (CCM_INSTANCE_HDL_
                0x080 + __g_cop_ccm_init_param[inst].m_bInitNodeId);
        }

#if ((CDRV_USE_HPT & CDRV_HPT_TYPEMASK) == CDRV_HPT_CONSUMER)
        {
            // config SYNC consumer
            ret = CcmConfigSyncConsumer (CCM_INSTANCE_HDL_
                0x00000080, 0);
        }
#endif

#if ((CDRV_USE_HPT & CDRV_HPT_TYPEMASK) == CDRV_HPT_PRODUCER)
        {
            // write the current time stamp to the OD -
            // so TPDO will not be send with value 0x00000000L
            dwHighPresitionTimeStamp_g = TgtHptGetCurrentTime ();

            // config SYNC producer (start to send cycelic message)
            ret = CcmConfigSyncProducer (CCM_INSTANCE_HDL_
                0x40000080, 1000000);
        }
#endif
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
    default:
        break;
    }

Exit:
    if (ret != kCopSuccessful) {
        PRINTF1 ("ERROR: AppCbNmtEvent(): ret = 0x%X\n", (WORD) ret);
    }

    return ret;

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
aw_cop_err_t aw_cop_slv_init (uint8_t inst, aw_cop_slv_info_t *p_info)
{
    tCopKernel ret;
    DWORD      cop_version;

    __cop_inst_info_t *p_cop_inst;
    tCcmInitParam     *p_cop_ccm_init_param;

    static aw_bool_t cop_comm_init_flag = AW_FALSE;

    tCcmInitCall ccm_init_flag;

    if (p_info == NULL) {
        return -AW_COP_ERR_INVALIDPARAM;
    }

    if (inst >= COP_MAX_INSTANCES) {
        return -AW_COP_ERR_NOTEXIST;
    }

    if (p_info->p_seg_cont == NULL || p_info->seg_cont_size == 0) {
        return -AW_COP_ERR_NOSEGCONT;
    }

    /*
     * get current software version of CANopen stack
     */
    cop_version = CopGetStackVersion();

    PRINTF3 ("*************************************************************\n"
            "*              CANopen master demo for aworks               *\n"
            "*************************************************************\n"
            "* Version: %d.%d.%d,  23.04.03                                *\n"
            "*                                                           *\n"
            "* Guangzhou ZLGMCU Development Co.,Ltd.                     *\n"
            "*************************************************************\n\n",
            (WORD) COP_GET_MAJOR_VERSION   (cop_version),
            (WORD) COP_GET_MINOR_VERSION   (cop_version),
            (WORD) COP_GET_RELEASE_VERSION (cop_version));

    /*
     * initiat global variables
     */
    p_cop_inst = &__g_cop_inst_info[inst];
    p_cop_ccm_init_param = &__g_cop_ccm_init_param[inst];

    if (p_cop_inst->is_used == AW_TRUE) {
        return -AW_COP_ERR_OPENED;
    }

    memset(p_cop_inst, 0x00, sizeof(*p_cop_inst));

    p_cop_inst->p_seg_cont = (uint8_t *)malloc(p_info->seg_cont_size);

    if (p_cop_inst->p_seg_cont == NULL) {
        return -AW_COP_ERR_NOMEM;
    }

    memcpy(p_cop_inst->p_seg_cont, p_info->p_seg_cont, p_info->seg_cont_size);

    p_cop_inst->pi_dscrpt.m_pbProcessImage = p_info->p_process_image;
    p_cop_inst->pi_dscrpt.m_dwProcessImageSize = p_info->process_image_size;

    p_cop_ccm_init_param->m_BaudIndex = cop_baud_index_get(p_info->baudrate);
    p_cop_ccm_init_param->m_bInitNodeId   = p_info->node_id;
    p_cop_ccm_init_param->m_pBdiTable     = __g_cop_brt_tab[inst];;
    p_cop_ccm_init_param->m_wSizeOfBdiTab = sizeof(__g_cop_brt_tab[inst]);
    p_cop_ccm_init_param->m_dwAmr         = 0xFFFFFFFFL;
    p_cop_ccm_init_param->m_dwAcr         = 0x00000000L;

    p_cop_ccm_init_param->m_HwParam.m_ZeroBasedCanNo.m_bCanNo =
        __g_cop_can_chn[inst];

    p_cop_ccm_init_param->m_fpCanInterruptEnable = TgtEnableCanInterrupt;
    p_cop_ccm_init_param->m_fpNmtEventCallback   = __cop_nmt_event_cb;
    p_cop_ccm_init_param->m_fpObdInitRam         = ObdInitRam;

    AW_MUTEX_LOCK(__g_cop_comm_mutex, AW_SEM_WAIT_FOREVER);
#if (COP_MAX_INSTANCES > 1)
    if (!cop_comm_init_flag) {
        cop_comm_init_flag = TRUE;
        ccm_init_flag = kCcmFirstInstance;
    } else {
        ccm_init_flag = kCcmAdditionalInstance;
    }
    ret = CcmInitCANopen(&p_cop_inst->inst_handle_num,
                         p_cop_ccm_init_param,
                         ccm_init_flag);

#else
    ret = CcmInitCANopen(p_cop_ccm_init_param, kCcmFirstInstance);
#endif
    AW_MUTEX_UNLOCK(__g_cop_comm_mutex);

    if (ret != kCopSuccessful) {
        return -AW_COP_ERR_LOADLIB;
    }

    p_cop_inst ->is_used = AW_TRUE;

    ret = __cop_build_dyn_od(p_cop_inst);
    if (ret != kCopSuccessful) {
        aw_cop_slv_deinit(inst);
        return -AW_COP_ERR_INTERNAL;
    }

    p_cop_inst ->is_dyn_od_build = AW_TRUE;

    return AW_COP_ERR_NO_ERROR;
}

/******************************************************************************/
aw_cop_err_t aw_cop_slv_deinit (uint8_t inst)
{
    __cop_inst_info_t *p_cop_inst;

    if (inst >= COP_MAX_INSTANCES) {
        return -AW_COP_ERR_NOTEXIST;
    }

    p_cop_inst = &__g_cop_inst_info[inst];

#if (COP_MAX_INSTANCES > 1)
    if (p_cop_inst ->is_used == AW_TRUE) {
        CcmShutDownCANopen(p_cop_inst->inst_handle_num);

        if (p_cop_inst ->is_dyn_od_build == AW_TRUE) {
            DynDestroyOd(p_cop_inst->inst_handle_num);
        }
    }
#else
    if (p_cop_inst ->is_used == AW_TRUE) {
        CcmShutDownCANopen();

        if (p_cop_inst ->is_dyn_od_build == AW_TRUE) {
            DynDestroyOd();
        }
    }
#endif

    free(p_cop_inst->p_seg_cont);

    p_cop_inst ->is_used = AW_FALSE;
    p_cop_inst ->is_dyn_od_build = AW_FALSE;

    COP_MEMSET(&__g_cop_ccm_init_param[inst], 0, sizeof(tCcmInitParam));

    return AW_COP_ERR_NO_ERROR;
}

/******************************************************************************/
aw_cop_err_t aw_cop_slv_connect_net (uint8_t inst)
{
    tCopKernel ret;

    __cop_inst_info_t *p_cop_inst;

    if (inst >= COP_MAX_INSTANCES) {
        return -AW_COP_ERR_NOTEXIST;
    }

    p_cop_inst = &__g_cop_inst_info[inst];
#if (COP_MAX_INSTANCES > 1)
    ret = CcmConnectToNet(p_cop_inst->inst_handle_num);
#else
    ret = CcmConnectToNet();
#endif

    if (ret != kCopSuccessful) {
        return -AW_COP_ERR_SLAVENOTEXIST;
    }
    return AW_COP_ERR_NO_ERROR;
}

/******************************************************************************/
aw_cop_err_t aw_cop_slv_boot (uint8_t inst)
{
    tCopKernel ret;

    __cop_inst_info_t *p_cop_inst;

    if (inst >= COP_MAX_INSTANCES) {
        return -AW_COP_ERR_NOTEXIST;
    }

    p_cop_inst = &__g_cop_inst_info[inst];
#if (COP_MAX_INSTANCES > 1)
    ret = CcmBootNetwork(p_cop_inst->inst_handle_num);
#else
    ret = CcmBootNetwork();
#endif

    if (ret != kCopSuccessful) {
        return -AW_COP_ERR_SLAVENOTEXIST;
    }
    return AW_COP_ERR_NO_ERROR;
}

/******************************************************************************/
aw_cop_err_t aw_cop_slv_status_set (
    uint8_t inst, aw_cop_slv_status_cmd_t status)
{
    tCopKernel ret;

    __cop_inst_info_t *p_cop_inst;

    if (inst >= COP_MAX_INSTANCES) {
        return -AW_COP_ERR_NOTEXIST;
    }

    p_cop_inst = &__g_cop_inst_info[inst];
#if (COP_MAX_INSTANCES > 1)
    ret = CcmNmtsExecCommand(p_cop_inst->inst_handle_num, status);
#else
    ret = CcmNmtsExecCommand(status);
#endif

    if (ret != kCopSuccessful) {
        return -AW_COP_ERR_SLAVENOTEXIST;
    }
    return AW_COP_ERR_NO_ERROR;
}

/******************************************************************************/
aw_cop_err_t aw_cop_slv_status_get (
    uint8_t inst, aw_cop_slv_status_t *p_status)
{
    __cop_inst_info_t *p_cop_inst;

    if (inst >= COP_MAX_INSTANCES) {
        return -AW_COP_ERR_NOTEXIST;
    }

    p_cop_inst = &__g_cop_inst_info[inst];
#if (COP_MAX_INSTANCES > 1)
    *p_status = (aw_cop_slv_status_t)CcmObdGetNodeState(
        p_cop_inst->inst_handle_num);
#else
    *p_status = (aw_cop_slv_status_t)CcmObdGetNodeState();
#endif

    return AW_COP_ERR_NO_ERROR;
}

/******************************************************************************/
void aw_cop_slv_process (uint8_t inst)
{
    if (inst >= COP_MAX_INSTANCES) {
        return;
    }

#if (COP_MAX_INSTANCES > 1)
    CcmProcess(__g_cop_inst_info[inst].inst_handle_num);
#else
    CcmProcess();
#endif
}

/******************************************************************************/
aw_cop_err_t aw_cop_slv_obd_read (uint8_t   inst,
                                  uint16_t  index,
                                  uint8_t   subindex,
                                  uint8_t  *p_data,
                                  uint32_t *p_length)
{
    tObdSize   size;
    tCopKernel ret;

    __cop_inst_info_t *p_cop_inst;

    if (p_data == NULL || p_length == NULL) {
        return -AW_COP_ERR_INVALIDPARAM;
    }

    if (inst >= COP_MAX_INSTANCES) {
        return -AW_COP_ERR_NOTEXIST;
    }

    size = *p_length;

    p_cop_inst = &__g_cop_inst_info[inst];
#if (COP_MAX_INSTANCES > 1)
    ret = CcmReadObject(
        p_cop_inst->inst_handle_num, index, subindex, p_data, &size);
#else
    ret = CcmReadObject(index, subindex, p_data, &size);
#endif

    if (ret != kCopSuccessful) {
        return -AW_COP_ERR_INTERNAL;
    }

    *p_length = size;

    return AW_COP_ERR_NO_ERROR;
}

/******************************************************************************/
aw_cop_err_t aw_cop_slv_obd_write (uint8_t   inst,
                                   uint16_t  index,
                                   uint8_t   subindex,
                                   uint8_t  *p_data,
                                   uint32_t  length)
{
    tCopKernel ret;

    __cop_inst_info_t *p_cop_inst;

    if (p_data == NULL) {
        return -AW_COP_ERR_INVALIDPARAM;
    }

    if (inst >= COP_MAX_INSTANCES) {
        return -AW_COP_ERR_NOTEXIST;
    }

    p_cop_inst = &__g_cop_inst_info[inst];
#if (COP_MAX_INSTANCES > 1)
    ret = CcmWriteObject(
        p_cop_inst->inst_handle_num, index, subindex, p_data, length);
#else
    ret = CcmWriteObject(index, subindex, p_data, length);
#endif

    if (ret != kCopSuccessful) {
        return -AW_COP_ERR_INTERNAL;
    }

    return AW_COP_ERR_NO_ERROR;
}

/******************************************************************************/
aw_cop_err_t aw_cop_slv_tpdo_send (uint8_t   inst,
                                   uint32_t  offset,
                                   uint8_t  *p_data,
                                   uint32_t  length)
{
    __cop_inst_info_t   *p_cop_inst;
    tProcessImageDscrpt *p_pi_dscrpt;

    if (p_data == NULL || p_data == NULL) {
        return -AW_COP_ERR_INVALIDPARAM;
    }

    if (inst >= COP_MAX_INSTANCES) {
        return -AW_COP_ERR_NOTEXIST;
    }

    p_cop_inst = &__g_cop_inst_info[inst];
    p_pi_dscrpt = &p_cop_inst->pi_dscrpt;

    if (offset >= p_pi_dscrpt->m_dwDynPIOffsetOut) {
        return -AW_COP_ERR_OFFSET;
    }

    if ((offset + length) >  p_pi_dscrpt->m_dwDynPISizeOut) {
        return -AW_COP_ERR_LENGTH;
    }

    offset += p_pi_dscrpt->m_dwDynPIOffsetOut;
    memcpy(&p_pi_dscrpt->m_pbProcessImage[offset], p_data, length);

    return AW_COP_ERR_NO_ERROR;
}

/******************************************************************************/
aw_cop_err_t aw_cop_slv_rpdo_read (uint8_t   inst,
                                   uint32_t  offset,
                                   uint8_t  *p_data,
                                   uint32_t  length)
{
    __cop_inst_info_t   *p_cop_inst;
    tProcessImageDscrpt *p_pi_dscrpt;

    if (p_data == NULL || p_data == NULL) {
        return -AW_COP_ERR_INVALIDPARAM;
    }

    if (inst >= COP_MAX_INSTANCES) {
        return -AW_COP_ERR_NOTEXIST;
    }

    p_cop_inst = &__g_cop_inst_info[inst];
    p_pi_dscrpt = &p_cop_inst->pi_dscrpt;

    if (offset >= p_pi_dscrpt->m_dwDynPISizeIn) {
        return -AW_COP_ERR_OFFSET;
    }

    if ((offset + length) > p_pi_dscrpt->m_dwDynPISizeIn) {
        return -AW_COP_ERR_LENGTH;
    }

    offset += p_pi_dscrpt->m_dwDynPIOffsetIn;
    memcpy(p_data, &p_pi_dscrpt->m_pbProcessImage[offset], length);

    return AW_COP_ERR_NO_ERROR;
}

/******************************************************************************/
aw_err_t aw_cop_init (
    int                         inst_cnt,
    const aw_hwtimer_handle_t   p_hwtimer,
    const aw_cop_brt_list_t    *p_chn_brt_list)
{
    int      i, j;
    aw_err_t ret;

    if (p_hwtimer == NULL || p_chn_brt_list == NULL) {
        ret = -AW_EINVAL;
        goto exit;
    }

    aw_hwtimer_enable(p_hwtimer, 10000);

    for (i = 0; i < inst_cnt; i++) {

        __g_cop_can_chn[i] = p_chn_brt_list[i].chn;

        for (j = 0; j < AW_COP_BRT_LIST_CNT; j++) {
            __g_cop_brt_tab[i][cop_baud_index_get(
                p_chn_brt_list[i].brt[j].val)] = p_chn_brt_list[i].brt[j].param;
        }
    }

    AW_MUTEX_INIT(__g_cop_comm_mutex, AW_SEM_Q_FIFO);

exit:
    return ret;
}
