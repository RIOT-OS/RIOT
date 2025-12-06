#!/usr/bin/python3
"""
Bridge that translates CoAP over YOLO to CoAP over WebSocket.
"""

import aiohttp
import aiohttp.web
import argparse
import asyncio
import sys

udp_ep = None
udp_transport = None
ws = None

class ForwardFromUdpProtocol:
    """
    Forward received UDP datagrams via the currently connected WebSocket
    """
    def connection_made(self, transport):
        pass

    def datagram_received(self, data, addr):
        global ws
        if ws is not None:
            asyncio.ensure_future(ws.send_bytes(data), loop=asyncio.get_event_loop())


async def websocket_handler(request):
    """
    Forward received WebSocket messages to the (statically) configured UDP
    destination endpoint
    """
    global udp_transport
    global udp_ep
    global ws
    if ws is not None:
        print("Someone already is connected")
        return
    ws = aiohttp.web.WebSocketResponse(protocols=("coap"))
    print("WebSocket connection opened")
    await ws.prepare(request)

    async for msg in ws:
        if msg.type == aiohttp.WSMsgType.BINARY:
            udp_transport.sendto(msg.data, udp_ep)
        elif msg.type == aiohttp.WSMsgType.CLOSED:
            udp_transport.sendto(b'', udp_ep)
            ws = None
            return
        else:
            print(f"Warning: Got unexpected WebSocket Message {msg}")

    udp_transport.sendto(b'', udp_ep)
    ws = None
    print("WebSocket connection closed")


async def ws2yolo(_udp_ep, ws_ep, udp_local_ep):
    """
    Run a WebSocket 2 CoAP over YOLO bridge with the given endpoints
    """
    global udp_transport
    global udp_ep
    udp_ep = _udp_ep
    loop = asyncio.get_running_loop()
    udp_transport, protocol = await loop.create_datagram_endpoint(
            ForwardFromUdpProtocol,
            local_addr=udp_local_ep)

    app = aiohttp.web.Application()
    app.router.add_route('GET', '/.well-known/coap', websocket_handler)
    runner = aiohttp.web.AppRunner(app)
    await runner.setup()
    site = aiohttp.web.TCPSite(runner)
    await site.start()
    await asyncio.Event().wait()


if __name__ == "__main__":
    DESCRIPTION = "Forward WebSocket messages via UDP"
    parser = argparse.ArgumentParser(description=DESCRIPTION)
    parser.add_argument("--udp-host", default="::1", type=str,
                        help="UDP host to forward to")
    parser.add_argument("--udp-port", default=1337, type=int,
                        help="UDP port to forward to")
    parser.add_argument("--local-host", default=None, type=str,
                        help="UDP host to forward from")
    parser.add_argument("--local-port", default=0, type=int,
                        help="UDP port to forward from")
    parser.add_argument("--ws-host", default="::1", type=str,
                        help="WebSocket host to listen at")
    parser.add_argument("--ws-port", default=8080, type=int,
                        help="WebSocket port to listen at")

    args = parser.parse_args()
    asyncio.run(ws2yolo((args.udp_host, args.udp_port),
                        (args.ws_host, args.ws_port),
                        (args.local_host, args.local_port)))
