/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief sd3078 rtc driver
 *
 * this driver is the implementation of RTC device for SD3078
 * just for 2000~2099
 *
 * \internal
 * \par modification history:
 * - 1.00 20-12-20  hyz, first implementation
 * \endinternal
 */
#include "apollo.h"
#include "awbus_lite.h"
#include "awbl_i2cbus.h"
#include "awbl_rtc.h"
#include "time.h"
#include "aw_vdebug.h"
#include "driver/rtc/aw_sd3078_rtc.h"
#include "driver/rtc/awbl_sd3078.h"


/******************************************************************************/
/* declare SD3078 device instance */
#define __SD3078_DEV_DECL(p_this, p_dev) \
    struct awbl_sd3078_dev *p_this = \
        (struct awbl_sd3078_dev *)(p_dev)

/* declare sd3078 device infomation */
#define __SD3078_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_sd3078_devinfo *p_devinfo = \
        (struct awbl_sd3078_devinfo *)AWBL_DEVINFO_GET(p_dev)

#define __SD3078_DEV_LOCK_INIT(p_dev) \
    do { \
        AW_MUTEX_INIT(((struct awbl_sd3078_dev *)(p_dev))->devlock, \
                         AW_SEM_Q_PRIORITY ); \
    } while(0)

#define __SD3078_DEV_LOCK(p_dev) \
    do { \
        AW_MUTEX_LOCK(((struct awbl_sd3078_dev *)(p_dev))->devlock, \
                      AW_SEM_WAIT_FOREVER); \
    } while(0)

#define __SD3078_DEV_UNLOCK(p_dev) \
    do { \
        AW_MUTEX_UNLOCK(((struct awbl_sd3078_dev *)(p_dev))->devlock); \
    } while(0)

/*******************************************************************************
  forward declarations
*******************************************************************************/
aw_local void __sd3078_inst_init1(awbl_dev_t *p_dev);
aw_local void __sd3078_inst_init2(awbl_dev_t *p_dev);
aw_local void __sd3078_inst_connect(awbl_dev_t *p_dev);

aw_local aw_err_t __sd3078_time_get (void *p_cookie, aw_tm_t *p_time);
aw_local aw_err_t __sd3078_time_set (void *p_cookie, aw_tm_t *p_time);
aw_local aw_err_t __sd3078_dev_ctrl (void *p_cookie, int req, void *arg);

aw_local aw_err_t __sd3078_rtcserv_get (struct awbl_dev *p_dev, void *p_arg);

/*******************************************************************************
  locals
*******************************************************************************/

/* driver functions (must defined as aw_const) */
aw_local aw_const struct awbl_drvfuncs __g_sd3078_drvfuncs = {
    __sd3078_inst_init1,
    __sd3078_inst_init2,
    __sd3078_inst_connect
};

/* sd3078 service functions (must defined as aw_const) */
aw_local aw_const struct awbl_rtc_servopts __g_sd3078_servopts = {
    __sd3078_time_get,                     /* time_get */
    __sd3078_time_set,                     /* time_set */
    __sd3078_dev_ctrl,
    NULL,
    NULL,
};

/* driver methods (must defined as aw_const) */
AWBL_METHOD_IMPORT(awbl_rtcserv_get);

aw_local aw_const struct awbl_dev_method __g_sd3078_dev_methods[] = {
    AWBL_METHOD(awbl_rtcserv_get, __sd3078_rtcserv_get),
    AWBL_METHOD_END
};

/* driver registration (must defined as aw_const) */
aw_local aw_const awbl_i2c_drvinfo_t __g_sd3078_drv_registration = {
    {
        AWBL_VER_1,                     /* awb_ver */
        AWBL_BUSID_I2C,                 /* bus_id */
        AWBL_SD3078_NAME,               /* p_drvname */
        &__g_sd3078_drvfuncs,           /* p_busfuncs */
        &__g_sd3078_dev_methods[0],     /* p_methods */
        NULL                            /* pfunc_drv_probe */
    }
};

/*******************************************************************************
  implementation
*******************************************************************************/
aw_local aw_err_t __sd3078_reg_write (void *p_cookie,
                                      uint8_t reg_addr,
                                      uint8_t *pbuf,
                                      uint32_t len)
{
    __SD3078_DEV_DECL(p_this, p_cookie);
    __SD3078_DEVINFO_DECL(p_devinfo, p_cookie);

    if (awbl_i2c_write((struct awbl_i2c_device *)p_this,
                       AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE,
                       p_devinfo->addr,
                       reg_addr,
                       pbuf,
                       len) != AW_OK) {
        return -AW_EIO;
    }

    return AW_OK;
}

/*******************************************************************************/
aw_local aw_err_t __sd3078_reg_read (void *p_cookie,
                                     uint8_t reg_addr,
                                     uint8_t *pbuf,
                                     uint32_t len)
{
    __SD3078_DEV_DECL(p_this, p_cookie);
    __SD3078_DEVINFO_DECL(p_devinfo, p_cookie);

    if (awbl_i2c_read((struct awbl_i2c_device *)p_this,
                       AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE,
                       p_devinfo->addr,
                       reg_addr,
                       pbuf,
                       len) != AW_OK) {
        return -AW_EIO;
    }

    return AW_OK;
}


/*******************************************************************************/
aw_local aw_err_t __sd3078_write_disable(void *p_cookie)
{
    aw_err_t    ret = -1;
    uint8_t     ctr[3] = {0};

    ret = __sd3078_reg_read (p_cookie, __SD3078_REG_CTR1, (uint8_t *)&ctr, sizeof(ctr));
    if (ret != AW_OK) {
        return -AW_EIO;
    }

    ctr[0] &= 0x7B;      /* 清WRTC3，WRTC2 */
    ctr[1] &= 0x7F;      /* 清WRTC1 */

    /* 先清WRTC2、3为0 */
    ret = __sd3078_reg_write (p_cookie, __SD3078_REG_CTR1, &ctr[0], 1);
    if (ret != AW_OK) {
        return -AW_EIO;
    }

    /* 再清WRTC1为0 */
    ret = __sd3078_reg_write (p_cookie, __SD3078_REG_CTR2, &ctr[1], 1);
    if (ret != AW_OK) {
        return -AW_EIO;
    }

    return AW_OK;
}


/*******************************************************************************/
aw_local aw_err_t __sd3078_write_enable(void *p_cookie)
{
    aw_err_t    ret = -1;
    uint8_t     ctr[3] = {0};

    ret = __sd3078_reg_read (p_cookie, __SD3078_REG_CTR1, (uint8_t *)&ctr, sizeof(ctr));
    if (ret != AW_OK) {
        return -AW_EIO;
    }

    ctr[0] |= 0x84;      /* 置WRTC3，WRTC2 */
    ctr[1] |= 0x80;      /* 置WRTC1 */

    /* 写使能时，先置WRTC1为1 */
    ret = __sd3078_reg_write (p_cookie, __SD3078_REG_CTR2, &ctr[1], 1);
    if (ret != AW_OK) {
        return -AW_EIO;
    }

    /* 再置WRTC2、3为1 */
    ret = __sd3078_reg_write (p_cookie, __SD3078_REG_CTR1, &ctr[0], 1);
    if (ret != AW_OK) {
        return -AW_EIO;
    }

    return AW_OK;
}


/******************************************************************************/
aw_local aw_err_t __sd3078_time_get (void *p_cookie, aw_tm_t *p_tm)
{
    aw_err_t    ret = -1;
    uint8_t     data[7];

    ret = __sd3078_reg_read (p_cookie, __SD3078_REG_SEC, (uint8_t *)&data, sizeof(data));
    if (ret != AW_OK) {
       return -AW_EIO;
    }

    /* 清除24小时制标志位对时间转换的影响 */
    data[2] &= 0x7F;

    p_tm->tm_sec  = AW_BCD_TO_HEX(data[0] & ~0x80);
    p_tm->tm_min  = AW_BCD_TO_HEX(data[1] & ~0x80);
    p_tm->tm_hour = AW_BCD_TO_HEX(data[2] & ~0xC0);
    p_tm->tm_wday = AW_BCD_TO_HEX(data[3] & ~0xC0);
    p_tm->tm_mday = AW_BCD_TO_HEX(data[4] & ~0xC0);
    p_tm->tm_mon  = AW_BCD_TO_HEX(data[5] & ~0xE0) - 1;
    p_tm->tm_year = AW_BCD_TO_HEX(data[6]);

    /* time overflow(valid 2000~2099) */
    if ((p_tm->tm_year > 0) || (p_tm->tm_year < 99)) {
        p_tm->tm_year += 100;
    }

    return AW_OK;
}

/******************************************************************************/
aw_local aw_err_t __sd3078_time_set (void *p_cookie, aw_tm_t *p_tm)
{
    aw_err_t    ret = -1;
    uint8_t     data[7];

    p_tm->tm_year = p_tm->tm_year % 100;
    data[0] = AW_HEX_TO_BCD(p_tm->tm_sec);
    data[1] = AW_HEX_TO_BCD(p_tm->tm_min);
    data[2] = AW_HEX_TO_BCD(p_tm->tm_hour);
    data[3] = AW_HEX_TO_BCD(p_tm->tm_wday);
    data[4] = AW_HEX_TO_BCD(p_tm->tm_mday);
    data[5] = AW_HEX_TO_BCD(p_tm->tm_mon + 1);
    data[6] = AW_HEX_TO_BCD(p_tm->tm_year);

    /* 默认使用24小时制 */
    data[2] |= 0x80;

    /* time overflow(valid 2000~2099) */
    if ((p_tm->tm_year < 0) || (p_tm->tm_year > 99)) {
        return -AW_ETIME;
    }

    ret = __sd3078_write_enable(p_cookie);
    if (ret != AW_OK) {
        aw_kprintf("[SD3078]:set write enable failed.\r\n");
        return -AW_ETIME;
    }

    ret = __sd3078_reg_write (p_cookie, __SD3078_REG_SEC, (uint8_t *)&data, sizeof(data));
    if (ret != AW_OK) {
       return -AW_EIO;
    }

    ret = __sd3078_write_disable(p_cookie);
    if (ret != AW_OK) {
        aw_kprintf("[SD3078]:set write disable failed.\r\n");
        return -AW_ETIME;
    }

    return AW_OK;
}


/******************************************************************************/
aw_local aw_err_t __sd3078_alarm_time_set (void *p_cookie, aw_tm_t *p_tm)
{
    aw_err_t    ret = -1;
    uint8_t     data[7];
    uint8_t     regval = 0;

    p_tm->tm_year = p_tm->tm_year % 100;
    data[0] = AW_HEX_TO_BCD(p_tm->tm_sec);
    data[1] = AW_HEX_TO_BCD(p_tm->tm_min);
    data[2] = AW_HEX_TO_BCD(p_tm->tm_hour);
    data[3] = AW_HEX_TO_BCD(p_tm->tm_wday);
    data[4] = AW_HEX_TO_BCD(p_tm->tm_mday);
    data[5] = AW_HEX_TO_BCD(p_tm->tm_mon + 1);
    data[6] = AW_HEX_TO_BCD(p_tm->tm_year);

    /* time overflow(valid 2000~2099) */
    if ((p_tm->tm_year < 0) || (p_tm->tm_year > 99)) {
        return -AW_ETIME;
    }

    /* 1.先对设备进行写使能 */
    ret = __sd3078_write_enable(p_cookie);
    if (ret != AW_OK) {
        aw_kprintf("[SD3078]:set write enable failed.\r\n");
        return -AW_ETIME;
    }

    /* 2.设置闹钟时间 */
    ret = __sd3078_reg_write (p_cookie, __SD3078_REG_ALARM_SEC, (uint8_t *)&data, sizeof(data));
    if (ret != AW_OK) {
       return -AW_EIO;
    }

    /* 3.关闭设备写使能 */
    ret = __sd3078_write_disable(p_cookie);
    if (ret != AW_OK) {
        aw_kprintf("[SD3078]:set write disable failed.\r\n");
        return -AW_ETIME;
    }

    return AW_OK;
}


/******************************************************************************/
aw_local aw_err_t __sd3078_alarm_time_get (void *p_cookie, aw_tm_t *p_tm)
{
    aw_err_t    ret = -1;
    uint8_t     data[7];

    ret = __sd3078_reg_read (p_cookie, __SD3078_REG_ALARM_SEC, (uint8_t *)&data, sizeof(data));
    if (ret != AW_OK) {
       return -AW_EIO;
    }

    /* 清除24小时制标志位对时间转换的影响 */
    data[2] &= 0x7F;

    p_tm->tm_sec  = AW_BCD_TO_HEX(data[0] & ~0x80);
    p_tm->tm_min  = AW_BCD_TO_HEX(data[1] & ~0x80);
    p_tm->tm_hour = AW_BCD_TO_HEX(data[2] & ~0xC0);
    p_tm->tm_wday = AW_BCD_TO_HEX(data[3] & ~0xC0);
    p_tm->tm_mday = AW_BCD_TO_HEX(data[4] & ~0xC0);
    p_tm->tm_mon  = AW_BCD_TO_HEX(data[5] & ~0xE0) - 1;
    p_tm->tm_year = AW_BCD_TO_HEX(data[6]);

    /* time overflow(valid 2000~2099) */
    if ((p_tm->tm_year > 0) || (p_tm->tm_year < 99)) {
        p_tm->tm_year += 100;
    }

    return AW_OK;
}



/******************************************************************************/
aw_local aw_err_t __sd3078_alarm_int_set (void *p_cookie, uint8_t *p_int)
{
    aw_err_t    ret = -1;
    uint8_t     regval = 0;

    /* 1.先对设备进行写使能 */
    ret = __sd3078_write_enable(p_cookie);
    if (ret != AW_OK) {
        aw_kprintf("[SD3078]:set write enable failed.\r\n");
        return -AW_ETIME;
    }

    /* 2.设置需要使能的中断，年、月、日、时、分、秒、星期。 */
    if (p_int) {
        regval = *p_int;
    } else {
        return -AW_EPERM;
    }
    ret = __sd3078_reg_write (p_cookie, __SD3078_REG_ALARM_ENBLE, &regval, 1);

    /* 3.使能闹钟中断，设置中断引脚中断类型，设置中断脉冲类型 */
    ret = __sd3078_reg_read (p_cookie, __SD3078_REG_CTR2, &regval, 1);
    if (*p_int == 0) {
        regval &= ~0x02;     /* 关闭时间报警中断 */
    } else {
        regval |= 0x02;      /* 使能时间报警中断 */
    }
    regval |= 0x10;          /* 设置中断输出为报警中断 */
    regval |= 0x40;          /* 设置中断输出为250ms低电平脉冲类型 */
    ret = __sd3078_reg_write (p_cookie, __SD3078_REG_CTR2, &regval, 1);

    /* 4.关闭设备写使能 */
    ret = __sd3078_write_disable(p_cookie);
    if (ret != AW_OK) {
        aw_kprintf("[SD3078]:set write disable failed.\r\n");
        return -AW_ETIME;
    }

    return AW_OK;
}


/******************************************************************************/
aw_local aw_err_t __sd3078_alarm_int_get (void *p_cookie, uint8_t *p_int)
{
    aw_err_t    ret = -1;
    uint8_t     regval = 0;

    ret = __sd3078_reg_read (p_cookie, __SD3078_REG_ALARM_ENBLE, &regval, 1);
    if (ret != AW_OK) {
       return -AW_EIO;
    }

    if (p_int) {
        *p_int = regval;
    }

    return AW_OK;
}


/******************************************************************************/
aw_local aw_err_t __sd3078_temperature_get (void *p_cookie, uint8_t *p_tmp)
{
    aw_err_t    ret = -1;
    uint8_t     regval = 0;

    ret = __sd3078_reg_read (p_cookie, __SD3078_REG_TEMP, &regval, 1);
    if (ret != AW_OK) {
       return -AW_EIO;
    }
    *p_tmp = regval;

    return AW_OK;
}


/******************************************************************************/
aw_local aw_err_t __sd3078_dev_ctrl (void *p_cookie, int req, void *arg)
{
    aw_err_t    ret = -1;
    uint8_t     regval = 0;

    switch (req) {

        case RTC_SET_ALARM_TIME:{
            aw_tm_t *p_tm = (aw_tm_t *)arg;
            ret = __sd3078_alarm_time_set (p_cookie, p_tm);
        } break;

        case RTC_GET_ALARM_TIME:{
            aw_tm_t *p_tm = (aw_tm_t *)arg;
            ret = __sd3078_alarm_time_get (p_cookie, p_tm);
        } break;

        case RTC_SET_ALARM_INT:{
            uint8_t *p_int = (aw_tm_t *)arg;
            ret = __sd3078_alarm_int_set (p_cookie, p_int);
        } break;

        case RTC_GET_ALARM_INT:{
            uint8_t *p_int = (aw_tm_t *)arg;
            ret = __sd3078_alarm_int_get (p_cookie, p_int);
        } break;

        case RTC_GET_TEMP:{
            uint8_t *p_tmp = (uint8_t *)arg;
            ret = __sd3078_temperature_get (p_cookie, p_tmp);
        } break;

        case RTC_3078_REG_TRANS:{
            aw_sd3078_trans_t *p_trans = (aw_sd3078_trans_t *)arg;
            if (p_trans->rw_flag) {
                ret = __sd3078_reg_read (p_cookie,
                                         p_trans->reg_addr,
                                         p_trans->pbuf,
                                         p_trans->buf_len);
            } else {
                ret = __sd3078_write_enable(p_cookie);
                if (ret != AW_OK) {
                    aw_kprintf("[SD3078]:set write enable failed.\r\n");
                    return -AW_ETIME;
                }
                ret = __sd3078_reg_write (p_cookie,
                                          p_trans->reg_addr,
                                          p_trans->pbuf,
                                          p_trans->buf_len);
                ret = __sd3078_write_disable(p_cookie);
                if (ret != AW_OK) {
                    aw_kprintf("[SD3078]:set write disable failed.\r\n");
                    return -AW_ETIME;
                }
            }
        } break;

        default:{
            ret = -AW_EPERM;
        } break;
    }

    return ret;
}


/**
 * \brief first level initialization routine
 *
 * \param[in] p_dev : device instance
 */
aw_local void __sd3078_inst_init1(awbl_dev_t *p_dev)
{
}

/**
 * \brief second level initialization routine
 *
 * \param[in] p_dev : device instance
 */
aw_local void __sd3078_inst_init2(awbl_dev_t *p_dev)
{
    __SD3078_DEV_LOCK_INIT(p_dev);
    return;
}

/**
 * \brief third level initialization routine
 *
 * \param[in] p_dev : device instance
 */
aw_local void __sd3078_inst_connect(awbl_dev_t *p_dev)
{
//    __SD3078_DEV_DECL(p_this, p_dev);
//    __SD3078_DEVINFO_DECL(p_devinfo, p_this);
//
//    uint8_t data[1];

    /* 初始化SD3078设备 */

    return;
}

/**
 * \brief method "awbl_sd3078serv_get" handler
 */
aw_local aw_err_t __sd3078_rtcserv_get (struct awbl_dev *p_dev, void *p_arg)
{
    __SD3078_DEV_DECL(p_this, p_dev);
    __SD3078_DEVINFO_DECL(p_devinfo, p_dev);

    struct awbl_rtc_service *p_serv = NULL;

    /* get intcltr service instance */

    p_serv = &p_this->rtc_serv;

    /* initialize the sd3078 service instance */

    p_serv->p_next      = NULL;
    p_serv->p_servinfo  = &p_devinfo->rtc_servinfo;
    p_serv->p_servopts  = &__g_sd3078_servopts;
    p_serv->p_cookie    = (void *)p_dev;

    /* send back the service instance */

    *(struct awbl_rtc_service **)p_arg = p_serv;

    return AW_OK;
}

/******************************************************************************/
void awbl_sd3078_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_sd3078_drv_registration);
}

/* end of file */
