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

#ifndef __AW_SEMB_H
#define __AW_SEMB_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_sem
 * \copydoc aw_sem.h
 * @{
 */



/**
 * \addtogroup grp_aw_if_semb 二进制信号量
 * @{
 */

  /**
 * \name 二进制信号量初始化状态
 * @{
 */

/**
 * \brief 信号量空
 * \hideinitializer
 */
#define AW_SEM_EMPTY            AW_PSP_SEM_EMPTY

/**
 * \brief 信号量满（用于二进制信号量）
 * \hideinitializer
 */
#define AW_SEM_FULL             AW_PSP_SEM_FULL

/** @} 二进制信号量初始化状态 */

typedef aw_psp_semb_id_t        aw_semb_id_t;      /**< \brief 二进制信号量ID*/

/**
 *  \brief 定义二进制信号量
 *
 * 本宏定义一个二进制信号量,在编译时完成信号量所需内存的分配; 在运行时, 需要调用
 * AW_SEMB_INIT() 完成信号量的初始化；调用 AW_SEMB_TERMINATE()终止信号量。
 *
 * 参数\a sem 只能作为宏 AW_SEMB_INIT(),  AW_SEMB_TAKE(),  AW_SEMB_GIVE()和
 * AW_SEMB_TERMINATE()的参数。
 *
 * AW_SEMB_INIT()返回信号量的\a sem_id, \a sem_id 作为函数 aw_semb_take() ,
 * aw_semb_give()和 aw_semb_terminate()的参数;这些函数的功能与对应的宏的功能相同,
 * 区别在于, 函数使用\a sem_id 作为参数,\a sem_id 可以作为信号量的句柄在函数调
 * 用之间传递。
 *
 * 可以使用本宏将信号量实体嵌入到另一个数据结构中,这样的话,分配那个大的数据对象
 * 的时候就自动完成了信号量实体的内存分配, 如下面的范例所示。
 *
 * \attention 若信号量没有跨模块(文件)使用的需求，推荐使用 AW_SEMB_DECL_STATIC()
 *            代替本宏，这样可避免模块之间信号量命名污染。
 *
 * \attention 除特殊用法外不, 切莫在函数内使用此宏来定义信号量，因为在函数内此宏
 *            分配的内存来自于栈上，一旦函数返回，分配的内存被自动回收。
 *            在函数内部使用 AW_SEMB_DECL_STATIC()来定义信号量。
 *
 * \param[in] sem 信号量实体, 与 AW_SEMB_INIT(), AW_SEMB_TAKE(), AW_SEMB_GIVE()
 *                和 AW_SEMB_TERMINATE() 一起使用。
 *
 * \par 范例：常规用法
 * \code
 *  #include "aw_sem.h"
 *
 *  AW_TASK_DECL_STATIC(task_a, 512);   // 定义任务 task_a
 *  AW_TASK_DECL_STATIC(task_b, 512);   // 定义任务 task_b
 *
 *  AW_SEMB_DECL_STATIC(sem);           // 定义二进制信号量 sem
 *
 *  void func_a (void *arg)
 *  {
 *      while (1) {
 *          //...
 *          AW_SEMB_GIVE(sem);          //释放信号量
 *      }
 *  }
 *
 *  void func_b (void *arg)
 *  {
 *      while (1) {
 *          AW_SEMB_TAKE(sem, AW_SEM_WAIT_FOREVER); //等待信号量
 *          //...
 *      }
 *  }
 *
 *  int main ()
 *  {
 *      // 初始化信号量
 *      AW_SEMB_INI(sem, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);
 *
 *      // 初始化任务：
 *      AW_TASK_INIT(task_a,            // 任务实体
 *                   "task_a",          // 任务名字
 *                   5,                 // 任务优先级
 *                   512,               // 任务堆栈大小
 *                   func_a,            // 任务入口函数
 *                   NULL);             // 任务入口参数
 *
 *      AW_TASK_INIT(task_b,            // 任务实体
 *                   "task_b",          // 任务名字
 *                   6,                 // 任务优先级
 *                   512,               // 任务堆栈大小
 *                   func_b,            // 任务入口函数
 *                   NULL);             // 任务入口参数
 *
 *      // 启动任务
 *      AW_TASK_STARTUP(task_a);
 *      AW_TASK_STARTUP(task_b);
 *
 *      return 0;
 *  }
 * \endcode
 *
 * \par 范例：将信号量实体嵌入另一个数据结构
 * \code
 *  #include "aw_sem.h"
 *
 *  struct my_struct {
 *      int my_data1;
 *      AW_TASK_DECL(task_a, 512);          // 任务实体task_a, 堆栈512
 *      AW_TASK_DECL(task_b, 512);          // 任务实体task_b, 堆栈512
 *      AW_SEMB_DECL(sem);                  // 信号量实体
 *      aw_semb_id_t sem_id;                // 信号量ID
 *  };
 *  struct my_struct   my_object;           // 定义大的数据对象
 *
 *  void func_a (void *arg)
 *  {
 *      struct my_struct *p_myobj = arg;
 *      while (1) {
 *          //...
 *          AW_SEMB_GIVE(p_myobj->sem);     //释放信号量(使用信号量实体)
 *      }
 *  }
 *
 *  void func_b (void *arg)
 *  {
 *      struct my_struct *p_myobj = arg;
 *      while (1) {
 *          // 等待信号量(使用信号量ID)
 *          aw_semb_take(p_myobj->sem_id, AW_SEM_WAIT_FOREVER);
 *          //...
 *      }
 *  }
 *
 *  int main ()
 *  {
 *
 *      // 初始化信号量
 *      my_object.sem_id = AW_SEMB_INIT(my_object.sem,
 *                                      AW_SEM_EMPTY,
 *                                      AW_SEM_Q_PRIORITY);
 *      if (my_object.sem_id == NULL) {
 *          return -1;                      // 信号量初始化失败
 *      }
 *
 *      // 初始化任务：
 *      AW_TASK_INIT(my_object.task_a,      // 任务实体
 *                   "task_a",              // 任务名字
 *                   5,                     // 任务优先级
 *                   512,                   // 任务堆栈大小
 *                   func_a,                // 任务入口函数
 *                   (void *)&my_object);   // 任务入口参数
 *
 *      AW_TASK_INIT(my_object.task_b,      // 任务实体
 *                   "task_b",              // 任务名字
 *                   6,                     // 任务优先级
 *                   512,                   // 任务堆栈大小
 *                   func_b,                // 任务入口函数
 *                   (void *)&my_object);   // 任务入口参数
 *
 *      // 启动任务
 *      AW_TASK_STARTUP(my_object.task_a);
 *      AW_TASK_STARTUP(my_object.task_b);
 *
 *      aw_mdelay(20 * 1000);   //延时20秒
 *
 *      // AW_SEMB_TERMINATE(my_object.sem);    // 终止信号量
 *      // aw_semb_terminate(my_object.sem_id); // 终止信号量(使用信号量ID)
 *
 *      return 0;
 *  }
 * \endcode
 *
 * \sa AW_SEMB_DECL_STATIC(), AW_SEMB_INIT(), AW_SEMB_TAKE(), AW_SEMB_GIVE,
 *     AW_SEMB_TERMINATE()
 * \sa aw_semb_take(), aw_semb_give(), aw_semb_terminate()
 * \hideinitializer
 */
#define AW_SEMB_DECL(sem)       AW_PSP_SEMB_DECL(sem)

/**
 * \brief 定义二进制信号量(静态)
 *
 * 本宏与 AW_SEMB_DECL() 的功能相同, 区别在于, AW_SEMB_DECL_STATIC() 在定义信
 * 号量所需内存时, 使用关键字 \b static , 如此一来, 便可以将信号量实体的作用域
 * 限制在模块内(文件内)从而避免模块之间的信号量命名冲突；还可以在函数内使用本
 * 宏定义信号量。
 *
 * \param[in] sem 信号量实体, 与 AW_SEMB_INIT(), AW_SEMB_TAKE(),
 *                AW_SEMB_GIVE() 和 aw_semb_terminate() 一起使用。
 *
 * \par 范例
 * 见 AW_SEMB_DECL()
 *
 * \sa AW_SEMB_DECL(), AW_SEMB_INIT(), AW_SEMB_TAKE(), AW_SEMB_GIVE,
 *     AW_SEMB_TERMINATE()
 * \sa aw_semb_take(), aw_semb_give(), aw_semb_terminate()
 * \hideinitializer
 */
#define AW_SEMB_DECL_STATIC(sem)    AW_PSP_SEMB_DECL_STATIC(sem)

/**
 * \brief 引用二进制信号量
 *
 * 本宏引入一个二进制信号量, 相当于使用关键 \b extern 声明一个外部符号。
 * 如果要使用另一个模块(文件)中定义的信号量, 首先使用本宏引入该信号量, 然后就
 * 可以操作该信号量。被引入的信号量应当是使用 AW_SEMB_DECL()定义的。
 *
 * \param[in] sem  信号量实体
 *
 * \par 范例
 * \code
 *  #include "aw_sem.h"
 *
 *  AW_SEMB_IMPORT(sem_x);      // 引入信号量 sem_x
 *
 *  int func ()
 *  {
 *      AW_SEMB_TAKE(sem_x);    // 等待信号量 sem_x
 *  }
 * \endcode
 *
 * \sa AW_SEMB_DECL(), AW_SEMB_DECL_STATIC(), AW_SEMB_INIT(), AW_SEMB_TAKE(),
 *     AW_SEMB_GIVE(), AW_SEMB_TERMINATE()
 * \sa aw_semb_take(), aw_semb_give(), aw_semb_terminate()
 * \hideinitializer
 */
#define AW_SEMB_IMPORT(sem)     AW_PSP_SEMB_IMPORT(sem)

/**
 * \brief 二进制信号量初始化
 *
 * 初始化由 AW_SEMB_DECL() 或 AW_SEMB_DECL_STATIC()静态定义的二进制信号量,
 * 或嵌入到另一个大数据对象中的二进制信号量。信号量被初始化为 #AW_SEM_EMPTY 或
 * #AW_SEM_FULL 状态。
 *
 * 二进制信号量的选项决定了阻塞于此信号量的任务的排队方式, 可以按任务本身优先级
 * 或先进先出的方式排队, 它们分别对应 #AW_SEM_Q_PRIORITY 和 #AW_SEM_Q_FIFO 。
 *
 * \param[in] sem           信号量实体,由 AW_SEMB_DECL()或
 *                          AW_SEMB_DECL_STATIC()定义
 * \param[in] initial_state 初始状态:
 *                              - #AW_SEM_EMPTY
 *                              - #AW_SEM_FULL
 * \param[in] options       初始化选项标志:
 *                              - #AW_SEM_Q_FIFO
 *                              - #AW_SEM_Q_PRIORITY
 *
 * \return 成功返回二进制信号量的ID, 失败则返回NULL
 *
 * \sa AW_SEM_Q_FIFO, AW_SEM_Q_PRIORITY
 * \sa AW_SEMB_DECL(), AW_SEMB_DECL_STATIC(), AW_SEMB_TAKE(), AW_SEMB_GIVE(),
 *     AW_SEMB_TERMINATE()
 * \sa aw_semb_take(), aw_semb_give(), aw_semb_terminate()
 * \hideinitializer
 */
#define AW_SEMB_INIT(sem, initial_state, options) \
            AW_PSP_SEMB_INIT(sem, initial_state, options)

/**
 * \brief 获取二进制信号量
 *
 * 获取信号量。如果信号量为空, 则任务将会被挂起直到信号量变得可用(其它任务调用
 * 了 AW_SEMB_GIVE()或 aw_semb_give())。如果信号量已经是可用的, 则本调用将会
 * 清空该信号量, 则其它任务不能获取到该信号量, 直到调用的任务释放此信号量。
 *
 * 参数 \a timeout 控制调用的阻塞:
 *   - 为 #AW_SEM_WAIT_FOREVER 时, 调用将会被阻塞, 直到该信号量被释放或删除,
 *     可以通过返回值来判断调用结果:AW_OK -成功获取到信号量；-AW_ENXIO -信号量已
 *     经被摧毁
 *
 *   - 为 #AW_SEM_NO_WAIT 时,调用不会被阻塞,立即返回,通过返回值来判断调用结果;
 *     AW_OK -成功获取到信号量; -AW_EAGAIN -未获取到信号量,稍后再试;
 *     -AW_ENXIO -信号量已经被终止
 *
 *   - 正数值为超时时间, 此时调用有超时地阻塞, 在设定的超时时间到达时, 若还未获
 *     取到信号量, 则调用返回, 可以通过返回值来判断调用结果： AW_OK -成功获取到
 *     信号量; -AW_ETIME -超时,未获取到信号量,稍后再试; -AW_ENXIO -信号量已经被
 *     终止
 *
 *   - 其它负数值为非法参数
 *
 * \param[in]   sem      信号量实体,由 AW_SEMB_DECL() 或
 *                       AW_SEMB_DECL_STATIC() 定义
 * \param[in]   timeout  最大等待时间(超时时间), 单位是系统节拍, 系统时钟节拍频率
 *                       由函数 aw_sys_clkrate_get()获得, 可使用 aw_ms_to_ticks()
 *                       将毫秒转换为节拍数; #AW_SEM_NO_WAIT 为无阻塞;
 *                       #AW_SEM_WAIT_FOREVER 为一直等待, 无超时
 *
 * \retval   AW_OK     成功
 * \retval  -AW_EINVAL 参数无效
 * \retval  -AW_EPERM  不允许的操作,在中断中调用了本函数
 * \retval  -AW_ETIME  获取信号量超时
 * \retval  -AW_ENXIO  信号量已经被终止
 * \retval  -AW_EAGAIN 获取信号量失败, 稍后再试 (\a timeout = #AW_SEM_NO_WAIT)
 *
 * \par 范例
 * 更多范例请参考 AW_SEMB_DECL()
 * \code
 *  #include "aw_sem.h"
 *
 *  AW_SEMB_TAKE(sem, AW_SEM_WAIT_FOREVER);         // 永久等待
 *
 *  aw_err_t ret;
 *  ret = AW_SEMB_TAKE(sem, aw_ms_to_ticks(500));   //超时等待,超时时间 500ms
 *  if (ret == -AW_ETIME) {                         // 等待信号量超时
 *      //...
 *  }
 *
 *  aw_err_t ret;
 *  ret = AW_SEMB_TAKE(sem, AW_SEM_NO_WAIT);        // 无等待
 *  if (ret == -AW_EAGAIN) {                        // 未获取到信号量
 *      //...
 *  }
 * \endcode
 * \sa AW_SEMB_DECL(), AW_SEMB_DECL_STATIC(), AW_SEMB_INIT(), AW_SEMB_GIVE(),
 *     AW_SEMB_TERMINATE()
 * \sa aw_semb_take(), aw_semb_give(), aw_semb_terminate()
 * \hideinitializer
 */
#define AW_SEMB_TAKE(sem, timeout)  AW_PSP_SEMB_TAKE(sem, timeout)

/**
 * \brief 释放二进制信号量
 *
 * 释放信号量。如果有更高优先级的任务获取了该信号量, 该任务就准备好了运行, 并且
 * 会立即抢占调用 AW_SEMB_GIVE() 的任务。若信号量是满的(即信号量被释放, 但是没
 * 有任务获取), 则没有任何操作。
 *
 * \param[in] sem 信号量实体,由 AW_SEMB_DECL() 或 AW_SEMB_DECL_STATIC 定义
 *
 * \retval   AW_OK     成功
 * \retval  -AW_EINVAL 参数无效
 *
 * \par 范例
 * 请参考 AW_SEMB_DECL()
 */
#define AW_SEMB_GIVE(sem)   AW_PSP_SEMB_GIVE(sem)

/**
 * \brief 重置二进制信号量
 *
 * \param[in] sem         二进制信号量实体,由 AW_SEMB_DECL() 或
 *                        AW_SEMB_DECL_STATIC() 定义
 * \param[in] reset_value 二进制信号量的重置值(0或者非0)
 *
 * \retval   AW_OK     成功
 * \retval  -AW_EINVAL 参数无效
 *
 * \par 范例
 * 请参考 AW_SEMC_DECL()
 * \sa AW_SEMC_DECL(), AW_SEMC_DECL_STATIC(), AW_SEMC_INIT(), AW_SEMC_TAKE(),
 *     AW_SEMC_TERMINATE()
 * \sa aw_semc_take(), aw_semc_give(), aw_semc_terminate()
 * \hideinitializer
 */
#define AW_SEMB_RESET(sem, reset_value)   AW_PSP_SEMB_RESET(sem, reset_value)

/**
 * \brief 终止二进制信号量
 *
 * 终止信号量。任何等待此信号量的任务将会解阻塞, 并返回 -AW_ENXIO 。此函数不会释
 * 放信号量相关的任何内存。
 *
 * \param[in] sem 信号量实体,由 AW_SEMB_DECL() 或 AW_SEMB_DECL_STATIC() 定义
 *
 * \retval   AW_OK     成功
 * \retval  -AW_EINVAL 参数无效
 *
 * \par 范例
 * 请参考 AW_SEMB_DECL() 和 AW_SEMB_DECL_STATIC()
 * \sa AW_SEMB_DECL(), AW_SEMB_DECL_STATIC(), AW_SEMB_INIT(), AW_SEMB_TAKE(),
 *     AW_SEMB_GIVE()
 * \sa aw_semb_take(), aw_semb_give(), aw_semb_terminate()
 * \hideinitializer
 */
#define AW_SEMB_TERMINATE(sem)  AW_PSP_SEMB_TERMINATE(sem)

/**
 * \brief 获取二进制信号量
 *
 * 本函数的功能与 AW_SEMB_TAKE() 相同, 差别在于, 本函数使用\a sem_id 作为参数。
 * \a sem_id 由 AW_SEMB_INIT() 返回。
 *
 * \param[in]   sem_id   信号量ID,由 AW_SEMB_INIT()返回
 * \param[in]   timeout  最大等待时间(超时时间), 单位是系统节拍, 系统时钟节拍频率
 *                       由函数 aw_sys_clkrate_get()获得, 可使用 aw_ms_to_ticks()
 *                       将毫秒转换为节拍数; #AW_SEM_NO_WAIT 为无阻塞;
 *                       #AW_SEM_WAIT_FOREVER 为一直等待, 无超时
 *
 * \retval   AW_OK     成功
 * \retval  -AW_EINVAL 参数无效
 * \retval  -AW_EPERM  不允许的操作,在中断中调用了本函数
 * \retval  -AW_ETIME  获取信号量超时
 * \retval  -AW_ENXIO  信号量已经被终止
 * \retval  -AW_EAGAIN 获取信号量失败, 稍后再试 (\a timeout = #AW_SEM_NO_WAIT)
 *
 * \par 范例
 * 更多范例请参考 AW_SEMB_DECL()
 * \code
 *  #include "aw_sem.h"
 *
 *  AW_SEMB_DECL_STATIC(sem);                   // 定义信号量实体
 *  aw_semb_id_t  sem_id;                       // 信号量ID
 *
 *  sem_id = AW_SEMB_INI(sem);                  // 初始化信号量, 得到信号量ID
 *
 *  aw_semb_take(sem_id, AW_SEM_WAIT_FOREVER);  // 永久等待
 *
 *  aw_err_t ret;
 *  ret = aw_semb_take(sem_id, aw_ms_to_ticks()); // 超时时间 500ms
 *  if (ret == -AW_ETIME) {                       // 等待信号量超时
 *      //...
 *  }
 *
 *  aw_err_t ret;
 *  ret = aw_semb_take(sem, AW_SEM_NO_WAIT);    // 无等待
 *  if (ret == -AW_EAGAIN) {                    // 未获取到信号量
 *      //...
 *  }
 * \endcode
 * \sa AW_SEMB_DECL(), AW_SEMB_DECL_STATIC(), AW_SEMB_INIT(), AW_SEMB_TAKE(),
 *     AW_SEMB_GIVE(), AW_SEMB_TERMINATE()
 * \sa aw_semb_give(), aw_semb_terminate()
 * \hideinitializer
 */
aw_err_t aw_semb_take (aw_semb_id_t sem_id, aw_tick_t timeout);

/**
 * \brief 释放二进制信号量
 *
 * 本函数的功能与 AW_SEMB_GIVE() 相同, 差别在于, 本函数使用\a sem_id 作为参数。
 * \a sem_id 由 AW_SEMB_INIT() 返回。
 *
 * \param[in] sem_id 信号量ID,由 AW_SEMB_INIT()返回
 *
 * \retval   AW_OK     成功
 * \retval  -AW_EINVAL 参数无效
 *
 * \par 范例
 * 请参考 AW_SEMB_DECL()
 *
 * \sa AW_SEMB_DECL(), AW_SEMB_DECL_STATIC(), AW_SEMB_INIT(), AW_SEMB_TAKE(),
 *     AW_SEMB_GIVE(), AW_SEMB_TERMINATE()
 * \sa aw_semb_take(), aw_semb_terminate()
 * \hideinitializer
 */
aw_err_t aw_semb_give (aw_semb_id_t sem_id);

/**
 * \brief 重置二进制信号量
 *
 * \param[in] sem_id      二进制信号量ID,由 AW_SEMB_INIT()返回
 * \param[in] reset_value 二进制信号量的重置值(0或者非0)
 *
 * \retval   AW_OK     成功
 * \retval  -AW_EINVAL 参数无效
 *
 * \par 范例
 * 请参考 AW_SEMC_DECL()
 * \sa AW_SEMC_DECL(), AW_SEMC_DECL_STATIC(), AW_SEMC_INIT(), AW_SEMC_TAKE(),
 *     AW_SEMC_TERMINATE()
 * \sa aw_semc_take(), aw_semc_give(), aw_semc_terminate()
 * \hideinitializer
 */
aw_err_t aw_semb_reset (aw_semb_id_t sem_id, unsigned int reset_value);

/**
 * \brief 终止二进制信号量
 *
 * 终止信号量。任何等待此信号量的任务将会解阻塞, 并返回 -AW_ENXIO 。此函数不会释
 * 放信号量相关的任何内存。
 *
 * \param[in] sem_id 信号量实体,由 AW_SEMB_DECL() 或 AW_SEMB_DECL_STATIC() 定义
 *
 * \retval   AW_OK     成功
 * \retval  -AW_EINVAL 参数无效
 *
 * \par 范例
 * 请参考 AW_SEMB_DECL()
 *
 * \sa AW_SEMB_DECL(), AW_SEMB_DECL_STATIC(), AW_SEMB_INIT(), AW_SEMB_TAKE(),
 *     AW_SEMB_GIVE(), AW_SEMB_TERMINATE()
 * \sa aw_semb_take(), aw_semb_give()
 * \hideinitializer
 */
aw_err_t aw_semb_terminate (aw_semb_id_t sem_id);


/** @} grp_aw_if_semb */
/** @} grp_aw_if_sem */

#ifdef __cplusplus
}
#endif

#endif /* __AW_SEMB_H */

/* end of file */
