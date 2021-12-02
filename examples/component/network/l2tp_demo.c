/*
 * netif_l2tp.c
 *
 * \note    如需使用l2tp，需在AWORKS构建时在lwipopts.h里使能如下宏定义
 *          PPPOL2TP_SUPPORT
 *          MSCHAP_SUPPORT
 *          PPP_NOTIFY_PHASE    可选使能，如使能则可在控制台打印连接状态，可用于调试
 *
 *          然后调用l2tp_create将会执行L2TP连接，连接成功控制台会显示IP信息
 *
 *          l2tp包结构：
 *
 *              ————————
 *             |  DATA  |       应用数据
 *              ————————
 *             |   IP   |       私有IP（由LNS动态分配，用户无需配置）
 *              ————————
 *             |  PPP   |       PPP头
 *              ————————
 *             |  L2TP  |       L2TP头
 *              ————————
 *             |  UDP   |       UDP头
 *              ————————
 *             |   IP   |       公网IP（物理网卡的IP地址，由LOCAL_IP_ADDR0配置）
 *              ————————
 *             |  LINK  |       物理网卡
 *              ————————
 *
 *
 * - 操作步骤：
 *   1. 需要在aw_prj_params.h头文件里使能
 *      - AW_COM_CONSOLE
 *      - 对应平台的网卡宏
 *      - 网络协议栈
 *   2. 用网线将板子和VPN路由器连接起来，根据不同型号的路由根据说明书进行配置，配置如下：
 *      （1）确保服务器的IP地址与LNS_IP_ADDR一致，并使LOCAL_IP_ADDR0与服务器保持同一网段
 *      （2）确保服务器的用户名和密码与LNS_USERNAME、LNS_PASSWORD一致
 *   3. 用户需打开电脑的网络共享中心，设置您的电脑使用静态IP地址，使REMOTE_IP_ADDR与电脑地址一致
 *   4. 打开电脑端TCP&UDP调试工具，创建TCP类型的服务器，指定IP地址为电脑的IP地址，端口为5000，然后启动服务器；
 *   5. 如果电脑端和开发板建立连接失败，请在串口打印界面执行shell命令：
 *      - "ip addr"， 查看开发板网卡0 ip地址是否设置成功；
 *      - "ping 电脑ip地址"，查看电脑和开发板相互之间网络是否连接成功；
 *      打开电脑cmd工具执行"ping 开发板ip地址"，查看电脑和开发板相互之间网络连接成功；
 *      如果以上步骤全都执行成功，则可建立连接；否则请查找其他原因，进行修改。
 *
 * - 实验现象：
 *   1. TCP&UDP调试工具连接成功后，在上位机上发送数据，串口打印出接收到的数据，并且上位机软件将收到开发板回发的信息。
 *
 *
 *  Created on: 2021年10月20日
 *      Author: weike
 */

#include <string.h>

#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_mem.h"
#include "aw_serial.h"
#include "aw_netif.h"

#include "netif/ppp/pppol2tp.h"
#include "netif/ppp/ppp.h"
#include "lwip/netif.h"
#include "lwip/sockets.h"

#include "aw_demo_config.h"

#define LOCAL_NIC0_NAME                DE_ENET_NAME     /* 开发板网卡0 名字(该名称在网卡0的配置文件中定义) */
#define LOCAL_NETIF_NAME               "et0"            /* 开发板网卡0 */

#define LOCAL_IP_ADDR0                 "172.16.18.199"  /* 开发板 网卡0 IP 地址，用户需保证开发板和电脑的IP地址在同一网段 */
#define LOCAL_NET_MASK0                "255.255.0.0"    /* 开发板 网卡0 子网掩码 */
#define LOCAL_GATAWAY_ADDR0            "172.16.18.254"  /* 开发板 网卡0 网关地址 ，用户需保证开发板和电脑的网关地址在同一网段 */

#define LNS_IP_ADDR                    "172.16.18.200"  /* VPN路由器的IP地址 */
#define LNS_USERNAME                   "aworks"         /* LNS 用户名 */
#define LNS_PASSWORD                   "aworks2019"     /* LNS 密码 */

#define REMOTE_IP_ADDR                 "192.168.1.2"    /* 远端服务器(VPN路由器内网的目标电脑)IP地址，用户需参照操作步骤，根据具体情况修改 */
#define REMOT_PORT                     5000             /* 远端服务器端口 */

typedef struct l2tp_session {
    struct netif netif;
    ppp_pcb *pcb;
} l2tp_session_t;

#ifdef PPP_NOTIFY_PHASE
void l2tp_notify_phase_cb(ppp_pcb *pcb, u8_t phase, void *ctx)
{

    switch(phase) {
    case PPP_PHASE_DEAD:{
        aw_kprintf("Session is dead \n");
        break;
    }
    case PPP_PHASE_HOLDOFF:{
        aw_kprintf("Session hold off \n");
        break;
    }
    case PPP_PHASE_INITIALIZE:{
        aw_kprintf("Session initialize \n");
        break;
    }
    case PPP_PHASE_ESTABLISH:{
        aw_kprintf("Session is establishing \n");
        break;
    }
    case PPP_PHASE_AUTHENTICATE:{
        aw_kprintf("Session is authenticate \n");
        break;
    }
    case PPP_PHASE_RUNNING:{
        aw_kprintf("Session running \n");
        break;
    }
    case PPP_PHASE_TERMINATE:{
        aw_kprintf("Session terminate \n");
        break;
    }
    case PPP_PHASE_DISCONNECT:{
        aw_kprintf("Session disconnect \n");
        break;
    }
    default:
        break;
    }
}
#endif

static void ppp_linkstate_cb(ppp_pcb *pcb, int err_code, void *ctx)
{
    //l2tp_session_t *p_l2tp = (l2tp_session_t *)ctx;

    switch (err_code) {
    case PPPERR_NONE: {
        aw_kprintf("status callback: connect\n");
        aw_kprintf("ip: %s\n", ipaddr_ntoa(&pcb->netif->ip_addr));
        aw_kprintf("gw: %s\n", ipaddr_ntoa(&pcb->netif->gw));
        aw_kprintf("netmask: %s\n", ipaddr_ntoa(&pcb->netif->netmask));
        break;
    }
    case PPPERR_PARAM: {
        aw_kprintf("Invalid parameter\n");
        break;
    }
    case PPPERR_OPEN:{
        aw_kprintf("Unable to open PPP session\n");
        break;
    }
    case PPPERR_DEVICE:{
        aw_kprintf("Invalid I/O device for PPP\n");
        break;
    }
    case PPPERR_ALLOC:{
        aw_kprintf("Unable to allocate resources\n");
        break;
    }
    case PPPERR_USER:{
        aw_kprintf("User interrupt\n");
        break;
    }
    case PPPERR_CONNECT:{
        aw_kprintf("Connection lost\n");
        break;
    }
    case PPPERR_AUTHFAIL:{
        aw_kprintf("Failed authentication challenge\n");
        break;
    }
    case PPPERR_PROTOCOL:{
        aw_kprintf("Failed to meet protocol\n");
        break;
    }
    case PPPERR_PEERDEAD:{
        aw_kprintf("Connection timeout\n");
        break;
    }
    case PPPERR_IDLETIMEOUT:{
        aw_kprintf("Idle Timeout\n");
        break;
    }
    case PPPERR_CONNECTTIME:{
        aw_kprintf("Max connect time reached\n");
        break;
    }
    case PPPERR_LOOPBACK:{
        aw_kprintf("Loopback detected\n");
        break;
    }
    default:
        aw_kprintf("Unknow error code %d\n", err_code);
        break;
    }

    if (err_code == PPPERR_NONE) {
        return;
    }

    if (err_code == PPPERR_USER) {
        //ppp_free();
        return;
    }

    ppp_connect(pcb, 30);
}


/* \brief       创建L2TP连接
 *
 * \param[in]   name        网卡名，L2TP通信所依赖的底层网卡名
 * \param[in]   ipaddr      L2TP服务器的IP地址
 * \param[in]   port        L2TP服务端口，一般配置为1701即可
 * \param[in]   username    用户名
 * \param[in]   password    密码
 *
 * \return      创建成功返回l2tp句柄，否则返回NULL
 * \note        返回句柄并非连接成功，请根据控制台信息判断。
 *
 * */
void* l2tp_create(const char *name, const ip_addr_t *ipaddr, uint16_t port, char *username, char *password)
{
    int ret;
    struct netif *p_netif = NULL;
    l2tp_session_t *p_l2tp;

    p_netif = netif_find(name);
    if (p_netif == NULL) {
        aw_kprintf("could not find the netif\n");
        return NULL;
    }

    p_l2tp = (l2tp_session_t *)aw_mem_alloc(sizeof(l2tp_session_t));
    if (p_l2tp == NULL) {
        aw_kprintf("there no enough memory for l2tp session!\n");
        return NULL;
    }
    memset(p_l2tp, 0, sizeof(l2tp_session_t));

    p_l2tp->pcb = pppol2tp_create(&p_l2tp->netif, p_netif, ipaddr, port, NULL, 0, ppp_linkstate_cb, (void *)p_l2tp);
    if (p_l2tp->pcb == NULL) {
        aw_mem_free(p_l2tp);
        return NULL;
    }
#ifdef PPP_NOTIFY_PHASE
    ppp_set_notify_phase_callback(p_l2tp->pcb, l2tp_notify_phase_cb);
#endif
    ppp_set_default(p_l2tp->pcb);
    ppp_set_usepeerdns(p_l2tp->pcb, 0);
    ppp_set_auth(p_l2tp->pcb, PPPAUTHTYPE_ANY, username, password);
    ret =  ppp_connect(p_l2tp->pcb, 2);
    if (ret != ERR_OK) {
        aw_kprintf("ppp connect failed!\n");
        netif_remove(&p_l2tp->netif);
        aw_mem_free(p_l2tp);
        return NULL;
    }

    return p_l2tp;
}


/**
 * \brief 配置网卡
 */
aw_local aw_err_t __config_eth0_ip(void)
{
    struct in_addr addr;
    aw_netif_t * p_netif;
    aw_err_t ret;
    aw_bool_t is_on;
    ip_addr_t ip;

    /* 打开网卡0设备 */
    p_netif = aw_netif_dev_open (LOCAL_NIC0_NAME);
    if (p_netif == NULL) {
        aw_kprintf("aw_netif_dev_open fail\r\n");
        return AW_ERROR;
    }

    /* 设置网卡0 IP 地址 */
    inet_aton(LOCAL_IP_ADDR0, &addr);
    ret = aw_netif_ipv4_ip_set(p_netif,&addr);
    if (ret != AW_OK) {
        aw_kprintf("aw_netif_ipv4_ip_set fail,error code:%d\r\n",ret);
        return ret;
    }

    /* 设置网卡0 子网掩码 */
    inet_aton(LOCAL_NET_MASK0, &addr);
    ret = aw_netif_ipv4_netmask_set(p_netif,&addr);
    if (ret != AW_OK) {
        aw_kprintf("aw_netif_ipv4_netmask_set fail,error code:%d\r\n",ret);
        return ret;
    }

    /* 设置网卡0 网关 */
    inet_aton(LOCAL_GATAWAY_ADDR0, &addr);
    aw_netif_ipv4_gw_set(p_netif,&addr);
    if (ret != AW_OK) {
        aw_kprintf("aw_netif_ipv4_gw_set fail,error code:%d\r\n",ret);
        return ret;
    }

    /* 关闭 DHCPC 客户端，网卡0使用静态IP地址 */
    aw_netif_dhcpc_is_on(p_netif, &is_on);
    if (is_on == AW_TRUE) {
        ret =  aw_netif_dhcpc_stop(p_netif);
        if (ret != AW_OK) {
            aw_kprintf("awbl_nic_dhcpc_stop fail,error code:%d\r\n",ret);
        }
    }

    /* 设置L2TP服务器地址 */
    memset(&ip, 0, sizeof(ip_addr_t));
    ip.type = IPADDR_TYPE_V4;
    ip.u_addr.ip4.addr = inet_addr("172.16.18.200");
    l2tp_create("et0", &ip, 1701, LNS_USERNAME, LNS_PASSWORD);

    return AW_OK;
}

/**
 * \brief tcp 客户端的入口函数
 */
aw_local void* __task_handle (void *arg)
{
    int                 ret = 0;
    int                 socfd;
    char                msg[32] = { 0 };
    struct sockaddr_in  remote_addr;        /* 远端(PC)地址 */
    int                 recv_size = 0;      /* 接收到的字符个数 */
    fd_set              rset;

    struct timeval tm = {
        .tv_sec  = 3,   /* select超时时间设置为3秒 */
        .tv_usec = 0
    };

    /* 配置开发板的网卡0 IP地址 */
    if (__config_eth0_ip() != AW_OK) {
        return 0;
    }

    remote_addr.sin_family      = AF_INET;
    remote_addr.sin_port        = htons(REMOT_PORT);
    remote_addr.sin_addr.s_addr = inet_addr(REMOTE_IP_ADDR);

    while (1) {
        socfd = socket(AF_INET, SOCK_STREAM, 0);  /* TCP为流式套接字 */
        if (socfd < 0) {
            AW_INFOF(("Interface binding failed\r\n"));
            return 0;
        }

        if (0 == connect(socfd,
                         (struct sockaddr *)&remote_addr,
                         sizeof(remote_addr))) {
            AW_INFOF(("TCP client: connected.\r\n"));

            while (1) {
                FD_ZERO(&rset);
                FD_SET(socfd, &rset);
                ret = select(socfd + 1, &rset, NULL, NULL, &tm);
                if (ret > 0) {

                    if (FD_ISSET(socfd, &rset)) {

                        /* 接收来自服务器的数据 */
                        recv_size = recv(socfd, msg, sizeof(msg), 0);

                        if (recv_size > 0) {

                            /* 将接收到的数据通过调试串口打印 */
                            AW_INFOF(("msg: "));
                            aw_serial_write(AW_CFG_CONSOLE_COMID, msg, recv_size);
                            AW_INFOF(("\r\n"));

                            /* 将数据立即发送回去 */
                            send(socfd, msg, recv_size, 0);

                            /* 清空缓存区 */
                            memset(msg, 0, sizeof(msg));
                        } else {

                            AW_INFOF(("TCP Server is closed\r\n"));
                            goto close_socket;
                        }
                    }
                } else if (ret == -1) {
                    goto close_socket;
                }
            }
        }

close_socket:
    closesocket(socfd);
    aw_mdelay(1000);
    }

    return 0;
}

/******************************************************************************/
void demo_l2tp_client_entry (void)
{
    aw_task_id_t    tsk;

    tsk = aw_task_create("Tcp client demo",
                         12,
                         4096,
                         __task_handle,
                         NULL);
    if (tsk == NULL) {
        aw_kprintf("Tcp client demo task create failed\r\n");
        return;
    }

    aw_task_startup(tsk);
}






