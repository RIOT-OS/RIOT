LwMAC implementation
====================


# Duty cycling

LwMAC interfaces LPM at the moement by setting `lpm_prevent_sleep` to 0 when
it idles (NETDEV put to NETOPT_STATE_SLEEP). To wake up it schedules an RTT
alarm. Therefore RTT has to be configured to be able to wake up the system.

## RX

Incoming packets will be queued to `lwmac.rx.queue`. When LwMAC is in the
`LISTENING` state and the queue is not empty a receive transaction is started.
This disables duty cycling as long as the transaction is in progress. In case
of success it changes back to `SLEEPING` state and resumes duty cycling. If
the reception has failed (e.g. packet for other node) it goes back to the
`LISTENING` state until a successful transaction or timeout has occured.
