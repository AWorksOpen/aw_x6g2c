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
#ifndef __USB_MEM_H
#define __USB_MEM_H


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_aw_if_usb_mem
 * \copydoc aw_usb_mem.h
 * @{
 */

/** \brief USB DMA���� */
#define USB_DMA_TO_DEVICE    0x01
#define USB_DMA_FROM_DEVICE  0x02

int usb_mem_sta (void);
/**
 * \brief USB�ڴ��ʼ���ӿ�
 *
 * \param[in] p_mem ���ʼ���ڴ��ַ
 * \param[in] size  �ڴ��ֽڴ�С
 *
 * \return ��
 */
void usb_mem_lib_init (void *p_mem, size_t size);

/**
 * \brief ����USBʹ�õĶ�����ڴ�
 *
 * \param[in] size  �����ֽڳ���
 * \param[in] align �����ֽڳ���
 *
 * \return ������ڴ���ʼ��ַ �� NULL
 */
void *usb_mem_align_alloc (size_t size, size_t align);

/**
 * \brief ����USBʹ�õ��ڴ�
 *
 * \param[in] size  �����ֽڳ���
 *
 * \return ������ڴ���ʼ��ַ �� NULL
 */
void *usb_mem_alloc (size_t size);

/**
 * \brief �ͷ�USBʹ�õ��ڴ�
 *
 * \param[in] ptr �ڴ���ʼ��ַ
 *
 * \return ��
 */
void usb_mem_free (void *ptr);


/**
 * \brief �����շ�ǰmap����
 *
 * \param[in] p_mem �ڴ���ʼ��ַ
 * \param[in] size  �ڴ��С
 * \param[in] dir   ���ݴ��䷽��
 *
 * \return �ڴ���ʼ��ַ
 */
void *usb_dma_map (void *p_mem, size_t size, uint8_t dir);


/**
 * \brief �����շ���ɺ�unmap����
 *
 * \param[in] p_mem �ڴ���ʼ��ַ
 * \param[in] size  �ڴ��С
 * \param[in] dir   ���ݴ��䷽��
 *
 * \return �ڴ���ʼ��ַ
 */
void *usb_dma_unmap (void *p_dma, size_t size, uint8_t dir);

/**
 * \brief ���������ڴ�
 *
 * \param[in] nbytes �ڴ��С
 * \param[in] align  ������
 *
 * \return �ڴ���ʼ��ַ
 */
void *usb_cache_dma_align (size_t nbytes, size_t align);

/**
 * \brief �ͷŶ����ڴ�
 *
 * \param[in] ptr �ڴ���ʼ��ַ
 */
void usb_cache_dma_free (void *ptr);

/** @} grp_aw_if_usb_mem */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AW_USB_MEM_H */

/* end of file */
