/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

#ifndef __AWBL_HWCONF_IMX6UL_ETH0_H
#define __AWBL_HWCONF_IMX6UL_ETH0_H

#ifdef AW_DEV_IMX6UL_ENET0

#include "driver/net/awbl_imx6ul_enet.h"
#include <string.h>

aw_local void eth0_reset_phy (void)
{
    aw_udelay(1000);
    aw_gpio_set(GPIO5_4,0);
    aw_udelay(1000);
    aw_gpio_set(GPIO5_4,1);
    aw_udelay(1000);
}

/* 平台相关初始化 */
aw_local void __imx6ul_eth0_plfm_init (void)
{
    aw_clk_enable(IMX6UL_CLK_ENET2_TX_REF_CLK);
    aw_clk_enable(IMX6UL_CLK_ENET_PTP);
    aw_clk_enable(IMX6UL_CLK_ENET);

    int enet_gpios[] = {
            GPIO2_14,//TX_CLK
            GPIO5_4,
#ifndef AW_DEV_IMX6UL_ENET1
            GPIO1_6,
            GPIO1_7,
#endif
            GPIO2_13,//TXEN
            GPIO2_11,//TXD0
            GPIO2_12,//TXD1
            GPIO2_10,//RXEN
            GPIO2_15,//RXER
            GPIO2_8, //RXD0
            GPIO2_9, //RXD1
     };

    if (aw_gpio_pin_request(AWBL_IMX6UL_ENET_NAME, enet_gpios, AW_NELEMENTS(enet_gpios)) == AW_OK) {
        /* 先为PHY芯片提供50Mhz时钟输入 */

        aw_gpio_pin_cfg(GPIO2_14, GPIO2_14_ENET2_REF_CLK2|IMX6UL_PAD_CTL(ENET_CLK_PAD_CTRL));    //ETH0_TXCLK
        aw_udelay(100);

        //ETH0_nRST
        aw_gpio_pin_cfg(GPIO5_4, GPIO5_4_GPIO|AW_GPIO_OUTPUT_INIT_HIGH_VAL);

        aw_gpio_pin_cfg(GPIO2_10, GPIO2_10_GPIO|AW_GPIO_OUTPUT_INIT_HIGH_VAL); //ETH1_RXEN,配置RMII
        aw_gpio_pin_cfg(GPIO2_8,  GPIO2_8_GPIO|AW_GPIO_OUTPUT_INIT_LOW_VAL);  //ETH1_RXD0，配置地址
        aw_gpio_pin_cfg(GPIO2_9,  GPIO2_9_GPIO|AW_GPIO_OUTPUT_INIT_LOW_VAL);  //ETH1_RXD1，配置地址

        aw_clk_enable(IMX6UL_CLK_ENET2_TX_REF_CLK);
        aw_clk_enable(IMX6UL_CLK_ENET_PTP);
        aw_clk_enable(IMX6UL_CLK_ENET);

        /**
         * Hard Reset PHY
         */
        eth0_reset_phy();

#ifndef AW_DEV_IMX6UL_ENET1
        aw_gpio_pin_cfg(GPIO1_6, GPIO1_6_ENET2_MDIO|IMX6UL_PAD_CTL(MDIO_PAD_CTRL));//ETH1_MDIO
        aw_gpio_pin_cfg(GPIO1_7, GPIO1_7_ENET2_MDC|IMX6UL_PAD_CTL(ENET_PAD_CTRL));//ETH1_MDC
#endif
        aw_gpio_pin_cfg(GPIO2_13, GPIO2_13_ENET2_TX_EN|IMX6UL_PAD_CTL(ENET_PAD_CTRL));//ETH0_TXEN
        aw_gpio_pin_cfg(GPIO2_11, GPIO2_11_ENET2_TDATA00|IMX6UL_PAD_CTL(ENET_PAD_CTRL));//ETH0_TXD0
        aw_gpio_pin_cfg(GPIO2_12, GPIO2_12_ENET2_TDATA01|IMX6UL_PAD_CTL(ENET_PAD_CTRL));//ETH0_TXD1

        aw_gpio_pin_cfg(GPIO2_10, GPIO2_10_ENET2_RX_EN|IMX6UL_PAD_CTL(ENET_PAD_CTRL));//ETH0_RXEN
        aw_gpio_pin_cfg(GPIO2_15, GPIO2_15_ENET2_RX_ER|IMX6UL_PAD_CTL(ENET_PAD_CTRL));//ETH0_RXER
        aw_gpio_pin_cfg(GPIO2_8, GPIO2_8_ENET2_RDATA00|IMX6UL_PAD_CTL(ENET_PAD_CTRL));//ETH0_RXD0
        aw_gpio_pin_cfg(GPIO2_9, GPIO2_9_ENET2_RDATA01|IMX6UL_PAD_CTL(ENET_PAD_CTRL));//ETH0_RXD1
    }
}

aw_local char *__get_ipaddr_0 (void)
{
    return "192.168.40.200";
}

aw_local char *__get_netmsk_0 (void)
{
    return "255.255.255.0";
}

aw_local char *__get_gateway_0 (void)
{
    return "192.168.40.254";
}

/* 试图获取从OTP中获取以太网地址，如果失败则随机产生 */

/** \brief 判断以太网地址是否全为0 */
aw_local inline aw_bool_t is_zero_eth0_addr (const uint8_t *addr)
{
    uint8_t val = 0;
    val = addr[0] | addr[1] | addr[2] | addr[3] | addr[4] | addr[5];
    if (val) {
        return AW_FALSE;
    } else {
        return AW_TRUE;
    }
}

/** \brief 判断以太网地址是否为多播地址 */
aw_local inline aw_bool_t is_multicast_eth0_addr (const uint8_t *addr)
{
    if (0x1 & addr[0]) {
        return AW_TRUE;
    }

    return AW_FALSE;
}

/** \brief 判断以太网地址是否合法 */
aw_local inline aw_bool_t is_valid_eth0_addr (const uint8_t *addr)
{
    /* FF:FF:FF:FF:FF:FF is a multicast address so we don't need to
     * explicitly check for it here. */
    return !is_multicast_eth0_addr(addr) && !is_zero_eth0_addr(addr);
}

aw_local aw_err_t __get_macaddr_0 (uint8_t *p_mac_buf, int buf_len)
{
    int i;
    uint8_t data[6];
    uint8_t temp_addr[6];

    aw_nvram_get("otp_zy_mac", 1, (char *) data, 0, sizeof(data));
    for (i = 0; i < 6; i++) {
        temp_addr[i] = data[5 - i];
    }

    if (!is_valid_eth0_addr(temp_addr)) {
        AW_WARNF(("invalid mac address, generate one\r\n"));

        srand(aw_sys_tick_get());

        temp_addr[0] = 0x00;
        temp_addr[1] = 0x14;
        temp_addr[2] = 0x97;
        temp_addr[3] = rand();
        temp_addr[4] = rand();
        temp_addr[5] = rand();
    }

    memcpy(p_mac_buf, temp_addr, sizeof(temp_addr));
    return AW_OK;
}

aw_local aw_bool_t __get_dhcp_en_0 (void)
{
     return AW_FALSE;
}

aw_local aw_const aw_netif_info_get_entry_t eth0_info = {
    __get_ipaddr_0,
    __get_netmsk_0,
    __get_gateway_0,
    __get_macaddr_0,
    __get_dhcp_en_0,
};


/* 设备信息 */
aw_local aw_const struct awbl_imx6ul_enet_devinfo __g_imx6ul_eth0_devinfo = {
    "eth0",
    &eth0_info,
    AW_TASK_SYS_PRIORITY(5),        /* 接收任务优先级    */

    IMX6UL_ENET2_BASE_ADDR,         /* enet寄存器基地址 */
    INUM_ENET2,                     /* 中断编号 */

    IMX6UL_MII0_BUSID,

    PHY_INTERFACE_MODE_RMII,        /* PHY MODE */
    __imx6ul_eth0_plfm_init,
};

/* 设备实例内存静态分配 */
aw_local struct awbl_imx6ul_enet_dev __g_imx6ul_eth0_dev;

#define AWBL_HWCONF_IMX6UL_ETH0                       \
    {                                                 \
        AWBL_IMX6UL_ENET_NAME,                       \
        1,                                            \
        AWBL_BUSID_PLB,                               \
        0,                                            \
        (struct awbl_dev *) &__g_imx6ul_eth0_dev,    \
        &__g_imx6ul_eth0_devinfo                     \
    },

#else
#define AWBL_HWCONF_IMX6UL_ETH0

#endif  /* AW_DEV_IMX6UL_ENET */

#endif  /* __AWBL_HWCONF_IMX6UL_ETH0_H */

/* end of file */

