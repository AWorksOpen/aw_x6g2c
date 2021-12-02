
#ifndef AWBL_TOUCH_CALIBRATE_NVRAM_H
#define AWBL_TOUCH_CALIBRATE_NVRAM_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


#define TOUCH_CALIBRATE_NVRAM_NAME "touch_calibrate_nvram"

/**
 * \brief imx6ul adc �豸��Ϣ
 */
typedef struct awbl_touch_calibrate_nvram_devinfo {

    /* 2��У׼�ļ�·�� */
    char *two_pathname;

    /* 5��У׼�ļ�·�� */
    char *five_pathname;
} awbl_touch_calibrate_nvram_devinfo_t;

/**
 * \brief imx6ul adc �豸ʵ��
 */
typedef struct awbl_touch_calibrate_nvram_dev {

    struct awbl_dev         super;             /**< \brief �̳��� AWBus �豸 */

} awbl_touch_calibrate_nvram_dev_t;


void awbl_touch_nvram_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* AWBL_TOUCH_CALIBRATE_NVRAM_H */
