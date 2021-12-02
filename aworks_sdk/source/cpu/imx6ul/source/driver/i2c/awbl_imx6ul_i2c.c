/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief Freescale i.MX6ULϵ��I2C����
 *
 * \internal
 * \par modification history:
 *
 * - 2.00 18-11-08  sls, update driver frame
 *
 * - 1.00 16-03-31  hgo, first implemetation
 * \endinternal
 */


/*******************************************************************************
  includes
*******************************************************************************/

#include "AWorks.h"
#include "aw_errno.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "aw_clk.h"
#include "aw_i2c.h"
#include "aw_delay.h"
#include "driver/i2c/awbl_imx6ul_i2c.h"
#include "awbl_i2cbus.h"

/*
 * ����˵��:
 *
 * 1. ��������ʱ��֧�ֶ���������Ϊ�������ٲõ���������ģ�⣨ͬʱ���Ϳ�ʼ�źţ�
 * 2. ��������ʱ��֧���ٲ�ʧ�ܺ��Զ����ԵĹ��ܣ���Ϊ����ٲ�ʧ�ܵ�״̬λ��1������кܶ��֣�1) �������µ��ٲ�ʧ��,
 *    ����ǿ������Ե�, 2) ���������ϳ��ֵĸ���,�����ܷ������ݷ���һ��������ϳ��ָ��Ż��ߴӻ����ֹ�����ɵ��ٲ�
 *    ʧ�ܣ�����Ͳ������ԡ�
 *
 *
 *
 *
 * ���ֺ����˵��
 *
 *  __IMXI2C_PATFORM_BASE_IDEL_TIME: ��ƽ̨��Releaseģʽ�´�STOP ���´�START֮�����̵Ŀ���
 *                                   ʱ��,�û�ͨ��aw_i2c_set_cfg����bus_idle_time,���
 *                                   bus_idle_time < Release�µ���С���������������
 *                                   ������С��������bus_idle_time��ȥ
 *                                   __IMXI2C_PATFORM_BASE_IDEL_TIME��������ʱΪ���ֵ
 *                                   ����������󻯵����Ч�ʣ�ʹ����ʱʱ����ӽ�����ֵ��
 *
 *  __IMXI2C_WAIT_IDEL_TIMEOUT_MS : ��д����߶�������,Ҫ�ȴ����߿���(��ѯ��Ӧ��״̬λ)������һ����ʱ
 *                                  ʱ�䣬����������ʱ����ܳ����������ٲõȴ������ʱ���Ҫ��λ��
 *                                  �豸
 *
 */

/** \brief ƽ̨���ٵ����߿�����ʱʱ��(us),��Release��ʽ,����  */
#define __IMXI2C_PATFORM_BASE_IDEL_TIME    4



/** \brief ��д�����ݺ����ȴ����߿���ʱ��   */
#define __IMXI2C_WAIT_IDEL_TIMEOUT_MS      1000

#define __WRITE                            0
#define __READ                             1

#define __SET_START_FLAG                   0
#define __CLR_START_FLAG                   1

#define __STOPED                           0
#define __STARTED                          1

/*******************************************************************************
  registers offset
*******************************************************************************/
/**
 * i2c registers
 * ����volatile�Ƿ�ֹʹ��writew�ӿڲ���ʱ�����ڴ�˺�ѵ�bug, ��writew���Ѿ�����volatile
 */
typedef struct {
     uint16_t iadr;       /* I2C Address Register           */
     uint16_t reserved0;  /* reserved                       */
     uint16_t ifdr;       /* I2C Frequency Divider Register */
     uint16_t reserved1;  /* reserved                       */
     uint16_t i2cr;       /* I2C Control Register           */
     uint16_t reserved2;  /* reserved                       */
     uint16_t i2sr;       /* I2C Status Register            */
     uint16_t reserved3;  /* reserved                       */
     uint16_t i2dr;       /* I2C Data I/O Register          */
             uint16_t reserved5;  /* reserved                       */
}ambl_imx6ul_i2c_regs_t;
/*******************************************************************************
 registers bit definition Macros
*******************************************************************************/

/* IADR */
#define __IMXI2C_IADR_ADR_MASK               (0x00FEu)
#define __IMXI2C_IADR_ADR_SHFIT              (1u)

/* IFDR */
#define __IMXI2C_IFDR_IC_MASK                (0x003Fu)
#define __IMXI2C_IFDR_IC_SHFIT               (0u)


/* I2CR */
#define __IMXI2C_I2CR_RSTA_MASK              (0x0004u)
#define __IMXI2C_I2CR_RSTA_SHFIT             (2u)
#define __IMXI2C_I2CR_TXAK_MASK              (0x0008u)
#define __IMXI2C_I2CR_TXAK_SHFIT             (3u)
#define __IMXI2C_I2CR_MTX_MASK               (0x0010u)
#define __IMXI2C_I2CR_MTX_SHFIT              (4u)
#define __IMXI2C_I2CR_MSTA_MASK              (0x0020u)
#define __IMXI2C_I2CR_MSTA_SHFIT             (5u)
#define __IMXI2C_I2CR_IIEN_MASK              (0x0040u)
#define __IMXI2C_I2CR_IIEN_SHFIT             (6u)
#define __IMXI2C_I2CR_IEN_MASK               (0x0080u)
#define __IMXI2C_I2CR_IEN_SHFIT              (7u)

/* I2SR */
#define __IMXI2C_I2SR_RXAK_MASK              (0x0001u)
#define __IMXI2C_I2SR_RXAK_SHFIT             (0u)
#define __IMXI2C_I2SR_IIF_MASK               (0x0002u)
#define __IMXI2C_I2SR_IIF_SHFIT              (1u)
#define __IMXI2C_I2SR_SRW_MASK               (0x0004u)
#define __IMXI2C_I2SR_SRW_SHFIT              (2u)
#define __IMXI2C_I2SR_IAL_MASK               (0x0010u)
#define __IMXI2C_I2SR_IAL_SHFIT              (4u)
#define __IMXI2C_I2SR_IBB_MASK               (0x0020u)
#define __IMXI2C_I2SR_IBB_SHFIT              (5u)
#define __IMXI2C_I2SR_IAAS_MASK              (0x0040u)
#define __IMXI2C_I2SR_IAAS_SHFIT             (6u)
#define __IMXI2C_I2SR_ICF_MASK               (0x0080u)
#define __IMXI2C_I2SR_ICF_SHFIT              (7u)

/* I2DR */
#define __IMXI2C_I2DR_DATA_MASK              (0x00FFu)
#define __IMXI2C_I2DR_DATA_SHFIT             (0u)


/*******************************************************************************
  macro operate
*******************************************************************************/
/* get imx6ul i2c hardware infomation */
#define __IMXI2C_HW_DECL(p_hw_i2c, p_devinfo)    \
        ambl_imx6ul_i2c_regs_t *p_hw_i2c =            \
            ((ambl_imx6ul_i2c_regs_t *)((p_devinfo)->regbase))


/* get imx6ul3 i2c device instance */
#define __IMXI2C_DEV_DECL(p_this, p_dev) \
    struct awbl_imx6ul_i2c_dev *p_this = \
        (struct awbl_imx6ul_i2c_dev *)(p_dev)

/* get imx6ul3 i2c device infomation */
#define __IMXI2C_DEVINFO_DECL(p_this_dvif, p_dev) \
    struct awbl_imx6ul_i2c_devinfo *p_this_dvif = \
        (struct awbl_imx6ul_i2c_devinfo *)AWBL_DEVINFO_GET(p_dev)

/* get i2c master instance */
#define __IMXI2C_MASTER_DEV_DECL(p_master, p_dev) \
    struct awbl_i2c_master *p_master = (struct awbl_i2c_master *)(p_dev)


#define __REG_OR_MODIFY32(addr, value) \
        do { \
            uint16_t val = readw((volatile void __iomem *)(addr)); \
            val |= value; \
            writew(val, (volatile void __iomem *)(addr)); \
        } while (0)

#define __REG_AND_MODIFY32(addr, value) \
        do { \
            uint16_t val = readw((volatile void __iomem *)(addr)); \
            val &= value; \
            writew(val, (volatile void __iomem *)(addr)); \
        } while (0)


/*******************************************************************************
  forward declarations
*******************************************************************************/
aw_local void __imxi2c_inst_init1(awbl_dev_t *p_dev);
aw_local void __imxi2c_inst_init2(awbl_dev_t *p_dev);
aw_local void __imxi2c_inst_connect(awbl_dev_t *p_dev);
aw_local void __imxi2c_isr(void *p_arg);
aw_local void __imxi2c_hard_init(struct awbl_imx6ul_i2c_dev  *p_this);


/**
 * \brief divider and IC value table
 * The following table describes the divider and
 * register values for the register IFDR field "IC."
 */
const  uint16_t __g_imxi2c_ic_divider[64][2] = {
        {0x20, 22  }, {0x21, 24  }, {0x22, 26  }, {0x23, 28  }, /* 0 - 3   */
        {0x00, 30  }, {0x01, 32  }, {0x24, 32  }, {0x02, 36  }, /* 4 - 7   */
        {0x25, 36  }, {0x26, 40  }, {0x03, 42  }, {0x27, 44  }, /* 8 - 11  */
        {0x04, 48  }, {0x28, 48  }, {0x05, 52  }, {0x29, 56  }, /* 12 - 15 */
        {0x06, 60  }, {0x2A, 64  }, {0x07, 72  }, {0x2B, 72  }, /* 16 - 19 */
        {0x08, 80  }, {0x2C, 80  }, {0x09, 88  }, {0x2D, 96  }, /* 20 - 23 */
        {0x0A, 104 }, {0x2E, 112 }, {0x0B, 128 }, {0x2F, 128 }, /* 24 - 27 */
        {0x0C, 144 }, {0x0D, 160 }, {0x30, 160 }, {0x0E, 192 }, /* 28 - 31 */
        {0x31, 192 }, {0x32, 224 }, {0x0F, 240 }, {0x33, 256 }, /* 32 - 35 */
        {0x10, 288 }, {0x11, 320 }, {0x34, 320 }, {0x12, 384 }, /* 36 - 39 */
        {0x35, 384 }, {0x36, 448 }, {0x13, 480 }, {0x37, 512 }, /* 40 - 43 */
        {0x14, 576 }, {0x15, 640 }, {0x38, 640 }, {0x16, 768 }, /* 44 - 47 */
        {0x39, 768 }, {0x3A, 896 }, {0x17, 960 }, {0x3B, 1024}, /* 48 - 51 */
        {0x18, 1152}, {0x19, 1280}, {0x3C, 1280}, {0x1A, 1536}, /* 52 - 55 */
        {0x3D, 1536}, {0x3E, 1792}, {0x1B, 1920}, {0x3F, 2048}, /* 56 - 59 */
        {0x1C, 2304}, {0x1D, 2560}, {0x1E, 3072}, {0x1F, 3840}  /* 60 - 63 */
};


aw_local aw_err_t __imxi2c_clock_set(struct awbl_imx6ul_i2c_devinfo *p_devinfo,
                                 uint32_t speed)
{
    uint32_t  divider;
    int  start = 0;
    int  end   = 63;
    int  mid   = 0;
    __IMXI2C_HW_DECL(p_hw_i2c, p_devinfo);

    if (speed == 0) {
        return -AW_ENOTSUP;
    }

    divider  = aw_clk_rate_get(p_devinfo->ref_clk_id);
    divider /= speed;

    /* i2c ����Ƶ��3840, ��С��Ƶ��22 */
    if (divider < 22) {
        divider = 22;
        goto __end;
    } else if (divider > 3840) {
        return -AW_ENOTSUP;
    }

    /* find the ic value according to divider */
    while (start < end) {
        mid = (start + end) / 2;
        if (__g_imxi2c_ic_divider[mid][1] > divider) {
            end = mid - 1;
        } else if (__g_imxi2c_ic_divider[mid][1] < divider) {
            start = mid + 1;
        } else {
           break;
        }
    }

__end:
    /* the closet value is __g_imxi2c_ic_divider[mid][0] */
    writew(__g_imxi2c_ic_divider[mid][0], &p_hw_i2c->ifdr);
    return AW_OK;

}


/**
 * \brief init i2c clock
 * \param p_devinfo
 * \note  duty cycle equal 50%
 */
aw_local void __imxi2c_clock_init (struct awbl_imx6ul_i2c_devinfo *p_devinfo)
{
    __imxi2c_clock_set(p_devinfo, p_devinfo->i2c_master_devinfo.speed);
}


/**
 * \brief I2C controller hardware initialize
 */
aw_local void __imxi2c_hard_init (struct awbl_imx6ul_i2c_dev *p_this)
{
    int32_t dly = 0xFF;
    __IMXI2C_DEVINFO_DECL(p_this_dvif, p_this);
    __IMXI2C_HW_DECL(p_hw_i2c, p_this_dvif);

    /* Release i2c reset */
    __REG_AND_MODIFY32(&p_hw_i2c->i2cr, ~__IMXI2C_I2CR_IEN_MASK);

    while (dly--> 0) {

    }
    __imxi2c_clock_init(p_this_dvif);
    p_this->timeout = p_this_dvif->i2c_master_devinfo.timeout;

    writew(0, &p_hw_i2c->i2sr);

    __REG_OR_MODIFY32(&p_hw_i2c->i2cr, __IMXI2C_I2CR_IEN_MASK);
    __REG_OR_MODIFY32(&p_hw_i2c->i2cr, __IMXI2C_I2CR_IIEN_MASK);
}


aw_local aw_inline int __i2c_isr_rw(void *p_arg)
{
    __IMXI2C_DEV_DECL(p_this, p_arg);
    __IMXI2C_DEVINFO_DECL(p_this_dvif, p_this);
    __IMXI2C_HW_DECL(p_hw_i2c, p_this_dvif);
    uint8_t data;

    /* deal write */
    if (p_this->dir == __WRITE) {

        /* ���е�buffer���������  */
        if (p_this->trans_buf_num) {

            if (p_this->p_trans_buf->buf_size) {
                data = p_this->p_trans_buf->p_buf[p_this->data_ptr++];
                p_this->p_trans_buf->buf_size--;

                writew(__IMXI2C_I2DR_DATA_MASK &
                       data,
                       &p_hw_i2c->i2dr);

                if (p_this->p_trans_buf->buf_size == 0) {

                   p_this->trans_buf_num--;
                   p_this->p_trans_buf++;
                   p_this->data_ptr = 0;
               }

               return AW_ERROR;
            }
        } else {
            /* �������ݶ��������  */
            return AW_OK;
        }

      /* deal read*/
    } else {

        /* if have a start condition, must dump read */
        if (p_this->p_trans_buf == NULL) {
            /* ֱ���ͷ��ź���  */
            AW_SEMC_GIVE(p_this->i2c_sem);
            return AW_ERROR;
        }

        if (p_this->data_ptr == p_this->p_trans_buf->buf_size - 1) {

            __REG_AND_MODIFY32(&p_hw_i2c->i2cr, ~__IMXI2C_I2CR_MSTA_MASK);
            __REG_AND_MODIFY32(&p_hw_i2c->i2cr, ~__IMXI2C_I2CR_MTX_MASK);
            __REG_AND_MODIFY32(&p_hw_i2c->i2cr, ~__IMXI2C_I2CR_TXAK_MASK);

            /* ��STOP����ȥ���Ĵ����Ͳ������SCL�Ĳ����� */
            *((uint8_t *)(p_this->p_trans_buf->p_buf + p_this->data_ptr)) =
                    readw(&p_hw_i2c->i2dr) & __IMXI2C_I2DR_DATA_MASK;

            /* ������� */
            p_this->data_ptr = 0;
            return AW_OK;

        } else if (p_this->data_ptr == p_this->p_trans_buf->buf_size - 2) {

            /* �����������һ���ֽں���һ��NAK�ź� , �����ڷ��͵Ĺ����в�����Ĵ���*/
            __REG_OR_MODIFY32(&p_hw_i2c->i2cr, __IMXI2C_I2CR_TXAK_MASK);
        }

        /* ��ȡ���ݵ�buffer,��ȡǰһ���ᷢ�ͺ�һ����SCL��׼�����պ�һ�� ,����˵���ں����������ǰ����ܳ�������ѭ��*/
        *((uint8_t *)(p_this->p_trans_buf->p_buf + p_this->data_ptr)) =
                readw(&p_hw_i2c->i2dr) & __IMXI2C_I2DR_DATA_MASK;

        /* ���յ������ݼ�1 */
        p_this->data_ptr++;
    }

    return AW_ERROR;
}


aw_local void __imxi2c_isr (void *p_arg)
{
    uint16_t isr_status, ctrl_val;
    __IMXI2C_DEV_DECL(p_this, p_arg);
    __IMXI2C_DEVINFO_DECL(p_this_dvif, p_this);
    __IMXI2C_HW_DECL(p_hw_i2c, p_this_dvif);

    /* read the i2c interrupt status reg */
    isr_status = readw(&p_hw_i2c->i2sr);

    /* read control reg */
    ctrl_val = readw(&p_hw_i2c->i2cr);

    /* clear i2c interrupt */
    writew(isr_status & (~__IMXI2C_I2SR_IIF_MASK), &p_hw_i2c->i2sr);

    /* �����ٲ�ʧ�� */
    if (isr_status & __IMXI2C_I2SR_IAL_MASK) {

        /* ���ݴ���ҲҪ�ͷ��ź��� */
        AW_SEMC_GIVE(p_this->i2c_sem);
        return;
    } else if (isr_status & __IMXI2C_I2SR_ICF_MASK) {

        if (isr_status & __IMXI2C_I2SR_RXAK_MASK) {
            if (ctrl_val & __IMXI2C_I2CR_MTX_MASK) {
                /* ���������������ݵ��ӻ���Ӧ������ */

                AW_SEMC_GIVE(p_this->i2c_sem);
                return;
            }
        }

        /* ������䳬ʱ��tick */
        p_this->late_tick = aw_sys_tick_get();

        /* ����д����򷵻�AW_OK */
        if (__i2c_isr_rw(p_arg) == AW_OK) {

            /* �ͷ��ź���  */
            AW_SEMC_GIVE(p_this->i2c_sem);
            return;
        } else{
            return;
        }
    } else {

        /* don't go to here */
        while(1);
    }
}


aw_local aw_err_t __i2c_reset(struct awbl_i2c_master *p_master)
{
    __IMXI2C_DEV_DECL(p_this, p_master);
    __IMXI2C_DEVINFO_DECL(p_this_dvif, p_this);
    __IMXI2C_HW_DECL(p_hw_i2c, p_this_dvif);

    /* ��ʱ����λ���� */
    aw_int_disable(p_this_dvif->inum);

    /* ��λI2C����  */
    __REG_AND_MODIFY32(&p_hw_i2c->i2cr, ~__IMXI2C_I2CR_IEN_MASK);

    /*
    * ��λ����ʱֵ,����ʵ�ʲ��Եó���һ������ֵ,������IMX6ul��ʹ��1000ms�ܹ���λ�ɹ�
    */
    aw_mdelay(1000);

    AW_SEMC_RESET(p_this->i2c_sem, 0);
    __REG_OR_MODIFY32(&p_hw_i2c->i2cr, __IMXI2C_I2CR_IEN_MASK);
    __REG_OR_MODIFY32(&p_hw_i2c->i2cr, __IMXI2C_I2CR_IIEN_MASK);

    __REG_OR_MODIFY32(&p_hw_i2c->i2cr, __IMXI2C_I2CR_MTX_MASK);

    /* ��ʹ�ܽӿ���Ӧ�����NVIC��pendingλ */
    aw_int_enable(p_this_dvif->inum);

    p_this->data_ptr = 0;

    return AW_OK;
}


aw_local aw_err_t __i2c_deal_error_code(struct awbl_i2c_master *p_master)
{
    __IMXI2C_DEV_DECL(p_this, p_master);
    __IMXI2C_DEVINFO_DECL(p_this_dvif, p_this);
    __IMXI2C_HW_DECL(p_hw_i2c, p_this_dvif);

    uint32_t isr_status = 0, ret = AW_OK, ctrl_val = 0;

    /* read the i2c interrupt status reg */
    isr_status = readw(&p_hw_i2c->i2sr);

    /* read control reg */
    ctrl_val = readw(&p_hw_i2c->i2cr);

    /* �����ٲ�ʧ�� */
    if (isr_status & __IMXI2C_I2SR_IAL_MASK) {
       /* clear ial bit */
       ret = -AW_EIO;

    } else if (isr_status & __IMXI2C_I2SR_ICF_MASK) {

       if (isr_status & __IMXI2C_I2SR_RXAK_MASK) {
           if (ctrl_val & __IMXI2C_I2CR_MTX_MASK) {
               /* ���������������ݵ��ӻ���Ӧ������ */
               ret = -AW_ENODEV;

           }
       }
    }

    /* ��ʾ�쳣�˳� Ҫ��ֹͣ�ź� */
    if (ret != AW_OK) {
        __i2c_reset(p_master);

        return ret;
    }

    return ret;
}


aw_local aw_err_t __take_smb(struct awbl_i2c_master *p_master)
{

    __IMXI2C_DEV_DECL(p_this, p_master);
    uint32_t late_ticks_copy = 0, code_ticks = 0;
    int   ret = AW_OK;

    /* ������䳬ʱ
     *
     * ����ź�����ʱ�ˣ����Ч���һ���ж�ʱ������tick, �ͳ�ʱ���tick�Ĳ�ֵ������������õĳ�ʱʱ�䣬���ж�Ϊ
     * ��ʱ����ʱ��Ĵ���:�ȹر��ж�(��ֹ�ڸ�λ����ʱ�жϲ���)����λI2C���������жϡ�
     *
     * ������һ���жϾ͸���һ�������ticks��ֵ�����ӻ���SCL����ʱ�䳬����ʱʱ�䣬���ߺܳ�һ��ʱ��û�н����жϣ���
     * ����볬ʱ����
     *
     */
    while(1) {
        /* �ȴ��ź���,�����ó�ʱʱ��  */
        ret = AW_SEMC_TAKE(p_this->i2c_sem, (p_this->timeout == AWBL_I2C_WAITFOREVER) ?
                                             p_this->timeout :
                                             aw_ms_to_ticks(p_this->timeout + 1));

        /* timeout, take out the message from queue */
        if (ret == -AW_ETIME) {
            late_ticks_copy = p_this->late_tick;

            /* ��֤��������CPU������ , late_ticks_copy �� code_ticks ���Ⱥ�˳��*/
            aw_mb();

            code_ticks = aw_sys_tick_get();
            if ((code_ticks - late_ticks_copy) >=
                aw_ms_to_ticks(p_this->timeout)) {

                ret =  -AW_ETIMEDOUT;
                return ret;
            }
        } else {
            break;
        }
    }

    return ret;
}

aw_local aw_err_t  __imx6ul_i2c_write(struct awbl_i2c_master *p_master,
                                      uint16_t                chip_addr,
                                      struct awbl_trans_buf  *p_trans_buf,
                                      uint32_t                trans_buf_num,
                                      uint8_t                 stop_ctl,
                                      uint16_t                dr_flags)
{
    __IMXI2C_DEV_DECL(p_this, p_master);
    __IMXI2C_DEVINFO_DECL(p_this_dvif, p_this);
    __IMXI2C_HW_DECL(p_hw_i2c, p_this_dvif);

    aw_tick_t start_ticks, end_ticks;
    uint32_t  timeout_ms;

    /* imx6ul�ĵ�ַֻ֧��7bit  */
    const uint8_t   addr  = (chip_addr << 1);
    int              ret  = AW_OK;

    /* ��Ƿ���Ϊд */
    p_this->dir    =  __WRITE;

    /* ��ȡһ��ֵ,��ֹû�н��ж�,p_this->late_tick��ֵ�ǳ�ʼֵ�������жϳ�ʱ */
    p_this->late_tick  = aw_sys_tick_get();

    /* ���͵�ַ�Ϳ�ʼ�ź� */


    /* ���Ϳ�ʼ�ź� */
    __REG_OR_MODIFY32(&p_hw_i2c->i2cr, __IMXI2C_I2CR_MTX_MASK |
                                       __IMXI2C_I2CR_MSTA_MASK);


    /* �Ƚ��ж�����Ҫ�����ȫ�ֱ������ڴ����жϵ�ǰ�棬������ֹ�жϵ���ʱ���ݻ�û�и��� */
    p_this->p_trans_buf   = p_trans_buf;
    p_this->trans_buf_num = trans_buf_num;

    /* ��д , 7bit ���豸��ַ, �����smp,�����ﻹӦ�ü�һ��smp�����ϣ���֤�ڽ����ж�ǰ����cpu�ܿ���
     * p_this->p_trans_buf �� p_this->trans_buf_num
     */
    writew(addr & __IMXI2C_I2DR_DATA_MASK, &p_hw_i2c->i2dr);

    ret = __take_smb(p_master);

    if (ret) {
        __i2c_reset(p_master);
        return ret;
    }

    ret = __i2c_deal_error_code(p_master);

    /* �ж�ʧ�� */
    if (ret != AW_OK) {
        return ret;
    }

    if (stop_ctl == AWBL_I2C_WRITE_STOP) {

        /* ��ֹͣ�ź�  */
        __REG_AND_MODIFY32(&p_hw_i2c->i2cr, ~__IMXI2C_I2CR_MSTA_MASK);

        /* ������˳�ǰӦ�õȴ����߿��У���ֹSTOPָ�û�в�����ɣ���һ��д�Ŀ�ʼ�ź������� */
        start_ticks = aw_sys_tick_get();
        do {
            end_ticks  = aw_sys_tick_get();
            timeout_ms = aw_ticks_to_ms(end_ticks - start_ticks);

            /* �ͷ�CPU */
            aw_task_delay(1);
        } while( (readw(&p_hw_i2c->i2sr) & __IMXI2C_I2SR_IBB_MASK) &&
                 (timeout_ms < __IMXI2C_WAIT_IDEL_TIMEOUT_MS));

        if (timeout_ms >= __IMXI2C_WAIT_IDEL_TIMEOUT_MS) {
            /* ����æ */
            ret =  -AW_EIO;
        }

        /* ����ֹͣ�ź�Ҫ��һ�����ߵ�idel_time */
        if (p_this->bus_idle_time > __IMXI2C_PATFORM_BASE_IDEL_TIME) {
            aw_udelay(p_this->bus_idle_time - __IMXI2C_PATFORM_BASE_IDEL_TIME);
        }
    }

    return ret;
}


aw_local aw_err_t  __imx6ul_i2c_read(struct awbl_i2c_master *p_master,
                                     uint16_t                chip_addr,
                                     struct awbl_trans_buf  *p_trans_buf,
                                     uint8_t                 start_ctl,
                                     uint16_t                dr_flags)
{
    __IMXI2C_DEV_DECL(p_this, p_master);
    __IMXI2C_DEVINFO_DECL(p_this_dvif, p_this);
    __IMXI2C_HW_DECL(p_hw_i2c, p_this_dvif);
    int ret = AW_OK;
    aw_tick_t start_ticks = 0, end_ticks = 0;
    uint32_t  timeout_ms = 0;

    /* imx6ul�ĵ�ַֻ֧��7bit  */
    const uint8_t    addr      = (chip_addr << 1) | 1;

    /* p_buf�Ƕ�buf */
    p_this->dir         =  __READ;

    /* �������ֹͣ�źţ�����START�������RSTART�ź�, stop_setĬ����1 */
    if (start_ctl == AWBL_I2C_READ_RESTART) {

       /* �������¿�ʼ����, ���ü�����,��Ϊ��һ��û��ֹͣ,����������æ  */
        __REG_OR_MODIFY32(&p_hw_i2c->i2cr, __IMXI2C_I2CR_RSTA_MASK);
    } else {
       /* ��ʼ�ź� */
        __REG_OR_MODIFY32(&p_hw_i2c->i2cr, __IMXI2C_I2CR_MSTA_MASK);
        __REG_OR_MODIFY32(&p_hw_i2c->i2cr, __IMXI2C_I2CR_MTX_MASK);
    }

    /* ����������дi2dr�Ĵ���ǰ�� */
    p_this->p_trans_buf = NULL;

    if (1 == p_trans_buf->buf_size) {
         /* the second last byte, so send a NAK codition */
        __REG_OR_MODIFY32(&p_hw_i2c->i2cr, __IMXI2C_I2CR_TXAK_MASK);
    } else {
        __REG_AND_MODIFY32(&p_hw_i2c->i2cr, ~__IMXI2C_I2CR_TXAK_MASK);
    }

    /* д�����ݺ��ȡ��ǰtick */
    p_this->late_tick  = aw_sys_tick_get();

    /* д���ݺ��治Ҫ��ʲô�����Ĳ�������Ϊ�ⲽ����Ϳ��ܲ����ж�, ֻ֧��7bit */
    writew(addr & __IMXI2C_I2DR_DATA_MASK, &p_hw_i2c->i2dr);

    ret = __take_smb(p_master);

    if (ret) {
        __i2c_reset(p_master);

        return ret;
    }

    ret = __i2c_deal_error_code(p_master);

    if (ret != AW_OK) {
        return ret;
    }

    /* dump read */
    /* set receive*/
    __REG_AND_MODIFY32(&p_hw_i2c->i2cr, ~__IMXI2C_I2CR_MTX_MASK);

    /* �ն������һ���ֽڵĲ��Σ����Բ����Ͽ��������ģ�ʵ�ʶ�����buffer����һ�� */
    p_this->p_trans_buf   = p_trans_buf;
    readw(&p_hw_i2c->i2dr);

    ret = __take_smb(p_master);

    if (ret) {
        __i2c_reset(p_master);

        return ret;
    }

    /* ����ֹͣ�ź�Ҫ��һ�����ߵ�idel_time */
    if (p_this->bus_idle_time > __IMXI2C_PATFORM_BASE_IDEL_TIME) {
        aw_udelay(p_this->bus_idle_time - __IMXI2C_PATFORM_BASE_IDEL_TIME);
    }

    ret = __i2c_deal_error_code(p_master);

    if (ret != AW_OK) {
        return ret;
    }

    /* ������˳�ǰӦ�õȴ����߿��У���ֹSTOPָ�û�в�����ɣ���һ��д�Ŀ�ʼ�ź������� */
    start_ticks = aw_sys_tick_get();
    do {
        end_ticks  = aw_sys_tick_get();
        timeout_ms = aw_ticks_to_ms(end_ticks - start_ticks);

        /* �ͷ�CPU */
        aw_task_delay(1);
    } while( (readw(&p_hw_i2c->i2sr) & __IMXI2C_I2SR_IBB_MASK) &&
             (timeout_ms < __IMXI2C_WAIT_IDEL_TIMEOUT_MS));

    if (timeout_ms >= __IMXI2C_WAIT_IDEL_TIMEOUT_MS) {

        /* ����æ */
        ret = -AW_EIO;
    }

    return ret;
}


aw_local aw_err_t  __imx6ul_i2c_set_cfg(struct awbl_i2c_master *p_master,
                                        struct aw_i2c_config *p_cfg)
{
    __IMXI2C_DEV_DECL(p_this, p_master);
    __IMXI2C_DEVINFO_DECL(p_this_dvif, p_this);

    int ret = AW_OK;

    ret = __imxi2c_clock_set(p_this_dvif, p_cfg->speed);

    if (ret) {
        return ret;
    }

    p_this->bus_idle_time = p_cfg->bus_idle_time;
    p_this->timeout = p_cfg->timeout;

    return AW_OK;
}


aw_local aw_err_t __imx6ul_i2c_get_cfg(struct awbl_i2c_master *p_master,
                                       struct aw_i2c_config   *p_cfg)
{
    __IMXI2C_DEV_DECL(p_this, p_master);
    __IMXI2C_DEVINFO_DECL(p_this_dvif, p_this);
    __IMXI2C_HW_DECL(p_hw_i2c, p_this_dvif);

    int  mid   = 0;
    uint8_t div_val = 0;

    div_val = readw(&p_hw_i2c->ifdr) & 0x3F;

    /* find the ic value according to divider */
    while (mid < 64) {
        if (__g_imxi2c_ic_divider[mid][0] != div_val) {
            mid++;
        } else {
            break;
        }
    }

    p_cfg->speed = aw_clk_rate_get(p_this_dvif->ref_clk_id) /
               __g_imxi2c_ic_divider[mid][1] ;

    p_cfg->timeout = p_this->timeout;

    p_cfg->bus_idle_time = p_this->bus_idle_time;

    return AW_OK;

}
/*******************************************************************************
  implementation
*******************************************************************************/

/*  device information submited by driver (must defined as aw_const) */
aw_local aw_const struct awbl_i2c_master_devinfo2 __g_imxi2c_devinfo2 = {
    AWBL_FEATURE_I2C_7BIT,  /* ֻ֧��7bit�ĵ�ַ */
    __imx6ul_i2c_write,
    __imx6ul_i2c_read,
    __imx6ul_i2c_set_cfg,
    __imx6ul_i2c_get_cfg,

};


/**
 * \brief first level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __imxi2c_inst_init1(awbl_dev_t *p_dev)
{
    return;
}

/**
 * \brief second level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __imxi2c_inst_init2(awbl_dev_t *p_dev)
{
    __IMXI2C_MASTER_DEV_DECL(p_master, p_dev);
    __IMXI2C_DEV_DECL(p_this, p_dev);
    __IMXI2C_DEVINFO_DECL(p_this_dvif, p_dev);

    /* platform initialization */
    if (p_this_dvif->pfunc_plfm_init != NULL) {
        p_this_dvif->pfunc_plfm_init();
    }

    /* AWBus i2c master general init2 */
    p_master->p_devinfo2 = &__g_imxi2c_devinfo2;

    /* init controller hardware */
    __imxi2c_hard_init(p_this);

    /* initialize the stack binary semaphore */
    AW_SEMC_INIT(p_this->i2c_sem, 0, AW_SEM_Q_FIFO);

    /* connect and enable EINT isr */
    aw_int_connect(p_this_dvif->inum, (aw_pfuncvoid_t)__imxi2c_isr, (void *)p_dev);
    aw_int_enable(p_this_dvif->inum);


    /* ��ʼ��һЩȫ�ֱ���  */
    p_this->dir            = __WRITE;
    p_this->late_tick      = 0;
    p_this->bus_idle_time  = 0;
    p_this->data_ptr       = 0;

    /* create i2c bus */
    awbl_i2cbus_create((struct awbl_i2c_master *)p_dev);

    return;
}

/**
 * \brief third level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __imxi2c_inst_connect(awbl_dev_t *p_dev)
{
    return;
}

/*******************************************************************************
  locals
*******************************************************************************/

/* driver functions (must defined as aw_const) */
aw_local aw_const struct awbl_drvfuncs __g_imxi2c_drvfuncs = {
        __imxi2c_inst_init1,
        __imxi2c_inst_init2,
        __imxi2c_inst_connect
};

/* driver registration (must defined as aw_const) */
aw_local aw_const awbl_plb_drvinfo_t __g_imxi2c_drv_registration = {
    {
        AWBL_VER_1,                             /* awb_ver */
        AWBL_BUSID_PLB | AWBL_DEVID_BUSCTRL,    /* bus_id */
        AWBL_IMX6UL_I2C_NAME,                   /* p_drvname */
        &__g_imxi2c_drvfuncs,                   /* p_busfuncs */
        NULL,                                   /* p_methods */
        NULL                                    /* pfunc_drv_probe */
    }
};


void awbl_imx6ul_i2c_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_imxi2c_drv_registration);
}


