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
 * \brief imx6ul ocotp 设备信息
 */
struct awbl_imx6ul_ocotp_devinfo {

    /** \brief 非易失性存储段配置列表 */
    const struct awbl_nvram_segment *p_seglst;

    /** \brief 寄存器基地址 */
    uint32_t regbase;

    /** \brief 输入时钟ID */
    uint32_t clk_id;

    /** \brief 非易失性存储段配置列表条目数 */
    size_t seglst_count;
};

/**
 * \brief imx6ul ocotp 设备实例
 */
struct awbl_imx6ul_ocotp_dev {
	struct awbl_dev super;
    AW_MUTEX_DECL(devlock);             /** \brief 设备锁 */
};

/**
 * \brief lpc11xx driver register
 */
void awbl_imx6ul_ocotp_register(void);



#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif
