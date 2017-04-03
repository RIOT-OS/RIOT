The purpose of this test is to ensure that C++ constructors are executed
properly during the startup of RIOT. This requires that the port calls
constructors somewhere during C-library initialization. On newlib ports this is
done by __libc_init_array(), other ports may need to manually iterate through
the list of initializer functions (usually .init_array), and call each one in
order.

There are three tests:
 - Global constructor
 - Static constructor
 - Local constructor

The global constructor test checks to see if the constructor of a global object
has been run during the boot process. The static constructor test does the
same, but for static object inside a function. The local constructor test checks
that a locally created object does have its constructor run.
The local constructor test will only fail if there is a significant problem with
the C++ tool chain, since it does not rely on any external C++ support code.
