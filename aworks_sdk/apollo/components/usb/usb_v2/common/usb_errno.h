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
/* USB�������� */
typedef int             usb_err_t;

#define USB_OK              0
#define USB_EPERM           1            /* ���������� */
#define USB_ENOENT          2            /* �ļ���Ŀ¼������ */
#define USB_EIO             5            /* I/O ���� */
#define USB_ENXIO           6            /* �豸���ַ������ */
#define USB_ENOEXEC         8            /* ��ִ���ļ���ʽ���� */
#define USB_EBADF           9            /* �ļ��������� */
#define USB_EAGAIN          11           /* ��Դ�����ã������� */
#define USB_ENOMEM          12           /* �ռ䣨�ڴ棩���� */
#define USB_EACCES          13           /* Ȩ�޲��� */
#define USB_EFAULT          14           /* ��ַ���� */
#define USB_EBUSY           16           /* �豸����Դæ */
#define USB_EEXIST          17           /* �ļ��Ѿ����� */
#define USB_EXDEV           18           /* ���豸���� */
#define USB_ENODEV          19           /* �豸������ */
#define USB_EINVAL          22           /* ��Ч���� */
#define USB_EFBIG           27           /* �ļ�̫�� */
#define USB_ENOSPC          28           /* �豸ʣ��ռ䲻�� */
#define USB_EPIPE           32           /* �𻵵Ĺܵ� */
#define USB_ENOTSUP         35           /* ��֧�� */
#define USB_EMSGSIZE        36           /* ��Ϣ̫�� */

#define USB_EDOM            37           /* ��ѧ��������������Χ */
#define USB_ERANGE          38           /* ��ѧ����������̫�� */

#define USB_EOPNOTSUPP      45           /* socket��֧�ָò��� */

#define USB_ECONNRESET      54           /* ���Ӹ�λ */
#define USB_ENOTCONN        57           /* Socketû������ */
#define USB_ESHUTDOWN       58           /* Socket�Ѿ��رգ����ܷ������� */

#define USB_EINPROGRESS     68           /* �������ڽ����� */

#define USB_ECANCELED       72           /* �����Ѿ�ȡ�� */
#define USB_ENOSR           74           /* û������Դ */
#define USB_EPROTO          76           /* Э����� */
#define USB_ENODATA         78           /* ���������� */
#define USB_ETIME           79           /* ��ioctl()��ʱ */
#define USB_ECOMM           109          /* Communication error on send */
#define USB_EOVERFLOW       112          /* Value too large for defined data type */

