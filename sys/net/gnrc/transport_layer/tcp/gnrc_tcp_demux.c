#include "random.h"
#include "tcp_internal/demux.h"

#include <stdio.h>

uint32_t _demux_build_context(const uint16_t peer_port, const uint16_t local_port)
{
    return (peer_port << 16) | local_port;
}

uint8_t _demux_is_context_unique(const uint16_t peer_port, const uint16_t local_port)
{
    if( gnrc_netreg_num(GNRC_NETTYPE_TCP, _demux_build_context(peer_port, local_port)) == 0 ){
        return 1;
    }
    return 0;
}

int8_t _demux_register(gnrc_tcp_tcb_t *tcb)
{
    tcb->netreg_entry.demux_ctx = _demux_build_context(tcb->peer_port, tcb->local_port);
    tcb->netreg_entry.pid = tcb->owner;
    gnrc_netreg_register(GNRC_NETTYPE_TCP, &tcb->netreg_entry);
    return 0;
}

int8_t _demux_remove(gnrc_tcp_tcb_t *tcb)
{
    tcb->netreg_entry.demux_ctx = _demux_build_context(tcb->peer_port, tcb->local_port);
    tcb->netreg_entry.pid = tcb->owner;
    gnrc_netreg_unregister(GNRC_NETTYPE_TCP, &tcb->netreg_entry);
    return 0;
}

int8_t _demux_change_context(gnrc_tcp_tcb_t *tcb, uint16_t new_peer_port, uint16_t new_local_port)
{
    _demux_remove(tcb);
    tcb->peer_port = new_peer_port;
    tcb->local_port = new_local_port;
    _demux_register(tcb);
    return 0;
}


uint16_t _demux_find_free_port(const uint8_t dst_port)
{
        uint16_t rnd_val = (genrand_uint32() >> 16);
        while( rnd_val < BEG_REG_PORTS 
              || gnrc_netreg_num(GNRC_NETTYPE_TCP, _demux_build_context(rnd_val, dst_port)) != 0
             )
        {
            rnd_val = (genrand_uint32() >> 16);
        }
        return rnd_val;
}

