This is a simple proof of concept test, that `gnrc_sock` with `gnrc_neterr`
works as expected to report errors within the network stack up to the `sock`
object.

It just tests if the sock reports `-EHOSTUNREACH` when sending to an unreachable
address. To confirm that this is indeed coming from the stack and not the `sock`
wrapper you can do

```
git grep EHOSTUNREACH -- sys/net/gnrc/sock
```
