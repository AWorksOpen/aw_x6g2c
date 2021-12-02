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

//#define USBH_EHCI_FRAME_LIST_SIZE  (1024)  /* 帧列表大小：1024(4K), 512(2K), 256(1K) */
#define USBH_EHCI_TASK_STACK_SIZE  (8192)  /* EHCI任务棧大小*/
#define USBH_EHCI_FRAME_LIST_SIZE  (256)  /* 帧列表大小：1024(4K), 512(2K), 256(1K) */
//#define USBH_EHCI_TASK_STACK_SIZE  (4096)  /* EHCI任务棧大小*/
/* 高速中断传输不得占用超过80%的微帧时间，全速和低速不得超过90%帧时间*/
/* 全速同步传输不得占用超过90%的帧时间，高速同步传输不得占用超过90%的微帧时间*/
#define USBH_EHCI_UFRAME_BANDWIDTH (100)   /* 一个微帧的带宽，125*80% us */
#define USBH_EHCI_BANDWIDTH_SIZE   (80)    /* 存储微帧带宽的数量，同时也是最小的周期 */
#define USBH_EHCI_BANDWIDTH_FRAMES (10)    /* 带宽帧数量，带宽微帧数量除以8*/
#define USBH_EHCI_TUNE_RL_HS       4       /* nak throttle; see 4.9 */
#define USBH_EHCI_TUNE_RL_TT       0
#define USBH_EHCI_TUNE_MULT_HS     1       /* 1-3事务每微帧 */
#define USBH_EHCI_TUNE_MULT_TT     1       /* 端点每一微帧产生一个事务*/

/* 全/低速带宽分配常数/支持*/
#define BW_HOST_DELAY   1000L       /* 纳秒*/
#define BW_HUB_LS_SETUP 333L        /* 纳秒*/

/* 类型标记来自 {qh,itd,sitd,fstn}->hw_next */
#define Q_NEXT_TYPE(dma)   ((dma) & USB_CPU_TO_LE32(3 << 1))

#define USBH_GET_EHCI_FROM_HC(hc) ((struct usbh_ehci *)(hc->controller))

union __ehci_ptr;
/** EHCI控制器结构体*/
struct usbh_ehci {
    uint8_t                    ctrllor_type;          /* 控制器类型*/
    struct usb_ctrllor_drv    *p_drv;                 /* EHCI 驱动函数集*/
    struct usbh               *p_hc;                  /* USB主机结构体*/
    usb_bool_t                 is_init;               /* 是否初始化标志*/

    struct usb_pool            qhs;                   /* QH(队列头)内存池*/
    struct usb_pool            qtds;                  /* qtd(队列传输描述符)内存池*/
    struct usb_pool            itds;                  /* itd(同步传输描述符)内存池*/
    struct usb_pool            sitds;                 /* sitd(分割事务同步描述符)内存池*/

    uint8_t                    evt;                   /* EHCI事件*/

    uint32_t                   i_thresh;
    uint32_t                   isoc_count;            /* 等时调度激活的次数*/
    int                        bandwidth_allocated;
    int                        next_uframe;           /* 下一微帧索引*/
    uint32_t                   now_frame;             /* 目前的帧索引*/
    uint32_t                   last_iso_frame;        /* 周期扫描的最新帧索引*/

    usb_sem_handle_t           sem;                   /* EHCI信号量*/
    usb_mutex_handle_t         lock;                  /* EHCI互斥锁*/
    usb_task_handle_t          task;                  /* EHCI任务*/

    uint32_t                   capregs;               /* USB性能寄存器*/
    uint32_t                   optregs;               /* EHCI操作寄存器*/

    uint32_t                   hcs;                   /* 主机控制器结构体参数*/
    usb_bool_t                 ppcd;                  /* 每一个端口变化检测*/
    uint8_t                    ps_bit;                /* 端口速度位*/
    uint8_t                    nports;                /* 端口数量*/
    uint32_t                   status;                /* EHCI状态*/

    struct usbh_ehci_qh       *async;                 /* 异步调度*/
    uint32_t                  *periodic;              /* 周期调度*/
    union __ehci_ptr          *pshadow;               /* 硬件周期表镜像，用来存放周期帧列表的数据结构*/
    uint32_t                   fls;                   /* 帧列表大小*/
    uint8_t                    bdwidth[USBH_EHCI_BANDWIDTH_SIZE];   /* 总线带宽*/
    int                        random;

    struct usb_list_head       intr_qhs;              /* 中断QH(队列头)*/

    /* 根集线器，第31位是复位变化状态位，第30位是挂起变化状态位，29~0位和USB端口状态控制寄存器值一样*/
    uint32_t                   psts[15];              /* 端口状态*/
    /* 根集线器变化回调函数*/
    void (*pfn_rh_change) (void *p_ehci, uint16_t change);
};

/** EHCI qTD(队列传输描述符) 结构体*/
struct usbh_ehci_qtd {
    /* EHCI qTD数据结构规范*/
    uint32_t                hw_next;       /* 指向下一个qTD的指针*/
    uint32_t                hw_alt_next;   /*  Aletmate Next qTD Pointer*/
    uint32_t                hw_token;      /* 令牌*/
    uint32_t                hw_buf[5];     /* qTD缓冲区页指针*/
    uint32_t                hw_buf_hi[5];

    /* USB主机控制器驱动所需*/
    struct usb_list_head    node;          /* 当前qTD节点*/
    struct usbh_trp        *p_trp;         /* 传输请求包 */
    size_t                  len;           /* 当前qTD数据长度*/
};

/* EHCI类型指针联合体*/
union __ehci_ptr {
    struct usbh_ehci_qh    *p_qh;        /* QH(队列头)结构，Q_TYPE_QH */
    struct usbh_ehci_itd   *p_itd;       /* 等时传输描述符结构，Q_TYPE_ITD */
    struct usbh_ehci_sitd  *p_sitd;      /* 分割等时传输描述符结构， Q_TYPE_SITD */
    struct usbh_ehci_fstn  *p_fstn;      /* FSTN结构，Q_TYPE_FSTN */
    uint32_t               *hw_next;     /* 所有类型适用*/
    void                   *ptr;
};

/**
 * 队列头结构体
 *
 * hw_info1[31:28] 重载Nak计数器字段
 * hw_info1[27]    控制端点标志(非高速控制端点设置为1，否则为0)
 * hw_info1[26:16] 最大包长度
 * hw_info1[15]
 * hw_info1[14]    数据翻转控制
 * hw_info1[13:12] 端点速度
 * hw_info1[11: 8] 端点号
 * hw_info1[7]     请求主机控制器取消激活位(只用于周期调度的低/全速设备)
 * hw_info1[ 6: 0] 设备地址
 *
 * hw_info2[31:30] 高带宽管道乘法器
 *                 01b 端点每一微帧产生一个事务
 *                 10b 端点每一微帧产生两个事务
 *                 11b 端点每一微帧产生三个事务
 * hw_info2[29:23] 端口号(用于低/全速设备的分割事务)
 * hw_info2[22:16] 集线器地址(用于低/全速设备的分割事务)
 * hw_info2[15: 8] 分割完成掩码(uFrame C-mask，只用于周期调度的低/全速设备)
 *                 当FRIMDEX寄存器位解码到一个uFrame C-mask为1的位置
 *                 则这个队列头是事务执行的候选
 * hw_info2[ 7: 0] 中断调度掩码(uFrame S-mask，如果是异步调度则为0，中断周期调度为非0)
 *                 当FRIMDEX寄存器位解码到一个uFrame S-mask为1的位置
 *                 则这个队列头是事务执行的候选
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
    uint8_t                     usecs;              /* 中断端点带宽*/
    uint8_t                     c_usecs;            /* 分割完成带宽*/
    struct usb_list_head        intr_node;
    struct usbh_endpoint       *p_ep;

};

/** EHCI等时传输描述符结构体*/
struct usbh_ehci_itd {
    uint32_t                 hw_next;
    uint32_t                 hw_transaction[8];           /* 等时传输描述符事务状态和控制列表(EHCI 3.3.2)*/
    uint32_t                 hw_bufp[7];                  /* 等时传输描述符缓存页指针链表(EHCI 3.3.3)*/
    uint32_t                 hw_bufp_hi[7];               /* 64位版本的数据结构(附录 B)*/

#define EHCI_ISOC_ACTIVE        (1<<31)                   /* 这个插槽激活传输中 */
#define EHCI_ISOC_BUF_ERR       (1<<30)                   /* 数据缓存错误*/
#define EHCI_ISOC_BABBLE        (1<<29)                   /* babble 检测*/
#define EHCI_ISOC_XACTERR       (1<<28)                   /* XactErr - 传输错误*/
#define EHCI_ITD_LENGTH(tok)    (((tok)>>16) & 0x0fff)
#define EHCI_ITD_IOC            (1 << 15)                 /* 完成中断*/

#define ITD_ACTIVE    USB_CPU_TO_LE32(EHCI_ISOC_ACTIVE)
    union __ehci_ptr         p_next;                      /* ptr to periodic q entry */
    uint32_t                 frame;                       /* 从哪里调度*/
    struct usb_list_head     itd_list;                    /* 数据流的等时传输描述符链表*/
    struct usbh_trp         *trp;                         /* USB传输请求包*/
    struct ehci_iso_stream  *stream;                      /* 端点队列*/
    uint32_t                 index[8];                    /* 在 trp->iso_frame_descde的索引*/
    uint32_t                 pg;                          /* 页选择*/
    uint8_t                  usecs[8];
};

/**
 * EHCI分割等时传输描述符结构体
 *
 * hw_fullspeed_ep[22:16] 事务传输集线器的地址
 * hw_fullspeed_ep[11: 8] 端点地址
 * hw_fullspeed_ep[ 6: 0] 设备地址
 *
 * hw_uframe[15: 8] 分割完成掩码(uFrame C-Mask)
 *                  当FRIMDEX寄存器位解码到一个uFrame C-mask为1的位置
 *                  则这个分割等时传输描述符是事务执行的候选
 * hw_uframe[ 7: 0] 分割起始掩码(uFrame S-Mask)
 *                  当FRIMDEX寄存器位解码到一个uFrame S-mask为1的位置
 *                  则这个分割等时传输描述符是事务执行的候选
 *
 * hw_results[31]    完成中断
 * hw_results[30]    数据缓存页指针选择
 * hw_results[25:16] 要传输的全部字节
 * hw_results[15: 8] 记录被执行的分割完成事务
 * hw_results[ 7: 0] 记录被执行的事务的状态
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
#define SITD_IOC        (1 << 31)             /* 完成中断*/
#define SITD_PAGE       (1 << 30)             /* 缓存 0/1 */
#define SITD_LENGTH(x)  (0x3ff & ((x)>>16))
#define SITD_STS_ACTIVE (1 << 7)              /* 激活*/
#define SITD_STS_ERR    (1 << 6)              /* 事物转换器错误*/
#define SITD_STS_DBE    (1 << 5)              /* 数据缓存错误(主机)*/
#define SITD_STS_BABBLE (1 << 4)              /* device was babbling */
#define SITD_STS_XACT   (1 << 3)              /* 非法的输入相应*/
#define SITD_STS_MMF    (1 << 2)              /* 未完成分割事务*/
#define SITD_STS_STS    (1 << 1)              /* 分割事务状态*/

#define SITD_ACTIVE   USB_CPU_TO_LE32(SITD_STS_ACTIVE)
    uint32_t                  hw_buf [2];
    uint32_t                  hw_backpointer;
    uint32_t                  hw_buf_hi [2];

    /* the rest is HCD-private */
    union __ehci_ptr          p_next;         /* ptr to periodic q entry */
    struct usbh_trp          *p_trp;
    struct ehci_iso_stream   *stream;         /* 端点队列*/
    struct usb_list_head      sitd_list;      /* 数据流的分割等时传输描述符链表*/
    uint32_t                  frame;          /* 调度位置*/
    uint32_t                  index;
};

/** \brief EHCI FSTN */
struct usbh_ehci_fstn {
    uint32_t            hw_next;
    union __ehci_ptr    p_next;
};

/***************************************函数声明**************************************************/
/**
 * USB 内部EHCI初始化函数
 *
 * @param p_ehci        EHCI结构体
 * @param reg_base      USB性能寄存器基地址
 * @param task_prio     EHCI任务优先级
 * @param pfn_rh_change 根集线器变化回调函数
 * @param ps_bit        端口速度位
 * @param nqhs          QH数量
 * @param nqtds         qTD数量
 * @param nitds         iTD数量
 * @param nsitds        siTD数量
 *
 * @return 成功返回USB_OK
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
/** 启动EHCI守护任务*/
void usbh_ehci_guard_evt_start(struct usbh_ehci *p_ehci);
/** EHCI中断处理函数*/
void usbh_ehci_irq (struct usbh_ehci *p_ehci);
/**
 * EHCI 内部根集线器变化检测函数
 *
 * @param p_ehci   EHCI结构体
 * @param p_change 返回的变化值
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbh_ehci_rh_change_check (struct usbh *p_ehci,
                                     uint16_t    *p_change);
/** EHCI 内部根集线器初始化，一般USB主机只有一个端口，这个端口就是连接根集线器*/
void usbh_ehci_rh_init (struct usbh_ehci *p_ehci);
/** EHCI端口变化函数*/
void usbh_ehci_port_change (struct usbh_ehci *p_ehci);
/**
 * 为EHCI所需的所有数据结构申请内存空间
 *
 * @param p_ehci EHCI结构体
 * @param nqhs   QH的数量
 * @param nqtds  qTD的数量
 * @param nitds  iTD的数量
 * @param sitds  siTD的数量
 *
 * @return 成功返回AW_OK
 */
usb_err_t usbh_ehci_mem_init (struct usbh_ehci *p_ehci,
                              int               nqhs,
                              int               nqtds,
                              int               nitds,
                              int               nsitds);
/** EHCI数据结构使用状况*/
int usbh_ehci_mem_sta (int contrl_num);
/** 分配一个队列头*/
struct usbh_ehci_qh *usbh_ehci_qh_alloc (struct usbh_ehci *p_ehci);
/** 分配一个队列传输描述符*/
struct usbh_ehci_qtd *usbh_ehci_qtd_alloc (struct usbh_ehci *p_ehci);
/** 释放一个队列头*/
usb_err_t usbh_ehci_qh_free (struct usbh_ehci    *p_ehci,
                             struct usbh_ehci_qh *p_qh);
/** 释放一个队列传输描述符*/
usb_err_t usbh_ehci_qtd_free (struct usbh_ehci     *p_ehci,
                              struct usbh_ehci_qtd *p_qtd);
/** 分配一个等时传输描述符*/
struct usbh_ehci_itd *usbh_ehci_itd_alloc (struct usbh_ehci *p_ehci);
/** 释放一个等时传输描述符*/
usb_err_t usbh_ehci_itd_free (struct usbh_ehci *p_ehci, struct usbh_ehci_itd *p_itd);
/** 分配一个分割等时传输描述符*/
struct usbh_ehci_sitd *usbh_ehci_sitd_alloc (struct usbh_ehci *p_ehci);
/** 释放分割等时传输描述符*/
usb_err_t usbh_ehci_sitd_free (struct usbh_ehci *p_ehci, struct usbh_ehci_sitd *p_sitd);
/**
 * EHCI传输完成函数
 *
 * @param p_ehci EHCI结构体
 * @param p_trp  完成的传输请求包
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbh_ehci_trp_done (struct usbh_ehci *p_ehci, struct usbh_trp  *p_trp);
/** 获取USB主机控制器周期帧列表的帧索引(精确到微帧)*/
uint32_t usbh_ehci_read_frame_index(struct usbh_ehci *p_ehci);
/**
 * 检查周期带宽
 *
 * @param p_ehci EHCI结构体
 * @param frame  帧索引
 * @param uframe 微帧索引
 *
 * @return 成功返回指定微帧的带宽使用情况
 */
uint16_t periodic_usecs (struct usbh_ehci *p_ehci, uint32_t frame, uint32_t uframe);
/**
 * 使能周期调度
 *
 * @param p_ehci EHCI结构体
 * @param is_iso 是不是等时传输
 */
void usbh_ehci_periodic_enable(struct usbh_ehci *p_ehci, uint8_t is_iso);
/** 关闭周期调度*/
void usbh_ehci_periodic_disable(struct usbh_ehci *p_ehci);
/**********************************************************************************************/

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_USBH_EHCI_H */

