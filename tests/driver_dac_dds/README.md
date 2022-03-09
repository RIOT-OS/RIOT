# About

This is a manual test, and demo application for DAC DDS.

# Usage

This application will initialize the single default DAC DDS channel, and play
sine, square, sawtooth waves or a greeting audio on it as requested on the command line.

Typical usage:

    > hello
    > sine 440 2

# Audio source and license

hello.raw is an audio rendition of GeoTrinity saying "Ahoy!". It was obtained
from <https://en.wikipedia.org/wiki/File:GT_Ahoi_Deutsch_GT.ogg>
where it is annotated to be licensed under CC-BY-SA 3.0 international.

It was converted and trimmed using:

    $ ffmpeg -i GT_Ahoi_Deutsch_GT.ogg -ac 1 -ar 8000 -f u8 -c:a pcm_u8 hello.raw
    $ python3 -c "s = open('hello.raw', 'rb').read(); s = s.strip(b'\x80') + b'\x80'; open('hello.raw', 'wb').write(s)"
