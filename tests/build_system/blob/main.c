/*
 * SPDX-FileCopyrightText: 2019 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       test application for including files as blobs
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "fmt.h"
#include "blob/blobtest.txt.h"
#include "blob/blob_subdir/blobtest_subdir.txt.h"

void blobtest_bin_print(void);

int main(void)
{
    print((char *)blobtest_txt, blobtest_txt_len);
    print("\n", 1);

    print((char *)blobtest_subdir_txt, blobtest_subdir_txt_len);
    print("\n", 1);

    blobtest_bin_print();

    return 0;
}
