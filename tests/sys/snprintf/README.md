# snprintf

This test aims to test if the stdio implementations correctly implements
standard format specifiers. Instead of relying on the transport of stdout to
be fast and reliable, it will use snprintf to format in-memory and compare
in the app with correctness.
