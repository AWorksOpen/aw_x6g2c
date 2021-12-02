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
 * \brief Freescale i.MX6UL系列I2C外设
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
 * 驱动说明:
 *
 * 1. 本驱动暂时不支持多主机，因为多主机仲裁的条件不好模拟（同时发送开始信号）
 * 2. 本驱动暂时不支持仲裁失败后自动重试的功能，因为遭成仲裁失败的状态位置1的情况有很多种，1) 多主机下的仲裁失败,
 *    这个是可以重试的, 2) 由于总线上出现的干扰,即可能发送数据发送一半后，总线上出现干扰或者从机出现故障遭成的仲裁
 *    失败，这个就不能重试。
 *
 *
 *
 *
 * 部分宏控制说明
 *
 *  __IMXI2C_PATFORM_BASE_IDEL_TIME: 是平台在Release模式下从STOP 到下次START之间的最短的空闲
 *                                   时间,用户通过aw_i2c_set_cfg设置bus_idle_time,如果
 *                                   bus_idle_time < Release下的最小空闲则不做处理，如果
 *                                   大于最小空闲则用bus_idle_time减去
 *                                   __IMXI2C_PATFORM_BASE_IDEL_TIME，空闲延时为其差值
 *                                   这样可以最大化的提高效率，使得延时时间更接近设置值。
 *
 *  __IMXI2C_WAIT_IDEL_TIMEOUT_MS : 当写完或者读完数据,要等待总线空闲(查询相应的状态位)，设置一个超时
 *                                  时间，如果超过这个时间可能出现了总线仲裁等错误，这个时候就要复位从
 *                                  设备
 *
 */

/** \brief 平台最少的总线空闲延时时间(us),用Release方式,卡测  */
#define __IMXI2C_PATFORM_BASE_IDEL_TIME    4



/** \brief 当写完数据后最大等待总线空闲时间   */
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
 * 不加volatile是防止使用writew接口操作时产生内存撕裂的bug, 在writew中已经有了volatile
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

    /* i2c 最大分频是3840, 最小分频是22 */
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

        /* 所有的buffer都发送完成  */
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
            /* 所有数据都发送完成  */
            return AW_OK;
        }

      /* deal read*/
    } else {

        /* if have a start condition, must dump read */
        if (p_this->p_trans_buf == NULL) {
            /* 直接释放信号量  */
            AW_SEMC_GIVE(p_this->i2c_sem);
            return AW_ERROR;
        }

        if (p_this->data_ptr == p_this->p_trans_buf->buf_size - 1) {

            __REG_AND_MODIFY32(&p_hw_i2c->i2cr, ~__IMXI2C_I2CR_MSTA_MASK);
            __REG_AND_MODIFY32(&p_hw_i2c->i2cr, ~__IMXI2C_I2CR_MTX_MASK);
            __REG_AND_MODIFY32(&p_hw_i2c->i2cr, ~__IMXI2C_I2CR_TXAK_MASK);

            /* 当STOP后再去读寄存器就不会产生SCL的波形了 */
            *((uint8_t *)(p_this->p_trans_buf->p_buf + p_this->data_ptr)) =
                    readw(&p_hw_i2c->i2dr) & __IMXI2C_I2DR_DATA_MASK;

            /* 接收完成 */
            p_this->data_ptr = 0;
            return AW_OK;

        } else if (p_this->data_ptr == p_this->p_trans_buf->buf_size - 2) {

            /* 当接收完最后一个字节后发送一个NAK信号 , 可能在发送的过程中查这个寄存器*/
            __REG_OR_MODIFY32(&p_hw_i2c->i2cr, __IMXI2C_I2CR_TXAK_MASK);
        }

        /* 读取数据到buffer,读取前一个会发送后一个的SCL，准备接收后一个 ,所以说放在后面读，放在前面可能出现无限循环*/
        *((uint8_t *)(p_this->p_trans_buf->p_buf + p_this->data_ptr)) =
                readw(&p_hw_i2c->i2dr) & __IMXI2C_I2DR_DATA_MASK;

        /* 接收到的数据加1 */
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

    /* 总线仲裁失败 */
    if (isr_status & __IMXI2C_I2SR_IAL_MASK) {

        /* 数据错误，也要释放信号量 */
        AW_SEMC_GIVE(p_this->i2c_sem);
        return;
    } else if (isr_status & __IMXI2C_I2SR_ICF_MASK) {

        if (isr_status & __IMXI2C_I2SR_RXAK_MASK) {
            if (ctrl_val & __IMXI2C_I2CR_MTX_MASK) {
                /* 处理主机发送数据但从机无应答的情况 */

                AW_SEMC_GIVE(p_this->i2c_sem);
                return;
            }
        }

        /* 更新码间超时的tick */
        p_this->late_tick = aw_sys_tick_get();

        /* 读或写完成则返回AW_OK */
        if (__i2c_isr_rw(p_arg) == AW_OK) {

            /* 释放信号量  */
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

    /* 超时过后复位主机 */
    aw_int_disable(p_this_dvif->inum);

    /* 复位I2C外设  */
    __REG_AND_MODIFY32(&p_hw_i2c->i2cr, ~__IMXI2C_I2CR_IEN_MASK);

    /*
    * 复位后延时值,根据实际测试得出的一个经验值,这里在IMX6ul上使用1000ms能够复位成功
    */
    aw_mdelay(1000);

    AW_SEMC_RESET(p_this->i2c_sem, 0);
    __REG_OR_MODIFY32(&p_hw_i2c->i2cr, __IMXI2C_I2CR_IEN_MASK);
    __REG_OR_MODIFY32(&p_hw_i2c->i2cr, __IMXI2C_I2CR_IIEN_MASK);

    __REG_OR_MODIFY32(&p_hw_i2c->i2cr, __IMXI2C_I2CR_MTX_MASK);

    /* 在使能接口中应该清除NVIC的pending位 */
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

    /* 总线仲裁失败 */
    if (isr_status & __IMXI2C_I2SR_IAL_MASK) {
       /* clear ial bit */
       ret = -AW_EIO;

    } else if (isr_status & __IMXI2C_I2SR_ICF_MASK) {

       if (isr_status & __IMXI2C_I2SR_RXAK_MASK) {
           if (ctrl_val & __IMXI2C_I2CR_MTX_MASK) {
               /* 处理主机发送数据但从机无应答的情况 */
               ret = -AW_ENODEV;

           }
       }
    }

    /* 表示异常退出 要给停止信号 */
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

    /* 处理码间超时
     *
     * 如果信号量超时了，则比效最近一次中断时产生的tick, 和超时后的tick的差值，如果大于设置的超时时间，则判定为
     * 超时，超时后的处理:先关闭中断(防止在复位主机时中断产生)，复位I2C主机，打开中断。
     *
     * 毎进入一次中断就更新一次最近的ticks的值，当从机将SCL拉低时间超过超时时间，或者很长一段时间没有进入中断，则
     * 会进入超时处理。
     *
     */
    while(1) {
        /* 等待信号量,并设置超时时间  */
        ret = AW_SEMC_TAKE(p_this->i2c_sem, (p_this->timeout == AWBL_I2C_WAITFOREVER) ?
                                             p_this->timeout :
                                             aw_ms_to_ticks(p_this->timeout + 1));

        /* timeout, take out the message from queue */
        if (ret == -AW_ETIME) {
            late_ticks_copy = p_this->late_tick;

            /* 保证编译器和CPU不乱序 , late_ticks_copy 和 code_ticks 有先后顺序*/
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

    /* imx6ul的地址只支持7bit  */
    const uint8_t   addr  = (chip_addr << 1);
    int              ret  = AW_OK;

    /* 标记方向为写 */
    p_this->dir    =  __WRITE;

    /* 获取一个值,防止没有进中断,p_this->late_tick的值是初始值，不便判断超时 */
    p_this->late_tick  = aw_sys_tick_get();

    /* 发送地址和开始信号 */


    /* 发送开始信号 */
    __REG_OR_MODIFY32(&p_hw_i2c->i2cr, __IMXI2C_I2CR_MTX_MASK |
                                       __IMXI2C_I2CR_MSTA_MASK);


    /* 先将中断里需要处理的全局变量放在触发中断的前面，这样防止中断到来时数据还没有更新 */
    p_this->p_trans_buf   = p_trans_buf;
    p_this->trans_buf_num = trans_buf_num;

    /* 首写 , 7bit 的设备地址, 如果是smp,则这里还应该加一个smp的屏障，保证在进入中断前其他cpu能看到
     * p_this->p_trans_buf 和 p_this->trans_buf_num
     */
    writew(addr & __IMXI2C_I2DR_DATA_MASK, &p_hw_i2c->i2dr);

    ret = __take_smb(p_master);

    if (ret) {
        __i2c_reset(p_master);
        return ret;
    }

    ret = __i2c_deal_error_code(p_master);

    /* 判断失败 */
    if (ret != AW_OK) {
        return ret;
    }

    if (stop_ctl == AWBL_I2C_WRITE_STOP) {

        /* 给停止信号  */
        __REG_AND_MODIFY32(&p_hw_i2c->i2cr, ~__IMXI2C_I2CR_MSTA_MASK);

        /* 读完成退出前应该等待总线空闲，防止STOP指令还没有操作完成，下一个写的开始信号又来临 */
        start_ticks = aw_sys_tick_get();
        do {
            end_ticks  = aw_sys_tick_get();
            timeout_ms = aw_ticks_to_ms(end_ticks - start_ticks);

            /* 释放CPU */
            aw_task_delay(1);
        } while( (readw(&p_hw_i2c->i2sr) & __IMXI2C_I2SR_IBB_MASK) &&
                 (timeout_ms < __IMXI2C_WAIT_IDEL_TIMEOUT_MS));

        if (timeout_ms >= __IMXI2C_WAIT_IDEL_TIMEOUT_MS) {
            /* 总线忙 */
            ret =  -AW_EIO;
        }

        /* 给了停止信号要给一个总线的idel_time */
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

    /* imx6ul的地址只支持7bit  */
    const uint8_t    addr      = (chip_addr << 1) | 1;

    /* p_buf是读buf */
    p_this->dir         =  __READ;

    /* 如果产生停止信号，则发送START，否则就RSTART信号, stop_set默认是1 */
    if (start_ctl == AWBL_I2C_READ_RESTART) {

       /* 发送重新开始条件, 不用检测空闲,因为上一个没有停止,所以现在是忙  */
        __REG_OR_MODIFY32(&p_hw_i2c->i2cr, __IMXI2C_I2CR_RSTA_MASK);
    } else {
       /* 开始信号 */
        __REG_OR_MODIFY32(&p_hw_i2c->i2cr, __IMXI2C_I2CR_MSTA_MASK);
        __REG_OR_MODIFY32(&p_hw_i2c->i2cr, __IMXI2C_I2CR_MTX_MASK);
    }

    /* 这个必须放在写i2dr寄存器前面 */
    p_this->p_trans_buf = NULL;

    if (1 == p_trans_buf->buf_size) {
         /* the second last byte, so send a NAK codition */
        __REG_OR_MODIFY32(&p_hw_i2c->i2cr, __IMXI2C_I2CR_TXAK_MASK);
    } else {
        __REG_AND_MODIFY32(&p_hw_i2c->i2cr, ~__IMXI2C_I2CR_TXAK_MASK);
    }

    /* 写完数据后获取当前tick */
    p_this->late_tick  = aw_sys_tick_get();

    /* 写数据后面不要有什么其他的操作，因为这步过后就可能产生中断, 只支持7bit */
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

    /* 空读会产生一个字节的波形，所以波形上看是正常的，实际读出的buffer少了一个 */
    p_this->p_trans_buf   = p_trans_buf;
    readw(&p_hw_i2c->i2dr);

    ret = __take_smb(p_master);

    if (ret) {
        __i2c_reset(p_master);

        return ret;
    }

    /* 给了停止信号要给一个总线的idel_time */
    if (p_this->bus_idle_time > __IMXI2C_PATFORM_BASE_IDEL_TIME) {
        aw_udelay(p_this->bus_idle_time - __IMXI2C_PATFORM_BASE_IDEL_TIME);
    }

    ret = __i2c_deal_error_code(p_master);

    if (ret != AW_OK) {
        return ret;
    }

    /* 读完成退出前应该等待总线空闲，防止STOP指令还没有操作完成，下一个写的开始信号又来临 */
    start_ticks = aw_sys_tick_get();
    do {
        end_ticks  = aw_sys_tick_get();
        timeout_ms = aw_ticks_to_ms(end_ticks - start_ticks);

        /* 释放CPU */
        aw_task_delay(1);
    } while( (readw(&p_hw_i2c->i2sr) & __IMXI2C_I2SR_IBB_MASK) &&
             (timeout_ms < __IMXI2C_WAIT_IDEL_TIMEOUT_MS));

    if (timeout_ms >= __IMXI2C_WAIT_IDEL_TIMEOUT_MS) {

        /* 总线忙 */
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
    AWBL_FEATURE_I2C_7BIT,  /* 只支持7bit的地址 */
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


    /* 初始化一些全局变量  */
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


