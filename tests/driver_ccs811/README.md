## About
This is a test application for the CCS811 gas sensor that can detect
Volatile Organic Compounds (VOCs).

## Usage
The application will first initialize the CCS811 sensor. Please see section 3.4
of the
[datasheet](https://ams.com/eng/content/.../CCS811_DS000459_4-00.pdf) for
details.

After initialization, the application:
* measures and prints the eCO2 level (in ppm)
* measures and prints the TVOC level (in ppb)

Note: there's a little boot phase where measures are not ready. After some
cycles the measures become ready.
