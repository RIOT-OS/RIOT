## TensorFlow Lite sample application

This application contains several examples of usage of TensorFlow Lite for
microcontrollers:
- The default example, `mnist` contains a complete example to perform
  hand-written digit recognition: it shows how to train a very simple
  MLP (Multi-Layer Perceptron) model and how to reuse it in a RIOT application.
  The code of this example is provided as an external module in the
  [mnist](mnist) directory.
- The other example, [Hello World](https://github.com/tensorflow/tensorflow/tree/master/tensorflow/lite/micro/examples/hello_world),
  taken as-is from TensorFlow Lite code, simply replicates a `sine` function
  from a trained model.

To get started with TensorFlow Lite on microcontrollers, please refer to
[this page](https://www.tensorflow.org/lite/microcontrollers).

### Usage

Simply run the application on the board of your choice using:

    make BOARD=<board of your choice> -C tests/pkg_tensorflow-lite flash term

Set `EXAMPLE=hello_world` from the command line to try the upstream hello_world
example.

Then type 's' to start the application.

### Examples details

#### mnist

expected output
---------------

```
Digit prediction: 7
```

scripts usage
-------------

First, install tensorflow:

```
pip3 install --user tensorflow
```

The scripts require TensorFlow >= 2, so a fairly recent version of pip is
required.

The `mnist_mlp` example comes with 2 Python scripts:
- `mnist_mlp.py` is used to train and store the model. To minimize the size of
  the generated model, the script uses
  [post-training quantization](https://www.tensorflow.org/lite/performance/post_training_quantization).
  The quantized model is stored in the `model.tflite` file in the FlatBuffers
  format and is embedded in the application using the BLOB mechanism.
- `generate_digit.py` is used to generate the `digit` from the MNIST dataset
  test data. The default digit generated corresponds to a 7.
  Use the `-i` option to choose another digit. The script displays the
  generated digit so you can compare with the prediction made by the RIOT
  application.
  Note that after a new digit is generated the firmware has to be rebuilt so
  that it embeds the array containing the pixel values.

#### hello_world

expected output
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
