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
#define __IMX6UL_USB_CAPLENGTH              (0x100)         /* USB 性能寄存器(包含EHCI操作寄存器地址偏移)*/

#define __IMX6UL_USB_CMD                    (0x140)         /* USB主机命令寄存器*/
#define __IMX6UL_UCMD_RUN_STOP              (1 << 0)        /* 控制器运行/停止*/
#define __IMX6UL_UCMD_RESET                 (1 << 1)        /* 控制器复位*/
#define __IMX6UL_UCMD_ITC_NO_THRESHOLD      (~(0xff << 16)) /* Interrupt Threshold */

#define __IMX6UL_USB_STS                    (0x144)         /* USB 状态寄存器*/
#define __IMX6UL_UOG_PORTSC1                (0x184)         /* USB端口状态和控制寄存器*/
#define __IMX6UL_PORTSC_PTS_MASK            (3 << 30)       /* parallel xcvr select mask */
#define __IMX6UL_PORTSC_PTS_UTMI            (0 << 30)       /* UTMI/UTMI+ */
#define __IMX6UL_PORTSC_PTW                 (1 << 28)       /* UTMI 宽度*/
#define __USBH_REG_PORTSC1_PSPD_BIT         (26)            /* USB 端口状态和控制寄存器中的端口速度位*/

#define __IMX6UL_PORT_PTS_MSK               (3<<30)
#define __IMX6UL_PORT_PTS_UTMI              (0<<30)
#define __IMX6UL_PORT_PTS_ULPI              (2<<30)
#define __IMX6UL_PORT_PTS_SERIAL            (3<<30)
#define __IMX6UL_PORT_PTS_PTW               (1<<28)
#define __IMX6UL_PORT_PTS_PSPD              (3<<26)
#define __IMX6UL_PORT_PTS_PSPD_H            (2<<26)

#define __IMX6UL_USBPHY_CTRL                (0x30)          /* USB PHY 控制寄存器偏移*/
#define __IMX6UL_USBPHY_CTRL_SET            (0x34)          /* USB PHY 控制寄存器(设置)偏移*/
#define __IMX6UL_USBPHY_CTRL_CLR            (0x38)          /* USB PHY 控制寄存器(清除)偏移*/
#define __IMX6UL_USBPHY_CTRL_SFTRST         (1u<<31)        /* USB PHY 软件复位*/
#define __IMX6UL_USBPHY_CTRL_CLKGATE        (1u<<30)
#define __IMX6UL_USBPHY_CTRL_FSDLL_RST_EN   (1u<<24)
#define __IMX6UL_USBPHY_CTRL_ENUTMILEVEL2   (1u<<15)        /* USB PHY 使能UTMI+ 3级(支持内部全速集线器对低速设备的连接)*/
#define __IMX6UL_USBPHY_CTRL_ENUTMILEVEL3   (1u<<14)        /* USB PHY 使能UTMI+ 2级(支持低速设备)*/
#define __IMX6UL_USBPHY_CTRL_HOSTDISCONIRQ  (1u<<3)         /* USB PHY 主机高速模式下设备断开标志*/
#define __IMX6UL_USBPHY_CTRL_ENHOSTDISCON   (1u<<1)         /* USB PHY 主机高速模式下断开检测使能*/

#define __IMX6UL_USBPHY_PWD                 (0x00)          /* USB PHY 掉电寄存器*/
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
/* 32位寄存器读写操作*/
#define __IMX_REG32_WR(data, addr)  writel(data, (volatile void __iomem *)(addr))
#define __IMX_REG32_RD(addr)        readl((volatile void __iomem *)(addr))

/* 延时函数*/
aw_local aw_inline void __mdelay(uint32_t us)
{
    while (us--) {
        volatile uint32_t i = 26 * 1000;
        while (i--);
    }
}

/**
 * PHY初始化
 * @param p_hc   USB主机控制器结构体
 * @param p_info USB主机控制器信息
 * 说明：
 *  USB PHY(Port Physical Layer)负责最底层的信号转换，作用类似于网口的PHY。PHY有两种接口，
 *  一种是UTMI(USB2.0 Transceiver Macrocell Interface)，一种是ULPI(UTMI+Low Pin Interface)，
 *  前者PIN多，后者PIN少。一般来说如果芯片的USB PHY封装在芯片内，基本采用UTMI+的接口。
 *  不封装到芯片内的一般采用ULPI接口，这样可以减少pin的数量。
 *  PHY初始化函数(__phy_init)，在imx6ul中，USB PHY通用控制寄存器共包含了4个寄存器，
 *  占用了16字节，分别是USB PHY通用控制寄存器，USB PHY通用控制寄存器(设置位)，
 *  USB PHY通用控制寄存器(清除位)，USB PHY通用控制寄存器(取反位)。
 */
aw_local void __imx6ul_phy_init (struct awbl_imx6ul_usbh      *p_hc,
                                 struct awbl_imx6ul_usbh_info *p_info)
{
    uint32_t temp, timeout = 10000;

    /* 停止USB HOST控制器*/
    temp =  __IMX_REG32_RD(p_info->usb_reg + __IMX6UL_USB_CMD);
    temp &= ~__IMX6UL_UCMD_RUN_STOP;
    /* 等待停止操作完成*/
    __IMX_REG32_WR(temp, p_info->usb_reg + __IMX6UL_USB_CMD);
    while (__IMX_REG32_RD(p_info->usb_reg + __IMX6UL_USB_CMD)
                          & __IMX6UL_UCMD_RUN_STOP);
    /* 复位USB HOST控制器*/
    temp |= __IMX6UL_UCMD_RESET;
    __IMX_REG32_WR(temp, p_info->usb_reg + __IMX6UL_USB_CMD);
    while (__IMX_REG32_RD(p_info->usb_reg + __IMX6UL_USB_CMD)
                          & __IMX6UL_UCMD_RESET);
    __mdelay(50);

    /* 复位PHY*/
    __IMX_REG32_WR(__IMX6UL_USBPHY_CTRL_SFTRST,
                    p_info->phy_reg + __IMX6UL_USBPHY_CTRL_SET);
    do {
        temp = __IMX_REG32_RD(p_info->phy_reg + __IMX6UL_USBPHY_CTRL);
    } while (!(temp & __IMX6UL_USBPHY_CTRL_CLKGATE) && (timeout--));

    /* 使能PHY时钟*/
    __IMX_REG32_WR( __IMX6UL_USBPHY_CTRL_SFTRST |
                    __IMX6UL_USBPHY_CTRL_CLKGATE,
                    p_info->phy_reg + __IMX6UL_USBPHY_CTRL_CLR);

    __mdelay(50);
    /* 选择UTMI接口和设置UTMI的数据宽度为16位*/
    temp = __IMX_REG32_RD(p_info->usb_reg + __IMX6UL_UOG_PORTSC1);
    temp &= ~__IMX6UL_PORTSC_PTS_MASK;
    temp |= (__IMX6UL_PORTSC_PTS_UTMI | __IMX6UL_PORTSC_PTW);
    __IMX_REG32_WR(temp, p_info->usb_reg + __IMX6UL_UOG_PORTSC1);

    /* 启动PHY*/
    __IMX_REG32_WR(0xffffffff, p_info->phy_reg + __IMX6UL_USBPHY_PWD_CLR);

    /* 使能支持全速/低速设备*/
    __IMX_REG32_WR( __IMX6UL_USBPHY_CTRL_ENUTMILEVEL2 |
                    __IMX6UL_USBPHY_CTRL_ENUTMILEVEL3,
                    p_info->phy_reg + __IMX6UL_USBPHY_CTRL_SET);
}

/** USB主机中断服务函数*/
aw_local void __imx6ul_usbh_isr(void *p_arg)
{
    struct awbl_imx6ul_usbh      *p_this;
    struct awbl_imx6ul_usbh_info *p_info;
    volatile uint32_t             status;

    p_this = __AWDEV_TO_IMX(p_arg);
    p_info = (struct awbl_imx6ul_usbh_info *)AWBL_DEVINFO_GET(p_arg);

    /* 读取USB状态寄存器*/
    status = __IMX_REG32_RD(p_info->usb_reg + __IMX6UL_USB_STS);
    /* 端口发生变化*/
    if (status & (1<<2)) {
        uint32_t temp;

        /* 读取USB PHY控制寄存器*/
        temp = __IMX_REG32_RD(p_info->phy_reg + __IMX6UL_USBPHY_CTRL);
        /* 高速模式下主机有设备断开连接*/
        if (temp & __IMX6UL_USBPHY_CTRL_HOSTDISCONIRQ) {
            __IMX_REG32_WR((__IMX6UL_USBPHY_CTRL_ENHOSTDISCON |
                            __IMX6UL_USBPHY_CTRL_HOSTDISCONIRQ),
                            p_info->phy_reg + __IMX6UL_USBPHY_CTRL_CLR);

        } else {
            /* 使能高速模式下的设备断开检测*/
            __IMX_REG32_WR(__IMX6UL_USBPHY_CTRL_ENHOSTDISCON,
                           p_info->phy_reg + __IMX6UL_USBPHY_CTRL_SET);
        }
    }

    /* 调用EHCI中断处理函数*/
    usbh_ehci_irq(&p_this->aw_ehci.p_echi);
}

/** 第一阶段初始化*/
aw_local void __drv_init1 (struct awbl_dev *p_awdev)
{
    return ;
}

/** 第二阶段初始化*/
aw_local void __drv_init2 (struct awbl_dev *p_awdev)
{
    struct awbl_imx6ul_usbh      *p_this;
    struct awbl_imx6ul_usbh_info *p_info;

    p_this = __AWDEV_TO_IMX(p_awdev);
    p_info = (struct awbl_imx6ul_usbh_info *)AWBL_DEVINFO_GET(p_awdev);

    /* 平台初始化*/
    if(p_info->pfunc_plfm_init) {
        p_info->pfunc_plfm_init();
    }
    /* 上电*/
    if (p_info->pfunc_pwr_ctrl) {
        p_info->pfunc_pwr_ctrl(AW_TRUE);
    }

    __mdelay(50);
    /* 初始化PHY*/
    __imx6ul_phy_init(p_this, p_info);

    return;
}

/** 第三阶段初始化*/
aw_local void __drv_connect (struct awbl_dev *p_awdev)
{
    aw_err_t ret;
    struct awbl_imx6ul_usbh      *p_this;
    struct awbl_imx6ul_usbh_info *p_info;

    p_this = __AWDEV_TO_IMX(p_awdev);
    p_info = (struct awbl_imx6ul_usbh_info *)AWBL_DEVINFO_GET(p_awdev);

    /* 创建USB主机*/
    ret = awbl_usbh_host_create(&p_this->aw_usbh);
    if(ret != AW_OK){
        return;
    }
    /* 初始化EHCI*/
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
    /* 初始化USB主机根集线器*/
    ret = usbh_rh_init(p_this->aw_usbh.hc, p_this->aw_ehci.p_echi.nports, USB_SPEED_HIGH);
    if(ret != USB_OK){
        return;
    }

    /* 设置USB主机控制器*/
    ret = usbh_ctrllor_set(p_this->aw_usbh.hc, &p_this->aw_ehci.p_echi);
    if(ret != AW_OK){
        return;
    }

    /* 设置中断服务函数*/
    aw_int_connect(p_info->inum,
                   (aw_pfuncvoid_t)__imx6ul_usbh_isr,
                   (void *)p_awdev);
    /* 使能中断*/
    aw_int_enable(p_info->inum);

    return ;
}

/* 三阶段初始化函数集*/
aw_local aw_const struct awbl_drvfuncs __g_drvfuncs = {
        __drv_init1,
        __drv_init2,
        __drv_connect
};

/* USB主机驱动信息*/
aw_local aw_const struct awbl_drvinfo __g_drv_registration = {
    AWBL_VER_1,                             /* awbus版本 */
    AWBL_BUSID_PLB | AWBL_DEVID_BUSCTRL,    /* 总线ID*/
    AWBL_IMX6UL_USBH_DRV_NAME,              /* 驱动名字*/
    &__g_drvfuncs,                          /* 驱动初始化函数*/
    NULL,                                   /* methods */
    NULL                                    /* 探测函数*/
};

/** USB主机驱动注册*/
void awbl_imx6ul_usbh_drv_register (void)
{
    aw_err_t err;
    err = awbl_drv_register((struct awbl_drvinfo *)&__g_drv_registration);
    err = (aw_err_t)err;
    return;
}

