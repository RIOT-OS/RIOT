How To Run
==========
Connect the DUT (device under test) UART6 to the BPT (bluepill tester).
Information for BPT setup can be found [here](https://github.com/MrKevinWeiss/Testing).

### Example Test With Nucleo-F401RE
1. Flash the periph_uart firmware on the nucleo-f401 with `make BOARD=nucleo-f401 flash` in the periph_uart directory
2. Connect the [BPT](https://github.com/MrKevinWeiss/Testing/blob/master/resources/bptpinout.jpg) to the cpu using a USB to Serial converter</br>
(BPT IF_TX = PA9)</br>
(BPT IF_RX = PA10)
3. Connect the BPT to the DUT</br>
(DUT RX/PA12 <=> BPT TX3/PB10)</br>
(DUT TX/PA11 <=> BPT RX3/PB11)</br>
4. `test.py` requires the path to `$RIOTBASE/dist/tests` to be added to `PYTHONPATH` environment variable. Either modify `PYTHONPATH` permanently in the `.bashrc` or modify it temporary before invocation:</br>
`PYTHONPATH=$PYTHONPATH:$RIOTBASE/dist/tests python3 test.py`
5. Run the `test.py` from the `periph_uart/tests` directory (with various options)</br>
`python3 test.py` for python 3</br>
`python3 test.py --log=DEBUG` to see all debug messages</br>
`python3 test.py --dut_port="/dev/ttyACM0"` to specify the port</br>
`python3 test.py --log=DEBUG --dut_port="/dev/ttyACM0" --dut_uart=6 --bpt_port="/dev/ttyACM1" > nucleo-f401_test.txt` for all the fix'ns</br>

Flags
==========
--log=DEBUG -> allows for debug output</br>
--dut_port -> the port name of the DUT</br>
--dut_uart -> DUT UART number to test</br>
--bpt_port -> the port name of the BPT
