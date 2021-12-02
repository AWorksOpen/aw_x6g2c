/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief gprs接口
 *
 * \internal
 * \par modification history:
 * - 1.00 2017-11-29   vih, first implementation.
 * \endinternal
 */

#ifndef __AWBL_GPRS_H
#define __AWBL_GPRS_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_awbl_if_gprs
 * \copydoc aw_gprs.h
 * @{
 */

#include "aworks.h"

#include "aw_gprs.h"
#include "aw_sem.h"
#include "ppp/sio_mgn.h"
#include "aw_reqsched.h"

#define AWBL_GPRS_VERSION "1.1.3"

#ifdef LWIP_2_X
#define AWBL_GPRS_USE_LWIP_1_4     0
#define AWBL_GPRS_USE_LWIP_2_X     1
#else
#define AWBL_GPRS_USE_LWIP_1_4     1
#define AWBL_GPRS_USE_LWIP_2_X     0
#endif

/**
 * \brief 使能 eth 模式，不使能则默认只支持 ppp 模式
 */
#define AWBL_GPRS_LL_TYPE_ETH_ENABLE       0


/**
 * \brief 使能使用  usbh_serial 接口，否则 为 usbh_pipe 接口
 */
#define AWBL_GPRS_USB_USBH_SERIAL     1


#if AWBL_GPRS_USE_LWIP_1_4
#include "ppp/ppp.h"
#endif /* AWBL_GPRS_USE_LWIP_1_4 */

#if AWBL_GPRS_USE_LWIP_2_X
#include "netif/ppp/pppos.h"
#include "lwip/sio.h"
#endif /* AWBL_GPRS_USE_LWIP_2_X */

struct awbl_gprs_dev;

enum {
    AWBL_GPRS_SIM800_ID_BASE    = 0,
    AWBL_GPRS_U9300C_ID_BASE    = 10,
    AWBL_GPRS_EC20_ID_BASE      = 20,
    AWBL_GPRS_ME909S_ID_BASE    = 30,
    AWBL_GPRS_N58_ID_BASE       = 40,
    AWBL_GPRS_GM510_ID_BASE     = 50,
    AWBL_GPRS_L610_ID_BASE      = 60,
};

/** \brief GPRS设备网络 link layer 类型  */
typedef enum {
    AWBL_GPRS_LL_TYPE_PPP = 1,   /* PPP模式(点对点协议)*/
    AWBL_GPRS_LL_TYPE_ETH,       /* ETHERNET模式(以太网模式)*/
} awbl_gprs_ll_type_t;


/** \brief GPRS模块的参数信息  */
struct awbl_gprs_info
{
    char     *devname;
    char     *p_dial_num;             /**< \brief 数据业务拨号号码*/
    char     *p_apn;                  /**< \brief APN名称*/
    char     *p_user;                 /**< \brief APN用户名*/
    char     *p_password;             /**< \brief APN密码*/
    char     *p_center_num;           /**< \brief 短信息中心号码*/
    char     *p_at_err[3];            /**< \brief AT 命令出错时接收到的字符串*/

    uint32_t  power_on_mdelay;        /**< \brief 上电后等待模组初始化的延时*/
    uint32_t  sf_reset_mdelay;        /**< \brief 软件复位后，等待设备被移除的延时  */
};

#if AWBL_GPRS_LL_TYPE_ETH_ENABLE

/* 以太网阶段状态*/
typedef enum {
    AWBL_GPRS_ETHERNET_PHASE_UNINIT  = 0,
    AWBL_GPRS_ETHERNET_PHASE_INIT    = 1,
    AWBL_GPRS_ETHERNET_PHASE_DIAL_UP = 2,   /* 以太网拨号*/
} awbl_gprs_ethernet_phase_t;

/* 记录以太网模式的地址*/
struct ethernet_addrs {
    struct ip_addr our_ipaddr;        /* IP地址*/
    struct ip_addr his_ipaddr;
    struct ip_addr netmask;           /* 子网掩码*/
    struct ip_addr gateway;           /* 网关*/
    struct ip_addr dns1;
    struct ip_addr dns2;
};


/* GPRS设备以太网结构体*/
struct awbl_gprs_ethernet {
    struct ethernet_addrs             ethernetaddr;     /* 以太网相关地址 */
    awbl_gprs_ethernet_phase_t        phase;              /* 以太网模式阶段*/
    /* gprs设备拨号接口  */
    aw_err_t (*pfn_dial_up)   (struct awbl_gprs_dev *p_this);
    /* gprs设备关闭拨号接口  */
    aw_err_t (*pfn_dial_down) (struct awbl_gprs_dev *p_this);
};

#endif /* AWBL_GPRS_LL_TYPE_ETH_ENABLE */

/**
 * \brief  ppp 阶段状态
 */
typedef enum {
    AWBL_GPRS_PPP_PHASE_UNINIT = 0,
    AWBL_GPRS_PPP_PHASE_INIT,
    AWBL_GPRS_PPP_PHASE_DIAL_UP,
    AWBL_GPRS_PPP_PHASE_CREATE,
    AWBL_GPRS_PPP_PHASE_CONNECT,
    AWBL_GPRS_PPP_PHASE_RUNNING,
} awbl_gprs_ppp_phase_t;


struct awbl_gprs_ppp {
    struct sio_mgn               sio_mgn;       /**< \brief sio管理 */

#if AWBL_GPRS_USE_LWIP_1_4
    int                          ppp_fd;        /**< \brief PPP句柄 */
    struct ppp_addrs            *p_pppaddr;     /**< \brief PPP获取到的地址 */

    /** \brief 定义ppp连接关闭的信号量 */
    AW_SEMB_DECL(link_close);
#endif /* AWBL_GPRS_USE_LWIP_1_4 */

    awbl_gprs_ppp_phase_t        phase;

#if AWBL_GPRS_USE_LWIP_2_X
    /** \brief ppp控制块指针 */
    ppp_pcb     *p_pcb;

    /** \brief ppp网卡 */
    struct netif netif;

    /** \brief 接收buf,被编码的数据存放处，需大于 PPP_MRU + PPP_HDRLEN */
    u_char       rxbuf[2048];

    /** \brief 定义ppp线程 */
    AW_TASK_DECL(thread, PPP_THREAD_STACKSIZE);

    /** \brief 定义ppp线程的关闭信号量 */
    AW_SEMB_DECL(thread_close);
#endif /* AWBL_GPRS_USE_LWIP_2_X */

    /** \brief gprs设备拨号接口  */
    aw_err_t (*pfn_dial_up)   (struct awbl_gprs_dev *p_this);

    /** \brief gprs设备关闭拨号接口  */
    aw_err_t (*pfn_dial_down) (struct awbl_gprs_dev *p_this);
};


/**
 * \brief GPRS设备的驱动操作接口
 */
struct awbl_gprs_drvopr {

    /**
     * \brief 管道接口
     */

    /** \brief 打开和关闭 at 管道  */
    void * (*pfn_pipe_open)(int         pid,
                            int         vid,
                            int         inum,
                            aw_bool_t   rngbuf,
                            int         size,
                            int         timeout);
    void   (*pfn_pipe_close)(void  *p_pipe);

    int (*pfn_pipe_send) (void  *p_pipe, uint8_t *p_data, size_t dsize);
    int (*pfn_pipe_recv) (void  *p_pipe, uint8_t *p_data, size_t dsize);

    /**
     * \brief 驱动功能回调
     */

    /** \brief 预初始化 */
    aw_err_t (*pfn_init_prev) (struct awbl_gprs_dev *p_this);

    /** \brief 获取 GPRS link layer 类型  */
    aw_err_t (*pfn_ll_type_get)(struct awbl_gprs_dev *p_this);

#if AWBL_GPRS_LL_TYPE_ETH_ENABLE
    /** \brief ETH 模式相关初始化 */
    aw_err_t (*pfn_eth_mode_init)(struct awbl_gprs_dev *p_this);
    aw_err_t (*pfn_eth_mode_uninit)(struct awbl_gprs_dev *p_this);

    /** \brief gprs设备 eth 拨号接口  */
    aw_err_t (*pfn_eth_dial_up)   (struct awbl_gprs_dev *p_this);
    aw_err_t (*pfn_eth_dial_down) (struct awbl_gprs_dev *p_this);
#endif /* AWBL_GPRS_LL_TYPE_ETH_ENABLE */

    /** \brief PPP 模式相关初始化 */
    aw_err_t (*pfn_ppp_mode_init)(struct awbl_gprs_dev *p_this);
    aw_err_t (*pfn_ppp_mode_uninit)(struct awbl_gprs_dev *p_this);

    /** \brief gprs设备 ppp 拨号接口  */
    aw_err_t (*pfn_ppp_dial_up)   (struct awbl_gprs_dev *p_this);
    aw_err_t (*pfn_ppp_dial_down) (struct awbl_gprs_dev *p_this);

    /** \brief ppp 拨号的特殊初始化  */
    aw_err_t (*pfn_ppp_dial_up_spec_init) (struct awbl_gprs_dev *p_this);

    /** \brief 获取 apn  */
    aw_err_t (*pfn_apn_get) (struct awbl_gprs_dev *p_this);


    /** \brief ppp 使用的 sio 读写接口  */
    u32_t (*pfn_sio_read)(void *fd, u8_t *data, u32_t len);
    u32_t (*pfn_sio_write)(void *fd, u8_t *data, u32_t len);

    /** \brief 初始化 at 模块的配置  */
    aw_err_t (*pfn_init_at_cfg) (struct awbl_gprs_dev *p_this);

    /** \brief 查询网络注册状态  */
    aw_err_t (*pfn_creg_chk) (struct awbl_gprs_dev *p_this);

    /** \brief 查询网络注册状态  */
    aw_err_t (*pfn_cgreg_chk) (struct awbl_gprs_dev *p_this);

    /** \brief 查询网络注册状态  */
    aw_err_t (*pfn_cereg_chk) (struct awbl_gprs_dev *p_this);

    /** \brief 定义 PDP 上下文 */
    aw_err_t (*pfn_cgdcont_set) (struct awbl_gprs_dev *p_this);

    /** \brief 附着 MT 到 Packet domain service. */
    aw_err_t (*pfn_cgatt_set) (struct awbl_gprs_dev *p_this);

    /** \brief 拨号  */
    aw_err_t (*pfn_atd_set) (struct awbl_gprs_dev *p_this);

    /** \brief 断开拨号  */
    aw_err_t (*pfn_ath_set) (struct awbl_gprs_dev *p_this);

    /** \brief 设置 AT 回显  */
    aw_err_t (*pfn_at_disp_set) (struct awbl_gprs_dev *p_this,
                                 char                  ate,
                                 char                  atv);

    /** \brief 查询产品 ID 信息  */
    aw_err_t (*pfn_ati_get) (struct awbl_gprs_dev   *p_this,
                             char                   *p_data,
                             size_t                  dsize);

    /** \brief 设置 phone full functionality level  */
    aw_err_t (*pfn_cfun_full_set) (struct awbl_gprs_dev   *p_this);

    /** \brief 查询信号强度接口  */
    aw_err_t (*pfn_csq_get) (struct awbl_gprs_dev   *p_this,
                             int                    *p_csq);

    /** \brief 检查 SIM 卡是否正常  */
    aw_err_t (*pfn_cpin_chk) (struct awbl_gprs_dev   *p_this);

    /** \brief 通过 AT 命令进行复位  */
    aw_err_t (*pfn_reset_by_at) (struct awbl_gprs_dev   *p_this);

    /** \brief 设置 at text 短信格式  */
    aw_err_t (*pfn_sms_text_init) (struct awbl_gprs_dev   *p_this);
    /** \brief 设置 pdu 短信格式  */
    aw_err_t (*pfn_sms_pdu_init) (struct awbl_gprs_dev   *p_this);
    /** \brief 获取第一条短信的索引值  */
    aw_err_t (*pfn_sms_first_index_get) (struct awbl_gprs_dev   *p_this,
                                         int                    *p_index);
    /** \brief 删除一条短信 */
    aw_err_t (*pfn_sms_del) (struct awbl_gprs_dev   *p_this, int index);
    /** \brief text 格式短信发送 */
    aw_err_t (*pfn_sms_text_send) (struct awbl_gprs_dev   *p_this,
                                   uint8_t                *p_sms,
                                   size_t                  len,
                                   char                   *p_num);
    /** \brief text 格式短信接收 */
    aw_err_t (*pfn_sms_text_recv) (struct awbl_gprs_dev   *p_this,
                                   int                     index,
                                   struct aw_gprs_sms_buf *p_sms);
    /** \brief pdu 格式短信发送  */
    aw_err_t (*pfn_sms_pdu_send) (struct awbl_gprs_dev   *p_this,
                                  char                   *pdu,
                                  size_t                  len);
    /** \brief pdu 格式短信接收  */
    aw_err_t (*pfn_sms_pdu_recv) (struct awbl_gprs_dev   *p_this,
                                  int                     index,
                                  uint8_t                *pdu,
                                  size_t                  len);

    /** \brief 通过 AT 建立 TCP 或 UDP 连接  */
    aw_err_t (*pfn_tcpudp_conn) (struct awbl_gprs_dev   *p_this,
                                 uint8_t                 mode,
                                 char                   *p_ip,
                                 uint16_t                port,
                                 uint32_t                timeout);
    /** \brief 断开 AT 命令建立的 TCP 或 UDP 连接  */
    aw_err_t (*pfn_tcpudp_disconn) (struct awbl_gprs_dev   *p_this, int hd);
    /** \brief AT 命令发送 TCP 或 udp 数据  */
    int (*pfn_tcpudp_send) (struct awbl_gprs_dev   *p_this,
                            int                     hd,
                            void                   *p_data,
                            size_t                  len,
                            uint32_t                timeout);
    /** \brief AT 命令接收 TCP 或 udp 数据  */
    int (*pfn_tcpudp_recv) (struct awbl_gprs_dev   *p_this,
                            int                     hd,
                            void                   *p_buf,
                            size_t                  len);

    /** \brief 初始化 https 命令上下文 */
    aw_err_t (*pfn_https_ctx_init) (struct awbl_gprs_dev   *p_this);
        /** \brief 激活 https 命令上下文 */
    aw_err_t (*pfn_https_ctx_act) (struct awbl_gprs_dev   *p_this);
    /** \brief 激活 https 命令的 ssl 上下文 */
    aw_err_t (*pfn_https_ctx_ssl_act) (struct awbl_gprs_dev   *p_this);
    /** \brief 获取该 url 数据的大小 */
    aw_err_t (*pfn_https_url_dsize_get) (struct awbl_gprs_dev   *p_this,
                                         char                   *p_url,
                                         size_t                  urllen,
                                         size_t                 *p_url_dsize);
    /** \brief 读取 url 的数据 */
    int (*pfn_https_url_read) (struct awbl_gprs_dev   *p_this,
                               uint8_t                *p_buf,
                               size_t                  rlen);

    /** \brief 获取 ccid */
    aw_err_t (*pfn_ccid_get) (struct awbl_gprs_dev *p_this,
                              char                 *buf,
                              size_t                size);
    /** \brief 获取 IMEI */
    aw_err_t (*pfn_imei_get) (struct awbl_gprs_dev *p_this,
                              char                 *buf,
                              size_t                size);
    /** \brief 获取 IMSI */
    aw_err_t (*pfn_imsi_get) (struct awbl_gprs_dev *p_this,
                              char                 *buf,
                              size_t                size);
    /** \brief 获取 4G 模块软件版本  */
    aw_err_t (*pfn_module_version_get) (struct awbl_gprs_dev *p_this,
                                        char                 *buf,
                                        size_t                size);

    aw_err_t (*pfn_radio_reboot) (struct awbl_gprs_dev *p_this);


    /**
     * \brief AT 命令发送接口接口
     */
    int (*pfn_at_send) (struct awbl_gprs_dev *p_this,
                        char                 *p_atmsg);
    int (*pfn_at_recv) (struct awbl_gprs_dev     *p_this,
                        char                     *p_buf,
                        uint32_t                  buf_size,
                        char                     *p_cond[],
                        uint32_t                  arg_num,
                        uint32_t                  wait_ms);

    /** \brief 探测设备是否可用  */
    aw_err_t (*pfn_probe) (struct awbl_gprs_dev   *p_this);

    /**
     * \brief 硬件相关的回调
     */
    void    (*pfn_hw_reset) (void);
    void    (*pfn_hw_power_on) (void);
    void    (*pfn_hw_power_off) (void);
    void    (*pfn_hw_write_en) (void);      /**< \brief 写使能 */

    /**
     * \brief GPS 功能相关接口
     */

    /** \brief 使能 gps 功能  */
    aw_err_t (*pfn_gnss_enable) (struct awbl_gprs_dev   *p_this,
                                 aw_gprs_gnss_cfg_t     *p_gnss_cfg);

    /** \brief 关闭 gps 功能 */
    aw_err_t (*pfn_gnss_disable) (struct awbl_gprs_dev   *p_this);

    /** \brief 获取 gnss 源数据 */
    aw_err_t (*pfn_gnss_nmeasrc_get) (struct awbl_gprs_dev  *p_this,
                                      uint8_t               *p_buf,
                                      size_t                 bufsize,
                                      uint32_t               timeout);
};

typedef void (*awbl_gprs_log_cb_t) (aw_gprs_log_t mask,
                                    void         *p_color,
                                    const char   *func_name,
                                    int           line,
                                    const char   *fmt, ...);

/** \brief GPRS usbh 信息结构体*/
typedef struct {
    uint32_t    vid;
    uint32_t    pid;

    /** \brief com 值为 -1 时无效  */
    int8_t      at_com;
    int8_t      ppp_com;
    int8_t      dm_com;
    int8_t      gnss_com;

} awbl_gprs_usbh_info;

/** \brief GPRS设备结构体   */
struct awbl_gprs_dev {

    struct awbl_gprs_dev        *p_next;

    /** \brief GPRS设备驱动接口   */
    struct awbl_gprs_drvopr      drvopr;

    /** \brief GPRS模块的参数信息  */
    struct awbl_gprs_info        info;

    /** \brief GPRS usbh 信息结构体*/
    awbl_gprs_usbh_info          usbh_info;

    /** \brief 短信的数据和信息 */
    struct aw_gprs_sms_buf       sms;

    /** \brief 异步处理的请求 id */
    uint32_t                     reqid;

    /** \brief 存放请求token*/
    uint32_t                     token;

    /** \brief GPRS link layer 类型*/
    awbl_gprs_ll_type_t          ll_type;

    /** \brief 当前使用的通信管道 */
    void                        *snd_pipe;

    /** \brief at 命令通信管道 */
    void                        *at_pipe;

    /** \brief gnss 命令通信管道 */
    void                        *gnss_pipe;

    /** \brief 状态标志 */
    uint32_t                     init_step:4,       /**< \brief 初始化步骤 */
                                 init_completed:1,  /**< \brief 初始化完成标志 */
//                                 power_on:1,      /**< \brief 已经上电标志 */
                                 ppp_dial_up:1,
                                 eth_dial_up:1,
                                 sf_reset:1,        /**< \brief 软件复位标志 */
                                 hw_reset:1,
                                 sms_pdu_init:1,
                                 sms_text_init:1,
                                 pipe_valid:1,
                                 htts_init_step:3;   /**< \brief 硬件复位标志 */

    /** \brief 管道监测器，监测管道是否可用  */
    void                        *pipe_monitor;

    /** \brief 状态标志检查，必须为0  */
    const uint8_t                flag_check;

    /** \brief at 命令缓存 */
    char                        *at_buf;
    size_t                       at_bufsz;

    struct awbl_gprs_ppp         ppp;

#if AWBL_GPRS_LL_TYPE_ETH_ENABLE
    /** \brief GPRS设备以太网结构体*/
    struct awbl_gprs_ethernet    ethernet;
#endif /* AWBL_GPRS_LL_TYPE_ETH_ENABLE */

    /** \brief gprs设备的唯一ID，用于给中间层获取设备 */
    int                          dev_id;

    /** \brief gprs标准接口使用的互斥锁 */
    AW_MUTEX_DECL(               mutex);

    aw_bool_t                    mutex_inited;

    /** \brief 下层gprs设备的指针  */
    void                        *p_gprs_dev;

    /** \brief socktet连接的数量 */
    uint8_t                      socket_num;

    /** \brief 是否使用外部协议栈标志 */
    aw_bool_t                    use_external_protocol;



    /** \brief gnss 功能已使能标志 */
    aw_bool_t                    gnss_en;

    aw_err_t                     status;

    aw_tick_t                    last_status_tick;

    /** \brief 记录上一次的 evt，在有事件回调被设置时，上报上一次的 evt */
    aw_gprs_evt_t                last_evt;

    /** \brief 信号最小有效值，低于该值无法启动使网络功能  */
    int                          csq_min;

#define AWBL_GPRS_EVT_CB_MAX    6
    /** \brief gprs 事件回调数组，不可阻塞  */
    aw_gprs_evt_cb_t             pfn_evt_cb_array[AWBL_GPRS_EVT_CB_MAX];

    awbl_gprs_log_cb_t           pfn_log_out;
};

/**
 * \brief 解释PDU（protocol data unit）包
 *
 * \param[in]  p_pdu_data     pdu数据
 * \param[in]  p_to_sms       存放解释到的pdu数据的结构体
 *
 * \retval AW_OK        :  成功
 * \retval -AW_EBADMSG  :  不是正确的数据包
 */
aw_err_t awbl_gprs_pdu_unpack (char                   *p_pdu_data,
                               struct aw_gprs_sms_buf *p_to_sms);

void awbl_gprs_init (void);

aw_err_t awbl_gprs_default_drvopr_set (struct awbl_gprs_dev *p_this);
aw_err_t awbl_gprs_default_init (struct awbl_gprs_dev *p_this);
aw_err_t awbl_gprs_snd_pipe_install (struct awbl_gprs_dev *p_this,
                                     void                 *p_pipe);
/** @}  grp_awbl_if_gprs */

#ifdef __cplusplus
}
#endif /* __cplusplus   */

#endif /* __AWBL_GPRS_H */

/* end of file */
