/**
 * @file
 * @brief Simple tty shell configurations
 *
 * @internal
 * @par history
 * - 120715, liuweiyun, first implementation
 * @endinternal
 */

#ifndef __SHELL_CFG_H /* { */
#define __SHELL_CFG_H

/**
 * @defgroup shell_cfg Shell configurations
 * The tty shell dependent on standard I/O
 * @{
 */

/**
 * SHELL_ENABLED            shell enabled
 * SHELL_THREAD_PRIO        shell thread priority
 * SHELL_THREAD_STK_SIZ     Shell thread stack size
 */
#define SHELL_ENABLED
#define SHELL_THREAD_PRIO       (1u)
#define SHELL_THREAD_STK_SIZ    4096u

/** Use shell symbol */
//#define SHELL_USE_SYMBOL

/** Use cursor */
#define SHELL_USE_CURSOR

/**
 * SHELL_USE_DAEMON             Enable shell command run as daemon
 * SHELL_DAEMON_THREAD_PRIO     Daemon thread prio
 * SHELL_DAEMON_THREAD_STKSIZ   Daemon thread stack size
 */
//#define SHELL_USE_DAEMON
#define SHELL_DAEMON_THREAD_PRIO    (1u)
#define SHELL_DAEMON_THREAD_STKSIZ  1024u

/**
 * SHELL_USE_HISTORY        Use history
 * SHELL_HISTORY_COUNT      History count
 */
#define SHELL_USE_HISTORY
#define SHELL_HISTORY_COUNT         9u

/**
 * SHELL_USE_MEMDUMP      Memory dump command
 */
//#define SHELL_USE_MEMDUMP

/* @} */

#endif  /* } __SHELL_CFG_H  */

/*  end of file */

