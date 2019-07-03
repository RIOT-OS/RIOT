#include "thread.h"
#include "log_module.h"
#include "log.h"

#ifndef LOG_MQUEUE_PRIORITY
    #define LOG_MQUEUE_PRIORITY (THREAD_PRIORITY_MAIN - 1)
#endif

#ifndef SYSLOG_MSG_BUF_NUMOF
    #define SYSLOG_MSG_BUF_NUMOF (4)
#endif

static char stack[(THREAD_STACKSIZE_DEFAULT)];
static char log_mqueue_rbuf_buf[SYSLOG_MSG_BUF_NUMOF*sizeof(log_msg_t)];
static ringbuffer_t log_mqueue_rbuf;
kernel_pid_t log_mqueue_pid;
pipe_t log_mqueue_pipe;
log_mqueue_handler_t log_mqueue_handlers[LOG_NFACILITIES];

static void log_mqueue_default_handler(int priority, int facility, char *msg, int msg_size,kernel_pid_t pid) {
    (void)priority;
    (void)facility;
    (void)msg_size;
    (void)pid;
    printf("%s",msg);
    return;
}

static void log_mqueue_dispatch(log_msg_t *msg) {
    int facility = (msg->level & (INT_MAX-((1<<3)-1)));
    int level    = (msg->level & (((uint32_t)1 << LOG_LEVEL_BITS) - 1));
    log_mqueue_handlers[(msg->level>>3)](level,facility,msg->buf,msg->len,msg->pid);
}

static void *thread(void *opt) {
    (void)opt;
    printf("run log backend\n");
    ringbuffer_init(&log_mqueue_rbuf,log_mqueue_rbuf_buf,sizeof(log_mqueue_rbuf_buf));
    pipe_init(&log_mqueue_pipe,&log_mqueue_rbuf,NULL);
    while(true) {
        char buf[sizeof(log_msg_t)];
        log_msg_t *msg = (log_msg_t *)buf;
        pipe_read(&log_mqueue_pipe,&buf,sizeof(buf));
        log_mqueue_dispatch(msg);
    }
    return NULL;
}

void log_mqueue_init(void)
{
    for(int i = 0;i<LOG_NFACILITIES;i++) {
        log_mqueue_handlers[i] = log_mqueue_default_handler;
    }
    log_mqueue_pid = thread_create(stack, sizeof(stack),
                            LOG_MQUEUE_PRIORITY, THREAD_CREATE_STACKTEST, thread, NULL, "log_mqueue");
    return;
}
