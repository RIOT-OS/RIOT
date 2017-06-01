## OpenThread on RIOT

This test demonstrates how to use NEST's OpenThread stack from [OpenThread](https://github.com/openthread/openthread)
from RIOT. It shows how to use OpenThread UDP capacilities and how to interact with the API.

## Quick usage

To test OpenThread on RIOT, you can do the following:

1. Flash some nodes with `make BOARD=<target> clean all flash`
2. Set the PAN ID in all nodes to the same with ifconfig command (e.g `ifconfig set panid 0x1234`).
3. Start OpenThread with `ifconfig thread start`
3. Check the state of each node with `ifconfig thread state`. After some
   seconds you will see one node being `leader` and some nodes being `router` or `child`.
5. Get the mesh IP address of a node with `ifconfig`.
   `ifconfig`
   `fdde:ad00:beef::ff:fe00:8000`
   `fe80::ff:fe00:8000`
   `fdde:ad00:beef:0:946a:c722:a5d9:8481`
   `fe80::3984:f4eb:d182:5dae`
   
   Addresses starting with `fd` are mesh-local, and addresses starting with `fe80` are link-local.
   Mesh-local address types that contain `ff:fe00` are classified as Router Locator (RLOC). Mesh-local address types
   that don't contain `ff:fe00` are Endpoint Identifies (EID).
6. Start a UDP server on each node with the `udp server` command (e.g `udp server 12345` will open port 12345)
7. Send a udp message with `udp send` command. Pick a mesh-local address (e.g `udp send fdde:ad00:beef:0:946a:c722:a5d9:8481 12345 "This is RIOT!"`)
8. Enjoy!

## Note

See the usage of OpenThread jobs (OT JOB functions). These are required because OpenThread is running in another thread,
and directly calling otApi functions may cause crashes due to concurrency. Please call all OpenThread API functions inside an OT JOB,
and execute with ot_exec_job. This may change in the future.
See the OpenThread documentation for more information about OpenThread API.
