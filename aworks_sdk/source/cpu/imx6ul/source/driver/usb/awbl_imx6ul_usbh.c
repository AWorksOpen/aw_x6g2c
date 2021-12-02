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
/*******************************************************************************
  includes
*******************************************************************************/
#include "apollo.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "driver/usb/awbl_imx6ul_usbh.h"
//#include "awbl_imx6ul_usbh.h"

/*******************************************************************************
  IMX6UL USB register define
*******************************************************************************/
#define __IMX6UL_USB_CAPLENGTH              (0x100)         /* USB ���ܼĴ���(����EHCI�����Ĵ�����ַƫ��)*/

#define __IMX6UL_USB_CMD                    (0x140)         /* USB��������Ĵ���*/
#define __IMX6UL_UCMD_RUN_STOP              (1 << 0)        /* ����������/ֹͣ*/
#define __IMX6UL_UCMD_RESET                 (1 << 1)        /* ��������λ*/
#define __IMX6UL_UCMD_ITC_NO_THRESHOLD      (~(0xff << 16)) /* Interrupt Threshold */

#define __IMX6UL_USB_STS                    (0x144)         /* USB ״̬�Ĵ���*/
#define __IMX6UL_UOG_PORTSC1                (0x184)         /* USB�˿�״̬�Ϳ��ƼĴ���*/
#define __IMX6UL_PORTSC_PTS_MASK            (3 << 30)       /* parallel xcvr select mask */
#define __IMX6UL_PORTSC_PTS_UTMI            (0 << 30)       /* UTMI/UTMI+ */
#define __IMX6UL_PORTSC_PTW                 (1 << 28)       /* UTMI ���*/
#define __USBH_REG_PORTSC1_PSPD_BIT         (26)            /* USB �˿�״̬�Ϳ��ƼĴ����еĶ˿��ٶ�λ*/

#define __IMX6UL_PORT_PTS_MSK               (3<<30)
#define __IMX6UL_PORT_PTS_UTMI              (0<<30)
#define __IMX6UL_PORT_PTS_ULPI              (2<<30)
#define __IMX6UL_PORT_PTS_SERIAL            (3<<30)
#define __IMX6UL_PORT_PTS_PTW               (1<<28)
#define __IMX6UL_PORT_PTS_PSPD              (3<<26)
#define __IMX6UL_PORT_PTS_PSPD_H            (2<<26)

#define __IMX6UL_USBPHY_CTRL                (0x30)          /* USB PHY ���ƼĴ���ƫ��*/
#define __IMX6UL_USBPHY_CTRL_SET            (0x34)          /* USB PHY ���ƼĴ���(����)ƫ��*/
#define __IMX6UL_USBPHY_CTRL_CLR            (0x38)          /* USB PHY ���ƼĴ���(���)ƫ��*/
#define __IMX6UL_USBPHY_CTRL_SFTRST         (1u<<31)        /* USB PHY �����λ*/
#define __IMX6UL_USBPHY_CTRL_CLKGATE        (1u<<30)
#define __IMX6UL_USBPHY_CTRL_FSDLL_RST_EN   (1u<<24)
#define __IMX6UL_USBPHY_CTRL_ENUTMILEVEL2   (1u<<15)        /* USB PHY ʹ��UTMI+ 3��(֧���ڲ�ȫ�ټ������Ե����豸������)*/
#define __IMX6UL_USBPHY_CTRL_ENUTMILEVEL3   (1u<<14)        /* USB PHY ʹ��UTMI+ 2��(֧�ֵ����豸)*/
#define __IMX6UL_USBPHY_CTRL_HOSTDISCONIRQ  (1u<<3)         /* USB PHY ��������ģʽ���豸�Ͽ���־*/
#define __IMX6UL_USBPHY_CTRL_ENHOSTDISCON   (1u<<1)         /* USB PHY ��������ģʽ�¶Ͽ����ʹ��*/

#define __IMX6UL_USBPHY_PWD                 (0x00)          /* USB PHY ����Ĵ���*/
#define __IMX6UL_USBPHY_PWD_SET             (0x04)
#define __IMX6UL_USBPHY_PWD_CLR             (0x08)
#define __IMX6UL_USBPHY_PWD_RXPWDRX         (1<<20)
#define __IMX6UL_USBPHY_PWD_RXPWDDIFF       (1<<19)
#define __IMX6UL_USBPHY_PWD_RXPWD1PT1       (1<<18)
#define __IMX6UL_USBPHY_PWD_RXPWDENV        (1<<17)
#define __IMX6UL_USBPHY_PWD_TXPWDV2I        (1<<12)
#define __IMX6UL_USBPHY_PWD_TXPWDIBIAS      (1<<11)
#define __IMX6UL_USBPHY_PWD_TXPWDFS         (1<<10)

#define __IMX6UL_USB2_MPH_HOST              0
#define __IMX6UL_USB2_DR_HOST               1
#define __IMX6UL_USB2_DR_DEVICE             2
#define __IMX6UL_USB2_DR_OTG                3

#define __IMX6UL_USB2_PHY_ULPI              0
#define __IMX6UL_USB2_PHY_UTMI              1
#define __IMX6UL_USB2_PHY_UTMI_WIDE         2
#define __IMX6UL_USB2_PHY_SERIAL            3

/*******************************************************************************
  macro operate
*******************************************************************************/
#define __AWDEV_TO_IMX(p_awdev) \
        AW_CONTAINER_OF(AW_CONTAINER_OF(AW_CONTAINER_OF(p_awdev, \
                                                        struct awbl_busctlr, \
                                                        super), \
                                        struct awbl_usbh, \
                                        super), \
                        struct awbl_imx6ul_usbh, \
                        aw_usbh)
/* 32λ�Ĵ�����д����*/
#define __IMX_REG32_WR(data, addr)  writel(data, (volatile void __iomem *)(addr))
#define __IMX_REG32_RD(addr)        readl((volatile void __iomem *)(addr))

/* ��ʱ����*/
aw_local aw_inline void __mdelay(uint32_t us)
{
    while (us--) {
        volatile uint32_t i = 26 * 1000;
        while (i--);
    }
}

/**
 * PHY��ʼ��
 * @param p_hc   USB�����������ṹ��
 * @param p_info USB������������Ϣ
 * ˵����
 *  USB PHY(Port Physical Layer)������ײ���ź�ת�����������������ڵ�PHY��PHY�����ֽӿڣ�
 *  һ����UTMI(USB2.0 Transceiver Macrocell Interface)��һ����ULPI(UTMI+Low Pin Interface)��
 *  ǰ��PIN�࣬����PIN�١�һ����˵���оƬ��USB PHY��װ��оƬ�ڣ���������UTMI+�Ľӿڡ�
 *  ����װ��оƬ�ڵ�һ�����ULPI�ӿڣ��������Լ���pin��������
 *  PHY��ʼ������(__phy_init)����imx6ul�У�USB PHYͨ�ÿ��ƼĴ�����������4���Ĵ�����
 *  ռ����16�ֽڣ��ֱ���USB PHYͨ�ÿ��ƼĴ�����USB PHYͨ�ÿ��ƼĴ���(����λ)��
 *  USB PHYͨ�ÿ��ƼĴ���(���λ)��USB PHYͨ�ÿ��ƼĴ���(ȡ��λ)��
 */
aw_local void __imx6ul_phy_init (struct awbl_imx6ul_usbh      *p_hc,
                                 struct awbl_imx6ul_usbh_info *p_info)
{
    uint32_t temp, timeout = 10000;

    /* ֹͣUSB HOST������*/
    temp =  __IMX_REG32_RD(p_info->usb_reg + __IMX6UL_USB_CMD);
    temp &= ~__IMX6UL_UCMD_RUN_STOP;
    /* �ȴ�ֹͣ�������*/
    __IMX_REG32_WR(temp, p_info->usb_reg + __IMX6UL_USB_CMD);
    while (__IMX_REG32_RD(p_info->usb_reg + __IMX6UL_USB_CMD)
                          & __IMX6UL_UCMD_RUN_STOP);
    /* ��λUSB HOST������*/
    temp |= __IMX6UL_UCMD_RESET;
    __IMX_REG32_WR(temp, p_info->usb_reg + __IMX6UL_USB_CMD);
    while (__IMX_REG32_RD(p_info->usb_reg + __IMX6UL_USB_CMD)
                          & __IMX6UL_UCMD_RESET);
    __mdelay(50);

    /* ��λPHY*/
    __IMX_REG32_WR(__IMX6UL_USBPHY_CTRL_SFTRST,
                    p_info->phy_reg + __IMX6UL_USBPHY_CTRL_SET);
    do {
        temp = __IMX_REG32_RD(p_info->phy_reg + __IMX6UL_USBPHY_CTRL);
    } while (!(temp & __IMX6UL_USBPHY_CTRL_CLKGATE) && (timeout--));

    /* ʹ��PHYʱ��*/
    __IMX_REG32_WR( __IMX6UL_USBPHY_CTRL_SFTRST |
                    __IMX6UL_USBPHY_CTRL_CLKGATE,
                    p_info->phy_reg + __IMX6UL_USBPHY_CTRL_CLR);

    __mdelay(50);
    /* ѡ��UTMI�ӿں�����UTMI�����ݿ��Ϊ16λ*/
    temp = __IMX_REG32_RD(p_info->usb_reg + __IMX6UL_UOG_PORTSC1);
    temp &= ~__IMX6UL_PORTSC_PTS_MASK;
    temp |= (__IMX6UL_PORTSC_PTS_UTMI | __IMX6UL_PORTSC_PTW);
    __IMX_REG32_WR(temp, p_info->usb_reg + __IMX6UL_UOG_PORTSC1);

    /* ����PHY*/
    __IMX_REG32_WR(0xffffffff, p_info->phy_reg + __IMX6UL_USBPHY_PWD_CLR);

    /* ʹ��֧��ȫ��/�����豸*/
    __IMX_REG32_WR( __IMX6UL_USBPHY_CTRL_ENUTMILEVEL2 |
                    __IMX6UL_USBPHY_CTRL_ENUTMILEVEL3,
                    p_info->phy_reg + __IMX6UL_USBPHY_CTRL_SET);
}

/** USB�����жϷ�����*/
aw_local void __imx6ul_usbh_isr(void *p_arg)
{
    struct awbl_imx6ul_usbh      *p_this;
    struct awbl_imx6ul_usbh_info *p_info;
    volatile uint32_t             status;

    p_this = __AWDEV_TO_IMX(p_arg);
    p_info = (struct awbl_imx6ul_usbh_info *)AWBL_DEVINFO_GET(p_arg);

    /* ��ȡUSB״̬�Ĵ���*/
    status = __IMX_REG32_RD(p_info->usb_reg + __IMX6UL_USB_STS);
    /* �˿ڷ����仯*/
    if (status & (1<<2)) {
        uint32_t temp;

        /* ��ȡUSB PHY���ƼĴ���*/
        temp = __IMX_REG32_RD(p_info->phy_reg + __IMX6UL_USBPHY_CTRL);
        /* ����ģʽ���������豸�Ͽ�����*/
        if (temp & __IMX6UL_USBPHY_CTRL_HOSTDISCONIRQ) {
            __IMX_REG32_WR((__IMX6UL_USBPHY_CTRL_ENHOSTDISCON |
                            __IMX6UL_USBPHY_CTRL_HOSTDISCONIRQ),
                            p_info->phy_reg + __IMX6UL_USBPHY_CTRL_CLR);

        } else {
            /* ʹ�ܸ���ģʽ�µ��豸�Ͽ����*/
            __IMX_REG32_WR(__IMX6UL_USBPHY_CTRL_ENHOSTDISCON,
                           p_info->phy_reg + __IMX6UL_USBPHY_CTRL_SET);
        }
    }

    /* ����EHCI�жϴ�����*/
    usbh_ehci_irq(&p_this->aw_ehci.p_echi);
}

/** ��һ�׶γ�ʼ��*/
aw_local void __drv_init1 (struct awbl_dev *p_awdev)
{
    return ;
}

/** �ڶ��׶γ�ʼ��*/
aw_local void __drv_init2 (struct awbl_dev *p_awdev)
{
    struct awbl_imx6ul_usbh      *p_this;
    struct awbl_imx6ul_usbh_info *p_info;

    p_this = __AWDEV_TO_IMX(p_awdev);
    p_info = (struct awbl_imx6ul_usbh_info *)AWBL_DEVINFO_GET(p_awdev);

    /* ƽ̨��ʼ��*/
    if(p_info->pfunc_plfm_init) {
        p_info->pfunc_plfm_init();
    }
    /* �ϵ�*/
    if (p_info->pfunc_pwr_ctrl) {
        p_info->pfunc_pwr_ctrl(AW_TRUE);
    }

    __mdelay(50);
    /* ��ʼ��PHY*/
    __imx6ul_phy_init(p_this, p_info);

    return;
}

/** �����׶γ�ʼ��*/
aw_local void __drv_connect (struct awbl_dev *p_awdev)
{
    aw_err_t ret;
    struct awbl_imx6ul_usbh      *p_this;
    struct awbl_imx6ul_usbh_info *p_info;

    p_this = __AWDEV_TO_IMX(p_awdev);
    p_info = (struct awbl_imx6ul_usbh_info *)AWBL_DEVINFO_GET(p_awdev);

    /* ����USB����*/
    ret = awbl_usbh_host_create(&p_this->aw_usbh);
    if(ret != AW_OK){
        return;
    }
    /* ��ʼ��EHCI*/
    ret = awbl_usbh_ehci_init(p_this->aw_usbh.hc,
                              &p_this->aw_ehci,
                              p_info->usb_reg + __IMX6UL_USB_CAPLENGTH,
                              p_info->prio,
                              __USBH_REG_PORTSC1_PSPD_BIT,
                              p_info->nqhs,
                              p_info->nqtds,
                              p_info->nitds,
                              p_info->nsitds);
    if(ret != AW_OK){
        return;
    }
    /* ��ʼ��USB������������*/
    ret = usbh_rh_init(p_this->aw_usbh.hc, p_this->aw_ehci.p_echi.nports, USB_SPEED_HIGH);
    if(ret != USB_OK){
        return;
    }

    /* ����USB����������*/
    ret = usbh_ctrllor_set(p_this->aw_usbh.hc, &p_this->aw_ehci.p_echi);
    if(ret != AW_OK){
        return;
    }

    /* �����жϷ�����*/
    aw_int_connect(p_info->inum,
                   (aw_pfuncvoid_t)__imx6ul_usbh_isr,
                   (void *)p_awdev);
    /* ʹ���ж�*/
    aw_int_enable(p_info->inum);

    return ;
}

/* ���׶γ�ʼ��������*/
aw_local aw_const struct awbl_drvfuncs __g_drvfuncs = {
        __drv_init1,
        __drv_init2,
        __drv_connect
};

/* USB����������Ϣ*/
aw_local aw_const struct awbl_drvinfo __g_drv_registration = {
    AWBL_VER_1,                             /* awbus�汾 */
    AWBL_BUSID_PLB | AWBL_DEVID_BUSCTRL,    /* ����ID*/
    AWBL_IMX6UL_USBH_DRV_NAME,              /* ��������*/
    &__g_drvfuncs,                          /* ������ʼ������*/
    NULL,                                   /* methods */
    NULL                                    /* ̽�⺯��*/
};

/** USB��������ע��*/
void awbl_imx6ul_usbh_drv_register (void)
{
    aw_err_t err;
    err = awbl_drv_register((struct awbl_drvinfo *)&__g_drv_registration);
    err = (aw_err_t)err;
    return;
}

