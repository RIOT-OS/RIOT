uTensor sample application
==========================

This application shows how to setup utensor and use it with RIOT: an MLP
(Multi-Layer Perceptron) model is trained with TensorFlow on the
[MNIST dataset](http://yann.lecun.com/exdb/mnist/) in order to be able to
recognize hand written digits in an image.

The code of this application comes from the following blog post:
https://www.hackster.io/news/simple-neural-network-on-mcus-a7cbd3dc108c

Build
-----

```
make BOARD=<board of your choice> all term
```

Expected output
---------------

The digit to recognize is stored in the `digit` binary file of the application
and is automatically added to the application firmware as a C array via the
blob mechanism of the build system.
By default, the digit contains the first test image of the MNIST dataset, a
hand-written `7`.

So by default the expected output should be:

```
Predicted label: 7
```

Use the `generate_digit.py` script provided with this application to update
the digit file from another test image of the MNIST dataset:
```
./generate_digit.py --index 1
```

Each selected digit is displayed at the end of the script to allow a "visual"
comparison with the value predicted by the firmware.
For each new digit generated, the firmware must be rebuilt: the image is
statically embedded as a blob in the firmware image.

Training the model
------------------

The application contains a pre-trained model in the `models` external module.
Except the Makefiles, all C++ files (model + weights) were generated using the
`utensor-cli` tool from a model trained with TensorFlow.

Here are the steps required to train a new model and update the C++ files in the
`models` external module within the application:

1. Install Python3 dependencies
    ```
    pip3 install --user utensor_cgen graphviz
    pip3 install --user tensorflow -U
    ```
  Note that utensor_cgen is only compatible with tensorflow 1 for the moment.

2. Clone the utensor-mnist-demo repository: it contains the Python script used
  to train the MLP model on the MNIST dataset.
    ```
    cd /tmp
    git clone https://github.com/uTensor/utensor-mnist-demo
    ```

3. Train the MLP model:
    ```
    cd /tmp/utensor-mnist-demo
    python3 /tmp/utensor-mnist-demo/tensorflow-models/deep_mlp.py
    ```

    The model is stored in `/tmp/utensor-mnist-demo/mnist_model/deep_mlp.pb`
    using the protocol buffer format.

4. Generate the C++ model files that will be included later in the RIOT build:
    ```
    cd $RIOTBASE/tests/pkg_utensor
    utensor-cli convert /tmp/utensor-mnist-demo/mnist_model/deep_mlp.pb --target utensor --output-nodes=y_pred
    ```
