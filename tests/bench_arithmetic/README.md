Benchmark for arithmetic operations
===================================

This benchmark measures the performance of various basic arithmetic operations
using different data types.
An array of TEST_BLOCKSIZE pseudorandom numbers is used as the input to the test
functions and applied TEST_ITERATIONS times to get a better timing average.

Tested operations
-----------------

The data types tested are:

 - `uint8_t`
 - `uint16_t`
 - `uint32_t`
 - `uint64_t`
 - `int8_t`
 - `int16_t`
 - `int32_t`
 - `int64_t`
 - `float`
 - `double`

The operations tested are:

 - assignment `=`
 - addition `+=`
 - subtraction `-=`
 - multiplication `*=`
 - division `/=`
 - left shift `<<=`
 - right shift `>>=`
 - bitwise OR `|=`
 - bitwise AND `&=`
 - bitwise XOR `^=`

Test function summary
---------------------

For variable operands, the test function will iterate over the data array and perform

    data[k] (operator) data[k+1];

for the tests with runtime and compile time constant second operand, the test
function instead applies:

    data[k] (operator) constant;


Results
=======

The output shows the execution time (in usec by default) required for applying
the given operator to TEST_ITERATIONS * TEST_BLOCKSIZE number of pseudorandom values.
