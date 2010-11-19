#include <shell.h>
#include <board_uart0.h>
#include <posix_io.h>
#include <thread.h>
#include <board.h>
#include <hwtimer.h>

#define SHELL_STACK_SIZE    (4096)

char shell_stack_buffer[SHELL_STACK_SIZE];

shell_t shell;
const shell_command_t sc[] = {{NULL, NULL, NULL}};

void shell_runner(void) {
    shell_init(&shell, sc, uart0_readc, uart0_putc);
    posix_open(uart0_handler_pid, 0);
    shell_run(&shell);
}

int main(void) {
   thread_create(shell_stack_buffer, SHELL_STACK_SIZE, PRIORITY_MAIN-1, CREATE_STACKTEST, shell_runner, "shell");
   
   while (1) {
       LED_GREEN_TOGGLE;
       hwtimer_wait(1000 * 1000);
   }
}
