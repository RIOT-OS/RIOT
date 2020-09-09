#!/usr/bin/env python3

"""Generate a binary file from a sample image of the CIFAR-10 dataset.
Pixel of the sample are stored as uint8, images have size 32x32x3.
"""

import os
import argparse

import numpy as np
import matplotlib.pyplot as plt
from tensorflow.keras.datasets import cifar10


SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))


def main(args):
    _, (cifar10_test, _) = cifar10.load_data()
    data = cifar10_test[args.index]
    data = data.astype('uint8')

    output_path = os.path.join(SCRIPT_DIR, args.output)
    np.ndarray.tofile(data, output_path)

    if args.no_plot is False:
        plt.imshow(data)
        plt.show()


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--index", type=int, default=0,
                        help="Image index in CIFAR test dataset")
    parser.add_argument("-o", "--output", type=str, default='input',
                        help="Output filename")
    parser.add_argument("--no-plot", default=False, action='store_true',
                        help="Disable image display in matplotlib")
    main(parser.parse_args())
