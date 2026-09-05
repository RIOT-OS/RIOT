@defgroup net_unicoap_client_cancellation Request cancellation
@ingroup  net_unicoap_client
@brief    Cancel pending requests
@{

Module. Specify `USEMODULE += unicoap_client_cancellation` in your application's Makefile.

When you call @ref unicoap_send_request_async, its return value is a token you use to refer
to this request ('refno'). Pass the refno you obtain this way to @ref unicoap_cancel_request
to cancel the request. It is safe to pass a refno whose corresponding request has already
failed or finished successfully.

@}
