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
#ifndef __AWBL_USBH_EHCI_DEF_H
#define __AWBL_USBH_EHCI_DEF_H

/* 寄存器*/
#define __CAP_REG_LENGTH    (0x00000000)
#define __CAP_REG_HCS       (0x00000004)
#define __CAP_REG_HCC       (0x00000008)
#define __CAP_REG_HCSP      (0x0000000C)

#define __OPT_REG_CMD       (0x00000000)
#define __OPT_REG_STS       (0x00000004)
#define __OPT_REG_INTR      (0x00000008)
#define __OPT_REG_FDIX      (0x0000000C)
#define __OPT_REG_CTRL      (0x00000010)
#define __OPT_REG_PERIOD    (0x00000014)
#define __OPT_REG_ASYNC     (0x00000018)
#define __OPT_REG_TTSTS     (0x0000001C)
#define __OPT_REG_CFG       (0x00000040)
#define __OPT_REG_PSC0      (0x00000044)
#define __OPT_REG_PSC1      (0x00000048)
#define __OPT_REG_PSC2      (0x0000004C)
#define __OPT_REG_PSC3      (0x00000050)
#define __OPT_REG_PSC4      (0x00000054)
#define __OPT_REG_PSC5      (0x00000058)
#define __OPT_REG_PSC6      (0x0000005C)
#define __OPT_REG_PSC7      (0x00000060)
#define __OPT_REG_PSC8      (0x00000064)
#define __OPT_REG_MODE      (0x00000068)

/* 寄存器位*/
//#define __CAP_HCS_PI(p)           ((p) & (1 << 16))
//#define __CAP_HCS_PPC(p)          ((p) & (1 << 4))      /* true: port power control */
#define __CAP_HCS_N_PORTS(p)      (((p) >> 0) & 0xf)      /* 位3:0, 主机控制器的端口数量*/

//#define __CAP_HCC_32FRAME_PERIODIC_LIST(p)    ((p) & (1 << 19))
#define __CAP_HCC_PER_PORT_CHANGE_EVENT(p)    ((p) & (1 << 18))
//#define __CAP_HCC_LPM(p)                      ((p) & (1 << 17))
//#define __CAP_HCC_HW_PREFETCH(p)              ((p) & (1 << 16))
//#define __CAP_HCC_EXT_CAPS(p)                 (((p) >> 8) & 0xff) /* for pci extended caps */
//#define __CAP_HCC_ISOC_CACHE(p)               ((p) & (1 << 7))    /* TRUE: 可以缓存等时帧*/
//#define __CAP_HCC_ISOC_THRES(p)               (((p) >> 4) & 0x7)  /* 位 6:4, 微帧缓存*/
//#define __CAP_HCC_CANPARK(p)                  ((p) & (1 << 2))    /* true: can park on async qh */
#define __CAP_HCC_PGM_FRAME_LIST_FLAG(p)      ((p) & (1 << 1))      /* TRUE: 周期帧链表大小可变*/
#define __CAP_HCC_64BIT_ADDR(p)               ((p) & (1))           /* TRUE: 可以使用64位地址*/



#define __REG_CMD_PPCEE       (1 << 15)             /* 每一个端口变化事件使能*/
#define __REG_CMD_PARK        (1 << 11)             /* enable "park" on async qh */
#define __REG_CMD_PARK_CNT(c) (((c) >> 8) & 3)      /* how many transfers to park for */
#define __REG_CMD_LRESET      (1 << 7)              /* partial reset (no ports, etc) */
#define __REG_CMD_IAAD        (1 << 6)              /* "doorbell" interrupt async advance */
#define __REG_CMD_ASE         (1 << 5)              /* 使能异步调度*/
#define __REG_CMD_PSE         (1 << 4)              /* 使能周期调度*/
//#define __REG_CMD_FLS_1024    (0x00000000)          /* 000 */
//#define __REG_CMD_FLS_512     (0x00000004)          /* 001 */
//#define __REG_CMD_FLS_256     (0x00000008)          /* 010 */
//#define __REG_CMD_FLS_128     (0x0000000C)          /* 011 */
//#define __REG_CMD_FLS_64      (0x00008000)          /* 100 */
//#define __REG_CMD_FLS_32      (0x00008004)          /* 101 */
//#define __REG_CMD_FLS_16      (0x00008008)          /* 110 */
//#define __REG_CMD_FLS_8       (0x0000800C)          /* 111 */
#define __REG_CMD_RESET       (1 << 1)              /* 复位主机控制器(非总线) */
#define __REG_CMD_RUN         (1 << 0)              /* 启动/停止主机控制器*/

#define __REG_STS_ASS     (1 << 15)                 /* 异步调度状态*/
#define __REG_STS_PSS     (1 << 14)                 /* 周期调度状态*/
#define __REG_STS_RECL    (1 << 13)                 /* Reclamation */
#define __REG_STS_HALT    (1 << 12)                 /* Not running (any reason) */
#define __REG_STS_IAA     (1 << 5)                  /* 异步推进中断*/
#define __REG_STS_FATAL   (1 << 4)                  /* such as some PCI access errors */
#define __REG_STS_FLR     (1 << 3)                  /* frame list rolled over */
#define __REG_STS_PCD     (1 << 2)                  /* 端口变化检测*/
#define __REG_STS_ERR     (1 << 1)                  /* "error" completion (overflow, ...) */
#define __REG_STS_INT     (1 << 0)                  /* "normal" completion (short, ...) */

#define __REG_MODE_SDIS    (1 << 3)      /* 流禁能*/
#define __REG_MODE_BE      (1 << 2)      /* 大/小端选择*/
#define __REG_MODE_CM_HC   (3 << 0)      /* 主机控制器模式*/
#define __REG_MODE_CM_IDLE (0 << 0)      /* 空闲状态*/

/* 数据结构类型定义*/
#define __Q_TYPE_ITD  (0 << 1)
#define __Q_TYPE_QH   (1 << 1)
#define __Q_TYPE_SITD (2 << 1)
#define __Q_TYPE_FSTN (3 << 1)


#define __QH_CONTROL_EP   (1 << 27)   /* 控制端点*/
#define __QH_HEAD         (1 << 15)   /* Head of async reclamation list */
#define __QH_TOGGLE_CTL   (1 << 14)   /* 数据切换控制*/
#define __QH_HIGH_SPEED   (2 << 12)   /* 高速端点*/
#define __QH_LOW_SPEED    (1 << 12)
//#define __QH_FULL_SPEED   (0 << 12)
//#define __QH_INACTIVATE   (1 << 7)  /* Inactivate on next transaction */

#define __QH_SMASK    0x000000ff
#define __QH_CMASK    0x0000ff00
#define __QH_HUBADDR  0x007f0000
#define __QH_HUBPORT  0x3f800000
#define __QH_MULT     0xc0000000


#define __QTD_PID_OUT     0
#define __QTD_PID_IN      1
#define __QTD_PID_SETUP   2


#define __QTD_TOGGLE      (1u << 31)               /* 数据切换*/
#define __QTD_LENGTH(tok) (((tok)>>16) & 0x7fffu)
#define __QTD_IOC         (1u << 15)               /* 完成中断*/
#define __QTD_CERR(tok)   (((tok)>>10) & 0x3u)
#define __QTD_PID(tok)    (((tok)>>8) & 0x3u)
#define __QTD_STS_ACTIVE  (1u << 7)   /* HC may execute this */
#define __QTD_STS_HALT    (1u << 6)                /* 错误停止*/
#define __QTD_STS_DBE     (1u << 5)                /* 数据缓存错误 (主机控制器) */
#define __QTD_STS_BABBLE  (1u << 4)   /* device was babbling (qtd halted) */
#define __QTD_STS_XACT    (1u << 3)                /* 设备返回非法响应 */
#define __QTD_STS_MMF     (1u << 2)   /* incomplete split transaction */
#define __QTD_STS_STS     (1u << 1)                /* 分割事务状态 */
#define __QTD_STS_PING    (1u << 0)   /* issue PING? */


#define __QH_ST_IDLE              0
#define __QH_ST_LINKED            1
#define __QH_ST_UNLINKED          2


#define __EHCI_LOCK_TIMEOUT  2000

#endif /* __USBH_EHCI_DEF_H */
