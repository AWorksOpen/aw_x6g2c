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
#include "aw_netif.h"
#include "aw_system.h"

#include "string.h"
#include "stdio.h"


#include "lwip/ext/net_sts.h"
#include "lwip/ext/vnetif.h"
#include "dhcp_server.h"
#include "netif/bridgeif.h"

#include "lwip/pbuf.h"
#include "lwip/prot/ethernet.h"
#include "lwip/ext/vlan.h"

#include "unity.h"
#include "unity_fixture.h"
#include <setjmp.h>
#include <stdio.h>

#include "aw_wifi.h"

/*****************************************************************************/
/* 宏定义 */
#define __LOG_BASE(mask, color, fmt, ...)   \
    if ((mask)) {\
         __log(color, PFX, __LINE__, fmt, ##__VA_ARGS__);\
    }

#define __LOG_PFX(mask, fmt, ...) \
    __LOG_BASE(mask, NULL, "[%s:%4d] " fmt "\n", \
               PFX, __LINE__, ##__VA_ARGS__)


#define PFX  "UNITY_TEST_NET"

#define __TIMEOUT_MSEC(tick_tmp, timeout) \
    for (tick_tmp = aw_sys_tick_get() + aw_ms_to_ticks(timeout); \
            aw_sys_tick_get() < tick_tmp; )

/* Caculate the MS interval time of current tick and given tick. */
#define __INTERVAL_MS_GET(tick)  aw_ticks_to_ms(aw_sys_tick_get() - (tick))
/******************************************************************************/
#define __VNETIF_NAME           "veth0"
#define __VNETIF_PARENT_NAME    "eth0"

#define WIFI_DEV_NAME                       "wifi43362"

#define __TOOLS_PRINT_HWADDR_FMT "%02X:%02X:%02X:%02X:%02X:%02X"
#define __TOOLS_PRINT_HWADDR_VAL(addr)                                      \
    (addr)[0], (addr)[1], (addr)[2], (addr)[3], (addr)[4], (addr)[5]


#define __VLAN_ID_DEFAULT   1
#define __ETH0_VLAN_ID  0
#define __ETH1_VLAN_ID  1
#define __WIFI_VLAN_ID  2


/******************************************************************************/
typedef enum {
    BRIDGEIF_INIT = 0,
    BRIDGEIF_PORT_ADD,
    VLAN_PORT_ADD_RANDOM,
    VLAN_PORT_ADD_SAME,
    VLAN_PORT_TEST,
} event_type_t;

typedef struct {
    event_type_t type;

    union {
       struct {
           struct netif            *bridgeif;
           aw_netif_t              *p_netif;
       } bridgeif_port_add;

       struct {
           int dst_vlan;  /* -1 为无效且地址不在 vlan_mac 表中 */
           int src_vlan;
           struct pbuf *pbuf;
       } vlan_port_test;
    } u;
} event_t;

typedef aw_err_t (*event_handler_t) (event_t *p_evt);
/*****************************************************************************/
/* 变量定义 */
aw_local struct netif *__g_br_portif[10] = { 0 };
aw_local uint8_t       __g_br_portif_num = 10;
/*****************************************************************************/
/* 申明 */
aw_local aw_err_t __vlan_event_handler (event_t *p_evt);

extern aw_bool_t g_unity_test_skip_flag;
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


/******************************************************************************/

aw_local aw_err_t __vlan_port_test (event_t *p_evt, aw_netif_t *p_if)
{
    aw_err_t ret;
    event_t evt = *p_evt;
    struct pbuf *pbuf = p_evt->u.vlan_port_test.pbuf;
    aw_bool_t allow;
    struct eth_addr *src, *dst;
    struct eth_hdr  *eth_hdr;
    struct netif *p_in;
    vlan_entry_t *p_entry;

    /* 随机选择一个不是当前网卡的网卡,作为接口端口(src) */
    for (int i = 0; i < __g_br_portif_num; i++) {
        if (__g_br_portif[i] &&
                __g_br_portif[i] != &p_if->netif) {

            pbuf->if_idx = netif_get_index(__g_br_portif[i]);
            break;
        }
    }

    eth_hdr = (struct eth_hdr *)pbuf->payload;
    dst = &eth_hdr->dest;
    src = &eth_hdr->src;

    p_evt = &evt;

    /* 转发端口就是该 netif,如果未指定  vlan mac 即指定为该 netif 的 vlan */
    if (p_evt->u.vlan_port_test.dst_vlan == -1) {
        p_entry = vlan_port_entry_get(&p_if->netif);
        p_evt->u.vlan_port_test.dst_vlan = p_entry ? p_entry->vid : -1;
    }

    if (p_evt->u.vlan_port_test.src_vlan == -1) {
        p_in = netif_get_by_index(pbuf->if_idx);
        p_entry = vlan_port_entry_get(p_in);
        p_evt->u.vlan_port_test.src_vlan = p_entry ? p_entry->vid : -1;
    }

    allow = vlan_port_forward_allow(&p_if->netif, pbuf);
    if (p_evt->u.vlan_port_test.dst_vlan ==
            p_evt->u.vlan_port_test.src_vlan) {
        aw_assert(allow);
    } else {
        aw_assert(!allow);
    }

    __LOG_PFX(1, "allow: %d " __TOOLS_PRINT_HWADDR_FMT "(vlan:%d) --> "
              __TOOLS_PRINT_HWADDR_FMT "(vlan:%d)",
              allow,
              __TOOLS_PRINT_HWADDR_VAL(src->addr),
              p_evt->u.vlan_port_test.src_vlan,
              __TOOLS_PRINT_HWADDR_VAL(dst->addr),
              p_evt->u.vlan_port_test.dst_vlan);

    return AW_OK;
}
/******************************************************************************/
TEST_GROUP(vlan);

TEST_SETUP(vlan)
{
}

TEST_TEAR_DOWN(vlan)
{
}

/******************************************************************************/
#include "gw_event_type.h"

aw_err_t __wifi_event_handler (event_t *p_evt)
{
    aw_err_t ret;
    struct gw_event_net_wifi_arg arg;
    ip4_addr_t ip4;

    aw_netif_t *p_if;
    p_if = aw_netif_dev_open("ap");
    aw_assert(p_if);
    aw_netif_dev_close(p_if);

    (void)ret;
    switch (p_evt->type) {
        case BRIDGEIF_INIT: {
            static aw_bool_t inited = 0;

            aw_assert(inited == 0);
            inited = 1;
            extern void app_wifi_init();
            app_wifi_init();

            arg.arg.cfg.ap_on = AW_TRUE;
            arg.arg.cfg.ap_hide_en = AW_TRUE;

            arg.arg.cfg.ap_wpa2_on = AW_TRUE;
            inet_aton("192.168.20.1", &ip4);
            arg.arg.cfg.ap_ip = ip4.addr;
            strcpy(arg.arg.cfg.ap_ssid, "ZYAP-13463456f");
            strcpy(arg.arg.cfg.ap_wpa2_pwd, "ABC45678");

            arg.arg.cfg.channel = 11;
            extern int app_wifi_start_ap (struct gw_event_net_wifi_arg *p_arg);
            app_wifi_start_ap(&arg);

            if (1) {
                aw_wifi_id_t id;
                id = aw_wifi_dev_open(WIFI_DEV_NAME);
                ret = aw_wifi_ioctl(id,
                                    AW_WO_OPT_SET,
                                    AW_WO_PROMISC_MODE,
                                    (void *) 0,
                                    (void *) 1);
                aw_assert(ret == AW_OK);
                aw_wifi_dev_close(id);
            }
        } break;

        case BRIDGEIF_PORT_ADD: {
            struct netif            *bridgeif;
            struct netif            *portif;

            bridgeif = p_evt->u.bridgeif_port_add.bridgeif;
            p_if     = p_evt->u.bridgeif_port_add.p_netif;
            portif   = &p_if->netif;

            if (!strcmp(p_if->p_name, "ap") == 0) {
                return AW_OK;
            }

            ret = bridgeif_add_port(bridgeif, portif);
            aw_assert(ret == 0);

            for (int i = 0; i < __g_br_portif_num; i++) {
                if (__g_br_portif[i] == NULL) {
                    __g_br_portif[i] = portif;
                    break;
                }
            }
        } break;

        case VLAN_PORT_ADD_SAME: {
            static vlan_entry_t entry;

            entry.mode = VLAN_ACCESS_LINK_PORT;
            entry.vid  = __VLAN_ID_DEFAULT ;
            ret = vlan_port_add(&p_if->netif, &entry);
            aw_assert(ret == AW_OK);
        } break;

        case VLAN_PORT_ADD_RANDOM: {
            static vlan_entry_t entry;

            entry.mode = VLAN_ACCESS_LINK_PORT;
            entry.vid  = __WIFI_VLAN_ID;
            ret = vlan_port_add(&p_if->netif, &entry);
            aw_assert(ret == AW_OK);
        } break;

        case VLAN_PORT_TEST: {
            ret = __vlan_port_test(p_evt, p_if);
            aw_assert(ret == AW_OK);
        } break;
    }
    return AW_OK;
}

/******************************************************************************/
aw_local aw_err_t __eth_init (event_t *p_evt,
                              char     *p_name,
                              char     *p_ip,
                              char     *p_nm,
                              char     *p_gw)
{
    aw_err_t ret;
    ip4_addr_t ip4;
    ip4_addr_t nm;
    ip4_addr_t gw;
    aw_netif_t *p_if;

    if (0) {
        aw_assert(p_ip && p_nm && p_gw);
        inet_aton(p_ip, &ip4);
        inet_aton(p_nm, &nm);
        inet_aton(p_gw, &gw);
        ret = aw_netif_ipv4_addr_set_by_name(p_name,
                                             (struct in_addr *)&ip4,
                                             (struct in_addr *)&nm,
                                             (struct in_addr *)&gw);
        aw_assert(ret == AW_OK);
    }

    if (1) {
        p_if = aw_netif_dev_open(p_name);
        aw_assert(p_if);
        aw_netif_dev_close(p_if);

        /* Enable promiscuous mode. */
        ret = aw_netif_io_ctl(p_if,
                              AW_NETIF_OPT_SET,
                              AW_NETIF_OPT_ETHADDR_FILTER,
                              (void *)1);
        aw_assert(ret == AW_OK);
    }

    return AW_OK;
}

aw_local aw_err_t __eth_port_add (event_t *p_evt,
                                  char    *p_name)
{
    aw_err_t                 ret;
    struct netif            *bridgeif;
    struct netif            *portif;
    aw_netif_t              *p_if;

    bridgeif = p_evt->u.bridgeif_port_add.bridgeif;
    p_if     = p_evt->u.bridgeif_port_add.p_netif;
    portif   = &p_if->netif;

    if (!strcmp(p_if->p_name, p_name) == 0) {
        return AW_OK;
    }

    ret = bridgeif_add_port(bridgeif, portif);
    aw_assert(ret == 0);
    for (int i = 0; i < __g_br_portif_num; i++) {
        if (__g_br_portif[i] == NULL) {
            __g_br_portif[i] = portif;
            break;
        }
        aw_assert(i < __g_br_portif_num - 1);
    }
    return ret;
}

/******************************************************************************/

aw_local aw_err_t __eth0_event_handler (event_t *p_evt)
{
    char *p_name = "eth0";
    aw_err_t ret;
    aw_netif_t *p_if;

    p_if = aw_netif_dev_open(p_name);
    aw_assert(p_if);
    aw_netif_dev_close(p_if);

    switch (p_evt->type) {
        case BRIDGEIF_INIT: {
            return __eth_init(p_evt, p_name, "192.168.20.130", "255.255.255.0", "192.168.20.1");
        } break;

        case BRIDGEIF_PORT_ADD: {
            return __eth_port_add(p_evt, p_name);
        } break;

        case VLAN_PORT_ADD_RANDOM: {
            static vlan_entry_t entry;

            entry.mode = VLAN_ACCESS_LINK_PORT;
            entry.vid  = __ETH0_VLAN_ID;
            ret = vlan_port_add(&p_if->netif, &entry);
            aw_assert(ret == AW_OK);
        } break;

        case VLAN_PORT_ADD_SAME: {
            static vlan_entry_t entry;

            entry.mode = VLAN_ACCESS_LINK_PORT;
            entry.vid  = __VLAN_ID_DEFAULT ;
            ret = vlan_port_add(&p_if->netif, &entry);
            aw_assert(ret == AW_OK);
        } break;

        case VLAN_PORT_TEST: {
            ret = __vlan_port_test(p_evt, p_if);
            aw_assert(ret == AW_OK);
        } break;
    }
    return AW_OK;
}
/******************************************************************************/

aw_local aw_err_t __eth1_event_handler (event_t *p_evt)
{
    char *p_name = "eth1";
    aw_err_t ret;
    aw_netif_t *p_if;

    p_if = aw_netif_dev_open(p_name);
    aw_assert(p_if);
    aw_netif_dev_close(p_if);

    switch (p_evt->type) {
        case BRIDGEIF_INIT: {
            return __eth_init(p_evt, p_name, NULL, NULL, NULL);
        } break;

        case BRIDGEIF_PORT_ADD: {
            return __eth_port_add(p_evt, p_name);
        } break;

        case VLAN_PORT_ADD_SAME: {
            static vlan_entry_t entry;

            entry.mode = VLAN_ACCESS_LINK_PORT;
            entry.vid  = __VLAN_ID_DEFAULT ;
            ret = vlan_port_add(&p_if->netif, &entry);
            aw_assert(ret == AW_OK);
        } break;

        case VLAN_PORT_ADD_RANDOM: {
            static vlan_entry_t entry;

            entry.mode = VLAN_ACCESS_LINK_PORT;
            entry.vid  = __ETH1_VLAN_ID;
            ret = vlan_port_add(&p_if->netif, &entry);
            aw_assert(ret == AW_OK);
        } break;

        case VLAN_PORT_TEST: {
            ret = __vlan_port_test(p_evt, p_if);
            aw_assert(ret == AW_OK);
        } break;
    }
    return AW_OK;
}
/******************************************************************************/
/* tag hdls */
aw_local event_handler_t __g_hdls[] = {
//          __wifi_event_handler,
          __eth0_event_handler,
          __eth1_event_handler,
          __vlan_event_handler,
          NULL,
};

aw_local aw_err_t __event_notify (event_t *p_evt)
{
    aw_err_t ret = AW_OK;
    event_handler_t pfn_hdl;
    for (int i = 0; __g_hdls[i] != NULL; i++) {
        pfn_hdl = __g_hdls[i];
        ret = pfn_hdl(p_evt);
        if (ret != AW_OK) {
            return ret;
        }
    }
    return ret;
}

/******************************************************************************/

void __netif_iterate_cb (aw_netif_t *p_netif, void *p_arg)
{
    aw_err_t                 ret;
    event_t                  evt;

    aw_assert(p_netif && p_arg);
    evt.type = BRIDGEIF_PORT_ADD;
    evt.u.bridgeif_port_add.bridgeif = p_arg;
    evt.u.bridgeif_port_add.p_netif  = p_netif;
    ret = __event_notify(&evt);
    aw_assert(ret == AW_OK);
}

aw_local void __vnetif_init (void)
{
    static struct dhcp_server   _dhcps_ctx;
    aw_err_t ret;
    ip4_addr_t ip4;
    ip4_addr_t nm;
    ip4_addr_t gw;
    aw_netif_t *p_if;

    inet_aton("192.168.30.130", &ip4);
    inet_aton("255.255.255.0", &nm);
    inet_aton("192.168.30.1", &gw);

    /* Check parent netif. */
    p_if = aw_netif_dev_open(__VNETIF_PARENT_NAME);
    aw_assert(p_if);
    aw_netif_dev_close(p_if);

    ret = aw_vnetif_add(__VNETIF_PARENT_NAME,
                        __VNETIF_NAME,
                        (struct in_addr *)&ip4,
                        (struct in_addr *)&nm,
                        (struct in_addr *)&gw);
    aw_assert(ret == AW_OK);

    dhcps_bind_netif(&_dhcps_ctx, __VNETIF_NAME);
    ret = dhcps_start(&_dhcps_ctx);
    aw_assert(ret == AW_OK);

}

TEST(vlan, __bridgeif_init)
{
    bridgeif_initdata_t      initdata;
    aw_netif_t              *p_netif;
    struct netif            *bridgeif;
    aw_err_t                 ret;
    event_t                  evt;

    evt.type = BRIDGEIF_INIT;
    ret = __event_notify(&evt);
    aw_assert(ret == AW_OK);

    __vnetif_init();

    p_netif = aw_netif_dev_open(__VNETIF_NAME);
    aw_assert(p_netif != NULL);
    aw_netif_dev_close(p_netif);

    bridgeif = &p_netif->netif;

    aw_netif_hwaddr_get(p_netif,
                        initdata.ethaddr.addr,
                        sizeof(initdata.ethaddr.addr));
    initdata.max_ports               = BRIDGEIF_MAX_PORTS;
    initdata.max_fdb_dynamic_entries = 128;
    initdata.max_fdb_static_entries  = 0;

    bridgeif->state = (void*)(&initdata);
    ret = bridgeif_init(bridgeif);
    aw_assert(ret == 0);

    aw_netif_iterate(__netif_iterate_cb, bridgeif);
}


/******************************************************************************/
static struct netif  *__gp_vlan_bridgeif = NULL;
aw_local aw_err_t __vlan_event_handler (event_t *p_evt)
{
    switch (p_evt->type) {
        case BRIDGEIF_INIT: {
            __gp_vlan_bridgeif = p_evt->u.bridgeif_port_add.bridgeif;
            aw_assert(__gp_vlan_bridgeif);
        } break;

        default: break;
    }
    return AW_OK;
}
/******************************************************************************/
/* 测试内容:加入基于 mac 的 vlan 表,同时使能基于 mac 和 port 的 vlan, ,所有网卡设置为不同 vlan
 * 测试结果:成功
 */
TEST(vlan, __vlan_test_with_mac_and_port)
{
    static vlan_entry_t entry_arr[] = {
           {
                .mode = VLAN_ACCESS_LINK_MAC,
                .vid  = 0,
                .eth = {.addr  = {0x2c, 0x4d, 0x54, 0xd7, 0x4e, 0x98},}
           },
//           {
//                .mode = VLAN_ACCESS_LINK_MAC,
//                .vid  = 1,
//                .eth = {.addr  = { 0xe8, 0x6a, 0x64, 0xcf, 0x0e, 0xc4 },}
//           },
    };

    aw_err_t ret;
    event_t     evt;
    struct pbuf buf;
    uint8_t payload[128];

    ret = vlan_init(__gp_vlan_bridgeif, entry_arr, AW_NELEMENTS(entry_arr));
    aw_assert(ret == AW_OK);

    evt.type = VLAN_PORT_ADD_RANDOM;
    ret = __event_notify(&evt);
    aw_assert(ret == AW_OK);

    memset(&buf, 0, sizeof(buf));
    buf.payload = (void *)payload;

    {
        struct eth_hdr  eth_hdr = {
           .dest = { {0x2c, 0x4d, 0x54, 0xd7, 0x4e, 0x98} },
           .src = { {0xe8, 0x6a, 0x64, 0xcf, 0x0e, 0xc4} },
        };

        memcpy(payload, &eth_hdr, sizeof(eth_hdr));

        evt.type = VLAN_PORT_TEST;
        evt.u.vlan_port_test.dst_vlan = 0;  /* 在表中且为 vlan 0 */
        evt.u.vlan_port_test.src_vlan = -1;
        evt.u.vlan_port_test.pbuf = &buf;
        ret = __event_notify(&evt);
        aw_assert(ret == AW_OK);
    }

    {
        struct eth_hdr  eth_hdr = {
           .dest = { {0xe8, 0x6a, 0x64, 0xcf, 0x0e, 0xc4} },
           .src = { {0x2c, 0x4d, 0x54, 0xd7, 0x4e, 0x98} },
        };

        memcpy(payload, &eth_hdr, sizeof(eth_hdr));

        evt.type = VLAN_PORT_TEST;
        evt.u.vlan_port_test.dst_vlan = -1;
        evt.u.vlan_port_test.src_vlan = 0;  /* 在表中且为 vlan 0 */
        evt.u.vlan_port_test.pbuf = &buf;
        ret = __event_notify(&evt);
        aw_assert(ret == AW_OK);
    }
}

/* 测试内容:测试基于 port 的 vlan,无 mac 表,所有网卡设置为相同 vlan
 * 测试结果:成功
 */
TEST(vlan, __vlan_test_with_port)
{
    aw_err_t ret;
    event_t     evt;
    struct pbuf buf;
    uint8_t payload[128];

    ret = vlan_init(__gp_vlan_bridgeif, NULL, 0);
    aw_assert(ret == AW_OK);

    evt.type = VLAN_PORT_ADD_SAME;
    ret = __event_notify(&evt);
    aw_assert(ret == AW_OK);

    memset(&buf, 0, sizeof(buf));
    buf.payload = (void *)payload;

    {
        struct eth_hdr  eth_hdr = {
           .dest = { {0x2c, 0x4d, 0x54, 0xd7, 0x4e, 0x98} },
           .src = { {0xe8, 0x6a, 0x64, 0xcf, 0x0e, 0xc4} },
        };

        memcpy(payload, &eth_hdr, sizeof(eth_hdr));

        evt.type = VLAN_PORT_TEST;
        evt.u.vlan_port_test.dst_vlan = -1;
        evt.u.vlan_port_test.src_vlan = -1;
        evt.u.vlan_port_test.pbuf = &buf;
        ret = __event_notify(&evt);
        aw_assert(ret == AW_OK);
    }

    {
        struct eth_hdr  eth_hdr = {
           .dest = { {0xe8, 0x6a, 0x64, 0xcf, 0x0e, 0xc4} },
           .src = { {0x2c, 0x4d, 0x54, 0xd7, 0x4e, 0x98} },
        };

        memcpy(payload, &eth_hdr, sizeof(eth_hdr));

        evt.type = VLAN_PORT_TEST;
        evt.u.vlan_port_test.dst_vlan = -1;
        evt.u.vlan_port_test.src_vlan = -1;
        evt.u.vlan_port_test.pbuf = &buf;
        ret = __event_notify(&evt);
        aw_assert(ret == AW_OK);
    }
}

/* 测试内容:测试基于 port 的 vlan,无 mac 表,所有网卡设置为不同 vlan
 * 测试结果:不允许转发
 */
TEST(vlan, __vlan_random_id_test_with_port)
{
    aw_err_t ret;
    event_t     evt;
    struct pbuf buf;
    uint8_t payload[128];

    ret = vlan_init(__gp_vlan_bridgeif, NULL, 0);
    aw_assert(ret == AW_OK);

    evt.type = VLAN_PORT_ADD_RANDOM;
    ret = __event_notify(&evt);
    aw_assert(ret == AW_OK);

    memset(&buf, 0, sizeof(buf));
    buf.payload = (void *)payload;

    {
        struct eth_hdr  eth_hdr = {
           .dest = { {0x2c, 0x4d, 0x54, 0xd7, 0x4e, 0x98} },
           .src = { {0xe8, 0x6a, 0x64, 0xcf, 0x0e, 0xc4} },
        };

        memcpy(payload, &eth_hdr, sizeof(eth_hdr));

        evt.type = VLAN_PORT_TEST;
        evt.u.vlan_port_test.dst_vlan = -1;
        evt.u.vlan_port_test.src_vlan = -1;
        evt.u.vlan_port_test.pbuf = &buf;
        ret = __event_notify(&evt);
        aw_assert(ret == AW_OK);
    }

    {
        struct eth_hdr  eth_hdr = {
           .dest = { {0xe8, 0x6a, 0x64, 0xcf, 0x0e, 0xc4} },
           .src = { {0x2c, 0x4d, 0x54, 0xd7, 0x4e, 0x98} },
        };

        memcpy(payload, &eth_hdr, sizeof(eth_hdr));

        evt.type = VLAN_PORT_TEST;
        evt.u.vlan_port_test.dst_vlan = -1;
        evt.u.vlan_port_test.src_vlan = -1;
        evt.u.vlan_port_test.pbuf = &buf;
        ret = __event_notify(&evt);
        aw_assert(ret == AW_OK);
    }
}

/******************************************************************************/
aw_local void __vlan_set_vid (event_type_t type,
                              vlan_entry_t *p_arr,
                              uint16_t arrnum)
{
    aw_err_t ret;
    event_t     evt;
    struct pbuf buf;
    uint8_t payload[128];
    aw_netif_t *p_if1;
    aw_netif_t *p_if2;

    ret = vlan_init(__gp_vlan_bridgeif, p_arr, arrnum);
    aw_assert(ret == AW_OK);

    evt.type = type;
    ret = __event_notify(&evt);
    aw_assert(ret == AW_OK);

    memset(&buf, 0, sizeof(buf));
    buf.payload = (void *)payload;

    p_if1 = NULL;
    p_if2 = NULL;

    /* 获取两个网卡 */
    for (int i = 0; i < __g_br_portif_num; i++) {
        if (__g_br_portif[i] && p_if1 == NULL) {
            p_if1 = AW_CONTAINER_OF(__g_br_portif[i], aw_netif_t, netif);
            continue;
        }
        if (__g_br_portif[i] && p_if2 == NULL) {
            p_if2 = AW_CONTAINER_OF(__g_br_portif[i], aw_netif_t, netif);
            break;
        }
    }

    aw_assert(p_if1 && p_if2);
    while (1) {
        aw_bool_t up;

        if (g_unity_test_skip_flag) {
            return;
        }

        ret = aw_netif_is_link_up(p_if1, &up);
        aw_assert(ret == AW_OK);
        if (!up) {
            __LOG_PFX(1, "Wait for neitf %s to link up...", p_if1->p_name);
            aw_mdelay(2000);
            continue;
        }
        ret = aw_netif_is_link_up(p_if2, &up);
        aw_assert(ret == AW_OK);
        if (!up) {
            __LOG_PFX(1, "Wait for neitf %s to link up...", p_if2->p_name);
            aw_mdelay(2000);
            continue;
        }

        break;
    }

    while (1) {
        aw_bool_t up;

        if (g_unity_test_skip_flag) {
            return;
        }

        ret = aw_netif_is_link_up(p_if1, &up);
        aw_assert(ret == AW_OK);
        if (up) {
            __LOG_PFX(1, "Wait for neitf %s to link down...", p_if1->p_name);
            aw_mdelay(2000);
            continue;
        }
        ret = aw_netif_is_link_up(p_if2, &up);
        aw_assert(ret == AW_OK);
        if (up) {
            __LOG_PFX(1, "Wait for neitf %s to link down...", p_if2->p_name);
            aw_mdelay(2000);
            continue;
        }

        break;
    }
}

/* 测试内容:配置相同的 vid ,等待以太网插入,测试完后,等待以太网拔出
 * 测试结果:互ping 成功
 */
TEST(vlan, __vlan_set_same_vid)
{
    __vlan_set_vid(VLAN_PORT_ADD_SAME, NULL, 0);
    g_unity_test_skip_flag = AW_FALSE;
}

/* 测试内容:配置不同的 vid ,等待以太网插入,测试完后,等待以太网拔出
 * 测试结果:互ping 失败
 */
TEST(vlan, __vlan_set_random_vid)
{
    __vlan_set_vid(VLAN_PORT_ADD_RANDOM, NULL, 0);
    g_unity_test_skip_flag = AW_FALSE;
}

/* 测试内容:配置不同的 vid ,等待以太网插入,测试互ping两次，第一次ping完后交换网口，再ping一次，,等待以太网拔出
 * 测试结果:互ping 时，一次失败，一次成功
 */
TEST(vlan, __vlan_set_random_vid_with_mac)
{
    static vlan_entry_t entry_arr[] = {
           {
                .mode = VLAN_ACCESS_LINK_MAC,
                .vid  = 0,
                .eth = {.addr  = {0x2c, 0x4d, 0x54, 0xd7, 0x4e, 0x98},}  // PC
           },
//           {
//                .mode = VLAN_ACCESS_LINK_MAC,
//                .vid  = 1,
//                .eth = {.addr  = { 0xe8, 0x6a, 0x64, 0xcf, 0x0e, 0xc4 },}
//           },
    };
    __vlan_set_vid(VLAN_PORT_ADD_RANDOM, entry_arr, AW_NELEMENTS(entry_arr));
    if (g_unity_test_skip_flag) {
        g_unity_test_skip_flag = AW_FALSE;
        return;
    }

    __vlan_set_vid(VLAN_PORT_ADD_RANDOM, entry_arr, AW_NELEMENTS(entry_arr));
    g_unity_test_skip_flag = AW_FALSE;
}
/******************************************************************************/

TEST_GROUP_RUNNER(vlan)
{
    RUN_TEST_CASE(vlan, __bridgeif_init);
    RUN_TEST_CASE(vlan, __vlan_test_with_port);
    RUN_TEST_CASE(vlan, __vlan_test_with_mac_and_port);
    RUN_TEST_CASE(vlan, __vlan_random_id_test_with_port);

    RUN_TEST_CASE(vlan, __vlan_set_same_vid);
    RUN_TEST_CASE(vlan, __vlan_set_random_vid);
    RUN_TEST_CASE(vlan, __vlan_set_random_vid_with_mac);
    aw_mdelay(1000);
}
