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

#include "aworks.h"
#include "aw_errno.h"
#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"
#include "io/aw_stropts.h"
#include "aw_ioctl.h"
#include "fcntl.h"
#include "unistd.h"
#include "sys/stat.h"
#include "string.h"
#include "stdarg.h"
#include <reent.h>
#include "aw_vdebug.h"
#include "io/sys/aw_stat.h"

#ifdef CONFIG_AW_TEST_GCOV

aw_local int __make_dir_path(char *new_path)
{
    char tmp_path[255];
    int i = 0,j = 0;
    int len = strlen(new_path);
    int ret = -1;
    while(len--) {
        if (new_path[i] == '/') {
            j++;
            if (j > 2) {
                memcpy(tmp_path, new_path, i + 1);
                tmp_path[i] = '\0';

                if (!strcmp(&tmp_path[strlen(tmp_path) - strlen(".gcda")], ".gcda"))
                {
                    return AW_OK;
                } else {
                    ret = aw_mkdir(tmp_path, S_IRWXU | S_IRWXG | S_IRWXO);
                    if (ret) {
                        if(ret != -AW_EEXIST) {
                            aw_kprintf("aw_mkdir err, ret is %d\n", ret);
                        } else {
                            ret = AW_OK;
                        }
                    }

                }

            }
        }
        i++;
    }
    return ret;
}


static int _posix_oflags_to_aw(int flags)
{
    int      aw_oflag = 0;

    switch (flags & O_ACCMODE) {
    case O_RDONLY:
        aw_oflag |= O_RDONLY;
        break;
    case O_WRONLY:
        aw_oflag |= O_WRONLY;
        break;
    default:
        aw_oflag |= O_RDWR;
        break;
    }

    if (flags & O_APPEND) {
        aw_oflag |= O_APPEND;
    }

    if (flags & O_CREAT) {
        aw_oflag |= O_CREAT;
    }

    if (flags & O_EXCL) {
        aw_oflag |= O_EXCL;
    }

    if (flags & O_TRUNC) {
        aw_oflag |= O_TRUNC;
    }

    return aw_oflag;
}

static int _posix_open_mode_to_aw(int mode)
{
    int      aw_mode = 0;

    if (mode & S_IXUSR) {
        aw_mode |= S_IXUSR;
    }
    if (mode & S_IWUSR) {
        aw_mode |= S_IWUSR;
    }
    if (mode & S_IRUSR) {
        aw_mode |= S_IRUSR;
    }

    if (mode & S_IXGRP) {
        aw_mode |= S_IXGRP;
    }
    if (mode & S_IWGRP) {
        aw_mode |= S_IWGRP;
    }
    if (mode & S_IRGRP) {
        aw_mode |= S_IRGRP;
    }

    if (mode & S_IXOTH) {
        aw_mode |= S_IXOTH;
    }
    if (mode & S_IWOTH) {
        aw_mode |= S_IWOTH;
    }
    if (mode & S_IROTH) {
        aw_mode |= S_IROTH;
    }

    return aw_mode;
}

__attribute__((__used__)) int
_open_r(struct _reent *ptr, const char *file, int flags, int mode)
{
    int      aw_oflag;
    int       aw_mode;
    int             fd;
    int i = 0;
    int ret = 0;

    aw_oflag = _posix_oflags_to_aw(flags);
    aw_mode = _posix_open_mode_to_aw(mode);

    if (file[1] == ':') {
        aw_kprintf("%s\n",file);

        char * aw_filename = (char *)malloc(strlen(file) + 4);
        sprintf(aw_filename,"/sd%s",&file[2]);

        for(i = 0; i < strlen(aw_filename); i ++){
            if(aw_filename[i] == '\\'){
                aw_filename[i] = '/';
            }
        }

        aw_kprintf("filename:%s\r\n",aw_filename);
        ret = __make_dir_path(aw_filename);
        if (ret) {
            aw_kprintf("make dir path error\n");
        }

        fd = aw_open(aw_filename,aw_oflag | O_CREAT,aw_mode);
    } else {
        fd = aw_open(file,aw_oflag,aw_mode);
    }

    return fd;
}

#endif

