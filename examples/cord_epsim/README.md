CoRE Resource Directory: Simple Endpoint Example
================================================

This example shows how a node can register with a CoRE resource directory using
the simple registration procedure as described in
draft-ietf-core-resource-directory-27, section 5.1.

When running this example, you **must** define the RD server's IPv6 address
statically, using the `RD_ADDR` environment variable:
```
RD_ADDR=\\\"[abc:0815::123]\\\" make all
```
Per default, this is set to some random global address (`[affe::1]`) for compile
test reasons. So change it!

Additionally, you can change the RD server's port by specifying it like so:
```
RD_ADDR=\\\"[affe::bee]:12345\\\" make all
```

The registration lifetime is set for demonstration purposes to a rather short
time of 60s in this example. You can easily override that value using the
`RD_LT` variable:
```
RD_ADDR=\\\"[abc::def]\\\" RD_LT=3600 make all ...
```
