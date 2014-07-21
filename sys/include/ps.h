
/**
 * @defgroup    sys_ps PS
 * @ingroup     sys
 * @brief       Show list with all threads
 */

#ifndef __PS_H
#define __PS_H


#ifdef __cplusplus
extern "C" {
#endif


void thread_print_all(void);
void _ps_handler(int argc, char **argv);


#ifdef __cplusplus
}
#endif

#endif /* __PS_H */
