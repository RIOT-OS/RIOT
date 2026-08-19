@defgroup net_unicoap_blockwise Block-Wise Transfers
@ingroup net_unicoap
@brief Transfer request and response payloads in blocks. 
@{

[RFC 7959](https://datatracker.ietf.org/doc/html/rfc7959) specifies _block-wise transfer_, 
a CoAP extension that enables exchanging large payloads in application layer _blocks_.

Block-wise transfers are facilitated through a set of CoAP options, through which larger request 
or response _bodies_ can be exchanged across a series of request-response pairs. 
These 'elementary' requests and responses transfer a single payload block each, leveraging regular 
message semantics. In block-wise transfers, the block size can be negotiated. 
The term _'body'_ refers to the content an endpoint wants to exchange with another, 
i.e., either as a request (client) or response (server). 
The term _payload_ is used for the data inside a singular CoAP message, and is a block of the
original _body_.

A `unicoap` node will engage in one the following activities at a time:

- *Slicing* is the process of dividing a larger body into smaller chunks and sending them to the peer.
- *Collecting* involves requesting more blocks from the peer and tracking progress, acting on each block individually.
    - *Reassembling* denotes the process of requesting more blocks, tracking progress, and copying blocks to a larger buffer, thereby recreating and processing just the original, contiguous body.

Both slicing and collecting involve two types of transfers:
- *Block1* transfer encompasses a series of requests sent by the client, each carrying a block of
the larger request payload body. Each request is followed by a response from the server
asking the client to send the next block.
- *Block2* transfer also spans multiple requests and responses. Each response contains a block
of the larger response body. The client sends a new request for each block.

`unicoap` implements two levels of support for transferring payloads in blocks.

To let `unicoap` automatically handle slicing and reassembling, you use the 
@ref net_unicoap_blockwise_automatic module.

If you want to manage a block-wise transfer manually, you do not need to import the
@ref net_unicoap_blockwise_automatic module. First, allocate a @ref unicoap_blockwise_iterator_t and
initialize the iterator using @ref unicoap_blockwise_iterator_init. Then, depending on whether
you are developing a client or server application, you use
@ref unicoap_blockwise_slice_block1 to slice a request body into blocks (client) or
@ref unicoap_blockwise_collect_block1 to collect request blocks (server).
You use
@ref unicoap_blockwise_slice_block2, or 
@ref unicoap_blockwise_slice_block2_process and @ref unicoap_blockwise_slice_block2_produce
to slice a block off of a response body (server). To collect response blocks, use
@ref unicoap_blockwise_collect_block2 (client).

@note The @ref net_unicoap_blockwise_automatic module is implemented using the same helper functions.
@}

@defgroup net_unicoap_blockwise_automatic Automatic Block-Wise Transfers
@ingroup net_unicoap_blockwise
@brief Let `unicoap` automatically handle block-wise transfers
@{

Module. Specify `USEMODULE += unicoap_blockwise` in your application's Makefile.

### Using Automatic Block-Wise Client Features
This module allows you to make use this feature through API flags.
If you intend to send a request to server, with a large payload, pass the
@ref UNICOAP_CLIENT_FLAG_SLICE flag to any of the `unicoap_send_request_*` methods.
If you can guarantee that the payload (@ref unicoap_message_t.payload) buffer stays alive for the
duration of the transfer, pass @ref UNICOAP_CLIENT_FLAG_SLICE_NO_COPY .

If you expect the server to send a large response, potentially spanning across multiple blocks,
specify the @ref UNICOAP_CLIENT_FLAG_REASSEMBLE flag. The stack will automatically
ask for more blocks, copying them into a larger buffer.

If the copying overhead is unacceptable for you or you want to implement streaming functionality,
use @ref unicoap_send_request_blockwise_async or @ref unicoap_send_request_blockwise_sync.
These methods will call the callback you provide for each response block that arrives.

### Using Automatic Block-Wise Server Features
For receiving a large request, specify @ref UNICOAP_RESOURCE_FLAG_REASSEMBLE when
creating your @ref unicoap_resource_t. Akin to the client, if you want to send large response,
specify @ref UNICOAP_RESOURCE_FLAG_SLICE. Provided you can guarantee, the payload pointer
stays alive even after the resource handler returns, pass @ref UNICOAP_RESOURCE_FLAG_SLICE.

### Overview of Limits and Optimization Opportunities
Each block will have maximum size of @ref CONFIG_UNICOAP_BLOCK_SIZE, which you can set
at compile-time.

@note BERT
If you use CoAP over TCP or another reliable transport protocol, consider enabling
@ref CONFIG_UNICOAP_BERT -- the Block-wise extension for reliable transports. BERT allows
the stack to send multiple blocks of size 1024 inside a single CoAP message. The BERT mechanism
will be automatically triggered if the endpoint you are communicating with is reliable.
@ref CONFIG_UNICOAP_BLOCKWISE_BERT_BLOCKS_PER_MESSAGE_MAX indicates the maximum
number of blocks that can be present in a single BERT message.

@remark Tip for developers wanting to extend unicoap: @ref unicoap_transport_is_reliable
determines whether BERT can be enabled for a given protocol.

The stack restricts the number of automatic Block-wise transfers that can be active at a time to
@ref CONFIG_UNICOAP_BLOCKWISE_TRANSFERS_CAPACITY (client and server).
@ref CONFIG_UNICOAP_BLOCKWISE_BUFFERS_POOL_CAPACITY dictates the number of large payload
body buffers reserved for the application by the stack. A buffer is used
- if either of the auto-reassemble flags is used, or
- if either of the auto-slice flags is used without specifying the no-copy slice flag.

Hence, if you don't rely on auto-reassembly and can guarantee your payload buffer pointer stays
valid for the duration of the transfer, you can set the Block-wise buffer count to zero.

To configure the maximum size of payload body that can be reassembled or sliced,
set @ref CONFIG_UNICOAP_BLOCKWISE_BODY_CAPACITY.

## See Also

@see @ref net\_unicoap\_config
@see @ref net\_unicoap\_client
@see @ref UNICOAP_CLIENT_FLAG_SLICE
@see @ref UNICOAP_CLIENT_FLAG_SLICE_NO_COPY
@see @ref UNICOAP_CLIENT_FLAG_REASSEMBLE
@see @ref unicoap_send_request_blockwise_async
@see @ref unicoap_send_request_blockwise_sync
@see @ref net\_unicoap\_server
@see @ref UNICOAP_RESOURCE_FLAG_SLICE
@see @ref UNICOAP_RESOURCE_FLAG_SLICE_NO_COPY
@see @ref UNICOAP_RESOURCE_FLAG_REASSEMBLE

@}
