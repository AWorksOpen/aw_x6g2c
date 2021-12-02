/*
 * reqsched.h
 *
 *  Created on: 2020年9月11日
 *      Author: wanglinfu
 */

#ifndef USER_CODE_REQSCHED_REQSCHED_OS_H_
#define USER_CODE_REQSCHED_REQSCHED_OS_H_

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aworks.h"
#include "aw_assert.h"

/**
 * \brief 错误码
 */
enum rs_err_code {
    RS_OK           = AW_OK,
    RS_EINVAL       = AW_EINVAL,
    RS_ENOTFOUND    = AW_ENOSR,
    RS_ENOMEM       = AW_ENOMEM,
    RS_ETIME        = AW_ETIMEDOUT,
    RS_ENOTSUP      = AW_ENOTSUP,

    RS_ESCHED       = AW_ENODEV,                    /** \brief 调度器无效 */
    RS_ENOREQ       = AW_ENOENT,                    /** \brief 未发现该请求 */
    RS_EPROCESSING  = AW_EINPROGRESS,               /** \brief 请求正在处理 */
};

#define ASSERT(cond)        aw_assert(cond)
#define ALIGN(x, align)     AW_ROUND_UP(x, align)

#ifdef __cplusplus
}
#endif /* __cplusplus   */


#endif /* USER_CODE_REQSCHED_REQSCHED_OS_H_ */
