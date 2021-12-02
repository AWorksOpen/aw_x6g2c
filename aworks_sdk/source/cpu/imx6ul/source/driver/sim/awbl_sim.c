/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief AWBus IMX6UL SIM interface
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * #include "awbl_sim.h"
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 16-06-12  win, first implementation
 * \endinternal
 */

#include "apollo.h"
#include "aw_crc.h"

#include "driver/sim/awbl_sim.h"

#include <string.h>

/*******************************************************************************
  define
*******************************************************************************/

/**
 * \name I块PCB字节编码
 * @{
 */

#define __AWBL_SIM_T1_PCB_I_BLOCK  (0x0 << 7)  /**< \brief I块 */

#define __AWBL_SIM_T1_PCB_I_N      (0x0 << 6)  /**< \brief I块发送序列数为N */
#define __AWBL_SIM_T1_PCB_I_S       AW_BIT(6)  /**< \brief I块发送序列数为S */
#define __AWBL_SIM_T1_PCB_I_M       AW_BIT(5)  /**< \brief 数据分链 */

/** @} */

/**
 * \name R块PCB字节编码
 * @{
 */

#define __AWBL_SIM_T1_PCB_R_BLOCK       AW_SBF(0x2, 6) /**< \brief R块 */

#define __AWBL_SIM_T1_PCB_R_ERR_NO      AW_SBF(0x0, 0) /**< \brief 无差错 */
#define __AWBL_SIM_T1_PCB_R_ERR_EDC     AW_SBF(0x1, 0) /**< \brief EDC错误  */
#define __AWBL_SIM_T1_PCB_R_ERR_OTHERS  AW_SBF(0x2, 0) /**< \brief 其他错误 */

/** @} */

/**
 * \name S块PCB字节编码
 * @{
 */

#define __AWBL_SIM_T1_PCB_S_BLOCK      AW_SBF(0x3, 6)  /**< \brief S块 */

#define __AWBL_SIM_T1_PCB_S_RES_REQ    AW_SBF(0x0, 0)  /**< \brief RESYNCH请求 */
#define __AWBL_SIM_T1_PCB_S_RES_ACK    AW_SBF(0x1, 5)  /**< \brief RESYNCH应答 */
#define __AWBL_SIM_T1_PCB_S_IFS_REQ    AW_SBF(0x1, 0)  /**< \brief IFS请求 */
#define __AWBL_SIM_T1_PCB_S_IFS_ACK    0x21            /**< \brief IFS应答 */
#define __AWBL_SIM_T1_PCB_S_ABORT_REQ  AW_SBF(0x1, 1)  /**< \brief ABORT请求 */
#define __AWBL_SIM_T1_PCB_S_ABORT_ACK  0x22            /**< \brief ABORT应答 */
#define __AWBL_SIM_T1_PCB_S_WTX_REQ    AW_SBF(0x3, 0)  /**< \brief WTX请求 */
#define __AWBL_SIM_T1_PCB_S_WTX_ACK    0x23            /**< \brief WTX响应 */
#define __AWBL_SIM_T1_PCB_S_VPP_ERR    0x24            /**< \brief VPP状态错误 */

/** @} */

/*******************************************************************************
  locals
*******************************************************************************/

AWBL_METHOD_DEF(awbl_simserv_get, "awbl_simserv_get");

/* pointer to first sim service */
struct awbl_sim_service *__gp_sim_serv_head = NULL;

/*******************************************************************************
  implementation
*******************************************************************************/

/**
 * \brief allocate sim service from instance (helper)
 *
 * \param[in]       p_dev   the current processing  instance
 * \param[in,out]   p_arg   argument
 *
 * \retval          AW_OK
 *
 * \see awbl_alldevs_method_run()
 */
aw_local aw_err_t __sim_serv_alloc_helper (struct awbl_dev *p_dev, void *p_arg)
{
    awbl_method_handler_t     pfunc_sim_serv = NULL;
    struct awbl_sim_service  *p_sim_serv     = NULL;

    /* find handler */
    pfunc_sim_serv = awbl_dev_method_get(p_dev,
                                         AWBL_METHOD_CALL(awbl_simserv_get));

    /* call method handler to allocate sim service */
    if (pfunc_sim_serv != NULL) {

        pfunc_sim_serv(p_dev, &p_sim_serv);

        /* found a sim service, insert it to the service list */
        if (p_sim_serv != NULL) {
            struct awbl_sim_service **pp_serv_cur = &__gp_sim_serv_head;

            while (*pp_serv_cur != NULL) {
                pp_serv_cur = &(*pp_serv_cur)->p_next;
            }

            *pp_serv_cur = p_sim_serv;
            p_sim_serv->p_next = NULL;
        }
    }

    return AW_OK;   /* iterating continue */
}

/**
 * \brief allocate sim services from instance tree
 */
aw_local aw_err_t __sim_serv_alloc (void)
{
    /* todo: count the number of service */

    /* Empty the list head, as every sim device will be find */
    __gp_sim_serv_head = NULL;

    awbl_dev_iterate(__sim_serv_alloc_helper, NULL, AWBL_ITERATE_INSTANCES);

    return AW_OK;
}

/**
 * \brief find out a service who accept the id
 */
aw_local struct awbl_sim_service * __sim_id_to_serv (int id)
{
    struct awbl_sim_service *p_serv_cur = __gp_sim_serv_head;

    while ((p_serv_cur != NULL)) {

        if (id == p_serv_cur->p_servinfo->dev_id) {

            return p_serv_cur;
        }

        p_serv_cur = p_serv_cur->p_next;
    }

    return NULL;
}

/**
 * \brief AWBus sim 接口模块初始化
 *
 * \attention 本函数应当在 awbl_dev_init2() 之后调用
 */
void awbl_sim_init (void)
{
    /* allocate sim services from instance tree */
    __sim_serv_alloc();
}


/**
 * \brief 计算LRC校验值
 *
 * \param[in] p_buf  : 存放数据的缓冲区
 * \param[in] nbytes : 数据的字节数
 *
 * \return LRC校验值
 */
aw_local uint32_t __awbl_sim_lrc_calc (const void *p_buf, uint32_t nbytes)
{
    uint32_t  i;
    uint32_t  lrc = 0;
    uint8_t  *p   = (uint8_t *)p_buf;

    if (p_buf == NULL || nbytes == 0) {
        return 0xffffffff;
    }

    for(i = 0; i < nbytes; i++) {
        lrc ^= p[i];
    }

    return lrc;
}

/**
 * \brief 计算CRC16校验值
 *
 * \param[in] p_buf  : 存放数据的缓冲区
 * \param[in] nbytes : 数据的字节数
 *
 * \return LRC校验值
 */
aw_local uint32_t __awbl_sim_crc16_calc (uint8_t *p_buf, uint32_t nbytes)
{
    aw_err_t ret;

    AW_CRC_DECL(crc16,
                16,
                0x1021,
                0x0000,
                AW_TRUE,
                AW_TRUE,
                0x0000);

    ret = AW_CRC_INIT(crc16,
                      crctable16_1021_ref,
                      AW_CRC_FLAG_AUTO);

    if (ret == AW_OK) {
        AW_CRC_CAL(crc16, p_buf, nbytes);
        return AW_CRC_FINAL(crc16);
    }

    return 0;
}

/**
 * \brief ISO7816-3 接触式IC卡PPS请求和响应设置
 */
aw_err_t awbl_sim_pps_set (int id, uint8_t *p_pps)
{
    uint8_t   tx_buf[6];
    uint8_t   rx_buf[6];
    uint8_t   index;
    uint8_t   fd;

    awbl_sim_xmt_t    xmt;
    awbl_sim_rcv_t    rcv;
    awbl_sim_baud_t   baud;
    awbl_sim_timing_t timing;

    struct awbl_sim_service *p_serv = __sim_id_to_serv(id);

    if (p_serv == NULL) {
        return -AW_ENXIO;  /* SIM[id] is invalid */
    }

    tx_buf[0] = 0xFF;    /* PPS请求和相应代码固定为0xFF */
    if (p_serv->protocol_type == 0) {
        tx_buf[1] = p_pps[0] & 0x70;
    } else {
        /* T= 1，除T=0和T=1之外的协议按T=1处理 */
        tx_buf[1] = (p_pps[0] & 0x70) | 0x01;
    }

    index = 2;
    if (p_pps[0] & 0x10) {
        tx_buf[index++] = p_pps[1];
    }
    if (p_pps[0] & 0x20) {
        tx_buf[index++] = p_pps[2];
    }
    if (p_pps[0] & 0x40) {
        tx_buf[index++] = p_pps[3];
    }

    tx_buf[index] = __awbl_sim_lrc_calc(tx_buf, index);
    index++;

    awbl_sim_mkxmt(&xmt, tx_buf, index, 0);
    awbl_sim_ioctl(id, AWBL_SIM_IOCTL_XMT, (void *)&xmt);
    if (xmt.error != AWBL_SIM_OK) {
        return -AW_EIO;
    }

    /* 接收响应 */
    awbl_sim_mkrcv(&rcv, rx_buf, index, 0);
    awbl_sim_ioctl(id, AWBL_SIM_IOCTL_RCV, (void *)&rcv);
    if (rcv.error != AWBL_SIM_OK) {
        return -AW_EIO;
    }

    if (tx_buf[0] == rx_buf[0] || (rx_buf[1] & 0x0f) == (p_pps[0] & 0x0f)) {
        if (p_pps[0] & 0x10) {
            /* PPS1存在 */
            if (rx_buf[1] & 0x10) {
                /* 响应PPS1请求 */
                if (rx_buf[2] == p_pps[1]) {
                    fd = p_pps[1];
                    awbl_sim_mkbaud(&baud, fd & 0x0f, fd >> 4);
                    awbl_sim_ioctl(id, AWBL_SIM_IOCTL_SET_BAUD, (void *)&baud);

                } else {
                    return -AW_EAGAIN;
                }
            }
        }

        if (p_pps[0] & 0x20) {
            /* PPS2存在 */
            if (rx_buf[1] & 0x20) {
                /* 响应PPS2请求 */
                if (rx_buf[3] == p_pps[2]) {
                    awbl_sim_ioctl(id, AWBL_SIM_IOCTL_GET_TIMING, &timing);
                    timing.cgt = p_pps[2];
                    awbl_sim_ioctl(id, AWBL_SIM_IOCTL_SET_TIMING, &timing);
                } else {
                    return -AW_EAGAIN;
                }
            }
        }

        if (p_pps[0] & 0x40) {
            /* PPS3存在 */
            if (rx_buf[1] & 0x40) {
                /* 响应PPS3请求，不支持PPS3 */
                if (rx_buf[4] != p_pps[3]) {
                    return -AW_ENOTSUP;
                }
            }
        }
    } else {
        return -AW_EAGAIN;
    }

    return AW_OK;
}

/**
 * \brief sim io control
 */
aw_err_t awbl_sim_ioctl (int id, int request, void *p_arg)
{
    struct awbl_sim_service *p_serv = __sim_id_to_serv(id);
    aw_err_t ret;

    if (p_serv == NULL) {
        return -AW_ENXIO;  /* SIM[id] is invalid */
    }

    AW_MUTEX_LOCK(p_serv->devlock, AW_SEM_WAIT_FOREVER);
    ret = p_serv->p_servopts->pfn_sim_iotcl(p_serv->p_cookie, request, p_arg);
    AW_MUTEX_UNLOCK(p_serv->devlock);
    
    return ret;
}

/**
 * \brief sim reset card
 */
aw_err_t awbl_sim_reset (int              id,
                         awbl_sim_baud_t *p_buad,
                         int              flags,
                         awbl_sim_atr_t  *p_atr)
{
    aw_err_t          ret;
    awbl_sim_baud_t   baud;
    awbl_sim_timing_t timing;
    awbl_sim_atr_t    sim_atr;   
    uint8_t           sim_atr_buf[33];

    int i, td_x;

    uint8_t extra_time = 0;
    uint8_t wi         = 10;
    uint8_t bwi        = 4;
    uint8_t cwi        = 5;

    struct awbl_sim_service *p_serv = __sim_id_to_serv(id);

    if (p_serv == NULL) {
        return -AW_ENXIO;  /* SIM[id] is invalid */
    }

    if (p_buad == NULL) {
        awbl_sim_mkbaud(&baud, 1, 1);
        awbl_sim_ioctl(id, AWBL_SIM_IOCTL_SET_BAUD, (void *)&baud);
    } else {
        awbl_sim_ioctl(id, AWBL_SIM_IOCTL_SET_BAUD, (void *)p_buad);
    }

    if (flags == AWBL_SIM_IOCTL_COLD_RESET) {
        awbl_sim_ioctl(id, AWBL_SIM_IOCTL_COLD_RESET, (void *)0);
    } else {
        awbl_sim_ioctl(id, AWBL_SIM_IOCTL_WARM_RESET, (void *)0);
    }

    sim_atr.p_atr_buf = sim_atr_buf;
    awbl_sim_ioctl(id, AWBL_SIM_IOCTL_GET_ATR, (void *)&sim_atr);
    
    awbl_sim_ioctl(id, AWBL_SIM_IOCTL_WAIT, (void *)24);
    
    if (p_atr != NULL) {
        p_atr->size  = sim_atr.size;
        p_atr->error = sim_atr.error;
        if (sim_atr.error == AWBL_SIM_OK) {
            if (p_atr->p_atr_buf != NULL) {
                memcpy(p_atr->p_atr_buf,
                       sim_atr.p_atr_buf,
                       sim_atr.size);
            }
            ret = AW_OK;
        } else {
            return -AW_EIO;
        }
    }

    /*
     * 先对T0字节做解析
     */
    i            = 1;
    td_x         = i;
    p_serv->ifsc = 32;
    p_serv->tc3  = 0;

    if (sim_atr.p_atr_buf[td_x] & 0x10) {

         /* TA(1)存在，更新通信速率 */
        ++i;

    } else {
        //pps = AW_FALSE;
    }

    if (sim_atr.p_atr_buf[td_x] & 0x20) {

        /* 对TB(1)不做处理 */
        ++i;
    }

    if (sim_atr.p_atr_buf[td_x] & 0x40) {

        /* TC(1)存在，更新额外保护时间 */
        ++i;
        extra_time = sim_atr.p_atr_buf[i];
//        if (extra_time == 0xFF) {
//            extra_time = 0;
//        }
    }

    if (!(sim_atr.p_atr_buf[td_x] & 0x80)) {

        /* TD(1)不存在 */

    } else {
        ++i;
        td_x = i;
        p_serv->protocol_type = sim_atr.p_atr_buf[i] & 0x0f;

        if (sim_atr.p_atr_buf[td_x] & 0x10) {
            /* TA(2)存在 */
            ++i;

        } else {
            /* TA(2)不存在，需要使用协商模式，修改参数 */

        }

        if (sim_atr.p_atr_buf[td_x] & 0x20) {
            ++i;    /* TB2存在(定义编程电压PI2，TB2存在，TB1定义的编程PI1无效) */
        }

        if (sim_atr.p_atr_buf[td_x] & 0x40) {
            ++i;
            wi = sim_atr.p_atr_buf[i];
        }

        if (sim_atr.p_atr_buf[td_x] & 0x80) {
            /* TD(2)存在 */
            ++i;
            td_x = i;
            if (sim_atr.p_atr_buf[td_x] & 0x10) {
                /* TA(3)存在，定义信息字段长度 */
                ++i;
                p_serv->ifsc = sim_atr.p_atr_buf[i];
            }
            if (sim_atr.p_atr_buf[td_x] & 0x20) {
                /* TB(3)存在，字符等待时间 */
                ++i;
                bwi = sim_atr.p_atr_buf[i] >> 4;
                cwi = sim_atr.p_atr_buf[i] & 0x0F;
            }
            if (sim_atr.p_atr_buf[td_x] & 0x40) {
                /* TC(3)存在，帧尾校验模式 */
                ++i;
                p_serv->tc3   = sim_atr.p_atr_buf[i];
            }
        }
    }

    if (p_serv->protocol_type != AWBL_SIM_PROTOCOL_T0) {
        /* 计算TCK */
        if (__awbl_sim_lrc_calc(&sim_atr.p_atr_buf[1],
                                sim_atr.size - 1)) {
            /* LRC的结果要为0，才是正确的数据 */
            return -AW_EBADMSG;
        }
    }

    if (p_serv->protocol_type == AWBL_SIM_PROTOCOL_T1) {
        awbl_sim_mktiming(&timing,
                          0,
                          (0x1 << cwi) + 11,
                          extra_time,
                          (0x1 << bwi) * 960 + 11,
                          22);
        awbl_sim_ioctl(id, AWBL_SIM_IOCTL_SET_TIMING, (void *)&timing);
    } else if (p_serv->protocol_type == AWBL_SIM_PROTOCOL_T0) {
        awbl_sim_mktiming(&timing,
                          0,
                          960 * wi,
                          extra_time,
                          0,
                          0);
        awbl_sim_ioctl(id, AWBL_SIM_IOCTL_SET_TIMING, (void *)&timing);
    }

    return ret;
}

/**
 * \brief T=0
 */
aw_local aw_err_t __awbl_sim_t0_apdu (int            id,
                                      const uint8_t *p_txbuf,
                                      int            n_tx,
                                      uint8_t       *p_rxbuf,
                                      int           *p_n_rx)
{
    aw_err_t ret = 0;
    uint8_t  cmd_header[5];
    uint8_t  ins, lc, le, ack, temp_le;
    uint8_t *p_send_buf;
    uint8_t *p_recv_buf;

    awbl_sim_xmt_t xmt;
    awbl_sim_rcv_t rcv;

    p_send_buf = (uint8_t *)p_txbuf;
    p_recv_buf = (uint8_t *)p_rxbuf;

    *p_n_rx = 0;

    ins     = p_txbuf[1];
    lc      = 0;
    le      = 0;

    memcpy(cmd_header, p_txbuf, 5);
    if (n_tx > 5) {
        /* 有数据域的情况, 情况3、4 */
        lc = p_send_buf[4];
        if (n_tx > lc + 5) {
            /* APDU命令情况3,lc Data */
            le = p_send_buf[n_tx - 1]; /* APDU命令情况4,Lc Data Le */
        }
    } else if (5 == n_tx) {
        le = p_send_buf[4];  /* APDU命令情况2, Data Le */
    } else {
        cmd_header[4] = 0;   /* APDU命令情况1 */
    }
    temp_le = le;

    /* 发送命令头 */
    awbl_sim_mkxmt(&xmt, cmd_header, 5, 0);
    awbl_sim_ioctl(id, AWBL_SIM_IOCTL_XMT, (void *)&xmt);
    if (xmt.error!= AWBL_SIM_OK) {
        return -AW_EIO;
    }

    AW_FOREVER {

        /* 接收响应 */
        awbl_sim_mkrcv(&rcv, &ack, 1, 0);
        awbl_sim_ioctl(id, AWBL_SIM_IOCTL_RCV, (void *)&rcv);
        if (rcv.error != AWBL_SIM_OK) {
            ret = -AW_EIO;
            break;
        }

        if (ack == ins) {

            /* NULL，表示继续等待过程字节 */
            if (lc != 0) {
                awbl_sim_ioctl(id, AWBL_SIM_IOCTL_WAIT, (void *)16);
                awbl_sim_mkxmt(&xmt, &p_send_buf[5], lc, 0);
                awbl_sim_ioctl(id, AWBL_SIM_IOCTL_XMT, (void *)&xmt);
                if (xmt.error!= AWBL_SIM_OK) {
                    ret = -AW_EIO;
                    break;
                }
                lc = 0;
            } else {

                awbl_sim_mkrcv(&rcv, &p_recv_buf[*p_n_rx], temp_le, 0);
                awbl_sim_ioctl(id, AWBL_SIM_IOCTL_RCV, (void *)&rcv);
                if (rcv.error != AWBL_SIM_OK) {
                    ret = -AW_EIO;
                    break;
                }

                if (0 != le) {
                    *p_n_rx += (temp_le < le) ? temp_le : le;
                } else {
                    *p_n_rx += temp_le;
                }
            }
        } else if (ack == 0x61) {

            /* 正常处理，还有XX字节可接收 */
            awbl_sim_mkrcv(&rcv, &temp_le, 1, 0);
            awbl_sim_ioctl(id, AWBL_SIM_IOCTL_RCV, (void *)&rcv);
            if (rcv.error != AWBL_SIM_OK) {
                ret = -AW_EIO;
                break;
            }

            /*
             * 发GetRespons命令
             */
            ins            = 0xC0;  /* APDU_INS_GET_RESPONSE; */
            cmd_header[0]  = 0x00;  /* APDU_CLA_ISO; */
            cmd_header[1]  = 0xC0;  /* 取响应数据 APDU_INS_GET_RESPONSE; */
            cmd_header[2]  = 0;
            cmd_header[3]  = 0;

            /* 取较小长度(Le = 0 为最大长度) */
            if (0 != le) {
                temp_le = (temp_le < le) ? temp_le : le;
            }
            cmd_header[4]  = temp_le;
            awbl_sim_ioctl(id, AWBL_SIM_IOCTL_WAIT, (void *)16);

            /* 发送命令头 */
            awbl_sim_mkxmt(&xmt, cmd_header, 5, 0);
            awbl_sim_ioctl(id, AWBL_SIM_IOCTL_XMT, (void *)&xmt);
            if (xmt.error!= AWBL_SIM_OK) {
                ret = -AW_EIO;
                break;
            }
        } else if (ack == 0x6C) {

            /* 命令情形2, Le长度错误，实际长度由下字节指出 */
            awbl_sim_mkrcv(&rcv, &temp_le, 1, 0);
            awbl_sim_ioctl(id, AWBL_SIM_IOCTL_RCV, (void *)&rcv);
            if (rcv.error != AWBL_SIM_OK) {
                ret = -AW_EIO;
                break;
            }

            cmd_header[4] = temp_le;
            awbl_sim_ioctl(id, AWBL_SIM_IOCTL_WAIT, (void *)16);

            /* 发送命令头 */
            awbl_sim_mkxmt(&xmt, cmd_header, 5, 0);
            awbl_sim_ioctl(id, AWBL_SIM_IOCTL_XMT, (void *)&xmt);
            if (xmt.error!= AWBL_SIM_OK) {
                ret = -AW_EIO;
                break;
            }
        } else {
            p_recv_buf[*p_n_rx] = ack;

            awbl_sim_mkrcv(&rcv, &ack, 1, 0);
            awbl_sim_ioctl(id, AWBL_SIM_IOCTL_RCV, (void *)&rcv);
            if (rcv.error != AWBL_SIM_OK) {
                ret = -AW_EIO;
                break;
            }

            *p_n_rx += 1;
            p_recv_buf[*p_n_rx] = ack;
            *p_n_rx += 1;
            break;
        }

    }

    awbl_sim_ioctl(id, AWBL_SIM_IOCTL_WAIT, (void *)10);

    return ret;
}

/**
 * \brief T=1
 */
aw_local aw_err_t __awbl_sim_t1_apdu (int            id,
                                      const uint8_t *p_txbuf,
                                      int            n_tx,
                                      uint8_t       *p_rxbuf,
                                      int           *p_n_rx)
{
    uint8_t   check_bytes;        /* 校验字节数 */
    uint8_t   info_length;        /* 当前帧的信息长度 */
    uint8_t  *p_send_buf;
    uint8_t  *p_recv_buf;
    uint8_t   temp_buf[0xFE + 0x5];
    aw_bool_t    recv_flag;

    uint8_t   pcb_send, pcb_recv;
    uint32_t  sent_nbytes = 0;    /* 已经发送的字节数 */
    uint32_t  recv_nbytes = 0;    /* 已结接收的字节数 */
    uint8_t   seq_num;            /* 分组发送序号 */
    uint8_t   error_count = 0;    /* 错误计数 */
    uint16_t  edc;
    uint8_t   wtx;

    awbl_sim_xmt_t xmt;
    awbl_sim_rcv_t rcv;
    awbl_sim_timing_t timing, detiming;

    struct awbl_sim_service *p_serv = __sim_id_to_serv(id);
    aw_err_t ret;

    if (p_serv == NULL) {
        return -AW_ENXIO;  /* SIM[id] is invalid */
    }

    p_send_buf = (uint8_t *)p_txbuf;
    p_recv_buf = (uint8_t *)p_rxbuf;

    if (p_serv->tc3 & 0x01) {
        check_bytes = 2;  /* CRC校验 2B */
    } else {
        check_bytes = 1;  /* LRC校验 1B */
    }

    /* 发送PCB为I块，序号为0 */
    pcb_send    = __AWBL_SIM_T1_PCB_I_BLOCK | __AWBL_SIM_T1_PCB_I_N;
    pcb_recv    = __AWBL_SIM_T1_PCB_R_BLOCK;
    recv_flag   = AW_FALSE;

    sent_nbytes = 0;     /* 已发送字节数为0 */
    seq_num     = 0x10;  /* 分组发送序号为0 */

    /* 临时保存时间参数 */
    awbl_sim_ioctl(id, AWBL_SIM_IOCTL_GET_TIMING,(void *)&detiming);

    AW_FOREVER {
        info_length = 0;

        switch (pcb_send & 0xC0) {

        case __AWBL_SIM_T1_PCB_I_BLOCK | __AWBL_SIM_T1_PCB_I_N : /* I块序号为0 */
        case __AWBL_SIM_T1_PCB_I_BLOCK | __AWBL_SIM_T1_PCB_I_S : /* I块序号为1 */

            if (recv_flag == AW_FALSE) {
                /* 是发送状态，发送I块 IFD <- IC */
                if ((n_tx - sent_nbytes) > p_serv->ifsc) {
                    /* 未发送的字节数大于IFSC，分包 */
                    info_length  = p_serv->ifsc;
                    pcb_send    |= __AWBL_SIM_T1_PCB_I_M;  /* 数据将会分多次发送出去 */
                } else {
                    /* 未发送的字节数小于IFSC，不分包 */
                    info_length = (uint8_t)(n_tx - sent_nbytes);
                    recv_flag   = AW_TRUE; /* 数据发送完毕，转接收状态 */
                }
                temp_buf[2] = info_length;
                memcpy(&temp_buf[3], &p_send_buf[sent_nbytes], info_length);
            }
            pcb_send |= seq_num & 0x40;

            break;

        case __AWBL_SIM_T1_PCB_R_BLOCK :
            if (error_count > 2) {
                return ret;   /* 出错次数大于2次，执行错误退出 */
            }
            if (ret == -AW_ETIME) {
                pcb_send |= __AWBL_SIM_T1_PCB_R_ERR_OTHERS;
            } else if ((-AW_EBADMSG == ret) || (__AWBL_SIM_T1_PCB_R_ERR_EDC == ret)) {
                pcb_send |= __AWBL_SIM_T1_PCB_R_ERR_EDC;
            } else if (ret != AW_OK) {
                pcb_send |= __AWBL_SIM_T1_PCB_R_ERR_OTHERS;
            }
            pcb_send |= seq_num & 0x10;
            break;

        case __AWBL_SIM_T1_PCB_S_BLOCK :
            pcb_send |= 0x20;   /* 发生了响应 */
            break;

        default :
            break;
        }

        temp_buf[0] = 0x00;   /* 不使用编址 */
        temp_buf[1] = pcb_send;

        if (check_bytes == 2) {
            /* 使用CRC校验 */
            edc = (uint16_t)__awbl_sim_crc16_calc(temp_buf, info_length + 3);
            temp_buf[info_length + 3] = edc & 0xff;
            temp_buf[info_length + 4] = (edc >> 8) & 0xff;
        } else {
            /* 使用LRC校验 */
            edc = (uint8_t)__awbl_sim_lrc_calc(temp_buf, info_length + 3);
            temp_buf[info_length + 3] = edc & 0xff;
        }

        /* 发送命令头 */
        awbl_sim_mkxmt(&xmt, temp_buf, info_length + 3 + check_bytes, 0);
        awbl_sim_ioctl(id, AWBL_SIM_IOCTL_XMT,(void *)&xmt);

        // 等待接收帧头
        if ((pcb_send & (0xC0 | 0x1f)) ==
            (__AWBL_SIM_T1_PCB_S_BLOCK | __AWBL_SIM_T1_PCB_S_WTX_REQ) ) {
            /* 更新BWT时间 */
            timing = detiming;
            timing.bwt = (0x01 << (wtx & 0x0F)) * 960L + 11;
            awbl_sim_ioctl(id, AWBL_SIM_IOCTL_SET_TIMING, (void *)&timing);

        } else {
            /* 使用默认的超时等待时间 */
            awbl_sim_ioctl(id, AWBL_SIM_IOCTL_SET_TIMING, (void *)&detiming);
        }
        awbl_sim_mkrcv(&rcv, temp_buf, 3 + check_bytes, 0);
        ret = awbl_sim_ioctl(id, AWBL_SIM_IOCTL_RCV, (void *)&rcv);

        if (rcv.error == AWBL_SIM_OK && ret == AW_OK) {
            if (temp_buf[2] != 0) {
                awbl_sim_mkrcv(&rcv, &temp_buf[3 + check_bytes], temp_buf[2], 0);
                ret = awbl_sim_ioctl(id, AWBL_SIM_IOCTL_RCV, (void *)&rcv);
            }
        }

        if (rcv.error == AWBL_SIM_OK && ret == AW_OK) {
            if (temp_buf[0] != 0x00) {
                ret = -AW_EBADMSG;
            }

            if (check_bytes == 2) {
                /* CRC校验 */
                edc  = temp_buf[temp_buf[2] + 3] & 0xff;
                edc |= (temp_buf[temp_buf[2] + 4] << 8) & 0xff00;
                if (edc != (uint16_t)__awbl_sim_crc16_calc(temp_buf,
                                                           temp_buf[2] + 3)) {
                    ret = -AW_EBADMSG;
                }
            } else {
                /* LRC校验 */
                edc = temp_buf[temp_buf[2] + 3] & 0xff;
                if (edc != (uint8_t)__awbl_sim_lrc_calc(temp_buf,
                                                        temp_buf[2] + 3)) {
                    ret = -AW_EBADMSG;
                }
            }
        }

        if (rcv.error != AWBL_SIM_OK || ret != AW_OK) {
            error_count++;
            pcb_send = __AWBL_SIM_T1_PCB_R_BLOCK;
            continue;
        }

        pcb_recv = temp_buf[1];
        switch (pcb_recv & 0xC0) {

        case __AWBL_SIM_T1_PCB_I_BLOCK | __AWBL_SIM_T1_PCB_I_N : /* I块序号为0 */
        case __AWBL_SIM_T1_PCB_I_BLOCK | __AWBL_SIM_T1_PCB_I_S : /* I块序号为1 */
            error_count = 0;
            if (260 < recv_nbytes + temp_buf[2]) {
                return -AW_EMSGSIZE;
            }
            memcpy(&p_recv_buf[recv_nbytes], &temp_buf[3], temp_buf[2]);
            recv_nbytes += temp_buf[2];

            if (temp_buf[1] & 0x20) {
                /* M=1 还有数据块 */
                seq_num ^= (0x40 | 0x10);
                pcb_send = __AWBL_SIM_T1_PCB_R_BLOCK;
                temp_buf[2] = 0x00;
            } else {
                /* 接收完成 */
                *p_n_rx = recv_nbytes;
                return AW_OK;
            }
            break;

        case __AWBL_SIM_T1_PCB_S_BLOCK :
            pcb_send = pcb_recv;
            switch (pcb_recv & 0x1f) {

            case __AWBL_SIM_T1_PCB_S_IFS_REQ :
            case __AWBL_SIM_T1_PCB_S_WTX_REQ :
                if (temp_buf[2] != 1) {
                    error_count++;
                    pcb_send = __AWBL_SIM_T1_PCB_R_BLOCK;
                    ret      = -AW_EPROTO;
                } else {
                    error_count   = 0;
                    wtx           = temp_buf[3];
                    temp_buf[2] = 0x00;
                }
                break;

            case __AWBL_SIM_T1_PCB_S_RES_REQ :
            case __AWBL_SIM_T1_PCB_S_ABORT_REQ :
                error_count = 0;
                break;

            default:
                error_count++;
                pcb_send = __AWBL_SIM_T1_PCB_R_BLOCK;
                ret      = -AW_EPROTO;
            }

            break;

        default :
//        case __AWBL_SIM_T1_PCB_R_BLOCK :
            if (temp_buf[2] == 0) {
                if ((pcb_recv ^ seq_num) & 0x10) {
                    /* 序号正确，判断错误码 */

                    if ((pcb_recv & 0x0f) == __AWBL_SIM_T1_PCB_R_ERR_NO) {
                        if (((pcb_send & 0x80) == __AWBL_SIM_T1_PCB_I_BLOCK) && (recv_flag == AW_FALSE)) {
                            sent_nbytes += info_length;
                        }
                        pcb_send = __AWBL_SIM_T1_PCB_I_BLOCK;
                        break;
                    } else {
                        /* 错误处理 */
                        if ((pcb_recv & 0x0F) == __AWBL_SIM_T1_PCB_R_ERR_EDC) {
                            ret = __AWBL_SIM_T1_PCB_R_ERR_EDC;
                        } else {
                            ret = __AWBL_SIM_T1_PCB_R_ERR_OTHERS;
                        }
                    }
                } else {
                    ret = -AW_EPROTO;
                }
            } else {
                ret = -AW_EPROTO;
            }

            error_count++;
            pcb_send = __AWBL_SIM_T1_PCB_R_BLOCK;
            break;
        }
    }

    return ret;
}


/**
 * \brief 处理一条APDU命令
 */
aw_err_t awbl_sim_apdu (int            id,
                        const uint8_t *p_c_apdu,
                        int            c_apdu_len,
                        uint8_t       *p_r_apdu,
                        int           *p_r_apdu_len)
{
    struct awbl_sim_service *p_serv = __sim_id_to_serv(id);
    aw_err_t ret;

    if (p_serv == NULL) {
        return -AW_ENXIO;  /* SIM[id] is invalid */
    }

    if (p_c_apdu == NULL || p_r_apdu == NULL || p_r_apdu_len == NULL) {
        return -AW_EINVAL;
    }

    switch (p_serv->protocol_type) {

    case AWBL_SIM_PROTOCOL_T0 :
        ret = __awbl_sim_t0_apdu(id, p_c_apdu, c_apdu_len, p_r_apdu, p_r_apdu_len);
        break;

    case AWBL_SIM_PROTOCOL_T1 :
        ret = __awbl_sim_t1_apdu(id, p_c_apdu, c_apdu_len, p_r_apdu, p_r_apdu_len);
        break;

    default :
        ret = -AW_ENOTSUP;
        break;

    }

    return ret;
}

/* end of file */
