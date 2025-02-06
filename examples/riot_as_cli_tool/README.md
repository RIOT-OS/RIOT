RIOT as a CLI tool
==================

This example demonstrates running RIOT as a one-shot CLI tool.
This can be used to easiely integrate RIOT into a CI pipeline for integration tests
with software that is supposed to interact with RIOT.

This example demonstrates running the nanoCoAP CLI commands from the host system.

For this the whole host file system is exported to RIOT, we also need to create a
virtual network interface by running

    sudo dist/tools/tapsetup/tapsetup -u eth0

where `eth0` is the network interface used for the internet uplink.
If no internet connectivity is needed, this can be left out.

RIOT commands can be invoked e.g. via

    bin/native/nanocoap_tool.elf tap0 ping riot-os.org

or via the provided wrapper scripts

    ./ping.sh riot-os.org
