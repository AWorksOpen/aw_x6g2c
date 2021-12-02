/*
 * gw_unity_test.c
 *
 *  Created on: 2019年06月28日
 *      Author: anu
 */

#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_shell.h"
#include "aw_assert.h"

#include "string.h"
#include "stdio.h"

#include "gw_comm_config.h"
#include "wireless_module_event.h"

#include "unity.h"
#include "unity_fixture.h"
#include <setjmp.h>
#include <stdio.h>

#include "reqsched.h"


/*****************************************************************************/
/* 宏定义 */
/*****************************************************************************/
/* 宏定义 */
#define __LOG_BASE(mask, color, fmt, ...)   \
    if ((mask)) {\
         __log(color, PFX, __LINE__, fmt, ##__VA_ARGS__);\
    }

#define __LOG_PFX(mask, fmt, ...) \
    __LOG_BASE(mask, NULL, "[%s:%4d] " fmt "\n", \
               PFX, __LINE__, ##__VA_ARGS__)

#define __GW_NET_STATUS_INFOF(x)  \
    AW_INFOF(("[net_status]")); AW_INFOF(x)

#define PFX  "UNITY_TEST_NET"

#define __TIMEOUT_MSEC(tick_tmp, timeout) \
    for (tick_tmp = aw_sys_tick_get() + aw_ms_to_ticks(timeout); \
            aw_sys_tick_get() < tick_tmp; )

/* Caculate the MS interval time of current tick and given tick. */
#define __INTERVAL_MS_GET(tick)  aw_ticks_to_ms(aw_sys_tick_get() - (tick))
/******************************************************************************/

/*****************************************************************************/
/* 变量定义 */
static uint32_t __g_req1;
static uint32_t __g_req1_token;
static uint32_t __g_req2;
static uint32_t __g_req2_token;
static uint32_t __g_req3_none;
static uint32_t __g_req3_token;

typedef struct {
    uint32_t reqid;
    uint32_t token;
} req_ctx_t;

typedef enum {
   REQ_TYPE_CHECK_ELAPSE = 3,
   REQ_TYPE_OPEN,
   REQ_TYPE_CLOSE,
   REQ_TYPE_UNINIT,
   REQ_TYPE_CHECK_ORDER,
   REQ_TYPE_TEST_AW_ITF,
   REQ_TYPE_MAX = 100
} req_type_t;

static req_ctx_t __g_req[] = {
    [REQ_TYPE_CHECK_ELAPSE] = { 0 },
    [REQ_TYPE_MAX] = { .reqid = 0},
};
/*****************************************************************************/
/* 申明 */

/*****************************************************************************/
static void __log (void         *p_color,
                   const char   *func_name,
                   int           line,
                   const char   *fmt, ...)
{
    char        log_buf[512 + 1];
    uint16_t    len;
    va_list     ap;

    va_start( ap, fmt );
    len = aw_vsnprintf((char *)log_buf, sizeof(log_buf), fmt, ap);
    va_end( ap );
    if (len >= sizeof(log_buf)) {
        aw_assert(len < sizeof(log_buf) - 1);
    }

#ifdef __VIH_DEBUG
    extern int aw_shell_printf_with_color (aw_shell_user_t *p_user,
                                           const char *color,
                                           const char *func_name,
                                           const char *fmt, ...);
    aw_shell_printf_with_color(AW_DBG_SHELL_IO, p_color, func_name, log_buf);
#else
    __GW_NET_STATUS_INFOF((log_buf));
#endif
}


/*****************************************************************************/
TEST_GROUP(reqsched);

TEST_SETUP(reqsched)
{

}

TEST_TEAR_DOWN(reqsched)
{

}

static void __request1_hdr (uint32_t reqid, void *arg, size_t size)
{
    rs_err_t ret;
    aw_tick_t rsp = *(aw_tick_t *)arg;
    aw_mdelay(8700);

    __LOG_PFX(1, "req1_rsp: %d ms", __INTERVAL_MS_GET(rsp));
    rsp = aw_sys_tick_get();
    *(aw_tick_t *)arg = rsp;
    ret = reqsched_rsp_send(reqid, arg);
    aw_assert(ret == RS_OK);
}

static void __request2_hdr (uint32_t reqid, void *arg, size_t size)
{
    rs_err_t ret;
    aw_tick_t rsp = *(aw_tick_t *)arg;
    aw_mdelay(2700);

    __LOG_PFX(1, "req2_rsp: %d ms", __INTERVAL_MS_GET(rsp));
    rsp = aw_sys_tick_get();
    *(aw_tick_t *)arg = rsp;
    ret = reqsched_rsp_send(reqid, arg);
    aw_assert(ret == RS_OK);
}



static void __check_order_hdr (uint32_t reqid, void *arg, size_t size)
{
    static uint32_t _id = 0;
    rs_err_t ret;
    uint32_t id = *(uint32_t *)arg;
    aw_mdelay(1000);
    __LOG_PFX(1, "__check_order_hdr id:%d", id);
    aw_mdelay(1000);

    ret = reqsched_rsp_send(reqid, arg);
    aw_assert(ret == RS_OK);

    /* 断言 id 值是按顺序  */
    _id++;
    aw_assert(_id == id);
}

static void __open_hdr (uint32_t reqid, void *arg, size_t size)
{
    rs_err_t ret;
    aw_tick_t rsp = *(aw_tick_t *)arg;
    aw_mdelay(3700);

    __LOG_PFX(1, "open_hdr: %d ms", __INTERVAL_MS_GET(rsp));
    rsp = aw_sys_tick_get();
    *(aw_tick_t *)arg = rsp;
    ret = reqsched_rsp_send(reqid, arg);
    aw_assert(ret == RS_OK);
}

/*
 * 测试内容：初始化多个请求类型
 * 测试结果： 成功
 */
TEST(reqsched, __reqsched_init)
{
//    rs_err_t ret;

    __g_req1 = reqsched_create(__request1_hdr, 10, sizeof(int));
    aw_assert(__g_req1 > 0);
    __g_req2 = reqsched_create(__request2_hdr, 10, sizeof(int));
    aw_assert(__g_req2 > 0);

    __g_req[REQ_TYPE_OPEN].reqid = reqsched_create(__open_hdr, 10, sizeof(int));
    aw_assert(__g_req[REQ_TYPE_OPEN].reqid > 0);
}


TEST(reqsched, __reqsched_send_by_multi_type)
{
    rs_err_t ret;
    aw_tick_t tick;
    aw_tick_t req1_tick;
    aw_tick_t req2_tick;
    aw_tick_t req1_rsp;
    aw_tick_t req2_rsp;
    aw_tick_t req_rsp;

    tick = aw_sys_tick_get();

    req1_tick = aw_sys_tick_get();
    req2_tick = aw_sys_tick_get();

    ret = reqsched_req_send(__g_req[REQ_TYPE_OPEN].reqid,
                            &req1_tick,
                            sizeof(req1_tick),
                            &__g_req[REQ_TYPE_OPEN].token);
    aw_assert(ret == RS_OK);

    ret = reqsched_req_send(__g_req1,
                            &req1_tick,
                            sizeof(req1_tick),
                            &__g_req1_token);
    aw_assert(ret == RS_OK);
    ret = reqsched_req_send(__g_req2,
                            &req2_tick,
                            sizeof(req2_tick),
                            &__g_req2_token);
    aw_assert(ret == RS_OK);

    while (1) {
        aw_mdelay(100);
        ret = reqsched_rsp_tryget(__g_req[REQ_TYPE_OPEN].reqid,
                                  __g_req[REQ_TYPE_OPEN].token,
                                  &req_rsp,
                                  sizeof(req_rsp));
        if (ret == RS_OK) {
            break;
        }
    }
    while (1) {
        aw_mdelay(100);

        ret = reqsched_rsp_tryget(__g_req1,
                                  __g_req1_token,
                                  &req1_rsp,
                                  sizeof(req1_rsp));
        if (ret == RS_OK) {
            break;
        }
    }
    while (1) {
        aw_mdelay(100);
        ret = reqsched_rsp_tryget(__g_req2,
                                  __g_req2_token,
                                  &req2_rsp,
                                  sizeof(req2_rsp));
        if (ret == RS_OK) {
            break;
        }
    }
    __LOG_PFX(1, "Elapsed time %d ms.", __INTERVAL_MS_GET(tick));
}

/*
 * 测试内容：发起请求3
 * 测试结果：请求失败
 */
TEST(reqsched, __reqsched_test_retval)
{
    rs_err_t ret;

    ret = reqsched_req_send(__g_req3_none, NULL, 0, &__g_req3_token);
    aw_assert(ret == -RS_ESCHED);
}

/*
 * 测试内容：测试同一个请求类型，的先后执行顺序
 * 测试结果：按顺序成功响应
 */
TEST(reqsched, __reqsched_send_by_same_type)
{
    static int j = 0;
    rs_err_t ret;
    uint32_t  id[10];
    uint32_t  req_rsp[10];
    uint32_t  token[10];
    uint8_t   num  = 0;

    __g_req[REQ_TYPE_CHECK_ORDER].reqid = reqsched_create(__check_order_hdr,
                                                          10,
                                                          sizeof(int));
    aw_assert(__g_req[REQ_TYPE_CHECK_ORDER].reqid > 0);

    num = AW_NELEMENTS(id);
    for (int i = 0; i < num; i++) {
        id[i] = ++j;
    }
    for (int i = 0; i < num; i++) {
        ret = reqsched_req_send(__g_req[REQ_TYPE_CHECK_ORDER].reqid,
                                &id[i],
                                sizeof(id[0]),
                                &token[i]);
        aw_assert(ret == RS_OK);
    }
    for (int i = 0; i < num;) {
        ret = reqsched_rsp_tryget(__g_req[REQ_TYPE_CHECK_ORDER].reqid,
                                  token[i],
                                  &req_rsp[i],
                                  sizeof(req_rsp[i]));
        if (ret == RS_OK) {
           i++;
        } else {
            aw_mdelay(5);
        }
    }
}

#if 1
#include "aw_reqsched.h"

static void __test_aw_itf_hdr (uint32_t reqid, void *arg, size_t size)
{
    static uint32_t _id = 0;
    rs_err_t ret;
    uint32_t id = *(uint32_t *)arg;
    aw_mdelay(1000);
    __LOG_PFX(1, "__check_order_hdr id:%d", id);
    aw_mdelay(1000);

    ret = reqsched_rsp_send(reqid, arg);
    aw_assert(ret == RS_OK);

    /* 断言 id 值是按顺序  */
    _id++;
    aw_assert(_id == id);
}

TEST(reqsched, __aw_reqsched_test)
{
    static int j = 0;
    rs_err_t ret;
    uint32_t  reqid;
    size_t    size;
    uint32_t  id[10];
    uint32_t  req_rsp[10];
    uint32_t  token[10];
    uint8_t   num  = 0;

    __g_req[REQ_TYPE_TEST_AW_ITF].reqid = aw_reqsched_create(__test_aw_itf_hdr,
                                                          10,
                                                          sizeof(int));
    aw_assert(__g_req[REQ_TYPE_TEST_AW_ITF].reqid > 0);
    reqid = __g_req[REQ_TYPE_TEST_AW_ITF].reqid;

    num = AW_NELEMENTS(id);
    for (int i = 0; i < num; i++) {
        id[i] = ++j;
    }
    for (int i = 0; i < num; i++) {
        ret = aw_reqsched_req_send(reqid, &id[i], sizeof(id[0]), &token[i]);
        aw_assert(ret == RS_OK);
    }
    for (int i = 0; i < num;) {
        size = sizeof(req_rsp[i]);
        ret = aw_reqsched_rsp_tryget(reqid, token[i], &req_rsp[i], size);
        if (ret == RS_OK) {
           i++;
        } else {
            aw_mdelay(5);
        }
    }
}
#endif

static void __send_no_rsp_hdr (uint32_t reqid, void *arg, size_t size)
{
    static uint32_t _id = 0;
    rs_err_t ret;
    uint32_t id = *(uint32_t *)arg;
    uint32_t id_rsp = 0;
    aw_mdelay(1000);
    __LOG_PFX(1, "__check_order_hdr id:%d", id);
    aw_mdelay(1000);

    ret = reqsched_rsp_send(reqid, arg);
    aw_assert(ret == RS_OK);
    ret = reqsched_rsp_tryget(reqid,
                              REQSCHED_DATA_TO_TOKEN(arg),
                              &id_rsp,
                              size);
    aw_assert(ret == RS_OK);

    /* 断言 id 值是按顺序  */
    _id++;
    aw_assert(_id == id);
}

/**
 * 测试内容：发出多个请求，应用不需要响应，由处理回调将 rsp 处理掉
 * 测试结果：成功
 */
TEST(reqsched, __reqsched_send_no_rsp)
{
    static int j = 0;
    rs_err_t ret;
    uint32_t  reqid;
    uint32_t  id[10];
    uint32_t  token[10];
    int       num  = 0;
    aw_tick_t tick;

    reqid = reqsched_create(__send_no_rsp_hdr, 10, sizeof(int));
    aw_assert(reqid > 0);

    num = AW_NELEMENTS(id);
    for (int i = 0; i < num; i++) {
        id[i] = ++j;
    }
    for (int i = 0; i < num; i++) {
        ret = reqsched_req_send(reqid, &id[i], sizeof(id[0]), &token[i]);
        aw_assert(ret == RS_OK);
    }
    ret = reqsched_req_send(reqid, &id[0], sizeof(id[0]), &token[0]);
    aw_assert(ret == -RS_ENOMEM);

    tick = aw_sys_tick_get();
    for (;;) {
        aw_mdelay(1000);
        num = reqsched_req_available(reqid);
        aw_assert(num >= 0);
        if (num == 10) {
            id[0] = ++j;
            ret = reqsched_req_send(reqid, &id[0], sizeof(id[0]), &token[0]);
            aw_assert(ret == RS_OK);
            break;
        }
        aw_assert(__INTERVAL_MS_GET(tick) <= 30000);
    }
}
TEST(reqsched, __reqsched_)
{
}

TEST_GROUP_RUNNER(reqsched)
{
    aw_err_t ret = AW_NELEMENTS(__g_req);
    aw_assert((REQ_TYPE_MAX + 1) == ret);

    extern void aw_reqsched_lib_init(void);
    aw_reqschd_lib_init();
    RUN_TEST_CASE(reqsched, __reqsched_init);
    RUN_TEST_CASE(reqsched, __reqsched_send_by_multi_type);
    RUN_TEST_CASE(reqsched, __reqsched_test_retval);
    RUN_TEST_CASE(reqsched, __reqsched_send_by_same_type);
    RUN_TEST_CASE(reqsched, __reqsched_send_no_rsp);
#if 1
    RUN_TEST_CASE(reqsched, __aw_reqsched_test);
#endif
    RUN_TEST_CASE(reqsched, __reqsched_);
}
