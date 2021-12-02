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
/**
 *
 * \brief 通用USB大容量设备演示例程
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - 对应平台的USBH宏和USBH协议栈
 *      - AW_COM_CONSOLE
 *   2. 将板子中的DURX、DUTX通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *   3. 在底板的USB端口中插入U盘
 *
 * - 实验现象：
 *   1. 输入指令，串口打印调试信息
 *
 * \brief 命令说明
 * ums_mem       检查当前USB大容量存储驱动的内存申请释放状态
 * ums_detect    检测当前存在的USB大容量存储设备，会返回设备号，分区号和分区名字
 * ums_lun_bsize 获取分区的块大小，需要传入两个参数，第一个是设备号，第二个是分区号
 * ums_lun_nblks 获取分区的块数量，需要传入两个参数，第一个是设备号，第二个是分区号
 * ums_mount     挂载分区，需要传入三个参数，第一个是设备号，第二个是分区号，
 *               第三个是是挂载点，例如"ums_mount 0 0 /u"
 * ums_umount    取消挂载分区，需传入挂载点参数，例如“ums_umount /u”
 * ums_wr_check  文件系统读写检测，需传入两个参数，第一个是挂载点，第二个是读写的数据大小
 * ums_wr_speed  文件系统读写速度检测，需传入三个参数，第一个是挂载点，
 *               第二个是读写数据大小，数据大小至少要1M大小以上，第三个参数是读写重复次数（3或以上）
 */
#include "Aworks.h"
#include "aw_delay.h"
#include "aw_shell.h"
#include "aw_vdebug.h"
#include "aw_mem.h"
#include "host/class/ms/usbh_mass_storage.h"
#include "host/class/ms/awbl_usbh_mass_storage.h"
#include "host/core/usbh.h"
#include "fs/aw_mount.h"
#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"
#include "io/sys/aw_stat.h"
#include "io/sys/aw_statvfs.h"
#include <string.h>

static uint32_t            __g_ums_dev_num  = 0;

struct usbh_ms_mount_info{
    char                mount_point[32];
    struct usbh_ms_lun *ums_lun;
    aw_bool_t           is_used;
};

static struct usbh_ms_mount_info mount_info[8];

static struct usbh_ms_mount_info *__mount_point_check(char *mount_point){
    int i;

    if(mount_point == NULL){
        return NULL;
    }

    for(i = 0;i < 8;i++){
        if(strcmp(mount_info[i].mount_point, mount_point) == 0){
            return &mount_info[i];
        }
    }
    return NULL;
}

static struct usbh_ms_mount_info *__mount_info_get(void){
    int i;

    for(i = 0;i < 8;i++){
        if(mount_info[i].is_used == AW_FALSE){
            mount_info[i].is_used = AW_TRUE;
            return &mount_info[i];
        }
    }
    return NULL;
}

static aw_err_t __mount_info_put(struct usbh_ms_mount_info *mount_info){
    if(mount_info == NULL){
        return -AW_EINVAL;
    }
    memset(mount_info, 0, sizeof(struct usbh_ms_mount_info));

    return AW_OK;
}

/* 获取最小值 */
static uint32_t get_average_min(uint32_t *data, int num)
{
    int      j          = 0;
    uint32_t temp_min   = 0;;

    temp_min = data[0];
    for (j = 0; j < num; j++) {
        if (temp_min > data[j]) {
            temp_min = data[j];
        }
    }
    return temp_min;
}

/* 获取最大值 */
static uint32_t get_average_max(uint32_t *data, int num)
{
    int      j          = 0;
    uint32_t temp_max   = 0;;

    temp_max = data[0];
    for (j = 0; j < num; j++) {
        if (temp_max < data[j]) {
            temp_max = data[j];
        }
    }
    return temp_max;
}

/** 查询USB内存堆的使用情况shell命令*/
static int __ums_mem_sta (int argc, char *argv[], struct aw_shell_user *p_user){
    usb_err_t              ret;
    struct usbh_mem_record mem_record;

    if(argc > 0){
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:ums_mem\r\n\r\n");
            aw_kprintf("Description:Check USB Mass Storage mem status\r\n\r\n");
            aw_kprintf("parameter:\r\n");
        } else {
            aw_kprintf("Try \"ums_mem --help\" for more information.\r\n");
        }
        return 0;
    }


    ret = ums_lib_mem_record_get(&mem_record);
    if(ret == USB_OK){
        aw_kprintf("mem record:%s\r\n", mem_record.drv_name);
        aw_kprintf("  mem   alloc : %d\r\n", mem_record.mem_alloc_num);
        aw_kprintf("  mem   free  : %d\r\n", mem_record.mem_free_num);
        aw_kprintf("  mem   total : %d\r\n", mem_record.mem_total_num);
        aw_kprintf("  mutex alloc : %d\r\n", mem_record.mutex_alloc_num);
        aw_kprintf("  mutex free  : %d\r\n", mem_record.mutex_free_num);
        aw_kprintf("  sem   alloc : %d\r\n", mem_record.sem_alloc_num);
        aw_kprintf("  sem   free  : %d\r\n", mem_record.sem_free_num);
    }
    else{
        aw_kprintf("ums mem record get failed\r\n");
    }
    return 0;
}

/** 检测当前存在的USB大容量存储设备*/
static int __ums_dev_detect (int argc, char *argv[], struct aw_shell_user *p_user){
    int                       ums_num  = 0;
    int                       i, j;
    struct usbh_mass_storage *ums_dev  = NULL;
    struct usbh_ms_lun       *ums_lun  = NULL;
    uint8_t                   lun_num  = 0;

    if(argc > 0){
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:ums_detect\r\n\r\n");
            aw_kprintf("Description:Detect all USB Mass Storage devices in system\r\n\r\n");
            aw_kprintf("parameter:\r\n");
        } else {
            aw_kprintf("Try \"ums_detect --help\" for more information.\r\n");
        }
        return 0;
    }

    if(__g_ums_dev_num == 0){
        aw_kprintf("Do not have USB Mass Storage device\r\n");
        return 0;
    }

    /* 获取UMS设备数量*/
    ums_num = usbh_ms_dev_num_get();
    if(ums_num <= 0){
        return 0;
    }
    for(i = 0;i < ums_num; i++){
        ums_dev = usbh_ms_dev_open(USBDEV_IGONRE_PID, USBDEV_IGONRE_VID, i + 1);
        if(ums_dev != NULL){
            usbh_ms_max_lun_get(ums_dev, &lun_num);
            aw_kprintf("UMS device %d(PID0x%x,VID0x%x) support %d lun:\r\n", i + 1,
                    USBH_DEV_PID_GET(ums_dev->p_fun),
                    USBH_DEV_VID_GET(ums_dev->p_fun), lun_num);
            lun_num = usbh_ms_lun_num_get(ums_dev);
            if(lun_num > 0){
                for(j = 0;j < lun_num; j++){
                    ums_lun = usbh_ms_valid_lun_get(ums_dev, j);
                    if(ums_lun != NULL){
                        aw_kprintf("    lun %d, name %s\r\n", j,ums_lun->name);
                        usbh_ms_valid_lun_put(ums_lun);
                    }
                    else{
                        aw_kprintf("Do not find USB Mass Storage device lun %d\r\n", j);
                    }
                }
            }
            usbh_ms_dev_close(ums_dev);
        }
        else{
            aw_kprintf("Do not find USB Mass Storage device %d\r\n", i + 1);
        }

    }
    return 0;
}

/* 获取USB大容量存储设备分区的块大小*/
static int __ums_lun_bsize_get (int argc, char *argv[], struct aw_shell_user *p_user){
    int                       dev_index = 0;
    int                       lun_index = 0;
    struct usbh_mass_storage *ums_dev   = NULL;
    struct usbh_ms_lun       *ums_lun   = NULL;
    uint32_t                  bsize;

    if(argc > 0){
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:ums_lun_bsize\r\n\r\n");
            aw_kprintf("Description:Get USB Mass Storage lun block size\r\n\r\n");
            aw_kprintf("parameter:\r\n");
            aw_kprintf("    1.dev index :USB Mass Storage device index\r\n");
            aw_kprintf("    2.lun index :USB Mass Storage device lun index\r\n");
            return 0;
        }
    }

    if(argc != 2){
        aw_kprintf("Try \"ums_lun_bsize --help\" for more information.\r\n");
        return 0;
    }

    if(__g_ums_dev_num == 0){
        aw_kprintf("Do not have USB Mass Storage device\r\n");
        return 0;
    }

    dev_index = atoi(argv[0]);
    lun_index = atoi(argv[1]);

    ums_dev = usbh_ms_dev_open(USBDEV_IGONRE_PID, USBDEV_IGONRE_VID, dev_index);
    if(ums_dev != NULL){
        ums_lun = usbh_ms_valid_lun_get(ums_dev, lun_index);
        if(ums_lun != NULL){
            bsize = usbh_ms_lun_bsize_get(ums_lun);
            aw_kprintf("Block Size:%d\r\n",bsize);
            usbh_ms_valid_lun_put(ums_lun);
        }
        else{
            aw_kprintf("Do not find USB Mass Storage device lun\r\n");
        }
        usbh_ms_dev_close(ums_dev);
    }
    else{
        aw_kprintf("Do not find USB Mass Storage device\r\n");
    }

    return 0;
}

static int __ums_lun_nblks_get (int argc, char *argv[], struct aw_shell_user *p_user){
    int                       dev_index = 0;
    int                       lun_index = 0;
    struct usbh_mass_storage *ums_dev   = NULL;
    struct usbh_ms_lun       *ums_lun   = NULL;
    uint32_t                  nblk;

    if(argc > 0){
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:ums_lun_nblks\r\n\r\n");
            aw_kprintf("Description:Get USB Mass Storage lun block number\r\n\r\n");
            aw_kprintf("parameter:\r\n");
            aw_kprintf("    1.dev index :USB Mass Storage device index\r\n");
            aw_kprintf("    2.lun index :USB Mass Storage device lun index\r\n");
            return 0;
        }
    }

    if(argc != 2){
        aw_kprintf("Try \"ums_lun_nblks --help\" for more information.\r\n");
        return 0;
    }

    if(__g_ums_dev_num == 0){
        aw_kprintf("Do not have USB Mass Storage device\r\n");
        return 0;
    }

    dev_index = atoi(argv[0]);
    lun_index = atoi(argv[1]);

    ums_dev =  usbh_ms_dev_open(USBDEV_IGONRE_PID, USBDEV_IGONRE_VID, dev_index);
    if(ums_dev != NULL){
        ums_lun = usbh_ms_valid_lun_get(ums_dev, lun_index);
        if(ums_lun != NULL){
            nblk = usbh_ms_lun_nblks_get(ums_lun);
            aw_kprintf("Block number:%d\r\n",nblk);
            usbh_ms_valid_lun_put(ums_lun);
        }
        else{
            aw_kprintf("Do not find USB Mass Storage device lun\r\n");
        }
        usbh_ms_dev_close(ums_dev);
    }
    else{
        aw_kprintf("Do not find USB Mass Storage device\r\n");
    }

    return 0;
}

/* 挂载USB大容量存储设备*/
static int __ums_mount (int argc, char *argv[], struct aw_shell_user *p_user){
    aw_err_t                   ret;
    struct aw_statvfs          fs_info;
    uint32_t                   total_size, remain_size;
    int                        dev_index    = 0;
    int                        lun_index    = 0;
    struct usbh_mass_storage  *ums_dev      = NULL;
    struct usbh_ms_lun        *ums_lun      = NULL;
    char                      *lun_name     = NULL;
    struct usbh_ms_mount_info *p_mount_info = NULL;

    if(argc > 0){
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:ums_mount\r\n\r\n");
            aw_kprintf("Description:Mount USB Mass Storage lun\r\n\r\n");
            aw_kprintf("parameter:\r\n");
            aw_kprintf("    1.dev index   :USB Mass Storage device index\r\n");
            aw_kprintf("    2.lun index   :USB Mass Storage device lun index\r\n");
            aw_kprintf("    3.mount point :mount point\r\n");
            return 0;
        }
    }

    if(argc != 3){
        aw_kprintf("Try \"ums_mount --help\" for more information.\r\n");
        return 0;
    }

    if(__g_ums_dev_num == 0){
        aw_kprintf("Do not have USB Mass Storage device\r\n");
        return 0;
    }

    p_mount_info = __mount_point_check(argv[2]);
    if(p_mount_info != NULL){
        aw_kprintf("Mount Point already exit\r\n");
        return 0;
    }

    dev_index = atoi(argv[0]);
    lun_index = atoi(argv[1]);

    ums_dev =  usbh_ms_dev_open(USBDEV_IGONRE_PID, USBDEV_IGONRE_VID, dev_index);
    if(ums_dev != NULL){
        ums_lun = usbh_ms_valid_lun_get(ums_dev, lun_index);
        if(ums_lun != NULL){
            lun_name = ums_lun->name;
        }
        else{
            aw_kprintf("Do not find USB Mass Storage device lun\r\n");
            usbh_ms_dev_close(ums_dev);
            return 0;
        }
        usbh_ms_dev_close(ums_dev);
    }
    else{
        aw_kprintf("Do not find USB Mass Storage device\r\n");
        return 0;
    }

    p_mount_info = __mount_info_get();
    if(p_mount_info == NULL){
        aw_kprintf("Can not mount resource\r\n");
        return 0;
    }

    /* 文件系统挂载到 "/u" 结点  */
    ret = aw_mount(argv[2], lun_name, "vfat", 0);
    if (ret != AW_OK) {
        aw_kprintf("%s mount FATFS Fail: %d\r\n", argv[2], ret);
        usbh_ms_valid_lun_put(ums_lun);
        __mount_info_put(p_mount_info);
        return 0;
    }
    aw_kprintf("%s mount success\r\n", argv[2]);
    strcpy(p_mount_info->mount_point, argv[2]);
    p_mount_info->ums_lun = ums_lun;

    /* 获取U盘信息*/
    ret = aw_statvfs(argv[2], &fs_info);
    if(ret == AW_OK){
        total_size = fs_info.f_bsize * fs_info.f_blocks / (1024 * 1024);
        aw_kprintf("USB Mass Storage total  size: %d MB(s)\r\n", total_size);
        remain_size = fs_info.f_bsize * fs_info.f_bfree / (1024 * 1024);
        aw_kprintf("USB Mass Storage remain size: %d MB(s)\r\n", remain_size);
    }

    return 0;
}

/* 取消挂载USB大容量存储设备*/
static int __ums_umount(int argc, char *argv[], struct aw_shell_user *p_user){
    struct usbh_ms_mount_info *p_mount_info = NULL;

    if(argc != 1){
        aw_kprintf("Try \"ums_umount --help\" for more information.\r\n");
        return 0;
    }

    if(argc > 0){
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:ums_umount\r\n\r\n");
            aw_kprintf("Description:Umount USB Mass Storage lun\r\n\r\n");
            aw_kprintf("parameter:\r\n");
            aw_kprintf("    1.mount point :mount point\r\n");
            return 0;
        }
    }

    p_mount_info = __mount_point_check(argv[0]);
    if(p_mount_info == NULL){
        aw_kprintf("Mount Point do not exit\r\n");
        return 0;
    }

    aw_umount(argv[0], 0);
    usbh_ms_valid_lun_put(p_mount_info->ums_lun);
    __mount_info_put(p_mount_info);

    aw_kprintf("%s umount success\r\n", argv[0]);
    return 0;
}

/* USB大容量存储设备读写测试*/
static int __ums_wr_check (int argc, char *argv[], struct aw_shell_user *p_user){
    struct usbh_ms_mount_info *p_mount_info    = NULL;
    uint32_t                   data_size       = 0;
    uint8_t                   *data_buf        = NULL;
    int                        i;
    char                       path[32]        = {0};
    int                        handle, len;

    if(argc > 0){
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:ums_wr_check\r\n\r\n");
            aw_kprintf("Description:Test USB Mass Storage write and read\r\n\r\n");
            aw_kprintf("parameter:\r\n");
            aw_kprintf("    1.mount point :mount point\r\n");
            aw_kprintf("    2.data size   :write and read data size\r\n");
            return 0;
        }
    }

    if(argc != 2){
        aw_kprintf("Try \"ums_wr_check --help\" for more information.\r\n");
        return 0;
    }
    /* 检查挂载点*/
    p_mount_info = __mount_point_check(argv[0]);
    if(p_mount_info == NULL){
        aw_kprintf("Mount Point do not exit\r\n");
        return 0;
    }
    /* 获取要写的数据大小*/
    data_size = atoi(argv[1]);
    if(data_size == 0){
        aw_kprintf("data size invalid\r\n");
        return 0;
    }

    data_buf = aw_mem_alloc(data_size);
    if(data_buf == NULL){
        aw_kprintf("data buf alloc failed\r\n");
        return 0;
    }

    /* 写数据 */
    for (i = 0; i < data_size; i++) {
        data_buf[i] = i % 256;
    }
    /* 设置文件路径*/
    strcpy(path, argv[0]);
    strcat(path, "/ums_test.txt");

    handle = aw_open(path, O_RDWR | O_CREAT, 0777);
    if (handle < 0) {
        aw_kprintf("Creat file Fail: %d\r\n", handle);
        goto __end;
    }
    len = aw_write(handle, data_buf, data_size);
    if (len != data_size) {
        aw_close(handle);
        aw_kprintf("Write file Fail\r\n");
        goto __end;
    }
    /* 关闭文件 */
    aw_close(handle);
    aw_kprintf("Write %d data success\r\n", data_size);
    memset(data_buf, 0, data_size);

    /* 打开文件 */
    handle = aw_open(path, O_RDONLY, 0777);
    if (handle < 0) {
        aw_kprintf("Open file Fail: %d\r\n", handle);
        goto __end;
    }

    if (aw_read(handle, data_buf, data_size) != data_size) {
        aw_close(handle);
        aw_kprintf("Read file Fail\r\n");
        goto __end;
    }

    /* 关闭文件 */
    aw_close(handle);
    aw_kprintf("Read %d data success\r\n", data_size);
    /* 检验数据是否正确 */
    for (i = 0; i < data_size; i++) {
        if (data_buf[i] != i % 256) {
            aw_kprintf("File data check Fail\r\n");
            aw_close(handle);
        }
    }
    aw_kprintf("Data check success\r\n");

__end:
    aw_mem_free(data_buf);
    return 0;
}

/* USB大容量存储设备读写速度测试*/
static int __ums_wr_speed (int argc, char *argv[], struct aw_shell_user *p_user){
    struct usbh_ms_mount_info *p_mount_info    = NULL;
    uint32_t                   data_size       = 0;
    uint8_t                   *data_buf        = NULL;
    uint32_t                   loop_num        = 1;
    int                        i,j;
    int                        repeat          = 0;
    char                       path[32]        = {0};
    int                        handle, len;
    aw_tick_t                  start_ticks;
    aw_tick_t                  end_ticks;
    uint32_t                  *time_ms         = NULL;
    uint32_t                   time_ms_min     = 0;
    uint32_t                   time_ms_max     = 0;
    uint32_t                   average_time_ms = 0;
    float                      result          = 0;

    if(argc > 0){
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:ums_wr_speed\r\n\r\n");
            aw_kprintf("Description:Test USB Mass Storage write and read speed\r\n\r\n");
            aw_kprintf("parameter:\r\n");
            aw_kprintf("    1.mount point :mount point\r\n");
            aw_kprintf("    2.data size   :write and read data size\r\n");
            aw_kprintf("    3.repeat      :write and read times\r\n");
            return 0;
        }
    }

    if(argc != 3){
        aw_kprintf("Try \"ums_wr_speed --help\" for more information.\r\n");
        return 0;
    }
    /* 检查挂载点*/
    p_mount_info = __mount_point_check(argv[0]);
    if(p_mount_info == NULL){
        aw_kprintf("Mount Point do not exit\r\n");
        return 0;
    }
    /* 获取要写的数据大小*/
    data_size = atoi(argv[1]);
    if((data_size == 0) || (data_size < 1024 * 1024)){
        aw_kprintf("data size invalid\r\n");
        return 0;
    }
    /* 获取重复次数*/
    repeat = atoi(argv[2]);
    if(repeat < 3){
        aw_kprintf("repeat invalid\r\n");
        return 0;
    }
    /* 设置文件路径*/
    strcpy(path, argv[0]);
    strcat(path, "/ums_test.txt");

    data_buf = aw_mem_alloc(data_size);
    if(data_buf == NULL){
        data_buf = aw_mem_alloc(4 * 1024);
        if(data_buf == NULL){
            aw_kprintf("data buf alloc failed\r\n");
            return 0;
        }
        else{
            loop_num = data_size / (4 * 1024);
            data_size = 4 * 1024;
        }
    }

    time_ms = aw_mem_alloc(repeat * sizeof(uint32_t));
    if(time_ms == NULL){
        aw_kprintf("time_ms alloc failed\r\n");
        aw_mem_free(data_buf);
        return 0;
    }

    /* 写数据 */
    for (i = 0; i < data_size; i++) {
        data_buf[i] = i % 256;
    }

    handle = aw_open(path, O_RDWR | O_CREAT, 0777);
    if (handle < 0) {
        aw_kprintf("Creat file Fail: %d\r\n", handle);
        goto __end;
    }

    for (i = 0; i < repeat; i++) {
        /* 写文件 */
        start_ticks = aw_sys_tick_get();
        for (j = 0;j < loop_num; j++) {
            len = aw_write(handle, data_buf, data_size);
            if (len != data_size) {
                aw_close(handle);
                aw_kprintf("Write file Fail\r\n");
                goto __end;
            }
        }
        end_ticks  = aw_sys_tick_get();

        time_ms[i] = aw_ticks_to_ms(end_ticks - start_ticks);
        aw_kprintf("Write %d data success,use time %d(ms)\r\n", data_size, time_ms[i]);
    }
    /* 关闭文件 */
    aw_close(handle);

    time_ms_min = get_average_min(time_ms, repeat);
    time_ms_max = get_average_max(time_ms, repeat);

    /* 时间取平均值 */
    for (i = 0; i < repeat; i++) {
        average_time_ms += time_ms[i];
    }
    average_time_ms -= (time_ms_min + time_ms_max);
    average_time_ms /= (repeat - 2);
    result = (data_size * loop_num)/(float)average_time_ms;

    /* 打印速度 */
    aw_kprintf("Write average speed : %.2f KB/s\r\n", result);

    average_time_ms = 0;

    /*
     * 读文件测试（包括打开，文件读操作，关闭操作）
     */
    /* 打开文件 */
    handle = aw_open(path, O_RDONLY, 0777);
    if (handle < 0) {
        aw_kprintf("Open file Fail: %d\r\n", handle);
        goto __end;
    }

    for (i = 0; i < repeat; i++) {
        /* 读文件 */
        start_ticks = aw_sys_tick_get();
        for (j = 0;j < loop_num; j++) {
            if (aw_read(handle, data_buf, data_size) != data_size) {
                aw_close(handle);
                aw_kprintf("Read file Fail\r\n");
                goto __end;
            }
        }
        end_ticks  = aw_sys_tick_get();
        time_ms[i] = aw_ticks_to_ms(end_ticks - start_ticks);
        aw_kprintf("Read %d data success,use time %d(ms)\r\n", data_size, time_ms[i]);
    }
    /* 关闭文件 */
    aw_close(handle);

    /* 时间取平均值 */
    time_ms_min = get_average_min(time_ms, repeat);
    time_ms_max = get_average_max(time_ms, repeat);

    /* 时间取平均值 */
    for (i = 0; i < repeat; i++) {
        average_time_ms += time_ms[i];
    }
    average_time_ms -= (time_ms_min + time_ms_max);
    average_time_ms /= (repeat - 2);
    result = (data_size * loop_num)/(float)average_time_ms;

    /* 打印速度 */
    aw_kprintf("Read average speed : %.2f KB/s\r\n", result);

__end:
    aw_mem_free(data_buf);
    aw_mem_free(time_ms);
    return AW_OK;
}

/* 命令集 */
aw_local const struct aw_shell_cmd __g_ums_cmdset_cmds[] = {
        /* 检查USB协议栈内存状态*/
        {__ums_mem_sta,         "ums_mem",        "check USB Mass Storage mem status"},
        /* 遍历当前的UMS设备 */
        {__ums_dev_detect,      "ums_detect",     "Detect USB Mass Storage device"},
        /* 获取USB大容量存储设备分区的块大小*/
        {__ums_lun_bsize_get,   "ums_lun_bsize",  "Get USB Mass Storage lun block size"},
        /* 获取USB大容量存储设备分区的块数量*/
        {__ums_lun_nblks_get,   "ums_lun_nblks",  "Get USB Mass Storage lun block number"},
        /* 挂载USB大容量存储设备*/
        {__ums_mount,           "ums_mount",      "Mount USB Mass Storage"},
        /* 取消挂载USB大容量存储设备*/
        {__ums_umount,          "ums_umount",     "Umount USB Mass Storage"},
        /* USB大容量存储设备读写测试*/
        {__ums_wr_check,        "ums_wr_check",         "Test USB Mass Storage write and read"},
        /* USB大容量存储设备读写速度测试*/
        {__ums_wr_speed,        "ums_wr_speed",   "Test USB Mass Storage write and read speed"},
};

/* 命令集描述 */
aw_local struct aw_shell_cmdset __g_ums_cmdset = {
    .p_enter_name   = "ums_test",                  /* 命令集进入名字*/
    .p_exit_name    = "ums_exit",                  /* 命令集退出 */
    .p_man          = "This cmdset is use to test USB Mass Storage",
    .pfn_always_run = NULL,
    .pfn_entry_init = NULL,
};

/** 大容量存储设备设备热插拔检测回调函数*/
static void ums_monitor(void *arg, uint8_t type){
    if((type == USBDEV_INJECT) || (type == USBDEV_EXIST)){
        aw_kprintf("find a USB Mass Storage device\r\n");
        __g_ums_dev_num++;
    }
    else if(type == USBDEV_EJECT){
        aw_kprintf("remove a USB Mass Storage device\r\n");
        __g_ums_dev_num--;
    }
}

/** USB大容量存储设备例程入口函数*/
void demo_ums_entry(void){
    usbh_monitor_handle   ums_monitor_handle;

    /* 注册USB大容量存储设备监控*/
    ums_monitor_handle = usbh_dev_monitor_register(USBDEV_IGONRE_VID, USBDEV_IGONRE_PID,
                                                   USBH_DEV_UMS,
                                                   USBDEV_INJECT | USBDEV_EJECT | USBDEV_EXIST,
                                                   ums_monitor, NULL);
    if(ums_monitor_handle == NULL){
        aw_kprintf("ums monitor register failed!\r\n");
        return;
    }

    /* 注册一个命令子集 */
    AW_SHELL_REGISTER_CMDSET(&__g_ums_cmdset, __g_ums_cmdset_cmds);
}
