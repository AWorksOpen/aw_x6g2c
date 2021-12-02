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

#ifndef __AWBL_SIM_H
#define __AWBL_SIM_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "awbus_lite.h"
#include "aw_ioctl_generic.h"
#include "aw_sem.h"

/*
 * \name sim ioctl function codes
 * @{
 */

/** \brief ioctl base */
#define AWBL_SIM_IOCTL_BASE              (0xc0)

/** \brief get current card status */
#define AWBL_SIM_IOCTL_GET_PRESENSE      __AW_IOR(AWBL_SIM_IOCTL_BASE, 1, int)

/** \brief get restet atr */
#define AWBL_SIM_IOCTL_GET_ATR           __AW_IOR(AWBL_SIM_IOCTL_BASE, 2, awbl_sim_atr_t)

/** \brief transmit data */
#define AWBL_SIM_IOCTL_XMT               __AW_IOR(AWBL_SIM_IOCTL_BASE, 3, awbl_sim_xmt_t)

/** \brief receive data */
#define AWBL_SIM_IOCTL_RCV               __AW_IOR(AWBL_SIM_IOCTL_BASE, 4, awbl_sim_rcv_t)

/** \brief power on activate sequence */
#define AWBL_SIM_IOCTL_ACTIVATE          __AW_IO(AWBL_SIM_IOCTL_BASE, 5)

/** \brief power off deactivate sequence */
#define AWBL_SIM_IOCTL_DEACTIVATE        __AW_IO(AWBL_SIM_IOCTL_BASE, 6)

/** \brief warm reset */
#define AWBL_SIM_IOCTL_WARM_RESET        __AW_IO(AWBL_SIM_IOCTL_BASE, 7)

/** \brief cold reset */
#define AWBL_SIM_IOCTL_COLD_RESET        __AW_IO(AWBL_SIM_IOCTL_BASE, 8)

/** \brief lock card */
#define AWBL_SIM_IOCTL_CARD_LOCK         __AW_IO(AWBL_SIM_IOCTL_BASE, 9)

/** \brief eject card */
#define AWBL_SIM_IOCTL_CARD_EJECT        __AW_IO(AWBL_SIM_IOCTL_BASE, 10)

/** \brief set protocol */
#define AWBL_SIM_IOCTL_SET_PROTOCOL      __AW_IOR(AWBL_SIM_IOCTL_BASE, 11, uint32_t)

/** \brief set time parameter */
#define AWBL_SIM_IOCTL_SET_TIMING        __AW_IOR(AWBL_SIM_IOCTL_BASE, 12, awbl_sim_timing_t)

/** \brief set communication baudrate */
#define AWBL_SIM_IOCTL_SET_BAUD          __AW_IOR(AWBL_SIM_IOCTL_BASE, 13, awbl_sim_baud_t)

/** \brief delay wait */
#define AWBL_SIM_IOCTL_WAIT              __AW_IOR(AWBL_SIM_IOCTL_BASE, 14, uint32_t)

/** \brief get protocol */
#define AWBL_SIM_IOCTL_GET_PROTOCOL      __AW_IOR(AWBL_SIM_IOCTL_BASE, 15, uint32_t)

/** \brief get time parameter */
#define AWBL_SIM_IOCTL_GET_TIMING        __AW_IOR(AWBL_SIM_IOCTL_BASE, 16, awbl_sim_timing_t)

/** \brief get communication baudrate */
#define AWBL_SIM_IOCTL_GET_BAUD          __AW_IOR(AWBL_SIM_IOCTL_BASE, 17, awbl_sim_baud_t)

/** @} */

#define AWBL_SIM_POWER_OFF      (0)   /**< \brief the card power on */
#define AWBL_SIM_POWER_ON       (1)   /**< \brief the card power off */

#define AWBL_SIM_PRESENT_REMOVED         (0)  /**< \brief the card had been removed */
#define AWBL_SIM_PRESENT_DETECTED        (1)  /**< \brief the card had been detected */
#define AWBL_SIM_PRESENT_OPERATIONAL     (2)  /**< \brief the card is being operated */

/*
 * \name sim ioctl function codes
 * \note AWBL_SIM_IOCTL_XMT and AWBL_SIM_IOCTL_RCV error codes
 * @{
 */

#define AWBL_SIM_OK                      (0)      /**< \brief transmit or receive success */
#define AWBL_SIM_ERROR_CWT               (1 << 0) /**< \brief trasnmit or receive cwt timeout */
#define AWBL_SIM_ERROR_BWT               (1 << 1) /**< \brief trasnmit or receive bwt timeout */
#define AWBL_SIM_ERROR_PARITY            (1 << 2) /**< \brief data parity error */
#define AWBL_SIM_ERROR_INVALID_TS        (1 << 3) /**< \brief invalid transmit */
#define AWBL_SIM_ERROR_FRAME             (1 << 4) /**< \brief data frame error */
#define AWBL_SIM_ERROR_ATR_TIMEROUT      (1 << 5) /**< \brief reset timeout */
#define AWBL_SIM_ERROR_NACK_THRESHOLD    (1 << 6) /**< \brief nack threshold error */
#define AWBL_SIM_ERROR_BGT               (1 << 7) /**< \brief trasnmit or receive bgt timeout */
#define AWBL_SIM_ERROR_ATR_DELAY         (1 << 8) /**< \brief reset delay error */

/** @} */

/**
 * \brief  ATR AWBL_SIM_IOCTL_GET_ATR
 */
typedef struct awbl_sim_atr {
    uint32_t  size;        /**< \brief length of ATR received */
    uint8_t  *p_atr_buf;   /**< \brief ATR string received */
    int       error;       /**< \brief operation result */
} awbl_sim_atr_t;

#define AWBL_SIM_PROTOCOL_T0  0   /**< \brief T=0 transmit protocol */
#define AWBL_SIM_PROTOCOL_T1  1   /**< \brief T=1 transmit protocol */

/**
 * \brief TIMING AWBL_SIM_IOCTL_SET_TIMING
 */
typedef struct awbl_sim_timing {
    uint32_t wwt;   /**< \brief 不为0，表示cwt和bwt都使用该值*/
    uint32_t cwt;   /**< \brief 相同方向两个连续字符之间最大延时 */
    uint32_t cgt;   /**< \brief 相同方向上两个连续字符之间最小延时 */
    uint32_t bwt;   /**< \brief 相反方向两个连续字符之间最大延时 */
    uint32_t bgt;   /**< \brief 相反方向两个连续字符之间最小延时 */
} awbl_sim_timing_t;

/**
 * \brief Transfer data for AWBL_SIM_IOCTL_XMT
 */
typedef struct awbl_sim_xmt_t {
    uint8_t  *p_xmt_buf;   /* transmit buffer pointer */
    int       xmt_length;  /* transmit buffer length */
    int       timeout;     /* transfer timeout in milliseconds */
    int       error;
} awbl_sim_xmt_t;

/**
 * \brief Receive data for AWBL_SIM_IOCTL_RCV
 */
typedef struct awbl_sim_rcv {
    uint8_t  *p_rcv_buf;   /* receive buffer pointer */
    int       rcv_length;  /* receive buffer length */
    int       timeout;     /* transfer timeout in milliseconds */
    int       error;
} awbl_sim_rcv_t;

/**
 * \brief Baudrate AWBL_SIM_IOCTL_BAUD
 */
typedef struct awbl_sim_baud {
    uint8_t di;     /**< \brief 速率调节因子 */
    uint8_t fi;     /**< \brief 时钟速率转换因子 */
} awbl_sim_baud_t;

/**
 * \brief AWBus SIM 服务实例的相关信息
 */
struct awbl_sim_servinfo {

    /** \brief ZLG600A 设备编号 */
    int dev_id;
};

/**
 * \brief AWBus SIM 服务函数
 */
struct awbl_sim_servopts {

    /** \brief sim ioctl function*/
    aw_err_t (*pfn_sim_iotcl) (void *p_cookie, int request, void *p_arg);
};

/**
 * \brief ZLG600A服务结构体
 */
typedef struct awbl_sim_service {

    /** \brief 指向下一个 SIM 服务 */
    struct awbl_sim_service *p_next;

    /** \brief ZLG600A 服务的相关信息 */
    const struct awbl_sim_servinfo *p_servinfo;

    /** \brief ZLG600A 服务的相关函数 */
    const struct awbl_sim_servopts *p_servopts;
    
    /** \brief 设备锁 */
    AW_MUTEX_DECL(devlock);

    /** \brief 协议类型 */
    int   protocol_type;
    
    uint8_t ifsc;  /**< \brief 最大接收信息长度 */
    uint8_t tc3;   /**< \brief 帧尾校验模式 */

    void *p_cookie;

} awbl_sim_service_t;

/**
 * \brief 构建ATR结构体
 */
aw_local aw_inline void awbl_sim_mkatr (awbl_sim_atr_t *p_atr,
                                        void           *p_rxbuf)
{
    p_atr->p_atr_buf = (uint8_t *)p_rxbuf;
}


/**
 * \brief 设置sim时间相关的参数
 *
 * \param[in] p_timing : 时间参数结构体指针
 * \param[in] wwt      : 不为0，表示cwt和bwt都使用该值, 单位etu
 * \param[in] cwt      : 相同方向两个连续字符之间最大延时, 单位etu
 * \param[in] cgt      : 相同方向上两个连续字符之间最小延时, 单位etu
 * \param[in] bwt      : 相反方向两个连续字符之间最大延时, 单位etu
 * \param[in] bgt      : 相反方向两个连续字符之间最小延时, 单位etu
 */
aw_local aw_inline void awbl_sim_mktiming (awbl_sim_timing_t *p_timing,
                                           uint32_t           wwt,
                                           uint32_t           cwt,
                                           uint32_t           cgt,
                                           uint32_t           bwt,
                                           uint32_t           bgt)
{
    p_timing->wwt = wwt;
    p_timing->cwt = cwt;
    p_timing->cgt = cgt;
    p_timing->bwt = bwt;
    p_timing->bgt = bgt;
}

/**
 * \brief 设置传输结构体
 *
 * \param[in] p_xmt      : 传输结构体指针
 * \param[in] p_txbuf    : 缓冲区指针
 * \param[in] n_tx       : 传输字节数
 * \param[in] timeout_ms : 超时时间（单位：ms）
 */
aw_local aw_inline void awbl_sim_mkxmt (awbl_sim_xmt_t *p_xmt,
                                        void           *p_txbuf,
                                        int             n_tx,
                                        int             timeout_ms)
{
    p_xmt->p_xmt_buf  = (uint8_t *)p_txbuf;
    p_xmt->xmt_length = n_tx;
    p_xmt->timeout    = timeout_ms;
}

/**
 * \brief 设置接收结构体
 *
 * \param[in] p_xmt      : 接收结构体指针
 * \param[in] p_txbuf    : 缓冲区指针
 * \param[in] n_tx       : 接收字节数，这里填入期待接收的字节数
 * \param[in] timeout_ms : 超时时间（单位：ms）
 */
aw_local aw_inline void awbl_sim_mkrcv (awbl_sim_rcv_t *p_rcv,
                                        void           *p_rxbuf,
                                        int             n_rx,
                                        int             timeout_ms)
{
    p_rcv->p_rcv_buf  = (uint8_t *)p_rxbuf;
    p_rcv->rcv_length = n_rx;
    p_rcv->timeout    = timeout_ms;
}

/**
 * \brief 设置波特率参数结构体
 *
 * \param[in] p_baud : 波特率结构体指针
 * \param[in] di     : 速率调节因子
 * \param[in] fi     : 时钟速率转换因子
 *
 * \par 范例
 * \code
 * #include "awbl_sim.h"
 *
 * awbl_sim_baud_t baud; // 波特率参数结构体
 *
 * // fclk=4000000, fi=1(f=372), di=1，波特率为10752
 * awbl_sim_mkbaud(&buad, 1, 1);
 * awbl_sim_ioctl(0, AWBL_SIM_IOCTL_SET_BAUD, (void *)&baud);
 * \endcode
 */
aw_local aw_inline void awbl_sim_mkbaud (awbl_sim_baud_t *p_baud,
                                         uint8_t          di,
                                         uint8_t          fi)
{
    p_baud->di = di;
    p_baud->fi = fi;
}

/**
 * \brief AWBus SIM 接口模块初始化
 *
 * \attention 本函数应当在 awbl_dev_connect() 之后调用
 */
void awbl_sim_init (void);

/**
 * \brief sim io control 函数
 *
 * \param[in] id      : 设备ID
 * \param[in] request : 请求
 *                      @arg AWBL_SIM_IOCTL_GET_PRESENSE， 当前控制器状态， 参数为int型指针
 *                      @arg AWBL_SIM_IOCTL_GET_ATR， 获取复位应答数据， 参数为awbl_sim_atr_t类型指针
 *                      @arg AWBL_SIM_IOCTL_XMT， TTY传输数据到IC卡，参数为awbl_sim_xmt_t类型指针
 *                      @arg AWBL_SIM_IOCTL_RCV， TTY接收数据到IC卡，参数为awbl_sim_rcv_t类型指针
 *                      @arg AWBL_SIM_IOCTL_ACTIVATE， 卡片上电激活序列, 无参数
 *                      @arg AWBL_SIM_IOCTL_DEACTIVATE， 卡片掉电取消激活序列， 无参数
 *                      @arg AWBL_SIM_IOCTL_WARM_RESET， 卡片热复位， 无参数
 *                      @arg AWBL_SIM_IOCTL_COLD_RESET， 卡片冷复位，无参数
 *                      @arg AWBL_SIM_IOCTL_CARD_LOCK，锁定卡片，无参数
 *                      @arg AWBL_SIM_IOCTL_CARD_EJECT，弹出卡片，无参数
 *                      @arg AWBL_SIM_IOCTL_SET_PROTOCOL，设置通信协议
 *                      @arg AWBL_SIM_IOCTL_SET_TIMING，设置时间参数，参数为awbl_sim_timing_t类型指针
 *                      @arg AWBL_SIM_IOCTL_SET_BAUD，设置通信速率，参数为awbl_sim_baud_t类型指针
 *                      @arg AWBL_SIM_IOCTL_WAIT，设置延时
 * \param[in] p_arg : ioctl命令请求参数，参数类型和取值取决于对应的请求命令
 *
 * \return 返回值为错误编码，见 aw_errno.h 。具体取决于请求命令，典型返回值：
 *  - \b AW_OK      : 成功
 *  - \b -AW_EINVAL : 参数错误
 *  - \b -AW_ENXIO  : 设备不存在
 *  - \b -AW_EIO    : 设备执行命令失败
 */
aw_err_t awbl_sim_ioctl (int id, int request, void *p_arg);

/**
 * \brief sim reset card
 *
 * \param[in] id     : 设备ID
 * \param[in] p_buad : 复位使用的波特率，缺省情况fi=1，di=1
 * \param[in] flags  : 复位类型
 *                      @arg AWBL_SIM_IOCTL_WARM_RESET， 卡片热复位
 *                      @arg AWBL_SIM_IOCTL_COLD_RESET， 卡片冷复位
 * \param[in] p_atr  : 复位应答数据，为awbl_sim_atr_t类型指针
 *
 * \retval AW_OK      : 成功
 * \retval -AW_EINVAL : 参数错误
 * \retval -AW_ENXIO  : 设备不存在
 * \retval -AW_EIO    : 设备执行命令失败
 */
aw_err_t awbl_sim_reset (int              id,
                         awbl_sim_baud_t *p_buad,
                         int              flags,
                         awbl_sim_atr_t  *p_atr);
                         
/**
 * \brief ISO7816-3 接触式IC卡PPS请求和响应设置
 *
 * \param[in]  id    : 设备ID
 * \param[in] p_pps  : PPS参数（4 字节）
 *                     PPS[0] — 指定是否存在PPS1、PPS2、PPS3
 *                     PPS[0].3:0 — 保留
 *                     PPS[0].4 = 1 — PPS1 存在；0 — PPS1 不存在
 *                     PPS[0].5 = 1 — PPS2 存在；0 — PPS2 不存在
 *                     PPS[0].6 = 1 — PPS3 存在；0 — PPS3 不存在
 *                     PPS[0].7 — 保留
 *                     PPS[1] — Fi/Di
 *                     PPS[2] — N
 *                     PPS[3] — 待定
 *
 * \retval AW_OK       : 成功
 * \retval -AW_EINVAL  : 参数错误
 * \retval -AW_ENXIO   : 设备不存在
 * \retval -AW_EIO     : 设备执行命令失败
 * \retval -AW_EAGAIN  : 需重试
 * \retval -AW_ENOTSUP : 不支持
 *
 * \par 范例
 * \code
 * #include "awbl_sim.h"
 *
 * //将接触式IC卡通信波特率改为115200bps（Fi为1；Di为8）
 * uint8_t pps[4]=[0x10, 0x18, 0x00, 0x00];
 *
 * awbl_sim_pps_set(0, pps);
 * \endcode
 */
aw_err_t awbl_sim_pps_set (int id, uint8_t *p_pps);

/**
 * \brief 处理一条APDU命令
 *
 * \param[in]  id           : 设备ID
 * \param[in]  p_c_apdu     : 命令
 * \param[in]  c_apdu_len   : 命令长度
 * \param[out] p_r_apdu     : 应答
 * \param[out] p_r_apdu_len : 应答长度
 *
 * \retval AW_OK       : 成功
 * \retval -AW_EINVAL  : 参数错误
 * \retval -AW_ENXIO   : 设备不存在
 * \retval -AW_ENOTSUP : 不支持
 */
aw_err_t awbl_sim_apdu (int            id,
                        const uint8_t *p_c_apdu,
                        int            c_apdu_len,
                        uint8_t       *p_r_apdu,
                        int           *p_r_apdu_len);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_SIM_H */

/* end of file */
