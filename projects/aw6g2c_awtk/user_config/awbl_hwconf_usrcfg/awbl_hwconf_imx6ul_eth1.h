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

#ifndef __AWBL_HWCONF_IMX6UL_ETH1_H
#define __AWBL_HWCONF_IMX6UL_ETH1_H


#include "driver/net/awbl_imx6ul_enet.h"

#define ENET_PAD_CTRL (PUS_2_100K_Ohm_Pull_Up    | \
                       PKE_1_Pull_Keeper_Enabled | \
                       PUE_1_Pull                | \
                       SPEED_3_max_200MHz        | \
                       DSE_5_R0_5                | \
                       SRE_0_Fast_Slew_Rate)

#define MDIO_PAD_CTRL (PUS_2_100K_Ohm_Pull_Up    | \
                       PKE_1_Pull_Keeper_Enabled | \
                       PUE_1_Pull                | \
                       DSE_5_R0_5                | \
                       SRE_0_Fast_Slew_Rate      | \
                       Open_Drain_Enabled)

#define ENET_CLK_PAD_CTRL (DSE_6_R0_6 | SRE_0_Fast_Slew_Rate)

#ifdef AW_DEV_IMX6UL_ENET1

aw_local void enet_reset_phy (void)
{
    aw_udelay(1000);
    aw_gpio_set(GPIO5_3, 0);
    aw_udelay(1000);
    aw_gpio_set(GPIO5_3, 1);
    aw_udelay(1000);
}

/* 平台相关初始化 */
aw_local void __imx6ul_enet_plfm_init (void)
{
    int enet_gpios[] = {
            GPIO2_6,//TX_CLK
            GPIO5_3,
            GPIO1_6,//ENET_MDIO
            GPIO1_7,//ENET_MDC
            GPIO2_5,//TXEN
            GPIO2_3,//TXD0
            GPIO2_4,//TRXD1
            GPIO2_2,//RXEN
            GPIO2_7,//RXER
            GPIO2_0,//RXD0
            GPIO2_1,//RXD1
     };

    if (aw_gpio_pin_request(AWBL_IMX6UL_ENET_NAME, enet_gpios, AW_NELEMENTS(enet_gpios)) == AW_OK) {
        /* 先为PHY芯片提供50Mhz时钟输入 */

        aw_gpio_pin_cfg(GPIO2_6, GPIO2_6_ENET1_REF_CLK1|IMX6UL_PAD_CTL(ENET_CLK_PAD_CTRL));    //ENET1_TXCLK
        aw_udelay(100);

        //ETH1_nRST
        aw_gpio_pin_cfg(GPIO5_3, GPIO5_3_GPIO|AW_GPIO_OUTPUT_INIT_HIGH_VAL);

        aw_gpio_pin_cfg(GPIO2_2, GPIO2_2_GPIO|AW_GPIO_OUTPUT_INIT_HIGH_VAL); //ETH1_RXEN,配置RMII
        aw_gpio_pin_cfg(GPIO2_0, GPIO2_0_GPIO|AW_GPIO_OUTPUT_INIT_LOW_VAL);  //ETH1_RXD0，配置地址
        aw_gpio_pin_cfg(GPIO2_1, GPIO2_1_GPIO|AW_GPIO_OUTPUT_INIT_LOW_VAL);  //ETH1_RXD1，配置地址


        aw_clk_enable(IMX6UL_CLK_ENET1_TX_REF_CLK);
        aw_clk_enable(IMX6UL_CLK_ENET_PTP);
        aw_clk_enable(IMX6UL_CLK_ENET);

        /**
         * Hard Reset PHY
         */
        enet_reset_phy();

        aw_gpio_pin_cfg(GPIO1_6, GPIO1_6_ENET1_MDIO|IMX6UL_PAD_CTL(MDIO_PAD_CTRL));//ETH1_MDIO
        aw_gpio_pin_cfg(GPIO1_7, GPIO1_7_ENET1_MDC|IMX6UL_PAD_CTL(ENET_PAD_CTRL));//ETH1_MDC

        aw_gpio_pin_cfg(GPIO2_5, GPIO2_5_ENET1_TX_EN|IMX6UL_PAD_CTL(ENET_PAD_CTRL));//ETH1_TXEN
        aw_gpio_pin_cfg(GPIO2_3, GPIO2_3_ENET1_TDATA00|IMX6UL_PAD_CTL(ENET_PAD_CTRL));//ETH1_TXD0
        aw_gpio_pin_cfg(GPIO2_4, GPIO2_4_ENET1_TDATA01|IMX6UL_PAD_CTL(ENET_PAD_CTRL));//ETH1_TXD1

        aw_gpio_pin_cfg(GPIO2_2, GPIO2_2_ENET1_RX_EN|IMX6UL_PAD_CTL(ENET_PAD_CTRL));//ETH1_RXEN
        aw_gpio_pin_cfg(GPIO2_7, GPIO2_7_ENET1_RX_ER|IMX6UL_PAD_CTL(ENET_PAD_CTRL));//ETH1_RXER
        aw_gpio_pin_cfg(GPIO2_0, GPIO2_0_ENET1_RDATA00|IMX6UL_PAD_CTL(ENET_PAD_CTRL));//ETH1_RXD0
        aw_gpio_pin_cfg(GPIO2_1, GPIO2_1_ENET1_RDATA01|IMX6UL_PAD_CTL(ENET_PAD_CTRL));//ETH1_RXD1
    }
}

aw_local char *__get_ipaddr (void)
{
    return "192.168.40.110";
}

aw_local char *__get_netmsk (void)
{
    return "255.255.255.0";
}

aw_local char *__get_gateway (void)
{
    return "192.168.40.13";
}

/* 试图获取从OTP中获取以太网地址，如果失败则随机产生 */

/** \brief 判断以太网地址是否全为0 */
aw_local inline aw_bool_t is_zero_ether_addr (const uint8_t *addr)
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
aw_local inline aw_bool_t is_multicast_ether_addr (const uint8_t *addr)
{
    if (0x1 & addr[0]) {
        return AW_TRUE;
    }

    return AW_FALSE;
}

/** \brief 判断以太网地址是否合法 */
aw_local inline aw_bool_t is_valid_ether_addr (const uint8_t *addr)
{
    /* FF:FF:FF:FF:FF:FF is a multicast address so we don't need to
     * explicitly check for it here. */
    return !is_multicast_ether_addr(addr) && !is_zero_ether_addr(addr);
}

aw_local aw_err_t __get_macaddr (uint8_t *p_mac_buf, int buf_len)
{
    int i;
    uint8_t data[6];
    uint8_t temp_addr[6];

    aw_nvram_get("otp_zy_mac", 2, (char *) data, 0, sizeof(data));
    for (i = 0; i < 6; i++) {
        temp_addr[i] = data[5 - i];
    }

    if (!is_valid_ether_addr(temp_addr)) {
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

aw_local aw_bool_t __get_dhcp_en (void)
{
     return AW_FALSE;
}

aw_local aw_const aw_netif_info_get_entry_t enet_info = {
    __get_ipaddr,
    __get_netmsk,
    __get_gateway,
    __get_macaddr,
    __get_dhcp_en,
};


/* 设备信息 */
aw_local aw_const struct awbl_imx6ul_enet_devinfo __g_imx6ul_eth1_devinfo = {
    "eth1",
    &enet_info,
    AW_TASK_SYS_PRIORITY(5),        /* 接收任务优先级    */

    IMX6UL_ENET1_BASE_ADDR,              /* enet寄存器基地址 */
    INUM_ENET1,                      /* 中断编号 */

    IMX6UL_MII0_BUSID,

    PHY_INTERFACE_MODE_RMII,        /* PHY MODE */
    __imx6ul_enet_plfm_init,
};

/* 设备实例内存静态分配 */
aw_local struct awbl_imx6ul_enet_dev __g_imx6ul_eth1_dev;

/* 通过单元号来间接的控制mii_bus设备的单元号  */
#define AWBL_HWCONF_IMX6UL_ETH1                      \
    {                                                 \
        AWBL_IMX6UL_ENET_NAME,                       \
        0,                                            \
        AWBL_BUSID_PLB,                               \
        0,                                            \
        (struct awbl_dev *) &__g_imx6ul_eth1_dev,    \
        &__g_imx6ul_eth1_devinfo                     \
    },

#else
#define AWBL_HWCONF_IMX6UL_ETH1

#endif  /* AW_DEV_IMX6UL_ENET */

#endif  /* __AWBL_HWCONF_IMX6UL_ENET1_H */

/* end of file */

