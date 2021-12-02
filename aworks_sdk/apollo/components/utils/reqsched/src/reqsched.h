/**
 * \file
 * \brief 异步请求响应处理功能，适用于耗时较长需异步处理的工作，处理完毕后需要回复响应数据的场景；
 */

#ifndef USER_CODE_REQSCHED_REQSCHED_H_
#define USER_CODE_REQSCHED_REQSCHED_H_

#include "reqsched_os.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


/** \brief 将 reqsched_hdr_t 类型的回调函数的 p_data 参数转换为该请求的 token  */
#define REQSCHED_DATA_TO_TOKEN(p_data)  \
    (uint32_t)container_of(p_data, reqsched_arg_t, data)

typedef int32_t rs_err_t;

/**
 * \brief 请求的数据结点
 */
typedef struct reqsched_arg {
    struct reqsched_arg *p_next;
    uint8_t              data[];
} reqsched_arg_t;

/**
 * \brief 操作系统相关的接口结构
 */
typedef struct {
    void       *wakeup;
    void     (*pfn_task_create) (const char *name, void (*cb) (void));
    void     (*pfn_semb_init) (void **p);
    rs_err_t (*pfn_semb_pend) (void *semb, uint32_t systick_tm);
    rs_err_t (*pfn_semb_release) (void *semb);
    void     (*pfn_mutex_init) (void **mutex);
    rs_err_t (*pfn_mutex_lock) (void *mutex, uint32_t systick_tm);
    rs_err_t (*pfn_mutex_unlock) (void *mutex);
    void     (*pfn_log) (void         *p_color,
                     const char   *func_name,
                     int           line,
                     const char   *fmt, ...);
} reqsched_os_t;

/**
 * \brief 请求处理回调类型定义
 */
typedef void (*reqsched_hdr_t) (uint32_t reqid, void *p_data, size_t size);


/**
 * \brief 操作系统相关接口初始化
 *
 * \param[in]   p_os  回调接口结构体
 */
void reqsched_os_init (reqsched_os_t *p_os);

/**
 * \brief 创建一个请求调度器
 *
 * \param[in]   pfn_req_hdr         请求处理回调
 * \param[in]   req_max_cache_num   请求最大缓存数量
 * \param[in]   req_arg_size        请求参数大小
 *
 * \retval  0                       创建失败
 * \retval  >0                      请求id
 */
uint32_t reqsched_create (reqsched_hdr_t pfn_req_hdr,
                          uint32_t       req_max_cache_num,
                          size_t         req_arg_size);
/**
 * \brief   销毁一个请求调度器
 *
 * \param[in]   reqid           请求调度器的id
 *
 * \retval  RS_OK               操作成功
 * \retval  -RS_ENOTSUP         不支持销毁功能
 * \retval  else                查看  rs_err_code
 */
rs_err_t reqsched_destroy (uint32_t reqid);

/**
 * \brief 发出一个请求
 *
 * \param[in]  reqid            请求的id
 * \param[in]  p_data           请求参数，因为只提供了一个请求参数，因此该参数应包含请求和响应的结构；
 * \param[in]  size             参数大小
 * \param[out] p_token          用于存放请求token
 *
 * \retval  RS_OK               操作成功
 * \retval  else                查看  rs_err_code
 */
rs_err_t reqsched_req_send (uint32_t  reqid,
                            void     *p_data,
                            size_t    size,
                            uint32_t *p_token);
/**
 * \brief   发出一个响应
 *
 * \param[in]   reqid    请求的id
 * \param[in]   p_data   响应参数，该参数必须为请求处理回调的参数 p_data;
 *                       因为发出请求时，只提供了一个请求参数，因此该参数包含请求和响应的结构；
 *
 * \retval  RS_OK               操作成功
 * \retval  else                查看  rs_err_code
 */
rs_err_t reqsched_rsp_send (uint32_t reqid, void *p_data);

/**
 * \brief   尝试获取请求的响应
 *
 * \param[in]   reqid    请求的id
 * \param[in]   token    请求token
 * \param[in]   p_data   存放响应的地址，其实际类型与发出请求时相同
 * \param[in]   size     获取的响应大小
 *
 * \retval  RS_OK                   操作成功
 * \retval  -RS_EPROCESSING         该请求正在处理
 * \retval  else                    查看  rs_err_code
 */
rs_err_t reqsched_rsp_tryget (uint32_t   reqid,
                              uint32_t   token,
                              void      *p_data,
                              size_t     size);

/**
 * \brief   获取空闲的请求缓存数量
 *
 * \param[in]   reqid    请求的id
 *
 * \retval  >=0                     空闲数量
 * \retval  <0                      查看错误码
 */
int reqsched_req_available (uint32_t   reqid);

#ifdef __cplusplus
}
#endif /* __cplusplus   */


#endif /* USER_CODE_REQSCHED_REQSCHED_H_ */
