/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <err.h>
#include <unistd.h>

#include "uri_parser.h"
#include "fuzzing.h"

int main(void)
{
    size_t input_len;
    char *input_buf = (char *)fuzzing_read_bytes(STDIN_FILENO, &input_len);

    if (input_buf == NULL) {
        errx(EXIT_FAILURE, "fuzzing_read_bytes failed");
    }

    uri_parser_result_t uri_res;

    uri_parser_process(&uri_res, input_buf, input_len);

    exit(EXIT_SUCCESS);
    return EXIT_SUCCESS;
}
