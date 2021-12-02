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
#include "aw_list.h"
#include "aw_vdebug.h"
#include "aw_spinlock.h"
#include "aw_int.h"
#include "host/controller/ehci/awbl_usbh_ehci.h"
#include "host/controller/ehci/usbh_ehci.h"
#include <string.h>


/* ˵����Ϊ�˷�ֹUSB�ж϶�ʧ�������˶�ʱ��ȥ��ѯEHCI���첽���ȵ�QH�Ƿ�������*/
/**
 * EHCI�ػ���ʱ���ص�����
 *
 * @param p_arg �ص���������(���ﴫ��EHCI�ṹ��)
 */
aw_local void __guarder_handle (void *p_arg)
{
    struct awbl_usbh_ehci *p_aw_ehci = (struct awbl_usbh_ehci *)p_arg;
    /* �ͷ�EHCI�ػ��¼��ź���*/
    usbh_ehci_guard_evt_start(&p_aw_ehci->p_echi);
    /* ����EHCI��ʱ��(100ms��ʱ)*/
    aw_timer_start(&p_aw_ehci->guarder, 100);
}

/**
 * USB EHCI��ʼ������
 *
 * @param p_ehci        EHCI�ṹ��
 * @param reg_base      USB���ܼĴ�������ַ
 * @param task_prio     EHCI�������ȼ�
 * @param pfn_rh_change ���������仯�ص�����
 * @param ps_bit        �˿��ٶ�λ
 * @param nqhs          QH����
 * @param nqtds         qTD����
 * @param nitds         iTD����
 * @param nsitds        siTD����
 *
 * @return �ɹ�����AW_OK
 */
aw_err_t awbl_usbh_ehci_init (struct usbh           *p_hc,
                              struct awbl_usbh_ehci *p_aw_ehci,
                              uint32_t               reg_base,
                              int                    task_prio,
                              uint8_t                ps_bit,
                              int                    nqhs,
                              int                    nqtds,
                              int                    nitds,
                              int                    nsitds){
    aw_err_t ret;
    /* ��ʼ��һ���ػ���ʱ��*/
    aw_timer_init(&p_aw_ehci->guarder,
                  __guarder_handle,
                  (void *)p_aw_ehci);
    /* ��ʼ��������*/
    aw_spinlock_isr_init(&p_aw_ehci->spinlock, 0);
    /* ��ʼ��EHCI*/
    ret = usbh_ehci_init(p_hc, &p_aw_ehci->p_echi, reg_base, task_prio,
            ps_bit, nqhs, nqtds, nitds, nsitds);
    if(ret == AW_OK){
        /* ����EHCI�ػ���ʱ��(500ms��ʱ)*/
        aw_timer_start(&p_aw_ehci->guarder, 500);
    }
    return ret;
}
