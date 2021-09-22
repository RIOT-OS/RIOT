#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/*provide some test program*/
#include "blob/main.wasm.h"
#include "blob/hello.wasm.h"

bool iwasm_start_thread(void);
/* this seems to be a very direct interpretation of "i like to have a wamr_run" */
int wamr_run(void *bytecode, size_t bytecode_len, int argc, char **argv);
/* this creates a copy bytecode and argv
 * if argc is 0 it is set to 1 and argv[0] is set to ""
 * to create some space for a return value */
int wamr_run_cp(const void *bytecode, size_t bytecode_len, int argc, const char **argv);

#define telltruth(X) ((X) ? "true" : "false")

int main(void)
{
    printf("iwasm_thread_initilised: %s\n", telltruth(iwasm_start_thread()));

    int app_argc = 0;
    const char *app_argv1 = "test";
    const char **app_argv = 0;
    int ret = wamr_run_cp(main_wasm, main_wasm_len, app_argc, app_argv);
    printf("ret = %d\n", ret);
    ret = wamr_run_cp(hello_wasm, hello_wasm_len, app_argc, app_argv);
    printf("ret = %d\n", ret);
}
