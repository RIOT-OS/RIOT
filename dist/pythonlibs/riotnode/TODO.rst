TODO list
=========

Some list of things I would like to do but not for first publication.


Legacy handling
---------------

Some handling was directly taken from ``testrunner``, without a justified/tested
reason. I just used it to not break existing setup for nothing.
I have more details in the code.

* Ignoring reset return value and error message
* Use killpg(SIGKILL) to kill terminal


Testing
-------

The current 'node' implementation is an ideal node where all output is captured
and reset directly resets. Having wilder implementations with output loss (maybe
as a deamon with a ``flash`` pre-requisite and sometime no ``reset`` would be
interesting.
