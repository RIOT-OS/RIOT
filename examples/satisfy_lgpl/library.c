/*
* Copyright (C) 2015 Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
*
* This file is subject to the terms and conditions of the GNU Lesser
* General Public License v2.1. See the file LICENSE in the top level
* directory for more details.
*/

/**
* @ingroup examples
* @{
* @file
* @brief Sample proprietary library code
*
* @author  Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
* @}
*
*/

#include "library.h"
#include "base64.h"

void HOOK_library_01(void) {
    unsigned char data_in[] = "[HOOK] Bazinga!";
    size_t data_in_size = 15;
    size_t base64_out_size = 0;

    unsigned char base64_out[20];
    base64_encode(data_in, data_in_size, NULL, &base64_out_size);
    base64_encode(data_in, data_in_size, base64_out, &base64_out_size);

    printf("\n%s ( %s )\n\n", (char*)data_in, (char*)base64_out );
}
