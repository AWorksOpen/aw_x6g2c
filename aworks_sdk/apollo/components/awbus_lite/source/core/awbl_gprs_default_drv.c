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

/**
 * \file
 * \brief GPRS interface implementation
 *
 * \internal
 * \par modification history:
 * - 1.00 2017.11.29  vih, first implementation
 * - 2.00 2020.11.30  vih, refactor
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/
#include "aworks.h"
#include "stdio.h"
#include "awbus_lite.h"
#include "aw_delay.h"
#include "aw_vdebug.h"

#include "awbus_lite.h"
#include "awbl_plb.h"

#include "driver/gprs/gb2312_unicode_table.h"
#include "awbl_gprs.h"
#include "awbl_gprs_default_drv.h"
#include "lwip/sockets.h"
#include "lwip/err.h"
#include "lwip/tcp.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "lwip/opt.h"
#include "lwip/netifapi.h"

#include "string.h"

#include "aw_ping.h"
#include "aw_time.h"

#if AWBL_GPRS_USB_USBH_SERIAL
#include "host/class/cdc/serial/awbl_usbh_cdc_serial.h"
#include "host/class/cdc/awbl_usbh_usbnet.h"
#else
#include "host/class/awbl_usbh_wireless.h"
#endif /* AWBL_GPRS_USB_USBH_SERIAL */

#if AWBL_GPRS_USE_LWIP_2_X
#include "lwip/ext/net_sts.h"
#endif /* AWBL_GPRS_USE_LWIP_2_X */
/*****************************************************************************/
#define PFX  "GPRS DEF"
#define __COLOR_RED        "\x1B[31m"
#define __COLOR_GREEN      "\x1B[32m"

#define __LOG_BASE(mask, color, fmt, ...)   \
    if (p_this->pfn_log_out) {\
         p_this->pfn_log_out(mask, color, PFX, __LINE__, fmt, ##__VA_ARGS__);\
    }

#define __LOG_PFX(mask, fmt, ...) \
    __LOG_BASE(mask, NULL, "[%s:%4d] " fmt "\n", \
               PFX, __LINE__, ##__VA_ARGS__)

#define __EOFF(err) ((err) - AW_GPRS_EBASE + AW_EREMOTEIO)
#define __ERRCODE(err)  (abs(err) >= AW_GPRS_EBASE ? \
           g_gprs_errcode_str[__EOFF(abs(err))] : g_gprs_errcode_str[abs(err)])

#define __EXP_EMSG(exp, err, fmt, ...)  \
    {\
        if (__ERRCODE(err) != NULL) {\
            __LOG_BASE(AW_GPRS_LOG_ERROR, __COLOR_RED,\
                    "[%s:%4d] %s", PFX, __LINE__, __ERRCODE(err));\
            if (fmt) \
                __LOG_BASE(AW_GPRS_LOG_ERROR, __COLOR_RED, fmt, ##__VA_ARGS__);\
            __LOG_BASE(1, __COLOR_RED, "\n");\
        }                                   \
        exp;                         \
    }

#define __GOTO_EMSG(where, err, fmt, ...)  \
    __EXP_EMSG(ret = err; goto where, err, fmt, ##__VA_ARGS__)

#define __RETURN_EMSG(err, fmt, ...)  \
    __EXP_EMSG(return err, err, fmt, ##__VA_ARGS__)

/*****************************************************************************/
/** \brief 打开设备通信管道   */
#define __PIPE_OPEN(com) \
    p_this->drvopr.pfn_pipe_open(p_this->usbh_info.pid,\
                                 p_this->usbh_info.vid,\
                                 com, AW_TRUE, 512, 90000);


#define __at_cmd_send(p_this, MSG)  \
        if (p_this->snd_pipe) {p_this->drvopr.pfn_at_send(p_this, MSG);}

#define __at_cmd_result_wait_ex(p_this, p_arg, arg_num, tim_over) \
    (p_this->snd_pipe ? p_this->drvopr.pfn_at_recv(p_this,  \
                                         p_this->at_buf, \
                                         p_this->at_bufsz,  \
                                         p_arg,  \
                                         arg_num,  \
                                         tim_over ? tim_over : __DEF_TIMEOUT)\
                                            : -AW_EINVAL)

#define __ITERATE(EXP, COND, ntrys, ms_delay)   \
{\
    uint8_t __ntrys = ntrys;\
    while (__ntrys) {\
        EXP;\
        if (COND) {\
            break;\
        } else {\
            __ntrys--;\
            aw_mdelay(ms_delay);\
        }\
    }\
}

#define __DEF_TIMEOUT   10000

/******************************************************************************/
#define __TIMEOUT_SEC(tick_tmp, timeout) for (tick_tmp = aw_sys_tick_get() + \
                                              aw_ms_to_ticks((timeout)*1000+1);\
                                              aw_sys_tick_get() < tick_tmp; )

#define __TIMEOUT_MSEC(tick_tmp, timeout) for (tick_tmp = aw_sys_tick_get() + \
                                               aw_ms_to_ticks(timeout + 1); \
                                               aw_sys_tick_get() < tick_tmp; )
/*****************************************************************************/
extern char *g_gprs_errcode_str[];
/*****************************************************************************/
#ifdef __VIH_DEBUG
/**
 * \brief   获取运行时间
 *
 * \param   opt = 1: 启动   opt = 0 ：结束并打印运行时间
 * \param   p_last_time ：用于保存计时时的时间
 *
 * \return  结束计算时返回计算的时间(ms)
 */
static void __get_run_time (struct awbl_gprs_dev     *p_this,
                            char                      opt,
                            u32_t                    *p_last_ms)
{
    aw_timespec_t timespec;
    u32_t         ms = 0;

    aw_timespec_get(&timespec);
    ms = timespec.tv_nsec / 1000000 + timespec.tv_sec * 1000;

    if (opt == 0) {
        __LOG_BASE(AW_GPRS_LOG_AT_RCV_TIME, NULL,
                   "\n[USE TIME] %d mS \n", ms - (*p_last_ms));
        __LOG_BASE(AW_GPRS_LOG_AT_RCV_TIME, NULL,
                   "--------------------------------------------------\n");
    }
    *p_last_ms = ms;    /* 保存毫秒 */
}
#else
#define __get_run_time(p,a,b)
#endif


/*
 * 设备获取地址函数
 * buf  ：数据缓存地址
 * addr ：要返回的转换完成的地址
 * retval ：成功返回AW_OK
 */
aw_err_t __ethernet_get_addr (char *buf, struct ip_addr *ip_addr)
{
    uint32_t temp_addr = 0, i, j;
    uint8_t temp[8];
    uint32_t addr_int[4];
    /* 检查参数合法性*/
    if(buf == NULL) {
        return -AW_EINVAL;
    }

    /*
     * 收到的地址数据为字符串，且其显示格式为网络字节序，需要转换为 小端格式
     * 例如收到的数据为0x2032A8C0，即192.168.50.32，0xC0为192，0xA8为168，0x32为50，0x20为32*/
    j = 0;

    for(i = 0;i < 8;){
        if((buf[i] >= 48) && (buf[i] <= 57)){
            temp[i] = (buf[i] - 48);
        }
        else if((buf[i] >= 65) && (buf[i] <= 70)){
            temp[i] = (buf[i] - 65 + 10);
        }
        if((buf[i + 1] >= 48) && (buf[i + 1] <= 57)){
            temp[i + 1] = (buf[i + 1] - 48);
        }
        else if((buf[i + 1] >= 65) && (buf[i + 1] <= 70)){
            temp[i + 1] = (buf[i + 1] - 65 + 10);
        }
        addr_int[j] = temp[i] * 16 + temp[i + 1];
        j++;
        i += 2;
    }

    temp_addr = (addr_int[0] << 24) |
                    (addr_int[1] << 16) | (addr_int[2] << 8) | addr_int[3];

#if AWBL_GPRS_USE_LWIP_2_X
    ip_addr_set_ip4_u32(ip_addr, temp_addr);
#else
    ip_addr->addr = temp_addr;
#endif /* AWBL_GPRS_USE_LWIP_2_X */

    return AW_OK;
}

/* 将 p_str 中的 p_substr 及其左边的字符串都去掉 */
aw_local char *__lstrip (char *p_str, char *p_substr)
{
    char *p_begin;

    p_begin = strstr(p_str, p_substr);
    if (p_begin != NULL) {
        return p_begin + strlen(p_substr);
    }
    return  NULL;
}


static aw_err_t __int_param_get (char    *p_atmsg,
                                char    *pre,
                                int      param_index,
                                int     *p_out)
{
    char *p = p_atmsg;
    int   len = strlen(p_atmsg);

    if (len <= 0) {
        return -AW_EINVAL;
    }
    if (pre) {
        p = __lstrip(p_atmsg, pre);
        if (p == NULL) {
            return -AW_EINVAL;
        }
    }
    for (int n = 0; n < param_index; n++) {
        while (1) {
            if (*p == 0) {
                return -AW_EINVAL;
            }
            if (*p++ == ',') {
                break;
            }
        }
    }

    while (p[0] == ' ') {
        if (*p == 0) {
            return -AW_EINVAL;
        }
        p++;
    }
    if (*p == ',') {
        return -AW_EINVAL;
    }

    aw_assert(p_out);
    *p_out = strtol(p, 0, 0);
    return AW_OK;
}

/* Detects device names that match the before and after delimiters. */
static aw_bool_t __device_name_detect (struct awbl_gprs_dev *p_this,
                                       char                 *p_name)
{
    char *p;
    char  sep[] = "'\" -_[](){}<>";

    p = strstr(p_this->info.devname, p_name);
    if (p == NULL) {
        return AW_FALSE;
    }
    if (p != p_this->info.devname) {
        p--;
        for (int n = 0; sep[n] != 0; n++) {
            if (*p == sep[n]) {
                break;
            }
            if (sep[n+1] == 0) {
                return AW_FALSE;
            }
        }
        p++;
    }

    p += strlen(p_name);
    if (*p == 0) {
        return AW_TRUE;
    }
    for (int n = 0; sep[n] != 0; n++) {
        if (*p == sep[n]) {
            return AW_TRUE;
        }
    }
    return AW_FALSE;
}
/*****************************************************************************/

aw_local int __at_cmd_result_wait (struct awbl_gprs_dev *p_this,
                                   char                 *p_str,
                                   uint32_t              ms_timeout)
{
    char *arg[1];

    arg[0] = p_str;

    if (ms_timeout == 0) {
        ms_timeout = __DEF_TIMEOUT;
    }

    if (p_this->snd_pipe == NULL) {
        return -AW_ESPIPE;
    }
    return p_this->drvopr.pfn_at_recv(p_this,
                                      p_this->at_buf,
                                      p_this->at_bufsz,
                                      arg,
                                      1,
                                      ms_timeout);
}

static aw_err_t __at_test (struct awbl_gprs_dev *p_this)
{
    aw_err_t err;
    err = p_this->drvopr.pfn_at_send(p_this, "AT\r");
    if (err) {
        return err;
    }
    if (__at_cmd_result_wait(p_this, "OK", 2000) != AW_OK) {
        return -AW_GPRS_EAT;
    }
    return AW_OK;
}
/*****************************************************************************/
static void __usbh_event_hdr (void *p_arg, uint8_t type)
{
    struct awbl_gprs_dev *p_this = p_arg;

    switch (type) {
        case AWBL_USBDEV_INJECT: {
            __LOG_PFX(AW_GPRS_LOG_PIPE, "Found '%s' module injected.",
                      p_this->info.devname);
            aw_assert(p_this->pipe_valid == 0);
            p_this->pipe_valid = 1;

        } break;

        case AWBL_USBDEV_EJECT: {
            __LOG_PFX(AW_GPRS_LOG_PIPE, "Found '%s' module ejected.",
                      p_this->info.devname);
            aw_assert(p_this->pipe_valid == 1);
            p_this->pipe_valid = 0;
        } break;
    }
}
/*****************************************************************************/
#if AWBL_GPRS_USB_USBH_SERIAL

aw_local int __default_pipe_send (void  *p_pipe, uint8_t *p_data, size_t dsize)
{
    int  retval;
    if (p_pipe == NULL) {
        return 0;
    }
    retval = aw_usb_serial_write_sync(p_pipe, p_data, dsize);
    if (retval < 0) {
        return 0;
    }
    return retval;
}

aw_local int __default_pipe_recv (void  *p_pipe, uint8_t *p_data, size_t dsize)
{
    int  retval;
    if (p_pipe == NULL) {
        return 0;
    }
    retval = aw_usb_serial_read_sync(p_pipe, p_data, dsize);
//    aw_assert(retval != -AW_EPIPE);  /* dsize is not align by 512 ? */
    if (retval < 0) {
        return 0;
    }
    return retval;
}

aw_local void __default_pipe_close (void *fd)
{
    awbl_usbh_serial_close(fd);
}

aw_local void *__default_pipe_open  (int         pid,
                                     int         vid,
                                     int         inum,
                                     aw_bool_t   rngbuf,
                                     int         size,
                                     int         timeout)
{
    void    *hd;
    aw_err_t ret;
    aw_tick_t tick;
    (void)rngbuf;
    (void)size;
    (void)timeout;

    __TIMEOUT_MSEC(tick, timeout) {
        hd = awbl_usbh_serial_open(pid, vid, inum);
        if (hd) {
            break;
        }
        aw_mdelay(100);
    }
    if (hd) {
        uint32_t ms;
        /* 读总超时设置 ms */
        ms = 300;
        ret = aw_usb_serial_timeout_set(hd, &ms);
        if (AW_OK != ret) {
            __default_pipe_close(hd);
            return NULL;
        }
    }
    return hd;
}

aw_local u32_t __default_sio_write (void *fd, u8_t *data, u32_t len)
{
    int  retval;
    retval = aw_usb_serial_write_sync(fd, data, len);
    if (retval < 0) {
        return 0;
    }
    return retval;
}

aw_local u32_t __default_sio_read (void *fd, u8_t *data, u32_t len)
{
    int  retval;
    retval = aw_usb_serial_read_sync(fd, data, len);
    if (retval < 0) {
        return 0;
    }
    return retval;
}


#else

aw_local int __default_pipe_send (void  *p_pipe, uint8_t *p_data, size_t dsize)
{
    int  retval;
    if (p_pipe == NULL) {
        return 0;
    }
    retval = awbl_usbh_wireless_pipe_write(p_pipe, p_data, dsize, 1000);
    if (retval < 0) {
        return 0;
    }
    return retval;
}

aw_local int __default_pipe_recv (void  *p_pipe, uint8_t *p_data, size_t dsize)
{
    int  retval;
    if (p_pipe == NULL) {
        return 0;
    }
    retval = awbl_usbh_wireless_pipe_read(p_pipe, p_data, dsize, 100);
    if (retval < 0) {
        return 0;
    }
    return retval;
}

aw_local void *__default_pipe_open  (int         pid,
                                     int         vid,
                                     int         inum,
                                     aw_bool_t   rngbuf,
                                     int         size,
                                     int         timeout)
{
    return awbl_usbh_wireless_pipe_open(pid, vid, inum, rngbuf, size, timeout);
}

aw_local void __default_pipe_close (void * fd)
{
    awbl_usbh_wireless_pipe_close(fd);
}

aw_local u32_t __default_sio_write (void *fd, u8_t *data, u32_t len)
{
    int  retval;
    aw_assert(fd);
    retval = awbl_usbh_wireless_pipe_write((void *)fd, data, len, 5000);
    if (retval < 0) {
        return 0;
    }
    return retval;
}

aw_local u32_t __default_sio_read (void *fd, u8_t *data, u32_t len)
{
    int  retval;
    aw_assert(fd);
    retval = awbl_usbh_wireless_pipe_read((void *)fd, data, len, 5000);
    if (retval < 0) {
        return 0;
    }
    return retval;
}
#endif /* AWBL_GPRS_USB_USBH_SERIAL */


/*****************************************************************************/
/** \brief 查询网络运营商，返回0是移动， 返回1是联通 */
aw_local aw_err_t __apn_get_by_cops (struct awbl_gprs_dev *p_this)
{
    char *p_res_dat[4];
    int   ret;

#if 1
    /* 设置格式  */
    __at_cmd_send(p_this, "AT+COPS=3,0\r");
    ret = __at_cmd_result_wait(p_this, "OK", 2000);
#endif

    __at_cmd_send(p_this, "AT+COPS?\r");
    p_res_dat[0] = "MOBILE";                /* 移动 */
    p_res_dat[1] = "UNICOM";                /* 联通 */
    p_res_dat[2] = "CHINA TELECOM";         /* 电信 */
    p_res_dat[3] = "CHN-CT";                /* 电信 */

    ret = __at_cmd_result_wait_ex(p_this, p_res_dat, 4, 8000);
    if (ret == 0) {
        p_this->info.p_apn = "CMNET";
    } else if (ret == 1) {
        p_this->info.p_apn = "3GNET";
    } else if (ret == 2 || ret == 3) {
        p_this->info.p_apn = "CTNET";
    } else {
        if (ret == -AW_EBADF) {
            aw_mdelay(__DEF_TIMEOUT);
        }
        return ret;
    }

    return AW_OK;
}

/** \brief 查询国际移动台设备标识 IMSI */
aw_local aw_err_t __apn_get_by_cimi (struct awbl_gprs_dev *p_this)
{
    char *p_res_dat[16];
    int   res_cnt = 0;
    int   ret;

    struct {
        char    *mnc;
        char    *apn;
    } info[] = {
          {"46000",   "CMNET"}, {"46002",   "CMNET"}, {"46003",   "CMNET"},
          {"46007",   "CMNET"}, {"46008",   "CMNET"},

          {"46004",   "CMIOT"},

          {"46001",   "3GNET"}, {"46006",   "3GNET"}, {"46001",   "3GNET"},
          {"46009",   "3GNET"}, {"46010",   "3GNET"},

          {"46003",   "CTNET"}, {"46005",   "CTNET"}, {"46011",   "CTNET"},
          {"46012",   "CTNET"},
          {NULL, NULL}
    };

    for (int i = 0; info[i].mnc != NULL; i++) {
        p_res_dat[i] = info[i].mnc;
        res_cnt++;
    }

    __at_cmd_send(p_this, "AT+CIMI\r");

    ret = __at_cmd_result_wait_ex(p_this, p_res_dat, res_cnt, 0);
    if (ret >= 0) {
        p_this->info.p_apn = info[ret].apn;
    } else {
        if (ret == -AW_EBADF) {
            aw_mdelay(__DEF_TIMEOUT);
        }
        return ret;
    }

    __LOG_PFX(AW_GPRS_LOG_SYS, "Obtain the APN: %s", p_this->info.p_apn);
    return AW_OK;
}

/*****************************************************************************/
/**
 * \retval ==0  Succeed to send AT string.
 * \retval <0   errno
 */
static int __default_at_send (struct awbl_gprs_dev *p_this,
                              char                 *p_atmsg)
{
    int     len, rlen;
    uint8_t data[128];
    /* 先接收掉残留数据 */
    p_this->drvopr.pfn_pipe_recv(p_this->snd_pipe, data, sizeof(data));

    len = strlen(p_atmsg);
    rlen = p_this->drvopr.pfn_pipe_send(p_this->snd_pipe, (uint8_t *)p_atmsg,
                                        len);
    __LOG_PFX(AW_GPRS_LOG_AT_SND, "%s", p_atmsg);
    if (rlen == len) {
        return AW_OK;
    }

    if (p_this->snd_pipe == p_this->at_pipe) {
        return -AW_ESPIPE;
    } else if (p_this->snd_pipe == p_this->ppp.sio_mgn.fd) {
        return -AW_EROFS;
    }

    aw_assert(p_this->snd_pipe == p_this->gnss_pipe);
    return -AW_EPIPE;
}

/**
 * \retval >=0  Succeed to receive specified reply.
 * \retval <0   errno
 */
static int __default_at_recv (struct awbl_gprs_dev     *p_this,
                              char                     *p_buf,
                              uint32_t                  buf_size,
                              char                     *p_cond[],
                              uint32_t                  arg_num,
                              uint32_t                  wait_ms)
{
    uint32_t    j      = 0;
    uint32_t    offset = 0;
    int         cnt   = 0;
    int         ret;
    aw_tick_t   tick_tmp;

#ifdef __VIH_DEBUG
    u32_t run_time = 0;
    __get_run_time(p_this, 1, &run_time);
#endif
    aw_assert(p_this->drvopr.pfn_pipe_recv);
    memset(p_buf, 0, buf_size);

    ret = -AW_ETIME;
    __TIMEOUT_MSEC(tick_tmp, wait_ms) {
        /* gprs模块可能回返回一些上报的数据  */

        cnt = p_this->drvopr.pfn_pipe_recv(p_this->snd_pipe,
                                           (uint8_t *)p_buf + offset,
                                           buf_size - offset);

        /* 接收到数据 */
        if (cnt > 0) {
            /* 没有指定要接收的数据，则直接退出 */
            if (p_cond == NULL || arg_num == 0) {
                return AW_OK;
            }
            offset += cnt;
            if (offset >= buf_size) {
                offset = 0;
            }
            j = 0;

            __LOG_BASE(AW_GPRS_LOG_AT_RCV, NULL, "%s",
                       (offset ? p_buf + offset - cnt : 0));
            while (j < arg_num) {
                if (strstr(p_buf, p_cond[j]) != 0) {
                    ret = j;
                    goto exit;      /* 获取成功，退出*/
                }
                j++;
            }

            /* 检查响应是否为错误码  */
            for (int n = 0; n < AW_NELEMENTS(p_this->info.p_at_err); n++) {
                if (strstr(p_buf, p_this->info.p_at_err[n])) {
                    ret = -AW_EBADF;
                    goto exit;
                }
            }
        } else {
            aw_mdelay(100);
        }
    }

exit:
    if (ret >= 0) {
        __LOG_PFX(AW_GPRS_LOG_AT_RCV_SUC,
                  "Receives the specified data successfully.(index:%d)", ret);
    }

#ifdef __VIH_DEBUG
    __get_run_time(p_this, 0, &run_time);
#endif
    __LOG_BASE(AW_GPRS_LOG_AT_RCV, NULL, "\r\n");
    return ret;
}
/*****************************************************************************/

static aw_err_t __default_cgdcont_set (struct awbl_gprs_dev *p_this)
{
    char *buf  = p_this->at_buf;

    sprintf(buf, "AT+CGDCONT=1,\"IP\",\"%s\"\r", p_this->info.p_apn);
    __at_cmd_send(p_this, buf);
    if (__at_cmd_result_wait(p_this, "OK", 8000) != AW_OK) {
        sprintf(buf, "AT+CGDCONT=1,\"IP\",\"%s\"\r", p_this->info.p_apn);
        __at_cmd_send(p_this, buf);
        if (__at_cmd_result_wait(p_this, "OK", 10000) != AW_OK) {
            __RETURN_EMSG(-AW_GPRS_ECGDCONT, NULL);
        }
    }

    return AW_OK;
}


static aw_err_t __default_creg_chk (struct awbl_gprs_dev *p_this)
{
    aw_err_t     err;
    int          val = 0;

    for (int i = 0; i < 6; i++) {
        __at_cmd_send(p_this, "AT+CREG?\r");
        __at_cmd_result_wait(p_this, "+CREG:", 0);
        err = __int_param_get(p_this->at_buf, "+CREG", 1, &val);
        if (!err && val == 1) {
            return AW_OK;
        }
        aw_mdelay(__DEF_TIMEOUT);
    }
    __RETURN_EMSG(-AW_GPRS_ECREG, NULL);
}

static aw_err_t __default_cgreg_chk (struct awbl_gprs_dev *p_this)
{
    aw_err_t     err;
    int          val = 0;

    for (int i = 0; i < 6; i++) {
        __at_cmd_send(p_this, "AT+CGREG?\r");
        __at_cmd_result_wait(p_this, "+CGREG:", 0);
        err = __int_param_get(p_this->at_buf, "+CGREG", 1, &val);
        if (!err && val == 1) {
            return AW_OK;
        }
        aw_mdelay(__DEF_TIMEOUT);
    }
    __RETURN_EMSG(-AW_GPRS_ECGREG, NULL);
}

static aw_err_t __default_cereg_chk (struct awbl_gprs_dev *p_this)
{
    aw_err_t     err;
    int          val = 0;

    for (int i = 0; i < 6; i++) {
        __at_cmd_send(p_this, "AT+CEREG?\r");
        __at_cmd_result_wait(p_this, "+CEREG:", 0);
        err = __int_param_get(p_this->at_buf, "+CEREG", 1, &val);
        if (!err && val == 1) {
            return AW_OK;
        }
        aw_mdelay(__DEF_TIMEOUT);
    }
    __RETURN_EMSG(-AW_GPRS_ECEREG, NULL);
}


static aw_err_t __default_cgatt_set (struct awbl_gprs_dev *p_this)
{
    aw_err_t         err;
    int              val = -1;

    __at_cmd_send(p_this, "AT+CGATT?\r");
    __at_cmd_result_wait(p_this, "OK", 15000);
    err = __int_param_get(p_this->at_buf, "+CGATT", 1, &val);
    if (!err && val == 1) {
        return AW_OK;
    }

    __at_cmd_send(p_this, "AT+CGATT=1\r");
    if (__at_cmd_result_wait(p_this, "OK", 15000) != AW_OK) {
        __RETURN_EMSG(-AW_GPRS_ECGATT, NULL);
    }
    return AW_OK;
}

static aw_err_t __default_atd_set (struct awbl_gprs_dev *p_this)
{
    aw_err_t err;
    __at_cmd_send(p_this, "ATD*99#\r");
    err =  __at_cmd_result_wait(p_this, "CONNECT", 30000);
    if (err) {
        __RETURN_EMSG(-AW_GPRS_EATD, NULL);
    }
    return err;
}

static aw_err_t __default_ath_set (struct awbl_gprs_dev *p_this)
{
    __at_cmd_send(p_this, "ATH\r");
    return AW_OK;
}

/**
 * \brief at命令的回显设置
 *
 * \param[in]  this    设备结构体变量
 * \param[in]  ate     0：关回显；  1：开回显
 * \param[in]  atv     0：返回数字型；  1：返回字符型
 *
 * \return aw_err_t的错误类型
 */
static aw_err_t __default_at_disp_set (struct awbl_gprs_dev *p_this,
                                       char                       ate,
                                       char                       atv)
{
    aw_assert(p_this);
    char        *buf     = p_this->at_buf;
    aw_err_t     err;

    sprintf(buf, "ATE%d\r", ate);
    __at_cmd_send(p_this, buf);
    err = __at_cmd_result_wait(p_this, "OK", 0);
    if (err) {
        return err;
    }

    sprintf(buf, "ATV%d\r", atv);
    __at_cmd_send(p_this, buf);
    err = __at_cmd_result_wait(p_this, "OK", 0);
    return err;
}

static aw_err_t __default_apn_get (struct awbl_gprs_dev *p_this)
{
    aw_err_t ret;

    if (p_this->info.p_apn == NULL) {
        __ITERATE(ret = __apn_get_by_cimi(p_this), ret == AW_OK, 3, 0);
        if (ret < 0) {
            __ITERATE(ret = __apn_get_by_cops(p_this), ret == AW_OK, 1, 0);
            if (ret != AW_OK) {
                __RETURN_EMSG(-AW_GPRS_EAPN, NULL);
            }
        }
    }
    return AW_OK;
}

/*****************************************************************************/
static aw_err_t __default_ll_type_get (struct awbl_gprs_dev *p_this)
{
#ifdef AWBL_GPRS_LL_TYPE_ETH_ENABLE
    p_this->ll_type = AWBL_GPRS_LL_TYPE_ETH;
#else
    p_this->ll_type = AWBL_GPRS_LL_TYPE_PPP;
#endif /* AWBL_GPRS_LL_TYPE_PPP */
    return AW_OK;
}


static aw_err_t __default_ppp_mode_init (struct awbl_gprs_dev *p_this)
{
    aw_assert(p_this->ll_type == AWBL_GPRS_LL_TYPE_PPP);

    /* PPP模式数据交互接口 */
    p_this->ppp.sio_mgn.fd = __PIPE_OPEN(p_this->usbh_info.ppp_com);
    if (p_this->ppp.sio_mgn.fd == NULL) {
        __RETURN_EMSG(-AW_EROFS, NULL);
    }

    /* Initialize only once. */
    if (p_this->ppp.sio_mgn.pfn_sio_write == NULL) {
        p_this->ppp.pfn_dial_up     = p_this->drvopr.pfn_ppp_dial_up;
        p_this->ppp.pfn_dial_down   = p_this->drvopr.pfn_ppp_dial_down;
        /* 设置管道读写函数*/
        p_this->ppp.sio_mgn.pfn_sio_read  = p_this->drvopr.pfn_sio_read;
        p_this->ppp.sio_mgn.pfn_sio_write = p_this->drvopr.pfn_sio_write;

        /*
         * 创建 sio_mgn 设备
         */
        p_this->ppp.sio_mgn.serial_cmd_state        = AW_FALSE;
        p_this->ppp.sio_mgn.flow_statistic_download = 0;
        p_this->ppp.sio_mgn.flow_statistic_upload   = 0;

        sys_sem_new(&p_this->ppp.sio_mgn.serial_abort_sem, 0);

        /* 添加ppp的管理器 */
        sio_list_add(&p_this->ppp.sio_mgn);

#if AWBL_GPRS_USE_LWIP_1_4
        AW_SEMB_INIT(p_this->ppp.link_close, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);
#endif /* AWBL_GPRS_USE_LWIP_1_4 */

#if AWBL_GPRS_USE_LWIP_2_X
        AW_SEMB_INIT(p_this->ppp.thread_close, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);
#endif /* AWBL_GPRS_USE_LWIP_2_X */
    }
    __LOG_PFX(AW_GPRS_LOG_PIPE, "Open the PPP pipeline of %s successfully.(%d)",
              p_this->info.devname, p_this->usbh_info.ppp_com);

    return AW_OK;
}

static aw_err_t __default_ppp_mode_uninit (struct awbl_gprs_dev *p_this)
{
    aw_assert(p_this);
    aw_assert(p_this->ll_type == AWBL_GPRS_LL_TYPE_PPP);
    aw_assert(p_this->ppp.sio_mgn.pfn_sio_write);

    /* 关闭USB */
    if (p_this->ll_type == AWBL_GPRS_LL_TYPE_PPP &&
            p_this->ppp.sio_mgn.fd) {
        p_this->drvopr.pfn_pipe_close(p_this->ppp.sio_mgn.fd);
        p_this->ppp.sio_mgn.fd = NULL;
    }

#if 0
    if (p_this->ll_type == AWBL_GPRS_LL_TYPE_PPP) {
        aw_assert(p_this->ppp.sio_mgn.pfn_sio_write);
        sys_sem_free(&p_this->ppp.sio_mgn.serial_abort_sem);
        sio_list_del(&p_this->ppp.sio_mgn);
    }
#endif

    return AW_OK;
}

#if AWBL_GPRS_LL_TYPE_ETH_ENABLE

aw_local void __net_sts_evt_hdr (aw_net_sts_ctl_t    *p_ctl,
                                 void                *p_arg)
{
    aw_err_t          ret;
    aw_net_sts_evt_t *p_evt = p_arg;
    (void)p_evt;
    ret = aw_net_sts_evt_proc(p_ctl, p_arg);
    aw_assert(!ret);
}

static aw_err_t __default_eth_mode_init (struct awbl_gprs_dev *p_this)
{
    aw_assert(p_this);
    aw_assert(p_this->ll_type == AWBL_GPRS_LL_TYPE_ETH);

    if (p_this->ethernet.pfn_dial_up == NULL) {
        /* 以太网模式拨号和挂起函数*/
        p_this->ethernet.pfn_dial_up    = p_this->drvopr.pfn_eth_dial_up;
        p_this->ethernet.pfn_dial_down  = p_this->drvopr.pfn_eth_dial_down;
    }
    return AW_OK;
}

static aw_err_t __default_eth_mode_uninit (struct awbl_gprs_dev *p_this)
{
    aw_assert(p_this);
    aw_assert(p_this->ll_type == AWBL_GPRS_LL_TYPE_ETH);
    aw_assert(p_this->ethernet.pfn_dial_up != NULL);
    return AW_OK;
}

/*
 * 设置设备以太网模式设置打包数据协议：PDP(Packet Data Protocol)
 */
static aw_err_t __default_eth_dial_up (struct awbl_gprs_dev *p_this)
{
    aw_err_t                 ret;
    int                      recive_len;
    aw_tick_t                tick_tmp;
    const char              *name = AWBL_USBH_USBNET_ETHIF_NAME;
    char                    *buf = p_this->at_buf;
    struct aw_netif         *netif_temp;
    uint32_t                 temp[4];
    struct ethernet_addrs   *p_eaddrs = &p_this->ethernet.ethernetaddr;

    if (!p_this->info.p_apn) {
        ret = p_this->drvopr.pfn_apn_get(p_this);
        if (ret) {
            return ret;
        }
    }

    ret = p_this->drvopr.pfn_cereg_chk(p_this);
    if (ret) {
        ret = p_this->drvopr.pfn_cgreg_chk(p_this);
        if (ret) {
            return ret;
        }
    }

    /* 定义PDP环境*/
    sprintf(buf, "AT^NDISDUP=1,1,\"%s\"\r", p_this->info.p_apn);
    __at_cmd_send(p_this, buf);
    if (__at_cmd_result_wait(p_this, "OK", 0) != AW_OK) {
        __RETURN_EMSG(-AW_GPRS_ECGDCONT, NULL);
    }
    memset(buf, 0, sizeof(p_this->at_bufsz));
    sprintf(buf, "AT^DHCP?\r");
    /* 等待超时*/
    __TIMEOUT_SEC(tick_tmp, 15) {
        memset(buf, 0, sizeof(p_this->at_bufsz));
        sprintf(buf, "AT^DHCP?\r");
        __at_cmd_send(p_this, buf);
        recive_len = p_this->drvopr.pfn_pipe_recv(p_this->at_pipe,
                                                  (uint8_t *)buf,
                                                  p_this->at_bufsz);
        if((recive_len > 80) && ((strncmp(buf, "\r\n^DHCP:", 8) == 0))){

            /* 获取IP地址*/
            ret = __ethernet_get_addr(&buf[9], &p_eaddrs->our_ipaddr);
            if(ret != AW_OK)
                continue;
            /* 获取子网掩码*/
            ret = __ethernet_get_addr(&buf[18], &p_eaddrs->netmask);
            if(ret != AW_OK)
                continue;
            /* 网关*/
            ret = __ethernet_get_addr(&buf[27], &p_eaddrs->gateway);
            if(ret != AW_OK)
                continue;
            ret = __ethernet_get_addr(&buf[45], &p_eaddrs->dns1);
            if(ret != AW_OK)
                continue;
            ret = __ethernet_get_addr(&buf[54], &p_eaddrs->dns2);
            if(ret != AW_OK)
                continue;
            goto find_IP;
        }
    }
    __LOG_PFX(AW_GPRS_LOG_ERROR, "'%s' can not get IP address",
              p_this->info.devname);
    return -AW_GPRS_EATD;

find_IP:

#if AWBL_GPRS_USE_LWIP_2_X
    /* 临时保存地址*/
    temp[0] = p_eaddrs->our_ipaddr.u_addr.ip4.addr;
    temp[1] = p_eaddrs->netmask.u_addr.ip4.addr;
    temp[2] = p_eaddrs->gateway.u_addr.ip4.addr;
    temp[3] = p_eaddrs->dns1.u_addr.ip4.addr;

#else
    /* 临时保存地址*/
    temp[0] = p_this->ethernet.ethernetaddr.our_ipaddr.addr;
    temp[1] = p_this->ethernet.ethernetaddr.netmask.addr;
    temp[2] = p_this->ethernet.ethernetaddr.gateway.addr;
    temp[3] = p_this->ethernet.ethernetaddr.dns1.addr;
#endif /* AWBL_GPRS_USE_LWIP_2_X */
    /* 设置网卡IP地址，子网掩码，网关*/
    ret = aw_netif_ipv4_addr_set_by_name(name,
            (struct in_addr *)&p_this->ethernet.ethernetaddr.our_ipaddr,
            (struct in_addr *)&p_this->ethernet.ethernetaddr.netmask,
            (struct in_addr *)&p_this->ethernet.ethernetaddr.gateway);
    if (ret != AW_OK) {
        __RETURN_EMSG(-AW_ENOTDIR, NULL);
    }

#if AWBL_GPRS_USE_LWIP_2_X

    /**
     * 设置 dns
     */
#ifdef LWIP_DNS_FILETER
    {
        aw_net_sts_evt_t sts;

        netif_temp = aw_netif_dev_open(name);
        if(netif_temp == NULL) {
            __RETURN_EMSG(-AW_EXDEV, ": %s", name);
        }
        aw_netif_dev_close(netif_temp);

        /** 上报事件，记录下dns */
        memset(&sts, 0, sizeof(sts));
        sts.type              = AW_NET_STS_EVT_TYPE_DNS;
        sts.u.dns.p_netif     = &netif_temp->netif;
        sts.u.dns.dns_server  = p_eaddrs->dns1;

        ret = aw_net_sts_ctl_iterate(__net_sts_evt_hdr, &sts);
        if (ret == -AW_NET_STS_ECTL) {
            dns_setserver(0, &p_eaddrs->dns1);
            dns_setserver(1, &p_eaddrs->dns2);
        } else {
            aw_assert(ret == AW_OK);
        }
    }
#else
    dns_setserver(0, &p_eaddrs->dns1);
    dns_setserver(1, &p_eaddrs->dns2);
#endif /* LWIP_DNS_FILETER */


#else
    dns_setserver(0, &p_eaddrs->dns1);
    dns_setserver(1, &p_eaddrs->dns2);
#endif /* AWBL_GPRS_USE_LWIP_2_X */

    /* 打印出IP地址，子网掩码，网关*/
    __LOG_PFX(AW_GPRS_LOG_DIAL,
               "%s network interface information:\n"
               "          IP     : %d.%d.%d.%d\r\n"
               "          Netmask: %d.%d.%d.%d\r\n"
               "          Gateway: %d.%d.%d.%d\r\n"
               "          Dns1   : %d.%d.%d.%d\r\n",
            p_this->info.devname,
            ntohl(temp[0]) >> 24 , (ntohl(temp[0]) & 0x00FF0000) >> 16,
            (ntohl(temp[0]) & 0x0000FF00) >> 8, (ntohl(temp[0]) & 0x000000FF),
            ntohl(temp[1]) >> 24 , (ntohl(temp[1]) & 0x00FF0000) >> 16,
            (ntohl(temp[1]) & 0x0000FF00) >> 8, (ntohl(temp[1]) & 0x000000FF),
            ntohl(temp[2]) >> 24 , (ntohl(temp[2]) & 0x00FF0000) >> 16,
            (ntohl(temp[2]) & 0x0000FF00) >> 8, (ntohl(temp[2]) & 0x000000FF),
            ntohl(temp[3]) >> 24 , (ntohl(temp[3]) & 0x00FF0000) >> 16,
            (ntohl(temp[3]) & 0x0000FF00) >> 8, (ntohl(temp[3]) & 0x000000FF));

    /* 获取网卡设备结构体*/
    netif_temp = aw_netif_dev_open(name);
    if(netif_temp == NULL) {
        __RETURN_EMSG(-AW_EXDEV, ": %s", name);
    }

    aw_netif_up(netif_temp);
    /* 启动网卡链接*/
    aw_netif_set_link_up(netif_temp);
    /* 打开USB网卡*/
    aw_netif_io_ctl(netif_temp, AW_USBNET_OPEN, 0, NULL);
    aw_netif_dev_close(netif_temp);

    return AW_OK;
}

/* 设备以太网模式拨号挂起函数  */
static aw_err_t __default_eth_dial_down (struct awbl_gprs_dev *p_this)
{
    /* 从通用GPRS设备结构体中获取设备结构体*/
    char    *buf = p_this->at_buf;
    aw_err_t ret;

    /* 网卡 link down */
    {
        aw_netif_t *netif_temp;
        /* 获取网卡设备结构体*/
        netif_temp = aw_netif_dev_open(AWBL_USBH_USBNET_ETHIF_NAME);
        if(netif_temp == NULL) {
            __RETURN_EMSG(-AW_EXDEV, ": %s", AWBL_USBH_USBNET_ETHIF_NAME);
        }

        aw_netif_up(netif_temp);
        /* 启动网卡链接*/
        aw_netif_set_link_up(netif_temp);
        /* 打开USB网卡*/
        aw_netif_io_ctl(netif_temp, AW_USBNET_OPEN, 0, NULL);
        aw_netif_dev_close(netif_temp);
    }

    sprintf(buf, "AT^NDISDUP=0,1\r");
    __at_cmd_send(p_this, buf);
    if ((ret = __at_cmd_result_wait(p_this, "OK", 0)) != AW_OK) {
        return ret;
    }
    return AW_OK;
}
#endif /* AWBL_GPRS_LL_TYPE_ETH_ENABLE */


static aw_err_t __default_ppp_dial_up (struct awbl_gprs_dev *p_this)
{
    aw_err_t         err, err2;

    if (!p_this->info.p_apn) {
        err = p_this->drvopr.pfn_apn_get(p_this);
        if (err) {
            return err;
        }
    }

    err = p_this->drvopr.pfn_ppp_dial_up_spec_init(p_this);
    if (err) {
        goto exit;
    }

    /* Attempt attaching. */
    err = p_this->drvopr.pfn_cgdcont_set(p_this);
    if (err) {
        goto exit;
    }
    err = p_this->drvopr.pfn_cgatt_set(p_this);
    if (err) {
        goto exit;
    }
    /* Check network registration status again. */
    err  = p_this->drvopr.pfn_cereg_chk(p_this);
    if (err) {
        err2 = p_this->drvopr.pfn_cgreg_chk(p_this);
    }
    if (err && err2) {
        if (err == AW_OK) {
            err = err2;
        }
        goto exit;
    }

    awbl_gprs_snd_pipe_install(p_this, p_this->ppp.sio_mgn.fd);
    /**
     * Tests whether the pipe is in command mode.
     */
    err = p_this->drvopr.pfn_at_send(p_this, "AT\r");
    if (err) {
        goto exit;
    }
    if (__at_cmd_result_wait(p_this, "OK", 2000) != AW_OK) {
        p_this->drvopr.pfn_ppp_dial_down(p_this);
        p_this->drvopr.pfn_at_send(p_this, "AT\r");
        if (__at_cmd_result_wait(p_this, "OK", 2000) != AW_OK) {
            err = -AW_EROFS;
            goto exit;
        }
    }

    err = p_this->drvopr.pfn_atd_set(p_this);
    if (err) {
        goto exit;
    }
    __LOG_PFX(AW_GPRS_LOG_DIAL, "Succeed to dial-up.");

    awbl_gprs_snd_pipe_install(p_this, p_this->at_pipe);
    return AW_OK;

#if 0
tyr_dial_up:
    {
        aw_err_t err1;
        err1 = p_this->drvopr.pfn_atd_set(p_this);
        if (err1 == AW_OK) {
            __LOG_PFX(AW_GPRS_LOG_DIAL, "The attempt to dial directly succeeded.");
            err = AW_OK;
        }
        awbl_gprs_snd_pipe_install(p_this, p_this->at_pipe);
        return err;
    }
#endif

exit:
    awbl_gprs_snd_pipe_install(p_this, p_this->at_pipe);
    return err;
}

static aw_err_t __default_ppp_dial_down (struct awbl_gprs_dev *p_this)
{
    aw_err_t err;

    awbl_gprs_snd_pipe_install(p_this, p_this->ppp.sio_mgn.fd);
    aw_mdelay(1000);
    __at_cmd_send(p_this, "+++\r");
    aw_mdelay(1000);

    awbl_gprs_snd_pipe_install(p_this, p_this->at_pipe);
    err = p_this->drvopr.pfn_ath_set(p_this);
    return err;
}

static aw_err_t __default_init_at_cfg (struct awbl_gprs_dev *p_this)
{
    aw_err_t     err;
    char         data[256];

    __ITERATE(err = __at_test(p_this), err == AW_OK, 10, 0);
    if (err) {
        return err;
    }
    err = p_this->drvopr.pfn_at_disp_set(p_this, 0, 1);
    if (err) {
        return err;
    }
    err = p_this->drvopr.pfn_ati_get(p_this, data, sizeof(data));
    if (err) {
        return err;
    }
    if (strlen(data) > 0) {
        __LOG_PFX(AW_GPRS_LOG_SYS, "Product Identification Information:\n%s",
                  data);
    }
    err = p_this->drvopr.pfn_cfun_full_set(p_this);
    if (err) {
        return err;
    }
    return AW_OK;
}

#ifdef __VIH_DEBUG
static aw_err_t __L610_init_at_cfg (struct awbl_gprs_dev *p_this)
{
    struct awbl_usbh_serial *usb_serial =
            (struct awbl_usbh_serial *)p_this->at_pipe;
    struct awbl_usbh_function *p_fun = usb_serial->p_fun;
    int i;

    if((usb_serial == NULL) || (p_fun == NULL)) {
        return -AW_EINVAL;
    }
    for(i = 0;i < 7;i++) {
        awbl_usbh_ctrl_sync(&p_fun->p_udev->ep0, 0x21, 0x22, 0, i, 0, NULL,
                            5000, 0);
        awbl_usbh_ctrl_sync(&p_fun->p_udev->ep0, 0x21, 0x22, 1, i, 0, NULL,
                            5000, 0);
    }

    return __default_init_at_cfg(p_this);
}
#endif /* __VIH_DEBUG */

static aw_err_t __default_ati_get (struct awbl_gprs_dev  *p_this,
                                   char                  *p_data,
                                   size_t                 dsize)
{
    aw_err_t     err;
    __at_cmd_send(p_this, "ATI\r");
    err = __at_cmd_result_wait(p_this, "OK", 0);

    if (!err && p_data) {
        /* If the default function is used,
         * the command is issued without parsing. */
        memcpy(p_data, p_this->at_buf, min(dsize, p_this->at_bufsz));
    }
    return err;
}

static aw_err_t __default_cfun_full_set (struct awbl_gprs_dev *p_this)
{
    aw_err_t     err;
    int          val;

    __at_cmd_send(p_this, "AT+CFUN?\r");
    err = __at_cmd_result_wait(p_this, "+CFUN:", 0);
    if (err) {
        return err;
    }
    err = __int_param_get(p_this->at_buf, "+CFUN:", 0, &val);
    if (!err && val == 1) {
        return AW_OK;
    }

    __at_cmd_send(p_this, "AT+CFUN=1\r");
    err = __at_cmd_result_wait(p_this, "OK", 0);
    if (err) {
        return err;
    }

    return AW_OK;
}
static aw_err_t __default_probe (struct awbl_gprs_dev *p_this)
{
    if (p_this->drvopr.pfn_hw_power_on) {
        p_this->drvopr.pfn_hw_power_on();
    }

    if (p_this->pipe_monitor == NULL) {
        p_this->pipe_monitor = awbl_usbh_dev_monitor_register(
                p_this->usbh_info.vid,
                p_this->usbh_info.pid,
                AWBL_USBH_DEV_UNKNOWN,
                AWBL_USBDEV_EJECT | AWBL_USBDEV_INJECT,
                __usbh_event_hdr, p_this);
        aw_assert(p_this->pipe_monitor);
    }

    if (p_this->pipe_valid) {
        return AW_OK;
    }

    return -AW_EPERM;
}
static aw_err_t __default_csq_get (struct awbl_gprs_dev *p_this, int *p_csq)
{
    char        *buf     = p_this->at_buf;
    aw_err_t     err;

    for (int i = 0; i < 5; i++) {
        __at_cmd_send(p_this, "AT+CSQ\r");
        err = __at_cmd_result_wait(p_this, "+CSQ:", 0);
        if (err == AW_OK) {
            /* 获取信号值 */
            *p_csq = (uint8_t)strtol(__lstrip(buf, "+CSQ:"), 0, 0);
            if (*p_csq != 99) {
                break;
            }
        }
        aw_mdelay(1500);
    }
    if (err) {
        __RETURN_EMSG(-AW_GPRS_ECSQR, NULL);
    }

    /* 如果遇到 +CSQ: 99 则表示没有信号 */
    if (*p_csq == 99) {
        *p_csq = 0;
    }
    return AW_OK;
}
static aw_err_t __default_cpin_chk (struct awbl_gprs_dev *p_this)
{
    aw_err_t err;
    for (int n = 0; n  < 3; n++) {
        __at_cmd_send(p_this, "AT+CPIN?\r");
        err = __at_cmd_result_wait(p_this, "READY", 0);
        if(err == AW_OK) {
            return AW_OK;
        } else if (err == -AW_EBADF) {
            aw_mdelay(3000);
        }
    }
    __RETURN_EMSG(-AW_GPRS_EPIN, NULL);
}



static aw_err_t __default_tcpudp_conn (struct awbl_gprs_dev   *p_gprs,
                               uint8_t                 mode,
                               char                   *p_ip,
                               uint16_t                port,
                               uint32_t                timeout)
{
    return -AW_ENOTSUP;
}

/** \brief 关闭GPRS连接 */
aw_local aw_err_t __default_tcpudp_disconn (struct awbl_gprs_dev *p_gprs, int hd)
{
    return -AW_ENOTSUP;
}

/**
 * \brief 使用at命令发送gprs数据，注意发送的数据里不能有回车符'\r'， 如果有则需要自行进行转义掉
 *
 * \param  p_gprs_dev     gprs设备结构体变量
 * \param  p_dat          要发送的数据
 *
 * \retval <0   查看 gprs 错误码或 aw_errno
 * \retval >=0  发送成功的数据长度
 */
aw_local int __default_tcpudp_send (struct awbl_gprs_dev *p_gprs,
                                         int                   hd,
                                         void                 *p_dat,
                                         size_t                len,
                                         uint32_t              timeout)
{
    return -AW_ENOTSUP;
}
/**
 * \retval <0   查看 gprs 错误码或 aw_errno
 * \retval >=0  接收成功的数据长度
 */
aw_local int __default_tcpudp_recv (struct awbl_gprs_dev *p_gprs,
                                         int                   hd,
                                         void                 *p_buf,
                                         size_t                bsize)
{
    return -AW_ENOTSUP;
}

static aw_err_t __default_sms_text_init (struct awbl_gprs_dev *p_this)
{
    __at_cmd_send(p_this, "AT+CMGF=1\r");
    if (__at_cmd_result_wait(p_this, "OK", 0) != AW_OK) {
        __RETURN_EMSG(-AW_GPRS_ESMSF, NULL);
    }

    __at_cmd_send(p_this, "AT+CSCS=\"GSM\"\r");
    if (__at_cmd_result_wait(p_this, "OK", 0) != AW_OK) {
        __RETURN_EMSG(-AW_GPRS_ESMSF, NULL);
    }
    return AW_OK;
}
static aw_err_t __default_sms_pdu_init (struct awbl_gprs_dev *p_this)
{
    /* 设置为 pdu 格式 */
    __at_cmd_send(p_this, "AT+CMGF=0\r");
    if (__at_cmd_result_wait(p_this, "OK", 0) != AW_OK) {
        __RETURN_EMSG(-AW_GPRS_ESMSF, NULL);
    }

    __at_cmd_send(p_this, "AT+CSCS=\"UCS2\"\r");
    if (__at_cmd_result_wait(p_this, "OK", 0) != AW_OK) {
        __RETURN_EMSG(-AW_GPRS_ESMSF, NULL);
    }
    return AW_OK;
}

static aw_err_t __default_sms_first_index_get (struct awbl_gprs_dev   *p_this,
                                               int                    *p_index)
{
    char    *buf;
    long     index;
    char *tail;
    aw_assert(p_index);

    /* 读取所有短信，然后解析第一条短信，并获取到这条短信的index，
     * 再用index删除这条短信*/
    __at_cmd_send(p_this, "AT+CMGL=4\r");
    if (__at_cmd_result_wait(p_this, "\r\n+CMGL:", 0) == AW_OK) {
        /* 获取短信index */
        buf = __lstrip(p_this->at_buf, "+CMGL:");
        aw_assert(buf);
        index = strtol(buf, &tail, 0);

        /* 获取完剩余数据   */
        __at_cmd_result_wait(p_this, "\r\nOK\r\n", 1000);

        if (tail == NULL) {
            __RETURN_EMSG(-AW_GPRS_ESMSGF, NULL);
        }
        *p_index = (int)index;
    } else {
        __RETURN_EMSG(-AW_GPRS_ESMSGF, NULL);
    }
    return AW_OK;
}
static aw_err_t __default_sms_del (struct awbl_gprs_dev  *p_this, int index)
{
    sprintf(p_this->at_buf, "AT+CMGD=%d\r", index);
    __at_cmd_send(p_this, p_this->at_buf);
    if (__at_cmd_result_wait(p_this, "OK", 25000) != AW_OK) {
        __RETURN_EMSG(-AW_GPRS_ESMSD, NULL);
    }
    return AW_OK;
}

static aw_err_t __default_sms_text_send (struct awbl_gprs_dev   *p_this,
                                         uint8_t                *p_sms,
                                         size_t                  len,
                                         char                   *p_num)
{
    char        buf[370] = { 0 };

    aw_assert(p_num && p_sms && len);

    /* 英文短信最大长度 */
    if (len >= 160) {
        return -AW_EMSGSIZE;
    }

    if (__device_name_detect(p_this, "GM510")) {
        if (p_num[0] == '+') {
            p_num += 3;
        }
        sprintf(buf, "AT+CMGS=%s\r", p_num);             /* 输入电话号码 */
    } else {
        sprintf(buf, "AT+CMGS=\"%s\"\r", p_num);             /* 输入电话号码 */
    }
    __at_cmd_send(p_this, buf);
    if (__at_cmd_result_wait(p_this, "\r\n>", 60000) != AW_OK) {
        __RETURN_EMSG(-AW_GPRS_ESMSI, NULL);
    }

    if (__device_name_detect(p_this, "GM510")) {
        aw_mdelay(1000);
    }
    __at_cmd_send(p_this, (char *)p_sms);                 /* 输入短信内容 */
    buf[0] = 0x1a;                                        /* 结束符 ctrl+z */
    buf[1] = 0;
    __at_cmd_send(p_this, buf);
    if (__at_cmd_result_wait(p_this, "OK", 60000) != AW_OK) {
        /* 在这一步失败，可能欠费 */
        __RETURN_EMSG(-AW_GPRS_ESMSI, NULL);
    }
    __LOG_PFX(AW_GPRS_LOG_SMS, "Send message to '%s' successfully.", p_num);
    return AW_OK;
}

static aw_err_t __default_sms_text_recv (struct awbl_gprs_dev   *p_this,
                                         int                     index,
                                         struct aw_gprs_sms_buf *p_sms)
{
    aw_assert(p_sms);
    __RETURN_EMSG(-AW_GPRS_ESMSRC, NULL);
}

static aw_err_t __default_sms_pdu_send (struct awbl_gprs_dev   *p_this,
                                        char                   *pdu,
                                        size_t                  len)
{
    aw_const char GPRS_MSG_END[]={0x1A,0x00};        /*  短信结束 */

    aw_assert(pdu && len);

    /* 这个长度是否应该为短信内容长度（除短信息中心号码 Service Center Address）*/
    sprintf(p_this->at_buf, "AT+CMGS=%d\r", len/2 - 1);
    __at_cmd_send(p_this, p_this->at_buf);
    if (__at_cmd_result_wait(p_this, "\r\n>", 60000) != AW_OK) {
        __RETURN_EMSG(-AW_GPRS_ESMSI, NULL);
    }
    strcat(pdu, GPRS_MSG_END);
#if 0
    p_this->drvopr.pfn_pipe_send(p_this->at_pipe, pdu, len);
#else
    __at_cmd_send(p_this, (char *)pdu);
#endif
    if (__at_cmd_result_wait(p_this, "OK", 60000) != AW_OK) {
        __RETURN_EMSG(-AW_GPRS_ESMSI, NULL);
    }
    __LOG_PFX(AW_GPRS_LOG_SMS, "Send message successfully.");
    return AW_OK;
}

aw_local aw_err_t __default_sms_pdu_recv (struct awbl_gprs_dev   *p_this,
                                          int                     index,
                                          uint8_t                *pdu,
                                          size_t                  len)
{
    char *buf, *buf_end;
    aw_snprintf(p_this->at_buf, p_this->at_bufsz, "AT+CMGR=%d\r", index);
    __at_cmd_send(p_this, p_this->at_buf);
    if (__at_cmd_result_wait(p_this, "\r\nOK\r\n", 0) == AW_OK) {
        buf = __lstrip(p_this->at_buf, "+CMGR:");
        if (buf == NULL) {
            __RETURN_EMSG(-AW_GPRS_ESMSRPDU, NULL);
        }
        buf = __lstrip(buf, "\r\n");
        if (buf == NULL) {
            __RETURN_EMSG(-AW_GPRS_ESMSRPDU, NULL);
        }

        while (buf[0] == '\r' || buf[0] == '\n') {
            buf++;
        }

        /* 设置结束符 */
        buf_end = strstr(buf, "\r\n");
        if (buf_end == NULL) {
            __RETURN_EMSG(-AW_GPRS_ESMSRPDU, NULL);
        }
        buf_end[0] = 0;
        memset(pdu, 0, len);
        memcpy(pdu, buf, strlen(buf));
    }
    return AW_OK;
}

static aw_err_t __ec20_https_ctx_init (struct awbl_gprs_dev *p_this)
{
    __at_cmd_send(p_this, "AT+QHTTPCFG=\"contextid\",1\r");
    if (__at_cmd_result_wait(p_this, "OK", 0) != AW_OK) {
        __RETURN_EMSG(-AW_GPRS_EHTTPSC, NULL);
    }

    __at_cmd_send(p_this, "AT+QHTTPCFG=\"responseheader\",1\r");
    if (__at_cmd_result_wait(p_this, "OK", 0) != AW_OK) {
        __RETURN_EMSG(-AW_GPRS_EHTTPSC, NULL);
    }

    __at_cmd_send(p_this, "AT+QICSGP=1,1,\"UNINET\",\"\" ,\"\" ,1\r");
    if (__at_cmd_result_wait(p_this, "OK", 0) != AW_OK) {
        __RETURN_EMSG(-AW_GPRS_EHTTPSC, NULL);
    }
    return AW_OK;
}
static aw_err_t __ec20_https_ctx_act (struct awbl_gprs_dev *p_this)
{
    /* 检查是否激活了 context 1，未激活则激活 */
    __at_cmd_send(p_this, "AT+QIACT?\r");
    if (__at_cmd_result_wait(p_this, "+QIACT: 1,1", 0) != AW_OK) {
        __at_cmd_send(p_this, "AT+QIACT=1\r");
        if (__at_cmd_result_wait(p_this, "OK", 0) != AW_OK) {
            __RETURN_EMSG(-AW_GPRS_EHTTPSA, NULL);
        }
    }

    return AW_OK;
}
static aw_err_t __ec20_https_ctx_ssl_act (struct awbl_gprs_dev *p_this)
{
    __at_cmd_send(p_this, "AT+QHTTPCFG=\"sslctxid\",1\r");
    if (__at_cmd_result_wait(p_this, "OK", 0) != AW_OK) {
        __RETURN_EMSG(-AW_GPRS_EHTTPSSSL, NULL);
    }
    __at_cmd_send(p_this, "AT+QSSLCFG=\"sslversion\",1,1\r");
    if (__at_cmd_result_wait(p_this, "OK", 0) != AW_OK) {
        __RETURN_EMSG(-AW_GPRS_EHTTPSSSL, NULL);
    }
    __at_cmd_send(p_this, "AT+QSSLCFG=\"ciphersuite\",1,0x0005\r");
    if (__at_cmd_result_wait(p_this, "OK", 0) != AW_OK) {
        __RETURN_EMSG(-AW_GPRS_EHTTPSSSL, NULL);
    }
    __at_cmd_send(p_this, "AT+QSSLCFG=\"seclevel\",1,0\r");
    if (__at_cmd_result_wait(p_this, "OK", 0) != AW_OK) {
        __RETURN_EMSG(-AW_GPRS_EHTTPSSSL, NULL);
    }
    return AW_OK;
}
static aw_err_t __ec20_https_url_dsize_get (struct awbl_gprs_dev *p_this,
                                            char                 *p_url,
                                            size_t                url_len,
                                            size_t               *p_url_dsize)
{
    char *buf       = p_this->at_buf;
    char *p_tmp       ;
    size_t bufsz    = p_this->at_bufsz;
    size_t dsize;

    aw_snprintf(buf, bufsz, "AT+QHTTPURL=%d,80\r", strlen(p_url));
    __at_cmd_send(p_this, buf);
    if (__at_cmd_result_wait(p_this, "CONNECT", 0) != AW_OK) {
        __RETURN_EMSG(-AW_GPRS_EHTTPSSZ, NULL);
    }
    aw_mdelay(50);
    if (p_this->drvopr.pfn_pipe_send(p_this->at_pipe, (uint8_t *)p_url, url_len)
            != url_len) {
        __RETURN_EMSG(-AW_GPRS_EHTTPSSZ, NULL);
    }
    if (__at_cmd_result_wait(p_this, "OK", 0) != AW_OK) {
        __RETURN_EMSG(-AW_GPRS_EHTTPSSZ, NULL);
    }

    __at_cmd_send(p_this, "AT+QHTTPGET=80\r");
    if (__at_cmd_result_wait(p_this, "+QHTTPGET", 0) != AW_OK) {
        __RETURN_EMSG(-AW_GPRS_EHTTPSSZ, NULL);
    }

    p_tmp = __lstrip(p_this->at_buf, "+QHTTPGET: 0,200,");
    if (p_tmp != NULL) {
        dsize = strtol(p_tmp, 0, 0);
        if (dsize == 0) {
            __RETURN_EMSG(-AW_GPRS_EHTTPSZERO, NULL);
        }
    } else {
        __RETURN_EMSG(-AW_GPRS_EHTTPSZERO, NULL);
    }
    /* 开始读数据 */
    __at_cmd_send(p_this, "AT+QHTTPREAD=80\r");
    if (p_this->drvopr.pfn_pipe_recv(p_this->at_pipe,
                                     (uint8_t *)p_this->at_buf, 9) >= 0) {
        if (strncmp(p_this->at_buf, "CONNECT\r\n", 9) != 0) {
            __RETURN_EMSG(-AW_GPRS_EHTTPSCONN, NULL);
        }
    } else {
        __RETURN_EMSG(-AW_GPRS_EHTTPSCONN, NULL);
    }
    return AW_OK;
}

static int __ec20_https_url_read (struct awbl_gprs_dev   *p_this,
                                  uint8_t                *p_buf,
                                  size_t                  rlen)
{
    return p_this->drvopr.pfn_pipe_recv(p_this->at_pipe, p_buf, rlen);
}


static aw_err_t __default_https_ctx_init (struct awbl_gprs_dev *p_this)
{
    return -AW_ENOTSUP;
}
static aw_err_t __default_https_ctx_act (struct awbl_gprs_dev *p_this)
{
    return -AW_ENOTSUP;
}
static aw_err_t __default_https_ctx_ssl_act (struct awbl_gprs_dev *p_this)
{
    return -AW_ENOTSUP;
}
static aw_err_t __default_https_url_dsize_get (struct awbl_gprs_dev *p_this,
                                               char                 *p_url,
                                               size_t                urllen,
                                               size_t            *p_url_dsize)
{
    return -AW_ENOTSUP;
}

static int __default_https_url_read (struct awbl_gprs_dev   *p_this,
                                     uint8_t                *p_buf,
                                     size_t                  rlen)
{
    return -AW_ENOTSUP;
}

static aw_err_t __default_ccid_get (struct awbl_gprs_dev *p_this,
                                    char                 *ccid,
                                    size_t                size)
{
    char *buf;
    for (int i = 0; i < 3; i++) {
        p_this->drvopr.pfn_pipe_recv(p_this->at_pipe,
                                     (uint8_t *)p_this->at_buf,
                                     p_this->at_bufsz);
    }

    if (__device_name_detect(p_this, "EC20")) {
        __at_cmd_send(p_this, "AT+QCCID\r");
        if (__at_cmd_result_wait(p_this, "+QCCID:", 0) == AW_OK) {
            buf = __lstrip(p_this->at_buf, "+QCCID:");
            if (buf) {
                for (int i = 0; buf[i] != '\r' && i < size - 1; i++) {
                    ccid[i] = buf[i];
                }
                return AW_OK;
            }
        }
        return -AW_ETIME;
    }

    return -AW_ENOTSUP;
}

static aw_err_t __default_imei_get (struct awbl_gprs_dev *p_this,
                                    char                 *pbuf,
                                    size_t                size)
{
    char *buf;
    for (int i = 0; i < 3; i++) {
        p_this->drvopr.pfn_pipe_recv(p_this->at_pipe,
                                     (uint8_t *)p_this->at_buf,
                                     p_this->at_bufsz);
    }

    __at_cmd_send(p_this, "AT+GSN\r");
    if (__at_cmd_result_wait(p_this, "OK", 0) == AW_OK) {
        buf = p_this->at_buf;
        if (buf) {
            if (buf[0] == '\r') {
                buf += 2;
            }

            for (int i = 0; buf[i] != '\r' && i < size - 1; i++) {
                pbuf[i] = buf[i];
            }
            return AW_OK;
        }
    }
    return -AW_ETIME;
}

static aw_err_t __default_imsi_get (struct awbl_gprs_dev *p_this,
                                    char                 *pbuf,
                                    size_t                size)
{
    char *buf;
    for (int i = 0; i < 3; i++) {
        p_this->drvopr.pfn_pipe_recv(p_this->at_pipe,
                                     (uint8_t *)p_this->at_buf,
                                     p_this->at_bufsz);
    }

    __at_cmd_send(p_this, "AT+CIMI\r");
    if (__at_cmd_result_wait(p_this, "OK", 0) == AW_OK) {
        buf = p_this->at_buf;
        if (buf) {
            if (buf[0] == '\r') {
                buf += 2;
            }
            for (int i = 0; buf[i] != '\r' && i < size - 1; i++) {
                pbuf[i] = buf[i];
            }
            return AW_OK;
        }
    }
    return -AW_ETIME;
}
static aw_err_t __default_module_version_get (struct awbl_gprs_dev *p_this,
                                              char                 *buf,
                                              size_t                size)
{
    char *pbuf;
    for (int i = 0; i < 3; i++) {
        p_this->drvopr.pfn_pipe_recv(p_this->at_pipe,
                                     (uint8_t *)p_this->at_buf,
                                     p_this->at_bufsz);
    }

    __at_cmd_send(p_this, "AT+CGMR\r");
    if (__at_cmd_result_wait(p_this, "OK", 0) == AW_OK) {
        pbuf = p_this->at_buf;
        if (pbuf) {
            if (pbuf[0] == '\r') {
                pbuf += 2;
            }
            for (int i = 0; pbuf[i] != '\r' && i < size - 1; i++) {
                buf[i] = pbuf[i];
            }
            return AW_OK;
        }
    }
    return -AW_ETIME;
}

static aw_err_t __default_radio_reboot (struct awbl_gprs_dev *p_this)
{
    __at_cmd_send(p_this, "AT+CFUN=0\r");
    if (__at_cmd_result_wait(p_this, "OK", 0) != AW_OK) {
        __RETURN_EMSG(-AW_GPRS_ECFUN0, NULL);
    }
    __at_cmd_send(p_this, "AT+CFUN=1\r");
    if (__at_cmd_result_wait(p_this, "OK", 0) != AW_OK) {
        __RETURN_EMSG(-AW_GPRS_ECFUN1, NULL);
    }
    return AW_OK;
}

static aw_err_t __default_reset_by_at (struct awbl_gprs_dev *p_this)
{
    if (__device_name_detect(p_this, "U9300C")) {
        __at_cmd_send(p_this, "AT^RESET\r");
        __at_cmd_result_wait(p_this, "OK", 0);
        return AW_OK;
    }

    __at_cmd_send(p_this, "AT+CFUN=1,1\r");
    if (__at_cmd_result_wait(p_this, "OK", 0) != AW_OK) {
        __RETURN_EMSG(-AW_GPRS_ECFUNRST, NULL);
    }
    return AW_OK;
}

static aw_err_t __default_notsup (struct awbl_gprs_dev *p_this)
{
    return -AW_ENOTSUP;
}

static aw_err_t __default_ok (struct awbl_gprs_dev *p_this)
{
    return AW_OK;
}


/*****************************************************************************/

static aw_err_t __default_gnss_nmeasrc_get (struct awbl_gprs_dev    *p_this,
                                            uint8_t                 *p_buf,
                                            size_t                   bufsize,
                                            uint32_t                 timeout)
{
    uint32_t     rlen;
    uint32_t     recv = 0;
    aw_tick_t    tick;


    if (!p_this->gnss_pipe) {
        return -AW_EPIPE;
    }

    __TIMEOUT_MSEC(tick, timeout) {
        rlen = p_this->drvopr.pfn_pipe_recv(p_this->gnss_pipe, p_buf, bufsize);
        if (rlen >= 0) {
            aw_assert(rlen <= bufsize);
            p_buf   += rlen;
            bufsize -= rlen;
            recv    += rlen;
            aw_mdelay(50);
        } else {
            return rlen;
        }

        /* buf 已经存满 */
        if (bufsize == 0) {
            break;
        }
    }
    return recv;
}
static aw_err_t __default_gnss_enable (struct awbl_gprs_dev    *p_this,
                                       aw_gprs_gnss_cfg_t      *p_gnss_cfg)
{
    return -AW_ENOTSUP;
}
static aw_err_t __ec20_gnss_enable (struct awbl_gprs_dev    *p_this,
                                    aw_gprs_gnss_cfg_t      *p_gnss_cfg)
{
    aw_err_t                 ret = -AW_ERROR;
    char                    *pbuf = p_this->at_buf;

    (void)ret;

    if (!p_this->at_pipe) {
        __RETURN_EMSG(-AW_ESPIPE, NULL);
    }
    aw_assert(!p_this->gnss_pipe);

    /* Configure BD */
    if (p_gnss_cfg && p_gnss_cfg->bd.flag_enable) {
        __RETURN_EMSG(-AW_ENOTSUP, ": BD position.");
    }

    /* Configure GLONASS */
    if (p_gnss_cfg && p_gnss_cfg->glonass.flag_enable) {
        __RETURN_EMSG(-AW_ENOTSUP, ": GLONASS position.");
    }

    do {
        /* Read current GNSS session state. */
        __at_cmd_send(p_this, "AT+QGPS?\r");
        if (__at_cmd_result_wait(p_this, "+QGPS", 0) != AW_OK) {
            __RETURN_EMSG(-AW_GPRS_EGNSSI, NULL);
        }

        if (strstr(p_this->at_buf, "+QGPS:1")) {
            /* Already turn on */
            break;
        }

        /*
         * Turn on the GNSS Engine.
         * After turning on GNSS engine, NMEA sentences will be outputted
         * from “usbnmea” port by default.
         */
        __at_cmd_send(p_this, "AT+QGPS=1\r");
        if (__at_cmd_result_wait(p_this, "OK", 0) != AW_OK) {
            __RETURN_EMSG(-AW_GPRS_EGNSSI, NULL);
        }
    } while (0);

    do {
        /* Query the current NMEA sentence out port. */
        __at_cmd_send(p_this, "AT+QGPSCFG=\"outport\"\r");
        if (__at_cmd_result_wait(p_this, "+QGPSCFG", 0) != AW_OK) {
            __RETURN_EMSG(-AW_GPRS_EGNSSI, NULL);
        }

        if (strstr(p_this->at_buf, "usbnmea")) {
            break;
        }

        /* Set it if the NMEA sentence is not output from usbnmea port. */
        __at_cmd_send(p_this, "AT+QGPSCFG=\"outport\",\"usbnmea\"\r");
        if (__at_cmd_result_wait(p_this, "OK", 0) != AW_OK) {
            __RETURN_EMSG(-AW_GPRS_EGNSSI, NULL);
        }
    } while (0);

    /* Delete assistance data to operate cold start, hot start and warm start. */
    if (p_gnss_cfg) {
        if (p_gnss_cfg->startup_mode == AW_GPRS_GNSS_STARTUP_HOT) {
            __at_cmd_send(p_this, "AT+QGPSDEL=1\r");
            if (__at_cmd_result_wait(p_this, "OK", 0) != AW_OK) {
                __RETURN_EMSG(-AW_GPRS_EGNSSI, NULL);
            }
        } else if (p_gnss_cfg->startup_mode == AW_GPRS_GNSS_STARTUP_WARM) {
            __at_cmd_send(p_this, "AT+QGPSDEL=2\r");
            if (__at_cmd_result_wait(p_this, "OK", 0) != AW_OK) {
                __RETURN_EMSG(-AW_GPRS_EGNSSI, NULL);
            }
        } else if (p_gnss_cfg->startup_mode == AW_GPRS_GNSS_STARTUP_COLD) {
            __at_cmd_send(p_this, "AT+QGPSDEL=0\r");
            if (__at_cmd_result_wait(p_this, "OK", 0) != AW_OK) {
                __RETURN_EMSG(-AW_GPRS_EGNSSI, NULL);
            }
        }
    }

    /* Configure GPS */
    if (p_gnss_cfg) {

        if (!p_gnss_cfg->gps.flag_enable) {
            __LOG_PFX(AW_GPRS_LOG_ERROR, "EC20 must enable gps.");
            __RETURN_EMSG(-AW_GPRS_EGNSSI, NULL);
        }

        /* Configure output type of GPS NMEA sentences */
        if (p_gnss_cfg->gps.nmea_type != AW_GPRS_GNSS_NMEA_AUTO) {
            aw_snprintf(pbuf,
                        p_this->at_bufsz,
                       "AT+QGPSCFG=\"gpsnmeatype\",%d\r",
                       p_gnss_cfg->gps.nmea_type);
            __at_cmd_send(p_this, pbuf);
            if (__at_cmd_result_wait(p_this, "OK", 0) != AW_OK) {
                __RETURN_EMSG(-AW_GPRS_EGNSSI, NULL);
            }
        }
    }

    return AW_OK;
}

static aw_err_t __ec20_gnss_disable (struct awbl_gprs_dev    *p_this)
{
    /* Turn off the GNSS Engine. */
    __at_cmd_send(p_this, "AT+QGPSEND\r");
    return __at_cmd_result_wait(p_this, "OK", 0);
}


/*****************************************************************************/
#ifdef __VIH_DEBUG
static void __test (struct awbl_gprs_dev *p_this)
{
    aw_err_t err;

    {
        int      val;
        err = __int_param_get("+CREG:   1,2,3", "+CREG:", 1, &val);
        aw_assert(!err && val == 2);
        err = __int_param_get("+CREG:   1,,3", "+CREG:", 1, &val);
        aw_assert(-AW_EINVAL);
        err = __int_param_get("+CREG:   1,2,3", "+CEG:", 1, &val);
        aw_assert(-AW_EINVAL);
        err = __int_param_get("+CREG:   1,2,3", "+CEG:", 3, &val);
        aw_assert(-AW_EINVAL);
        err = __int_param_get("+CREG:   1,2,3", "+CEG:", 3, &val);
        aw_assert(-AW_EINVAL);
        val = -1;
        err = __int_param_get("+CREG:   ,2,3", "+CEG:", 0, &val);
        aw_assert(-AW_EINVAL);
    }
    {
        struct awbl_gprs_dev dev;

        dev.info.devname = "ASDBSDFBSDB EC20";
        aw_assert(__device_name_detect(&dev, "EC20") == 1);

        dev.info.devname = "ASDBSDFBSDB EC20 ";
        aw_assert(__device_name_detect(&dev, "EC20") == 1);

        dev.info.devname = "ASDBSDFBSDB EC20-";
        aw_assert(__device_name_detect(&dev, "EC20") == 1);

        dev.info.devname = "ASDBSDFBSDB EC20 ";
        aw_assert(__device_name_detect(&dev, "EC20") == 1);

        dev.info.devname = "ASDBSDFBSDB EC20_";
        aw_assert(__device_name_detect(&dev, "EC20") == 1);

        dev.info.devname = "ASDBSDFBSDB 'EC20'";
        aw_assert(__device_name_detect(&dev, "EC20") == 1);

        dev.info.devname = "ASDBSDFBSDB-EC20";
        aw_assert(__device_name_detect(&dev, "EC20") == 1);

        dev.info.devname = "ASDBSDFBSDB-EC20}";
        aw_assert(__device_name_detect(&dev, "EC20") == 1);

        dev.info.devname = "ASDBSDFBSDB EC200_";
        aw_assert(__device_name_detect(&dev, "EC20") == 0);

        dev.info.devname = "ASDBSDFBSDB EC20T";
        aw_assert(__device_name_detect(&dev, "EC20") == 0);
        dev.info.devname = "ASDBSDFBSDB EEC20T";
        aw_assert(__device_name_detect(&dev, "EC20") == 0);
        dev.info.devname = "ASDBSDFBSDB EEC20";
        aw_assert(__device_name_detect(&dev, "EC20") == 0);
    }
}

static void __callback_check (struct awbl_gprs_dev *p_this)
{
    int size = sizeof(p_this->drvopr);
    int cnt;
    void **pfn;

    cnt = size / sizeof(pfn);
    pfn = (void **)&p_this->drvopr;
    for (int i = 0; i < cnt; i++, pfn++) {
        aw_assert(*pfn != NULL);
    }
}
#endif

/*****************************************************************************/
aw_err_t awbl_gprs_default_init (struct awbl_gprs_dev *p_this)
{
    if (!p_this->info.devname) {
        p_this->info.devname = "Unknown";
    }
    if (!p_this->info.power_on_mdelay) {
        p_this->info.power_on_mdelay = 10000;
    }
    if (!p_this->info.sf_reset_mdelay) {
        p_this->info.sf_reset_mdelay = 60000;
    }

    if (!p_this->info.p_at_err) {
        aw_assert(AW_NELEMENTS(p_this->info.p_at_err) >= 3);
        p_this->info.p_at_err[0] = "ERROR";
        p_this->info.p_at_err[1] = "+CME";
        p_this->info.p_at_err[2] = "+CMS";
    }

    if (!p_this->sms.p_num_buff) {
        p_this->sms.p_num_buff      = malloc(32);
        p_this->sms.num_buff_len    = 32;
    }
    if (!p_this->sms.p_sms_buff) {
        aw_assert(p_this->sms.sms_buff_len == 0);
        p_this->sms.p_sms_buff      = malloc(512);
        p_this->sms.sms_buff_len    = 512;
    } else {
        aw_assert(p_this->sms.sms_buff_len <= 1023);
    }
    if (!p_this->sms.p_time_buff) {
        p_this->sms.p_time_buff      = malloc(64);
        p_this->sms.time_buff_len    = 64;
    }

    if (!p_this->at_buf) {
        p_this->at_buf = malloc(1024);
        p_this->at_bufsz = 1024;
        aw_assert(p_this->at_buf);
    }

    return AW_OK;
}

aw_err_t awbl_gprs_default_drvopr_set (struct awbl_gprs_dev *p_this)
{
    aw_assert(p_this);

    if (!p_this->drvopr.pfn_ll_type_get) {
        p_this->drvopr.pfn_ll_type_get = __default_ll_type_get;
    }

    if (!p_this->drvopr.pfn_ppp_mode_init) {
        p_this->drvopr.pfn_ppp_mode_init = __default_ppp_mode_init;
    }
    if (!p_this->drvopr.pfn_ppp_mode_uninit) {
        p_this->drvopr.pfn_ppp_mode_uninit = __default_ppp_mode_uninit;
    }
    if (!p_this->drvopr.pfn_ppp_dial_up_spec_init) {
        p_this->drvopr.pfn_ppp_dial_up_spec_init = __default_ok;
    }

    if (!p_this->drvopr.pfn_ppp_dial_up) {
        p_this->drvopr.pfn_ppp_dial_up = __default_ppp_dial_up;
    }
    if (!p_this->drvopr.pfn_ppp_dial_down) {
        p_this->drvopr.pfn_ppp_dial_down = __default_ppp_dial_down;
    }

#if AWBL_GPRS_LL_TYPE_ETH_ENABLE
    if (!p_this->drvopr.pfn_eth_mode_init) {
        p_this->drvopr.pfn_eth_mode_init = __default_eth_mode_init;
    }
    if (!p_this->drvopr.pfn_eth_mode_uninit) {
        p_this->drvopr.pfn_eth_mode_uninit = __default_eth_mode_uninit;
    }
    if (!p_this->drvopr.pfn_eth_dial_up) {
        p_this->drvopr.pfn_eth_dial_up = __default_eth_dial_up;
    }
    if (!p_this->drvopr.pfn_eth_dial_down) {
        p_this->drvopr.pfn_eth_dial_down = __default_eth_dial_down;
    }
#endif /* AWBL_GPRS_LL_TYPE_ETH_ENABLE */

    if (!p_this->drvopr.pfn_sio_read) {
        p_this->drvopr.pfn_sio_read = __default_sio_read;
    }
    if (!p_this->drvopr.pfn_sio_write) {
        p_this->drvopr.pfn_sio_write = __default_sio_write;
    }
    if (!p_this->drvopr.pfn_apn_get) {
        p_this->drvopr.pfn_apn_get = __default_apn_get;
    }
    if (!p_this->drvopr.pfn_cgdcont_set) {
        p_this->drvopr.pfn_cgdcont_set = __default_cgdcont_set;
    }
    if (!p_this->drvopr.pfn_cgatt_set) {
        p_this->drvopr.pfn_cgatt_set = __default_cgatt_set;
    }
    if (!p_this->drvopr.pfn_atd_set) {
        p_this->drvopr.pfn_atd_set = __default_atd_set;
    }
    if (!p_this->drvopr.pfn_ath_set) {
        p_this->drvopr.pfn_ath_set = __default_ath_set;
    }
    if (!p_this->drvopr.pfn_creg_chk) {
        p_this->drvopr.pfn_creg_chk = __default_creg_chk;
    }
    if (!p_this->drvopr.pfn_cereg_chk) {
        p_this->drvopr.pfn_cereg_chk = __default_cereg_chk;
    }
    if (!p_this->drvopr.pfn_cgreg_chk) {
        p_this->drvopr.pfn_cgreg_chk = __default_cgreg_chk;
    }
    if (!p_this->drvopr.pfn_at_disp_set) {
        p_this->drvopr.pfn_at_disp_set = __default_at_disp_set;
    }
    if (!p_this->drvopr.pfn_ati_get) {
        p_this->drvopr.pfn_ati_get = __default_ati_get;
    }
    if (!p_this->drvopr.pfn_init_at_cfg) {

#ifdef __VIH_DEBUG
        if (__device_name_detect(p_this, "L610")) {
            p_this->drvopr.pfn_init_at_cfg = __L610_init_at_cfg;
        } else
#endif
        {
            p_this->drvopr.pfn_init_at_cfg = __default_init_at_cfg;
        }

    }

    if (!p_this->drvopr.pfn_pipe_open) {
        p_this->drvopr.pfn_pipe_open = __default_pipe_open;
    }
    if (!p_this->drvopr.pfn_pipe_close) {
        p_this->drvopr.pfn_pipe_close = __default_pipe_close;
    }
    if (!p_this->drvopr.pfn_pipe_send) {
        p_this->drvopr.pfn_pipe_send = __default_pipe_send;
    }
    if (!p_this->drvopr.pfn_pipe_recv) {
        p_this->drvopr.pfn_pipe_recv = __default_pipe_recv;
    }

    if (!p_this->drvopr.pfn_at_send) {
        p_this->drvopr.pfn_at_send = __default_at_send;
    }
    if (!p_this->drvopr.pfn_at_recv) {
        p_this->drvopr.pfn_at_recv = __default_at_recv;
    }
    if (!p_this->drvopr.pfn_gnss_enable) {
        p_this->drvopr.pfn_gnss_enable = __default_gnss_enable;
        if (__device_name_detect(p_this, "EC20")) {
            p_this->drvopr.pfn_gnss_enable = __ec20_gnss_enable;
        }
    }
    if (!p_this->drvopr.pfn_gnss_disable) {
        p_this->drvopr.pfn_gnss_disable = __default_ok;
        if (__device_name_detect(p_this, "EC20")) {
            p_this->drvopr.pfn_gnss_disable = __ec20_gnss_disable;
        }
    }
    if (!p_this->drvopr.pfn_gnss_nmeasrc_get) {
        p_this->drvopr.pfn_gnss_nmeasrc_get = __default_gnss_nmeasrc_get;
    }
    if (!p_this->drvopr.pfn_cfun_full_set) {
        p_this->drvopr.pfn_cfun_full_set = __default_cfun_full_set;
    }
    if (!p_this->drvopr.pfn_probe) {
        p_this->drvopr.pfn_probe = __default_probe;
    }
    if (!p_this->drvopr.pfn_csq_get) {
        p_this->drvopr.pfn_csq_get = __default_csq_get;
    }
    if (!p_this->drvopr.pfn_cpin_chk) {
        p_this->drvopr.pfn_cpin_chk = __default_cpin_chk;
    }

    if (!p_this->drvopr.pfn_init_prev) {
        p_this->drvopr.pfn_init_prev = __default_ok;
    }

    if (!p_this->drvopr.pfn_tcpudp_conn) {
        p_this->drvopr.pfn_tcpudp_conn = __default_tcpudp_conn;
    }
    if (!p_this->drvopr.pfn_tcpudp_disconn) {
        p_this->drvopr.pfn_tcpudp_disconn = __default_tcpudp_disconn;
    }
    if (!p_this->drvopr.pfn_tcpudp_send) {
        p_this->drvopr.pfn_tcpudp_send = __default_tcpudp_send;
    }
    if (!p_this->drvopr.pfn_tcpudp_recv) {
        p_this->drvopr.pfn_tcpudp_recv = __default_tcpudp_recv;
    }

    if (!p_this->drvopr.pfn_sms_text_init) {
        p_this->drvopr.pfn_sms_text_init = __default_sms_text_init;
    }
    if (!p_this->drvopr.pfn_sms_pdu_init) {
        p_this->drvopr.pfn_sms_pdu_init = __default_sms_pdu_init;
    }
    if (!p_this->drvopr.pfn_sms_text_send) {
        p_this->drvopr.pfn_sms_text_send = __default_sms_text_send;
    }
    if (!p_this->drvopr.pfn_sms_pdu_send) {
        p_this->drvopr.pfn_sms_pdu_send = __default_sms_pdu_send;
    }

    if (!p_this->drvopr.pfn_https_ctx_init) {
        p_this->drvopr.pfn_https_ctx_init = __default_https_ctx_init;
        if (__device_name_detect(p_this, "EC20")) {
            p_this->drvopr.pfn_https_ctx_init = __ec20_https_ctx_init;
        }
    }
    if (!p_this->drvopr.pfn_https_ctx_act) {
        p_this->drvopr.pfn_https_ctx_act = __default_https_ctx_act;
        if (__device_name_detect(p_this, "EC20")) {
            p_this->drvopr.pfn_https_ctx_act = __ec20_https_ctx_act;
        }
    }
    if (!p_this->drvopr.pfn_https_ctx_ssl_act) {
        p_this->drvopr.pfn_https_ctx_ssl_act = __default_https_ctx_ssl_act;
        if (__device_name_detect(p_this, "EC20")) {
            p_this->drvopr.pfn_https_ctx_ssl_act = __ec20_https_ctx_ssl_act;
        }
    }
    if (!p_this->drvopr.pfn_https_url_dsize_get) {
        p_this->drvopr.pfn_https_url_dsize_get = __default_https_url_dsize_get;
        if (__device_name_detect(p_this, "EC20")) {
            p_this->drvopr.pfn_https_url_dsize_get = __ec20_https_url_dsize_get;
        }
    }
    if (!p_this->drvopr.pfn_https_url_read) {
        p_this->drvopr.pfn_https_url_read = __default_https_url_read;
        if (__device_name_detect(p_this, "EC20")) {
            p_this->drvopr.pfn_https_url_read = __ec20_https_url_read;
        }
    }

    if (!p_this->drvopr.pfn_sms_text_recv) {
        p_this->drvopr.pfn_sms_text_recv = __default_sms_text_recv;
    }
    if (!p_this->drvopr.pfn_sms_pdu_recv) {
        p_this->drvopr.pfn_sms_pdu_recv = __default_sms_pdu_recv;
    }
    if (!p_this->drvopr.pfn_sms_first_index_get) {
        p_this->drvopr.pfn_sms_first_index_get = __default_sms_first_index_get;
    }
    if (!p_this->drvopr.pfn_sms_del) {
        p_this->drvopr.pfn_sms_del = __default_sms_del;
    }
    if (!p_this->drvopr.pfn_ccid_get) {
        p_this->drvopr.pfn_ccid_get = __default_ccid_get;
    }
    if (!p_this->drvopr.pfn_imei_get) {
        p_this->drvopr.pfn_imei_get = __default_imei_get;
    }
    if (!p_this->drvopr.pfn_imsi_get) {
        p_this->drvopr.pfn_imsi_get = __default_imsi_get;
    }
    if (!p_this->drvopr.pfn_module_version_get) {
        p_this->drvopr.pfn_module_version_get = __default_module_version_get;
    }
    if (!p_this->drvopr.pfn_radio_reboot) {
        p_this->drvopr.pfn_radio_reboot = __default_radio_reboot;
    }
    if (!p_this->drvopr.pfn_reset_by_at) {
        p_this->drvopr.pfn_reset_by_at = __default_reset_by_at;
    }

#ifdef __VIH_DEBUG
    __test(p_this);
    __callback_check(p_this);
#endif

    return AW_OK;
}

/*****************************************************************************/
/** 获取备结构体*/
#define __DEV_DECL(p_this, p_dev) \
        struct awbl_gprs_default_dev *p_this = AW_CONTAINER_OF(p_dev, \
                                                   struct awbl_gprs_default_dev, \
                                                   dev)
/** 从GPRS设备中获取设备结构体*/
#define __DECL_FROM_GPRS(p_this, p_gprs) \
        struct awbl_gprs_default_dev *p_this = \
                     (struct awbl_gprs_default_dev *)p_gprs->p_gprs_dev


static aw_err_t __gprs_drvopr_init ( struct awbl_gprs_default_dev  *p_this,
                                     struct awbl_gprs_dev          *p_gprs)
{
    return AW_OK;
}

/** 添加到GPRS服务*/
aw_local aw_err_t __dev_get (struct awbl_dev *p_dev, void *p_arg)
{
    __DEV_DECL(p_this, p_dev);

    struct awbl_gprs_dev *p_gprs = NULL;
    /* 合法性检查*/
    if (!p_dev) {
        return -AW_EINVAL;
    }
    /* 获取GPRS结构体*/
    p_gprs = &p_this->gprs;
    /* 初始化互斥锁*/
    AW_MUTEX_INIT(p_gprs->mutex, AW_SEM_Q_PRIORITY);
    /* 初始化GPRS结构体成员*/
    p_gprs->p_gprs_dev  = (void *)p_this;
    __gprs_drvopr_init(p_this, p_gprs);

    /* 把初始化完的GPRS结构体返回给GPRS服务*/
    *(struct awbl_gprs_dev **)p_arg = p_gprs;
    AW_INFOF(("%s: %s driver register.\n", AWBL_GPRS_VERSION,
            p_gprs->info.devname));
    return AW_OK;
}

aw_local void __drv_connect(struct awbl_dev *p_dev)
{
}

/** 驱动初始化函数*/
aw_local aw_const struct awbl_drvfuncs __g_drvfuncs = {
    NULL,
    NULL,
    __drv_connect,
};

/** GPRS服务METHOD*/
AWBL_METHOD_IMPORT(awbl_gprs_dev_get);

aw_local aw_const struct awbl_dev_method __g_gprs_methods[] = {
    AWBL_METHOD(awbl_gprs_dev_get, __dev_get),
    AWBL_METHOD_END
};

/** 驱动信息*/
aw_local aw_const awbl_plb_drvinfo_t __g_drv_registration = {
    {
        AWBL_VER_1,                         /* awb_ver */
        AWBL_BUSID_PLB,                     /* bus_id */
        AWBL_GPRS_DEFAULT_NAME,             /* p_drvname */
        &__g_drvfuncs,                      /* p_busfuncs */
        &__g_gprs_methods[0],               /* p_methods */
        NULL                                /* pfunc_drv_probe */
    }
};

/** 默认 4G 驱动注册函数*/
void awbl_gprs_default_drv_register (void)
{
    int ret;

    ret = awbl_drv_register((struct awbl_drvinfo *)&__g_drv_registration);
    if (ret != AW_OK) {
        return;
    }
}



