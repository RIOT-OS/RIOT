#include <thread.h>
#include <kernel.h>
#include <msg.h>
#include <board.h>
#include <ringbuffer.h>

#include <stdio.h>
#include <errno.h>
#include <irq.h>
#include <posix_io.h>
#include "uart0.h"

//#define ENABLE_DEBUG
#include <debug.h>

static char buffer[UART0_BUFSIZE];
ringbuffer uart0_ringbuffer;

static void uart0_loop();

void uart0_init() {
    ringbuffer_init(&uart0_ringbuffer, buffer, UART0_BUFSIZE);
    int pid = thread_create(KERNEL_CONF_STACKSIZE_MAIN, PRIORITY_MAIN-1, CREATE_STACKTEST, uart0_loop, "uart0");
    uart0_handler_pid = pid;
    puts("uart0_init() [OK]");
}

static int min(int a, int b) {
    if (b>a) return a;
    else return b;
}

static void uart0_loop() {
    msg m;

    int pid = thread_getpid();

    int reader_pid = -1;
    struct posix_iop *r = NULL;

    puts("UART0 thread started.");

    while (1) {
        msg_receive(&m);

        if (m.sender_pid == pid) {
        } else {
            switch (m.type) {
                case OPEN:
                    if (reader_pid == -1) {
                        reader_pid = m.sender_pid;
                        m.content.value = 0; // no error
                    } else {
                        m.content.value = -EBUSY;
                    }
                    msg_reply(&m,&m);
                    break;
                case READ:
                    if (m.sender_pid != reader_pid) {
                        m.content.value = -EINVAL;
                        msg_reply(&m, &m);
                    } else {
                        r = (struct posix_iop *)m.content.ptr;
                    }
                    break;
                case CLOSE:
                    if (m.sender_pid == reader_pid) {
                        DEBUG("uart0_thread: closing file from %i\n", reader_pid);
                        reader_pid = -1;
                        r = NULL;
                        m.content.value = 0;
                    } else {
                        m.content.value = -EINVAL;
                    }
                    msg_reply(&m,&m);
                    break;
                default:
                    m.content.value = -EINVAL;
                    msg_reply(&m, &m);
            }
        }

        if (uart0_ringbuffer.avail && (r != NULL)) {
            int state = disableIRQ();
            int nbytes = min(r->nbytes, uart0_ringbuffer.avail);
            DEBUG("uart0_thread: sending %i bytes to pid %i\n", nbytes, reader_pid);
            rb_get_elements(&uart0_ringbuffer, r->buffer, nbytes);
            r->nbytes = nbytes;

            m.sender_pid = reader_pid;
            m.type = OPEN;
            m.content.ptr = (char*)r;

            msg_reply(&m, &m);
           // DEBUG("uart0_thread: sending res=%i\n", res);

            r = NULL;
            restoreIRQ(state);
        }
    }
}
