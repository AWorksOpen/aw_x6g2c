#ifndef             __IMX6UL_EIM_REGS_H__
#define             __IMX6UL_EIM_REGS_H__
#include "imx6ul_iomuxc_regs.h"
/*******************************************************************************
    register define
*******************************************************************************/

/**
 * \brief iomuxc_gpr1 寄存器
 * @{
 */
#define __EIM_CS0_EN     (0x1u)
#define __EIM_CS1_EN     (0x1u<<3)
#define __EIM_CS2_EN     (0x1u<<6)
#define __EIM_CS3_EN     (0x1u<<9)
#define __EIM_CS_EN      __EIM_CS0_EN | __EIM_CS1_EN | \
                         __EIM_CS2_EN | __EIM_CS3_EN

#define __EIM_CS0_SPACE  (0x0u<<1)
#define __EIM_CS1_SPACE  (0x0u<<4)
#define __EIM_CS2_SPACE  (0x0u<<7)
#define __EIM_CS3_SPACE  (0x0u<<10)
#define __EIM_CS_SPACE   __EIM_CS0_SPACE | __EIM_CS1_SPACE | \
                         __EIM_CS2_SPACE | __EIM_CS3_SPACE
/** @} */


struct imx6ul_eim_config {
    volatile uint32_t GCR1;
#define __EIM_CSn_EN  (0x1u)            /* 使能  */
#define __EIM_SWR_EN  (0x1u << 1)       /* 写同步使能  */
#define __EIM_SRD_EN  (0x1u << 2)       /* 读同步使能  */
#define __EIM_MUM_EN  (0x1u << 3)       /* 复用模式  */
#define __EIM_WFL_EN  (0x1u << 4)
#define __EIM_RFL_EN  (0x1u << 5)
#define __EIM_CRE_EN  (0x1u << 6)       /* CRE信号使能*/
#define __EIM_CREP    (0x1u << 7)       /* CRE高电平有效 */
#define __EIM_BL      (0x7u << 8)
#define __EIM_WC_EN   (0x1u << 11)      /* 突发连续写使能 */
#define __EIM_BCD     (0x3u << 12)      /* 突发时钟分频，1,2,4分频  */
#define __EIM_BCS     (0x3u << 14)      /* 时钟延时，访问(操作)前延时0-3个时钟周期  */
#define __EIM_DSZ     (0x3u << 16)      /* 字数据大小  */
#define __EIM_SP      (0x1u << 19)
#define __EIM_CSREC   (0x3u << 20)      /* CS恢复时间，1次操作的间隔(1次操作包括写地址，读写数据)  */
#define __EIM_AUS     (0x1u << 23)
#define __EIM_GBC     (0x7u << 24)      /* CS片切换时间  */
#define __EIM_WP      (0x1u << 27)
#define __EIM_PZS     (0xfu << 28)

#define __EIM_SET_CS(n)     (n & __EIM_CSn_EN)
#define __EIM_SET_SWR(n)    ((n << 1) & __EIM_SWR_EN)
#define __EIM_SET_SRD(n)    ((n << 2) & __EIM_SRD_EN)
#define __EIM_SET_MUM(n)    ((n << 3) & __EIM_MUM_EN)
#define __EIM_SET_WFL(n)    ((n << 4) & __EIM_WFL_EN)
#define __EIM_SET_RFL(n)    ((n << 5) & __EIM_RFL_EN)
#define __EIM_SET_CRE(n)    ((n << 6) & __EIM_CRE_EN)
#define __EIM_SET_CREP(n)   ((n << 7) & __EIM_CREP)
#define __EIM_SET_BL(n)     ((n << 8) & __EIM_BL)
#define __EIM_SET_WC(n)     ((n << 11) & __EIM_WC_EN)
#define __EIM_SET_BCD(n)    ((n << 12) & __EIM_BCD)
#define __EIM_SET_BCS(n)    ((n << 14) & __EIM_BCS)
#define __EIM_SET_DSZ(n)    ((n << 16) & __EIM_DSZ)
#define __EIM_SET_SP(n)     ((n << 19) & __EIM_SP)
#define __EIM_SET_CSREC(n)  ((n << 20) & __EIM_CSREC)
#define __EIM_SET_AUS(n)    ((n << 23) & __EIM_AUS)
#define __EIM_SET_GBC(n)    ((n << 24) & __EIM_GBC)
#define __EIM_SET_WP(n)     ((n << 27) & __EIM_WP)
#define __EIM_SET_PZS(n)    ((n << 28) & __EIM_PZS)

    volatile uint32_t GCR2;
#define __EIM_ADH               (0x3u)      /* ALE置低后，地址保持时间  */
#define __EIM_DAPS              (0xfu << 4)
#define __EIM_DAE               (0x1u << 8)
#define __EIM_DAP               (0x1u << 9)
#define __EIM_MUX16_BYP_GRANT   (0x1u << 12)

#define __EIM_SET_ADH(n)    (n & __EIM_ADH)
#define __EIM_SET_DAPS(n)   ((n << 4) & __EIM_DAPS)
#define __EIM_SET_DAE(n)    ((n << 8) & __EIM_DAE)
#define __EIM_SET_DAP(n)    ((n << 9) & __EIM_DAP)
#define __EIM_SET_BYP(n)    ((n << 12) & __EIM_MUX16_BYP_GRANT)
};

struct imx6ul_eim_timing {
    volatile uint32_t RCR1;
#define __EIM_RCSN    (0x7u)            //CS拉高后的持续周期,同步模式忽略
#define __EIM_RCSA    (0x7u << 4)       //CS拉低前的持续周期
#define __EIM_OEN     (0x7u << 8)       //OE拉高后的持续周期,同步模式忽略
#define __EIM_OEA     (0x7u << 12)      //OE拉低前的持续周期
#define __EIM_RADVN   (0x7u << 16)      //ADV信号低电平保持周期-1
#define __EIM_RAL     (0x1u << 19)      //置0时，ADV低电平时间(RADVN)使能
#define __EIM_RADVA   (0x7u << 20)      //ADV拉低前的周期
#define __EIM_RWSC    (0x3fu << 24)

#define __EIM_SET_RCSN(n)   (n & __EIM_RCSN)
#define __EIM_SET_RCSA(n)   ((n << 4) & __EIM_RCSA)
#define __EIM_SET_OEN(n)    ((n << 8) & __EIM_OEN)
#define __EIM_SET_OEA(n)    ((n << 12) & __EIM_OEA)
#define __EIM_SET_RADVN(n)  ((n << 16) & __EIM_RADVN)
#define __EIM_SET_RAL(n)    ((n << 19) & __EIM_RAL)
#define __EIM_SET_RADVA(n)  ((n << 20) & __EIM_RADVA)
#define __EIM_SET_RWSC(n)   ((n << 24) & __EIM_RWSC)

    volatile uint32_t RCR2;
#define __EIM_RBEN    (0x7u)
#define __EIM_RBE_EN  (0x1u << 3)
#define __EIM_RBEA    (0x7u << 4)
#define __EIM_RL      (0x3u << 8)
#define __EIM_PAT     (0x7u << 12)
#define __EIM_APR     (0x1u << 15)

#define __EIM_SET_RBEN(n)   (n & __EIM_RBEN)
#define __EIM_SET_RBE_EN(n) ((n << 3) & __EIM_RBE_EN)
#define __EIM_SET_RBEA(n)   ((n << 4) & __EIM_RBEA)
#define __EIM_SET_RL(n)     ((n << 8) & __EIM_RL)
#define __EIM_SET_PAT(n)    ((n << 12) & __EIM_PAT)
#define __EIM_SET_APR(n)    ((n << 15) & __EIM_APR)

    volatile uint32_t WCR1;
#define __EIM_WCSN    (0x7u)            //CS拉高后的持续周期,同步模式忽略
#define __EIM_WCSA    (0x7u << 3)       //CS拉低前的持续周期
#define __EIM_WEN     (0x7u << 6)       //WE拉高后的持续周期,同步模式忽略
#define __EIM_WEA     (0x7u << 9)       //WE拉低前的持续周期
#define __EIM_WBEN    (0x7u << 12)
#define __EIM_WBEA    (0x7u << 15)
#define __EIM_WADVN   (0x7u << 18)      //ADV信号低电平保持周期-1，WAL==0时有效
#define __EIM_WADVA   (0x7u << 21)      //ADV拉低前的周期
#define __EIM_WWSC    (0x3fu << 24)
#define __EIM_WBED    (0x1u << 30)
#define __EIM_WAL     (0x1u << 31)      //置0时，ADV低电平周期设置(WADVN)使能

#define __EIM_SET_WCSN(n)   (n & __EIM_WCSN)
#define __EIM_SET_WCSA(n)   ((n << 3) & __EIM_WCSA)
#define __EIM_SET_WEN(n)    ((n << 6) & __EIM_WEN)
#define __EIM_SET_WEA(n)    ((n << 9) & __EIM_WEA)
#define __EIM_SET_WBEN(n)   ((n << 12) & __EIM_WBEN)
#define __EIM_SET_WBEA(n)   ((n << 15) & __EIM_WBEA)
#define __EIM_SET_WADVN(n)  ((n << 18) & __EIM_WADVN)
#define __EIM_SET_WADVA(n)  ((n << 21) & __EIM_WADVA)
#define __EIM_SET_WWSC(n)   ((n << 24) & __EIM_WWSC)
#define __EIM_SET_WBED(n)   ((n << 30) & __EIM_WBED)
#define __EIM_SET_WAL(n)    ((n << 31) & __EIM_WAL)

    volatile uint32_t WCR2;
#define __EIM_WBCDD   (0x1u)

#define __EIM_SET_WBCDD(n)  (n & __EIM_WBCDD)
};

struct imx6ul_eim_reg {
    struct imx6ul_eim_config cfg;
    struct imx6ul_eim_timing timing;
};


struct imx6ul_eim_regs {
    struct imx6ul_eim_reg cs0;
    struct imx6ul_eim_reg cs1;
    struct imx6ul_eim_reg cs2;
    struct imx6ul_eim_reg cs3;
    volatile uint32_t     WCR;
};

struct eim_general_config {
    /* GCR1 */
    uint8_t CSEN;
    uint8_t SWR;
    uint8_t SRD;
    uint8_t MUM;
    uint8_t WFL;
    uint8_t RFL;
    uint8_t CRE;
    uint8_t CREP;
    uint8_t BL;
    uint8_t WC;
    uint8_t BCD;
    uint8_t BCS;
    uint8_t DSZ;
    uint8_t SP;
    uint8_t CSREC;
    uint8_t AUS;
    uint8_t GBC;
    uint8_t WP;
    uint8_t PZS;

    /* GCR2 */
    uint8_t ADH;
    uint8_t DAPS;
    uint8_t DAE;
    uint8_t DAP;
    uint8_t MUX16_BYP_GRANT;
};

struct eim_read_config {
    /* RCR1 */
    uint8_t RCSN;
    uint8_t RCSA;
    uint8_t OEN;
    uint8_t OEA;
    uint8_t RADVN;
    uint8_t RAL;
    uint8_t RADVA;
    uint8_t RWSC;

    /* RCR2 */
    uint8_t RBEN;
    uint8_t RBE_EN;
    uint8_t RBEA;
    uint8_t RL;
    uint8_t PAT;
    uint8_t APR;
};

struct eim_write_config {
    /* WCR1 */
    uint8_t WCSN;
    uint8_t WCSA;
    uint8_t WEN;
    uint8_t WEA;
    uint8_t WBEN;
    uint8_t WBEA;
    uint8_t WADVN;
    uint8_t WADVA;
    uint8_t WWSC;
    uint8_t WBED;
    uint8_t WAL;

    /* WCR2 */
    uint8_t WBCDD;
};

/** \brief  Eim寄存器配置 */
struct awbl_eim_config {
    struct eim_general_config gcr;
    struct eim_read_config    rcr;
    struct eim_write_config   wcr;
};

#endif
