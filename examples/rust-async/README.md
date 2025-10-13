async Rust runtime and applications
===================================

In this example, we introduce a practical async-Rust runtime empowered by the [embassy-executor-riot](https://gitlab.com/etonomy/riot-module-examples) crate.  Using this runtime, we demonstrate how to

- write an async RIOT application starting from the provided [`async fn main()`](src/lib.rs) entry point,
- run the [Rust gcoap server](../rust-gcoap) application as a spawned async task, and
- use the accompanying [async shell module](src/shell.rs) to interactively test your Rust code within running RIOT.

async shell
-----------

The runtime comes with an async-Rust compatible shell module that helps you test your Rust code interactively within RIOT.  It is designed as a superset of the conventional RIOT shell.  As such, it allows not only executing your Rust code for testing, but also invoking the existing RIOT system/application shell commands as well.

In addition, the shell supports some rudimentary line-input aliasing features:

- *named alias* for creating shorthand for your shell input (e.g. `h` for `help`),
- *function alias* for executing Rust functions for testing,
- *enumerated alias* that could serve as "manually curated" input history,

all of which can be easily customised by editing [src/alias.rs](src/alias.rs).

How to use
----------

Build by

```
$ make
```

and run with a tap interface:

```
$ sudo ip tuntap add dev tap3 mode tap
$ sudo ip link set tap3 down
$ sudo ip link set tap3 up
```

```
$ ./bin/native/rust_async.elf tap3
RIOT native interrupts/signals initialized.
RIOT native board initialized.
RIOT native hardware initialization complete.

main(): This is RIOT! (Version: 2023.07-devel-2828-g193073-rust-async-wip)
task_server(): starting
constfs mounted successfully
CoAP server ready
task_main(): starting
async Rust example application
task_shell(): starting
---- named alias ----
[a] alias
[h] help
---- function alias ----
[f0] <function>
[f1] <function>
[f2] <function>
---- enumerated alias ----
[0] version
[1] ifconfig
[2] ping ::1
(async)> announce_netif():
active interface from PID KernelPID(6) ("gnrc_netdev_tap")
    Address fe80:0000:0000:0000:ec85:2eff:fe5a:64e9

(async)>
```

Call sync/async Rust functions defined in [src/alias.rs](src/alias.rs):

```
(async)> f0
hello world!
(async)> f1
test_async_sleep():
1
2
3
```

RIOT-c (system/application) shell commands are available as well:

```
(async)> help
Command              Description
---------------------------------------
ifconfig             Configure network interfaces
ls                   list files
nib                  Configure neighbor information base
ping                 Ping via ICMPv6
[...]
```

```
(async)> ifconfig
Iface  6  HWaddr: EE:85:2E:5A:64:E9
          L2-PDU:1500  MTU:1500  HL:64  Source address length: 6
          Link type: wired
          inet6 addr: fe80::ec85:2eff:fe5a:64e9  scope: link  VAL
          inet6 group: ff02::1
          inet6 group: ff02::1:ff5a:64e9
(async)>
```
