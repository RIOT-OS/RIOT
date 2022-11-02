#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <err.h>

#include "fuzzing.h"
#include "ut_process.h"

#define BUFF_SIZE 1000

int main(void)
{
    char output_buf[BUFF_SIZE];
    
    size_t input_len;
    char *input_buf = (char *)fuzzing_read_bytes(STDIN_FILENO, &input_len);
    if (input_buf == NULL) {
        errx(EXIT_FAILURE, "fuzzing_read_bytes failed");
    }

    /* typically looks like this: "http://www.example.com/foo{?query,number}" */
    ut_process_var_t vars[] = {
        { .name = "query", .value = "foo", },
        { .name = "number", .value = "1234567", },
    };
    size_t res_buf_len = BUFF_SIZE;

    ut_process_expand(input_buf, input_len, vars, 2, output_buf, &res_buf_len);

    exit(EXIT_SUCCESS);
    return 0;
}
