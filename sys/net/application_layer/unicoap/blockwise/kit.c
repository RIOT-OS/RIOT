/*
 * SPDX-FileCopyrightText: 2024-2026 Carl Seifert
 * SPDX-FileCopyrightText: 2024-2026 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @ingroup net_unicoap_blockwise_kit
 * @brief   Block-wise implementation (helper functions and support for automatic block-wise transfers)
 * @author  Carl Seifert <carl.seifert@tu-dresden.de>
 */

#include <stdint.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>

#include "bitarithm.h"

#include "net/unicoap/options.h"
#include "net/unicoap/blockwise.h"

#define ENABLE_DEBUG CONFIG_UNICOAP_DEBUG_LOGGING
#include "debug.h"
#include "private.h"

#define _BLOCKWISE_DEBUG_SIZE(new_szx, pref_szx, suggested_szx)                  \
    _BLOCKWISE_DEBUG("using block size %" PRIuSIZE "%s (we preferred %" PRIuSIZE \
                    "%s, peer suggested %" PRIuSIZE "%s)\n",                    \
                    unicoap_block_size_from_szx(new_szx),                       \
                    new_szx == UNICOAP_BLOCK_SZX_1024_BERT ? " (BERT)" : "",    \
                    unicoap_block_size_from_szx(pref_szx),                      \
                    pref_szx == UNICOAP_BLOCK_SZX_1024_BERT ? " (BERT)" : "",   \
                    unicoap_block_size_from_szx(suggested_szx),                 \
                    suggested_szx == UNICOAP_BLOCK_SZX_1024_BERT ? " (BERT)" : "");

#define _BLOCKWISE_DEBUG_SIZE_EXCEEDS(szx, suggested_szx)                                \
    _BLOCKWISE_DEBUG("block size %" PRIuSIZE "%s exceeds configured: %" PRIuSIZE "%s\n", \
                    unicoap_block_size_from_szx(szx),                                   \
                    szx == UNICOAP_BLOCK_SZX_1024_BERT ? " (BERT)" : "",                \
                    unicoap_block_size_from_szx(suggested_szx),                         \
                    suggested_szx == UNICOAP_BLOCK_SZX_1024_BERT ? " (BERT)" : "");

#define _UNICOAP_BLOCK_SZX_FIXED_FLAG (2 << _UNICOAP_BLOCK_FIXED_BIT_WIDTH)

unicoap_block_szx_t unicoap_block_szx_from_size(size_t size) {
    assert(size >= 16);
    assert((size & (size - 1)) == 0 /* power of two */);
    assert(size <= 1024);

    /* We must wait to subtract the szx offset of 4 until after the assert below.
     * Input should be a power of two, but if not it may have a stray low order
     * '1' bit that would invalidate the subtraction. */
    unsigned int szx = bitarithm_lsb((int)size);
    assert(szx >= 4);
    return szx - 4;
}

void unicoap_block_get_info(unicoap_block_option_t option, unicoap_block_info_t *info) {
    uint8_t szx = unicoap_block_get_szx(option);
    info->size = unicoap_block_size_from_szx(szx);
    info->offset = unicoap_block_get_offset(option);
    info->number = unicoap_block_get_number(option);
    info->bert = szx == UNICOAP_BLOCK_SZX_1024_BERT;
    info->more_available = unicoap_block_get_more(option);
}

// MARK: - Slicing, Collecting, and Reassembling

void unicoap_blockwise_iterator_init(unicoap_blockwise_iterator_t* iterator,
                                     unicoap_block_szx_t max_szx, uint8_t* body,
                                     size_t body_size) {
    iterator->body = body;
    iterator->body_size = body_size;
    iterator->offset = 0;
    iterator->_block_option = unicoap_block_from_szx(
        max_szx, 0, body_size >= unicoap_block_size_from_szx(max_szx));
}

void unicoap_print_blockwise_iterator(const unicoap_blockwise_iterator_t* iterator) {
    printf("<blockwise_iterator "
           "block_size=%" PRIuSIZE " "
           "block_number=%" PRIu32 " offset=%" PRIuSIZE " "
           "body=<buffer at %p size=%" PRIuSIZE ">>",
           unicoap_block_get_size(iterator->_block_option),
           unicoap_block_get_number(iterator->_block_option), iterator->offset,
           iterator->body, iterator->body_size);
}

static inline int _check_same_szx(unicoap_blockwise_iterator_t* iterator, unicoap_block_szx_t szx) {
    if (unicoap_block_get_szx(iterator->_block_option) != szx) {
        _BLOCKWISE_DEBUG("illegal attempt to change block szx from %x to %x\n",
                        unicoap_block_get_szx(iterator->_block_option), szx);
        return -EBADMSG;
    }

    return 0;
}

static int _check_valid_chunk_size(unicoap_block_option_t block, size_t chunk_size) {
    if (unicoap_block_get_szx(block) == UNICOAP_BLOCK_SZX_1024_BERT) {
        if (unicoap_block_get_more(block) && chunk_size % UNICOAP_BERT_BLOCK_SIZE != 0) {
            _BLOCKWISE_DEBUG("chunk size %" PRIuSIZE " is not multiple of BERT block size\n",
                            chunk_size);
            return -EBADMSG;
        }
    }
    else {
        size_t block_size = unicoap_block_get_size(block);
        if ((unicoap_block_get_more(block) && chunk_size != block_size) ||
            chunk_size > block_size) {
            _BLOCKWISE_DEBUG("chunk size %" PRIuSIZE " does not match block size: %" PRIuSIZE "\n",
                            chunk_size, block_size);
            return -EBADMSG;
        }
    }
    return 0;
}

static inline int _collect(unicoap_blockwise_iterator_t* collector, uint8_t* chunk,
                           size_t chunk_size, bool more) {
    if (chunk_size > 0 && !chunk) {
        _BLOCKWISE_DEBUG("chunk missing\n");
        return -EBADMSG;
    }
    if (collector->body_size > 0) {
        /* reassemble if we're supposed to */
        assert(collector->body);
        size_t total_size = collector->offset + chunk_size;

        if (!more && collector->offset == 0) {
            /* first and only message, avoid copying payload
             * set buffer size to actual body size */
            collector->body = chunk;
        }
        else if (chunk) {
            if (total_size > collector->body_size) {
                _BLOCKWISE_DEBUG("no space to reassemble " _UNICOAP_NEED_HAVE "\n",
                                total_size, collector->body_size);
                return -EMSGSIZE;
            }
            memcpy(collector->body + collector->offset, chunk, chunk_size);
        }

        if (!more) {
            /* last message,
             * set buffer size to actual body size */
            collector->body_size = total_size;
        }
    }

    collector->offset += chunk_size;
    return 0;
}

static size_t _slice(unicoap_blockwise_iterator_t* slicer, uint8_t** chunk) {
    assert(slicer);
    assert(slicer->body);

    *chunk = slicer->body + slicer->offset;
    size_t remaining = slicer->body_size - slicer->offset;

    size_t max_chunk_size = unicoap_block_get_size(slicer->_block_option);

    if (unicoap_block_get_szx(slicer->_block_option) == UNICOAP_BLOCK_SZX_1024_BERT) {
        _BLOCKWISE_DEBUG("BERT: slicing off %u blocks\n",
                        CONFIG_UNICOAP_BLOCKWISE_BERT_BLOCKS_PER_MESSAGE_MAX);
        max_chunk_size *= CONFIG_UNICOAP_BLOCKWISE_BERT_BLOCKS_PER_MESSAGE_MAX;
    }

    /* last block may be smaller than a fully filled block */
    size_t chunk_size = MIN(remaining, max_chunk_size);
    unicoap_block_set_more(&slicer->_block_option, remaining > max_chunk_size);

    _BLOCKWISE_DEBUG("slicing off block <#%" PRIu32 ", total=%" PRIuSIZE "B, %s>\n",
                    unicoap_block_get_number(slicer->_block_option), chunk_size,
                    remaining > max_chunk_size ? "M" : "last");

    slicer->offset += chunk_size;
    return chunk_size;
}

int unicoap_blockwise_collect_block2(unicoap_blockwise_iterator_t* collector,
                                     unicoap_block_option_t block, uint8_t* chunk,
                                     size_t chunk_size) {
    assert(collector);

    if (block == UNICOAP_BLOCK_OPTION_NONE) {
        if (chunk) {
            /* got a response, Block2 option absent */
            if (collector->offset == 0) {
                /* first and only response, no Block2 option, so assume body
                 * not transmitted blockw-wise */
                _BLOCKWISE_DEBUG("single response (not transmitted block-wise)\n");
                _collect(collector, chunk, chunk_size, false);
                unicoap_block_set_more(&collector->_block_option, false);
                /* We're done, do not request more. */
                return 0;
            }
            else {
                /* There were responses before carrying the Block2 option.
                 * Given the client (us) requesting the exact Block we want,
                 * the server forgot to add the Block2 option. */
                _BLOCKWISE_DEBUG("missing block option\n");
                return -EBADMSG;
            }
        }
        else {
            /* As expected, the first time we get called, there's no Block2
             * option and no response payload */
            assert(collector->offset == 0);
            unicoap_block_set_more(&collector->_block_option, false);
            /* return 1 to indicate a Block2 request shall be sent */
            return 1;
        }
    }
    else {
        /* got a Block2 response */
        if (_check_valid_chunk_size(collector->_block_option, chunk_size) < 0) {
            return -EBADMSG;
        }

        if (unicoap_block_get_number(collector->_block_option) != unicoap_block_get_number(block)) {
            _BLOCKWISE_DEBUG("expected block #%" PRIu32 ", got #%" PRIu32 "\n",
                            unicoap_block_get_number(collector->_block_option),
                            unicoap_block_get_number(block));
            return -ENOMSG;
        }

        unicoap_block_szx_t szx = unicoap_block_get_szx(block);
        if (collector->offset == 0) {
            /* first Block2 response */
            if (szx > unicoap_block_get_szx(collector->_block_option)) {
                /* From RFC 7959 (Block-wise), section 2.4:
                 * A server MUST use the block size indicated or a smaller size. */
                _BLOCKWISE_DEBUG_SIZE_EXCEEDS(szx, unicoap_block_get_szx(collector->_block_option));
                return -EBADMSG;
            }
            _BLOCKWISE_DEBUG_SIZE(szx, unicoap_block_get_szx(collector->_block_option), szx);
            unicoap_block_set_szx(&collector->_block_option, szx); /* (SZX) */
        }
        else {
            /* block size must not change after initial request */
            if (_check_same_szx(collector, szx) < 0) {
                return -EBADMSG;
            }
        }

        /* indicate whether another Block2 request shall be sent */
        bool more = unicoap_block_get_more(block);

        /* no need to check if we've already collected this block,
         * function must not be called twice for same response */
        _BLOCKWISE_DEBUG("collecting block <#%" PRIu32 ", total=%" PRIuSIZE "B, %s>\n",
                        unicoap_block_get_number(block), chunk_size,
                        unicoap_block_get_more(block) ? "M" : "last");
        int res = _collect(collector, chunk, chunk_size, more);
        if (res < 0) {
            return res;
        }

        /* (NUM) */
        unicoap_block_set_number_from_offset(&collector->_block_option, collector->offset);

        /* RFC 7959 (Block-wise), section 2.3:
         * In this case, the M bit has no function and MUST be set to zero. */
        unicoap_block_set_more(&collector->_block_option, false); /* (M) */
        return more;

        /* if more == true, send Block2 request:
         * number: incremented by 1 (NUM)
         * SZX:    adjusted above (SZX)
         * M:      false (M) */
    }
}

int unicoap_blockwise_slice_block2_process(unicoap_blockwise_iterator_t* slicer,
                                           unicoap_block_option_t block) {
    assert(slicer);

    if (block == UNICOAP_BLOCK_OPTION_NONE) {
        /* client (re-)starts Block-wise transfer, first request not required
         * to carry Block2 option; reset state */
        slicer->offset = 0;
    }
    else {
        /* Block2 option is present */
        if (unicoap_block_get_more(block)) {
            /* intermediary block, more coming */
            _BLOCKWISE_DEBUG("error: M flag set in control block\n");
            return -EBADMSG;
        }

        if (unicoap_block_get_offset(block) > slicer->body_size) {
            _BLOCKWISE_DEBUG("error: requested block number out of bounds\n");
            return -EBADMSG;
        }

        unicoap_block_szx_t szx = unicoap_block_get_szx(block);
        if ((slicer->_block_option & _UNICOAP_BLOCK_SZX_FIXED_FLAG) == 0) {
            /* first Block2 request with desired block size */
            /* RFC 7959 (Block-wise), section 2.4: 
             * Any further block-wise requests for blocks beyond the first one
             * MUST indicate the same block size that was used by the server in
             * the response for the first request that gave a desired size using
             * a Block2 Option. [...] The server behavior MUST ensure that this
             * client behavior results in the same block size for all responses
             * in a sequence (except for the last one with the M bit not set,
             * and possibly the first one if the initial request did not contain
             * a Block2 Option). */
            /* This means we have to track whether we've seen such a first
             * request with a Block2 option, and we do so using a hidden flag
             * in the block option. Note that this is fine as we documented*/
            unicoap_block_szx_t old_szx = unicoap_block_get_szx(slicer->_block_option);
            unicoap_block_szx_t new_szx = MIN(old_szx, szx);
            _BLOCKWISE_DEBUG_SIZE(new_szx, old_szx, szx);
            unicoap_block_set_szx(&block, new_szx); /* (SZX) */
            block |= _UNICOAP_BLOCK_SZX_FIXED_FLAG; /* prevent further changes */
        }
        else {
            /* intermediary block */
            /* block size must not change after initial request */
            if (_check_same_szx(slicer, szx)) {
                return -EBADMSG;
            }
        }

        slicer->_block_option = block; /* (NUM) */
        slicer->offset = unicoap_block_get_offset(block);
    }
    return 0;
}

ssize_t unicoap_blockwise_slice_block2_produce(unicoap_blockwise_iterator_t* slicer,
                                               uint8_t** chunk) {
    return _slice(slicer, chunk); /* (M) */

    /* Send Block2 response:
     * number: same as request (NUM)
     * SZX:    adjusted above (SZX)
     * M:      set by _slice (M) */
}

ssize_t unicoap_blockwise_slice_block2(unicoap_blockwise_iterator_t* slicer,
                                       unicoap_block_option_t block, uint8_t** chunk) {
    assert(slicer);

    int res = unicoap_blockwise_slice_block2_process(slicer, block);
    if (res < 0) {
        return res;
    }

    if (chunk) {
        return _slice(slicer, chunk); /* (M) */

        /* Send Block2 response:
         * number: same as request (NUM)
         * SZX:    adjusted above (SZX)
         * M:      set by _slice (M) */
    }
    else {
        return 0;
    }
}

ssize_t unicoap_blockwise_slice_block1(unicoap_blockwise_iterator_t* slicer,
                                       unicoap_block_option_t block, uint8_t** chunk) {
    assert(chunk);
    if (block == UNICOAP_BLOCK_OPTION_NONE) {
        if (slicer->offset > 0) {
            /* This is not the first time this API is called, hence
             * a Block1 control option must be present in the response! */
            _BLOCKWISE_DEBUG("warning: no Block1 control option\n");
            return (int)_slice(slicer, chunk);
        }
        else {
            /* As expected, the first time we get called, there's no Block2
             * option and no response payload */
        }
    }
    else {
        /* Block1 control option present in a Block1 response.
         * We must have sent a Block1 response before, hence offset not be 0 */
        assert(slicer->offset > 0);
        _BLOCKWISE_DEBUG("got control block\n");

        if (unicoap_block_get_number(slicer->_block_option) != unicoap_block_get_number(block)) {
            _BLOCKWISE_DEBUG("expected block #%" PRIu32 ", got #%" PRIu32 "\n",
                            unicoap_block_get_number(slicer->_block_option),
                            unicoap_block_get_number(block));
            return -EBADMSG;
        }

        if (unicoap_block_get_more(slicer->_block_option) == false &&
            unicoap_block_get_more(block)) {
            _BLOCKWISE_DEBUG("error: M flag set for last control block\n");
            return -EBADMSG;
        }

        unicoap_block_szx_t szx = unicoap_block_get_szx(block);
        if (unicoap_block_get_number(slicer->_block_option) == 0) {
            /* first Block1 response */
            unicoap_block_szx_t old_szx = unicoap_block_get_szx(slicer->_block_option);
            unicoap_block_szx_t new_szx = MIN(old_szx, szx);
            _BLOCKWISE_DEBUG_SIZE(new_szx, old_szx, szx);
            unicoap_block_set_szx(&slicer->_block_option, new_szx); /* (SZX) */
        }
        else {
            /* block size must not change after initial request */
            if (_check_same_szx(slicer, szx)) {
                return -EBADMSG;
            }
        }
    }

    /* (NUM) */
    unicoap_block_set_number_from_offset(&slicer->_block_option, slicer->offset);

    return (int)_slice(slicer, chunk); /* (M) */

    /* If return value > 0, send Block1 request:
     * number: set according to current offset and current block size (NUM)
     * SZX:    adjusted above (SZX)
     * M:      set by _slice (M) */
}

int unicoap_blockwise_collect_block1(unicoap_blockwise_iterator_t* collector,
                                     unicoap_block_option_t block, uint8_t* chunk,
                                     size_t chunk_size) {
    assert(collector);

    if (!chunk || block == UNICOAP_BLOCK_OPTION_NONE) {
        /* Block1 option is absent */
        if (collector->offset == 0) {
            /* body is not transmitted block-wise */
            _BLOCKWISE_DEBUG("not transmitted block-wise\n");
            _collect(collector, chunk, chunk_size, false);
            collector->_block_option = UNICOAP_BLOCK_OPTION_NONE;
            return 0;
        }
        else {
            /* block option is missing */
            _BLOCKWISE_DEBUG("missing block option\n");
            return -EBADMSG;
        }
    }
    else {
        /* got a Block1 request */
        if (_check_valid_chunk_size(block, chunk_size) < 0) {
            return -EBADMSG;
        }

        if (unicoap_block_get_number(block) == 0) {
            /* first request */
            _BLOCKWISE_DEBUG("transfer started\n");
            /* do not re-set block size to our suggestion, may be cause for restart */
            collector->offset = 0;
        }

        if (collector->offset != unicoap_block_get_offset(block)) {
            _BLOCKWISE_DEBUG("expected block #%" PRIu32 ", got #%" PRIu32 "\n",
                            unicoap_block_get_number(collector->_block_option),
                            unicoap_block_get_number(block));
            return -ENOMSG;
        }

        unicoap_block_szx_t szx = unicoap_block_get_szx(block);
        if (collector->offset == 0) {
        }
        else if (unicoap_block_get_number(collector->_block_option) == 0) {
            /* second Block1 request */
            if (szx > unicoap_block_get_szx(collector->_block_option)) {
                /* RFC 7959 (Block-wise), section 2.5:
                 * Still, the client SHOULD heed the preference indicated and, for all further
                 * blocks, use the block size preferred by the server or a smaller one.
                 * Note that any reduction in the block size may mean that the second
                 * request starts with a block number larger than one, as the first
                 * request already transferred multiple blocks as counted in the smaller
                 * size. */
                _BLOCKWISE_DEBUG_SIZE_EXCEEDS(szx, unicoap_block_get_szx(collector->_block_option));
                return -EBADMSG;
            }
            _BLOCKWISE_DEBUG_SIZE(szx, unicoap_block_get_szx(collector->_block_option), szx);
            unicoap_block_set_szx(&collector->_block_option, szx); /* (SZX) */
        }
        else {
            /* block size must not change after initial request */
            if (_check_same_szx(collector, szx) < 0) {
                return -EBADMSG;
            }
        }
    }

    /* indicate whether another Block2 request shall be sent */
    bool more = (int)unicoap_block_get_more(block);

    /* need to check if we've already collected this block,
     * as this API is supposed to be safe to call multiple times for the same
     * request (retransmissions) */
    if (collector->offset == unicoap_block_get_offset(block)) {
        _BLOCKWISE_DEBUG("collecting block <#%" PRIu32 ", total=%" PRIuSIZE "B, %s>\n",
                        unicoap_block_get_number(block), chunk_size,
                        unicoap_block_get_more(block) ? "M" : "last");
        int res = _collect(collector, chunk, chunk_size, more);
        if (res < 0) {
            return res;
        }
    }

    unicoap_block_set_szx(&block, unicoap_block_get_szx(collector->_block_option)); /* (SZX) */
    collector->_block_option = block;                                               /* (NUM), (M) */
    return more;

    /* if more == true, send Block2 request:
     * number: same as request (NUM)
     * SZX:    use configured (SZX)
     * M:      same as request (M) */
}

static inline unicoap_block_szx_t _configured_szx(unicoap_proto_t proto) {
    if (IS_ACTIVE(CONFIG_UNICOAP_BERT)) {
        if (unicoap_transport_is_reliable(proto) && CONFIG_UNICOAP_BLOCK_SZX == UNICOAP_BLOCK_SZX_1024) {
            return UNICOAP_BLOCK_SZX_1024_BERT;
        }
    }

    return (unicoap_block_szx_t)CONFIG_UNICOAP_BLOCK_SZX;
}

int unicoap_blockwise_transfer_setup(unicoap_message_t* outbound_message,
                                     unicoap_blockwise_transfer_t* transfer,
                                     unicoap_blockwise_flags_t blockwise_flags,
                                     unicoap_proto_t proto, bool init) {
    ssize_t res = 0;
    uint8_t* buffer = NULL;
    size_t size = 0;

    _BLOCKWISE_DEBUG("setting up transfer\n");

    if (outbound_message) {
        transfer->code = outbound_message->code;
        if (outbound_message->options) {
            _BLOCKWISE_DEBUG("copying options\n");
            transfer->options = *outbound_message->options;
            if ((res = unicoap_options_swap_storage(&transfer->options, transfer->options_storage,
                                                    sizeof(transfer->options_storage))) < 0) {
                return res;
            }
        }
        else {
            /* don't copy, but use full available storage */
            transfer->options.storage_size = 0;
            transfer->options.storage_capacity = sizeof(transfer->options_storage);
            transfer->options.option_count = 0;
            transfer->options.entries->data = transfer->options_storage;
        }
        /* set new outbound message options structure below, so in case something goes wrong below, we don't
         * corrupt the message */
    }

    switch (transfer->stage) {
    case UNICOAP_BLOCKWISE_STAGE_SLICE:
        _BLOCKWISE_DEBUG("preparing transfer for SLICE stage: no_copy=%u\n",
                        (blockwise_flags & UNICOAP_BLOCKWISE_FLAG_DURABLE_MESSAGE) != 0);

        /* FIXME: what if there's no outbound message (see server.c L395) */
        if (outbound_message) {
            if (blockwise_flags & 
                (UNICOAP_BLOCKWISE_FLAG_DURABLE_MESSAGE | UNICOAP_BLOCKWISE_FLAG_SLICE)) {
                buffer = outbound_message->payload;
                unicoap_blockwise_buffer_free(transfer);
            }
            else {
                _BLOCKWISE_DEBUG("copying payload into block-wise buffer\n");
                if (outbound_message->payload_size > CONFIG_UNICOAP_BLOCKWISE_BODY_CAPACITY) {
                    _BLOCKWISE_DEBUG("no buffer space to copy body, " _UNICOAP_NEED_HAVE "\n", outbound_message->payload_size, (size_t)CONFIG_UNICOAP_BLOCKWISE_BODY_CAPACITY);
                    return -ENOBUFS;
                }
                memcpy(transfer->buffer, outbound_message->payload, outbound_message->payload_size);
                buffer = transfer->buffer;
            }
            size = outbound_message->payload_size;
        }

        break;
    case UNICOAP_BLOCKWISE_STAGE_COLLECT:
        _BLOCKWISE_DEBUG("preparing transfer for COLLECT stage, reassemble=%u\n",
                        (blockwise_flags & UNICOAP_BLOCKWISE_FLAG_REASSEMBLE) != 0);

        if (blockwise_flags & UNICOAP_BLOCKWISE_FLAG_REASSEMBLE) {
            buffer = transfer->buffer;
            size = CONFIG_UNICOAP_BLOCKWISE_BODY_CAPACITY;
        }
        break;

    default:
        assert(false);
        return -1;
    }

    if (init) {
        unicoap_blockwise_iterator_init(&transfer->iterator, _configured_szx(proto), buffer, size);
        transfer->options.storage_capacity = sizeof(transfer->options_storage);
        transfer->options.entries->data = transfer->options_storage;
    }
    else {
        transfer->iterator.body = buffer;
        transfer->iterator.body_size = size;
    }

    if (outbound_message) {
        outbound_message->options = &transfer->options;
    }
    return 0;
}

int unicoap_blockwise_transfer_collector_validate(unicoap_message_t* message,
                                                  unicoap_blockwise_transfer_t* transfer) {
    const uint8_t* etag = NULL;
    int res = unicoap_options_get_first_etag(message->options, &etag);
    if (res > 8) {
        _CLIENT_DEBUG("error: invalid ETag length\n");
        return -EBADMSG;
    }

    if (transfer->etag_length == 0 && res > 0) {
        memcpy(transfer->etag, etag, res);
        transfer->etag_length = res;
    }
    else if (res >= 0 && transfer->etag_length > 0 &&
             (transfer->etag_length != res || !memcmp(etag, transfer->etag, res))) {
        _CLIENT_DEBUG("error: ETag changed between blocks\n");
        return -EBADMSG;
    }

    /* recognize Content-Format changes in a Block-wise transfer */
    uint16_t format = UINT16_MAX;
    unicoap_options_get_content_format(message->options, &format);

    if (transfer->content_format == UINT16_MAX) {
        transfer->content_format = format;
    }

    if (transfer->content_format != UINT16_MAX && format != UINT16_MAX &&
        format != transfer->content_format) {
        _CLIENT_DEBUG("error: Content-Format changed between blocks: expected %u, got %u\n",
                     transfer->content_format, format);
        return -EBADMSG;
    }
    return 0;
}
