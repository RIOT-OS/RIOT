FlatBuffer sample application
=============================

This application shows how to use the FlatBuffer library from Google for
serializing/deserializing a runtime C++ object to/from a binary buffer.

The application is
[taken as is from the upstream repository](https://github.com/google/flatbuffers/blob/master/samples/sample_binary.cpp).

More details are available on
[the FlatBuffers website](http://google.github.io/flatbuffers/).

Usage
-----

Simply flash and run the application on the board of your choice using:

    make BOARD=<board of your choice> -C tests/pkg_flatbuffers flash term

Expected result
---------------

The application creates an object, serialize it in a FlatBuffer and after
reloading the serialized object it displays:

    "The FlatBuffer was successfully created and verified!"
