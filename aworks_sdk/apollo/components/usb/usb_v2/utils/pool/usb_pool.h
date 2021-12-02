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

/**
 * \file
 * \brief �ڴ��
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ� usb_pool.h
 *
 * �ڴ���ṩ��һ�ֿ�����Ч���ڴ����ʽ��ʹ���ڴ�ؼ����˴Ӷ��з����ڴ�Ĵ�����
 * �Ӷ�������Ƶ�����䡢�ͷ��ڴ������µ��ڴ���Ƭ��
 *
 * �ڴ���е����ж��󶼾�����ͬ�Ĵ�С��
 *
 */

#ifndef __USB_POOL_H
#define __USB_POOL_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "usb_errno.h"

/** \brief �ڴ�صĶ��� */
struct usb_pool {

    /** \brief ���п�������� */
    void *p_free;

    /** \brief �ڴ�ص�ԭʼ��� */
    void *p_start;

    /** \brief �ڴ�ع�������һ���ڴ�� */
    void *p_end;

    /** \brief �ڴ�ص�Ԫ�����ֵ */
    size_t item_size;

    /** \brief �ڴ�ص�Ԫ������ */
    size_t item_count;

    /** \brief ʣ����п���� */
    size_t nfree;

    /**
     * \brief ���п�����������
     *
     * ����һ��ͳ���������������ڴ������������ʣ���������ֵ��
     *
     * \sa usb_pool_margin_get()
     */
    size_t nmin;

};

/** \brief �ڴ�� */
typedef struct usb_pool usb_pool_t;

/** \brief �ڴ�ؾ�� */
typedef usb_pool_t *usb_pool_id_t;

/**
 * \brief �ڴ��ʵ����ʼ��
 *
 * \param[in] p_pool       �ڴ��ʵ��ָ��
 * \param[in] p_pool_mem   �ڴ�ص�Ԫʹ�ÿռ����ʼ��ַ
 * \param[in] pool_size    �ڴ�ص�Ԫ�ռ��ܴ�С
 * \param[in] item_size    �����ڴ�ص�Ԫ�ռ��С
 *
 * \return ���ɹ��������ڴ�ؾ������ʧ��ʱ������NULL
 */
usb_pool_id_t usb_pool_init(usb_pool_t *p_pool,
                            void       *p_pool_mem,
                            size_t      pool_size,
                            size_t      item_size);

/**
 * \brief ���ڴ���л�ȡһ����Ԫ
 *
 * \param[in] pool_id      �ڴ�ؾ��
 *
 * \return ���ɹ�������һ���ڴ�ص�Ԫָ�룻��ʧ��ʱ������NULL
 */
void *usb_pool_item_get (usb_pool_id_t pool_id);

/**
 * \brief �ͷ�һ���ڴ�ص�Ԫ
 *
 * \param[in] pool_id  �ڴ�ؾ��
 * \param[in] p_item   �ڴ�ص�Ԫָ��
 *
 * \retval AW_OK       �ͷųɹ�
 * \retval -AW_EFAULT  �ͷ�ʧ��
 */
usb_err_t usb_pool_item_return (usb_pool_id_t pool_id, void *p_item);


/**
 * \brief ��ȡ��ǰ�ڴ�أ������������½�
 *
 * �����鿴�ڴ�ص�ʹ�����������ķ�ֵ��
 *
 * \param[in] pool_id   �ڴ�ؾ��
 *
 * \return �����ڴ�ؽṹ��� \e nmin ��Ա
 */
size_t usb_pool_margin_get (usb_pool_id_t pool_id);

/**
 * \brief ��ȡ�ڴ�ص�Ԫ�ռ��С
 *
 * \param[in] pool_id  �ڴ�ؾ��
 *
 * \return �����ڴ�ص�Ԫ�ռ��С
 */
static inline size_t usb_pool_item_size (usb_pool_id_t pool_id)
{
    return pool_id->item_size;
}



#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __USB_POOL_H */
