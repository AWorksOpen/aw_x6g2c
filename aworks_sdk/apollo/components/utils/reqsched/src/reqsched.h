/**
 * \file
 * \brief �첽������Ӧ�����ܣ������ں�ʱ�ϳ����첽����Ĺ�����������Ϻ���Ҫ�ظ���Ӧ���ݵĳ�����
 */

#ifndef USER_CODE_REQSCHED_REQSCHED_H_
#define USER_CODE_REQSCHED_REQSCHED_H_

#include "reqsched_os.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


/** \brief �� reqsched_hdr_t ���͵Ļص������� p_data ����ת��Ϊ������� token  */
#define REQSCHED_DATA_TO_TOKEN(p_data)  \
    (uint32_t)container_of(p_data, reqsched_arg_t, data)

typedef int32_t rs_err_t;

/**
 * \brief ��������ݽ��
 */
typedef struct reqsched_arg {
    struct reqsched_arg *p_next;
    uint8_t              data[];
} reqsched_arg_t;

/**
 * \brief ����ϵͳ��صĽӿڽṹ
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
 * \brief ������ص����Ͷ���
 */
typedef void (*reqsched_hdr_t) (uint32_t reqid, void *p_data, size_t size);


/**
 * \brief ����ϵͳ��ؽӿڳ�ʼ��
 *
 * \param[in]   p_os  �ص��ӿڽṹ��
 */
void reqsched_os_init (reqsched_os_t *p_os);

/**
 * \brief ����һ�����������
 *
 * \param[in]   pfn_req_hdr         ������ص�
 * \param[in]   req_max_cache_num   ������󻺴�����
 * \param[in]   req_arg_size        ���������С
 *
 * \retval  0                       ����ʧ��
 * \retval  >0                      ����id
 */
uint32_t reqsched_create (reqsched_hdr_t pfn_req_hdr,
                          uint32_t       req_max_cache_num,
                          size_t         req_arg_size);
/**
 * \brief   ����һ�����������
 *
 * \param[in]   reqid           �����������id
 *
 * \retval  RS_OK               �����ɹ�
 * \retval  -RS_ENOTSUP         ��֧�����ٹ���
 * \retval  else                �鿴  rs_err_code
 */
rs_err_t reqsched_destroy (uint32_t reqid);

/**
 * \brief ����һ������
 *
 * \param[in]  reqid            �����id
 * \param[in]  p_data           �����������Ϊֻ�ṩ��һ�������������˸ò���Ӧ�����������Ӧ�Ľṹ��
 * \param[in]  size             ������С
 * \param[out] p_token          ���ڴ������token
 *
 * \retval  RS_OK               �����ɹ�
 * \retval  else                �鿴  rs_err_code
 */
rs_err_t reqsched_req_send (uint32_t  reqid,
                            void     *p_data,
                            size_t    size,
                            uint32_t *p_token);
/**
 * \brief   ����һ����Ӧ
 *
 * \param[in]   reqid    �����id
 * \param[in]   p_data   ��Ӧ�������ò�������Ϊ������ص��Ĳ��� p_data;
 *                       ��Ϊ��������ʱ��ֻ�ṩ��һ�������������˸ò��������������Ӧ�Ľṹ��
 *
 * \retval  RS_OK               �����ɹ�
 * \retval  else                �鿴  rs_err_code
 */
rs_err_t reqsched_rsp_send (uint32_t reqid, void *p_data);

/**
 * \brief   ���Ի�ȡ�������Ӧ
 *
 * \param[in]   reqid    �����id
 * \param[in]   token    ����token
 * \param[in]   p_data   �����Ӧ�ĵ�ַ����ʵ�������뷢������ʱ��ͬ
 * \param[in]   size     ��ȡ����Ӧ��С
 *
 * \retval  RS_OK                   �����ɹ�
 * \retval  -RS_EPROCESSING         ���������ڴ���
 * \retval  else                    �鿴  rs_err_code
 */
rs_err_t reqsched_rsp_tryget (uint32_t   reqid,
                              uint32_t   token,
                              void      *p_data,
                              size_t     size);

/**
 * \brief   ��ȡ���е����󻺴�����
 *
 * \param[in]   reqid    �����id
 *
 * \retval  >=0                     ��������
 * \retval  <0                      �鿴������
 */
int reqsched_req_available (uint32_t   reqid);

#ifdef __cplusplus
}
#endif /* __cplusplus   */


#endif /* USER_CODE_REQSCHED_REQSCHED_H_ */
