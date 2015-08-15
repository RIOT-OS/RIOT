

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "beacon.h"
#include "math.h"
#include "board.h"
#include "debug.h"
#define ENABLE_DEBUG  0


node_regis_struc node_regis_list[MAX_LOCAL_NODE_NUM];


unit32_t subframeduration=0;
unit32_t cp_duration_us = 0;
unit32_t tdma_duration=0;
unit32_t sp_duration=0;


unit8_t total_tdma_node_num =0;
unit16_t total_tdma_slot_num =0;

unit8_t tdma_node_id_list[MAX_LOCAL_NODE_NUM];
unit8_t tdma_slot_number_list[MAX_LOCAL_NODE_NUM];
bool vtdma_exist = false;

unit8_t beacon_buf[120];



float drand(void)
{
    float   x;
    x=rand();
    x=65539*x+1743251541;
    x=fmod(x,2147483638);
    return(x/2147483638);
}

void mac_vtdma_list_generate(void)
{
    DEBUG("iQueue-MAC: start making the vTDMA list\n");

    total_tdma_node_num = 0;
    total_tdma_slot_num = 0;

    unit8_t i;
    unit8_t j = 0;
    bool slots_full = false;

    vtdma_exist = false;

    for(i=0; i<MAX_LOCAL_NODE_NUM; i++)
    {
        if(node_regis_list[i].node_ID == 0)  /* search for all the registered simple nodes! */
        {
            break;
        }

        if(node_regis_list[i].queue_length > 0)
        {
            tdma_node_id_list[j] = node_regis_list[i].node_ID;
            tdma_slot_number_list[j] = node_regis_list[i].queue_length;
            j ++;
            total_tdma_node_num ++;
            total_tdma_slot_num += node_regis_list[i].queue_length;

            while((total_tdma_slot_num * SLOT_DURATION)>(subframeduration - TDMA_TIME_BUFFER_DURATION))
            {
                slots_full = true;

                tdma_slot_number_list[total_tdma_node_num-1] --;
                total_tdma_slot_num --;

                if(tdma_slot_number_list[total_tdma_node_num-1] == 0)
                {
                    tdma_node_id_list[total_tdma_node_num-1] = 0;
                    total_tdma_node_num --;
                    break;
                }
            }
        }

        if(slots_full == true)
        {
            break;
        }

    }

    tdma_list_init();

    DEBUG("iQueue-MAC: Successfully generated vTDMA list\n");
    DEBUG("iQueue-MAC: %u TDMA slots are assigned\n", total_tdma_slot_num);
}


void tdma_list_init(void)
{
    if(total_tdma_slot_num > 0)
    {
        vtdma_exist = true;
        tdma_duration = total_tdma_slot_num*SLOT_DURATION + TDMA_TIME_BUFFER_DURATION;
    }
    else
    {
        vtdma_exist = false;
        tdma_duration = 0;
    }

    int i = 0;
    for(i=0; i<MAX_LOCAL_NODE_NUM; i++)
        node_regis_list[i].queue_length = 0;
}

void mac_creat_beacon(void)
{
    /* generate the random subframe duration */
    float r;
    r=drand();
    subframeduration = DEFAULT_SUBFRAME_DURATION/2+ r*DEFAULT_SUBFRAME_DURATION;

    DEBUG("iQueue-MAC: The subframeduration is %lu.\n", subframeduration);

    /* generate the vTDMA slots allocation lists */
    mac_vtdma_list_generate();

    cp_duration_us = A_UNIT_CP_PERIOD_uS;

    DEBUG("iQueue-MAC: loading beacon packet... \n");

    beacon_buf[0] = 14 + 2*total_tdma_node_num;                 /* identifies a "beacon" for S-CoSens */
    /* next subframeduration period duration in us */
    beacon_buf[1] = (subframeduration & 0xff);
    beacon_buf[2] = (subframeduration >> 8);
    beacon_buf[3] = (subframeduration >> 16);
    beacon_buf[4] = (subframeduration >> 24);
    beacon_buf[5] = 0xff;
    beacon_buf[6] = (cp_duration_us & 0xff);
    beacon_buf[7] = (cp_duration_us >> 8);
    beacon_buf[8] = (cp_duration_us >> 16);
    beacon_buf[9] = (cp_duration_us >> 24);
    beacon_buf[10] = 0xff;
    beacon_buf[12] = total_tdma_node_num;
    beacon_buf[13] = total_tdma_slot_num;


    if(total_tdma_slot_num > 0)
    {
        unit8_t i;
        for(i=1; i<=total_tdma_node_num; i++)
        {
            beacon_buf[13+i] = tdma_node_id_list[i-1];
            beacon_buf[13+total_tdma_node_num+i] = tdma_slot_number_list[i-1];
        }
    }

}





















