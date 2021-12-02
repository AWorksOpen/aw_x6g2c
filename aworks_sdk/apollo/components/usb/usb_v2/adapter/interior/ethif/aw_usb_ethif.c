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
#include "apollo.h"
#include "aw_vdebug.h"
#include "aw_sem.h"
#include "aw_mem.h"
#include "host/usbh.h"
#include "host/class/cdc/net/usbh_net.h"
#include "aw_netif.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

struct usb_netif{
    aw_netif_t ethif;
    void      *priv;
};

/** USB�����豸���ӳ�ʼ������*/
static aw_err_t awbl_usbnet_link_init (aw_netif_t *p_ethif){
    struct usb_netif *p_usb_netif = AW_CONTAINER_OF(p_ethif, struct usb_netif, ethif);
    struct usbh_usbnet *dev = p_usb_netif->priv;
    /* �Ϸ��Լ��*/
    if(dev == NULL){
        return -AW_EINVAL;
    }
    return usbnet_link_init(dev);
}

/** USB�����豸��·���Ӻ���*/
static aw_err_t  awbl_usbnet_up (struct aw_netif *p_ethif)
{
    return AW_OK;
}

/** USB�����豸��·�Ͽ�����*/
static aw_err_t  awbl_usbnet_down (struct aw_netif *p_ethif)
{
    return AW_OK;
}

/** USB�����豸�������*/
static aw_err_t awbl_usbnet_output (aw_netif_t *p_ethif, aw_net_buf_t *p_buf){

    aw_err_t retval = AW_OK;
    struct usb_netif *p_usb_netif = AW_CONTAINER_OF(p_ethif, struct usb_netif, ethif);
    struct usbh_usbnet *dev = p_usb_netif->priv;
    uint8_t *buf = NULL;
    uint32_t  tx_len = 0;

    /* �Ϸ��Լ��*/
    if(dev == NULL){
        return -AW_EINVAL;
    }
    /* ��������ݻ���*/
    buf = aw_mem_alloc(p_buf->tot_len + 1);
    if(buf == NULL)
        return -AW_ENOMEM;
    memset(buf, 0, p_buf->tot_len + 1);

    /* ����һ��pbuf������ȡ�������ݳ���*/
    tx_len = aw_net_buf_copy_partial(p_buf, buf, (size_t) -1, 0);
    retval = usbnet_output(dev, buf, tx_len);
    aw_mem_free(buf);
    return retval;
}

/** USB����io����*/
static aw_err_t awbl_usbnet_ioctl (aw_netif_t *p_netif, int opt, int name, void *p_arg){
    struct usb_netif *p_usb_netif = AW_CONTAINER_OF(p_netif, struct usb_netif, ethif);
    struct usbh_usbnet *dev = p_usb_netif->priv;
    /* �Ϸ��Լ��*/
    if(dev == NULL){
        return -AW_EINVAL;
    }
    return usbnet_ioctl(dev, opt);
}

static void awbl_usbnet_release (aw_netif_t *p_ethif){
    return;
}

/* USB�����豸Ĭ�ϲ�������*/
aw_local aw_const aw_netif_info_get_entry_t __usbnet_netif_info = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

/* ���������ṩ�Ļ�������*/
aw_local aw_netif_ops_t __usbnet_netif_ops = {
    awbl_usbnet_link_init,
    awbl_usbnet_up,
    awbl_usbnet_down,
    awbl_usbnet_output,
    awbl_usbnet_ioctl,
    awbl_usbnet_release,
};

/** AWorks������̫��������·�������亯��*/
usb_err_t usb_netif_set_link_up(void *p_handle){
    struct usb_netif *p_usb_ethif = (struct usb_netif *)p_handle;

    return aw_netif_set_link_up(&p_usb_ethif->ethif);
}

/** AWorks������̫��������·���������亯��*/
usb_err_t usb_netif_set_link_down(void *p_handle){
    struct usb_netif *p_usb_ethif = (struct usb_netif *)p_handle;

    return aw_netif_set_link_down(&p_usb_ethif->ethif);
}

/** AWorks�����̫��������·״̬���亯��*/
usb_err_t usb_netif_is_link_up(void *p_handle, usb_bool_t *p_up){
    struct usb_netif *p_usb_ethif = (struct usb_netif *)p_handle;

    return aw_netif_is_link_up(&p_usb_ethif->ethif, p_up);
}

/**  AWorks�����������״̬���亯��*/
usb_err_t usb_netif_is_up(void *p_handle, usb_bool_t *p_up){
    struct usb_netif *p_usb_ethif = (struct usb_netif *)p_handle;

    return aw_netif_is_up(&p_usb_ethif->ethif, p_up);
}

/** AWorks��̫�������ύ���亯��*/
usb_err_t usb_netif_input(void *p_handle, void *data, uint32_t data_len){
    struct usb_netif *p_usb_ethif = (struct usb_netif *)p_handle;

    return aw_netif_input(&p_usb_ethif->ethif, data, data_len);
}

/** AWorks����һ����̫���������亯��*/
void* usb_ethif_create(char *p_name, uint8_t *p_hwaddr, int mtu, void* p_user_data){
    struct usb_netif *p_usb_ethif;
    aw_err_t ret;

    p_usb_ethif = aw_mem_alloc(sizeof(struct usb_netif));
    if(p_usb_ethif == NULL){
        return NULL;
    }
    memset(p_usb_ethif, 0, sizeof(struct usb_netif));

    p_usb_ethif->priv = p_user_data;

    ret = aw_netif_add(&p_usb_ethif->ethif, p_name, &__usbnet_netif_ops, &__usbnet_netif_info,
                      AW_NETIF_TYPE_ETHERNET, p_hwaddr, mtu);
    if(ret != AW_OK){
        aw_mem_free(p_usb_ethif);
        return NULL;
    }

    return p_usb_ethif;
}

/** AWorksɾ���������亯��*/
usb_err_t usb_netif_remove(void *p_handle){
    struct usb_netif *p_usb_ethif = (struct usb_netif *)p_handle;

    return aw_netif_remove(&p_usb_ethif->ethif);
}






