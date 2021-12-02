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
/* USB错误类型 */
typedef int             usb_err_t;

#define USB_OK              0
#define USB_EPERM           1            /* 操作不允许 */
#define USB_ENOENT          2            /* 文件或目录不存在 */
#define USB_EIO             5            /* I/O 错误 */
#define USB_ENXIO           6            /* 设备或地址不存在 */
#define USB_ENOEXEC         8            /* 可执行文件格式错误 */
#define USB_EBADF           9            /* 文件描述符损坏 */
#define USB_EAGAIN          11           /* 资源不可用，需重试 */
#define USB_ENOMEM          12           /* 空间（内存）不足 */
#define USB_EACCES          13           /* 权限不够 */
#define USB_EFAULT          14           /* 地址错误 */
#define USB_EBUSY           16           /* 设备或资源忙 */
#define USB_EEXIST          17           /* 文件已经存在 */
#define USB_EXDEV           18           /* 跨设备连接 */
#define USB_ENODEV          19           /* 设备不存在 */
#define USB_EINVAL          22           /* 无效参数 */
#define USB_EFBIG           27           /* 文件太大 */
#define USB_ENOSPC          28           /* 设备剩余空间不足 */
#define USB_EPIPE           32           /* 损坏的管道 */
#define USB_ENOTSUP         35           /* 不支持 */
#define USB_EMSGSIZE        36           /* 消息太大 */

#define USB_EDOM            37           /* 数学函数参数超出范围 */
#define USB_ERANGE          38           /* 数学函数计算结果太大 */

#define USB_EOPNOTSUPP      45           /* socket不支持该操作 */

#define USB_ECONNRESET      54           /* 连接复位 */
#define USB_ENOTCONN        57           /* Socket没有连接 */
#define USB_ESHUTDOWN       58           /* Socket已经关闭，不能发送数据 */

#define USB_EINPROGRESS     68           /* 操作正在进行中 */

#define USB_ECANCELED       72           /* 操作已经取消 */
#define USB_ENOSR           74           /* 没有流资源 */
#define USB_EPROTO          76           /* 协议错误 */
#define USB_ENODATA         78           /* 流中无数据 */
#define USB_ETIME           79           /* 流ioctl()超时 */
#define USB_ECOMM           109          /* Communication error on send */
#define USB_EOVERFLOW       112          /* Value too large for defined data type */

