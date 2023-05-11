#include "fmt.h"

#include "blob/blobtest.bin.h"

void blobtest_bin_print(void)
{
    for (size_t n = 0; n < blobtest_bin_len; n++) {
        print("0x", 2);
        print_byte_hex(blobtest_bin[n]);
        print("\n", 1);
    }
}
