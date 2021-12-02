#ifndef __AWBL_IMX283_OCOTP__
#define __AWBL_IMX283_OCOTP__

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */



#include "aw_task.h"
#include "awbl_nvram.h"
#include "aw_sem.h"

#define AWBL_IMX6UL_OCOTP_NAME   "imx6ul_ocotp"

/**
 * \brief imx6ul ocotp �豸��Ϣ
 */
struct awbl_imx6ul_ocotp_devinfo {

    /** \brief ����ʧ�Դ洢�������б� */
    const struct awbl_nvram_segment *p_seglst;

    /** \brief �Ĵ�������ַ */
    uint32_t regbase;

    /** \brief ����ʱ��ID */
    uint32_t clk_id;

    /** \brief ����ʧ�Դ洢�������б���Ŀ�� */
    size_t seglst_count;
};

/**
 * \brief imx6ul ocotp �豸ʵ��
 */
struct awbl_imx6ul_ocotp_dev {
	struct awbl_dev super;
    AW_MUTEX_DECL(devlock);             /** \brief �豸�� */
};

/**
 * \brief lpc11xx driver register
 */
void awbl_imx6ul_ocotp_register(void);



#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif
