/*******************************************************************************
*                                 Apollo
*                       ---------------------------
*                       innovating embedded systems
*
* Copyright (c) 2001-2021 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/
/*******************************************************************************
  includes
*******************************************************************************/
#include "apollo.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "aw_vdebug.h"
#include "aw_mem.h"
#include "aw_cache.h"
#include "device/core/usbd.h"
#include "driver/usb/awbl_imx6ul_usbd.h"
//#include "awbl_imx6ul_usbd.h"
#include "string.h"
#include "aw_pool.h"


/* 6ul USB�豸ģʽ�Ĵ���*/
struct __imx6ul_usbd_reg {
    uint32_t id;
    uint32_t general;
    uint32_t host;
    uint32_t device;
    uint32_t txbuf;
    uint32_t rxbuf;
    uint32_t reserved1[26];
    uint32_t gptimer0ld;
    uint32_t gptimer0ctrl;
    uint32_t gptimer1ld;
    uint32_t gptimer1ctrl;
    uint32_t sbuscfg;
    uint32_t reserved2[27];

    uint32_t caplength;
    uint32_t hcsparams;
    uint32_t hccparams;
    uint32_t reserved3[5];
    uint32_t dciversion;
    uint32_t dccparams;
#define __DCCPARAMS_DC                  0x00000080
#define __DCCPARAMS_DEN_MASK            0x0000001f
    uint32_t reserved4[6];

    uint32_t usbcmd;
#define __USBCMD_ITC                    0x00FF0000
#define __USBCMD_ATDTW                  0x00004000
#define __USBCMD_SUTW                   0x00002000
#define __USBCMD_ASYNC_SCH_PARK_EN      0x00000800
#define __USBCMD_ASP                    0x00000300
#define __USBCMD_INT_AA_DOORBELL        0x00000040
#define __USBCMD_ASYNC_SCHEDULE_EN      0x00000020
#define __USBCMD_PERIODIC_SCHEDULE_EN   0x00000010
#define __USBCMD_CTRL_RESET             0x00000002
#define __USBCMD_RUN_STOP               0x00000001



    uint32_t usbsts;
#define __USBSTS_AS                     0x00008000
#define __USBSTS_PS                     0x00004000
#define __USBSTS_RCL                    0x00002000
#define __USBSTS_HCH                    0x00001000
#define __USBSTS_SLI                    0x00000100
#define __USBSTS_SRI                    0x00000080
#define __USBSTS_URI                    0x00000040
#define __USBSTS_AAI                    0x00000020
#define __USBSTS_SEI                    0x00000010
#define __USBSTS_FRI                    0x00000008
#define __USBSTS_PCI                    0x00000004
#define __USBSTS_UEI                    0x00000002
#define __USBSTS_UI                     0x00000001

    uint32_t usbintr;
#define __USBINTR_SLE                   0x00000100
#define __USBINTR_SRE                   0x00000080
#define __USBINTR_URE                   0x00000040
#define __USBINTR_AAE                   0x00000020
#define __USBINTR_SEE                   0x00000010
#define __USBINTR_FRE                   0x00000008
#define __USBINTR_PCE                   0x00000004
#define __USBINTR_UEE                   0x00000002
#define __USBINTR_UE                    0x00000001

    uint32_t frindex;
    uint32_t reserved5[1];

    uint32_t deviceaddr;
#define __DEVICE_ADDRESS_ADR_MASK       0xFE000000
#define __DEVICE_ADDRESS_ADR_POS        25
#define __DEVICE_ADDRESS_ADRA_MASK      0x01000000
#define __DEVICE_ADDRESS_ADRA_POS       24

    uint32_t endpointlistaddr;
#define __EP_LIST_ADDRESS_MASK      0xfffff800
    uint32_t ttctrl;
    uint32_t burstsize;
    uint32_t txttfilltuning;
    uint32_t reserved6[6];

    uint32_t configflag;
    uint32_t portsc1;
#define __PORTSCX_PTS               0xC0000000
#define __PORTSCX_PTW               0x10000000
#define __PORTSCX_PSPD              0x0C000000
    #define __PORTSCX_PSPD_FULL     0x00000000
    #define __PORTSCX_PSPD_LOW      0x04000000
    #define __PORTSCX_PSPD_HIGH     0x08000000
    #define __PORTSCX_PSPD_UNDEF    0x0C000000
    #define __PORTSCX_PSPD_POS      26
#define __PORTSCX_PFSC              0x01000000
#define __PORTSCX_PHCD              0x00800000
#define __PORTSCX_WKOC              0x00400000
#define __PORTSCX_WKDS              0x00200000
#define __PORTSCX_WKCN              0x00100000
#define __PORTSCX_PTC               0x000F0000
#define __PORTSCX_PIC               0x0000C000
#define __PORTSCX_PO                0x00002000
#define __PORTSCX_PP                0x00001000
#define __PORTSCX_LS                0x00000C00
#define __PORTSCX_PR                0x00000100
#define __PORTSCX_SUSP              0x00000080
#define __PORTSCX_FPR               0x00000040
#define __PORTSCX_OCC               0x00000020
#define __PORTSCX_OCA               0x00000010
#define __PORTSCX_PEC               0x00000008
#define __PORTSCX_PE                0x00000004
#define __PORTSCX_CCS               0x00000002
#define __PORTSCX_CSC               0x00000001
    uint32_t reserved7[7];

    uint32_t otgsc;
    uint32_t usbmode;
#define __USBMODE_STREAM_DISABLE    0x00000010
#define __USBMODE_SETUP_LOCK_OFF    0x00000008
#define __USBMODE_CTRL_MODE_RSV     0x00000001
#define __USBMODE_CTRL_MODE_HOST    0x00000003
#define __USBMODE_CTRL_MODE_DEVICE  0x00000002
#define __USBMODE_CTRL_MODE_IDLE    0x00000000

    uint32_t endptsetupstat;
#define __EP_SETUP_STATUS_MASK      0x0000003F
#define __EP_SETUP_STATUS_EP0       0x00000001

    uint32_t endpointprime;
    uint32_t endptflush;
    uint32_t endptstatus;
    uint32_t endptcomplete;
    uint32_t endptctrl[8];
#define __EPCTRL_TXE                0x00800000
#define __EPCTRL_TXR                0x00400000
#define __EPCTRL_TXI                0x00200000
#define __EPCTRL_TXT_POS            18
#define __EPCTRL_TXT                0x000C0000
#define __EPCTRL_TXD                0x00020000
#define __EPCTRL_TXS                0x00010000
#define __EPCTRL_RXE                0x00000080
#define __EPCTRL_RXR                0x00000040
#define __EPCTRL_RXI                0x00000020
#define __EPCTRL_RXT_POS            2
#define __EPCTRL_RXT                0x0000000C
#define __EPCTRL_RXD                0x00000002
#define __EPCTRL_RXS                0x00000001
};


#ifndef __IMX6UL_USBPHY_CTRL
#define __IMX6UL_USBPHY_CTRL                 (0x30)          /* USB PHY Control register offsets */
#define __IMX6UL_USBPHY_CTRL_SET             (0x34)
#define __IMX6UL_USBPHY_CTRL_CLR             (0x38)
#define __IMX6UL_USBPHY_CTRL_SFTRST          (1u<<31)
#define __IMX6UL_USBPHY_CTRL_CLKGATE         (1u<<30)
#define __IMX6UL_USBPHY_CTRL_FSDLL_RST_EN    (1u<<24)
#define __IMX6UL_USBPHY_CTRL_ENUTMILEVEL2    (1u<<15)
#define __IMX6UL_USBPHY_CTRL_ENUTMILEVEL3    (1u<<14)
#define __IMX6UL_USBPHY_CTRL_HOSTDISCONIRQ   (1u<<3)
#define __IMX6UL_USBPHY_CTRL_ENHOSTDISCON    (1u<<1)

#define __IMX6UL_USBPHY_PWD                  (0x00)
#define __IMX6UL_USBPHY_PWD_SET              (0x04)
#define __IMX6UL_USBPHY_PWD_CLR              (0x08)
#define __IMX6UL_USBPHY_PWD_RXPWDRX          (1<<20)
#define __IMX6UL_USBPHY_PWD_RXPWDDIFF        (1<<19)
#define __IMX6UL_USBPHY_PWD_RXPWD1PT1        (1<<18)
#define __IMX6UL_USBPHY_PWD_RXPWDENV         (1<<17)
#define __IMX6UL_USBPHY_PWD_TXPWDV2I         (1<<12)
#define __IMX6UL_USBPHY_PWD_TXPWDIBIAS       (1<<11)
#define __IMX6UL_USBPHY_PWD_TXPWDFS          (1<<10)

#define __IMX6UL_USB2_PHY_ULPI               0
#define __IMX6UL_USB2_PHY_UTMI               1
#define __IMX6UL_USB2_PHY_UTMI_WIDE          2
#define __IMX6UL_USB2_PHY_SERIAL             3
#endif


#define __IMX6UL_TD_MAX_NUM  32


/*******************************************************************************
  macro operate
*******************************************************************************/
/* ��ȡawbl_imx6ul_usbd�ṹ��*/
#define __IMX6UL_USBD_DECL(p_imx_usbd, p_usbd) \
    struct awbl_imx6ul_usbd *p_imx_usbd = \
        (struct awbl_imx6ul_usbd *)AW_CONTAINER_OF(p_usbd, struct awbl_imx6ul_usbd, usbd)
/* ��ȡ������Ϣ*/
#define __IMX6UL_USBD_INFO_DECL(p_info, p_usbd) \
    struct awbl_imx6ul_usbd_info *p_info = \
        (struct awbl_imx6ul_usbd_info *)AWBL_DEVINFO_GET(&((p_usbd)->awdev));

#define __IMX6UL_USBD_REG_DECL(p_reg, p_info) \
        volatile __iomem struct __imx6ul_usbd_reg *p_reg = \
            (volatile __iomem struct __imx6ul_usbd_reg *)(p_info->usb_regbase);

#define __IMX6UL_USBD_EP_DECL(p_imx_ep, p_ep) \
    struct awbl_imx6ul_usbd_ep *p_imx_ep = \
        (struct awbl_imx6ul_usbd_ep *)AW_CONTAINER_OF(p_ep, struct awbl_imx6ul_usbd_ep, ep)

/* �Ĵ���������*/
#define __IMX_REG32_WR(data, addr)  writel(data, (volatile void __iomem *)addr)
/* �Ĵ���д����*/
#define __IMX_REG32_RD(addr)        readl((volatile void __iomem *)addr)

/** ��ʼ�������������ڴ��*/
aw_local aw_err_t __td_pool_init (struct awbl_imx6ul_usbd *p_imx_usbd)
{
    __IMX6UL_USBD_INFO_DECL(p_info, &p_imx_usbd->usbd);

    size_t  pool_size, item_size;
    void    *p_pool_mem;
#define __TS_MAX_DEF  30
    /* ����32�ֽڶ���*/
    item_size = sizeof(struct awbl_imx6ul_usbd_td);
    if (item_size & 0x1F) {
        item_size = (item_size & ~0x1F) + 0x20;
    }

    if (p_info->td_max) {
        pool_size = item_size * p_info->td_max;
    } else {
        pool_size = item_size * __TS_MAX_DEF;
    }

    p_pool_mem = aw_cache_dma_malloc(pool_size);
    if (p_pool_mem == NULL) {
        return -AW_ENOMEM;
    }
    /* ��ʼ���ڴ��*/
    if (NULL == aw_pool_init(   &p_imx_usbd->td_pool,
                                p_pool_mem,
                                pool_size,
                                item_size)) {
        return -AW_ENOMEM;
    }

    return AW_OK;
}

/** ���䴫��������*/
aw_local struct awbl_imx6ul_usbd_td *__td_alloc (struct awbl_imx6ul_usbd *p_imx_usbd)
{
    struct awbl_imx6ul_usbd_td *p_td;
    p_td = (struct awbl_imx6ul_usbd_td *)
            aw_pool_item_get (&p_imx_usbd->td_pool);

    return p_td;
}

/** �ͷŴ���������*/
aw_local void __td_free (   struct awbl_imx6ul_usbd      *p_imx_usbd,
                            struct awbl_imx6ul_usbd_td   *p_td)
{
    aw_pool_item_return (&p_imx_usbd->td_pool, p_td);
}

/** ��ȡ�˵�*/
aw_local struct awbl_imx6ul_usbd_ep *
__get_ep (  struct awbl_imx6ul_usbd  *p_imx_usbd,
            uint8_t                 ep_addr)
{
    if ((ep_addr & 0x0F) >= (p_imx_usbd->neps >> 1)) {
        return NULL;
    }
    return &(p_imx_usbd->eps[(((ep_addr & 0x0F) << 1) + (ep_addr >> 7)) & 0x1F]);
}

/** ��ʼ������ͷ*/
aw_local void __qh_init (   struct awbl_imx6ul_usbd  *p_imx_usbd,
                            uint8_t                 ep_addr,
                            uint8_t                 ep_type,
                            uint32_t                max_pkt_len,
                            aw_bool_t                  zlt,
                            uint8_t                 mult)
{
    struct awbl_imx6ul_usbd_qh   *p_qh;
    uint32_t                    tmp;

    p_qh = __get_ep(p_imx_usbd, ep_addr)->p_qh;
    tmp = (max_pkt_len << __DQH_MAX_PKT_POS);

    /* ���ö���ͷ�Ķ˵�����*/
    switch (ep_type) {
    case USB_EP_TYPE_CTRL:
        /* ���ƶ˵㣬�����ж�Interrupt On Setup (IOS)*/
        tmp |= __DQH_ISO_MASK;
        break;
    case USB_EP_TYPE_ISO:
        tmp |= (mult << __DQH_MULT_POS);
        break;
    case USB_EP_TYPE_BULK:
    case USB_EP_TYPE_INT:
        break;
    default:
        AW_ERRF(("IMX6UL USBD: error EP type.\n"));
        return;
    }
    if (zlt) {
        tmp |= __DQH_ZLT_MASK;
    }

    p_qh->info1     = USB_CPU_TO_LE32(tmp);
    p_qh->next_dtd  = USB_CPU_TO_LE32(1u);
    p_qh->info2     = 0;

    return;
}

/** ��ʼ���˵�*/
static void __ep_init ( struct awbl_imx6ul_usbd  *p_imx_usbd,
                        uint8_t                 ep_addr,
                        uint8_t                 ep_type)
{
    __IMX6UL_USBD_INFO_DECL(p_info, &p_imx_usbd->usbd);
    __IMX6UL_USBD_REG_DECL(p_reg, p_info);
    uint32_t ep_ctrl = 0;

    ep_ctrl = __IMX_REG32_RD(&p_reg->endptctrl[ep_addr & 0x0F]);

    if (ep_addr & USB_DIR_IN) {
        if (ep_addr & 0x0F) {
            ep_ctrl |= __EPCTRL_TXR;
        }
        /* ʹ�ܷ��ʹ���*/
        ep_ctrl |= __EPCTRL_TXE;
        ep_ctrl |= ((uint32_t)(ep_type) << __EPCTRL_TXT_POS);
    } else {
        if (ep_addr & 0x0F) {
            /* ���Ƕ˵�0*/
            ep_ctrl |= __EPCTRL_RXR;
        }
        /* ʹ�ܽ��մ���*/
        ep_ctrl |= __EPCTRL_RXE;
        ep_ctrl |= ((uint32_t)(ep_type) << __EPCTRL_RXT_POS);
    }

    __IMX_REG32_WR(ep_ctrl, &p_reg->endptctrl[ep_addr & 0x0F]);
}

/** ˢ�¶˵�FIFO*/
aw_local void __ep_fifo_flush ( struct awbl_imx6ul_usbd  *p_imx_usbd,
                                uint8_t                 ep_addr)
{
    __IMX6UL_USBD_INFO_DECL(p_info, &p_imx_usbd->usbd);
    __IMX6UL_USBD_REG_DECL(p_reg, p_info);
    uint32_t    bits;

    /* �жϷ���*/
    if (ep_addr & USB_DIR_IN) {
        bits = 1 << (16 + (ep_addr & 0x0F));
    } else {
        bits = 1 << (ep_addr & 0x0F);
    }

    do {
        __IMX_REG32_WR(bits, &p_reg->endptflush);

        /* �ȴ�ˢ�����*/
        while (__IMX_REG32_RD(&p_reg->endptflush)) {
            ;
        }

    /* ����Ƿ���Ҫ��ˢ��һ��*/
    } while (__IMX_REG32_RD(&p_reg->endptstatus) & bits);
}

/** imx6ul USB�豸Ӳ��ʹ�ܶ˵�*/
aw_local void __hw_ep_enable (  struct awbl_imx6ul_usbd  *p_imx_usbd,
                                uint8_t                 ep_addr)
{
    __IMX6UL_USBD_INFO_DECL(p_info, &p_imx_usbd->usbd);
    __IMX6UL_USBD_REG_DECL(p_reg, p_info);

    uint32_t ep_ctrl;

    ep_ctrl = __IMX_REG32_RD(&p_reg->endptctrl[ep_addr & 0x0F]);
    if (ep_addr & USB_DIR_IN) {
        ep_ctrl |= __EPCTRL_TXE;
    } else {
        ep_ctrl |= __EPCTRL_RXE;
    }
    __IMX_REG32_WR(ep_ctrl, &p_reg->endptctrl[ep_addr & 0x0F]);
}

/** imx6ul USB�豸Ӳ�����ܶ˵�*/
aw_local void __hw_ep_disable ( struct awbl_imx6ul_usbd  *p_imx_usbd,
                                uint8_t                 ep_addr)
{
    __IMX6UL_USBD_INFO_DECL(p_info, &p_imx_usbd->usbd);
    __IMX6UL_USBD_REG_DECL(p_reg, p_info);

    uint32_t ep_ctrl;

    ep_ctrl = __IMX_REG32_RD(&p_reg->endptctrl[ep_addr & 0x0F]);
    if (ep_addr & USB_DIR_IN) {
        ep_ctrl &= ~__EPCTRL_TXE;
    } else {
        ep_ctrl &= ~__EPCTRL_RXE;
    }
    __IMX_REG32_WR(ep_ctrl, &p_reg->endptctrl[ep_addr & 0x0F]);
}

/** ɨ��˵㴫��������*/
aw_local void __ep_td_scan (struct awbl_imx6ul_usbd      *p_imx_usbd,
                            struct awbl_imx6ul_usbd_ep   *p_imx_ep)
{

    struct awbl_imx6ul_usbd_td *p_td,
                               *p_prev_td = NULL,
                               *p_td_tmp;
    uint32_t                    token,
                                temp;
    uint16_t                    remain_len;
    int                         status = AW_OK;

    /* ������������������*/
    aw_list_for_each_entry_safe(p_td,
                                p_td_tmp,
                                &p_imx_ep->td_list,
                                struct awbl_imx6ul_usbd_td,
                                node) {
        if (p_prev_td) {
            if (p_prev_td->p_trans != p_td->p_trans) {
                /* ��������һ��������������������ɻص�*/
                p_prev_td->p_trans->status = status;
                if (p_prev_td->p_trans->complete) {
                    p_prev_td->p_trans->complete(
                            p_prev_td->p_trans->p_arg);
                }

                if (status != AW_OK) {
                    /* ��������ֹͣ������*/
                    temp = USB_CPU_TO_LE32(p_imx_ep->p_qh->info2);
                    temp &= ~(  __DTD_STA_HALTED_MASK |
                                __DTD_STA_DATA_BUFF_ERR_MASK |
                                __DTD_STA_TRANSACTION_ERR_MASK);
                    p_imx_ep->p_qh->info2 =
                            USB_CPU_TO_LE32(temp);
                    status = AW_OK;
                }
            }

            /* �ͷŴ���������*/
            __td_free(p_imx_usbd, p_prev_td);
            p_prev_td = NULL;
        }

        /* ��鴫���Ƿ����*/
        if ((p_imx_ep->state != __EP_ST_ENABLE) ||
            (p_td->p_trans->status == -AW_ECANCELED)) {
            status = -AW_ECANCELED;
        }

        /* ��鴫����������״̬*/
        if (status == AW_OK) {
            token       = USB_CPU_TO_LE32(p_td->token);
            remain_len  = __DQH_TOTAL_BYTES(token);

            if (token & __DTD_STA_HALTED_MASK) {
                /* ֹͣ*/
                status = -AW_EPIPE;
            } else if (token & __DTD_STA_DATA_BUFF_ERR_MASK) {
                /* �������*/
                status = -AW_EPROTO;
            } else if (token & __DTD_STA_TRANSACTION_ERR_MASK) {
                /* �������*/
                status = -AW_EILSEQ;
            } else if (token & __DTD_STA_ACTIVE_MASK){
                /* �������ڽ����У�������һ������������*/
                continue;
            } else if (remain_len) {
                /* ����һЩ���ݻ�û�д���*/
                if (p_td->p_trans->flag & USBD_SHORT_NOT_OK) {
                    status = -AW_EPROTO;
                } else {
                    status = AW_OK;
                }
            } else {
                /* �������*/
                status = AW_OK;
            }

            if (status == AW_OK) {
                p_td->p_trans->act_len += (p_td->len - remain_len);
            }
        }

        /* ������ȡ����˵㱻���ܣ���Ҫ������*/
        if (p_td->node.prev != &p_imx_ep->td_list) {
            p_prev_td = aw_list_entry ( p_td->node.prev,
                                        struct awbl_imx6ul_usbd_td,
                                        node);
            p_prev_td->next = p_td->next;
        }

        /* ���������Ƴ�����������*/
        aw_list_del(&p_td->node);
        p_prev_td = p_td;
    }

    if (p_prev_td) {
        /* ���һ������������*/
        p_prev_td->p_trans->status = status;
        if (p_prev_td->p_trans->complete) {
            p_prev_td->p_trans->complete(
                    p_prev_td->p_trans->p_arg);
        }
        __td_free(p_imx_usbd, p_prev_td);
        p_prev_td = NULL;
    }
}

/** ����imx6ul USB�豸������*/
aw_local aw_err_t __imx6ul_usbd_run (struct usbd *p_usbd)
{
    struct awbl_imx6ul_usbd_info *p_info = NULL;
    if((p_usbd == NULL) || (p_usbd->usr_priv == NULL)){
        return -AW_EINVAL;
    }
    p_info = (struct awbl_imx6ul_usbd_info *)AWBL_DEVINFO_GET(p_usbd->usr_priv);
    if(p_info == NULL){
        return -AW_EINVAL;
    }
    __IMX6UL_USBD_REG_DECL(p_reg, p_info);
    uint32_t temp;

    if (p_usbd == NULL) {
        return -AW_EINVAL;
    }

    /* ʹ���жϼĴ���*/
    temp = __USBINTR_UE | __USBINTR_UEE
        | __USBINTR_PCE | __USBINTR_URE
        | __USBINTR_SLE | __USBINTR_SEE;

    __IMX_REG32_WR(temp, &p_reg->usbintr);
    __IMX_REG32_WR(temp, &p_reg->usbsts);

    /* ���ÿ�����Ϊ�豸ģʽ */
    temp = __IMX_REG32_RD(&p_reg->usbmode);
    temp |= __USBMODE_CTRL_MODE_DEVICE;
    __IMX_REG32_WR(temp, &p_reg->usbmode);

    /* ���ÿ���������*/
    temp = __IMX_REG32_RD(&p_reg->usbcmd);
    temp |= __USBCMD_RUN_STOP;
    __IMX_REG32_WR(temp, &p_reg->usbcmd);

    return AW_OK;
}

/** ֹͣimx6ul USB�豸������*/
aw_local aw_err_t __imx6ul_usbd_stop (struct usbd *p_usbd)
{
    struct awbl_imx6ul_usbd_info *p_info = NULL;
    if((p_usbd == NULL) || (p_usbd->usr_priv == NULL)){
        return -AW_EINVAL;
    }
    p_info = (struct awbl_imx6ul_usbd_info *)AWBL_DEVINFO_GET(p_usbd->usr_priv);
    if(p_info == NULL){
        return -AW_EINVAL;
    }
    __IMX6UL_USBD_REG_DECL(p_reg, p_info);
    uint32_t temp;

    if (p_usbd == NULL) {
        return -AW_EINVAL;
    }

    /* �ر��ж�*/
    __IMX_REG32_WR(0, &p_reg->usbintr);

    /* ���ÿ�����ֹͣ*/
    temp = __IMX_REG32_RD(&p_reg->usbcmd);
    temp &= ~__USBCMD_RUN_STOP;
    __IMX_REG32_WR(temp, &p_reg->usbcmd);

    return AW_OK;
}

/** imx6ul USB�豸����*/
aw_local aw_err_t __imx6ul_usbd_pullup ( struct usbd    *p_usbd,
                                         aw_bool_t       is_set)
{
    struct awbl_imx6ul_usbd_info *p_info = NULL;
    p_info = (struct awbl_imx6ul_usbd_info *)AWBL_DEVINFO_GET(p_usbd->usr_priv);
    if(p_info == NULL){
        return -AW_EINVAL;
    }
    __IMX6UL_USBD_REG_DECL(p_reg, p_info);

    if (is_set == AW_TRUE) {
        __IMX_REG32_WR((__IMX_REG32_RD(&p_reg->usbcmd) | __USBCMD_RUN_STOP),
                &p_reg->usbcmd);
    } else {
        __IMX_REG32_WR((__IMX_REG32_RD(&p_reg->usbcmd) & ~__USBCMD_RUN_STOP),
                &p_reg->usbcmd);
    }

    return 0;
}

/** imx6ul USB�豸ʹ�ܶ˵�*/
aw_local aw_err_t __imx6ul_usbd_ep_enable ( struct usbd    *p_usbd,
                                            struct usbd_ep *p_ep)
{
    struct awbl_imx6ul_usbd *p_imx_usbd =
            (struct awbl_imx6ul_usbd *)AW_CONTAINER_OF(p_usbd->usr_priv,
                                        struct awbl_usbd, awdev);
    __IMX6UL_USBD_EP_DECL(p_imx_ep, p_ep);
    uint16_t    mkt = 0;
    uint8_t     mult = 0, type;
    aw_bool_t      zlt;

    /* �Ϸ��Լ��*/
    if ((p_usbd == NULL) || (p_ep == NULL) || (p_imx_usbd == NULL)) {
        return -AW_EINVAL;
    }

    mkt  = p_ep->cur_mps;
    type = p_ep->cur_type;
    zlt  = AW_TRUE;

    switch (type) {
    case USB_EP_TYPE_CTRL:
    case USB_EP_TYPE_BULK:
    case USB_EP_TYPE_INT:
        mult = 0;
        break;
    case USB_EP_TYPE_ISO:
        /* ����ߴ����ʱ������Ҫ��������*/
        mult = (uint8_t)(1 + ((mkt >> 11) & 0x03));
        mkt = mkt & 0x7ff;  /* bit 0~10 */
        /* ���3������*/
        if (mult > 3) {
            return -AW_EINVAL;
        }
        break;
    default:
        return -AW_EINVAL;
    }
    aw_spinlock_isr_take(&p_imx_usbd->lock);
    /* ��ʼ������ͷ*/
    __qh_init(  p_imx_usbd,
                p_ep->ep_addr,
                type,
                mkt,
                zlt,
                mult);
    /* ��ʼ���˵�*/
    __ep_init(  p_imx_usbd,
                p_ep->ep_addr,
                type);

    p_imx_ep->state = __EP_ST_ENABLE;

    /* ˢ��FIFO*/
    __ep_fifo_flush(p_imx_usbd,
                    p_ep->ep_addr);
    /* Ӳ��ʹ��*/
    __hw_ep_enable( p_imx_usbd,
                    p_ep->ep_addr);

    aw_spinlock_isr_give(&p_imx_usbd->lock);

    return AW_OK;
}

/** imx6ul USB�豸����*/
aw_err_t __imx6ul_usbd_ep_disable ( struct usbd    *p_usbd,
                                    struct usbd_ep *p_ep)
{
    struct awbl_imx6ul_usbd *p_imx_usbd =
            (struct awbl_imx6ul_usbd *)AW_CONTAINER_OF(p_usbd->usr_priv,
                                        struct awbl_usbd, awdev);
    __IMX6UL_USBD_EP_DECL(p_imx_ep, p_ep);

    if ((p_usbd == NULL) || (p_ep == NULL)) {
        return -AW_EINVAL;
    }

    __hw_ep_disable(p_imx_usbd,
                    p_ep->ep_addr);

    /* ˢ��FIFO*/
    __ep_fifo_flush(p_imx_usbd, p_ep->ep_addr);

    aw_spinlock_isr_take(&p_imx_usbd->lock);
    p_imx_ep->state = __EP_ST_DISABLE;
    /* ɨ��˵㴫��������*/
    __ep_td_scan (p_imx_usbd, p_imx_ep);
    aw_spinlock_isr_give(&p_imx_usbd->lock);


    return AW_OK;
}

/** imx6ul USB�豸�˵㸴λ*/
aw_err_t __imx6ul_usbd_ep_reset (struct usbd    *p_usbd,
                                 struct usbd_ep *p_ep)
{
    struct awbl_imx6ul_usbd *p_imx_usbd =
            (struct awbl_imx6ul_usbd *)AW_CONTAINER_OF(p_usbd->usr_priv,
                                        struct awbl_usbd, awdev);
    __IMX6UL_USBD_EP_DECL(p_imx_ep, p_ep);
    uint8_t state;

    aw_spinlock_isr_take(&p_imx_usbd->lock);

    /* ˢ��FIFO*/
    __ep_fifo_flush(p_imx_usbd, p_ep->ep_addr);
    state           = p_imx_ep->state;
    p_imx_ep->state = __EP_ST_RESET;
    /* ɨ��˵㴫��������*/
    __ep_td_scan (p_imx_usbd, p_imx_ep);
    p_imx_ep->state = state;
    aw_spinlock_isr_give(&p_imx_usbd->lock);

    return AW_OK;
}

/** imx6ul USB�豸���õ�ַ*/
aw_local aw_err_t __imx6ul_usbd_set_address (struct usbd    *p_usbd,
                                             uint8_t         addr)
{
    struct awbl_imx6ul_usbd_info *p_info = NULL;
    p_info = (struct awbl_imx6ul_usbd_info *)AWBL_DEVINFO_GET(p_usbd->usr_priv);
    if(p_info == NULL){
        return -AW_EINVAL;
    }
    __IMX6UL_USBD_REG_DECL(p_reg, p_info);

    if (p_usbd == NULL) {
        return -AW_EINVAL;
    }

    /* �����µĵ�ַ�����ؼĴ��������һ���˵�0�����Ӧ�����µ�ַ*/
    __IMX_REG32_WR((addr << __DEVICE_ADDRESS_ADR_POS) |
                    __DEVICE_ADDRESS_ADRA_MASK,
                    &p_reg->deviceaddr);

    return AW_OK;
}

/** imx6ul USB�豸��ȡ�˵�״̬*/
aw_local aw_err_t __imx6ul_usbd_ep_status ( struct usbd    *p_usbd,
                                            uint8_t         ep_addr,
                                            aw_bool_t       is_set,
                                            uint16_t       *status)
{
    struct awbl_imx6ul_usbd *p_imx_usbd =
            (struct awbl_imx6ul_usbd *)AW_CONTAINER_OF(p_usbd->usr_priv,
                                        struct awbl_usbd, awdev);
    struct awbl_imx6ul_usbd_info *p_info = NULL;
    p_info = (struct awbl_imx6ul_usbd_info *)AWBL_DEVINFO_GET(p_usbd->usr_priv);
    if(p_info == NULL){
        return -AW_EINVAL;
    }
    __IMX6UL_USBD_REG_DECL(p_reg, p_info);

    struct awbl_imx6ul_usbd_ep *p_imx_ep;
    uint32_t            epctrl;

    if ((p_usbd == NULL) ||
        (status == NULL) ||
        ((ep_addr & 0x0F) >= (p_imx_usbd->neps >> 1))) {
        return -AW_EINVAL;
    }
    /* ��ȡ�˵�*/
    p_imx_ep = __get_ep (p_imx_usbd, ep_addr);
    if (p_imx_ep == NULL || !p_imx_ep->ep.used) {
        return -AW_ENODEV;
    }

    epctrl = __IMX_REG32_RD(&p_reg->endptctrl[ep_addr & 0x0F]);
    if (ep_addr & USB_DIR_IN) {
        *status = (epctrl & __EPCTRL_TXS) ? 1 : 0;
    } else {
        *status = (epctrl & __EPCTRL_RXS) ? 1 : 0;
    }

    return AW_OK;
}

/** imx6ul USB�豸��ȡ�˵�����*/
aw_local aw_err_t __imx6ul_usbd_ep_feature (struct usbd     *p_usbd,
                                            uint8_t          ep_addr,
                                            aw_bool_t        is_set)
{
    struct awbl_imx6ul_usbd *p_imx_usbd =
            (struct awbl_imx6ul_usbd *)AW_CONTAINER_OF(p_usbd->usr_priv,
                                        struct awbl_usbd, awdev);
    struct awbl_imx6ul_usbd_info *p_info = NULL;
    p_info = (struct awbl_imx6ul_usbd_info *)AWBL_DEVINFO_GET(p_usbd->usr_priv);
    if(p_info == NULL){
        return -AW_EINVAL;
    }
    __IMX6UL_USBD_REG_DECL(p_reg, p_info);
    struct awbl_imx6ul_usbd_ep   *p_imx_ep;
    uint32_t                    ep_ctrl = 0;
    uint8_t                     dir, ep_num;

    if ((p_usbd == NULL) ||
        ((ep_addr & 0x0F) >= (p_imx_usbd->neps >> 1))) {
        return -AW_EINVAL;
    }

    p_imx_ep = __get_ep (p_imx_usbd, ep_addr);
    if (p_imx_ep == NULL || !p_imx_ep->ep.used) {
        return -AW_ENODEV;
    }

    if (p_imx_ep->ep.cur_type == USB_EP_TYPE_ISO) {
        return -AW_EOPNOTSUPP;
    }

    dir    = p_imx_ep->ep.ep_addr & 0x80;
    ep_num = p_imx_ep->ep.ep_addr & 0x0f;

    /* ������κδ��������ڶ����ֹͣ�˵��ʧ��*/
    if ((is_set == AW_TRUE) &&
        (dir == USB_DIR_IN) &&
        !aw_list_empty(&p_imx_ep->td_list)) {
        return -AW_EAGAIN;
    }

    aw_spinlock_isr_take(&p_imx_usbd->lock);

    ep_ctrl = __IMX_REG32_RD(&p_reg->endptctrl[ep_num]);

    if (is_set == AW_TRUE) {
        /* ����ֹͣλ*/
        if (dir == USB_DIR_IN) {
            ep_ctrl |= __EPCTRL_TXS;
        } else {
            ep_ctrl |= __EPCTRL_RXS;
        }
    } else {
        /* ���ֹͣλ�͸�λ���ݷ�ת*/
        if (dir == USB_DIR_IN) {
            ep_ctrl &= ~__EPCTRL_TXS;
            ep_ctrl |= __EPCTRL_TXR;
        } else {
            ep_ctrl &= ~__EPCTRL_RXS;
            ep_ctrl |= __EPCTRL_RXR;
        }
    }
    __IMX_REG32_WR(ep_ctrl, &p_reg->endptctrl[ep_num]);

    aw_spinlock_isr_give(&p_imx_usbd->lock);

    return AW_OK;
}


/** ��ʼ������������*/
aw_local size_t __init_td ( struct awbl_imx6ul_usbd_td   *p_td,
                            void                        *p_buf,
                            size_t                      len,
                            uint16_t                    maxpacket)
{

    uint32_t    temp, count;
    uint8_t     i;

    temp = (uint32_t)p_buf;
    /* ��ʼ����һ������ҳָ��*/
    p_td->buf[0] = USB_CPU_TO_LE32(temp);

    /* 4K����*/
    count = 0x1000 - (temp & 0x0fff);

    if (len <= count) {
        count = len;
    } else {
        /* ��һ��4K����ĵ�ַ*/
        temp +=  0x1000;
        temp &= ~0x0fff;

        /* ��ʼ����һ������ҳָ��*/
        for (i = 1; (count < len) && (i < 5); i++) {

            p_td->buf[i] = USB_CPU_TO_LE32(temp);

            temp += 0x1000;

            if ((count + 0x1000) < len) {
                count += 0x1000;
            } else {
                count = len;
            }
        }

        if (count != len) {
            count -= (count % maxpacket);
        }
    }

    p_td->next    = USB_CPU_TO_LE32(1u);
    p_td->p_trans = NULL;
    /* ��䴫���С�����ü���λ*/
    temp = ((count << __DQH_TOTAL_BYTES_POS) | __DTD_STA_ACTIVE_MASK);
    p_td->token = USB_CPU_TO_LE32(temp);

    p_td->addr_dma  = (void *)p_td;
    p_td->len       = count;

    AW_INIT_LIST_HEAD(&p_td->node);

    return count;
}

/** ���촫������������*/
aw_local aw_err_t __make_td_list (  struct awbl_imx6ul_usbd  *p_imx_usbd,
                                    struct usbd_trans        *p_trans,
                                    struct aw_list_head      *td_list)
{

    struct awbl_imx6ul_usbd_td   *p_td, *p_last_td = NULL;
    size_t                      temp, len;
    uint8_t                     *ptr;
    uint16_t                    mpk;

    len     = p_trans->len;
    ptr     = p_trans->p_buf;
    mpk = p_trans->p_hw->cur_mps;

    do {
        /* ���䴫��������*/
        p_td = __td_alloc(p_imx_usbd);
        if (p_td == NULL) {
            goto _faield;
        }

        if (p_last_td) {
            /* ������һ������������*/
            p_last_td->next = USB_CPU_TO_LE32((uint32_t)p_td->addr_dma);
        }

        /* ��ʼ������������*/
        temp = __init_td (  p_td,
                            ptr,
                            len,
                            mpk);

        len                 -=  temp;
        ptr                 +=  temp;
        p_last_td           =   p_td;
        p_td->p_trans    =   p_trans;

        aw_list_add_tail(&p_td->node, td_list);
    } while (len);

    /* ���һ������������ʹ���ж� */
    p_td->token = USB_CPU_TO_LE32(p_td->token) | __DTD_IOC_MASK;

    return AW_OK;

_faield:
    aw_list_for_each_entry_safe(p_td, p_last_td,
            td_list, struct awbl_imx6ul_usbd_td, node)  {
        aw_list_del(&p_td->node);
        __td_free(p_imx_usbd, p_td);
    }

    return -AW_ENOMEM;
}

/** imx6ul USB�豸��������*/
aw_local aw_err_t __imx6ul_transfer_req (struct usbd         *p_usbd,
                                         struct usbd_trans   *p_trans)
{
    struct awbl_imx6ul_usbd *p_imx_usbd =
            (struct awbl_imx6ul_usbd *)AW_CONTAINER_OF(p_usbd->usr_priv,
                                        struct awbl_usbd, awdev);
    struct awbl_imx6ul_usbd_info *p_info = NULL;
    if((p_usbd == NULL) || (p_usbd->usr_priv == NULL)){
        return -AW_EINVAL;
    }
    p_info = (struct awbl_imx6ul_usbd_info *)AWBL_DEVINFO_GET(p_usbd->usr_priv);
    if(p_info == NULL){
        return -AW_EINVAL;
    }
    __IMX6UL_USBD_REG_DECL(p_reg, p_info);
    __IMX6UL_USBD_EP_DECL(p_imx_ep, p_trans->p_hw);
    struct aw_list_head         td_list;
    struct awbl_imx6ul_usbd_td *p_td;
    aw_err_t                    err;
    uint32_t                    temp;
    uint32_t                    bitmask;
    uint8_t                     ep_num;

    if (p_imx_ep->state != __EP_ST_ENABLE) {
        return -AW_ESHUTDOWN;
    }

    ep_num = p_imx_ep->ep.ep_addr & 0x0f;

    AW_INIT_LIST_HEAD(&td_list);

    /* Ϊ���δ��乹��һ����������������*/
    err = __make_td_list(p_imx_usbd, p_trans, &td_list);
    if (err != AW_OK) {
        return err;
    }

    bitmask = (p_imx_ep->ep.ep_addr & USB_DIR_IN)
            ? (1 << (ep_num + 16))
            : (1 << (ep_num));

    /* ��ȡ�µĴ�������������ĵ�һ������������*/
    p_td = aw_list_entry(td_list.next,
                         struct awbl_imx6ul_usbd_td,
                         node);

    aw_spinlock_isr_take(&p_imx_usbd->lock);

    /* ��鴫�������������Ƿ�Ϊ��*/
    if (!(aw_list_empty(&p_imx_ep->td_list))) {
        struct awbl_imx6ul_usbd_td   *p_tail_td;
        uint32_t                    ep_status;

        /* ��ȡ�˵����������������һ������������*/
        p_tail_td = aw_list_entry(  p_imx_ep->td_list.prev,
                                    struct awbl_imx6ul_usbd_td,
                                    node);

        /* ƴ��������������������*/
        aw_list_splice_tail(&td_list, &p_imx_ep->td_list);

        /* ����������������������*/
        p_tail_td->next = USB_CPU_TO_LE32(
                (uint32_t)p_td->addr_dma & __DTD_ADDR_MASK);

        /* ��ȡ�˵�Primeλ�����˵��Ƿ���һЩ�ɵĴ����������ڴ���*/
        if (__IMX_REG32_RD(&p_reg->endpointprime) & bitmask) {
            aw_spinlock_isr_give(&p_imx_usbd->lock);
            return AW_OK;
        }

        do {
            /* ����ATDTWλ�����߿��������µĴ���������*/
            temp = __IMX_REG32_RD(&p_reg->usbcmd);
            __IMX_REG32_WR(temp | __USBCMD_ATDTW, &p_reg->usbcmd);

            /* ��ȡ��ǰ��״̬λ*/
            ep_status = __IMX_REG32_RD(&p_reg->endptstatus) & bitmask;

        } while (!(__IMX_REG32_RD(&p_reg->usbcmd) & __USBCMD_ATDTW));

        /* ��� ATDTW λ*/
        temp = __IMX_REG32_RD(&p_reg->usbcmd);
        __IMX_REG32_WR(temp & ~__USBCMD_ATDTW, &p_reg->usbcmd);

        /* ����˵㻹�ڴ��䣬���˵�״̬*/
        if (ep_status) {
            aw_spinlock_isr_give(&p_imx_usbd->lock);
            return AW_OK;
        }
    }

    /* �˵��Ѿ�ֹͣ����*/

    /* ƴ��������������������*/
    aw_list_splice_tail(&td_list, &p_imx_ep->td_list);

    /* Write dQH next pointer and terminate bit to 0 */
    temp = (uint32_t)p_td->addr_dma & __DTD_ADDR_MASK;
    p_imx_ep->p_qh->next_dtd = USB_CPU_TO_LE32(temp);

    /* �������λ��ֹͣλ*/
    temp = ~(__DQH_STA_ACTIVE_MASK | __DQH_STA_HALT_MASK);
    p_imx_ep->p_qh->info2 &= USB_CPU_TO_LE32(temp);

    /* ENDPTPRIME�Ĵ�����Ӧ�Ķ˵�λд1��ʼ����*/
    __IMX_REG32_WR(bitmask, &p_reg->endpointprime);

    aw_spinlock_isr_give(&p_imx_usbd->lock);

    return AW_OK;
}

/** imx10xx USB�豸����ȡ��*/
aw_local aw_err_t __imx6ul_transfer_cancel (
        struct usbd         *p_usbd,
        struct usbd_trans   *p_trans)
{
    struct awbl_imx6ul_usbd *p_imx_usbd =
            (struct awbl_imx6ul_usbd *)AW_CONTAINER_OF(p_usbd->usr_priv,
                                        struct awbl_usbd, awdev);
    __IMX6UL_USBD_EP_DECL(p_imx_ep, p_trans->p_hw);

    struct awbl_imx6ul_usbd_td   *p_td;
    aw_err_t                    err;


    if ((p_imx_usbd == NULL) ||
        (p_trans == NULL)) {
        return -AW_EINVAL;
    }

    aw_spinlock_isr_take(&p_imx_usbd->lock);

    /* �������֮ǰ��ֹͣ�˵�*/
    __hw_ep_disable(p_imx_usbd,
                    p_imx_ep->ep.ep_addr);


    aw_list_for_each_entry( p_td,
                            &p_imx_ep->td_list,
                            struct awbl_imx6ul_usbd_td, node) {
        if (p_td->p_trans == p_trans) {
            break;
        }
    }

    if (p_td->p_trans != p_trans) {
        err = -AW_EINVAL;
        goto _exit;
    }

    __ep_td_scan (p_imx_usbd, p_imx_ep);

    err = AW_OK;

_exit:

    /* ʹ�ܶ˵�*/
    __hw_ep_enable( p_imx_usbd,
                    p_imx_ep->ep.ep_addr);

    aw_spinlock_isr_give(&p_imx_usbd->lock);
    return err;
}

/** ��ʼ��Ӳ��*/
aw_local void __handware_init (struct awbl_imx6ul_usbd *p_imx_usbd)
{
    __IMX6UL_USBD_INFO_DECL(p_info, &p_imx_usbd->usbd);
    __IMX6UL_USBD_REG_DECL(p_reg, p_info);
    uint32_t temp;

    /* ֹͣ���Ҹ�λ������*/
    temp = __IMX_REG32_RD(&p_reg->usbcmd);
    temp &= ~__USBCMD_RUN_STOP;
    __IMX_REG32_WR(temp, &p_reg->usbcmd);

    temp = __IMX_REG32_RD(&p_reg->usbcmd);
    temp |= __USBCMD_CTRL_RESET;
    __IMX_REG32_WR(temp, &p_reg->usbcmd);

    /* �ȴ���λ���*/
    while (__IMX_REG32_RD(&p_reg->usbcmd) & __USBCMD_CTRL_RESET);

    /* ���ÿ�����Ϊ�豸ģʽ*/
    temp = __IMX_REG32_RD(&p_reg->usbmode);
    temp |= __USBMODE_CTRL_MODE_DEVICE;
    /* �ر�Setup��*/
    temp |= __USBMODE_SETUP_LOCK_OFF;
    __IMX_REG32_WR(temp, &p_reg->usbmode);

    /* �ر������ж�*/
    __IMX_REG32_WR(0, &p_reg->usbintr);

    /* ���USB״̬*/
    __IMX_REG32_WR(0, &p_reg->usbsts);
    /* ���ö˵������ַ*/
    temp = (uint32_t)p_imx_usbd->qhs;
    temp &= __EP_LIST_ADDRESS_MASK;
    __IMX_REG32_WR(temp, &p_reg->endpointlistaddr);


}

/** USB �豸�жϷ�����*/
aw_local void __imx6ul_usbd_isr(void *p_arg)
{
    struct awbl_imx6ul_usbd *p_imx_usbd = (struct awbl_imx6ul_usbd *)p_arg;
    __IMX6UL_USBD_INFO_DECL(p_info, &p_imx_usbd->usbd);
    /* �ͷ��ź���*/
    AW_SEMB_GIVE(p_imx_usbd->usbd_sem_sync);

    /* �ر��ж�*/
    aw_int_disable(p_info->inum);
}

/** ��ʼ��imx6ul PHY*/
aw_local void __imx6ul_phy_init (struct awbl_imx6ul_usbd *p_imx_usbd)
{
    __IMX6UL_USBD_INFO_DECL(p_info, &p_imx_usbd->usbd);
    uint32_t temp, timeout = 10000;

    /* ��λPHY */
    __IMX_REG32_WR(__IMX6UL_USBPHY_CTRL_SFTRST,
                    p_info->phy_regbase + __IMX6UL_USBPHY_CTRL_SET);
    do {

        temp = __IMX_REG32_RD(p_info->phy_regbase + __IMX6UL_USBPHY_CTRL);
    } while (!(temp & __IMX6UL_USBPHY_CTRL_CLKGATE) && (timeout--));

    /* ʹ��PHYʱ��*/
    __IMX_REG32_WR( __IMX6UL_USBPHY_CTRL_SFTRST |
                    __IMX6UL_USBPHY_CTRL_CLKGATE,
                    p_info->phy_regbase + __IMX6UL_USBPHY_CTRL_CLR);

    /* ����PHY*/
    __IMX_REG32_WR(0, p_info->phy_regbase + __IMX6UL_USBPHY_PWD);

    /* ʹ��ȫ/�����豸֧��*/
    __IMX_REG32_WR( __IMX6UL_USBPHY_CTRL_ENUTMILEVEL2 |
                    __IMX6UL_USBPHY_CTRL_ENUTMILEVEL3,
                    p_info->phy_regbase + __IMX6UL_USBPHY_CTRL_SET);
}

/** ��һ�׶γ�ʼ��*/
aw_local void __imx6ul_usbd_inst_init1 (struct awbl_dev *p_awdev)
{
    return ;
}

/** �ڶ��׶γ�ʼ��*/
aw_local void __imx6ul_usbd_inst_init2 (struct awbl_dev *p_awdev)
{

    __IMX6UL_USBD_DECL(p_imx_usbd, (struct awbl_usbd*)p_awdev);
    __IMX6UL_USBD_INFO_DECL(p_info, (struct awbl_usbd*)p_awdev);
    __IMX6UL_USBD_REG_DECL(p_reg, p_info);

    uint32_t    dccparams;
    uint32_t    i;

    aw_spinlock_isr_init(&p_imx_usbd->lock, 0);
    /* ƽ̨��ʼ��*/
    p_info->pfunc_plfm_init();
    /* ��ʼ��PHY*/
    __imx6ul_phy_init(p_imx_usbd);

    /* ��ȡ�豸���������ܲ����Ĵ���*/
    dccparams = __IMX_REG32_RD(&p_reg->dccparams);
    if (!(dccparams & __DCCPARAMS_DC)) {
        AW_ERRF(("IMX6UL USBD: This SOC doesn't support device role\n"));
        return;
    }
    /* ��ʼ���ڴ��*/
    if (AW_OK != __td_pool_init (p_imx_usbd)) {
        AW_ERRF(("IMX6UL USBD: Not enough memory for TD pool.\n"));
        goto _exit;
    }

    p_imx_usbd->eps = NULL;
    p_imx_usbd->qhs = NULL;

    /* Ϊ���ж˵�����ڴ�*/
    p_imx_usbd->neps    = (dccparams & __DCCPARAMS_DEN_MASK) * 2;
    i                   = p_imx_usbd->neps * sizeof(struct awbl_imx6ul_usbd_ep);
    p_imx_usbd->eps     = aw_mem_alloc(i);
    if (p_imx_usbd->eps == NULL) {
        AW_ERRF(("IMX6UL USBD: Not enough memory for EPs.\n"));
        goto _exit;
    }
    memset(p_imx_usbd->eps , 0, i);


    /* Ϊÿ���˵�������ͷ�ڴ�*/
    i               = p_imx_usbd->neps * sizeof(struct awbl_imx6ul_usbd_qh);
    p_imx_usbd->qhs = aw_cache_dma_align(i, 2048);
    if (p_imx_usbd->qhs == NULL) {
        AW_ERRF(("IMX6UL USBD: Not enough memory for QHs.\n"));
        goto _exit;
    }

    memset(p_imx_usbd->qhs , 0, i);
    /* ��ʼ��������Ӳ��*/
    __handware_init(p_imx_usbd);

    aw_spinlock_isr_take(&p_imx_usbd->lock);

    p_imx_usbd->usbd.p_usbd = aw_mem_alloc(sizeof(struct usbd));
    if(p_imx_usbd->usbd.p_usbd == NULL){
        goto _exit;
    }
    memset(p_imx_usbd->usbd.p_usbd, 0, sizeof(struct usbd));
    /* ����һ��USB�豸������*/
    usbd_create(p_imx_usbd->usbd.p_usbd);
    /* �����û�˽������*/
    usbd_set_priv(p_imx_usbd->usbd.p_usbd, &p_imx_usbd->usbd.awdev);

    /* ��ʼ�����еĶ˵�*/
    for (i=0; i<p_imx_usbd->neps; i++) {
        p_imx_usbd->eps[i].p_usbd   = p_imx_usbd;
        p_imx_usbd->eps[i].p_qh     = &p_imx_usbd->qhs[i];
        AW_INIT_LIST_HEAD(&p_imx_usbd->eps[i].td_list);

        if (AW_OK != usbd_ep_register (p_imx_usbd->usbd.p_usbd,
                                       &p_imx_usbd->eps[i].ep,
                                       ((i & 0x01) << 7) | (i >> 1),
                                       (((i >> 1) == 0) ?
                                       USBD_EP_SUPPORT_CTRL :
                                       USBD_EP_SUPPORT_ALL),
                                       USBD_EP_MPS_NO_LIMT)) {
            AW_ERRF(("IMX6UL USBD: EPs register failed.\n"));
        }
    }
    aw_spinlock_isr_give(&p_imx_usbd->lock);
    /* �����жϷ�����*/
    aw_int_connect( p_info->inum,
                    (aw_pfuncvoid_t)__imx6ul_usbd_isr,
                    (void *)p_imx_usbd);
    /* ʹ���ж�*/
    aw_int_enable(p_info->inum);

    return;

_exit:
    if (p_imx_usbd->eps) {
        aw_mem_free(p_imx_usbd->eps);
    }
    if (p_imx_usbd->qhs) {
        aw_cache_dma_free(p_imx_usbd->qhs);
    }
    if(p_imx_usbd->usbd.p_usbd){
        aw_mem_free(p_imx_usbd->usbd.p_usbd);
    }
    return;
}

/** USB�豸��������*/
aw_local void __usb_device_task_entry (struct awbl_dev *p_awdev)
{
    __IMX6UL_USBD_DECL(p_imx_usbd, (struct awbl_usbd*)p_awdev);
    __IMX6UL_USBD_INFO_DECL(p_info, (struct awbl_usbd*)p_awdev);
    __IMX6UL_USBD_REG_DECL(p_reg, p_info);

    uint32_t    status;
    aw_err_t    err;

    AW_FOREVER  {

        AW_SEMB_TAKE(p_imx_usbd->usbd_sem_sync, AW_SEM_WAIT_FOREVER);
        /* ��ȡ�жϼĴ���*/
        status = __IMX_REG32_RD(&p_reg->usbsts) & __IMX_REG32_RD(&p_reg->usbintr);
        /* ����ж�״̬*/
        __IMX_REG32_WR(status, &p_reg->usbsts);

        /* USB�ж�*/
        if (status & __USBSTS_UI) {

            /* Setup����ֻ֧�ֶ˵�0��Ϊ���ƶ˵�*/
            if (__IMX_REG32_RD(&p_reg->endptsetupstat)
                       & __EP_SETUP_STATUS_EP0) {
                struct usb_ctrlreq            setup;
                struct awbl_imx6ul_usbd_qh   *qh;
                uint32_t                      temp;

                /* ���ENDPTSETUPSTATλ*/
                temp = __IMX_REG32_RD(&p_reg->endptsetupstat);
                __IMX_REG32_WR(temp | (1 << 0), &p_reg->endptsetupstat);

                qh = &p_imx_usbd->qhs[0];
                do {
                    /* ����Setup Tripwire��ȷ������д�뻺��*/
                    temp = __IMX_REG32_RD(&p_reg->usbcmd);
                    __IMX_REG32_WR(temp | __USBCMD_SUTW, &p_reg->usbcmd);

                    /* ����setup�������ػ���*/
                    memcpy(&setup, (uint8_t *)qh->setup_buf, 8);
                } while (!(__IMX_REG32_RD(&p_reg->usbcmd) & __USBCMD_SUTW));

                /* ���Setup Tripwire */
                temp = __IMX_REG32_RD(&p_reg->usbcmd);
                __IMX_REG32_WR(temp & ~__USBCMD_SUTW, &p_reg->usbcmd);
                /* �����¼�*/
                err = usbd_event_handle(p_imx_usbd->usbd.p_usbd,
                                        USBD_EVT_SETUP_PKT,
                                        &setup);
                if (AW_OK != err) {
                    AW_ERRF(("IMX6UL USBD: USBD handle setup packet failed: %d.\n",err));
                }
            }

            /* ����豸����������*/
            if (__IMX_REG32_RD(&p_reg->endptcomplete)) {
                struct awbl_imx6ul_usbd_ep   *p_imx_ep;
                uint8_t                       i, j = 0;
                uint32_t                      bits;

                /* ����Ĵ�������ɱ�־*/
                bits = __IMX_REG32_RD(&p_reg->endptcomplete);
                __IMX_REG32_WR(bits, &p_reg->endptcomplete);

                if (bits) {
                    while (j < 32) {
                        for (i=0; i<(p_imx_usbd->neps>>1); i++) {
                            if ((bits & (1<<(i + j)))) {
                                /* ��ȡ�˵�*/
                                p_imx_ep = __get_ep(p_imx_usbd, i | (j<<3));
                                if (p_imx_ep == NULL) {
                                    AW_ERRF(("IMX6UL USBD: get EP failed.\n"));
                                    continue;
                                }

                                /* ɨ��˵�Ĵ������������� */
                                __ep_td_scan (p_imx_usbd, p_imx_ep);
                            }
                        }
                        j+=16;
                    }
                }
            }
        }

        /* SOF (���ڵ�ʱ����) */
        if (status & __USBSTS_SRI) {

        }

        /* �˿�״̬�����仯*/
        if (status & __USBSTS_PCI) {
            uint8_t speed;
            if (!(__IMX_REG32_RD(&p_reg->portsc1) & __PORTSCX_PR)) {
                /* ��ȡ�ٶ�*/
                switch ((__IMX_REG32_RD(&p_reg->portsc1) & __PORTSCX_PSPD)) {
                case __PORTSCX_PSPD_HIGH:
                    speed = USBD_SPEED_HIGH;
                    break;
                case __PORTSCX_PSPD_FULL:
                    speed = USBD_SPEED_FULL;
                    break;
                case __PORTSCX_PSPD_LOW:
                    speed = USBD_SPEED_LOW;
                    break;
                default:
                    speed = USBD_SPEED_UNKNOWN;
                    break;
                }

                if (AW_OK != usbd_event_handle(p_imx_usbd->usbd.p_usbd,
                                               USBD_EVT_UPDATE_SPEED,
                                               &speed)) {
                    AW_ERRF(("IMX6UL USBD: USBD update speed failed.\n"));
                }
            }
        }

        /* ���յ���λ*/
        if (status & __USBSTS_URI) {
            uint32_t temp;
            /* ����豸��ַ*/
            temp = __IMX_REG32_RD(&p_reg->deviceaddr);
            __IMX_REG32_WR(temp & ~__DEVICE_ADDRESS_ADR_MASK, &p_reg->deviceaddr);

            /* ������е�setup����*/
            temp = __IMX_REG32_RD(&p_reg->endptsetupstat);
            __IMX_REG32_WR(temp, &p_reg->endptsetupstat);

            /* ������еĶ˵����״̬λ*/
            temp = __IMX_REG32_RD(&p_reg->endptcomplete);
            __IMX_REG32_WR(temp, &p_reg->endptcomplete);

            while (__IMX_REG32_RD(&p_reg->endpointprime));

            /* ˢ�¶˵㻺��*/
            __IMX_REG32_WR(0xffffffff, &p_reg->endptflush);

            if (__IMX_REG32_RD(&p_reg->portsc1) & __PORTSCX_PR) {
                if (AW_OK != usbd_event_handle(p_imx_usbd->usbd.p_usbd,
                                               USBD_EVT_BUS_RST,
                                               NULL)) {
                    AW_ERRF(("IMX6UL USBD: USBD bus reset failed.\n"));
                }

            } else {
                /* ��ʼ��USB ����Ӳ���Ĵ���*/
                __handware_init(p_imx_usbd);
                /* ʹ���жϼĴ��������ÿ���������*/
                __imx6ul_usbd_run(p_imx_usbd->usbd.p_usbd);

                /* ���������¼�*/
                if (AW_OK != usbd_event_handle(p_imx_usbd->usbd.p_usbd,
                                               USBD_EVT_CONNECT,
                                               NULL)) {
                    AW_ERRF(("IMX6UL USBD:USBD: connect failed.\n"));
                }
            }

            /*�ָ�*/
            if (!(__IMX_REG32_RD(&p_reg->portsc1) & __PORTSCX_SUSP)
                       && (p_imx_usbd->usbd.p_usbd->state == USBD_ST_SUSPENDED)) {
                if (AW_OK != usbd_event_handle(p_imx_usbd->usbd.p_usbd,
                                               USBD_EVT_RESUME,
                                                NULL)) {
                    AW_ERRF(("IMX6UL USBD: USBD resume failed.\n"));
                }
            }
        }

        /* ����ʹ��*/
        if (status & __USBSTS_SLI) {
            if ((p_imx_usbd->usbd.p_usbd->state != USBD_ST_POWERED) &&
                    (p_imx_usbd->usbd.p_usbd->state != USBD_ST_NOTATTACHED)) {
                if (AW_OK != usbd_event_handle(p_imx_usbd->usbd.p_usbd,
                                               USBD_EVT_SUSPENDED,
                                               NULL)) {
                    AW_ERRF(("IMX6UL USBD: USBD suspend failed.\n"));
                }
            }
        }

        if (status & (__USBSTS_UEI | __USBSTS_SEI)) {
            if (AW_OK != usbd_event_handle(p_imx_usbd->usbd.p_usbd,
                                           USBD_EVT_ERROR,
                                           NULL)) {
                AW_ERRF(("IMX6UL USBD: USBD error handle failed.\n"));
            }
            AW_ERRF(("IMX6UL USBD: IRQ, ERROR.\n"));
        }
        /* ʹ���ж�*/
        aw_int_enable(p_info->inum);
    }
}

/** �����׶γ�ʼ��*/
aw_local void __imx6ul_usbd_inst_connect (struct awbl_dev *p_awdev)
{
    __IMX6UL_USBD_DECL(p_imx_usbd, (struct awbl_usbd*)p_awdev);
    __IMX6UL_USBD_INFO_DECL(p_info, (struct awbl_usbd*)p_awdev);
    __IMX6UL_USBD_REG_DECL(p_reg, p_info);

    /* ��ʼ��һ���������ź���*/
    AW_SEMB_INIT(p_imx_usbd->usbd_sem_sync, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);

    /* ��ʼ��һ��USB�豸��������*/
    AW_TASK_INIT(p_imx_usbd->__usb_device_task,     /* ������*/
                 "usb_device_task",                 /* ��������*/
                  p_info->task_prio,                /* �������ȼ�*/
                 __USB_DEVICE_TASK_STACK_SIZE,      /* ����ջ��С*/
                 __usb_device_task_entry,           /* ���������*/
                 (void *)p_awdev);                  /* �������*/
    AW_TASK_STARTUP(p_imx_usbd->__usb_device_task);

    return ;
}

/* ��ʼ������*/
aw_local aw_const struct awbl_drvfuncs __g_imx6ul_usbd_drvfuncs = {
        __imx6ul_usbd_inst_init1,
        __imx6ul_usbd_inst_init2,
        __imx6ul_usbd_inst_connect
};

/** USB �豸����������������*/
static struct usbd_drv usbd_drv = {
        NULL,
        __imx6ul_usbd_run,
        __imx6ul_usbd_stop,
        __imx6ul_transfer_req,
        __imx6ul_transfer_cancel,
        __imx6ul_usbd_ep_enable,
        __imx6ul_usbd_ep_disable,
        __imx6ul_usbd_ep_reset,
        __imx6ul_usbd_set_address,
        NULL,
        __imx6ul_usbd_ep_status,
        __imx6ul_usbd_ep_feature,
        NULL,
        __imx6ul_usbd_pullup
};

/* USB�豸������Ϣ*/
aw_local aw_const struct awbl_usbd_drv __g_imx6ul_usbd_drv_registration = {
    {
        AWBL_VER_1,                             /* AWBL�汾*/
        AWBL_BUSID_PLB | AWBL_DEVID_DEVICE,     /* ����ID*/
        AWBL_IMX6UL_USBD_DRV_NAME,              /* ��������*/
        &__g_imx6ul_usbd_drvfuncs,              /* ������ʼ��*/
        NULL,                                   /* methods*/
        NULL                                    /* ̽�⺯��*/
    },
    &usbd_drv,
};

/** USB�豸ģʽ����ע��*/
void awbl_imx6ul_usbd_drv_register (void)
{
    aw_err_t err;
    err = awbl_drv_register(
            (struct awbl_drvinfo *)&__g_imx6ul_usbd_drv_registration);
    err = (aw_err_t)err;
    return;
}

