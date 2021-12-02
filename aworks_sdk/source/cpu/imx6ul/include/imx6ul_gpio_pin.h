/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief IMX6UL GPIO引脚定义
 *
 * 定义GPIO引脚
 *
 * \par 1.兼容设备
 *
 *  - 飞思卡尔IMX6UL MCU
 *
 * \internal
 * \par modification history
 * - 1.00 2015.10.21  jac, first implementation
 * \endinternal
 */

#ifndef __IMX6UL_GPIO_PIN_H
#define __IMX6UL_GPIO_PIN_H

#include "apollo.h"

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus	*/

/**
 * \addtogroup grp_awbl_drv_imx6ul_gpio IMX6UL GPIO驱动
 * @{
 */
/**
 * \addtogroup grp_awbl_drv_imx6ul_gpio_pin_defs GPIO引脚定义
 * @{
 */

typedef enum
{
    GPIO0 = 0,          /**< GPIO1,bit0 */
    GPIO1_0 = GPIO0,    /**< GPIO1,bit0 */
    GPIO1 = 1,          /**< GPIO1,bit1 */
    GPIO1_1 = GPIO1,    /**< GPIO1,bit1 */
    GPIO2 = 2,          /**< GPIO1,bit2 */
    GPIO1_2 = GPIO2,    /**< GPIO1,bit2 */
    GPIO3 = 3,          /**< GPIO1,bit3 */
    GPIO1_3 = GPIO3,    /**< GPIO1,bit3 */
    GPIO4 = 4,          /**< GPIO1,bit4 */
    GPIO1_4 = GPIO4,    /**< GPIO1,bit4 */
    GPIO5 = 5,          /**< GPIO1,bit5 */
    GPIO1_5 = GPIO5,    /**< GPIO1,bit5 */
    GPIO6 = 6,          /**< GPIO1,bit6 */
    GPIO1_6 = GPIO6,    /**< GPIO1,bit6 */
    GPIO7 = 7,          /**< GPIO1,bit7 */
    GPIO1_7 = GPIO7,    /**< GPIO1,bit7 */
    GPIO8 = 8,          /**< GPIO1,bit8 */
    GPIO1_8 = GPIO8,    /**< GPIO1,bit8 */
    GPIO9 = 9,          /**< GPIO1,bit9 */
    GPIO1_9 = GPIO9,    /**< GPIO1,bit9 */
    GPIO10 = 10,        /**< GPIO1,bit10 */
    GPIO1_10 = GPIO10,  /**< GPIO1,bit10 */
    GPIO11 = 11,        /**< GPIO1,bit11 */
    GPIO1_11 = GPIO11,  /**< GPIO1,bit11 */
    GPIO12 = 12,        /**< GPIO1,bit12 */
    GPIO1_12 = GPIO12,  /**< GPIO1,bit12 */
    GPIO13 = 13,        /**< GPIO1,bit13 */
    GPIO1_13 = GPIO13,  /**< GPIO1,bit13 */
    GPIO14 = 14,        /**< GPIO1,bit14 */
    GPIO1_14 = GPIO14,  /**< GPIO1,bit14 */
    GPIO15 = 15,        /**< GPIO1,bit15 */
    GPIO1_15 = GPIO15,  /**< GPIO1,bit15 */
    GPIO16 = 16,        /**< GPIO1,bit16 */
    GPIO1_16 = GPIO16,  /**< GPIO1,bit16 */
    GPIO17 = 17,        /**< GPIO1,bit17 */
    GPIO1_17 = GPIO17,  /**< GPIO1,bit17 */
    GPIO18 = 18,        /**< GPIO1,bit18 */
    GPIO1_18 = GPIO18,  /**< GPIO1,bit18 */
    GPIO19 = 19,        /**< GPIO1,bit19 */
    GPIO1_19 = GPIO19,  /**< GPIO1,bit19 */
    GPIO20 = 20,        /**< GPIO1,bit20 */
    GPIO1_20 = GPIO20,  /**< GPIO1,bit20 */
    GPIO21 = 21,        /**< GPIO1,bit21 */
    GPIO1_21 = GPIO21,  /**< GPIO1,bit21 */
    GPIO22 = 22,        /**< GPIO1,bit22 */
    GPIO1_22 = GPIO22,  /**< GPIO1,bit22 */
    GPIO23 = 23,        /**< GPIO1,bit23 */
    GPIO1_23 = GPIO23,  /**< GPIO1,bit23 */
    GPIO24 = 24,        /**< GPIO1,bit24 */
    GPIO1_24 = GPIO24,  /**< GPIO1,bit24 */
    GPIO25 = 25,        /**< GPIO1,bit25 */
    GPIO1_25 = GPIO25,  /**< GPIO1,bit25 */
    GPIO26 = 26,        /**< GPIO1,bit26 */
    GPIO1_26 = GPIO26,  /**< GPIO1,bit26 */
    GPIO27 = 27,        /**< GPIO1,bit27 */
    GPIO1_27 = GPIO27,  /**< GPIO1,bit27 */
    GPIO28 = 28,        /**< GPIO1,bit28 */
    GPIO1_28 = GPIO28,  /**< GPIO1,bit28 */
    GPIO29 = 29,        /**< GPIO1,bit29 */
    GPIO1_29 = GPIO29,  /**< GPIO1,bit29 */
    GPIO30 = 30,        /**< GPIO1,bit30 */
    GPIO1_30 = GPIO30,  /**< GPIO1,bit30 */
    GPIO31 = 31,        /**< GPIO1,bit31 */
    GPIO1_31 = GPIO31,  /**< GPIO1,bit31 */
    GPIO32 = 32,        /**< GPIO2,bit0 */
    GPIO2_0 = GPIO32,   /**< GPIO2,bit0 */
    GPIO33 = 33,        /**< GPIO2,bit1 */
    GPIO2_1 = GPIO33,   /**< GPIO2,bit1 */
    GPIO34 = 34,        /**< GPIO2,bit2 */
    GPIO2_2 = GPIO34,   /**< GPIO2,bit2 */
    GPIO35 = 35,        /**< GPIO2,bit3 */
    GPIO2_3 = GPIO35,   /**< GPIO2,bit3 */
    GPIO36 = 36,        /**< GPIO2,bit4 */
    GPIO2_4 = GPIO36,   /**< GPIO2,bit4 */
    GPIO37 = 37,        /**< GPIO2,bit5 */
    GPIO2_5 = GPIO37,   /**< GPIO2,bit5 */
    GPIO38 = 38,        /**< GPIO2,bit6 */
    GPIO2_6 = GPIO38,   /**< GPIO2,bit6 */
    GPIO39 = 39,        /**< GPIO2,bit7 */
    GPIO2_7 = GPIO39,   /**< GPIO2,bit7 */
    GPIO40 = 40,        /**< GPIO2,bit8 */
    GPIO2_8 = GPIO40,   /**< GPIO2,bit8 */
    GPIO41 = 41,        /**< GPIO2,bit9 */
    GPIO2_9 = GPIO41,   /**< GPIO2,bit9 */
    GPIO42 = 42,        /**< GPIO2,bit10 */
    GPIO2_10 = GPIO42,  /**< GPIO2,bit10 */
    GPIO43 = 43,        /**< GPIO2,bit11 */
    GPIO2_11 = GPIO43,  /**< GPIO2,bit11 */
    GPIO44 = 44,        /**< GPIO2,bit12 */
    GPIO2_12 = GPIO44,  /**< GPIO2,bit12 */
    GPIO45 = 45,        /**< GPIO2,bit13 */
    GPIO2_13 = GPIO45,  /**< GPIO2,bit13 */
    GPIO46 = 46,        /**< GPIO2,bit14 */
    GPIO2_14 = GPIO46,  /**< GPIO2,bit14 */
    GPIO47 = 47,        /**< GPIO2,bit15 */
    GPIO2_15 = GPIO47,  /**< GPIO2,bit15 */
    GPIO48 = 48,        /**< GPIO2,bit16 */
    GPIO2_16 = GPIO48,  /**< GPIO2,bit16 */
    GPIO49 = 49,        /**< GPIO2,bit17 */
    GPIO2_17 = GPIO49,  /**< GPIO2,bit17 */
    GPIO50 = 50,        /**< GPIO2,bit18 */
    GPIO2_18 = GPIO50,  /**< GPIO2,bit18 */
    GPIO51 = 51,        /**< GPIO2,bit19 */
    GPIO2_19 = GPIO51,  /**< GPIO2,bit19 */
    GPIO52 = 52,        /**< GPIO2,bit20 */
    GPIO2_20 = GPIO52,  /**< GPIO2,bit20 */
    GPIO53 = 53,        /**< GPIO2,bit21 */
    GPIO2_21 = GPIO53,  /**< GPIO2,bit21 */
    GPIO54 = 54,        /**< GPIO2,bit22 */
    GPIO2_22 = GPIO54,  /**< GPIO2,bit22 */
    GPIO55 = 55,        /**< GPIO2,bit23 */
    GPIO2_23 = GPIO55,  /**< GPIO2,bit23 */
    GPIO56 = 56,        /**< GPIO2,bit24 */
    GPIO2_24 = GPIO56,  /**< GPIO2,bit24 */
    GPIO57 = 57,        /**< GPIO2,bit25 */
    GPIO2_25 = GPIO57,  /**< GPIO2,bit25 */
    GPIO58 = 58,        /**< GPIO2,bit26 */
    GPIO2_26 = GPIO58,  /**< GPIO2,bit26 */
    GPIO59 = 59,        /**< GPIO2,bit27 */
    GPIO2_27 = GPIO59,  /**< GPIO2,bit27 */
    GPIO60 = 60,        /**< GPIO2,bit28 */
    GPIO2_28 = GPIO60,  /**< GPIO2,bit28 */
    GPIO61 = 61,        /**< GPIO2,bit29 */
    GPIO2_29 = GPIO61,  /**< GPIO2,bit29 */
    GPIO62 = 62,        /**< GPIO2,bit30 */
    GPIO2_30 = GPIO62,  /**< GPIO2,bit30 */
    GPIO63 = 63,        /**< GPIO2,bit31 */
    GPIO2_31 = GPIO63,  /**< GPIO2,bit31 */
    GPIO64 = 64,        /**< GPIO3,bit0 */
    GPIO3_0 = GPIO64,   /**< GPIO3,bit0 */
    GPIO65 = 65,        /**< GPIO3,bit1 */
    GPIO3_1 = GPIO65,   /**< GPIO3,bit1 */
    GPIO66 = 66,        /**< GPIO3,bit2 */
    GPIO3_2 = GPIO66,   /**< GPIO3,bit2 */
    GPIO67 = 67,        /**< GPIO3,bit3 */
    GPIO3_3 = GPIO67,   /**< GPIO3,bit3 */
    GPIO68 = 68,        /**< GPIO3,bit4 */
    GPIO3_4 = GPIO68,   /**< GPIO3,bit4 */
    GPIO69 = 69,        /**< GPIO3,bit5 */
    GPIO3_5 = GPIO69,   /**< GPIO3,bit5 */
    GPIO70 = 70,        /**< GPIO3,bit6 */
    GPIO3_6 = GPIO70,   /**< GPIO3,bit6 */
    GPIO71 = 71,        /**< GPIO3,bit7 */
    GPIO3_7 = GPIO71,   /**< GPIO3,bit7 */
    GPIO72 = 72,        /**< GPIO3,bit8 */
    GPIO3_8 = GPIO72,   /**< GPIO3,bit8 */
    GPIO73 = 73,        /**< GPIO3,bit9 */
    GPIO3_9 = GPIO73,   /**< GPIO3,bit9 */
    GPIO74 = 74,        /**< GPIO3,bit10 */
    GPIO3_10 = GPIO74,  /**< GPIO3,bit10 */
    GPIO75 = 75,        /**< GPIO3,bit11 */
    GPIO3_11 = GPIO75,  /**< GPIO3,bit11 */
    GPIO76 = 76,        /**< GPIO3,bit12 */
    GPIO3_12 = GPIO76,  /**< GPIO3,bit12 */
    GPIO77 = 77,        /**< GPIO3,bit13 */
    GPIO3_13 = GPIO77,  /**< GPIO3,bit13 */
    GPIO78 = 78,        /**< GPIO3,bit14 */
    GPIO3_14 = GPIO78,  /**< GPIO3,bit14 */
    GPIO79 = 79,        /**< GPIO3,bit15 */
    GPIO3_15 = GPIO79,  /**< GPIO3,bit15 */
    GPIO80 = 80,        /**< GPIO3,bit16 */
    GPIO3_16 = GPIO80,  /**< GPIO3,bit16 */
    GPIO81 = 81,        /**< GPIO3,bit17 */
    GPIO3_17 = GPIO81,  /**< GPIO3,bit17 */
    GPIO82 = 82,        /**< GPIO3,bit18 */
    GPIO3_18 = GPIO82,  /**< GPIO3,bit18 */
    GPIO83 = 83,        /**< GPIO3,bit19 */
    GPIO3_19 = GPIO83,  /**< GPIO3,bit19 */
    GPIO84 = 84,        /**< GPIO3,bit20 */
    GPIO3_20 = GPIO84,  /**< GPIO3,bit20 */
    GPIO85 = 85,        /**< GPIO3,bit21 */
    GPIO3_21 = GPIO85,  /**< GPIO3,bit21 */
    GPIO86 = 86,        /**< GPIO3,bit22 */
    GPIO3_22 = GPIO86,  /**< GPIO3,bit22 */
    GPIO87 = 87,        /**< GPIO3,bit23 */
    GPIO3_23 = GPIO87,  /**< GPIO3,bit23 */
    GPIO88 = 88,        /**< GPIO3,bit24 */
    GPIO3_24 = GPIO88,  /**< GPIO3,bit24 */
    GPIO89 = 89,        /**< GPIO3,bit25 */
    GPIO3_25 = GPIO89,  /**< GPIO3,bit25 */
    GPIO90 = 90,        /**< GPIO3,bit26 */
    GPIO3_26 = GPIO90,  /**< GPIO3,bit26 */
    GPIO91 = 91,        /**< GPIO3,bit27 */
    GPIO3_27 = GPIO91,  /**< GPIO3,bit27 */
    GPIO92 = 92,        /**< GPIO3,bit28 */
    GPIO3_28 = GPIO92,  /**< GPIO3,bit28 */
    GPIO93 = 93,        /**< GPIO3,bit29 */
    GPIO3_29 = GPIO93,  /**< GPIO3,bit29 */
    GPIO94 = 94,        /**< GPIO3,bit30 */
    GPIO3_30 = GPIO94,  /**< GPIO3,bit30 */
    GPIO95 = 95,        /**< GPIO3,bit31 */
    GPIO3_31 = GPIO95,  /**< GPIO3,bit31 */
    GPIO96 = 96,        /**< GPIO4,bit0 */
    GPIO4_0 = GPIO96,   /**< GPIO4,bit0 */
    GPIO97 = 97,        /**< GPIO4,bit1 */
    GPIO4_1 = GPIO97,   /**< GPIO4,bit1 */
    GPIO98 = 98,        /**< GPIO4,bit2 */
    GPIO4_2 = GPIO98,   /**< GPIO4,bit2 */
    GPIO99 = 99,        /**< GPIO4,bit3 */
    GPIO4_3 = GPIO99,   /**< GPIO4,bit3 */
    GPIO100 = 100,      /**< GPIO4,bit4 */
    GPIO4_4 = GPIO100,  /**< GPIO4,bit4 */
    GPIO101 = 101,      /**< GPIO4,bit5 */
    GPIO4_5 = GPIO101,  /**< GPIO4,bit5 */
    GPIO102 = 102,      /**< GPIO4,bit6 */
    GPIO4_6 = GPIO102,  /**< GPIO4,bit6 */
    GPIO103 = 103,      /**< GPIO4,bit7 */
    GPIO4_7 = GPIO103,  /**< GPIO4,bit7 */
    GPIO104 = 104,      /**< GPIO4,bit8 */
    GPIO4_8 = GPIO104,  /**< GPIO4,bit8 */
    GPIO105 = 105,      /**< GPIO4,bit9 */
    GPIO4_9 = GPIO105,  /**< GPIO4,bit9 */
    GPIO106 = 106,      /**< GPIO4,bit10 */
    GPIO4_10 = GPIO106, /**< GPIO4,bit10 */
    GPIO107 = 107,      /**< GPIO4,bit11 */
    GPIO4_11 = GPIO107, /**< GPIO4,bit11 */
    GPIO108 = 108,      /**< GPIO4,bit12 */
    GPIO4_12 = GPIO108, /**< GPIO4,bit12 */
    GPIO109 = 109,      /**< GPIO4,bit13 */
    GPIO4_13 = GPIO109, /**< GPIO4,bit13 */
    GPIO110 = 110,      /**< GPIO4,bit14 */
    GPIO4_14 = GPIO110, /**< GPIO4,bit14 */
    GPIO111 = 111,      /**< GPIO4,bit15 */
    GPIO4_15 = GPIO111, /**< GPIO4,bit15 */
    GPIO112 = 112,      /**< GPIO4,bit16 */
    GPIO4_16 = GPIO112, /**< GPIO4,bit16 */
    GPIO113 = 113,      /**< GPIO4,bit17 */
    GPIO4_17 = GPIO113, /**< GPIO4,bit17 */
    GPIO114 = 114,      /**< GPIO4,bit18 */
    GPIO4_18 = GPIO114, /**< GPIO4,bit18 */
    GPIO115 = 115,      /**< GPIO4,bit19 */
    GPIO4_19 = GPIO115, /**< GPIO4,bit19 */
    GPIO116 = 116,      /**< GPIO4,bit20 */
    GPIO4_20 = GPIO116, /**< GPIO4,bit20 */
    GPIO117 = 117,      /**< GPIO4,bit21 */
    GPIO4_21 = GPIO117, /**< GPIO4,bit21 */
    GPIO118 = 118,      /**< GPIO4,bit22 */
    GPIO4_22 = GPIO118, /**< GPIO4,bit22 */
    GPIO119 = 119,      /**< GPIO4,bit23 */
    GPIO4_23 = GPIO119, /**< GPIO4,bit23 */
    GPIO120 = 120,      /**< GPIO4,bit24 */
    GPIO4_24 = GPIO120, /**< GPIO4,bit24 */
    GPIO121 = 121,      /**< GPIO4,bit25 */
    GPIO4_25 = GPIO121, /**< GPIO4,bit25 */
    GPIO122 = 122,      /**< GPIO4,bit26 */
    GPIO4_26 = GPIO122, /**< GPIO4,bit26 */
    GPIO123 = 123,      /**< GPIO4,bit27 */
    GPIO4_27 = GPIO123, /**< GPIO4,bit27 */
    GPIO124 = 124,      /**< GPIO4,bit28 */
    GPIO4_28 = GPIO124, /**< GPIO4,bit28 */
    GPIO125 = 125,      /**< GPIO4,bit29 */
    GPIO4_29 = GPIO125, /**< GPIO4,bit29 */
    GPIO126 = 126,      /**< GPIO4,bit30 */
    GPIO4_30 = GPIO126, /**< GPIO4,bit30 */
    GPIO127 = 127,      /**< GPIO4,bit31 */
    GPIO4_31 = GPIO127, /**< GPIO4,bit31 */
    GPIO128 = 128,      /**< GPIO5,bit0 */
    GPIO5_0 = GPIO128,  /**< GPIO5,bit0 */
    GPIO129 = 129,      /**< GPIO5,bit1 */
    GPIO5_1 = GPIO129,  /**< GPIO5,bit1 */
    GPIO130 = 130,      /**< GPIO5,bit2 */
    GPIO5_2 = GPIO130,  /**< GPIO5,bit2 */
    GPIO131 = 131,      /**< GPIO5,bit3 */
    GPIO5_3 = GPIO131,  /**< GPIO5,bit3 */
    GPIO132 = 132,      /**< GPIO5,bit4 */
    GPIO5_4 = GPIO132,  /**< GPIO5,bit4 */
    GPIO133 = 133,      /**< GPIO5,bit5 */
    GPIO5_5 = GPIO133,  /**< GPIO5,bit5 */
    GPIO134 = 134,      /**< GPIO5,bit6 */
    GPIO5_6 = GPIO134,  /**< GPIO5,bit6 */
    GPIO135 = 135,      /**< GPIO5,bit7 */
    GPIO5_7 = GPIO135,  /**< GPIO5,bit7 */
    GPIO136 = 136,      /**< GPIO5,bit8 */
    GPIO5_8 = GPIO136,  /**< GPIO5,bit8 */
    GPIO137 = 137,      /**< GPIO5,bit9 */
    GPIO5_9 = GPIO137,  /**< GPIO5,bit9 */
    GPIO138 = 138,      /**< GPIO5,bit10 */
    GPIO5_10 = GPIO138, /**< GPIO5,bit10 */
    GPIO139 = 139,      /**< GPIO5,bit11 */
    GPIO5_11 = GPIO139, /**< GPIO5,bit11 */
    GPIO140 = 140,      /**< GPIO5,bit12 */
    GPIO5_12 = GPIO140, /**< GPIO5,bit12 */
    GPIO141 = 141,      /**< GPIO5,bit13 */
    GPIO5_13 = GPIO141, /**< GPIO5,bit13 */
    GPIO142 = 142,      /**< GPIO5,bit14 */
    GPIO5_14 = GPIO142, /**< GPIO5,bit14 */
    GPIO143 = 143,      /**< GPIO5,bit15 */
    GPIO5_15 = GPIO143, /**< GPIO5,bit15 */
    GPIO144 = 144,      /**< GPIO5,bit16 */
    GPIO5_16 = GPIO144, /**< GPIO5,bit16 */
    GPIO145 = 145,      /**< GPIO5,bit17 */
    GPIO5_17 = GPIO145, /**< GPIO5,bit17 */
    GPIO146 = 146,      /**< GPIO5,bit18 */
    GPIO5_18 = GPIO146, /**< GPIO5,bit18 */
    GPIO147 = 147,      /**< GPIO5,bit19 */
    GPIO5_19 = GPIO147, /**< GPIO5,bit19 */
    GPIO148 = 148,      /**< GPIO5,bit20 */
    GPIO5_20 = GPIO148, /**< GPIO5,bit20 */
    GPIO149 = 149,      /**< GPIO5,bit21 */
    GPIO5_21 = GPIO149, /**< GPIO5,bit21 */
    GPIO150 = 150,      /**< GPIO5,bit22 */
    GPIO5_22 = GPIO150, /**< GPIO5,bit22 */
    GPIO151 = 151,      /**< GPIO5,bit23 */
    GPIO5_23 = GPIO151, /**< GPIO5,bit23 */
    GPIO152 = 152,      /**< GPIO5,bit24 */
    GPIO5_24 = GPIO152, /**< GPIO5,bit24 */
    GPIO153 = 153,      /**< GPIO5,bit25 */
    GPIO5_25 = GPIO153, /**< GPIO5,bit25 */
    GPIO154 = 154,      /**< GPIO5,bit26 */
    GPIO5_26 = GPIO154, /**< GPIO5,bit26 */
    GPIO155 = 155,      /**< GPIO5,bit27 */
    GPIO5_27 = GPIO155, /**< GPIO5,bit27 */
    GPIO156 = 156,      /**< GPIO5,bit28 */
    GPIO5_28 = GPIO156, /**< GPIO5,bit28 */
    GPIO157 = 157,      /**< GPIO5,bit29 */
    GPIO5_29 = GPIO157, /**< GPIO5,bit29 */
    GPIO158 = 158,      /**< GPIO5,bit30 */
    GPIO5_30 = GPIO158, /**< GPIO5,bit30 */
    GPIO159 = 159,      /**< GPIO5,bit31 */
    GPIO5_31 = GPIO159, /**< GPIO5,bit31 */
    GPIO_MAX = GPIO159, /**< \brief GPIO引脚最大编号 */
}imx6ul_gpio_pin_t;

/** @} end of grp_awbl_drv_imx6ul_gpio_pin_defs */
/** @} end of grp_awbl_drv_imx6ul_gpio */

#ifdef __cplusplus
}
#endif	/* __cplusplus 	*/

#endif	/* __IMX6UL_CLOCK_CFG_H */

/* end of file */
