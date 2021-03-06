/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*   Default configuration for HTTPSRV.*/

#ifndef _HTTPSRV_CONFIG_H_
#define _HTTPSRV_CONFIG_H_

#include "httpsrv_types.h"
#include "httpsrv_string.h"
/* Configuration parameters.*/

/* Default HTTP port */
#ifndef HTTPSRV_CFG_DEFAULT_HTTP_PORT
#define HTTPSRV_CFG_DEFAULT_HTTP_PORT (80)
#endif

/* Default HTTPS port */
#ifndef HTTPSRV_CFG_DEFAULT_HTTPS_PORT
#define HTTPSRV_CFG_DEFAULT_HTTPS_PORT (443)
#endif

#ifndef HTTPSRV_CFG_DEFAULT_INDEX_PAGE
#define HTTPSRV_CFG_DEFAULT_INDEX_PAGE "index.html"
#endif

/* Number of seconds for caching */
#ifndef HTTPSRV_FILE_LOAD_BOUNDARY_SIZE
#define HTTPSRV_FILE_LOAD_BOUNDARY_SIZE  (128)
#endif

/* Number of seconds for caching */
#ifndef HTTPSRV_CFG_CACHE_MAXAGE
#define HTTPSRV_CFG_CACHE_MAXAGE (3600)
#endif

/* Session buffer size */
#ifndef HTTPSRV_CFG_SES_BUFFER_SIZE
#define HTTPSRV_CFG_SES_BUFFER_SIZE (1360)
#endif
#if HTTPSRV_CFG_SES_BUFFER_SIZE < 512
#undef HTTPSRV_CFG_SES_BUFFER_SIZE
#define HTTPSRV_CFG_SES_BUFFER_SIZE (512)
#endif

///* Maximal URL length */
//#ifndef HTTPSRV_CFG_DEFAULT_URL_LEN
//#define HTTPSRV_CFG_DEFAULT_URL_LEN (128)
//#endif

/* Maximal length for script line */
#ifndef HTTPSRV_CFG_MAX_SCRIPT_LN
#define HTTPSRV_CFG_MAX_SCRIPT_LN (32)
#endif

/* WebSocket protocol support */
#ifndef HTTPSRV_CFG_WEBSOCKET_ENABLED
#define HTTPSRV_CFG_WEBSOCKET_ENABLED (0)
#endif

/* WolfSSL support (TBD Under development - not working yet).*/
#ifndef HTTPSRV_CFG_WOLFSSL_ENABLE
#define HTTPSRV_CFG_WOLFSSL_ENABLE (0)
#endif

/* MbedTLS support (TBD Under development - not working yet).*/
#ifndef HTTPSRV_CFG_MBEDTLS_ENABLE
#define HTTPSRV_CFG_MBEDTLS_ENABLE (0)
#endif

/* Data storage mode */
//#define HTTPSRV_BIG_ENDIAN

#ifndef HTTPSRV_CFG_CGI_SUFFIX
#define HTTPSRV_CFG_CGI_SUFFIX              ".php"
#endif

#ifndef HTTPSRV_CFG_SSI_SUFFIX
#define HTTPSRV_CFG_SSI_SUFFIX              ".shtml"
#endif

#ifndef HTTPSRV_CFG_ABORT_ON_CGI_REQUEST
#define HTTPSRV_CFG_ABORT_ON_CGI_REQUEST 1
#endif


#if HTTPSRV_CFG_WOLFSSL_ENABLE && HTTPSRV_CFG_MBEDTLS_ENABLE
#error You may not enable both WolfSSL and MbedTLS simultaneously
#endif


#endif /* _HTTPSRV_CONFIG_H_ */
