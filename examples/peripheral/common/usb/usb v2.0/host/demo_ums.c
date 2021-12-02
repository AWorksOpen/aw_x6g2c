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
 * \brief ͨ��USB�������豸��ʾ����
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - ��Ӧƽ̨��USBH���USBHЭ��ջ
 *      - AW_COM_CONSOLE
 *   2. �������е�DURX��DUTXͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *   3. �ڵװ��USB�˿��в���U��
 *
 * - ʵ������
 *   1. ����ָ����ڴ�ӡ������Ϣ
 *
 * \brief ����˵��
 * ums_mem       ��鵱ǰUSB�������洢�������ڴ������ͷ�״̬
 * ums_detect    ��⵱ǰ���ڵ�USB�������洢�豸���᷵���豸�ţ������źͷ�������
 * ums_lun_bsize ��ȡ�����Ŀ��С����Ҫ����������������һ�����豸�ţ��ڶ����Ƿ�����
 * ums_lun_nblks ��ȡ�����Ŀ���������Ҫ����������������һ�����豸�ţ��ڶ����Ƿ�����
 * ums_mount     ���ط�������Ҫ����������������һ�����豸�ţ��ڶ����Ƿ����ţ�
 *               ���������ǹ��ص㣬����"ums_mount 0 0 /u"
 * ums_umount    ȡ�����ط������贫����ص���������硰ums_umount /u��
 * ums_wr_check  �ļ�ϵͳ��д��⣬�贫��������������һ���ǹ��ص㣬�ڶ����Ƕ�д�����ݴ�С
 * ums_wr_speed  �ļ�ϵͳ��д�ٶȼ�⣬�贫��������������һ���ǹ��ص㣬
 *               �ڶ����Ƕ�д���ݴ�С�����ݴ�С����Ҫ1M��С���ϣ������������Ƕ�д�ظ�������3�����ϣ�
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

/* ��ȡ��Сֵ */
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

/* ��ȡ���ֵ */
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

/** ��ѯUSB�ڴ�ѵ�ʹ�����shell����*/
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

/** ��⵱ǰ���ڵ�USB�������洢�豸*/
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

    /* ��ȡUMS�豸����*/
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

/* ��ȡUSB�������洢�豸�����Ŀ��С*/
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

/* ����USB�������洢�豸*/
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

    /* �ļ�ϵͳ���ص� "/u" ���  */
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

    /* ��ȡU����Ϣ*/
    ret = aw_statvfs(argv[2], &fs_info);
    if(ret == AW_OK){
        total_size = fs_info.f_bsize * fs_info.f_blocks / (1024 * 1024);
        aw_kprintf("USB Mass Storage total  size: %d MB(s)\r\n", total_size);
        remain_size = fs_info.f_bsize * fs_info.f_bfree / (1024 * 1024);
        aw_kprintf("USB Mass Storage remain size: %d MB(s)\r\n", remain_size);
    }

    return 0;
}

/* ȡ������USB�������洢�豸*/
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

/* USB�������洢�豸��д����*/
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
    /* �����ص�*/
    p_mount_info = __mount_point_check(argv[0]);
    if(p_mount_info == NULL){
        aw_kprintf("Mount Point do not exit\r\n");
        return 0;
    }
    /* ��ȡҪд�����ݴ�С*/
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

    /* д���� */
    for (i = 0; i < data_size; i++) {
        data_buf[i] = i % 256;
    }
    /* �����ļ�·��*/
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
    /* �ر��ļ� */
    aw_close(handle);
    aw_kprintf("Write %d data success\r\n", data_size);
    memset(data_buf, 0, data_size);

    /* ���ļ� */
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

    /* �ر��ļ� */
    aw_close(handle);
    aw_kprintf("Read %d data success\r\n", data_size);
    /* ���������Ƿ���ȷ */
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

/* USB�������洢�豸��д�ٶȲ���*/
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
    /* �����ص�*/
    p_mount_info = __mount_point_check(argv[0]);
    if(p_mount_info == NULL){
        aw_kprintf("Mount Point do not exit\r\n");
        return 0;
    }
    /* ��ȡҪд�����ݴ�С*/
    data_size = atoi(argv[1]);
    if((data_size == 0) || (data_size < 1024 * 1024)){
        aw_kprintf("data size invalid\r\n");
        return 0;
    }
    /* ��ȡ�ظ�����*/
    repeat = atoi(argv[2]);
    if(repeat < 3){
        aw_kprintf("repeat invalid\r\n");
        return 0;
    }
    /* �����ļ�·��*/
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

    /* д���� */
    for (i = 0; i < data_size; i++) {
        data_buf[i] = i % 256;
    }

    handle = aw_open(path, O_RDWR | O_CREAT, 0777);
    if (handle < 0) {
        aw_kprintf("Creat file Fail: %d\r\n", handle);
        goto __end;
    }

    for (i = 0; i < repeat; i++) {
        /* д�ļ� */
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
    /* �ر��ļ� */
    aw_close(handle);

    time_ms_min = get_average_min(time_ms, repeat);
    time_ms_max = get_average_max(time_ms, repeat);

    /* ʱ��ȡƽ��ֵ */
    for (i = 0; i < repeat; i++) {
        average_time_ms += time_ms[i];
    }
    average_time_ms -= (time_ms_min + time_ms_max);
    average_time_ms /= (repeat - 2);
    result = (data_size * loop_num)/(float)average_time_ms;

    /* ��ӡ�ٶ� */
    aw_kprintf("Write average speed : %.2f KB/s\r\n", result);

    average_time_ms = 0;

    /*
     * ���ļ����ԣ������򿪣��ļ����������رղ�����
     */
    /* ���ļ� */
    handle = aw_open(path, O_RDONLY, 0777);
    if (handle < 0) {
        aw_kprintf("Open file Fail: %d\r\n", handle);
        goto __end;
    }

    for (i = 0; i < repeat; i++) {
        /* ���ļ� */
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
    /* �ر��ļ� */
    aw_close(handle);

    /* ʱ��ȡƽ��ֵ */
    time_ms_min = get_average_min(time_ms, repeat);
    time_ms_max = get_average_max(time_ms, repeat);

    /* ʱ��ȡƽ��ֵ */
    for (i = 0; i < repeat; i++) {
        average_time_ms += time_ms[i];
    }
    average_time_ms -= (time_ms_min + time_ms_max);
    average_time_ms /= (repeat - 2);
    result = (data_size * loop_num)/(float)average_time_ms;

    /* ��ӡ�ٶ� */
    aw_kprintf("Read average speed : %.2f KB/s\r\n", result);

__end:
    aw_mem_free(data_buf);
    aw_mem_free(time_ms);
    return AW_OK;
}

/* ��� */
aw_local const struct aw_shell_cmd __g_ums_cmdset_cmds[] = {
        /* ���USBЭ��ջ�ڴ�״̬*/
        {__ums_mem_sta,         "ums_mem",        "check USB Mass Storage mem status"},
        /* ������ǰ��UMS�豸 */
        {__ums_dev_detect,      "ums_detect",     "Detect USB Mass Storage device"},
        /* ��ȡUSB�������洢�豸�����Ŀ��С*/
        {__ums_lun_bsize_get,   "ums_lun_bsize",  "Get USB Mass Storage lun block size"},
        /* ��ȡUSB�������洢�豸�����Ŀ�����*/
        {__ums_lun_nblks_get,   "ums_lun_nblks",  "Get USB Mass Storage lun block number"},
        /* ����USB�������洢�豸*/
        {__ums_mount,           "ums_mount",      "Mount USB Mass Storage"},
        /* ȡ������USB�������洢�豸*/
        {__ums_umount,          "ums_umount",     "Umount USB Mass Storage"},
        /* USB�������洢�豸��д����*/
        {__ums_wr_check,        "ums_wr_check",         "Test USB Mass Storage write and read"},
        /* USB�������洢�豸��д�ٶȲ���*/
        {__ums_wr_speed,        "ums_wr_speed",   "Test USB Mass Storage write and read speed"},
};

/* ������� */
aw_local struct aw_shell_cmdset __g_ums_cmdset = {
    .p_enter_name   = "ums_test",                  /* �����������*/
    .p_exit_name    = "ums_exit",                  /* ����˳� */
    .p_man          = "This cmdset is use to test USB Mass Storage",
    .pfn_always_run = NULL,
    .pfn_entry_init = NULL,
};

/** �������洢�豸�豸�Ȳ�μ��ص�����*/
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

/** USB�������洢�豸������ں���*/
void demo_ums_entry(void){
    usbh_monitor_handle   ums_monitor_handle;

    /* ע��USB�������洢�豸���*/
    ums_monitor_handle = usbh_dev_monitor_register(USBDEV_IGONRE_VID, USBDEV_IGONRE_PID,
                                                   USBH_DEV_UMS,
                                                   USBDEV_INJECT | USBDEV_EJECT | USBDEV_EXIST,
                                                   ums_monitor, NULL);
    if(ums_monitor_handle == NULL){
        aw_kprintf("ums monitor register failed!\r\n");
        return;
    }

    /* ע��һ�������Ӽ� */
    AW_SHELL_REGISTER_CMDSET(&__g_ums_cmdset, __g_ums_cmdset_cmds);
}
