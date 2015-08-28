/*
 * Copyright (C) 2015 Alexandru Razvan Caciulescu
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>
#include <inttypes.h>


#include "net/lwm2m_cli/inst_util.h"


void lwm2m_cli_inst_dump(lwm2m_object_t *obj, uint16_t inst_id)
{
    (void)obj;
    printf("Dumping Instance %" PRIu16 ":\n", inst_id);

    /* TODO: read the instance and display it's resources */

    // int resources_len; int size;
    // lwm2m_tlv_t *resources;

    // uint8_t * buffer;
    // uint16_t res;

    // numData = 0;
    // res = obj->readFunc(instance, &resources_len, &resources, obj);
    // if (res != COAP_205_CONTENT)
    // {
    //     printf("error: unable to read instance\n");
    //     return;
    // }

    // /* TODO: djmp some more information */
    // // dump_tlv(stdout, numData, dataArray, 0);

    // size = lwm2m_tlv_serialize(numData, dataArray, &buffer);
    // printf("char objectTlv[%d] = {", size);
    // for (int i = 0 ; i < size ; i++)
    // {
    //     printf("0x%02X, ", buffer[i]);
    // }
    // printf("\b\b};\r\n");
    // lwm2m_tlv_free(numData, dataArray);
    // lwm2m_free(buffer);
}
