## ARM CMSIS-NN package

This application shows how to use the neural network API provided by the ARM CMSIS
package in order to determine the type of "object" present in an RGB image.
The image are part of the [SIFAR10 dataset](http://www.cs.toronto.edu/~kriz/cifar.html)
which contains 10 classes of objects: plane, car, cat, bird, deer, dog, frog,
horse, ship and truck.

Expected output
---------------

```
Predicted class: cat
```

Change the input image
----------------------

Use the `generate_image.py` script and the `-i` option to generate a new
input image.
For example, the following command
```
./generate_image.py -i 1
```
will generate an input containing an image with a boat.

The generated image is displayed at the end of the script execution, for visual
validation of the prediction made by the neural network running on the device.

Note that each time a new image is generated, the firmware must be rebuilt so
that it embeds the new image.
