How To Run
==========
Connect the DUT (device under test) I2C0 to the BPT (bluepill tester).  Information for BPT setup can be found [here](https://github.com/MrKevinWeiss/Testing).

### Example Test With Nucleo-F401RE
1. Flash the periph_i2c firmware on the nucleo-f401 with `make BOARD=nucleo-f401 flash` in the periph_i2c directory
2. Connect the [BPT](https://github.com/MrKevinWeiss/Testing/blob/master/resources/bptpinout.jpg) to the cpu using a USB to Serial converter</br>
(BPT IF_TX = PA9)</br>
(BPT IF_RX = PA10)
3. Connect the BPT to the DUT</br>
(DUT SCL/D15 <=> BPT SCL1/PB6)</br>
(DUT SDA/D14 <=> BPT SDA1/PB7)</br>
(DUT NRST <=> BPT PB1)</br>
4. Run the test.py from the periph_i2c/tests directory (with various options)</br>
`python test.py` for basic tests with autoconnect</br>
`python3 test.py` for python 3</br>
`python test.py --log=DEBUG` to see all debug messages</br>
`python test.py --dut_port="/dev/ttyACM0"` to specify the port</br>
`python test.py --log=DEBUG --dut_port="/dev/ttyACM0" --dut_baud=9600 --bpt_port="/dev/ttyACM1" > nucleo-f401_test.txt` for all the fix'ns</br>

Flags
==========
--log=DEBUG -> allows for debug output</br>
--dut_port -> the port name of the DUT</br>
--dut_baud -> the baud rate of the DUT</br>
--bpt_port -> the port name of the BPT

Notes
==========
- If no serial connection to the BPT the test should still be able to run, it just will not be able to reset.
- Connect the SDA, SCL and if possible the DUT reset pin.
- Autoconnect *may* work if no ports are specified.
- Default baud rate is 115200
