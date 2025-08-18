# tflm\_fmnist\_demo (INT8, runtime-quantized samples)

A minimal **TensorFlow Lite Micro** (INT8) example for RIOT.

* Loads an **INT8 TFLite** model embedded in `model_data.cpp` (`g_model`, `g_model_len`)
* Uses **48×48 grayscale** Fashion-MNIST test images from `imgs/`
* Converts them at runtime into the model’s input tensor and prints predictions

## Directory layout

```
tflm_fmnist_demo/
├─ imgs/                     # 10 PNGs already included (no extra step needed)
├─ main.cpp                  # RIOT app (C++)
├─ Makefile                  # enables C++ and tflite-micro
├─ model_data.cpp            # INT8 .tflite as a C array (g_model/g_model_len)
├─ pngs_to_samples_h.py      # optional: build a header from PNGs (not required)
├─ samples.h                 # header consumed by main.cpp (generated from imgs/)
└─ save_fmnist_10_pngs.py    # optional: re-generate 10 Fashion-MNIST PNGs
```

> The **images are already present**, so you don’t need to run any Python to try the demo.

## Build & flash (nRF52840 DK)

```bash
# from this folder
PORT=/dev/ttyACM0 BOARD=nrf52840dk make all term flash WERROR=0
```

* `PORT` must point to your board’s serial device.
* It’s normal to see warnings; `WERROR=0` keeps them non-fatal.

### First build takes a while (expected)

The initial flash compiles several TFLM packages (flatbuffers, gemmlowp, ruy, kernels, micro, etc.). You’ll see long lines like:

```
"make" -C .../tflite-micro/tensorflow/lite/kernels -f .../tflite-kernels.mk
"make" -C .../tflite-micro/tensorflow/lite/kernels/internal -f .../tflite-kernels-internal.mk
"make" -C .../tflite-micro/tensorflow/lite/micro -f .../tflite-micro.mk
...
```

That’s expected for the **first** build on a fresh tree.

## Expected output

After flashing, the app runs inference over the 10 images and prints something like:

```
Done flashing
/home/andrej/RIOT2/dist/tools/pyterm/pyterm -p "/dev/ttyACM0" -b "115200" ...
Welcome to pyterm!
Type '/exit' to exit.
[#0] pred=Sneaker  (true=Sandal)      time=3148495 us
[#1] pred=Shirt    (true=Shirt)       time=3148515 us
[#2] pred=Shirt    (true=Coat)        time=3148498 us
[#3] pred=Trouser  (true=Trouser)     time=3148511 us
[#4] pred=Shirt    (true=Coat)        time=3148493 us
[#5] pred=Coat     (true=Coat)        time=3148493 us
[#6] pred=Ankle boot  (true=Ankle boot) time=3148493 us
[#7] pred=Shirt    (true=Coat)        time=3148492 us
[#8] pred=Sneaker  (true=Bag)         time=3148505 us
[#9] pred=Ankle boot  (true=Ankle boot) time=3148493 us

Done.
```

> On nRF52840 with reference kernels, \~3.1 s per inference is normal.
> For a **2–4× speedup**, you can enable CMSIS-NN (see “Tips” below).

## Regenerating images / labels (optional)

You already have 10 images in `imgs/`. If you want to change them:

1. Create new PNGs (two options):

* Use your own 48×48 grayscale PNGs, or
* Generate 10 Fashion-MNIST PNGs deterministically:

  ```bash
  python3 save_fmnist_10_pngs.py --out-dir ./imgs --size 48
  ```

2. Rebuild `samples.h` from the PNGs:

```bash
python3 pngs_to_samples_h.py --img-dir ./imgs --out-header samples.h --limit 10
```

The script infers labels from filenames (e.g., `01_Sandal.png`, `02_Shirt.png`, …).

## Model provenance & related idea

* The compact INT8 model was distilled from a larger teacher model; weights and training scripts are from:
  **[https://github.com/KiyoLelou10/FashionMNISTDistill](https://github.com/KiyoLelou10/FashionMNISTDistill)**

* That repo also includes **people images**. Using the same RIOT/TFLM pattern you see here, you could train or distill a small **people-counting** model (e.g., 0–5+) and deploy it in an embedded example.