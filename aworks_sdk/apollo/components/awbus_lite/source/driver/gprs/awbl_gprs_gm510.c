/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/
#include "aworks.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_gprs.h"

#include <stdio.h>
#include <string.h>

#include "driver/gprs/awbl_gprs_gm510.h"

#define __RETURN_EMSG(err, a) return err

/** ��ȡGM510�豸�ṹ��*/
#define __GM510_DEV_DECL(p_this, p_dev) \
        struct awbl_gprs_gm510_dev *p_this = AW_CONTAINER_OF(p_dev, \
                                                           struct awbl_gprs_gm510_dev, \
                                                           dev)
/** ��GPRS�豸�л�ȡGM510�豸�ṹ��*/
#define __GM510_DECL_FROM_GPRS(p_this, p_gprs) \
        struct awbl_gprs_gm510_dev *p_this = \
                     (struct awbl_gprs_gm510_dev *)p_gprs->p_gprs_dev

#define __at_cmd_send(p_this, MSG)  \
        if (p_this->at_pipe) {p_this->drvopr.pfn_at_send(p_this, MSG);}

aw_local int __at_cmd_result_wait (struct awbl_gprs_dev *p_this,
                                   char                 *p_str,
                                   uint32_t              ms_timeout)
{
    char *arg[1];

    arg[0] = p_str;

    if (ms_timeout == 0) {
        ms_timeout = 5000;
    }
    if (p_this->at_pipe == NULL) {
        return -1;
    }
    return p_this->drvopr.pfn_at_recv(p_this,
                                      p_this->at_buf,
                                      p_this->at_bufsz,
                                      arg, 1, ms_timeout);
}

static aw_err_t __ll_type_get (struct awbl_gprs_dev *p_gprs)
{
    p_gprs->ll_type = AWBL_GPRS_LL_TYPE_PPP;
    return AW_OK;
}


static aw_err_t __cgatt_set (struct awbl_gprs_dev *p_this)
{
    aw_err_t         err;

    __at_cmd_send(p_this, "AT+ZPAS?\r");
    if (__at_cmd_result_wait(p_this, "+ZPAS:", 10000) != AW_OK) {
        return (-AW_GPRS_ECGATT);
    }

#if 0
    __at_cmd_send(p_this, "AT+CGATT?\r");
    __at_cmd_result_wait(p_this, "OK", 15000);

    __at_cmd_send(p_this, "AT+CGATT=1\r");
    if (__at_cmd_result_wait(p_this, "OK", 15000) != AW_OK) {
        __RETURN_EMSG(-AW_GPRS_ECGATT, NULL);
    }

    for (int n = 0; n < 0; n++) {
        __at_cmd_send(p_this, "AT+CGDCONT?\r");
        __at_cmd_result_wait(p_this, "OK", 0);
        __at_cmd_send(p_this, "AT+CGPADDR?\r");
        __at_cmd_result_wait(p_this, "OK", 0);
        aw_mdelay(10000);
    }
#endif
    return AW_OK;
}


static aw_err_t __sms_text_send (struct awbl_gprs_dev   *p_this,
                                 uint8_t                *p_sms,
                                 size_t                  len,
                                 char                   *p_num)
{
    char        buf[370] = { 0 };

    aw_assert(p_num && p_sms && len);

    /* Ӣ�Ķ�����󳤶� */
    if (len >= 160) {
        return -AW_EMSGSIZE;
    }

    if (p_num[0] == '+') {
        p_num += 3;
    }
    sprintf(buf, "AT+CMGS=%s\r", p_num);             /* ����绰���� */

    __at_cmd_send(p_this, buf);
    if (__at_cmd_result_wait(p_this, "\r\n>", 5000) != AW_OK) {
        __RETURN_EMSG(-AW_GPRS_ESMSI, NULL);
    }

    aw_mdelay(1000);
    __at_cmd_send(p_this, (char *)p_sms);                 /* ����������� */
    buf[0] = 0x1a;                                        /* ������ ctrl+z */
    buf[1] = 0;
    __at_cmd_send(p_this, buf);
    if (__at_cmd_result_wait(p_this, "OK", 180000) != AW_OK) {
        /* ����һ��ʧ�ܣ�����Ƿ�� */
        __RETURN_EMSG(-AW_GPRS_ESMSI, NULL);
    }
    return AW_OK;
}

static aw_err_t __sms_pdu_send (struct awbl_gprs_dev   *p_this,
                                char                   *pdu,
                                size_t                  len)
{
    aw_const char GPRS_MSG_END[]={0x1A,0x00};        /*  ���Ž��� */

    aw_assert(pdu && len);

    /* ��������Ƿ�Ӧ��Ϊ�������ݳ��ȣ�������Ϣ���ĺ��� Service Center Address��*/
    sprintf(p_this->at_buf, "AT+CMGS=%d\r", len/2 - 1);
    __at_cmd_send(p_this, p_this->at_buf);
    if (__at_cmd_result_wait(p_this, "\r\n>", 5000) != AW_OK) {
        __RETURN_EMSG(-AW_GPRS_ESMSI, NULL);
    }
    strcat(pdu, GPRS_MSG_END);
#if 0
    p_this->drvopr.pfn_pipe_send(p_this->at_pipe, pdu, len);
#else
    aw_mdelay(1000);
    __at_cmd_send(p_this, (char *)pdu);
#endif
    if (__at_cmd_result_wait(p_this, "OK", 180000) != AW_OK) {
        __RETURN_EMSG(-AW_GPRS_ESMSI, NULL);
    }
    return AW_OK;
}

static aw_err_t __cgdcont_set (struct awbl_gprs_dev *p_this)
{
    char *buf  = p_this->at_buf;

#if 0
    __at_cmd_send(p_this, "AT+CGDCONT?\r");
    __at_cmd_result_wait(p_this, "OK", 0);
    __at_cmd_send(p_this, "AT+CGPADDR?\r");
    __at_cmd_result_wait(p_this, "OK", 0);
#endif

    sprintf(buf, "AT+CGDCONT=1,\"IP\",\"%s\"\r", p_this->info.p_apn);
    __at_cmd_send(p_this, buf);
    if (__at_cmd_result_wait(p_this, "OK", 8000) != AW_OK) {
        sprintf(buf, "AT+CGDCONT=1,\"IP\",\"%s\"\r", p_this->info.p_apn);
        __at_cmd_send(p_this, buf);
        if (__at_cmd_result_wait(p_this, "OK", 10000) != AW_OK) {
            __RETURN_EMSG(-AW_GPRS_ECGDCONT, NULL);
        }
    }
#if 0
    __at_cmd_send(p_this, "AT+CGDCONT?\r");
    __at_cmd_result_wait(p_this, "OK", 0);
    __at_cmd_send(p_this, "AT+CGPADDR?\r");
    __at_cmd_result_wait(p_this, "OK", 0);
#endif
    return AW_OK;
}


static aw_err_t __gprs_drvopr_init ( struct awbl_gprs_gm510_dev *p_this,
                                     struct awbl_gprs_dev       *p_gprs)
{
    if (!p_gprs->drvopr.pfn_ll_type_get) {
        p_gprs->drvopr.pfn_ll_type_get      = __ll_type_get;
    }
    p_gprs->drvopr.pfn_cgdcont_set          = __cgdcont_set;
    p_gprs->drvopr.pfn_cgatt_set            = __cgatt_set;
    p_gprs->drvopr.pfn_sms_text_send        = __sms_text_send;
    p_gprs->drvopr.pfn_sms_pdu_send         = __sms_pdu_send;
    return AW_OK;
}

/** ��ӵ�GPRS����*/
aw_local aw_err_t __gm510_dev_get (struct awbl_dev *p_dev, void *p_arg)
{
    __GM510_DEV_DECL(p_this, p_dev);

    struct awbl_gprs_dev *p_gprs = NULL;
    /* �Ϸ��Լ��*/
    if (!p_dev) {
        return -AW_EINVAL;
    }
    /* ��ȡGPRS�ṹ��*/
    p_gprs = &p_this->gprs;
    /* ��ʼ��������*/
    AW_MUTEX_INIT(p_gprs->mutex, AW_SEM_Q_PRIORITY);
    /* ��ʼ��GPRS�ṹ���Ա*/
    p_gprs->p_gprs_dev  = (void *)p_this;
    __gprs_drvopr_init(p_this, p_gprs);

    /* �ѳ�ʼ�����GPRS�ṹ�巵�ظ�GPRS����*/
    *(struct awbl_gprs_dev **)p_arg = p_gprs;
    AW_INFOF(("%s driver register.\n", p_gprs->info.devname));

    return AW_OK;
}

/** ������ʼ������*/
aw_local aw_const struct awbl_drvfuncs __g_drvfuncs = {
    NULL,
    NULL,
    NULL,
};

/** GPRS����METHOD*/
AWBL_METHOD_IMPORT(awbl_gprs_dev_get);

aw_local aw_const struct awbl_dev_method __g_gprs_gm510_methods[] = {
    AWBL_METHOD(awbl_gprs_dev_get, __gm510_dev_get),
    AWBL_METHOD_END
};

/** ������Ϣ*/
aw_local aw_const awbl_plb_drvinfo_t __g_gm510_drv_registration = {
    {
        AWBL_VER_1,                         /* awb_ver */
        AWBL_BUSID_PLB,                     /* bus_id */
        AWBL_GPRS_GM510_NAME,                /* p_drvname */
        &__g_drvfuncs,                      /* p_busfuncs */
        &__g_gprs_gm510_methods[0],          /* p_methods */
        NULL                                /* pfunc_drv_probe */
    }
};


/** GM510 4G����ע�ắ��*/
void awbl_gprs_gm510_drv_register (void)
{
    int ret;
    ret = awbl_drv_register((struct awbl_drvinfo *)&__g_gm510_drv_registration);
    if (ret != AW_OK) {
        return;
    }
}
