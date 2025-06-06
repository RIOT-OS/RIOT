# RIOT TLS over TCP Example Application

This application demonstrates secure communication using TLS over TCP sockets with [RIOT OS] and [wolfSSL] on the `native`/`native64` board. It provides both server and client modes, allowing two RIOT instances to establish an encrypted connection over IPv6 using virtual network interfaces.

## Features

- Simple TLS server and client using RIOT's `sock_tls` API and wolfSSL
- IPv6 networking over tap (virtual) interfaces
- Interactive shell: start server/client with commands
- Example certificate/key management via header files

---

## Setup Instructions

---

### 1. Create Virtual Network Interfaces

The RIOT native board uses Linux tap interfaces for networking.
**Before running the application, create the tap interfaces:**

```sh
sudo ./../RIOT/dist/tools/tapsetup/tapsetup -c 2
```
- This creates two interfaces, `tap0` and `tap1`.
- You can check they exist with: `ip link show tap0 tap1`

**To delete these interfaces when done:**
```sh
sudo ./../RIOT/dist/tools/tapsetup/tapsetup -d
```

### 2. Start the Server and Client

Open **two terminals**:

**Terminal 1 (Server):**
```sh
PORT=tap0 make term
```
At the RIOT shell prompt, type:
```
tls_server
```

**Terminal 2 (Client):**
```sh
PORT=tap1 make term
```
At the RIOT shell prompt, type:
```
tls_client <server_ipv6>
```
Replace `<server_ipv6>` with the IPv6 address of the server instance (visible with the `ifconfig` shell command).
Typically, this will be something like `fe80::...`.

---

## File Structure

- `main.c` &mdash; Application entry point with shell commands for server/client
- `ca_cert.h`, `cert_data.h` &mdash; Certificates and private key in PEM format as C arrays
- `Makefile` &mdash; Build configuration

---

## Notes

- Make sure you have valid certificates and keys in `ca_cert.h` and `cert_data.h`.
- You can generate test certificates using OpenSSL, or use the provided ones for development only.
- This app requires root privileges to create/delete tap interfaces, but not to run the RIOT binaries themselves.

---

## Cleaning Up

When finished, delete the tap interfaces with:
```sh
sudo ./../RIOT/dist/tools/tapsetup/tapsetup -d
```

---

## Troubleshooting

- If you get errors related to interfaces or networking, ensure tap interfaces are created and you are using the right `PORT`.
- Use `ifconfig` in the RIOT shell to find your node's IPv6 address.
- If TLS setup fails, check that certificate/key data matches and is passed correctly to the TLS functions.
