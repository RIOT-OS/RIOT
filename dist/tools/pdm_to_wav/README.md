# PDM to WAV tool

This tool takes the saved output of the `tests/periph/pdm` firmware, filters
through it to find recorded samples, then formats the samples into a wav file.

## Requirements

To run this script one only needs python3.7+.
If `matplotlib` is installed, then a plot will be generated along with the wav
file.


## Usage
Point to log file with the output from the test, here we provide an example.
```
python pdm_to_wav.py example.txt
```

## Misc

See the [firmware README](../../../tests/periph/pdm) for more info.
