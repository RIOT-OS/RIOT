Expected result
===============

This is a test application for using the timeout module of gnrc_mac for setting timeouts.

When everything works as expected, you should see timeouts expired at the time they are set
to be expired. Also, you should see the status of timeouts are corresponding to their real
status, i.e., the system should state that a timeout is "running" when it is set and
awaiting to be expired, and state "not running" when a timeout has expired and not set
again.

Background
==========
Test for verifying the functionalities of the timeout module of gnrc_mac.
