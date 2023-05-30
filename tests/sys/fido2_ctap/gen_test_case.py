from fido2.ctap2 import CTAP2
from fido2 import cbor
from fido2.client import Fido2Client
from fido2.server import Fido2Server
from unittest import mock


def mock_device():
    device = mock.MagicMock()
    return CTAP2(device)


def get_cbor(data):
    request = b""
    if data is not None:
        request += cbor.encode(data)

    return request


def args(*params):
    """Constructs a dict from a list of arguments for sending a CBOR command.
    None elements will be omitted.
    """
    return dict((i, v) for i, v in enumerate(params, 1) if v is not None)


def print_req(req, prefix):
    print(f"static uint8_t {prefix}_data[] = {{", end='')
    print("".join(f"{hex(x)}, " for x in req), end='')
    print("};")


def gen_mc_req():
    dev = mock_device()
    dev.capabilities = 0
    user = {"id": b"user_id", "name": "A. User"}
    client = Fido2Client(dev, "https://example.com")
    server = Fido2Server({"id": "example.com", "name": "Example RP"}, attestation="direct")

    create_options, _ = server.register_begin(user)
    create_options = create_options['publicKey']
    client_data = client._build_client_data("webauthn.create", create_options['challenge'])

    options = {}
    options["rk"] = True

    return get_cbor(
        args(
            client_data.hash,
            create_options["rp"],
            create_options["user"],
            create_options["pubKeyCredParams"],
            None,  # exclude list
            None,  # extensions
            options,
            None,  # pin_auth
            None,  # pin protocol version
        ))


def gen_ga_req():
    dev = mock_device()
    dev.capabilities = 0
    # user = {"id": b"user_id", "name": "A. User"}
    client = Fido2Client(dev, "https://example.com")
    server = Fido2Server({"id": "example.com", "name": "Example RP"}, attestation="direct")

    request_options, _ = server.authenticate_begin()
    request_options = request_options['publicKey']
    client_data = client._build_client_data("webauthn.get",  request_options['challenge'])

    return get_cbor(
        args(
            request_options["rpId"],
            client_data.hash,
            None,  # allow list
            None,  # extensions
            None,  # options
            None,  # pin_uv_param
            None   # pin_uv_protocol
        ))


if __name__ == "__main__":
    req = gen_mc_req()
    print_req(req, "mc")

    print("")

    req = gen_ga_req()
    print_req(req, "ga")
