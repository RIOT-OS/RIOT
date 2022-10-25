#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <err.h>

#include "ut_process.h"

#define BUFF_SIZE 1000


int main(void)
{
    char input_buf[BUFF_SIZE];
    char output_buf[BUFF_SIZE];

    ssize_t input_len = read(STDIN_FILENO, input_buf, BUFF_SIZE);

    if (input_len == -1) {
        exit(EXIT_SUCCESS);
        return 0;
    }
    if (input_len < BUFF_SIZE) {
        input_buf[input_len] = '\0';
    }
    else {
        input_buf[BUFF_SIZE - 1] = '\0';
    }

    /* typically looks like this: "http://www.example.com/foo{?query,number}" */
    ut_process_var_t vars[] = {
        { .name = input_buf, .value = &input_buf[11], },
        { .name = &input_buf[22], .value = &input_buf[33], },
    };
    size_t res_buf_len = BUFF_SIZE;

    ut_process_str_expand(&input_buf[44], vars, 2, output_buf, &res_buf_len);
    exit(EXIT_SUCCESS);
    return 0;
}
