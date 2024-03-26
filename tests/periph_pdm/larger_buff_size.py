import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import re
import numpy as np
import os
import csv
from itertools import groupby
from operator import itemgetter
import serial
import struct
import wave
import time

str1 = "Start of the new buffer".encode('ascii')
str2= "End of the new buffer".encode('ascii')

start_index_buf = 0
stop_index_buf = 0
start_found = False
buffer_number = 0
start = True

ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)
data = {}
key = 0

ser.writelines(["start\n".encode('ascii')])
while start:
    line = ser.readline()
    #print(line)
    value = line.strip()
    data[key] = value
    if line.find(str1) != -1:
        #print("Start Recording")
        start_found = True
        start_index_buf = key
    if line.find(str2) != -1:

        if start_found:
            stop_index_buf = key
            buffer_number = stop_index_buf - start_index_buf - 2
            start = False
            #print("Stop recording")
            break
    key +=1

print(start_index_buf)
print(stop_index_buf)
print(buffer_number)
ser.close()

# Extract the data within the specified range
data_to_plot = [int(data[i]) for i in range(start_index_buf+1, stop_index_buf)]
data_to_bytes = [int(data[i]).to_bytes(2,byteorder="little",signed=True) for i in range(start_index_buf+1, stop_index_buf)]
data_to_export = [(data[i]) for i in range(start_index_buf+1, stop_index_buf)]
#print(data_to_bytes)

# Save the data to 16-b file
# Open a file in binary write mode
with open("binary_data.bin", "wb") as f:
    for binary_data in data_to_bytes:
        # Write the binary data to the file
        f.write(binary_data)

# Sample rate (Hz)
sample_rate = 16000

# Open a new wave file
with wave.open("output_wave.wav", "w") as output_wave:
    # Set parameters for the wave file
    output_wave.setnchannels(1)  # Mono
    output_wave.setsampwidth(2)  # 2 bytes per sample for 16-bit audio
    output_wave.setframerate(sample_rate)

    # Convert data_to_plot to numpy array
    data_np = np.array(data_to_bytes)

    # Convert data to bytes and write to the wave file
    output_wave.writeframes(data_np)

# Sample rate (Hz)
samplerate = 16000
length = 1

# The left channel for 1 second.
t = np.linspace(0, length, samplerate)
left_channel = 0.5 * np.sin(2 * np.pi * 440.0 * t)

# Noise on the right channel.
#right_channel = np.random.random(size=samplerate)
left_channel = data_to_plot/np.max(np.abs(data_to_plot))
audio = np.array([left_channel]).T

# Convert to (little-endian) 16 bit integers.
audio = (audio * (2 ** 15 - 1)).astype("<h")

with wave.open("sound_new_buf.wav", "w") as f:
    # Mono Channel
    f.setnchannels(1)
    # 2 bytes per sample.
    f.setsampwidth(2)
    f.setframerate(samplerate)
    f.writeframes(audio.tobytes())

print("WAV file successfully created.")

# Plot the sliced data
plt.plot(data_to_plot)

plt.xlabel('Number of Sample')
plt.ylabel('Signal Value', rotation=90)

# Adjusting the limits of the axes for more detail
plt.xlim(start_index_buf, len(data_to_plot))
plt.ylim(min(data_to_plot), max(data_to_plot))
plt.grid(True)
plt.show()
