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


/*******************************************************************************
  includes
*******************************************************************************/
#include "apollo.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_delayed_work.h"
#include "fs/aw_blk_dev.h"
#include "aw_atomic.h"
#include "host/awbl_usbh.h"
#include "aw_mem.h"
#include "host/class/ms/awbl_usbh_mass_storage.h"
#include <string.h>
#include <stdio.h>
#include "host/class/ms/usbh_mass_storage.h"

#define AWBL_USBH_MASS_STORAGE_DRV_NAME   "awbl_usbh_mass_storage"

/* ��ӡ����*/
#define __AMS_TRACE(info, ...) \
            do { \
                AW_INFOF(("AUSB-MS:"info, ##__VA_ARGS__)); \
            } while (0)

#define __MS_LOCK_TIMEOUT      10000

aw_local int  __g_buf_size = 16 * 1024;

/* �������豸����ƥ����Ϣ��*/
aw_local const struct awbl_usbh_fundrv_info_tab __drv_info_tab[] = {

    {
        AWBL_USBH_FUNDRV_MATCH_FUN_CLASS,
        0,0,0,0,0,
        USB_CLASS_MASS_STORAGE,
        0,0,0
    },
    AWBL_USBH_FUNDRV_INFO_END
};

/** �������洢�豸��д��*/
aw_local aw_err_t __ms_rw_nblks (struct awbl_usbh_ms_lun *p_lun,
                                 uint_t                   blk_no,
                                 uint_t                   nblks,
                                 aw_bool_t                read)
{
    aw_err_t  ret;

    if(p_lun->p_ms_lun == NULL){
        return -AW_EINVAL;
    }

    if (read) {
        /* ��*/
        ret = usbh_ms_blk_read(p_lun->p_ms_lun, blk_no, nblks, NULL);
    } else {
        /* д*/
        ret = usbh_ms_blk_write(p_lun->p_ms_lun, blk_no, nblks, NULL);
    }

    if (ret >= 0) {
        /* ����д�����ݴ�С*/
        if (ret != nblks * usbh_ms_lun_bsize_get(p_lun->p_ms_lun)) {
            ret = -AW_EIO;
        } else {
            ret = AW_OK;
        }
    }

    return ret;
}

/** ���豸���ƺ���*/
static int __bd_ioctl (struct aw_blk_dev *p_dev, int cmd, void *arg)
{
    int                       ret   = AW_OK;
    struct awbl_usbh_ms_lun  *p_lun = NULL;
    struct usbh_mass_storage *p_ms  = NULL;

    p_lun = AW_CONTAINER_OF(p_dev, struct awbl_usbh_ms_lun, bd);

    if(p_lun->p_ms_lun == NULL){
        return -AW_EINVAL;
    }
    /* ��ȡ�������洢�豸*/
    p_ms = usbh_ms_dev_get_from_lun(p_lun->p_ms_lun);
    if(p_ms == NULL){
        return -AW_EINVAL;
    }

    /* ����豸�Ƿ��Ƴ�*/
    if (usbh_ms_dev_removed_check(p_ms)) {
        return -AW_ENODEV;
    }

    switch (cmd) {
    case AW_BD_SYNC:
        break;

    case AW_BD_GET_BASE_NAME:
        /* ��ȡ��������*/
        strcpy(arg, usbh_ms_lun_name_get(p_lun->p_ms_lun));
        break;

    case AW_BD_TEST:
        break;

    case AW_BD_STACK_COMPLETE:
        break;

    case AW_BD_GET_ATTRIBUTE:
        /* ��ȡ��������*/
        if (usbh_ms_lun_is_wp(p_lun->p_ms_lun)) {
            *(int*)arg = (AW_BD_ARRT_REMOVABLE | AW_BD_ARRT_READ_ONLY);
        } else {
            *(int*)arg = AW_BD_ARRT_REMOVABLE;
        }
        break;

    case AW_BD_GET_BSIZE:
        /* ��ȡ�������С*/
        *(uint32_t*)arg = usbh_ms_lun_bsize_get(p_lun->p_ms_lun);
        break;

    case AW_BD_GET_BLKS:
        /* ��ȡ����������*/
        *(uint32_t*)arg = usbh_ms_lun_nblks_get(p_lun->p_ms_lun);
        break;

    default:
        ret = -AW_ENOTSUP;
        break;
    }

    return ret;
}

/** ���豸������*/
static int __bd_areq (struct aw_blk_dev *p_dev,
                      struct aw_blk_arp *p_arp)
{
    struct awbl_usbh_ms_lun  *p_lun;
    uint8_t                  *ptr;
    uint_t                    total, nblks, blk_no, tmp, limt;
    aw_err_t                  ret = AW_OK;
    struct usbh_mass_storage *p_ms  = NULL;

    p_lun = AW_CONTAINER_OF(p_dev, struct awbl_usbh_ms_lun, bd);

    if (p_lun->p_ms_lun == NULL){
        return -AW_EINVAL;
    }
    /* ��ȡ�������洢�豸*/
    p_ms = usbh_ms_dev_get_from_lun(p_lun->p_ms_lun);
    if(p_ms == NULL){
        return -AW_EINVAL;
    }
    /* ����豸�Ƿ��Ƴ�*/
    if (usbh_ms_dev_removed_check(p_ms)) {
        return -AW_ENODEV;
    }

    /* ��ѯ���������*/
    for (; p_arp != NULL; p_arp = p_arp->p_next) {
        /* ����д����ʼ����*/
        if (p_arp->blk_no >= usbh_ms_lun_nblks_get(p_lun->p_ms_lun)) {
            p_arp->error = -AW_ENOSPC;
            p_arp->resid = p_arp->nbytes;
        } else {
            /* ����Ҫ��д�Ŀ������*/
            nblks  = usbh_ms_lun_nblks_get(p_lun->p_ms_lun) - p_arp->blk_no;
            /* ���ƶ�д�Ŀ�����*/
            total  = min(nblks, (p_arp->nbytes / usbh_ms_lun_bsize_get(p_lun->p_ms_lun)));
            /* ����һ���ܶ�д�Ŀ������*/
            limt   = __g_buf_size / usbh_ms_lun_bsize_get(p_lun->p_ms_lun);
            blk_no = p_arp->blk_no;
            tmp    = total;
            ptr    = p_arp->p_data;

            while (total) {
                if (usbh_ms_dev_removed_check(p_ms)) {
                    ret = -AW_ENODEV;
                    break;
                }
                /* ����һ�ζ�д�Ŀ�����*/
                nblks = min(limt, total);

                /* ��*/
                if (p_arp->flags & AW_BD_ARQ_READ) {
                    ret = __ms_rw_nblks(p_lun, blk_no, nblks, AW_TRUE);
                    if (ret != AW_OK) {
                        break;
                    }
                    usbh_ms_lun_databuf_get(p_lun->p_ms_lun, ptr,
                            nblks * usbh_ms_lun_bsize_get(p_lun->p_ms_lun));

                } else {
                    /* д*/
                    usbh_ms_lun_databuf_set(p_lun->p_ms_lun, ptr,
                            nblks * usbh_ms_lun_bsize_get(p_lun->p_ms_lun));
                    ret = __ms_rw_nblks(p_lun, blk_no, nblks, AW_FALSE);
                    if (ret != AW_OK) {
                        break;
                    }
                }

                /* ����ƫ��*/
                ptr += (nblks * usbh_ms_lun_bsize_get(p_lun->p_ms_lun));
                /* ���ö�д��ʼ����*/
                blk_no += nblks;
                total  -= nblks;
            }

            p_arp->error = ret;
            /* ����ʣ��û��д���ֽ���*/
            p_arp->resid = p_arp->nbytes - (tmp - total) * usbh_ms_lun_bsize_get(p_lun->p_ms_lun);
            /* ��ɻص�*/
            if (p_arp->pfn_done) {
                p_arp->pfn_done(p_arp);
            }
        }
    }

    return AW_OK;
}

/** ���豸�ͷź���*/
static void __bd_release (struct aw_blk_dev *p_dev)
{
    struct awbl_usbh_ms_lun  *p_lun;

    p_lun = AW_CONTAINER_OF(p_dev, struct awbl_usbh_ms_lun, bd);
    /* �ͷ��߼�����*/
    usbh_ms_valid_lun_put(p_lun->p_ms_lun);
}

/** ��ȡ�������豸����*/
char *awbl_usbh_mass_storage_lun_blkname_get(struct usbh_ms_lun *p_lun){
    if(p_lun == NULL){
        return NULL;
    }

    struct awbl_usbh_ms_lun *p_aw_lun = usbh_ms_lun_usrdata_get(p_lun);
    if(p_aw_lun == NULL){
        return NULL;
    }

    return p_aw_lun->bd.name;
}

/** �Ƴ�����*/
aw_local aw_err_t __drv_remove (struct awbl_dev *p_awdev,
                                void            *arg)
{
    __AWBL_USBH_FUN_DECL(     p_fun, p_awdev);
    struct usbh_mass_storage *p_ms  = NULL;
    struct usbh_ms_lun       *p_lun = NULL;
    int                       i;

    /* ��ȡUSB��������˽������*/
    p_ms = (struct usbh_mass_storage *)usbh_func_drvdata_get(p_fun->usb_func);
    if(p_ms == NULL){
        return AW_OK;
    }

    /* ���������ٿ��豸*/
    for (i = 0; i < p_ms->nluns; i++) {

        p_lun = usbh_ms_valid_lun_get(p_ms, i);
        if(p_lun != NULL){
            struct awbl_usbh_ms_lun *p_aw_lun =
                    (struct awbl_usbh_ms_lun *)(usbh_ms_lun_usrdata_get(&p_ms->luns[i]));
            usbh_ms_valid_lun_put(p_lun);
            aw_blk_dev_eject(&(p_aw_lun->bd));
            aw_blk_dev_destroy(&(p_aw_lun->bd));
            aw_mem_free(p_aw_lun);
        }
    }
    /* ���ٴ������洢�豸*/
    usbh_ms_dev_destory(p_ms);

    return AW_OK;
}

/** ̽�⺯��*/
aw_local aw_bool_t __drv_probe (awbl_dev_t *p_awdev)
{
    __AWBL_USBH_FUN_DECL(p_fun, p_awdev);
    int                  proto;
    int                  subclass;

    if(p_fun->usb_func == NULL){
        return AW_FALSE;
    }

    /* ���Э��*/
    proto = USBH_FUNC_PROTO_GET(p_fun->usb_func);
    if ((proto != USBH_MS_PR_BULK) &&
        (proto != USBH_MS_PR_CB)   &&
        (proto != USBH_MS_PR_CBI)) {
        return AW_FALSE;
    }
    /* �����Э��*/
    subclass = USBH_FUNC_SUBCLASS_GET(p_fun->usb_func);
    if(subclass == USBH_MS_SC_SCSI){
        return AW_TRUE;
    }

    __AMS_TRACE("no sub class support\r\n");
    return AW_FALSE;
}

/** ��һ�׶γ�ʼ��*/
aw_local void __drv_init1 (struct awbl_dev *p_awdev){
    return;
}

/** �ڶ��׶γ�ʼ��*/
aw_local void __drv_init2 (struct awbl_dev *p_awdev)
{
    __AWBL_USBH_FUN_DECL(     p_fun, p_awdev);
    aw_err_t                  ret;
    struct usbh_mass_storage *p_ms     = NULL;
    struct awbl_usbh_ms_lun  *p_aw_lun = NULL;
    struct usbh_ms_lun       *p_lun    = NULL;
    uint8_t                   nluns    = 0;
    int                       i;

    /* ����һ���������洢�豸*/
    p_ms = usbh_ms_dev_create(p_fun->usb_func, __g_buf_size);
    if(p_ms == NULL){
        return;
    }
    __AMS_TRACE("create a ms device\r\n");
    /* ����USB��������˽������*/
    usbh_func_drvdata_set(p_fun->usb_func, p_ms);

    /* ��ȡ֧�ֵ����Ԫ����*/
    ret = usbh_ms_max_lun_get(p_ms, &nluns);
    if(ret != AW_OK){
        return;
    }
    __AMS_TRACE("ms max luns:%d\r\n", nluns);

    nluns = usbh_ms_lun_num_get(p_ms);
    __AMS_TRACE("ms current valid luns:%d\r\n", nluns);

    p_aw_lun = aw_mem_alloc(sizeof(struct awbl_usbh_ms_lun) * nluns);
    if(p_aw_lun == NULL){
        goto __failed;
    }
    memset(p_aw_lun, 0, sizeof(struct awbl_usbh_ms_lun) * nluns);

    for(i = 0;i < nluns;i++){
        /* ��ȡ��Ч���߼���Ԫ*/
        p_lun = usbh_ms_valid_lun_get(p_ms, i);
        if(p_lun == NULL){
            continue;
        }
        /* �������豸*/
        ret = aw_blk_dev_create(&p_aw_lun[i].bd, usbh_ms_lun_name_get(p_lun));
        if (ret != AW_OK) {
            __AMS_TRACE("create blk dev failed %d\r\n",ret);
            goto __failed;
        }

        p_aw_lun[i].bd.pfn_ioctl   = __bd_ioctl;
        p_aw_lun[i].bd.pfn_areq    = __bd_areq;
        p_aw_lun[i].bd.pfn_release = __bd_release;
        /* ����*/
        usbh_ms_lun_usrdata_set(p_lun, &p_aw_lun[i]);
        p_aw_lun[i].p_ms_lun = p_lun;
        /* ��ȡ�߼���������*/
        ret = ((uint64_t)usbh_ms_lun_nblks_get(p_lun)) *
                (usbh_ms_lun_bsize_get(p_lun)) / (1024 * 1024);
        __AMS_TRACE("luns%d's capacity is: %d MB\r\n", i, ret);
        __AMS_TRACE("create blk dev %s success\r\n", usbh_ms_lun_name_get(p_lun));
        /* ������豸*/
        ret = aw_blk_dev_inject(&p_aw_lun[i].bd);
        if (ret != AW_OK) {
            __AMS_TRACE("inject blk dev failed %dr\n",ret);
            aw_blk_dev_destroy(&p_aw_lun[i].bd);
            goto __failed;
        }
        __AMS_TRACE("inject blk dev %s\r\n", usbh_ms_lun_name_get(p_lun));
    }

    return;
__failed:
    usbh_ms_dev_destory(p_ms);
    return;
}

/** �����׶γ�ʼ��*/
aw_local void __drv_connect (struct awbl_dev *p_awdev)
{
    return;
}

/* method*/
aw_local aw_const struct awbl_dev_method __g_methods[] = {
        AWBL_METHOD(awbl_drv_unlink, __drv_remove),
        AWBL_METHOD_END
};

/* ���׶γ�ʼ��*/
aw_local aw_const struct awbl_drvfuncs __g_drvfuncs = {
        __drv_init1,
        __drv_init2,
        __drv_connect
};

/* USB����������Ϣ�ṹ��*/
aw_local aw_const struct awbl_usbh_fundrv_info __g_drv_info = {
    {
        AWBL_VER_1,                             /* AWBus �汾 */
        AWBL_BUSID_USBH | AWBL_DEVID_DEVICE,    /* ����ID*/
        AWBL_USBH_MASS_STORAGE_DRV_NAME,        /* �������� */
        &__g_drvfuncs,                          /* ������ʼ�������� */
        __g_methods,                            /* p_methods */
        __drv_probe                             /* ����̽�⺯��*/
    },
    __drv_info_tab                              /* USB����������Ϣ��*/
};

/** USB�������洢�豸����ע��*/
void awbl_usbh_mass_storage_drv_register (int buffer_size)
{
    aw_err_t ret;

    ret = awbl_drv_register((struct awbl_drvinfo *)&__g_drv_info);

    if (ret != AW_OK) {
        __AMS_TRACE("driver register failed %d\r\n", ret);
    } else {
        __g_buf_size = buffer_size;
    }
    (void)ret;
}
