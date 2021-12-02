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
#ifndef __USBH_DEV_H
#define __USBH_DEV_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "usb_adapter_inter.h"
#include "list/usb_list.h"
#include "usb_common_def.h"

/* USB设备最大接口数量*/
#define USBH_MAX_INTERFACES   32
/* USB设备最大配置数量*/
#define USBH_MAX_CONFIG       5
/* USB设备最大端点数目*/
#define USBH_MAX_EP_NUM       32

#define USBH_TRP_SHORT_NOT_OK  0x00000001    /* 短包错误*/
#define USBH_TRP_ISO_ASAP      0x00000002    /* Start ISO transfer at the earliest */
#define USBH_TRP_ZERO_PACKET   0x00000004    /* 用0包结束批量传输*/
#define USBH_TRP_NO_INTERRUPT  0x00000008    /* no interrupt needed,except for error */

#define AWBL_USBDEV_INJECT       1          /* 设备插入标志*/
#define AWBL_USBDEV_EJECT        2          /* 设备拔出标志*/
#define AWBL_USBH_DEV_UNKNOWN   (1 << 8)    /* 未知类型*/

#define USBDEV_INJECT      1    /* 设备插入标志*/
#define USBDEV_EJECT       2    /* 设备拔出标志*/
#define USBDEV_EXIST       4    /* 设备当前是否存在标志*/
#define USBDEV_IGONRE_PID  0    /* 忽略使用PID*/
#define USBDEV_IGONRE_VID  0    /* 忽略使用VID*/

/* 超时时间，毫秒， 用于发送/接收控制信息*/
#define USB_CTRL_GET_TIMEOUT    5000
#define USB_CTRL_SET_TIMEOUT    5000

/* USB设备特殊配置结构体*/
struct usbh_dev_spec_config_info{
    uint16_t pid;
    uint16_t vid;
    uint16_t config_num;
};

typedef void* usbh_monitor_handle;

/** 设备类型*/
#define USBH_DEV_HUB     (1 << 0)  /* USB集线器设备*/
#define USBH_DEV_UMS     (1 << 1)  /* USB大容量存储设备*/
#define USBH_DEV_UVC     (1 << 2)  /* USB摄像头设备*/
#define USBH_DEV_USERIAL (1 << 3)  /* USB串口设备*/
#define USBH_DEV_UNIC    (1 << 4)  /* USB网卡*/
#define USBH_DEV_UHID    (1 << 5)  /* USB人体接口设备*/
#define USBH_DEV_UAC     (1 << 7)  /* USB音频设备*/
#define USBH_DEV_UNKNOWN (1 << 8)  /* 未知类型*/

/** USB设备监视结构体*/
struct usbh_dev_monitor {
    uint16_t             pid;                                            /* 监控的设备的PID*/
    uint16_t             vid;                                            /* 监控的设备的VID*/
    uint8_t              type;                                           /* 监控类型*/
    void               (*user_callback) (void *p_arg, uint8_t evt_type); /* 用户回调函数*/
    void                *p_arg;                                          /* 回调函数参数*/
    struct usb_list_head node;                                           /* 节点*/
    uint32_t             dev_type;                                       /* 设备类型*/
    void                *job;                                            /* 用户数据*/
    uint8_t              state;                                          /* 状态*/
};

/** USB 等时包描述符*/
struct usb_iso_packet_descriptor {
    uint32_t offset;         /* 偏移*/
    uint32_t  length;        /* 期望传输的长度*/
    uint32_t actual_length;  /* 实际传输的长度*/
    int status;              /* 状态*/
};

/** USB 传输请求包*/
struct usbh_trp {
    struct usbh_endpoint             *p_ep;                    /* 传输请求包相关端点*/
    struct usb_ctrlreq               *p_ctrl;                  /* SETUP包*/
    void                             *p_data;                  /* 数据缓存*/
    size_t                            len;                     /* 要传输的长度*/
    void                             *p_ctrl_dma;              /* SETUP包映射的DMA内存*/
    void                             *p_data_dma;              /* 数据过程映射的DMA内存*/
    void                            (*pfn_done) (void *p_arg); /* 传输完成回调函数*/
    void                             *p_arg;                   /* 传输完成回调函数参数*/
    size_t                            act_len;                 /* 实际传输长度*/
    int                               status;                  /* 传输状态*/
    int                               flag;                    /* 传输标志*/
    void                             *priv;                    /* 用户私有数据*/
    int                               interval;                /* 传输时间周期(中断/等时)*/
    void                             *hcpriv;                  /* 主机控制器的私有数据*/
    int                               error_count;             /* (返回)等时传输的错误数量*/
    int                               start_frame;             /* 等时起始帧*/
    int                               number_of_packets;       /* (输入)同步包的数量(add by CYX at 9/17-2019)*/
    struct usb_iso_packet_descriptor *iso_frame_desc;          /* 等时包描述符(add by CYX at 9/17-2019)*/
    struct usb_list_head              node;                    /* 当前USB传输请求包的节点*/
};

/* USB 端点结构体*/
struct usbh_endpoint {
    struct usbh_device       *p_udev;    /* 所属设备结构体*/
    struct usb_endpoint_desc *p_desc;    /* USB端点描述符定义*/
    usb_bool_t                enabled;   /* 使能标志位*/
    int                       bandwidth; /* 端点所需带宽*/
    int                       interval;  /* 传输时间周期(中断/等时)*/
    struct usb_list_head      node;      /* 本端点结构体节点*/
    usb_sem_handle_t          sem;       /* 信号量*/
    void                     *priv;      /* 私有数据域*/
    int                       extralen;  /* 额外的描述符的长度*/
    uint8_t                  *extra;     /* 额外的描述符(例如，特定类描述符或特定产商描述符) */
};

/** USB 接口结构体*/
struct usbh_interface {
    struct usb_list_head        node;      /* 当前接口节点*/
    struct usb_interface_desc  *p_desc;    /* 接口描述符*/
    struct usbh_endpoint       *eps;       /* 接口下的端点*/
    int                         extralen;  /* 额外的描述符的长度*/
    uint8_t                    *extra;     /* 额外的描述符(例如，特定类描述符或特定产商描述符) */
};

/* USB 配置结构体*/
struct usbh_config {
    struct usb_config_desc          *p_desc;    /* 配置描述符*/
    char                            *string;    /* 配置描述*/
    struct usbh_function            *funs;      /* 接口功能结构体*/
    int                              nfuns;     /* 接口功能数*/
    int                              extralen;  /* 额外的描述符的长度*/
    uint8_t                         *extra;     /* 额外的描述符(例如，特定类描述符或特定产商描述符) */
};

/** USB 设备结构体*/
struct usbh_device {
    struct usbh                    *p_hc;                 /* USB主机结构体*/
    char                            name[32];             /* 设备名字*/
    int                             ref;                  /* 设备引用计数*/
    usb_mutex_handle_t              lock;                 /* 设备互斥锁*/
    struct usb_list_head            node;                 /* 当前设备结构体节点*/
    uint8_t                         addr;                 /* 设备地址*/
    uint8_t                         speed;                /* 设备速度*/
    struct usbh_hub                *p_hub;                /* 所属集线器*/
    uint8_t                         port;                 /* 所属集线器端口号*/
    uint8_t                         status;               /* 设备状态*/
#define USBH_DEV_STA_CONN       0x01                      /* 设备已连接状态*/
#define USBH_DEV_STA_INJECT     0x02                      /* 设备已插入状态*/
#define USBH_DEV_STA_CFG        0x04                      /* 设备已配置状态*/
/* 判断设备是否是连接状态*/
#define USBH_IS_DEV_CONN(p_dev) \
            ((p_dev)->status & USBH_DEV_STA_CONN)
/* 判断设备是否是插入状态*/
#define USBH_IS_DEV_INJECT(p_dev) \
            ((p_dev)->status & USBH_DEV_STA_INJECT)
/* 判断设备是否是配置状态*/
#define USBH_IS_DEV_CFG(p_dev) \
            ((p_dev)->status & USBH_DEV_STA_CFG)
/* 判断设备状态是否有效*/
#define USBH_IS_DEV_VALID(p_dev) \
            ((p_dev)->status == (USBH_DEV_STA_CONN | \
                                 USBH_DEV_STA_CFG | \
                                 USBH_DEV_STA_INJECT))
    struct usb_ctrlreq        *p_ctrl;                   /* 控制请求(SETUP)*/
    struct usb_device_desc    *p_desc;                   /* USB设备描述符定义*/
    char                      *p_str_mft;                /* 设备制造商*/
    char                      *p_str_pdt;                /* 设备产商*/
    char                      *p_str_snum;               /* 设备序列号*/
    struct usbh_config         cfg;                      /* USB设备的配置结构体*/
    struct usbh_endpoint       ep0;                      /* 端点0*/
    struct usb_endpoint_desc   ep0_desc;                 /* 端点0描述符定义*/
    struct usbh_endpoint      *ep_in[16];                /* 设备输入端点*/
    struct usbh_endpoint      *ep_out[16];               /* 设备输出端点*/
    uint16_t                   langid;                   /* 语言ID*/
    uint32_t                   quirks;                   /* 设备兼容*/
    uint32_t                   dev_type;                 /* 设备类型*/
    struct usbh_tt            *tt;                       /* 事务转换器(低/全速设备接到高速集线器)*/
    int                        ttport;                   /* 设备在事物转换器集线器的端口号*/
};

/** USB功能结构体*/
struct usbh_function {
    char                                 name[16];    /* 设备名字*/
    struct usbh_device                  *p_udev;      /* 相关的USB设备结构体*/
    struct usb_list_head                 node;        /* 本功能节点*/
    void                                *user_priv;   /* 用户私有信息*/

    uint8_t                              ifirst;      /* 第一个接口的编号*/
    uint8_t                              nitfs;       /* 接口的数量*/
    uint8_t                              nitfs_type;  /* 接口类型的数量(普通接口该值为1，联合接口该值为联合接口包含的接口类型数量)*/
    uint8_t                              clss;        /* 类*/
    uint8_t                              sclss;       /* 子类*/
    uint8_t                              proto;       /* 协议*/
    uint8_t                              type;        /* 接口类型(普通接口或联合接口)*/

    struct usb_list_head                 itfs;        /* 接口链表*/
    void                                *driver_priv; /* 驱动私有信息*/
};

/***************************************函数声明************************************************/
/**
 * 寻找特定的描述符
 *
 * @param buffer 数据
 * @param size   数据大小
 * @param dt1    要找到的描述符类型1
 * @param dt2    要找到的描述符类型2
 * @param n      跳过的描述符个数
 *
 * @return 跳过的数据大小(字节)
 */
int find_next_descriptor(unsigned char *buffer,
                         int            size,
                         int            dt1,
                         int            dt2,
                         int           *num_skipped);
/** 初始化USB主机设备库*/
usb_err_t usbh_dev_lib_init (void);
/**
 * 创建一个USB设备
 *
 * @param p_hc  USB主机结构体
 * @param p_hub 所属集线器结构体
 * @param port  端口号
 *
 * @return 成功返回创建成功的USB设备结构体
 */
struct usbh_device *usbh_dev_create (struct usbh     *p_hc,
                                     struct usbh_hub *p_hub,
                                     uint8_t          port);
/** 销毁USB设备结构体*/
usb_err_t usbh_dev_destroy (struct usbh_device *p_udev);
/** USB设备插入函数*/
usb_err_t usbh_dev_inject (struct usbh_device *p_udev);
/** USB设备弹出函数*/
usb_err_t usbh_dev_eject (struct usbh_device *p_udev);
/** 获取设备类型*/
uint32_t usbh_dev_type_get(struct usbh_device *p_udev);
/** 设置设备类型*/
usb_err_t usbh_dev_type_set(struct usbh_device *p_udev, uint32_t dev_type);
/** 增加USB设备的引用*/
usb_err_t usbh_dev_ref_get (struct usbh_device *p_udev);
/** 减少USB设备的引用*/
usb_err_t usbh_dev_ref_put (struct usbh_device *p_udev);
/** 设置USB功能用户数据*/
usb_err_t usbh_func_usrdata_set(struct usbh_function *p_fun, void *usrdata);
/** 获取USB功能用户数据*/
void *usbh_func_usrdata_get(struct usbh_function *p_fun);
/** 设置USB功能驱动数据*/
usb_err_t usbh_func_drvdata_set(struct usbh_function *p_fun, void *usrdata);
/** 获取USB功能驱动数据*/
void *usbh_func_drvdata_get(struct usbh_function *p_fun);
/**
 * USB获取描述符函数
 *
 * @param p_dev 设备结构体
 * @param type  请求描述符类型
 * @param value 参数
 * @param index 索引
 * @param len   要写/读的数据长度
 * @param p_buf 数据缓存
 *
 * @return 成功返回实际传输长度，失败返回传输请求包状态
 */
usb_err_t usbh_dev_desc_get (struct usbh_device *p_dev,
                             uint16_t            type,
                             uint16_t            value,
                             uint16_t            index,
                             uint16_t            len,
                             void               *p_buf);
/**
 * USB主机获取设备字符串描述符
 *
 * @param p_udev 目标设备
 * @param index  描述符索引
 *
 * @return 成功返回获取到的字符串
 */
char *usbh_dev_string_get (struct usbh_device *p_udev,
                           uint8_t             index);
/**
 * 获取设备当前的配置编号
 *
 * @param p_dev USB设备
 *
 * @return 成功返回配置编号
 */
int usbh_dev_config_num_get(struct usbh_device *p_dev);
/**
 * USB主机设备地址设置
 *
 * @param p_dev 设置地址的设备结构体
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbh_dev_addr_set (struct usbh_device *p_dev);
/**
 * USB主机设置设备配置描述符
 *
 * @param p_dev 目标设备
 * @param val   配置编号
 *
 * 返回：成功返回USB_OK
 */
usb_err_t usbh_dev_cfg_set (struct usbh_device *p_dev,
                            uint8_t             val);
/**
 * 寻找USB总线上的设备
 *
 * @param vid      设备VID
 * @param pid      设备PID
 * @param is_print 是否把信息打印出来
 *
 * @return 返回对应的PID和VID的设备
 */
void *usbh_dev_find(uint16_t vid, uint16_t pid, uint32_t dev_type, usb_bool_t is_print);
/** 通过USB设备名字找对应的USB设备*/
void *usbh_dev_find_byname(char *p_name);
/** USB设备重新初始化*/
usb_err_t usbh_dev_reinit(void *handle);
/**
 * 检查USB设备是否在连接状态
 *
 * @param p_udev 要检查的USB设备
 *
 * @return 连接返回USB_TRUE，没连接返回USB_FALSE
 */
usb_bool_t usbh_dev_connect_check (struct usbh_device *p_udev);
/**
 * 通过接口号找到对应的接口结构体
 *
 * @param p_udev  USB设备结构体
 * @param intfnum 接口编号
 * @param altnum  接口的备用设置号
 *
 * @return 找到返回接口结构体。
 */
struct usbh_interface *usbh_dev_intfnum_to_intf(struct usbh_device *p_udev,
                                                uint32_t            intfnum,
                                                uint32_t            altnum);
/**
 * 通过接口号找到对应的功能结构体
 *
 * @param p_udev  USB设备结构体
 * @param intfnum 接口编号
 *
 * @return 找到返回功能结构体。
 */
struct usbh_function *usbh_dev_intfnum_to_func(struct usbh_device *p_udev, uint32_t intfnum);

/**
 * 复位一个端点的状态
 *
 * @param p_udev 要复位端点的设备
 * @param epaddr 端点地址
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbh_dev_ep_reset(struct usbh_device *p_udev, unsigned int epaddr);

/**
 * USB设备监控注册函数
 *
 * @param vid           要监控的设备的PID
 * @param pid           要监控的设备的VID
 * @param type          监控类型
 * @param user_callback 用户回调函数
 * @param data          回调函数参数
 *
 * @return 成功返回监控句柄
 */
usbh_monitor_handle usbh_dev_monitor_register(uint16_t vid,
                                              uint16_t pid,
                                              uint32_t dev_type,
                                              uint8_t  monitor_type,
                                              void   (*user_callback) (void *p_arg, uint8_t type),
                                              void    *p_arg);
usbh_monitor_handle awbl_usbh_dev_monitor_register(uint16_t vid,
                                                   uint16_t pid,
                                                   uint32_t dev_type,
                                                   uint8_t  monitor_type,
                                                   void   (*user_callback) (void *p_arg, uint8_t type),
                                                   void    *p_arg);
/** USB设备监控注销函数*/
usb_err_t usbh_dev_monitor_unregister(usbh_monitor_handle handle);
/**
 * 设置USB设备备用接口
 *
 * @param p_udev  USB设备结构体
 * @param intfnum 接口编号
 * @param altnum  接口的备用设置号
 *
 * @return 成功返回0。
 */
int usbh_func_intf_set(struct usbh_function *p_fun,int intfnum, int altnum);
/** 获取USB 功能第一个接口*/
struct usbh_interface *usbh_func_first_intf_get(struct usbh_function *p_ufunc);
/**
 * 通过接口寻找端点
 *
 * @param p_itf  USB接口结构体
 * @param epaddr 端点地址
 *
 * @return 成功返回找到的端点
 */
struct usbh_endpoint *usbh_intf_ep_find(struct usbh_interface *p_itf,
                                        uint8_t                epaddr);
/**
 * 清除端点停止状态
 *
 * @param p_ep 要清除停止状态的端点
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbh_ep_halt_clear (struct usbh_endpoint *p_ep);

/**
 * USB主机控制传输
 *
 * @param p_ep    使用控制传输的端点
 * @param type    传输方向|请求类型|请求目标
 * @param req     具体的USB请求
 * @param val     参数
 * @param idx     索引
 * @param len     要写/读的数据长度
 * @param p_data  要写/读的数据缓存
 * @param timeout 超时时间
 * @param flag    标志
 *
 * @return 如果没有数据，成功返回 0，如果有数据，成功返回传输的数据大小
 */
usb_err_t usbh_ctrl_trp_sync (struct usbh_endpoint *p_ep,
                              uint8_t               type,
                              uint8_t               req,
                              uint16_t              val,
                              uint16_t              idx,
                              uint16_t              len,
                              void                 *p_data,
                              int                   timeout,
                              int                   flag);
/**
 * USB主机提交传输请求包
 *
 * @param p_ep    使用的端点
 * @param p_ctrl  控制传输请求结构体
 * @param p_data  要写/读的数据缓存
 * @param len     要写/读的数据长度
 * @param timeout 超时时间
 * @param flag    标志
 *
 * @return 成功返回实际传输长度，失败返回传输请求包状态
 */
usb_err_t usbh_trp_sync (struct usbh_endpoint  *p_ep,
                         struct usb_ctrlreq    *p_ctrl,
                         void                  *p_data,
                         int                    len,
                         int                    timeout,
                         int                    flag);
/** 传输请求包完成回调函数*/
void usbh_trp_done (struct usbh_trp *p_trp, int status);
/**
 * 提交传输请求包
 *
 * @param p_trp 要提交的传输请求包
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbh_trp_submit (struct usbh_trp *p_trp);
/**
 * 取消传输请求包
 *
 * @param p_trp 要取消的传输请求包
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbh_trp_cancel (struct usbh_trp *p_trp);
#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __USBH_DEV_H */

