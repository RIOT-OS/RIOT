# Registry CLI

This application demonstrates the most basic usage of the RIOT Registry CLI.

## Usage

Simply build and flash the application for your target board:

```shell
BOARD=YOUR_BOARD_NAME_HERE make flash term
```

Now you should have access to the RIOT shell on your board. For interacting
with the RIOT Registry, use the `registry` shell command, e.g.:

```text
registry export         <- prints the IDs of the whole configuration tree
registry get 0/0/0/0    <- get the value of the red parameter of the instance 0 of the RGB LED Schema inside of the SYS namespace
registry set 0/0/0/0 56 <- set the value of the red parameter to 56
registry apply 0/0/0/0  <- apply new value for the 0/0/0/0 path
registry save           <- save configurations to persistent storage
registry load           <- load configurations from persistent storage
```
