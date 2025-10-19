#!/usr/bin/env python3

import logging
import asyncio
from argparse import ArgumentParser

from aiocoap import CON, NON, GET, PUT, POST, DELETE, PATCH, iPATCH, FETCH, Context, Message
import aiocoap.resource as resource
import aiocoap
from aiocoap.numbers.constants import TransportTuning

logging.basicConfig(level=logging.DEBUG)

print(
    "usage: client.py" +
    " -m <GET|PUT|POST|DELETE|PATCH|iPATCH|FETCH>" +
    " -u <URI>" +
    " [--type <NON|CON>] [--observe] [-p <PAYLOAD>]")


def _get(_list, i, default):
    return _list[i] if len(_list) > i else default


def message_type(arg):
    return {"CON": CON, "NON": NON}[arg]


def method(arg):
    return {
        "GET": GET,
        "PUT": PUT,
        "POST": POST,
        "DELETE": DELETE,
        "PATCH": PATCH,
        "iPATCH": iPATCH,
        "FETCH": FETCH
    }[arg]


async def main():
    parser = ArgumentParser()

    parser.add_argument(
        "-m", "--method",
        help='GET|PUT|POST|DELETE|PATCH|iPATCH|FETCH',
        required=True)

    parser.add_argument(
        "-u", "--uri",
        help='URI',
        required=True)

    parser.add_argument(
        "-t", "--type",
        help='NON|CON',
        default="NON")

    parser.add_argument(
        "--observe",
        action="store_true",
        help='Register for notifications',
        default=False)

    parser.add_argument(
        "--observe-cancel",
        action="store_true",
        help='Cancel notifications',
        default=False)

    parser.add_argument(
        "-p", "--payload",
        help='Payload',
        default=None)

    parser.add_argument(
        "-s", "--timeout",
        type=float,
        help='Request timeout',
        default="4")

    args = parser.parse_args()

    observeValue = None

    if args.observe and args.observe_cancel:
        raise ValueError("cannot register for and cancel notifications")

    if args.observe:
        observeValue = 0

    elif args.observe_cancel:
        observeValue = 1

    print(f"using {message_type(args.type)} {method(args.method)} request")
    print(f"timeout set to {args.timeout}s")

    TransportTuning.REQUEST_TIMEOUT = args.timeout;
    tuning = TransportTuning()

    port = 5600
    protocol = await Context.create_server_context(bind=("::", port), site=resource.Site())
    protocol.client_credentials.load_from_dict({
        '*': {
            'dtls': {
                'psk': b'secretPSK',
                'client-identity': b'Client_identity',
            }
        }
    })

    request = Message(
        mtype=message_type(args.type),
        code=method(args.method),
        uri=args.uri,
        payload=bytes(args.payload, 'utf-8') if args.payload else "",
        observe=observeValue,
        transport_tuning=tuning
    )

    try:
        pr = protocol.request(request)

        r = await pr.response
        print("response: %s\n%r" % (r.code, r.payload))

        if args.observe:
            print("waiting for resource notifications")

            async for r in pr.observation:
                print("notification: %s\n%r" % (r, r.payload))
                break

    except Exception as e:
        print("error:")
        print(e)

if __name__ == "__main__":
    asyncio.run(main())
