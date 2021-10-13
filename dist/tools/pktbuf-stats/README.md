`pktbuf` parser
===============

This parses the output of the command `pktbuf` provided by the module
`gnrc_pktbuf_cmd` for the `gnrc_pktbuf_static` implementation.

The command expects the ELF file of the binary the `pktbuf` command was executed
in to get some binary information on structs potentially stored in the packet
buffer.

A packet buffer dump can also be provided as a file. If not provided, it is read
from STDIN.

```sh
./pktbuf-stats.py <ELF file> [<pktbuf-dump>]
```

Requires GDB to be installed
