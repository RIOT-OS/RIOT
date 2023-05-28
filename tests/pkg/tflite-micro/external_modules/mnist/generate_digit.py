#!/usr/bin/env python3

"""Generate a binary file from a sample image of the MNIST dataset.
Pixel of the sample are stored as float32, images have size 28x28.
"""

import os
import argparse

import numpy as np
import matplotlib.pyplot as plt
from tensorflow.keras.datasets import mnist


SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))


def main(args):
    _, (mnist_test, _) = mnist.load_data()
    data = mnist_test[args.index]
    data = data.astype('uint8')

    output_path = os.path.join(SCRIPT_DIR, args.output)
    np.ndarray.tofile(data, output_path)

    if args.no_plot is False:
        plt.gray()
        plt.imshow(data.reshape(28, 28))
        plt.show()


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--index", type=int, default=0,
                        help="Image index in MNIST test dataset")
    parser.add_argument("-o", "--output", type=str, default='digit',
                        help="Output filename")
    parser.add_argument("--no-plot", default=False, action='store_true',
                        help="Disable image display in matplotlib")
    main(parser.parse_args())
