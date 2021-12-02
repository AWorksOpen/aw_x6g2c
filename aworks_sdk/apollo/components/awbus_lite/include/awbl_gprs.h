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
 * \brief gprs�ӿ�
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
 * \brief ʹ�� eth ģʽ����ʹ����Ĭ��ֻ֧�� ppp ģʽ
 */
#define AWBL_GPRS_LL_TYPE_ETH_ENABLE       0


/**
 * \brief ʹ��ʹ��  usbh_serial �ӿڣ����� Ϊ usbh_pipe �ӿ�
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

/** \brief GPRS�豸���� link layer ����  */
typedef enum {
    AWBL_GPRS_LL_TYPE_PPP = 1,   /* PPPģʽ(��Ե�Э��)*/
    AWBL_GPRS_LL_TYPE_ETH,       /* ETHERNETģʽ(��̫��ģʽ)*/
} awbl_gprs_ll_type_t;


/** \brief GPRSģ��Ĳ�����Ϣ  */
struct awbl_gprs_info
{
    char     *devname;
    char     *p_dial_num;             /**< \brief ����ҵ�񲦺ź���*/
    char     *p_apn;                  /**< \brief APN����*/
    char     *p_user;                 /**< \brief APN�û���*/
    char     *p_password;             /**< \brief APN����*/
    char     *p_center_num;           /**< \brief ����Ϣ���ĺ���*/
    char     *p_at_err[3];            /**< \brief AT �������ʱ���յ����ַ���*/

    uint32_t  power_on_mdelay;        /**< \brief �ϵ��ȴ�ģ���ʼ������ʱ*/
    uint32_t  sf_reset_mdelay;        /**< \brief �����λ�󣬵ȴ��豸���Ƴ�����ʱ  */
};

#if AWBL_GPRS_LL_TYPE_ETH_ENABLE

/* ��̫���׶�״̬*/
typedef enum {
    AWBL_GPRS_ETHERNET_PHASE_UNINIT  = 0,
    AWBL_GPRS_ETHERNET_PHASE_INIT    = 1,
    AWBL_GPRS_ETHERNET_PHASE_DIAL_UP = 2,   /* ��̫������*/
} awbl_gprs_ethernet_phase_t;

/* ��¼��̫��ģʽ�ĵ�ַ*/
struct ethernet_addrs {
    struct ip_addr our_ipaddr;        /* IP��ַ*/
    struct ip_addr his_ipaddr;
    struct ip_addr netmask;           /* ��������*/
    struct ip_addr gateway;           /* ����*/
    struct ip_addr dns1;
    struct ip_addr dns2;
};


/* GPRS�豸��̫���ṹ��*/
struct awbl_gprs_ethernet {
    struct ethernet_addrs             ethernetaddr;     /* ��̫����ص�ַ */
    awbl_gprs_ethernet_phase_t        phase;              /* ��̫��ģʽ�׶�*/
    /* gprs�豸���Žӿ�  */
    aw_err_t (*pfn_dial_up)   (struct awbl_gprs_dev *p_this);
    /* gprs�豸�رղ��Žӿ�  */
    aw_err_t (*pfn_dial_down) (struct awbl_gprs_dev *p_this);
};

#endif /* AWBL_GPRS_LL_TYPE_ETH_ENABLE */

/**
 * \brief  ppp �׶�״̬
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
    struct sio_mgn               sio_mgn;       /**< \brief sio���� */

#if AWBL_GPRS_USE_LWIP_1_4
    int                          ppp_fd;        /**< \brief PPP��� */
    struct ppp_addrs            *p_pppaddr;     /**< \brief PPP��ȡ���ĵ�ַ */

    /** \brief ����ppp���ӹرյ��ź��� */
    AW_SEMB_DECL(link_close);
#endif /* AWBL_GPRS_USE_LWIP_1_4 */

    awbl_gprs_ppp_phase_t        phase;

#if AWBL_GPRS_USE_LWIP_2_X
    /** \brief ppp���ƿ�ָ�� */
    ppp_pcb     *p_pcb;

    /** \brief ppp���� */
    struct netif netif;

    /** \brief ����buf,����������ݴ�Ŵ�������� PPP_MRU + PPP_HDRLEN */
    u_char       rxbuf[2048];

    /** \brief ����ppp�߳� */
    AW_TASK_DECL(thread, PPP_THREAD_STACKSIZE);

    /** \brief ����ppp�̵߳Ĺر��ź��� */
    AW_SEMB_DECL(thread_close);
#endif /* AWBL_GPRS_USE_LWIP_2_X */

    /** \brief gprs�豸���Žӿ�  */
    aw_err_t (*pfn_dial_up)   (struct awbl_gprs_dev *p_this);

    /** \brief gprs�豸�رղ��Žӿ�  */
    aw_err_t (*pfn_dial_down) (struct awbl_gprs_dev *p_this);
};


/**
 * \brief GPRS�豸�����������ӿ�
 */
struct awbl_gprs_drvopr {

    /**
     * \brief �ܵ��ӿ�
     */

    /** \brief �򿪺͹ر� at �ܵ�  */
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
     * \brief �������ܻص�
     */

    /** \brief Ԥ��ʼ�� */
    aw_err_t (*pfn_init_prev) (struct awbl_gprs_dev *p_this);

    /** \brief ��ȡ GPRS link layer ����  */
    aw_err_t (*pfn_ll_type_get)(struct awbl_gprs_dev *p_this);

#if AWBL_GPRS_LL_TYPE_ETH_ENABLE
    /** \brief ETH ģʽ��س�ʼ�� */
    aw_err_t (*pfn_eth_mode_init)(struct awbl_gprs_dev *p_this);
    aw_err_t (*pfn_eth_mode_uninit)(struct awbl_gprs_dev *p_this);

    /** \brief gprs�豸 eth ���Žӿ�  */
    aw_err_t (*pfn_eth_dial_up)   (struct awbl_gprs_dev *p_this);
    aw_err_t (*pfn_eth_dial_down) (struct awbl_gprs_dev *p_this);
#endif /* AWBL_GPRS_LL_TYPE_ETH_ENABLE */

    /** \brief PPP ģʽ��س�ʼ�� */
    aw_err_t (*pfn_ppp_mode_init)(struct awbl_gprs_dev *p_this);
    aw_err_t (*pfn_ppp_mode_uninit)(struct awbl_gprs_dev *p_this);

    /** \brief gprs�豸 ppp ���Žӿ�  */
    aw_err_t (*pfn_ppp_dial_up)   (struct awbl_gprs_dev *p_this);
    aw_err_t (*pfn_ppp_dial_down) (struct awbl_gprs_dev *p_this);

    /** \brief ppp ���ŵ������ʼ��  */
    aw_err_t (*pfn_ppp_dial_up_spec_init) (struct awbl_gprs_dev *p_this);

    /** \brief ��ȡ apn  */
    aw_err_t (*pfn_apn_get) (struct awbl_gprs_dev *p_this);


    /** \brief ppp ʹ�õ� sio ��д�ӿ�  */
    u32_t (*pfn_sio_read)(void *fd, u8_t *data, u32_t len);
    u32_t (*pfn_sio_write)(void *fd, u8_t *data, u32_t len);

    /** \brief ��ʼ�� at ģ�������  */
    aw_err_t (*pfn_init_at_cfg) (struct awbl_gprs_dev *p_this);

    /** \brief ��ѯ����ע��״̬  */
    aw_err_t (*pfn_creg_chk) (struct awbl_gprs_dev *p_this);

    /** \brief ��ѯ����ע��״̬  */
    aw_err_t (*pfn_cgreg_chk) (struct awbl_gprs_dev *p_this);

    /** \brief ��ѯ����ע��״̬  */
    aw_err_t (*pfn_cereg_chk) (struct awbl_gprs_dev *p_this);

    /** \brief ���� PDP ������ */
    aw_err_t (*pfn_cgdcont_set) (struct awbl_gprs_dev *p_this);

    /** \brief ���� MT �� Packet domain service. */
    aw_err_t (*pfn_cgatt_set) (struct awbl_gprs_dev *p_this);

    /** \brief ����  */
    aw_err_t (*pfn_atd_set) (struct awbl_gprs_dev *p_this);

    /** \brief �Ͽ�����  */
    aw_err_t (*pfn_ath_set) (struct awbl_gprs_dev *p_this);

    /** \brief ���� AT ����  */
    aw_err_t (*pfn_at_disp_set) (struct awbl_gprs_dev *p_this,
                                 char                  ate,
                                 char                  atv);

    /** \brief ��ѯ��Ʒ ID ��Ϣ  */
    aw_err_t (*pfn_ati_get) (struct awbl_gprs_dev   *p_this,
                             char                   *p_data,
                             size_t                  dsize);

    /** \brief ���� phone full functionality level  */
    aw_err_t (*pfn_cfun_full_set) (struct awbl_gprs_dev   *p_this);

    /** \brief ��ѯ�ź�ǿ�Ƚӿ�  */
    aw_err_t (*pfn_csq_get) (struct awbl_gprs_dev   *p_this,
                             int                    *p_csq);

    /** \brief ��� SIM ���Ƿ�����  */
    aw_err_t (*pfn_cpin_chk) (struct awbl_gprs_dev   *p_this);

    /** \brief ͨ�� AT ������и�λ  */
    aw_err_t (*pfn_reset_by_at) (struct awbl_gprs_dev   *p_this);

    /** \brief ���� at text ���Ÿ�ʽ  */
    aw_err_t (*pfn_sms_text_init) (struct awbl_gprs_dev   *p_this);
    /** \brief ���� pdu ���Ÿ�ʽ  */
    aw_err_t (*pfn_sms_pdu_init) (struct awbl_gprs_dev   *p_this);
    /** \brief ��ȡ��һ�����ŵ�����ֵ  */
    aw_err_t (*pfn_sms_first_index_get) (struct awbl_gprs_dev   *p_this,
                                         int                    *p_index);
    /** \brief ɾ��һ������ */
    aw_err_t (*pfn_sms_del) (struct awbl_gprs_dev   *p_this, int index);
    /** \brief text ��ʽ���ŷ��� */
    aw_err_t (*pfn_sms_text_send) (struct awbl_gprs_dev   *p_this,
                                   uint8_t                *p_sms,
                                   size_t                  len,
                                   char                   *p_num);
    /** \brief text ��ʽ���Ž��� */
    aw_err_t (*pfn_sms_text_recv) (struct awbl_gprs_dev   *p_this,
                                   int                     index,
                                   struct aw_gprs_sms_buf *p_sms);
    /** \brief pdu ��ʽ���ŷ���  */
    aw_err_t (*pfn_sms_pdu_send) (struct awbl_gprs_dev   *p_this,
                                  char                   *pdu,
                                  size_t                  len);
    /** \brief pdu ��ʽ���Ž���  */
    aw_err_t (*pfn_sms_pdu_recv) (struct awbl_gprs_dev   *p_this,
                                  int                     index,
                                  uint8_t                *pdu,
                                  size_t                  len);

    /** \brief ͨ�� AT ���� TCP �� UDP ����  */
    aw_err_t (*pfn_tcpudp_conn) (struct awbl_gprs_dev   *p_this,
                                 uint8_t                 mode,
                                 char                   *p_ip,
                                 uint16_t                port,
                                 uint32_t                timeout);
    /** \brief �Ͽ� AT ������� TCP �� UDP ����  */
    aw_err_t (*pfn_tcpudp_disconn) (struct awbl_gprs_dev   *p_this, int hd);
    /** \brief AT ����� TCP �� udp ����  */
    int (*pfn_tcpudp_send) (struct awbl_gprs_dev   *p_this,
                            int                     hd,
                            void                   *p_data,
                            size_t                  len,
                            uint32_t                timeout);
    /** \brief AT ������� TCP �� udp ����  */
    int (*pfn_tcpudp_recv) (struct awbl_gprs_dev   *p_this,
                            int                     hd,
                            void                   *p_buf,
                            size_t                  len);

    /** \brief ��ʼ�� https ���������� */
    aw_err_t (*pfn_https_ctx_init) (struct awbl_gprs_dev   *p_this);
        /** \brief ���� https ���������� */
    aw_err_t (*pfn_https_ctx_act) (struct awbl_gprs_dev   *p_this);
    /** \brief ���� https ����� ssl ������ */
    aw_err_t (*pfn_https_ctx_ssl_act) (struct awbl_gprs_dev   *p_this);
    /** \brief ��ȡ�� url ���ݵĴ�С */
    aw_err_t (*pfn_https_url_dsize_get) (struct awbl_gprs_dev   *p_this,
                                         char                   *p_url,
                                         size_t                  urllen,
                                         size_t                 *p_url_dsize);
    /** \brief ��ȡ url ������ */
    int (*pfn_https_url_read) (struct awbl_gprs_dev   *p_this,
                               uint8_t                *p_buf,
                               size_t                  rlen);

    /** \brief ��ȡ ccid */
    aw_err_t (*pfn_ccid_get) (struct awbl_gprs_dev *p_this,
                              char                 *buf,
                              size_t                size);
    /** \brief ��ȡ IMEI */
    aw_err_t (*pfn_imei_get) (struct awbl_gprs_dev *p_this,
                              char                 *buf,
                              size_t                size);
    /** \brief ��ȡ IMSI */
    aw_err_t (*pfn_imsi_get) (struct awbl_gprs_dev *p_this,
                              char                 *buf,
                              size_t                size);
    /** \brief ��ȡ 4G ģ������汾  */
    aw_err_t (*pfn_module_version_get) (struct awbl_gprs_dev *p_this,
                                        char                 *buf,
                                        size_t                size);

    aw_err_t (*pfn_radio_reboot) (struct awbl_gprs_dev *p_this);


    /**
     * \brief AT ����ͽӿڽӿ�
     */
    int (*pfn_at_send) (struct awbl_gprs_dev *p_this,
                        char                 *p_atmsg);
    int (*pfn_at_recv) (struct awbl_gprs_dev     *p_this,
                        char                     *p_buf,
                        uint32_t                  buf_size,
                        char                     *p_cond[],
                        uint32_t                  arg_num,
                        uint32_t                  wait_ms);

    /** \brief ̽���豸�Ƿ����  */
    aw_err_t (*pfn_probe) (struct awbl_gprs_dev   *p_this);

    /**
     * \brief Ӳ����صĻص�
     */
    void    (*pfn_hw_reset) (void);
    void    (*pfn_hw_power_on) (void);
    void    (*pfn_hw_power_off) (void);
    void    (*pfn_hw_write_en) (void);      /**< \brief дʹ�� */

    /**
     * \brief GPS ������ؽӿ�
     */

    /** \brief ʹ�� gps ����  */
    aw_err_t (*pfn_gnss_enable) (struct awbl_gprs_dev   *p_this,
                                 aw_gprs_gnss_cfg_t     *p_gnss_cfg);

    /** \brief �ر� gps ���� */
    aw_err_t (*pfn_gnss_disable) (struct awbl_gprs_dev   *p_this);

    /** \brief ��ȡ gnss Դ���� */
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

/** \brief GPRS usbh ��Ϣ�ṹ��*/
typedef struct {
    uint32_t    vid;
    uint32_t    pid;

    /** \brief com ֵΪ -1 ʱ��Ч  */
    int8_t      at_com;
    int8_t      ppp_com;
    int8_t      dm_com;
    int8_t      gnss_com;

} awbl_gprs_usbh_info;

/** \brief GPRS�豸�ṹ��   */
struct awbl_gprs_dev {

    struct awbl_gprs_dev        *p_next;

    /** \brief GPRS�豸�����ӿ�   */
    struct awbl_gprs_drvopr      drvopr;

    /** \brief GPRSģ��Ĳ�����Ϣ  */
    struct awbl_gprs_info        info;

    /** \brief GPRS usbh ��Ϣ�ṹ��*/
    awbl_gprs_usbh_info          usbh_info;

    /** \brief ���ŵ����ݺ���Ϣ */
    struct aw_gprs_sms_buf       sms;

    /** \brief �첽��������� id */
    uint32_t                     reqid;

    /** \brief �������token*/
    uint32_t                     token;

    /** \brief GPRS link layer ����*/
    awbl_gprs_ll_type_t          ll_type;

    /** \brief ��ǰʹ�õ�ͨ�Źܵ� */
    void                        *snd_pipe;

    /** \brief at ����ͨ�Źܵ� */
    void                        *at_pipe;

    /** \brief gnss ����ͨ�Źܵ� */
    void                        *gnss_pipe;

    /** \brief ״̬��־ */
    uint32_t                     init_step:4,       /**< \brief ��ʼ������ */
                                 init_completed:1,  /**< \brief ��ʼ����ɱ�־ */
//                                 power_on:1,      /**< \brief �Ѿ��ϵ��־ */
                                 ppp_dial_up:1,
                                 eth_dial_up:1,
                                 sf_reset:1,        /**< \brief �����λ��־ */
                                 hw_reset:1,
                                 sms_pdu_init:1,
                                 sms_text_init:1,
                                 pipe_valid:1,
                                 htts_init_step:3;   /**< \brief Ӳ����λ��־ */

    /** \brief �ܵ�����������ܵ��Ƿ����  */
    void                        *pipe_monitor;

    /** \brief ״̬��־��飬����Ϊ0  */
    const uint8_t                flag_check;

    /** \brief at ����� */
    char                        *at_buf;
    size_t                       at_bufsz;

    struct awbl_gprs_ppp         ppp;

#if AWBL_GPRS_LL_TYPE_ETH_ENABLE
    /** \brief GPRS�豸��̫���ṹ��*/
    struct awbl_gprs_ethernet    ethernet;
#endif /* AWBL_GPRS_LL_TYPE_ETH_ENABLE */

    /** \brief gprs�豸��ΨһID�����ڸ��м���ȡ�豸 */
    int                          dev_id;

    /** \brief gprs��׼�ӿ�ʹ�õĻ����� */
    AW_MUTEX_DECL(               mutex);

    aw_bool_t                    mutex_inited;

    /** \brief �²�gprs�豸��ָ��  */
    void                        *p_gprs_dev;

    /** \brief socktet���ӵ����� */
    uint8_t                      socket_num;

    /** \brief �Ƿ�ʹ���ⲿЭ��ջ��־ */
    aw_bool_t                    use_external_protocol;



    /** \brief gnss ������ʹ�ܱ�־ */
    aw_bool_t                    gnss_en;

    aw_err_t                     status;

    aw_tick_t                    last_status_tick;

    /** \brief ��¼��һ�ε� evt�������¼��ص�������ʱ���ϱ���һ�ε� evt */
    aw_gprs_evt_t                last_evt;

    /** \brief �ź���С��Чֵ�����ڸ�ֵ�޷�����ʹ���繦��  */
    int                          csq_min;

#define AWBL_GPRS_EVT_CB_MAX    6
    /** \brief gprs �¼��ص����飬��������  */
    aw_gprs_evt_cb_t             pfn_evt_cb_array[AWBL_GPRS_EVT_CB_MAX];

    awbl_gprs_log_cb_t           pfn_log_out;
};

/**
 * \brief ����PDU��protocol data unit����
 *
 * \param[in]  p_pdu_data     pdu����
 * \param[in]  p_to_sms       ��Ž��͵���pdu���ݵĽṹ��
 *
 * \retval AW_OK        :  �ɹ�
 * \retval -AW_EBADMSG  :  ������ȷ�����ݰ�
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
