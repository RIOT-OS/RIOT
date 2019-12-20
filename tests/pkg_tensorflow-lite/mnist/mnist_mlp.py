#!/usr/bin/env python3

import os

# imports for array-handling
import numpy as np

import tensorflow as tf

# keras imports for the dataset and building our neural network
from tensorflow.keras.datasets import mnist
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense, Dropout


# let's keep our keras backend tensorflow quiet
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'

# load mnist dataset
(X_train, y_train), (X_test, y_test) = mnist.load_data()

# building the input vector from the 28x28 pixels
X_train = X_train.reshape(60000, 784)
X_test = X_test.reshape(10000, 784)
X_train = X_train.astype('float32')
X_test = X_test.astype('float32')

# Split the train set in a train + validation set
X_valid = X_train[50000:]
y_valid = y_train[50000:]
X_train = X_train[:50000]
y_train = y_train[:50000]

# Normalize the data
X_train = X_train / 255.0
X_test = X_test / 255.0
X_valid = X_valid / 255.0

# building a very simple linear stack of layers using a sequential model
model = Sequential([
    Dense(64, activation='relu', input_shape=(784,)),
    Dropout(0.2),
    Dense(10, activation='softmax')
])

# compiling the sequential model
model.compile(loss='sparse_categorical_crossentropy', metrics=['accuracy'],
              optimizer='adam')

batch_size = 32
epochs = 30

# training the model and saving metrics in history
history = model.fit(X_train, y_train,
                    batch_size=batch_size, epochs=epochs,
                    verbose=2,
                    validation_data=(X_valid, y_valid))

# saving the model
# Convert the model to the TensorFlow Lite format without quantization
converter = tf.lite.TFLiteConverter.from_keras_model(model)
tflite_model = converter.convert()

# Save the basic model to disk
open("model_basic.tflite", "wb").write(tflite_model)

# Convert the model to the TensorFlow Lite format with quantization
converter = tf.lite.TFLiteConverter.from_keras_model(model)

(mnist_train, _), (_, _) = mnist.load_data()
mnist_train = mnist_train.reshape(60000, 784)
mnist_train = mnist_train.astype('float32')
mnist_train = mnist_train / 255.0
mnist_ds = tf.data.Dataset.from_tensor_slices((mnist_train)).batch(1)


def representative_data_gen():
    for input_value in mnist_ds.take(100):
        yield [input_value]


converter.representative_dataset = representative_data_gen
converter.optimizations = [tf.lite.Optimize.OPTIMIZE_FOR_SIZE]
tflite_model = converter.convert()

# # Save the quantized model to disk
open("model.tflite", "wb").write(tflite_model)

basic_model_size = os.path.getsize("model_basic.tflite")
print("Basic model is %d bytes" % basic_model_size)
quantized_model_size = os.path.getsize("model.tflite")
print("Quantized model is %d bytes" % quantized_model_size)
difference = basic_model_size - quantized_model_size
print("Difference is %d bytes" % difference)

# Now let's verify the model on a few input digits
# Instantiate an interpreter for the model
model_quantized_reloaded = tf.lite.Interpreter('model.tflite')

# Allocate memory for each model
model_quantized_reloaded.allocate_tensors()

# Get the input and output tensors so we can feed in values and get the results
model_quantized_input = model_quantized_reloaded.get_input_details()[0]["index"]
model_quantized_output = model_quantized_reloaded.get_output_details()[0]["index"]

# Create arrays to store the results
model_quantized_predictions = np.empty(X_test.size)

for i in range(10):
    # Invoke the interpreter
    model_quantized_reloaded.set_tensor(model_quantized_input, X_test[i:i+1, :])
    model_quantized_reloaded.invoke()
    model_quantized_prediction = model_quantized_reloaded.get_tensor(model_quantized_output)

    print("Digit: {} - Prediction:\n{}".format(y_test[i], model_quantized_prediction))
    print("")
