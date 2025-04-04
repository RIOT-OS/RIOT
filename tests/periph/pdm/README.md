# Test for `periph_pdm`

This application will test the PDM peripheral typically used with a microphone.
It will start recording in a large application allocated buffer.
Whenever the smaller PDM buffer is full it empties it into the application
buffer.
Once the application buffer is full, it will print all the data.

This allows continuous recording as simply printing the buffer in the PDM callback could be too slow.

After the initial recording, there will be a number of smaller recordings to
verify the sample rates are correct.

## Usage

Simply run:
```
make flash test
```

## Advanced Usage

Maybe one really wants to check the signals are correct, there is a
`pdm_to_wav.py` that allows you to do that.

One can run the following
```
make flash test > /tmp/pdm && \
../../../dist/tools/pdm_to_wav/pdm_to_wav.py \
/tmp/pdm \
--output-file /tmp/output
```

then open the `/tmp/output.wav` to verify, if `matplotlib` is installed you
can also check `tmp/output.png`

## Configuration

This test should run by default with boards that have the PDM feature, but
there are a few parameters that can be tuned:

- `PDM_TEST_MODE` - can be `PDM_MODE_MONO` or `PDM_MODE_STEREO`
- `PDM_TEST_GAIN` - Is the gane, maybe you want something LOUD
- `RECORD_SAMPLE_RATE` - Will be the sample rate used for the recording in Hz
- `RECORD_TIME_IN_MS` - Will be how long the recording lasts... Careful too long
and you won't have enough memory for your board.
