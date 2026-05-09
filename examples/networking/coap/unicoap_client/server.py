#!/usr/bin/env python3

# SPDX-FileCopyrightText: Christian Amsüss and the aiocoap contributors
#
# SPDX-License-Identifier: MIT

"""This is a usage example of aiocoap that demonstrates how to implement a
simple server. See the "Usage Examples" section in the aiocoap documentation
for some more information."""

import datetime
import logging

import asyncio

import aiocoap.resource as resource
from aiocoap.numbers.contentformat import ContentFormat
import aiocoap
import sys


class Welcome(resource.Resource):
    bodys = {
        ContentFormat.TEXT: b"Welcome to the demo server",
        ContentFormat.LINKFORMAT: b"</.well-known/core>,ct=40",
        # ad-hoc for application/xhtml+xml;charset=utf-8
        ContentFormat(65000): b'<html xmlns="http://www.w3.org/1999/xhtml">'
        b"<head><title>aiocoap demo</title></head>"
        b"<body><h1>Welcome to the aiocoap demo server!</h1>"
        b'<ul><li><a href="time">Current time</a></li>'
        b'<li><a href="whoami">Report my network address</a></li>'
        b"</ul></body></html>",
    }

    default_body = ContentFormat.TEXT

    async def render_get(self, request):
        cf = (
            self.default_body
            if request.opt.accept is None
            else request.opt.accept
        )
        try:
            return aiocoap.Message(payload=self.bodys[cf], content_format=cf)
        except KeyError:
            raise aiocoap.error.UnsupportedContentFormat


class BlockResource(resource.Resource):
    """Example resource which supports the GET and PUT methods. It sends large
    responses, which trigger blockwise transfer."""

    def __init__(self):
        super().__init__()
        self.set_content(
            b"This is the resource's default content. It is padded "
            b"with numbers to be large enough to trigger blockwise "
            b"transfer.\n"
        )

    def set_content(self, content):
        self.content = content
        while len(self.content) <= 1024:
            self.content = self.content + b"0123456789\n"

    async def render_get(self, request):
        return aiocoap.Message(payload=self.content)

    async def render_put(self, request):
        print("PUT payload: %s" % request.payload)
        self.set_content(request.payload)
        return aiocoap.Message(code=aiocoap.CHANGED, payload=self.content)


class SeparateLargeResource(resource.Resource):
    """Example resource which supports the GET method. It uses asyncio.sleep to
    simulate a long-running operation, and thus forces the protocol to send
    empty ACK first."""

    def get_link_description(self):
        # Publish additional data in .well-known/core
        return dict(**super().get_link_description(), title="A large resource")

    async def render_get(self, request):
        await asyncio.sleep(3)

        payload = (
            "Three rings for the elven kings under the sky, seven rings "
            "for dwarven lords in their halls of stone, nine rings for "
            "mortal men doomed to die, one ring for the dark lord on his "
            "dark throne.".encode("ascii")
        )
        return aiocoap.Message(payload=payload)


class TimeResource(resource.ObservableResource):
    """Example resource that can be observed. The `notify` method keeps
    scheduling itself, and calles `update_state` to trigger sending
    notifications."""

    def __init__(self):
        super().__init__()

        self.handle = None

    def notify(self):
        self.updated_state()
        self.reschedule()

    def reschedule(self):
        self.handle = asyncio.get_event_loop().call_later(5, self.notify)

    def update_observation_count(self, count):
        if count and self.handle is None:
            print("Starting the clock")
            self.reschedule()
        if count == 0 and self.handle:
            print("Stopping the clock")
            self.handle.cancel()
            self.handle = None

    async def render_get(self, request):
        payload = datetime.datetime.now().strftime("%Y-%m-%d %H:%M").encode("ascii")
        return aiocoap.Message(payload=payload)


class WhoAmI(resource.Resource):
    async def render_get(self, request):
        text = ["Used protocol: %s." % request.remote.scheme]

        text.append("Request came from %s." % request.remote.hostinfo)
        text.append("The server address used %s." % request.remote.hostinfo_local)

        claims = list(request.remote.authenticated_claims)
        if claims:
            text.append(
                "Authenticated claims of the client: %s."
                % ", ".join(repr(c) for c in claims)
            )
        else:
            text.append("No claims authenticated.")

        return aiocoap.Message(content_format=0, payload="\n".join(text).encode("utf8"))


# logging setup

logging.basicConfig(level=logging.DEBUG)
logging.getLogger("coap-server").setLevel(logging.DEBUG)


async def main():
    # Resource tree creation
    root = resource.Site()

    root.add_resource(
        [".well-known", "core"], resource.WKCResource(root.get_resources_as_linkheader)
    )
    root.add_resource([], Welcome())
    root.add_resource(["time"], TimeResource())
    root.add_resource(["other", "block"], BlockResource())
    root.add_resource(["other", "separate"], SeparateLargeResource())
    root.add_resource(["whoami"], WhoAmI())

    port = 5683
    if len(sys.argv) >= 2:
        port = int(sys.argv[1])

    print(f"using {port=}")
    protocol = await aiocoap.Context.create_server_context(bind=("::", port), site=root)
    protocol.server_credentials.load_from_dict(
        {'coaps://*': {'dtls': {
            'psk': b'secretPSK',
            'client-identity': b'Client_Identity',
        }}})

    # Run forever
    await asyncio.get_running_loop().create_future()

if __name__ == "__main__":
    asyncio.run(main())
