#include "tcp_internal/option.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

int8_t _option_parse(gnrc_tcp_tcb_t *tcb, tcp_hdr_t *hdr)
{
    uint8_t word_idx = 0;
    uint8_t byte_idx = 0;
    uint8_t word_end = 0;
    uint32_t word = 0;
    uint16_t off_ctl = byteorder_ntohs(hdr->off_ctl);

    word_end = GET_OFFSET(off_ctl) - OPTION_OFFSET_BASE;

    /* There are existing options */
    while(word_idx < word_end){
        /* Read current word from options */
        word = byteorder_ntohl(hdr->options[word_idx]);

        /* If byte index is not aligned to word index. Fill word with bytes from next word. */
        if(byte_idx){
            word >>= (byte_idx * 8);
            word |= (byteorder_ntohl(hdr->options[word_idx+1]) << ((sizeof(word)-byte_idx)*8));
        }

        /* Option handling */
        switch(OPT_GET_KIND(word)){
            case OPT_KIND_EOL:
                /* Option: End of List */
                DEBUG("tcp: Option eol\n");
                return 0;

            case OPT_KIND_NOP:
                /* Option: No operation */
                byte_idx += 1;
                DEBUG("tcp: Option nop\n");
                break;

            case OPT_KIND_MSS:
                /* Option: Maximum Segment Size */
                DEBUG("tcp: Option mss\n");
                if(OPT_GET_LENGTH(word) == OPT_LENGTH_MSS){
                    tcb->mss = OPT_GET_VAL_2B(word);
                    byte_idx += 4;
                }else{
                    DEBUG("tcp: invalid MSS Option length.\n");
                    return -1;
                }
                break;

                /* Add options support HERE */
            default:
                DEBUG("tcp: Unsupported option received\n");
                byte_idx += 1;
        }

        /* Update index */
        if(byte_idx >= 4){
            word_idx += 1;
            byte_idx -= 4;
        }
    }
    return 0;
}

