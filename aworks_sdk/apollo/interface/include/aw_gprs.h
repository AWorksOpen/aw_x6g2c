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
 * \brief 通用GPRS定义
 *
 * \par 使用示例
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.04 17-12-09  vih, redefine the architecture
 * - 1.03 15-03-06  ops, add sms if and aw_gprs_sms_data.
 * - 1.02 15-01-23  ops, add signal strength if.
 * - 1.01 15-01-09  ops, create the gprs abstract class, and redefine the std if.
 * - 1.00 14-12-15  afm, first implementation.
 * \endinternal
 */

#ifndef __AW_GPRS_H_
#define __AW_GPRS_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_gprs
 * \copydoc aw_gprs.h
 * @{
 */

#include "aworks.h"
#include "aw_types.h"
#include "aw_netif.h"

//#include "arpa/inet.h"

/**
 * \brief gprs 错误码
 */

#define AW_GPRS_EBASE       1000
/** \brief Failed to create a new PPPoS session */
#define AW_GPRS_ECPPP       1002
/** \brief Failed to initiate a PPP connection */
#define AW_GPRS_EIPPP       1003
/** \brief Failed to establish a PPP connection */
#define AW_GPRS_EEPPP       1004
/** \brief GNSS function is not enabled */
#define AW_GPRS_EGNSS       1005
/** \brief The signal strength is below the minimum */
#define AW_GPRS_ECSQ        1006
/** \brief Failed to query the signal strength. */
#define AW_GPRS_ECSQR       1007
/** \brief Failed to obtain APN */
#define AW_GPRS_EAPN        1010
/** \brief Network registration status is abnormal */
#define AW_GPRS_ECREG       1011
/** \brief Failed to set packet domain protocol */
#define AW_GPRS_ECGDCONT    1012
/** \brief Failed to attach to PS service */
#define AW_GPRS_ECGATT      1013
/** \brief Failed to dial-up */
#define AW_GPRS_EATD        1014
/** \brief MT is waiting for (U)SIM PIN to be given */
#define AW_GPRS_EPIN        1015
/** \brief AT test failed. */
#define AW_GPRS_EAT         1016
/** \brief Failed to set socket option */
#define AW_GPRS_ESOPT       1017

/** \brief The sent SMS buffer is not available using the receive buffer. */
#define AW_GPRS_ESMSSBUF    1019
/** \brief Failed to set sms format */
#define AW_GPRS_ESMSF       1020
/** \brief The AT interaction to send an sms failed */
#define AW_GPRS_ESMSI       1021
/** \brief Insufficient SMS array space */
#define AW_GPRS_ESMSM       1022
/** \brief Unsopported SMS encoding format. */
#define AW_GPRS_ESMSC       1023
/** \brief Receiving the SMS format is not support. */
#define AW_GPRS_ESMSRC      1024
/** \brief Failed to convert UCS2 to UTF8 encoding. */
#define AW_GPRS_EUCS2UTF    1025
/** \brief Failed to get the first SMS index. */
#define AW_GPRS_ESMSGF      1026
/** \brief Failed to delete a SMS by index. */
#define AW_GPRS_ESMSD       1027
/** \brief SMS PDU data is incomplete */
#define AW_GPRS_ESMSPDU     1028
/** \brief Not support the PDU with UDHI flag. */
#define AW_GPRS_EPDUUDHI    1029
/** \brief Failed to receive PDP SMS by AT command */
#define AW_GPRS_ESMSRPDU    1030

/** \brief Failed to configure HTTPS command context.  */
#define AW_GPRS_EHTTPSC     1031
/** \brief Failed to activate HTTPS command context.  */
#define AW_GPRS_EHTTPSA     1032
/** \brief Failed to activate HTTPS SSL context.  */
#define AW_GPRS_EHTTPSSSL   1033
/** \brief Failed to obtain url data size. */
#define AW_GPRS_EHTTPSSZ    1034
/** \brief The url data size is zero. */
#define AW_GPRS_EHTTPSZERO  1035
/** \brief Failed to receive 'CONNECT' reply. */
#define AW_GPRS_EHTTPSCONN  1036
/** \brief Failed to initialize GNSS function  */
#define AW_GPRS_EGNSSI      1037
/** \brief Ping time out.  */
#define AW_GPRS_EPINGTM     1040
/** \brief Failed to enable minimum functionality.  */
#define AW_GPRS_ECFUN0      1045
/** \brief Failed to enable full functionality.  */
#define AW_GPRS_ECFUN1      1046
/** \brief Failed to reset module with +CFUN command.  */
#define AW_GPRS_ECFUNRST    1047


/**
 * \brief GPRS log 掩码
 */
typedef enum {
    AW_GPRS_LOG_API         = 1,            /**< \brief 调用 api 的结果 log */
    AW_GPRS_LOG_DBG         = (1<<1),       /**< \brief 调试信息 log */
    AW_GPRS_LOG_SYS         = (1<<2),       /**< \brief 系统信息 log */
    AW_GPRS_LOG_ASYNSYS     = (1<<5),       /**< \brief 异步处理的系统 log */
    AW_GPRS_LOG_AT_RCV      = (1<<10),      /**< \brief 接收的 AT 数据 log */
    AW_GPRS_LOG_AT_RCV_SUC  = (1<<11),      /**< \brief 接收到指定 AT 数据的 log */
    AW_GPRS_LOG_AT_RCV_TIME = (1<<12),      /**< \brief 接收 AT 数据的耗时 log */
    AW_GPRS_LOG_AT_SND      = (1<<13),      /**< \brief 发送的 AT 数据 log */
    AW_GPRS_LOG_PIPE        = (1<<14),      /**< \brief 与模组通信的管道 log */
    AW_GPRS_LOG_DIAL        = (1<<15),      /**< \brief 拨号相关的log */
    AW_GPRS_LOG_NET_PING    = (1<<16),      /**< \brief 网络 ping 的log */
    AW_GPRS_LOG_SOCK        = (1<<17),      /**< \brief sock 相关的log */
    AW_GPRS_LOG_SMS         = (1<<18),      /**< \brief 短信 log */
    AW_GPRS_LOG_PPP         = (1<<19),      /**< \brief PPP 拨号相关的 log */
    AW_GPRS_LOG_ERROR       = (1<<20),      /**< \brief 错误信息 log */
    AW_GPRS_LOG_WARN        = (1<<21),      /**< \brief 警告信息 log */
    AW_GPRS_LOG_MAX         = (1<<31),
} aw_gprs_log_t;

struct aw_gprs_sms_buf {
    char     *p_sms_buff;                   /**< \brief 短信内容缓存 */
    uint16_t  sms_buff_len;                 /**< \brief 短信内容缓存长度 */

    char     *p_num_buff;                   /**< \brief 手机号码内容缓存 */
    uint16_t  num_buff_len;                 /**< \brief 手机号码内容缓存长度 */

    char     *p_time_buff;                  /**< \brief 时间内容缓存   */
    uint16_t  time_buff_len;                /**< \brief 时间内容缓存长度 */

    /** \brief 接收的短信编码类型  */
    #define AW_GPRS_SMS_CODE_7BIT        (0)     /**< \brief 7bit 编码短信 */
    #define AW_GPRS_SMS_CODE_8BIT        (1)     /**< \brief 8bit 编码短信 */
    #define AW_GPRS_SMS_CODE_UCS2        (2)     /**< \brief UCS2编码短信 */
    #define AW_GPRS_SMS_CODE_UTF8        (3)     /**< \brief UTF8编码短信 */
    #define AW_GPRS_SMS_CODE_ATTEXT      (4)     /**< \brief AT 文本短信，只支持英文 */
    uint8_t   sms_code;                      /**< \brief 短信编码 */
};


/**
 * \brief GPRS 事件类型
 */
typedef enum {
    AW_GPRS_EVT_TYPE_NONE = 0,          /**< \brief 无效事件，该类型不发起通知  */
    AW_GPRS_EVT_TYPE_PWR_OFF_PRE,       /**< \brief 设备准备掉电  */
    AW_GPRS_EVT_TYPE_PWR_OFF,           /**< \brief 设备已掉电  */
    AW_GPRS_EVT_TYPE_PWR_ON,            /**< \brief 设备已上电  */

    AW_GPRS_EVT_TYPE_RESET_PRE,         /**< \brief 设备准备复位  */
    AW_GPRS_EVT_TYPE_RESET_COMPLETED,   /**< \brief 设备复位成功   */

    AW_GPRS_EVT_TYPE_PPP_LINK_UP,       /** \brief PPP 连接成功事件 */
    AW_GPRS_EVT_TYPE_PPP_LINK_DOWN,     /** \brief PPP 连接断开事件  */
    AW_GPRS_EVT_TYPE_ETH_LINK_UP,       /** \brief eth 连接成功事件  */
    AW_GPRS_EVT_TYPE_ETH_LINK_DOWN,     /** \brief eth 连接断开事件 */
} aw_gprs_evt_type_t;

/**
 * \brief GPRS 事件结构体
 */
typedef struct {
    aw_gprs_evt_type_t type;

    union {
        struct {
            /**< \brief 由回调函数设置，=0 则已准备好该操作，<0 则未准备好 */
            aw_err_t retval;
        } pwr_off_pre, pwr_off, reset_pre;

        /** \brief AW_GPRS_EVT_TYPE_ETH_LINK_UP 事件参数  */
        struct {
            aw_netif_t  *p_if;

            ip_addr_t    ip;
            ip_addr_t    netmask;
            ip_addr_t    gateway;
            ip_addr_t    dns;
        } eth_link_up;
    } u;

} aw_gprs_evt_t;

typedef void (*aw_gprs_evt_cb_t) (int devid, aw_gprs_evt_t *p_evt);

/**
 * \brief IO控制的选项
 */
typedef enum aw_gprs_ioctl_req {

    /** \brief 设置使用的协议栈，设为0则为外部协议栈（默认），为1则使用内部协议栈   */
    AW_GPRS_REQ_PROTOCOL_SET,

    /**
     * \brief 请求建立网络连接
     *
     * 连接成功则上报 AW_GPRS_EVT_TYPE_PPP_LINK_UP 或
     *  AW_GPRS_EVT_TYPE_ETH_LINK_UP 事件，需要先设置事件回调函数；
     *
     * 只能在使用外部协议时才能进行该操作；
     * 如果通过ioctl 进行 NET_LINK_UP 后，则可以在应用层调用socket的接口进行连接和通信，
     * 此时不再调用 aw_gprs_ip_start 、aw_gprs_ip_send、aw_gprs_ip_receive
     * 和 aw_gprs_ip_close 接口；
     */
    AW_GPRS_REQ_NET_LINK_UP,

    /** \brief 请求断开网络连接    */
    AW_GPRS_REQ_NET_LINK_DOWN,

    /** \brief 设备复位，参数给0则为软件复位，给1则为硬件复位    */
    AW_GPRS_REQ_RESET,

    /** \brief 设置 GPRS模块信号最小有效值，低于该值无法启动使网络功能   */
    AW_GPRS_REQ_CSQ_MIN_SET,

    /** \brief GPRS模块信号强度获取   */
    AW_GPRS_REQ_CSQ_GET,

    /** \brief 获取GPRS模块CCID   */
    AW_GPRS_REQ_CCID_GET,

    /** \brief 获取 APN  */
    AW_GPRS_REQ_APN_GET,

    /** \brief 设置 APN  */
    AW_GPRS_REQ_APN_SET,

    /** \brief PPP拨号的IP地址获取    */
    AW_GPRS_REQ_IP_GET,

    /** \brief 获取GPRS模块的版本号      */
    AW_GPRS_REQ_VERSION_GET,

    /** \brief 获取GPRS模块的IMEI号   */
    AW_GPRS_REQ_IMEI_GET,

    /** \brief 获取GPRS模块的IMSI号    */
    AW_GPRS_REQ_IMSI_GET,

    /** \brief 获取GPRS模块的短信息中心号码   */
    AW_GPRS_REQ_SCA_GET,

    /** \brief 获取网络模式   */
    AW_GPRS_REQ_NET_MODE_GET,

    /** \brief 获取基站信息   */
    AW_GPRS_REQ_BS_GET,

    /** \brief 请求 AT 命令通信  */
    AW_GPRS_REQ_AT_TRANS,

    /** \brief ping功能   */
    AW_GPRS_REQ_PING,

    /** \brief 查询设备状态   */
    AW_GPRS_REQ_STS,

    /**\brief 设置输出 log 的回调 */
    AW_GPRS_LOG_SET,

    /** \brief 设置 gprs 事件回调 */
    AW_GPRS_EVT_CB_SET,

    /** \brief 清除 gprs 事件回调 */
    AW_GPRS_EVT_CB_CLR,

    /** \brief 读取由4g模块解码的定位数据；需要  aw_gprs_gnss_enable() 使能 gnss 功能 */
    AW_GPRS_GNSS_LOC,

    /** \brief 重启射频功能 */
    AW_GPRS_REQ_RF_REBOOT

} aw_gprs_ioctl_req_t;

typedef union {
    /** \brief AW_GPRS_REQ_PROTOCOL_SET 选项的参数 */
    int             proto_opt;

    /** \brief AW_GPRS_REQ_RESET 选项的参数 */
    struct {
        aw_bool_t       flag_hw_rst;    /**< \brief 硬件复位标志   */
        uint32_t        delay_ms;       /**< \brief 设备复位与初始化之间的延时时间   */
    } reset;

    /** \brief AW_GPRS_REQ_CSQ_GET、AW_GPRS_REQ_CSQ_MIN_SET 选项的参数 */
    int             csq;

    /** \brief AW_GPRS_REQ_CCID_GET 选项的参数 */
    char            ccid[64];

    /** \brief AW_GPRS_REQ_APN_GET、AW_GPRS_REQ_APN_GET 选项的参数,需为静态变量地址 */
    char           *p_apn;

    /** \brief AW_GPRS_REQ_PING */
    char           *p_ping_dst;

    /** \brief AW_GPRS_REQ_IP_GET 选项的参数 */
    ip_addr_t       ip_addr;

    /** \brief AW_GPRS_REQ_VERSION_GET 选项的参数 */
    char            version[64];

    /** \brief AW_GPRS_REQ_IMEI_GET 选项的参数 */
    char            imei[64];

    /** \brief AW_GPRS_REQ_IMSI_GET 选项的参数 */
    char            imsi[64];

    /** \brief AW_GPRS_REQ_SCA_GET 选项的参数 */
    char            sca[32];

    /** \brief AW_GPRS_REQ_NET_MODE_GET 选项的参数，为字符串 */
    char            net_mode[64];

    /**
     * \brief AW_GPRS_REQ_BS_GET 选项的参数,
     *        bs_info[0] 为基站区域编号， bs_info[1] 为小区ID
     */
    uint32_t        bs_info[2];

    /** \brief AW_GPRS_REQ_AT_TRANS 选项的参数 */
    struct {
        /**< \brief 发送的 AT 命令字符串，为 NULL 则只处理接收  */
        char       *p_at_cmd;

        /** \brief 期待接收的数据，接收到则立即返回，为 NULL 则收到任意数据或超时则返回  */
        char       *p_recv_expect;
        char       *p_recv_buf;     /**< \brief 接收 buf，为 NULL 则不接收数据  */
        size_t      bufsize;        /**< \brief 接收 buf 大小  */
        uint32_t    recvlen;        /**< \brief 接收到的数据长度  */
        uint32_t    recv_tm;        /**< \brief 接收超时，ms  */
    } at_trans;

    /** \brief AW_GPRS_REQ_STS 选项的参数 */
    aw_err_t        sts;

    /** \brief AW_GPRS_LOG_SET 选项的参数 */
    void          (*pfn_log_out) (  aw_gprs_log_t mask,
                                    void         *p_color,
                                    const char   *func_name,
                                    int           line,
                                    const char   *fmt, ...);

    /** \brief AW_GPRS_EVT_CB_SET 选项的参数 */
    aw_gprs_evt_cb_t pfn_evt_handler;

    /** \brief AW_GPRS_GNSS_LOC 选项参数 */
    struct {
        char data[12];      /**< \brief Format: ddmmyy */
        char utc[12];       /**< \brief UTC time. Format: hhmmss.sss */
        char latitude[12];  /**< \brief Latitude. Format: ddmm.mmmm N/S */
        char longitude[12]; /**< \brief Longitude. Format: dddmm.mmmm E/W */
        char spkm[12];      /**< \brief Speed over ground. Format: xxxx.x, unit: Km/h */
    } gnss_loc;

} aw_gprs_ioctl_args_t;


/**
 * \brief 支持的 nmea 类型
 */
#define AW_GPRS_GNSS_NMEA_AUTO      0
#define AW_GPRS_GNSS_NMEA_GGA       1
#define AW_GPRS_GNSS_NMEA_RMC       2
#define AW_GPRS_GNSS_NMEA_GSV       4
#define AW_GPRS_GNSS_NMEA_GSA       8
#define AW_GPRS_GNSS_NMEA_VTG       16

/**
 * \brief gnss 的启动方式
 */
typedef enum {
    AW_GPRS_GNSS_STARTUP_HOT = 0,      /**< \brief 使用辅助数据，启动时间最长 */
    AW_GPRS_GNSS_STARTUP_WARM,         /**< \brief 使用部分辅助数据，启动时间最中等 */
    AW_GPRS_GNSS_STARTUP_COLD,         /**< \brief 删除所有辅助数据，启动时间最慢 */
    AW_GPRS_GNSS_STARTUP_AUTO = 0xFF,  /**< \brief 由模块自动设置  */
} aw_gprs_gnss_startup_t;

/**
 * \brief gnss 功能配置参数
 */
typedef struct {
    aw_gprs_gnss_startup_t  startup_mode;

    struct {
        aw_bool_t   flag_enable;
        uint32_t    nmea_type;      /** \brief 为 0 则使用默认值 */
    } gps;

    struct {
        aw_bool_t   flag_enable;
        uint32_t    nmea_type;
    } bd;

    struct {
        aw_bool_t   flag_enable;
        uint32_t    nmea_type;
    } glonass;

    /** \brief gprs 事件回调，不可阻塞  */
    aw_gprs_evt_cb_t  pfn_evt_cb;
} aw_gprs_gnss_cfg_t;

/**
 * \brief 异步处理结果
 */
typedef struct {
    aw_bool_t    completed;
    aw_err_t     errcode;
    const char * errdesc;
} aw_gprs_result_t;


/**
 * \brief 获取一个可用的设备
 *
 * \param[in]  timeout_sec      超时时间，单位 second
 *
 * \retval > 0        :  设备 id
 * \retval -1         :  无可用设备
 */
int aw_gprs_any_get (uint32_t timeout_sec);

/**
 * \brief gprs设备上电并初始化
 *
 * \param[in]  id           设备id
 *
 * \retval AW_OK        :  成功
 * \retval ELSE         :  查看 GPRS 错误码和 aw_errno
 */
aw_err_t aw_gprs_power_on (int   id);

/**
 * \brief gprs设备上电并初始化(异步处理)
 *
 * \param[in]  id           设备id
 * \param[out] p_result     异步处理结果
 *
 * \retval AW_OK        :  成功
 * \retval ELSE         :  查看 GPRS 错误码和 aw_errno
 */
aw_err_t aw_gprs_power_on_asyn (int id, aw_gprs_result_t *p_result);

/**
 * \brief gprs设备掉电
 *
 * \param[in]  id     设备id
 *
 * \retval AW_OK        :  成功
 * \retval ELSE         :  查看 GPRS 错误码和 aw_errno
 */
aw_err_t aw_gprs_power_off (int   id);

/**
 * \brief gprs设备掉电(异步处理)
 *
 * \param[in]  id     设备id
 * \param[out] p_result     异步处理结果
 *
 * \retval AW_OK        :  成功
 * \retval ELSE         :  查看 GPRS 错误码和 aw_errno
 */
aw_err_t aw_gprs_power_off_asyn (int   id, aw_gprs_result_t *p_result);

/**
 * \brief 建立GPRS连接
 * 使用该接口建立连接时，如果是使用的外部协议，则会自动打开进行ppp拨号
 *
 * \param[in]  id       设备id
 * \param[in]  p_mode   模式: "TCP" 或  "UDP"
 * \param[in]  p_ip     连接服务器的IP地址或域名
 * \param[in]  port     连接服务器的端口号
 * \param[in]  timeout  超时时间，单位为秒
 *
 * \retval  >= 0            : gprs连接成功，返回该连接的句柄
 * \retval  -AW_EINVAL      : 参数无效
 * \retval  -AW_ENOMSG      : GPRS设备无信号
 * \retval  -AW_ENOTSUP     : 该 p_mode 模式不支持
 */
int  aw_gprs_start (int                 id,
                    const char         *p_mode,
                    char               *p_ip,
                    uint16_t            port,
                    uint32_t            timeout);

/**
 * \brief 建立GPRS连接 (异步)
 * 使用该接口建立连接时，如果是使用的外部协议，则会自动打开进行ppp拨号
 *
 * \param[in]  id       设备id
 * \param[in]  p_mode   模式: "TCP" 或  "UDP"
 * \param[in]  p_ip     连接服务器的IP地址或域名
 * \param[in]  port     连接服务器的端口号
 * \param[in]  timeout  超时时间，单位为秒
 * \param[out] p_hd     连接成功的 socket 句柄
 * \param[out] p_result 异步处理结果
 *
 * \retval AW_OK        :  成功
 * \retval ELSE         :  查看 GPRS 错误码和 aw_errno
 */
aw_err_t  aw_gprs_start_asyn (int                 id,
                              const char         *p_mode,
                              char               *p_ip,
                              uint16_t            port,
                              uint32_t            timeout,
                              int                *p_hd,
                              aw_gprs_result_t   *p_result);

/**
 * \brief 关闭一个GPRS连接
 * 使用该接口断开连接时，如果是使用的外部协议，则会自动关闭ppp拨号
 *
 * \param[in]  id       设备id
 * \param[in]  hd       gprs连接的句柄
 *
 * \retval AW_OK    成功
 * \retval ELSE         :  查看 GPRS 错误码和 aw_errno
 */
aw_err_t aw_gprs_close (int   id, int hd);

/**
 * \brief 发送GPRS数据
 *
 * \note  使用内部协议栈时
 *
 * \param[in]  id       设备id
 * \param[in]  hd       gprs连接的句柄
 * \param[in]  p_data   发送的数据
 * \param[in]  len      发送数据长度
 * \param[in]  tm       发送超时
 *
 * \retval > 0    成功发送的数据长度
 * \retval ELSE   查看 GPRS 错误码和 aw_errno
 */
ssize_t aw_gprs_send (int id, int hd, void *p_data, size_t len, uint32_t tm);

/**
 * \brief 接收GPRS数据
 *
 * \param[in]   id       设备id
 * \param[in]   hd       gprs连接的句柄
 * \param[out]  p_buf    接收数据缓存
 * \param[in]   len      接收数据缓存大小
 *
 * \retval > 0    成功接收的数据长度
 * \retval ELSE   查看 GPRS 错误码和 aw_errno
 */
ssize_t aw_gprs_recv (int id, int hd, void *p_buf, size_t len);

/**
 * \brief 短信发送
 *
 * \param[in]  id       设备id
 * \param[in]  p_sms    短信内容
 * \param[in]  len      短信内容长度
 * \param[in]  p_num    手机号码
 * \param[in]  sms_code 短信编码格式
 *
 * \retval AW_OK    成功
 * \retval ELSE   查看 GPRS 错误码和 aw_errno
 */
aw_err_t aw_gprs_sms_send (int id, uint8_t *p_sms, uint8_t len, char *p_num,
                           uint8_t   sms_code);

/**
 * \brief 短信发送(异步)
 *
 * \param[in]  id       设备id
 * \param[in]  p_sms    短信内容
 * \param[in]  len      短信内容长度
 * \param[in]  p_num    手机号码
 * \param[in]  sms_code 短信编码格式
 * \param[out] p_result 异步处理结果
 *
 * \retval AW_OK    成功
 * \retval ELSE   查看 GPRS 错误码和 aw_errno
 */
aw_err_t aw_gprs_sms_send_asyn (int                  id,
                                uint8_t             *p_sms,
                                uint8_t              len,
                                char                *p_num,
                                uint8_t              sms_code,
                                aw_gprs_result_t    *p_result);

/**
 * \brief 短信接收
 *
 * \param[in]   id       设备id
 * \param[out]  p_sms    短信内容
 *
 * \return > 0     还有剩余短信可以继续接收
 * \retval = 0     短信接收完成
 * \retval ELSE    查看 GPRS 错误码和 aw_errno
 */
aw_err_t aw_gprs_sms_recv (int                      id,
                           struct aw_gprs_sms_buf  *p_sms,
                           uint8_t                  sms_code);
/**
 * \brief 短信接收
 *
 * \param[in]   id       设备id
 * \param[out]  p_sms    短信内容
 * \param[in]   timeout  接收到短信的超时时间
 * \param[out]  p_result 异步处理结果
 *
 * \return > 0     还有剩余短信可以继续接收
 * \retval = 0     短信接收完成
 * \retval ELSE    查看 GPRS 错误码和 aw_errno
 */
aw_err_t aw_gprs_sms_recv_asyn (int                      id,
                                struct aw_gprs_sms_buf  *p_sms,
                                uint8_t                  sms_code,
                                aw_gprs_result_t        *p_result);

/**
 * \brief 删除全部短信
 *
 * \param[in]  id       设备id
 *
 * \retval AW_OK    成功
 * \retval ELSE    查看 GPRS 错误码和 aw_errno
 */
aw_err_t aw_gprs_all_sms_del (int id);

/**
 * \brief io控制
 *
 * \param[in]     id      : 设备id
 * \param[in]     req     : io请求
 * \param[in,out] p_arg   : 数据
 *
 * \retval  AW_OK   操作成功
 * \retval  ELSE    查看 GPRS 错误码和 aw_errno
 *
 * \par 范例
 * \code
 *
 *  // 发送 AT 命令并接收响应示例
 *  aw_err_t ret;
 *  aw_gprs_ioctl_args_t arg;
 *  int  dev_id = 0;   //使用id为0的gprs设备，该id从 awbl_hwconf_gprs_xxx.h 里查找
 *
 *  //接收AT命令结果
 *  char  rec_buf[256];
 *
 *  memset(rec_buf, 0, 256);
 *
 *  arg.at_trans.p_at_cmd        = "AT+CSQ\r\n";
 *  arg.at_trans.p_recv_expect   = "CSQ:";
 *  arg.at_trans.p_recv_buf      = rec_buf;
 *  arg.at_trans.bufsize         = sizeof(rec_buf);
 *  arg.at_trans.recv_tm         = 1500;
 *
 *  ret = aw_gprs_ioctl(dev_id, AW_GPRS_REQ_AT_TRANS, &arg);
 *  if (ret == AW_OK) {
 *      AW_INFOF(("recv : %s.\n", rec_buf));
 *  }
 *
 * \endcode
 */
aw_err_t aw_gprs_ioctl (int                   id,
                        aw_gprs_ioctl_req_t   req,
                        aw_gprs_ioctl_args_t *p_arg);

/**
 * \brief 使用 at 命令发送 HTTS 请求
 *
 * \param[in]     id        : 设备id
 * \param[in]     p_url     : 请求的 url
 * \param[in,out] url_len   : url 长度
 * \param[in]     p_response_cb: 响应处理回调
 *
 * \retval  AW_OK   操作成功
 * \retval  ELSE    查看 GPRS 错误码和 aw_errno
 */
aw_err_t aw_gprs_https_get (int              id,
                            char            *p_url,
                            size_t           url_len,
                            void (          *p_response_cb) (void *rdata,
                                                             size_t rsize));

/**
 * \brief io控制(异步处理)
 *
 * \param[in]     id      : 设备id
 * \param[in]     req     : io请求
 * \param[in,out] p_arg   : 数据
 * \param[in,out] p_result: 异步处理结果
 *
 * \retval  AW_OK        : 操作成功
 * \return  ELSE         : 查看 errno
 */
aw_err_t aw_gprs_ioctl_asyn (int                   id,
                             aw_gprs_ioctl_req_t   req,
                             aw_gprs_ioctl_args_t *p_arg,
                             aw_gprs_result_t     *p_result);

/**
 * \brief 使能 gps 功能
 *
 * \param dev_id            4g 模块 id
 * \param p_gnss_cfg        gnss 配置
 *
 * \retval  AW_OK           操作成功
 * \retval  -AW_EINVAL      参数异常
 * \retval  -AW_ENODEV      无设备
 */
aw_err_t aw_gprs_gnss_enable (int dev_id, aw_gprs_gnss_cfg_t *p_gnss_cfg);

/**
 * \brief 关闭 gps 功能
 *
 * \param dev_id            4g 模块 id
 *
 * \retval  AW_OK           操作成功
 * \retval  -AW_EINVAL      参数异常
 * \retval  -AW_ENODEV      无设备
 */
aw_err_t aw_gprs_gnss_disable (int dev_id);

/**
 * \brief 获取 gnss 源数据
 *
 * \param dev_id            4g 模块 id
 * \param p_buf             存放buf
 * \param bufsize           buf大小
 * \param timeout           ms 超时
 *
 * \retval  >=0             返回获取数据长度
 * \retval  -AW_EINVAL      参数异常
 * \retval  -AW_ENODEV      无设备
 */
aw_err_t aw_gprs_gnss_nmeasrc_get (int      dev_id,
                                   uint8_t *p_buf,
                                   size_t   bufsize,
                                   uint32_t timeout);

/** @} grp_aw_if_gprs */

#ifdef __cplusplus
}
#endif

#endif /* __AW_GPRS_H_ */

/* end of file */

