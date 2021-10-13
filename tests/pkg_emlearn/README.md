## Emlearn package test application

This application shows how to use a machine learning model with emlearn on RIOT
in order to predict a value from a hand written digit image.
The model is a [Scikit-Learn](https://scikit-learn.org) random forest estimator
trained on the MNIST dataset.

### Expected output

The default digit to predict is a hand-written '6', so the application output
is the following:

```
Predicted digit: 6
```

### Use the Python scripts

The application comes with 3 Python scripts:
- `generate_digit.py` is used to generate a new digit file. This file is
  embedded in the firmware image and is used as input for the inference engine.
  Use the `-i` option to select a different digit.
  For example, the following command:
  ```
  $ ./generate_digit.py -i 1
  ```
  will generate a digit containing a '9'.
  The digit is displayed at the end of the script so one knows which digit is
  stored.
  Note that each time a new digit is generated, the firmware image must be
  rebuilt to include this new digit.
- `train_model.py` is used to train a new Scikit-Learn Random Forest estimator.
  The trained model is stored in the `model` binary file.
  ```
  $ ./train_model.py
  ```
  will just train the model.
- `generate_model.py` is used to generate the `sonar.h` header file from the
  `model` binary file. The script is called automatically by the build system
  when the `model` binary file is updated.
