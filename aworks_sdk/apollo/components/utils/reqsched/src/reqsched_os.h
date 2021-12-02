/*
 * reqsched.h
 *
 *  Created on: 2020��9��11��
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
 * \brief ������
 */
enum rs_err_code {
    RS_OK           = AW_OK,
    RS_EINVAL       = AW_EINVAL,
    RS_ENOTFOUND    = AW_ENOSR,
    RS_ENOMEM       = AW_ENOMEM,
    RS_ETIME        = AW_ETIMEDOUT,
    RS_ENOTSUP      = AW_ENOTSUP,

    RS_ESCHED       = AW_ENODEV,                    /** \brief ��������Ч */
    RS_ENOREQ       = AW_ENOENT,                    /** \brief δ���ָ����� */
    RS_EPROCESSING  = AW_EINPROGRESS,               /** \brief �������ڴ��� */
};

#define ASSERT(cond)        aw_assert(cond)
#define ALIGN(x, align)     AW_ROUND_UP(x, align)

#ifdef __cplusplus
}
#endif /* __cplusplus   */


#endif /* USER_CODE_REQSCHED_REQSCHED_OS_H_ */
