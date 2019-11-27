TensorFlow Lite sample application
==================================

This application is taken as-is from the hello-world sample
application of TensorFlow Lite for microcontrollers.

The original code is
[available on GitHub](https://github.com/tensorflow/tensorflow/tree/master/tensorflow/lite/experimental/micro/examples/hello_world).

This application simply replicates a `sine` function from a trained model.

To get started with TensorFlow Lite on microcontrollers, please refer to
[this page](https://www.tensorflow.org/lite/microcontrollers).

Usage
-----

Simply run the application on the board of your choice using:

    make BOARD=<board of your choice> -C tests/pkg_tensorflow-lite flash term

Then type 's' to start the application.

Expected result
---------------

The application prints the values of the `sine` function:
```
x_value: 1.0*2^-127, y_value: 1.9783614*2^-8

x_value: 1.2566366*2^-2, y_value: 1.3910355*2^-2

x_value: 1.2566366*2^-1, y_value: 1.1282844*2^-1

x_value: 1.8849551*2^-1, y_value: 1.5455950*2^-1

x_value: 1.2566366*2^0, y_value: 1.8238020*2^-1

x_value: 1.5707957*2^0, y_value: 1.8701699*2^-1

x_value: 1.8849551*2^0, y_value: 1.8547139*2^-1

x_value: 1.995567*2^1, y_value: 1.4683149*2^-1

x_value: 1.2566366*2^1, y_value: 1.1128282*2^-1

x_value: 1.4137159*2^1, y_value: 1.819164*2^-2

x_value: 1.5707957*2^1, y_value: -1.2364758*2^-5

x_value: 1.7278753*2^1, y_value: -1.6074185*2^-2

x_value: 1.8849551*2^1, y_value: -1.2982997*2^-1

x_value: 1.210171*2^2, y_value: -1.7928901*2^-1

x_value: 1.995567*2^2, y_value: -1.46367*2^0

x_value: 1.1780966*2^2, y_value: -1.46367*2^0
```
