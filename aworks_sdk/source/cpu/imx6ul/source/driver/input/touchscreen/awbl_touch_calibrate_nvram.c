/*******************************************************************************
*                                 Apollo
*                       ---------------------------
*                       innovating embedded systems
*
* Copyright (c) 2001-2015 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief touch screen calibrate virtual nvram driver source file
 *
 *
 * \internal
 * \par modification history:
 * - 1.00 18-01-10  sls, first implementation
 * \endinternal
 */


/*******************************************************************************
  includes
*******************************************************************************/
#include "apollo.h"
#include "aw_vdebug.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "awbl_nvram.h"
#include "fcntl.h"
#include "fs/aw_mount.h"
#include "io/aw_unistd.h"
#include <string.h>
#include "driver/input/touchscreen/awbl_touch_calibrate_nvram.h"


#define __TOUCH_NVRAM_DEVINFO_DECL(p_info, p_touch_nvram_dev) \
        awbl_touch_calibrate_nvram_devinfo_t *p_info = \
        (awbl_touch_calibrate_nvram_devinfo_t *)AWBL_DEVINFO_GET(p_touch_nvram_dev)

/**
 * \brief nvram获取方法
 */
aw_local aw_err_t __touch_nvram_get (  struct awbl_dev *p_dev,
                                        char            *p_name,
                                        int              unit,
                                        void            *p_buf,
                                        int              offset,
                                        int              len)
{

    int         fd;
    __TOUCH_NVRAM_DEVINFO_DECL(p_info, p_dev);
    char        *path_five = p_info->five_pathname;
    char        *path_two  = p_info->two_pathname;

    if (strcmp(p_name, "calib_two") == 0) {
        fd = aw_open(path_two, O_RDONLY, 0777);
        if (fd < 0) {
            AW_ERRF(("open file error: %d\n", fd));
            return AW_ERROR;
        }

        if (aw_read(fd, p_buf, len) != len) {
            aw_close(fd);
            AW_ERRF(("read file error\n"));
            return AW_ERROR;
        }

        aw_close(fd);
        return AW_OK;
    }else if (strcmp(p_name, "calib_five") == 0) {
        fd = aw_open(path_five, O_RDONLY , 0777);
        if (fd < 0) {
            AW_ERRF(("creat file error: %d\n", fd));
            return AW_ERROR;
        }

        if (aw_read(fd, p_buf, len) != len) {
            aw_close(fd);
            AW_ERRF(("read file error\n"));
            return AW_ERROR;
        }

        aw_close(fd);
        return AW_OK;

    }else {
        return AW_ERROR;
    }

}


/******************************************************************************/

/**
 * \brief nvram设置方法
 */
aw_local aw_err_t __touch_nvram_set (  struct awbl_dev *p_dev,
                                        char            *p_name,
                                        int              unit,
                                        char            *p_buf,
                                        int              offset,
                                        int              len)
{

    int         fd;
    __TOUCH_NVRAM_DEVINFO_DECL(p_info, p_dev);
    char        *path_five = p_info->five_pathname;
    char        *path_two  = p_info->two_pathname;

    if (strcmp(p_name, "calib_two") == 0) {
        fd = aw_open(path_two, O_RDWR | O_CREAT , 0777);
        if (fd < 0) {
            AW_ERRF(("creat file error: %d\n", fd));
            return AW_ERROR;
        }

        if (aw_write(fd, p_buf, len) != len) {
            aw_close(fd);
            AW_ERRF(("write file error\n"));
            return AW_ERROR;
        }

        aw_close(fd);
        return AW_OK;

    }else if (strcmp(p_name, "calib_five") == 0) {
        fd = aw_open(path_five, O_RDWR | O_CREAT , 0777);
        if (fd < 0) {
            AW_ERRF(("creat file error: %d\n", fd));
            return AW_ERROR;
        }

        if (aw_write(fd, p_buf, len) != len) {
            aw_close(fd);
            AW_ERRF(("write file error\n"));
            return AW_ERROR;
        }
        aw_close(fd);
        return AW_OK;
    }else {
        return AW_ERROR;
    }
}

/******************************************************************************/
AWBL_METHOD_IMPORT(awbl_nvram_get);
AWBL_METHOD_IMPORT(awbl_nvram_set);

aw_local aw_const struct awbl_dev_method __g_touch_nvram_methods[] = {
    AWBL_METHOD(awbl_nvram_get, __touch_nvram_get),
    AWBL_METHOD(awbl_nvram_set, __touch_nvram_set),
    AWBL_METHOD_END
};


/******************************************************************************/
aw_local void __touch_nvram_inst_init1 (struct awbl_dev *p_awdev)
{
    return ;
}

/******************************************************************************/
aw_local void __touch_nvram_inst_init2 (struct awbl_dev *p_awdev)
{
    return;
}

/******************************************************************************/
aw_local void __touch_nvram_inst_connect (struct awbl_dev *p_awdev)
{

    return ;
}

/******************************************************************************/
aw_local aw_const struct awbl_drvfuncs __g_touch_nvram_drvfuncs = {
        __touch_nvram_inst_init1,
        __touch_nvram_inst_init2,
        __touch_nvram_inst_connect
};


/******************************************************************************/
aw_local aw_const awbl_plb_drvinfo_t __g_touch_calibrate_nvram_drv_registration = {
    {
        AWBL_VER_1,                             /* awb_ver */
        AWBL_BUSID_PLB,                         /* bus_id */
        TOUCH_CALIBRATE_NVRAM_NAME,             /* p_drvname */
        &__g_touch_nvram_drvfuncs,              /* p_busfuncs */
        &__g_touch_nvram_methods[0],            /* p_methods */
        NULL                                    /* pfunc_drv_probe */
    }
};

/******************************************************************************/
void awbl_touch_nvram_drv_register (void)
{

    awbl_drv_register((struct awbl_drvinfo *)&__g_touch_calibrate_nvram_drv_registration);
}
