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

/**
 * \file
 * \brief contains the global variable daylight.
 *
 * \internal
 * \par modification history:
 * - 1.00 14-09-26 orz, first implementation.
 * \endinternal
 */
 
#ifndef __RTT_FCNTL_H__
#define __RTT_FCNTL_H__

/* Operation flags */
#ifndef O_RDONLY
#define O_RDONLY        0x0000000
#endif

#ifndef O_WRONLY
#define O_WRONLY        0x0000001
#endif

#ifndef O_RDWR
#define O_RDWR          0x0000002
#endif

#ifndef O_ACCMODE
#define O_ACCMODE       0x0000003
#endif

#ifndef O_CREAT
#define O_CREAT         0x0000100
#endif

#ifndef O_EXCL
#define O_EXCL          0x0000200
#endif

#ifndef O_TRUNC
#define O_TRUNC         0x0001000
#endif

#ifndef O_APPEND
#define O_APPEND        0x0002000
#endif

#ifndef O_DIRECTORY
#define O_DIRECTORY     0x0200000
#endif

#ifndef O_BINARY
#define O_BINARY        0x0008000
#endif
#endif
