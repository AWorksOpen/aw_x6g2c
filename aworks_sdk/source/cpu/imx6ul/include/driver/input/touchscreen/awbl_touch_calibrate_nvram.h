
#ifndef AWBL_TOUCH_CALIBRATE_NVRAM_H
#define AWBL_TOUCH_CALIBRATE_NVRAM_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


#define TOUCH_CALIBRATE_NVRAM_NAME "touch_calibrate_nvram"

/**
 * \brief imx6ul adc 设备信息
 */
typedef struct awbl_touch_calibrate_nvram_devinfo {

    /* 2点校准文件路径 */
    char *two_pathname;

    /* 5点校准文件路径 */
    char *five_pathname;
} awbl_touch_calibrate_nvram_devinfo_t;

/**
 * \brief imx6ul adc 设备实例
 */
typedef struct awbl_touch_calibrate_nvram_dev {

    struct awbl_dev         super;             /**< \brief 继承自 AWBus 设备 */

} awbl_touch_calibrate_nvram_dev_t;


void awbl_touch_nvram_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* AWBL_TOUCH_CALIBRATE_NVRAM_H */
