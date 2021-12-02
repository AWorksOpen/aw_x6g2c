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

#include "qcloud_iot_export_error.h"
#include "qcloud_iot_export_log.h"
#include "qcloud_iot_import.h"

#ifdef LOG_UPLOAD

#define LOG_SAVE_FILE_PATH "upload-fail-save.log"

size_t HAL_Log_Save(const char *log, size_t wLen)
{
    int fd;
    size_t len;

    if ((fd = aw_open(LOG_SAVE_FILE_PATH, O_RDWR|O_APPEND)) < 0) {
        Log_e("fail to open file %s", LOG_SAVE_FILE_PATH);
        return 0;
    }

    len = aw_write(fd, (void *)log, wLen);
    Log_d("write %d of %d to log file", len, wLen);

    aw_close(fd);

    return len;
}

size_t HAL_Log_Read(char *buff, size_t rLen)
{
    int fd;
    size_t len;

    if ((fd = aw_open(LOG_SAVE_FILE_PATH, O_RDWR|O_APPEND)) < 0) {
        Log_e("fail to open file %s", LOG_SAVE_FILE_PATH);
        return 0;
    }

    len = aw_read(fd, (void *)buff, rLen);
    Log_d("read %d of %d from log file", len, rLen);

    aw_close(fd);

    return len;
}

int HAL_Log_Del(void)
{
    return remove(LOG_SAVE_FILE_PATH);
}

size_t HAL_Log_Get_Size(void)
{
    struct aw_stat buf;
    int ret;

    ret = aw_stat(LOG_SAVE_FILE_PATH, &buf);
    if (ret < 0) {
        return 0;
    }

    return (size_t)buf.st_size;
}

#endif
