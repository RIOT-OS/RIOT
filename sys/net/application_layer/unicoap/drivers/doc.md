@defgroup net_unicoap_drivers Drivers
@ingroup net_unicoap
@brief Collection of transport drivers
@{

Drivers enable you to use different transports while still relying on the same high-level CoAP
API. Each driver implements framing and a messaging model optimized for the given transport.
Which transport you choose depends on the deployment. For most constrained deployments,
we recommend the @ref net_unicoap_drivers_udp or @ref net_unicoap_drivers_dtls.

@}
