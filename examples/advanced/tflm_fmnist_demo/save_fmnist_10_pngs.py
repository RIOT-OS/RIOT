#!/usr/bin/env python3
# save_fmnist_10_pngs.py
# Downloads Fashion-MNIST, picks N samples, resizes to 48x48 grayscale, saves PNGs.

import os
import argparse
import numpy as np
import tensorflow as tf
from tensorflow import keras

CLASS_NAMES = [
    "T-shirt_top", "Trouser", "Pullover", "Dress", "Coat",
    "Sandal", "Shirt", "Sneaker", "Bag", "Ankle_boot"
]

def save_png(img_u8_hw1: tf.Tensor, path: str) -> None:
    """
    img_u8_hw1: uint8 tensor of shape (H, W, 1)
    """
    png_bytes = tf.image.encode_png(img_u8_hw1)
    tf.io.write_file(path, png_bytes)

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--out-dir", default="imgs", help="Output folder for PNGs")
    ap.add_argument("--count", type=int, default=10, help="Number of images to save")
    ap.add_argument("--size", type=int, default=48, help="Resize to HxW (default 48)")
    ap.add_argument("--seed", type=int, default=123, help="Random seed")
    args = ap.parse_args()

    os.makedirs(args.out_dir, exist_ok=True)

    # Load Fashion-MNIST test set (28x28 grayscale)
    (_, _), (x_test, y_test) = keras.datasets.fashion_mnist.load_data()

    rng = np.random.default_rng(args.seed)
    idxs = rng.choice(len(x_test), size=args.count, replace=False)

    for i, idx in enumerate(idxs, 1):
        img = x_test[idx]  # (28,28) uint8
        lab = int(y_test[idx])

        # → float32 [0,1] → add channel → resize to (size,size,1)
        img_f = tf.convert_to_tensor(img, dtype=tf.float32) / 255.0
        img_f = tf.expand_dims(img_f, axis=-1)  # (28,28,1)
        img_f = tf.image.resize(img_f, (args.size, args.size), method="bilinear")

        # → uint8 [0,255] (grayscale)
        img_u8 = tf.cast(tf.round(img_f * 255.0), tf.uint8)  # (H,W,1)

        fname = f"{i:02d}_{CLASS_NAMES[lab]}.png"
        out_path = os.path.join(args.out_dir, fname)
        save_png(img_u8, out_path)

    print(f"Saved {args.count} PNGs to: {os.path.abspath(args.out_dir)}")

if __name__ == "__main__":
    main()
