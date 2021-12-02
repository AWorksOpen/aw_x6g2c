/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2013 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief imx6ul gpio driver
 *
 * this driver is the implementation of aw_gpio.h for imx28x
 *
 * \internal
 * \par modification history:
 * - 1.00 2015-10-20, jac, first implementation.
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/

#include "apollo.h"

#include <string.h>

#include "aw_spinlock.h"
#include "aw_int.h"
#include "aw_gpio.h"
#include "aw_assert.h"
#include "aw_vdebug.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "awbl_gpio.h"
#include "imx6ul_inum.h"
#include "driver/gpio/awbl_imx6ul_gpio.h"
#include "driver/gpio/awbl_imx6ul_gpio_private.h"
#include "aw_clk.h"
#include "imx6ul_iomuxc_regs.h"
#include "imx6ul_pin_cfg_map.h"

/* is pin valid */
#define __GPIO_PIN_IS_VALID(pin) \
    (((pin) >= 0) && ((pin) < (IMX6UL_PORT_PINS*IMX6UL_GPIO_PORTS) ))

/* device lock */
aw_local aw_spinlock_isr_t __g_lock_infomap = AW_SPINLOCK_ISR_UNLOCK;

/* lock this device */
aw_local void gpio_dev_lock(void)
{
    aw_spinlock_isr_take(&__g_lock_infomap);
}

/* unlock this device */
aw_local void gpio_dev_unlock(void)
{
    aw_spinlock_isr_give(&__g_lock_infomap);
}

/**
 * \addtogroup grp_imx6ul_bsp IMX6UL BSP֧��
 * @{
 */
/**
 * \addtogroup grp_awbus_lite_driver AWbus lite����
 * @{
 */
/**
 * \addtogroup grp_awbl_drv_imx6ul_gpio IMX6UL GPIO����
 * \copydetails awbl_imx6ul_gpio.h
 * @{
 */

/**
 * \addtogroup grp_awbl_drv_imx6ul_gpio_implent GPIO�����ؼ�����
 * @{
 */
/**
 * \addtogroup grp_awbl_drv_imx6ul_gpio_interface GPIO�����ӿ�
 * @{
 */

/**
 * \brief GPIO��������
 *
 * aw_gpio_pin_cfg()������ʱʵ��
 * \param   p_cookie    ����ʱ�������ı�����ǰ�豸��ֵ
 * \param   pin         Ҫ��ֵ�����ű��
 * \param   flags       Ҫ���õ����Ա�־���ο�aw gpioģ������ȡ��Ч�Ĳ���ֵ
 * \return  ʧ�ܷ��ظ��Ĵ����룬���򷵻�AW_OK
 */
aw_local aw_err_t __gpio_pin_cfg (void *p_cookie, int pin, uint32_t flags)
{
    struct awbl_imx6ul_gpio_dev  *p_gpio_dev = awbldev_to_imx6ul_gpio_dev(p_cookie);
    uint32_t                      func       = AW_GPIO_FUNCBITS_GET(flags);
    uint32_t                      mode       = AW_GPIO_MODEBITS_GET(flags);

    uint32_t                      mux     = IMX6UL_PIN_MUX_GET(flags);
    uint32_t                      pad_ctl = IMX6UL_PAD_CTL_GET(flags);

    uint32_t                      port    = pin / IMX6UL_PORT_PINS;
    uint32_t                      index   = pin % IMX6UL_PORT_PINS;

    aw_err_t                      err = AW_OK;

    imx6ul_gpio_bank_regs_t      *p_bank = GET_GPIO_BANK_REGS(p_gpio_dev->reg,port);
    uint32_t                     *p_iomuxc_regs;
    aw_const imx6ul_pin_cfg_map_t*p_cfg_map;
    uint32_t                      temp;

    if ( !__GPIO_PIN_IS_VALID(pin)) {
        err = -AW_EINVAL;
        goto cleanup;
    }
    /* plfm �������� */

    /* MUX_CTL_PAD  */
    gpio_dev_lock();
    if (IS_PIN_MUX_SET_ENABLE(flags)) {
        p_cfg_map = _imx6ul_get_pin_cfg_map(pin);
        p_iomuxc_regs = (uint32_t *)(p_cfg_map->mux_ctl_offset
                         + p_gpio_dev->iomuxc_base);

        AW_REG_WRITE32(p_iomuxc_regs, mux);
        mux &= (~PIN_MUX_SI_ON);

        //�޸����Ÿ���֮�󣬿�����Ҫ�޸�SELECT_INPUT
        temp = p_cfg_map->select_input_cfgs[mux].select_input_offset;
        if(0 != temp) {
            p_iomuxc_regs = (uint32_t *)(temp
                + p_gpio_dev->iomuxc_base);
            temp = p_cfg_map->select_input_cfgs[mux].select_input_val;
            AW_REG_WRITE32(p_iomuxc_regs,temp);
        }
    }

    /* PAD_CTL_PAD */
    if (IS_PAD_CTL_SET_ENABLE(flags)) {
        p_cfg_map = _imx6ul_get_pin_cfg_map(pin);
        p_iomuxc_regs = (uint32_t *)(p_cfg_map->pad_ctl_offset
                        + p_gpio_dev->iomuxc_base);

        AW_REG_WRITE32(p_iomuxc_regs, pad_ctl);
    }

    /* GPIO general function */
    if (AW_GPIO_FUNC_INVALID_VAL != func) {

        /* Set the MUX to ALT5(GPIO) */
        p_cfg_map = _imx6ul_get_pin_cfg_map(pin);
        p_iomuxc_regs = (uint32_t *)(p_cfg_map->mux_ctl_offset
                         + p_gpio_dev->iomuxc_base);

        /* bit0 ~ bit3, MUX_MODE */
        AW_REG_BITS_SET32(p_iomuxc_regs, 0, 4, PIN_MUX_ALT5);

        switch (func) {

        case AW_GPIO_INPUT_VAL:
            AW_REG_BIT_CLR32(&p_bank->dir,index);
            break;
        case AW_GPIO_OUTPUT_VAL:
            AW_REG_BIT_SET32(&p_bank->dir,index);
            break;
        case AW_GPIO_OUTPUT_INIT_HIGH_VAL:
            AW_REG_BIT_SET32(&p_bank->data,index);
            AW_REG_BIT_SET32(&p_bank->dir,index);
            break;
        case AW_GPIO_OUTPUT_INIT_LOW_VAL:
            AW_REG_BIT_CLR32(&p_bank->data,index);
            AW_REG_BIT_SET32(&p_bank->dir,index);
            break;
        default:
            break;
        }
    }

    /* General mode configuration */
    if (AW_GPIO_MODE_INVALID_VAL != mode) {

        pad_ctl = 0;
        p_cfg_map = _imx6ul_get_pin_cfg_map(pin);
        p_iomuxc_regs = (uint32_t *)(p_cfg_map->pad_ctl_offset
                         + p_gpio_dev->iomuxc_base);

        pad_ctl = AW_REG_READ32(p_iomuxc_regs);

        if (AW_GPIO_PULL_UP_VAL == mode) {

            pad_ctl |= PKE_1_Pull_Keeper_Enabled | PUE_1_Pull;
            pad_ctl &= ~(PUS_3_22K_Ohm_Pull_Up);

            /* The default pull up is 47K ohm */
            pad_ctl |= PUS_1_47K_Ohm_Pull_Up;

        } else if (AW_GPIO_PULL_DOWN_VAL == mode) {

            pad_ctl |= PKE_1_Pull_Keeper_Enabled | PUE_1_Pull;
            pad_ctl &= ~(PUS_3_22K_Ohm_Pull_Up);
            pad_ctl |= PUS_0_100K_Ohm_Pull_Down;

        } else if (AW_GPIO_FLOAT_VAL == mode) {

            pad_ctl &= ~(PKE_1_Pull_Keeper_Enabled);

        } else if (AW_GPIO_OPEN_DRAIN_VAL == mode) {

            pad_ctl |= Open_Drain_Enabled;
        }

        AW_REG_WRITE32(p_iomuxc_regs, pad_ctl);
    }

    gpio_dev_unlock();

cleanup:
    return  err;
}


/**
 * \brief implementation the function of aw_gpio_pin_request()
 */
aw_local aw_err_t __gpio_pin_request (void       *p_cookie,
                                      const char *p_name,
                                      int         pin)
{
    struct awbl_imx6ul_gpio_dev     *p_gpio_dev = awbldev_to_imx6ul_gpio_dev(p_cookie);
    struct awbl_imx6ul_gpio_info *p_devinfo = \
        (struct awbl_imx6ul_gpio_info *)AWBL_DEVINFO_GET(p_gpio_dev);

    if ( !__GPIO_PIN_IS_VALID(pin)) {
        return -AW_EIO;
    }

    gpio_dev_lock();

    if (p_devinfo->p_pin_cfg != NULL) {

        /* �������Ѿ���ռ�� */
        if (p_devinfo->p_pin_cfg[pin / 8] & AW_BIT(pin % 8)) {
            gpio_dev_unlock();
            aw_kprintf("%d pin is occupied.\r\n",pin);
            return -AW_EPERM;
        }

        p_devinfo->p_pin_cfg[pin / 8] |= AW_BIT(pin % 8);
    }

    gpio_dev_unlock();
    return AW_OK;
}


/**
 * \brief implementation the function of aw_gpio_pin_release()
 */
aw_local aw_err_t __gpio_pin_release (void *p_cookie, int pin)
{
    struct awbl_imx6ul_gpio_dev     *p_gpio_dev = awbldev_to_imx6ul_gpio_dev(p_cookie);
    struct awbl_imx6ul_gpio_info *p_devinfo = \
        (struct awbl_imx6ul_gpio_info *)AWBL_DEVINFO_GET(p_gpio_dev);

    if ( !__GPIO_PIN_IS_VALID(pin)) {
        return -AW_EIO;
    }

    gpio_dev_lock();
    if (p_devinfo->p_pin_cfg != NULL) {
        p_devinfo->p_pin_cfg[pin / 8] &= (~AW_BIT(pin % 8));
    }
    gpio_dev_unlock();

    return AW_OK;
}


/**
 * \brief ��ȡGPIO���ŵĵ�ǰ����
 *
 * aw_gpio_get()������ʱʵ��
 * \param   p_cookie    ����ʱ�������ı�����ǰ�豸��ֵ
 * \param   pin         ���ű��
 * \return  ����Ϊ�߷���1���ͷ���0��ʧ�ܷ��ظ��Ĵ�����
 */
aw_local aw_err_t __gpio_get (void *p_cookie, int pin)
{
    struct awbl_imx6ul_gpio_dev     *p_gpio_dev = awbldev_to_imx6ul_gpio_dev(p_cookie);
    uint32_t                        port = pin / IMX6UL_PORT_PINS;
    uint32_t                        index = pin % IMX6UL_PORT_PINS;
    imx6ul_gpio_bank_regs_t         *p_bank = GET_GPIO_BANK_REGS(p_gpio_dev->reg,port);
    aw_err_t                        err = AW_OK;
    uint32_t  dir   = 0;
    uint32_t  val   = 0;

    if ( !__GPIO_PIN_IS_VALID(pin)) {
        err = -AW_EINVAL;
        goto cleanup;
    }

    dir = AW_REG_READ32(&p_bank->dir);
    dir = ((dir >> index) & 0x01);

    if (dir == 0x01) {
        val = AW_REG_READ32(&p_bank->data);
    }
    else {
        val = AW_REG_READ32(&p_bank->psr);
    }

    val = ((val >> index) & 0x01);

//    var = AW_REG_BIT_GET32(&p_bank->psr, index);
    err = val;
cleanup:
    return err;
}

/**
 * \brief ����ĳ��gpio���ŵ����ֵ
 *
 * aw_gpio_set()������ʱʵ��
 * \param   p_cookie    ����ʱ�������ı�����ǰ�豸��ֵ
 * \param   pin         ���ű��
 * \param   value       0����ͣ����������
 * \return  ʧ�ܷ��ظ��Ĵ����룬���򷵻�AW_OK
 */
aw_local aw_err_t __gpio_set (void *p_cookie, int pin, int value)
{
    struct awbl_imx6ul_gpio_dev     *p_gpio_dev = awbldev_to_imx6ul_gpio_dev(p_cookie);
    uint32_t                        port = pin / IMX6UL_PORT_PINS;
    uint32_t                        index = pin % IMX6UL_PORT_PINS;
    imx6ul_gpio_bank_regs_t         *p_bank = GET_GPIO_BANK_REGS(p_gpio_dev->reg,port);
    aw_err_t                        err = AW_OK;

    if ( !__GPIO_PIN_IS_VALID(pin)) {
        err = -AW_EINVAL;
        goto cleanup;
    }

    gpio_dev_lock();
    if (value) {
        AW_REG_BIT_SET32(&p_bank->data,index);
    } else {
        AW_REG_BIT_CLR32(&p_bank->data,index);
    }
    gpio_dev_unlock();

cleanup:
    return err;
}

/**
 * \brief ��תĳ��gpio���ŵ����ֵ
 *
 * aw_gpio_toggle()������ʱʵ��
 * \param   p_cookie    ����ʱ�������ı�����ǰ�豸��ֵ
 * \param   pin         ���ű��
 * \return  ʧ�ܷ��ظ��Ĵ����룬���򷵻�AW_OK
 */
aw_local aw_err_t __gpio_toggle (void *p_cookie, int pin)
{
    struct awbl_imx6ul_gpio_dev     *p_gpio_dev = awbldev_to_imx6ul_gpio_dev(p_cookie);
    uint32_t                        port = pin / IMX6UL_PORT_PINS;
    uint32_t                        index = pin % IMX6UL_PORT_PINS;
    imx6ul_gpio_bank_regs_t         *p_bank = GET_GPIO_BANK_REGS(p_gpio_dev->reg,port);
    aw_err_t                        err = AW_OK;
    uint32_t  dir   = 0;
    uint32_t  val   = 0;

    if ( !__GPIO_PIN_IS_VALID(pin)) {
        err = -AW_EINVAL;
        goto cleanup;
    }

    gpio_dev_lock();
    writel((readl(&p_bank->data) ^ (1u << index)), &p_bank->data);
    gpio_dev_unlock();

    dir = AW_REG_READ32(&p_bank->dir);
    dir = ((dir >> index) & 0x01);

    if (dir == 0x01) {
        val = AW_REG_READ32(&p_bank->data);
    }
    else {
        val = AW_REG_READ32(&p_bank->psr);
    }

    val = ((val >> index) & 0x01);

    err = val;

cleanup:
    return err;
}

/**
 * \brief ����GPIO���ŵĴ���(�ж�)
 *
 * aw_gpio_trigger_cfg()������ʱʵ��
 * \param   p_cookie    ����ʱ�������ı�����ǰ�豸��ֵ
 * \param   pin         ���ű��
 * \param   flags       �жϴ����ı�־���ο�aw_gpio.h��ȡ��Ч�ı�־
 * \return  ʧ�ܷ��ظ��Ĵ����룬���򷵻�AW_OK
 */
aw_local aw_err_t __gpio_trigger_cfg(void *p_cookie, int pin, uint32_t flags)
{
    struct awbl_imx6ul_gpio_dev     *p_gpio_dev = awbldev_to_imx6ul_gpio_dev(p_cookie);
    uint32_t                        port = pin / IMX6UL_PORT_PINS;
    uint32_t                        index = pin % IMX6UL_PORT_PINS;
    imx6ul_gpio_bank_regs_t         *p_bank = GET_GPIO_BANK_REGS(p_gpio_dev->reg,port);
    aw_err_t                        err = AW_OK;
    uint32_t                        val;
    volatile uint32_t *             p_icr;
    uint32_t                        val_table[4] = {0x1,0x0,0x2,0x3};

    if ( !__GPIO_PIN_IS_VALID(pin)) {
        err = -AW_EINVAL;
        goto cleanup;
    }

    if ((flags != AW_GPIO_TRIGGER_FALL)        &&
        (flags != AW_GPIO_TRIGGER_RISE)        &&
        (flags != AW_GPIO_TRIGGER_HIGH)        &&
        (flags != AW_GPIO_TRIGGER_LOW) ) {
        err  = -AW_ENOTSUP;
        goto cleanup;
    }

    val = val_table[flags-1];
    if(index <=15 ) {
        p_icr = &p_bank->icr1;
    }
    else {
        p_icr = &p_bank->icr2;
    }
    index %= 16;

    gpio_dev_lock();

    /* set pin's trigger type */
    uint32_t value = readl((volatile void __iomem *)(p_icr));
    AW_BITS_SET(value, index*2, 2, val );
    writel(value, (volatile void __iomem *)(p_icr));

    gpio_dev_unlock();
cleanup:
    return err;
}

/**
 * \brief ��GPIO���ŵĴ�������
 *
 * aw_gpio_trigger_on()������ʱʵ��
 * \param   p_cookie    ����ʱ�������ı�����ǰ�豸��ֵ
 * \param   pin         ���ű��
 * \return  ʧ�ܷ��ظ��Ĵ����룬���򷵻�AW_OK
 */
aw_local aw_err_t __gpio_trigger_on (void *p_cookie, int pin)
{
    struct awbl_imx6ul_gpio_dev     *p_gpio_dev = awbldev_to_imx6ul_gpio_dev(p_cookie);
    uint32_t                        port = pin / IMX6UL_PORT_PINS;
    uint32_t                        index = pin % IMX6UL_PORT_PINS;
    imx6ul_gpio_bank_regs_t         *p_bank = GET_GPIO_BANK_REGS(p_gpio_dev->reg,port);
    aw_err_t                        err = AW_OK;

    if ( !__GPIO_PIN_IS_VALID(pin)) {
        err = -AW_EINVAL;
        goto cleanup;
    }

    gpio_dev_lock();
    /* clear the trigger status */
    AW_REG_BIT_SET32(&p_bank->isr, index);
    /* pin's trigger on */
    AW_REG_BIT_SET32(&p_bank->imr,index );
    gpio_dev_unlock();;

cleanup:
    return err;
}

/**
 * \brief �ر�GPIO���ŵĴ�������
 *
 * aw_gpio_trigger_off()������ʱʵ��
 * \param   p_cookie    ����ʱ�������ı�����ǰ�豸��ֵ
 * \param   pin         ���ű��
 * \return  ʧ�ܷ��ظ��Ĵ����룬���򷵻�AW_OK
 */
aw_local aw_err_t __gpio_trigger_off (void *p_cookie, int pin)
{
    struct awbl_imx6ul_gpio_dev     *p_gpio_dev = awbldev_to_imx6ul_gpio_dev(p_cookie);
    uint32_t                        port = pin / IMX6UL_PORT_PINS;
    uint32_t                        index = pin % IMX6UL_PORT_PINS;
    imx6ul_gpio_bank_regs_t         *p_bank = GET_GPIO_BANK_REGS(p_gpio_dev->reg,port);
    aw_err_t                        err = AW_OK;

    if ( !__GPIO_PIN_IS_VALID(pin)) {
        err = -AW_EINVAL;
        goto cleanup;
    }

    gpio_dev_lock();
    /* pin's trigger off */
    AW_REG_BIT_CLR32(&p_bank->imr,index );
    /* clear the trigger status */
    AW_REG_BIT_SET32(&p_bank->isr,index );
    gpio_dev_unlock();;

cleanup:
    return err;
}

/**
 * \brief ����GPIO���ŵĴ�������
 *
 * aw_gpio_trigger_connect()������ʱʵ��
 * \param   p_cookie    ����ʱ�������ı�����ǰ�豸��ֵ
 * \param   pin         ���ű��
 * \param   pfunc_callback �����ж�ʱ�Ļص�����
 * \param   p_arg        �����жϻص������Ĳ���
 * \return  ʧ�ܷ��ظ��Ĵ����룬���򷵻�AW_OK
 */
aw_local aw_err_t __gpio_trigger_connect (void           *p_cookie,
                                          int             pin,
                                          aw_pfuncvoid_t  pfunc_callback,
                                          void           *p_arg)
{
    struct awbl_imx6ul_gpio_dev     *p_gpio_dev = awbldev_to_imx6ul_gpio_dev(p_cookie);
    uint32_t                        port = pin / IMX6UL_PORT_PINS;
    uint32_t                        index = pin % IMX6UL_PORT_PINS;
    aw_err_t                        err = AW_OK;

    if ( !__GPIO_PIN_IS_VALID(pin)) {
        err = -AW_EINVAL;
        goto cleanup;
    }

    gpio_dev_lock();
    if (p_gpio_dev->trig_info[port][index].pfunc_callback != NULL) {
        gpio_dev_unlock();
        err = -AW_ENOSPC; /* Must Disconnect Old Isr First */
        goto cleanup;
    }

    p_gpio_dev->trig_info[port][index].pfunc_callback = pfunc_callback;
    p_gpio_dev->trig_info[port][index].p_arg          = p_arg;

    gpio_dev_unlock();;

cleanup:
    return err;
}

/**
 * \brief �Ͽ�GPIO���ŵĴ���
 *
 * aw_gpio_trigger_disconnect()������ʱʵ��
 * \param   p_cookie    ����ʱ�������ı�����ǰ�豸��ֵ
 * \param   pin         ���ű��
 * \param   pfunc_callback �����ж�ʱ�Ļص�����
 * \param   p_arg        �����жϻص������Ĳ���
 * \return  ʧ�ܷ��ظ��Ĵ����룬���򷵻�AW_OK
 */
aw_local aw_err_t __gpio_trigger_disconnect(void            *p_cookie,
                                            int              pin,
                                            aw_pfuncvoid_t   pfunc_isr,
                                            void            *p_arg)
{
    struct awbl_imx6ul_gpio_dev     *p_gpio_dev = awbldev_to_imx6ul_gpio_dev(p_cookie);
    uint32_t                        port = pin / IMX6UL_PORT_PINS;
    uint32_t                        index = pin % IMX6UL_PORT_PINS;
    aw_err_t                        err = AW_OK;

    if ( !__GPIO_PIN_IS_VALID(pin)) {
        err = -AW_EINVAL;
        goto cleanup;
    }

    gpio_dev_lock();
    if (NULL == p_gpio_dev->trig_info[port][index].pfunc_callback ) {
        gpio_dev_unlock();
        err = -AW_ENOSPC;  /* no isr present */
        goto cleanup;
    }

    p_gpio_dev->trig_info[port][index].pfunc_callback = NULL;
    p_gpio_dev->trig_info[port][index].p_arg          = NULL;

    gpio_dev_unlock();;

cleanup:
    return err;
}
/** @} grp_awbl_drv_imx6ul_gpio_interface*/

/**
 * \addtogroup grp_awbl_drv_imx6ul_gpio_interrupt GPIO�жϴ���
 * @{
 */

/**
 * \brief GPIO�жϴ�����ĺ���
 *
 * �˺���ͨ������ķ����жϵĶ˿ں������������������ö�Ӧ���жϻص�����
 * \param   p_gpio_dev  ��ǰGPIO�豸
 * \param   port        �����жϵĶ˿�
 * \param   min         �����ж����ŷ�Χ��Сֵ����ЧֵΪ0��16
 * \param   max         �����ж����ŷ�Χ���ֵ����ЧֵΪ15��31
 * \return  ��
 */
aw_local void __gpio_process_int(struct awbl_imx6ul_gpio_dev *p_gpio_dev,
                                 uint32_t port,
                                 uint32_t min,
                                 uint32_t max
                                 )
{
    imx6ul_gpio_bank_regs_t        *p_bank = GET_GPIO_BANK_REGS(p_gpio_dev->reg,port);
    uint32_t                        pin;
    register uint32_t               temp;
    for (pin = min; pin <= max; pin++) {
        if ( AW_REG_BIT_GET32(&p_bank->isr, pin) ) {
            AW_REG_WRITE32(&p_bank->isr,AW_BIT(pin) );
            temp = AW_REG_READ32(&p_bank->imr);
            if( ( temp & AW_BIT(pin) ) &&
                    ( NULL != p_gpio_dev->trig_info[port][pin].pfunc_callback ) ) {
                p_gpio_dev->trig_info[port][pin].pfunc_callback (
                    p_gpio_dev->trig_info[port][pin].p_arg);
            }
        }
    }
}

/**
 * \brief GPIO PORT0 ��0��15λ�жϴ�����
 *
 * \param   p_cookie    �ж���ϵͳ�������ı�־��ǰʧ�ܵ�ֵ
 * \return  ��
 */
aw_local void __gpio_port0_0_15_eint_isr(void *p_cookie)
{
    struct awbl_imx6ul_gpio_dev     *p_gpio_dev = awbldev_to_imx6ul_gpio_dev(p_cookie);
    __gpio_process_int(p_gpio_dev,0,0,15);
}

/**
 * \brief GPIO PORT0 ��16��31λ�жϴ�����
 * \param   p_cookie    �ж���ϵͳ�������ı�־��ǰʧ�ܵ�ֵ
 * \return  ��
 */
aw_local void __gpio_port0_16_31_eint_isr(void *p_cookie)
{
    struct awbl_imx6ul_gpio_dev     *p_gpio_dev = awbldev_to_imx6ul_gpio_dev(p_cookie);
    __gpio_process_int(p_gpio_dev,0,16,31);
}

/**
 * \brief GPIO PORT1 ��0��15λ�жϴ�����
 *
 * \param   p_cookie    �ж���ϵͳ�������ı�־��ǰʧ�ܵ�ֵ
 * \return  ��
 */
aw_local void __gpio_port1_0_15_eint_isr(void *p_cookie)
{
    struct awbl_imx6ul_gpio_dev     *p_gpio_dev = awbldev_to_imx6ul_gpio_dev(p_cookie);
    __gpio_process_int(p_gpio_dev,1,0,15);
}

/**
 * \brief GPIO PORT1 ��16��31λ�жϴ�����
 *
 * \param   p_cookie    �ж���ϵͳ�������ı�־��ǰʧ�ܵ�ֵ
 * \return  ��
 */
aw_local void __gpio_port1_16_31_eint_isr(void *p_cookie)
{
    struct awbl_imx6ul_gpio_dev     *p_gpio_dev = awbldev_to_imx6ul_gpio_dev(p_cookie);
    __gpio_process_int(p_gpio_dev,1,16,31);
}

/**
 * \brief GPIO PORT2 ��0��15λ�жϴ�����
 *
 * \param   p_cookie    �ж���ϵͳ�������ı�־��ǰʧ�ܵ�ֵ
 * \return  ��
 */
aw_local void __gpio_port2_0_15_eint_isr(void *p_cookie)
{
    struct awbl_imx6ul_gpio_dev     *p_gpio_dev = awbldev_to_imx6ul_gpio_dev(p_cookie);
    __gpio_process_int(p_gpio_dev,2,0,15);
}

/**
 * \brief GPIO PORT2��16��31λ�жϴ�����
 *
 * \param   p_cookie    �ж���ϵͳ�������ı�־��ǰʧ�ܵ�ֵ
 * \return  ��
 */
aw_local void __gpio_port2_16_31_eint_isr(void *p_cookie)
{
    struct awbl_imx6ul_gpio_dev     *p_gpio_dev = awbldev_to_imx6ul_gpio_dev(p_cookie);
    __gpio_process_int(p_gpio_dev,2,16,31);
}

/**
 * \brief GPIO PORT3 ��0��15λ�жϴ�����
 *
 * \param   p_cookie    �ж���ϵͳ�������ı�־��ǰʧ�ܵ�ֵ
 * \return  ��
 */
aw_local void __gpio_port3_0_15_eint_isr(void *p_cookie)
{
    struct awbl_imx6ul_gpio_dev     *p_gpio_dev = awbldev_to_imx6ul_gpio_dev(p_cookie);
    __gpio_process_int(p_gpio_dev,3,0,15);
}

/**
 * \brief GPIO PORT3 ��16��31λ�жϴ�����
 *
 * \param   p_cookie    �ж���ϵͳ�������ı�־��ǰʧ�ܵ�ֵ
 * \return  ��
 */
aw_local void __gpio_port3_16_31_eint_isr(void *p_cookie)
{
    struct awbl_imx6ul_gpio_dev     *p_gpio_dev = awbldev_to_imx6ul_gpio_dev(p_cookie);
    __gpio_process_int(p_gpio_dev,3,16,31);
}

/**
 * \brief GPIO PORT4 ��0��15λ�жϴ�����
 *
 * \param   p_cookie    �ж���ϵͳ�������ı�־��ǰʧ�ܵ�ֵ
 * \return  ��
 */
aw_local void __gpio_port4_0_15_eint_isr(void *p_cookie)
{
    struct awbl_imx6ul_gpio_dev     *p_gpio_dev = awbldev_to_imx6ul_gpio_dev(p_cookie);
    __gpio_process_int(p_gpio_dev,4,0,15);
}

/**
 * \brief GPIO PORT4 ��16��31λ�жϴ�����
 *
 * \param   p_cookie    �ж���ϵͳ�������ı�־��ǰʧ�ܵ�ֵ
 * \return  ��
 */
aw_local void __gpio_port4_16_31_eint_isr(void *p_cookie)
{
    struct awbl_imx6ul_gpio_dev     *p_gpio_dev = awbldev_to_imx6ul_gpio_dev(p_cookie);
    __gpio_process_int(p_gpio_dev,4,16,31);
}
/** @} grp_awbl_drv_imx6ul_gpio_interrupt*/

/**
 * \addtogroup grp_awbl_drv_imx6ul_gpio_awbus_interface GPIO����Awbus�ӿ�
 * @{
 */


/**
 * \brief GPIO������ʼ������1
 *
 * \param   p_dev   AWbus������������ǰ�豸
 * \return  ��
 */
aw_local void __gpio_inst_init1 (awbl_dev_t *p_dev)
{
    struct awbl_imx6ul_gpio_dev        *gpio = awbldev_to_imx6ul_gpio_dev(p_dev);
    const struct awbl_imx6ul_gpio_info *info = imx6ul_gpio_get_param(p_dev);

    aw_assert(info->mem_base != 0);

    aw_clk_enable(IMX6UL_CLK_GPIO1);
    aw_clk_enable(IMX6UL_CLK_GPIO2);
    aw_clk_enable(IMX6UL_CLK_GPIO3);
    aw_clk_enable(IMX6UL_CLK_GPIO4);
    aw_clk_enable(IMX6UL_CLK_GPIO5);

    /* platform initialization */
    if (info->plat_init != NULL) {
        info->plat_init();
    }
    gpio->reg = (imx6ul_gpio_regs_t *)(info->mem_base);
    gpio->iomuxc_base = info->iomuxc_base;
    awbl_gpio_init();
}


/**
 * \brief GPIO��������
 */
aw_local aw_const struct awbl_gpio_servfuncs __g_gpio_servfuncs = {
    __gpio_pin_cfg,                 /* pfunc_gpio_pin_cfg */
    __gpio_get,                     /* pfunc_gpio_get */
    __gpio_set,                     /* pfunc_gpio_set */
    __gpio_toggle,                  /* pfunc_gpio_toggle */
    __gpio_trigger_cfg,             /* pfunc_gpio_trigger_cfg */
    __gpio_trigger_connect,         /* pfunc_gpio_trigger_connect */
    __gpio_trigger_disconnect,      /* pfunc_gpio_trigger_disconnect */
    __gpio_trigger_on,              /* pfunc_gpio_trigger_on */
    __gpio_trigger_off,             /* pfunc_gpio_trigger_off */
    __gpio_pin_request,
    __gpio_pin_release

};

/**
 * \brief GPIO�жϷ�������
 */
aw_local aw_const struct awbl_intctlr_servfuncs __g_intctlr_servfuncs = {
    __gpio_trigger_connect,         /* pfunc_int_connect */
    __gpio_trigger_disconnect,      /* pfunc_int_disconnect */
    __gpio_trigger_on,              /* pfunc_int_enable */
    __gpio_trigger_off              /* pfunc_int_disable */
};

/**
 * \brief ��ȡGPIO����ĺ����б�
 *
 * awbl_gpioserv_get������ʱʵ��
 * \param   p_dev   AWbus������������ǰ�豸
 * \param   p_arg   �������GPIO�������б��ָ��
 * \return  ��
 */
aw_local aw_err_t __gpio_gpioserv_get (struct awbl_dev *p_dev, void *p_arg)
{
    struct awbl_imx6ul_gpio_dev        *p_this = awbldev_to_imx6ul_gpio_dev(p_dev);
    const struct awbl_imx6ul_gpio_info *p_info = imx6ul_gpio_get_param(p_dev);
    struct awbl_gpio_service            *p_serv = &p_this->gpio_serv;

    /* initialize the gpio service instance */
    p_serv->p_next      = NULL;
    p_serv->p_servinfo  = &p_info->gpio_info;
    p_serv->p_servfuncs = &__g_gpio_servfuncs;
    p_serv->p_cookie    = (void *)p_this;

    /* send back the service instance */
    *(struct awbl_gpio_service **)p_arg = p_serv;

    return AW_OK;
}


/**
 * \brief ��ȡGPIO�жϷ���ĺ����б�
 *
 * awbl_icserv_get������ʱʵ��
 * \param   p_dev   AWbus������������ǰ�豸
 * \param   p_arg   �������GPIO�жϷ������б��ָ��
 * \return  ��
 */
aw_local aw_err_t __gpio_icserv_get (struct awbl_dev *p_dev, void *p_arg)
{
    struct awbl_imx6ul_gpio_dev        *p_this = awbldev_to_imx6ul_gpio_dev(p_dev);
    const struct awbl_imx6ul_gpio_info *p_devinfo = imx6ul_gpio_get_param(p_dev);
    struct awbl_intctlr_service            *p_serv = &p_this->intctlr_serv;

    /* get intcltr service instance */

    p_serv = &p_this->intctlr_serv;

    /* initialize the gpio service instance */

    p_serv->p_next      = NULL;
    p_serv->p_servinfo  = &p_devinfo->int_info;
    p_serv->p_servfuncs = &__g_intctlr_servfuncs;
    p_serv->p_cookie    = (void *)p_dev;

    /* send back the service instance */

    *(struct awbl_intctlr_service **)p_arg = p_serv;

    return AW_OK;
}

/******************************************************************************/

/**
 * \brief GPIO������ʼ���׶�2
 *
 * �˽׶���Ҫ��ʹ�ܲ�ע����GPIO�˿��ж�
 * \param   p_dev   AWbus������������ǰ�豸
 * \return  ��
 */
aw_local void __gpio_inst_init2(awbl_dev_t *p_dev)
{
    aw_int_connect(INUM_GPIO1_0_15, __gpio_port0_0_15_eint_isr, p_dev);
    aw_int_connect(INUM_GPIO1_16_31, __gpio_port0_16_31_eint_isr, p_dev);
    aw_int_connect(INUM_GPIO2_0_15, __gpio_port1_0_15_eint_isr, p_dev);
    aw_int_connect(INUM_GPIO2_16_31, __gpio_port1_16_31_eint_isr, p_dev);
    aw_int_connect(INUM_GPIO3_0_15, __gpio_port2_0_15_eint_isr, p_dev);
    aw_int_connect(INUM_GPIO3_16_31, __gpio_port2_16_31_eint_isr, p_dev);
    aw_int_connect(INUM_GPIO4_0_15, __gpio_port3_0_15_eint_isr, p_dev);
    aw_int_connect(INUM_GPIO4_16_31, __gpio_port3_16_31_eint_isr, p_dev);
    aw_int_connect(INUM_GPIO5_0_15, __gpio_port4_0_15_eint_isr, p_dev);
    aw_int_connect(INUM_GPIO5_16_31, __gpio_port4_16_31_eint_isr, p_dev);

    aw_int_enable(INUM_GPIO1_0_15);
    aw_int_enable(INUM_GPIO1_16_31);
    aw_int_enable(INUM_GPIO2_0_15);
    aw_int_enable(INUM_GPIO2_16_31);
    aw_int_enable(INUM_GPIO3_0_15);
    aw_int_enable(INUM_GPIO3_16_31);
    aw_int_enable(INUM_GPIO4_0_15);
    aw_int_enable(INUM_GPIO4_16_31);
    aw_int_enable(INUM_GPIO5_0_15);
    aw_int_enable(INUM_GPIO5_16_31);

}

/**
 * \brief GPIO������ʼ���׶�3
 *
 * \param   p_dev   AWbus������������ǰ�豸
 * \return  ��
 */
aw_local void __gpio_inst_connect(awbl_dev_t *p_dev)
{
}


/**
 * \brief GPIO����AWbus��������
 */
aw_local aw_const struct awbl_drvfuncs __g_gpio_drvfuncs = {
    __gpio_inst_init1,
    __gpio_inst_init2,
    __gpio_inst_connect
};

/******************************************************************************/

/* driver methods (must defined as aw_const) */
AWBL_METHOD_IMPORT(awbl_gpioserv_get);
AWBL_METHOD_IMPORT(awbl_icserv_get);
/**
 * \brief GPIO�����豸ר�к���
 */
aw_local aw_const struct awbl_dev_method __g_gpio_dev_methods[] = {
    AWBL_METHOD(awbl_gpioserv_get, __gpio_gpioserv_get),
    AWBL_METHOD(awbl_icserv_get, __gpio_icserv_get),
    AWBL_METHOD_END
};

/**
 * \brief GPIO����ע����Ϣ
 */
aw_local aw_const awbl_plb_drvinfo_t __g_gpio_drv_registration = {
    {
        AWBL_VER_1,                 /* awb_ver */
        AWBL_BUSID_PLB,             /* bus_id */
        AWBL_IMX6UL_GPIO_NAME,      /* p_drvname */
        &__g_gpio_drvfuncs,         /* p_busfuncs */
        &__g_gpio_dev_methods[0],   /* p_methods */
        NULL                        /* pfunc_drv_probe */
    }
};

/******************************************************************************/
void awbl_imx6ul_gpio_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_gpio_drv_registration);
}

/* end of file */


