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
 * ʹ�ñ�ģ����Ҫ��������ͷ�ļ���
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
    uint32_t wwt;   /**< \brief ��Ϊ0����ʾcwt��bwt��ʹ�ø�ֵ*/
    uint32_t cwt;   /**< \brief ��ͬ�������������ַ�֮�������ʱ */
    uint32_t cgt;   /**< \brief ��ͬ���������������ַ�֮����С��ʱ */
    uint32_t bwt;   /**< \brief �෴�������������ַ�֮�������ʱ */
    uint32_t bgt;   /**< \brief �෴�������������ַ�֮����С��ʱ */
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
    uint8_t di;     /**< \brief ���ʵ������� */
    uint8_t fi;     /**< \brief ʱ������ת������ */
} awbl_sim_baud_t;

/**
 * \brief AWBus SIM ����ʵ���������Ϣ
 */
struct awbl_sim_servinfo {

    /** \brief ZLG600A �豸��� */
    int dev_id;
};

/**
 * \brief AWBus SIM ������
 */
struct awbl_sim_servopts {

    /** \brief sim ioctl function*/
    aw_err_t (*pfn_sim_iotcl) (void *p_cookie, int request, void *p_arg);
};

/**
 * \brief ZLG600A����ṹ��
 */
typedef struct awbl_sim_service {

    /** \brief ָ����һ�� SIM ���� */
    struct awbl_sim_service *p_next;

    /** \brief ZLG600A ����������Ϣ */
    const struct awbl_sim_servinfo *p_servinfo;

    /** \brief ZLG600A �������غ��� */
    const struct awbl_sim_servopts *p_servopts;
    
    /** \brief �豸�� */
    AW_MUTEX_DECL(devlock);

    /** \brief Э������ */
    int   protocol_type;
    
    uint8_t ifsc;  /**< \brief ��������Ϣ���� */
    uint8_t tc3;   /**< \brief ֡βУ��ģʽ */

    void *p_cookie;

} awbl_sim_service_t;

/**
 * \brief ����ATR�ṹ��
 */
aw_local aw_inline void awbl_sim_mkatr (awbl_sim_atr_t *p_atr,
                                        void           *p_rxbuf)
{
    p_atr->p_atr_buf = (uint8_t *)p_rxbuf;
}


/**
 * \brief ����simʱ����صĲ���
 *
 * \param[in] p_timing : ʱ������ṹ��ָ��
 * \param[in] wwt      : ��Ϊ0����ʾcwt��bwt��ʹ�ø�ֵ, ��λetu
 * \param[in] cwt      : ��ͬ�������������ַ�֮�������ʱ, ��λetu
 * \param[in] cgt      : ��ͬ���������������ַ�֮����С��ʱ, ��λetu
 * \param[in] bwt      : �෴�������������ַ�֮�������ʱ, ��λetu
 * \param[in] bgt      : �෴�������������ַ�֮����С��ʱ, ��λetu
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
 * \brief ���ô���ṹ��
 *
 * \param[in] p_xmt      : ����ṹ��ָ��
 * \param[in] p_txbuf    : ������ָ��
 * \param[in] n_tx       : �����ֽ���
 * \param[in] timeout_ms : ��ʱʱ�䣨��λ��ms��
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
 * \brief ���ý��սṹ��
 *
 * \param[in] p_xmt      : ���սṹ��ָ��
 * \param[in] p_txbuf    : ������ָ��
 * \param[in] n_tx       : �����ֽ��������������ڴ����յ��ֽ���
 * \param[in] timeout_ms : ��ʱʱ�䣨��λ��ms��
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
 * \brief ���ò����ʲ����ṹ��
 *
 * \param[in] p_baud : �����ʽṹ��ָ��
 * \param[in] di     : ���ʵ�������
 * \param[in] fi     : ʱ������ת������
 *
 * \par ����
 * \code
 * #include "awbl_sim.h"
 *
 * awbl_sim_baud_t baud; // �����ʲ����ṹ��
 *
 * // fclk=4000000, fi=1(f=372), di=1��������Ϊ10752
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
 * \brief AWBus SIM �ӿ�ģ���ʼ��
 *
 * \attention ������Ӧ���� awbl_dev_connect() ֮�����
 */
void awbl_sim_init (void);

/**
 * \brief sim io control ����
 *
 * \param[in] id      : �豸ID
 * \param[in] request : ����
 *                      @arg AWBL_SIM_IOCTL_GET_PRESENSE�� ��ǰ������״̬�� ����Ϊint��ָ��
 *                      @arg AWBL_SIM_IOCTL_GET_ATR�� ��ȡ��λӦ�����ݣ� ����Ϊawbl_sim_atr_t����ָ��
 *                      @arg AWBL_SIM_IOCTL_XMT�� TTY�������ݵ�IC��������Ϊawbl_sim_xmt_t����ָ��
 *                      @arg AWBL_SIM_IOCTL_RCV�� TTY�������ݵ�IC��������Ϊawbl_sim_rcv_t����ָ��
 *                      @arg AWBL_SIM_IOCTL_ACTIVATE�� ��Ƭ�ϵ缤������, �޲���
 *                      @arg AWBL_SIM_IOCTL_DEACTIVATE�� ��Ƭ����ȡ���������У� �޲���
 *                      @arg AWBL_SIM_IOCTL_WARM_RESET�� ��Ƭ�ȸ�λ�� �޲���
 *                      @arg AWBL_SIM_IOCTL_COLD_RESET�� ��Ƭ�临λ���޲���
 *                      @arg AWBL_SIM_IOCTL_CARD_LOCK��������Ƭ���޲���
 *                      @arg AWBL_SIM_IOCTL_CARD_EJECT��������Ƭ���޲���
 *                      @arg AWBL_SIM_IOCTL_SET_PROTOCOL������ͨ��Э��
 *                      @arg AWBL_SIM_IOCTL_SET_TIMING������ʱ�����������Ϊawbl_sim_timing_t����ָ��
 *                      @arg AWBL_SIM_IOCTL_SET_BAUD������ͨ�����ʣ�����Ϊawbl_sim_baud_t����ָ��
 *                      @arg AWBL_SIM_IOCTL_WAIT��������ʱ
 * \param[in] p_arg : ioctl��������������������ͺ�ȡֵȡ���ڶ�Ӧ����������
 *
 * \return ����ֵΪ������룬�� aw_errno.h ������ȡ��������������ͷ���ֵ��
 *  - \b AW_OK      : �ɹ�
 *  - \b -AW_EINVAL : ��������
 *  - \b -AW_ENXIO  : �豸������
 *  - \b -AW_EIO    : �豸ִ������ʧ��
 */
aw_err_t awbl_sim_ioctl (int id, int request, void *p_arg);

/**
 * \brief sim reset card
 *
 * \param[in] id     : �豸ID
 * \param[in] p_buad : ��λʹ�õĲ����ʣ�ȱʡ���fi=1��di=1
 * \param[in] flags  : ��λ����
 *                      @arg AWBL_SIM_IOCTL_WARM_RESET�� ��Ƭ�ȸ�λ
 *                      @arg AWBL_SIM_IOCTL_COLD_RESET�� ��Ƭ�临λ
 * \param[in] p_atr  : ��λӦ�����ݣ�Ϊawbl_sim_atr_t����ָ��
 *
 * \retval AW_OK      : �ɹ�
 * \retval -AW_EINVAL : ��������
 * \retval -AW_ENXIO  : �豸������
 * \retval -AW_EIO    : �豸ִ������ʧ��
 */
aw_err_t awbl_sim_reset (int              id,
                         awbl_sim_baud_t *p_buad,
                         int              flags,
                         awbl_sim_atr_t  *p_atr);
                         
/**
 * \brief ISO7816-3 �Ӵ�ʽIC��PPS�������Ӧ����
 *
 * \param[in]  id    : �豸ID
 * \param[in] p_pps  : PPS������4 �ֽڣ�
 *                     PPS[0] �� ָ���Ƿ����PPS1��PPS2��PPS3
 *                     PPS[0].3:0 �� ����
 *                     PPS[0].4 = 1 �� PPS1 ���ڣ�0 �� PPS1 ������
 *                     PPS[0].5 = 1 �� PPS2 ���ڣ�0 �� PPS2 ������
 *                     PPS[0].6 = 1 �� PPS3 ���ڣ�0 �� PPS3 ������
 *                     PPS[0].7 �� ����
 *                     PPS[1] �� Fi/Di
 *                     PPS[2] �� N
 *                     PPS[3] �� ����
 *
 * \retval AW_OK       : �ɹ�
 * \retval -AW_EINVAL  : ��������
 * \retval -AW_ENXIO   : �豸������
 * \retval -AW_EIO     : �豸ִ������ʧ��
 * \retval -AW_EAGAIN  : ������
 * \retval -AW_ENOTSUP : ��֧��
 *
 * \par ����
 * \code
 * #include "awbl_sim.h"
 *
 * //���Ӵ�ʽIC��ͨ�Ų����ʸ�Ϊ115200bps��FiΪ1��DiΪ8��
 * uint8_t pps[4]=[0x10, 0x18, 0x00, 0x00];
 *
 * awbl_sim_pps_set(0, pps);
 * \endcode
 */
aw_err_t awbl_sim_pps_set (int id, uint8_t *p_pps);

/**
 * \brief ����һ��APDU����
 *
 * \param[in]  id           : �豸ID
 * \param[in]  p_c_apdu     : ����
 * \param[in]  c_apdu_len   : �����
 * \param[out] p_r_apdu     : Ӧ��
 * \param[out] p_r_apdu_len : Ӧ�𳤶�
 *
 * \retval AW_OK       : �ɹ�
 * \retval -AW_EINVAL  : ��������
 * \retval -AW_ENXIO   : �豸������
 * \retval -AW_ENOTSUP : ��֧��
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
