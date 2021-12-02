/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2021 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/
#ifndef __USBH_EHCI_H
#define __USBH_EHCI_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */
#include "usb_adapter_inter.h"
#include "list/usb_list.h"
#include "host/core/usbh.h"
#include "pool/usb_pool.h"
#include "usb_common.h"

//#define USBH_EHCI_FRAME_LIST_SIZE  (1024)  /* ֡�б��С��1024(4K), 512(2K), 256(1K) */
#define USBH_EHCI_TASK_STACK_SIZE  (8192)  /* EHCI���񗣴�С*/
#define USBH_EHCI_FRAME_LIST_SIZE  (256)  /* ֡�б��С��1024(4K), 512(2K), 256(1K) */
//#define USBH_EHCI_TASK_STACK_SIZE  (4096)  /* EHCI���񗣴�С*/
/* �����жϴ��䲻��ռ�ó���80%��΢֡ʱ�䣬ȫ�ٺ͵��ٲ��ó���90%֡ʱ��*/
/* ȫ��ͬ�����䲻��ռ�ó���90%��֡ʱ�䣬����ͬ�����䲻��ռ�ó���90%��΢֡ʱ��*/
#define USBH_EHCI_UFRAME_BANDWIDTH (100)   /* һ��΢֡�Ĵ���125*80% us */
#define USBH_EHCI_BANDWIDTH_SIZE   (80)    /* �洢΢֡�����������ͬʱҲ����С������ */
#define USBH_EHCI_BANDWIDTH_FRAMES (10)    /* ����֡����������΢֡��������8*/
#define USBH_EHCI_TUNE_RL_HS       4       /* nak throttle; see 4.9 */
#define USBH_EHCI_TUNE_RL_TT       0
#define USBH_EHCI_TUNE_MULT_HS     1       /* 1-3����ÿ΢֡ */
#define USBH_EHCI_TUNE_MULT_TT     1       /* �˵�ÿһ΢֡����һ������*/

/* ȫ/���ٴ�����䳣��/֧��*/
#define BW_HOST_DELAY   1000L       /* ����*/
#define BW_HUB_LS_SETUP 333L        /* ����*/

/* ���ͱ������ {qh,itd,sitd,fstn}->hw_next */
#define Q_NEXT_TYPE(dma)   ((dma) & USB_CPU_TO_LE32(3 << 1))

#define USBH_GET_EHCI_FROM_HC(hc) ((struct usbh_ehci *)(hc->controller))

union __ehci_ptr;
/** EHCI�������ṹ��*/
struct usbh_ehci {
    uint8_t                    ctrllor_type;          /* ����������*/
    struct usb_ctrllor_drv    *p_drv;                 /* EHCI ����������*/
    struct usbh               *p_hc;                  /* USB�����ṹ��*/
    usb_bool_t                 is_init;               /* �Ƿ��ʼ����־*/

    struct usb_pool            qhs;                   /* QH(����ͷ)�ڴ��*/
    struct usb_pool            qtds;                  /* qtd(���д���������)�ڴ��*/
    struct usb_pool            itds;                  /* itd(ͬ������������)�ڴ��*/
    struct usb_pool            sitds;                 /* sitd(�ָ�����ͬ��������)�ڴ��*/

    uint8_t                    evt;                   /* EHCI�¼�*/

    uint32_t                   i_thresh;
    uint32_t                   isoc_count;            /* ��ʱ���ȼ���Ĵ���*/
    int                        bandwidth_allocated;
    int                        next_uframe;           /* ��һ΢֡����*/
    uint32_t                   now_frame;             /* Ŀǰ��֡����*/
    uint32_t                   last_iso_frame;        /* ����ɨ�������֡����*/

    usb_sem_handle_t           sem;                   /* EHCI�ź���*/
    usb_mutex_handle_t         lock;                  /* EHCI������*/
    usb_task_handle_t          task;                  /* EHCI����*/

    uint32_t                   capregs;               /* USB���ܼĴ���*/
    uint32_t                   optregs;               /* EHCI�����Ĵ���*/

    uint32_t                   hcs;                   /* �����������ṹ�����*/
    usb_bool_t                 ppcd;                  /* ÿһ���˿ڱ仯���*/
    uint8_t                    ps_bit;                /* �˿��ٶ�λ*/
    uint8_t                    nports;                /* �˿�����*/
    uint32_t                   status;                /* EHCI״̬*/

    struct usbh_ehci_qh       *async;                 /* �첽����*/
    uint32_t                  *periodic;              /* ���ڵ���*/
    union __ehci_ptr          *pshadow;               /* Ӳ�����ڱ��������������֡�б�����ݽṹ*/
    uint32_t                   fls;                   /* ֡�б��С*/
    uint8_t                    bdwidth[USBH_EHCI_BANDWIDTH_SIZE];   /* ���ߴ���*/
    int                        random;

    struct usb_list_head       intr_qhs;              /* �ж�QH(����ͷ)*/

    /* ������������31λ�Ǹ�λ�仯״̬λ����30λ�ǹ���仯״̬λ��29~0λ��USB�˿�״̬���ƼĴ���ֵһ��*/
    uint32_t                   psts[15];              /* �˿�״̬*/
    /* ���������仯�ص�����*/
    void (*pfn_rh_change) (void *p_ehci, uint16_t change);
};

/** EHCI qTD(���д���������) �ṹ��*/
struct usbh_ehci_qtd {
    /* EHCI qTD���ݽṹ�淶*/
    uint32_t                hw_next;       /* ָ����һ��qTD��ָ��*/
    uint32_t                hw_alt_next;   /*  Aletmate Next qTD Pointer*/
    uint32_t                hw_token;      /* ����*/
    uint32_t                hw_buf[5];     /* qTD������ҳָ��*/
    uint32_t                hw_buf_hi[5];

    /* USB������������������*/
    struct usb_list_head    node;          /* ��ǰqTD�ڵ�*/
    struct usbh_trp        *p_trp;         /* ��������� */
    size_t                  len;           /* ��ǰqTD���ݳ���*/
};

/* EHCI����ָ��������*/
union __ehci_ptr {
    struct usbh_ehci_qh    *p_qh;        /* QH(����ͷ)�ṹ��Q_TYPE_QH */
    struct usbh_ehci_itd   *p_itd;       /* ��ʱ�����������ṹ��Q_TYPE_ITD */
    struct usbh_ehci_sitd  *p_sitd;      /* �ָ��ʱ�����������ṹ�� Q_TYPE_SITD */
    struct usbh_ehci_fstn  *p_fstn;      /* FSTN�ṹ��Q_TYPE_FSTN */
    uint32_t               *hw_next;     /* ������������*/
    void                   *ptr;
};

/**
 * ����ͷ�ṹ��
 *
 * hw_info1[31:28] ����Nak�������ֶ�
 * hw_info1[27]    ���ƶ˵��־(�Ǹ��ٿ��ƶ˵�����Ϊ1������Ϊ0)
 * hw_info1[26:16] ��������
 * hw_info1[15]
 * hw_info1[14]    ���ݷ�ת����
 * hw_info1[13:12] �˵��ٶ�
 * hw_info1[11: 8] �˵��
 * hw_info1[7]     ��������������ȡ������λ(ֻ�������ڵ��ȵĵ�/ȫ���豸)
 * hw_info1[ 6: 0] �豸��ַ
 *
 * hw_info2[31:30] �ߴ���ܵ��˷���
 *                 01b �˵�ÿһ΢֡����һ������
 *                 10b �˵�ÿһ΢֡������������
 *                 11b �˵�ÿһ΢֡������������
 * hw_info2[29:23] �˿ں�(���ڵ�/ȫ���豸�ķָ�����)
 * hw_info2[22:16] ��������ַ(���ڵ�/ȫ���豸�ķָ�����)
 * hw_info2[15: 8] �ָ��������(uFrame C-mask��ֻ�������ڵ��ȵĵ�/ȫ���豸)
 *                 ��FRIMDEX�Ĵ���λ���뵽һ��uFrame C-maskΪ1��λ��
 *                 ���������ͷ������ִ�еĺ�ѡ
 * hw_info2[ 7: 0] �жϵ�������(uFrame S-mask��������첽������Ϊ0���ж����ڵ���Ϊ��0)
 *                 ��FRIMDEX�Ĵ���λ���뵽һ��uFrame S-maskΪ1��λ��
 *                 ���������ͷ������ִ�еĺ�ѡ
 */
struct usbh_ehci_qh {
    /* for HC */
    uint32_t                    hw_next;
    uint32_t                    hw_info1;
    uint32_t                    hw_info2;
    uint32_t                    hw_cur_qtd;
    uint32_t                    hw_next_qtd;
    uint32_t                    hw_alt_next_qtd;
    uint32_t                    hw_token;
    uint32_t                    hw_buf [5];
    uint32_t                    hw_buf_hi [5];

    /* for HCD */
    union __ehci_ptr            p_next;        /* next QH or others*/
    struct usb_list_head        qtds;          /* QTDs on this QH */
    struct usbh_ehci_qtd       *dummy;         /* dummy */
    struct usbh_ehci           *p_ehci;        /* controller */
    uint8_t                     state;         /* state */
    uint8_t                     retry;          /* XactErr retry count */
    usb_bool_t                  unlink;
    /* for interrupt transfer */
    uint16_t                    f_phase;        /* frame index */
    uint8_t                     u_phase;        /* uframe index */
    uint16_t                    f_period;       /* frame period */
    uint8_t                     usecs;              /* �ж϶˵����*/
    uint8_t                     c_usecs;            /* �ָ���ɴ���*/
    struct usb_list_head        intr_node;
    struct usbh_endpoint       *p_ep;

};

/** EHCI��ʱ�����������ṹ��*/
struct usbh_ehci_itd {
    uint32_t                 hw_next;
    uint32_t                 hw_transaction[8];           /* ��ʱ��������������״̬�Ϳ����б�(EHCI 3.3.2)*/
    uint32_t                 hw_bufp[7];                  /* ��ʱ��������������ҳָ������(EHCI 3.3.3)*/
    uint32_t                 hw_bufp_hi[7];               /* 64λ�汾�����ݽṹ(��¼ B)*/

#define EHCI_ISOC_ACTIVE        (1<<31)                   /* �����ۼ������ */
#define EHCI_ISOC_BUF_ERR       (1<<30)                   /* ���ݻ������*/
#define EHCI_ISOC_BABBLE        (1<<29)                   /* babble ���*/
#define EHCI_ISOC_XACTERR       (1<<28)                   /* XactErr - �������*/
#define EHCI_ITD_LENGTH(tok)    (((tok)>>16) & 0x0fff)
#define EHCI_ITD_IOC            (1 << 15)                 /* ����ж�*/

#define ITD_ACTIVE    USB_CPU_TO_LE32(EHCI_ISOC_ACTIVE)
    union __ehci_ptr         p_next;                      /* ptr to periodic q entry */
    uint32_t                 frame;                       /* ���������*/
    struct usb_list_head     itd_list;                    /* �������ĵ�ʱ��������������*/
    struct usbh_trp         *trp;                         /* USB���������*/
    struct ehci_iso_stream  *stream;                      /* �˵����*/
    uint32_t                 index[8];                    /* �� trp->iso_frame_descde������*/
    uint32_t                 pg;                          /* ҳѡ��*/
    uint8_t                  usecs[8];
};

/**
 * EHCI�ָ��ʱ�����������ṹ��
 *
 * hw_fullspeed_ep[22:16] �����伯�����ĵ�ַ
 * hw_fullspeed_ep[11: 8] �˵��ַ
 * hw_fullspeed_ep[ 6: 0] �豸��ַ
 *
 * hw_uframe[15: 8] �ָ��������(uFrame C-Mask)
 *                  ��FRIMDEX�Ĵ���λ���뵽һ��uFrame C-maskΪ1��λ��
 *                  ������ָ��ʱ����������������ִ�еĺ�ѡ
 * hw_uframe[ 7: 0] �ָ���ʼ����(uFrame S-Mask)
 *                  ��FRIMDEX�Ĵ���λ���뵽һ��uFrame S-maskΪ1��λ��
 *                  ������ָ��ʱ����������������ִ�еĺ�ѡ
 *
 * hw_results[31]    ����ж�
 * hw_results[30]    ���ݻ���ҳָ��ѡ��
 * hw_results[25:16] Ҫ�����ȫ���ֽ�
 * hw_results[15: 8] ��¼��ִ�еķָ��������
 * hw_results[ 7: 0] ��¼��ִ�е������״̬
 *                   7 Active
 *                   6 ERR
 *                   5 Data Buffer Error
 *                   4 Babble Detected
 *                   3 Transaction Error
 *                   2 Missed Micro-Frame
 *                   1 Split Transaction State(0b Do Start Split, 1b Do Complete Split)
 */
struct usbh_ehci_sitd {
    uint32_t                 hw_next;
    uint32_t                 hw_fullspeed_ep;
    uint32_t                 hw_uframe;
    uint32_t                 hw_results;
#define SITD_IOC        (1 << 31)             /* ����ж�*/
#define SITD_PAGE       (1 << 30)             /* ���� 0/1 */
#define SITD_LENGTH(x)  (0x3ff & ((x)>>16))
#define SITD_STS_ACTIVE (1 << 7)              /* ����*/
#define SITD_STS_ERR    (1 << 6)              /* ����ת��������*/
#define SITD_STS_DBE    (1 << 5)              /* ���ݻ������(����)*/
#define SITD_STS_BABBLE (1 << 4)              /* device was babbling */
#define SITD_STS_XACT   (1 << 3)              /* �Ƿ���������Ӧ*/
#define SITD_STS_MMF    (1 << 2)              /* δ��ɷָ�����*/
#define SITD_STS_STS    (1 << 1)              /* �ָ�����״̬*/

#define SITD_ACTIVE   USB_CPU_TO_LE32(SITD_STS_ACTIVE)
    uint32_t                  hw_buf [2];
    uint32_t                  hw_backpointer;
    uint32_t                  hw_buf_hi [2];

    /* the rest is HCD-private */
    union __ehci_ptr          p_next;         /* ptr to periodic q entry */
    struct usbh_trp          *p_trp;
    struct ehci_iso_stream   *stream;         /* �˵����*/
    struct usb_list_head      sitd_list;      /* �������ķָ��ʱ��������������*/
    uint32_t                  frame;          /* ����λ��*/
    uint32_t                  index;
};

/** \brief EHCI FSTN */
struct usbh_ehci_fstn {
    uint32_t            hw_next;
    union __ehci_ptr    p_next;
};

/***************************************��������**************************************************/
/**
 * USB �ڲ�EHCI��ʼ������
 *
 * @param p_ehci        EHCI�ṹ��
 * @param reg_base      USB���ܼĴ�������ַ
 * @param task_prio     EHCI�������ȼ�
 * @param pfn_rh_change ���������仯�ص�����
 * @param ps_bit        �˿��ٶ�λ
 * @param nqhs          QH����
 * @param nqtds         qTD����
 * @param nitds         iTD����
 * @param nsitds        siTD����
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbh_ehci_init (struct usbh           *p_hc,
                          struct usbh_ehci      *p_ehci,
                          uint32_t               reg_base,
                          int                    task_prio,
                          uint8_t                ps_bit,
                          int                    nqhs,
                          int                    nqtds,
                          int                    nitds,
                          int                    nsitds);
/** ����EHCI�ػ�����*/
void usbh_ehci_guard_evt_start(struct usbh_ehci *p_ehci);
/** EHCI�жϴ�����*/
void usbh_ehci_irq (struct usbh_ehci *p_ehci);
/**
 * EHCI �ڲ����������仯��⺯��
 *
 * @param p_ehci   EHCI�ṹ��
 * @param p_change ���صı仯ֵ
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbh_ehci_rh_change_check (struct usbh *p_ehci,
                                     uint16_t    *p_change);
/** EHCI �ڲ�����������ʼ����һ��USB����ֻ��һ���˿ڣ�����˿ھ������Ӹ�������*/
void usbh_ehci_rh_init (struct usbh_ehci *p_ehci);
/** EHCI�˿ڱ仯����*/
void usbh_ehci_port_change (struct usbh_ehci *p_ehci);
/**
 * ΪEHCI������������ݽṹ�����ڴ�ռ�
 *
 * @param p_ehci EHCI�ṹ��
 * @param nqhs   QH������
 * @param nqtds  qTD������
 * @param nitds  iTD������
 * @param sitds  siTD������
 *
 * @return �ɹ�����AW_OK
 */
usb_err_t usbh_ehci_mem_init (struct usbh_ehci *p_ehci,
                              int               nqhs,
                              int               nqtds,
                              int               nitds,
                              int               nsitds);
/** EHCI���ݽṹʹ��״��*/
int usbh_ehci_mem_sta (int contrl_num);
/** ����һ������ͷ*/
struct usbh_ehci_qh *usbh_ehci_qh_alloc (struct usbh_ehci *p_ehci);
/** ����һ�����д���������*/
struct usbh_ehci_qtd *usbh_ehci_qtd_alloc (struct usbh_ehci *p_ehci);
/** �ͷ�һ������ͷ*/
usb_err_t usbh_ehci_qh_free (struct usbh_ehci    *p_ehci,
                             struct usbh_ehci_qh *p_qh);
/** �ͷ�һ�����д���������*/
usb_err_t usbh_ehci_qtd_free (struct usbh_ehci     *p_ehci,
                              struct usbh_ehci_qtd *p_qtd);
/** ����һ����ʱ����������*/
struct usbh_ehci_itd *usbh_ehci_itd_alloc (struct usbh_ehci *p_ehci);
/** �ͷ�һ����ʱ����������*/
usb_err_t usbh_ehci_itd_free (struct usbh_ehci *p_ehci, struct usbh_ehci_itd *p_itd);
/** ����һ���ָ��ʱ����������*/
struct usbh_ehci_sitd *usbh_ehci_sitd_alloc (struct usbh_ehci *p_ehci);
/** �ͷŷָ��ʱ����������*/
usb_err_t usbh_ehci_sitd_free (struct usbh_ehci *p_ehci, struct usbh_ehci_sitd *p_sitd);
/**
 * EHCI������ɺ���
 *
 * @param p_ehci EHCI�ṹ��
 * @param p_trp  ��ɵĴ��������
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbh_ehci_trp_done (struct usbh_ehci *p_ehci, struct usbh_trp  *p_trp);
/** ��ȡUSB��������������֡�б��֡����(��ȷ��΢֡)*/
uint32_t usbh_ehci_read_frame_index(struct usbh_ehci *p_ehci);
/**
 * ������ڴ���
 *
 * @param p_ehci EHCI�ṹ��
 * @param frame  ֡����
 * @param uframe ΢֡����
 *
 * @return �ɹ�����ָ��΢֡�Ĵ���ʹ�����
 */
uint16_t periodic_usecs (struct usbh_ehci *p_ehci, uint32_t frame, uint32_t uframe);
/**
 * ʹ�����ڵ���
 *
 * @param p_ehci EHCI�ṹ��
 * @param is_iso �ǲ��ǵ�ʱ����
 */
void usbh_ehci_periodic_enable(struct usbh_ehci *p_ehci, uint8_t is_iso);
/** �ر����ڵ���*/
void usbh_ehci_periodic_disable(struct usbh_ehci *p_ehci);
/**********************************************************************************************/

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_USBH_EHCI_H */

