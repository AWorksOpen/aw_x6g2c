/*
 * Tencent is pleased to support the open source community by making IoT Hub
 available.
 * Copyright (C) 2018-2020 THL A29 Limited, a Tencent company. All rights
 reserved.

 * Licensed under the MIT License (the "License"); you may not use this file
 except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software
 distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 KIND,
 * either express or implied. See the License for the specific language
 governing permissions and
 * limitations under the License.
 *
 */
#include <stdio.h>
#include <stdlib.h>

#include "AWorks.h"
#include "aw_ioctl.h"
#include "io/aw_stropts.h"
#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"
#include "io/sys/aw_stat.h"

typedef struct {
    int fd;
} hal_file_t;

void *HAL_FileOpen(const char *filename, const char *mode)
{
    hal_file_t *hal_file;
    int fd;

    fd = aw_open(filename, O_RDWR, 0777);
    if (fd < 0) {
        return NULL;
    }

    hal_file = aw_mem_alloc(sizeof(hal_file_t));
    if (hal_file == NULL) {
        return NULL;
    }
    memset(hal_file, 0, sizeof(hal_file_t));

    hal_file->fd = fd;
    return (void *)hal_file;
}

size_t HAL_FileRead(void *ptr, size_t size, size_t nmemb, void *fp)
{
    return aw_read(((hal_file_t*)fp)->fd, ptr, size*nmemb);
}

size_t HAL_FileWrite(const void *ptr, size_t size, size_t nmemb, void *fp)
{
    return aw_write(((hal_file_t*)fp)->fd, ptr, size*nmemb);
}

int HAL_FileSeek(void *fp, long int offset, int whence)
{
    return aw_lseek(((hal_file_t*)fp)->fd, offset, whence);
}

int HAL_FileClose(void *fp)
{
    int fd = ((hal_file_t*)fp)->fd;

    aw_mem_free(fp);

    return aw_close(fd);
}

int HAL_FileRemove(const char *filename)
{
    return aw_unlink(filename);
}

int HAL_FileRewind(void *fp)
{
    aw_lseek(((hal_file_t*)fp)->fd, 0, SEEK_SET);

    return 0;
}

long HAL_FileTell(void *fp)
{
    long offset = 0;
    aw_ioctl(((hal_file_t*)fp)->fd, AW_FIOWHERE, &offset);
    return offset;
}

long HAL_FileSize(void *fp)
{
    long size = 0;
    struct aw_stat buf;

    aw_fstat(((hal_file_t*)fp)->fd, &buf);
    size = buf.st_size;

    return size;
}

char *HAL_FileGets(char *str, int n, void *fp)
{
    return NULL;
}

int HAL_FileRename(const char *old_filename, const char *new_filename)
{
    return aw_rename(old_filename, new_filename);
}

int HAL_FileEof(void *fp)
{
    return 0;
}

int HAL_FileFlush(void *fp)
{
    return aw_fsync(((hal_file_t*)fp)->fd);
}
