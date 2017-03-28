### About

This example enables to execute arbitrary Javascript directly from the command line on the RIOT shell.

### Acknowledgement
This example is based on [Jerryscript](https://github.com/jerryscript-project/jerryscript) which does all the heavy lifting, providing full ECMAScript 5.1 profile on your IoT device (kudos guys!).

### Caveats

- On your local board: best used with a serial communication program such [minicom](https://help.ubuntu.com/community/Minicom) instead of PyTerm (see why below).

- On a testbed: you can try it on [IoT-lab](https://www.iot-lab.info). Tested and works so far on [IoT-lab M3](https://www.iot-lab.info/hardware/m3/), upload and flash the .elf, ssh into the node, and script away!

- Except in the `print` instruction in your script, you have to replace single brackets `'` with `\'`.

- Expect some issues with PyTerm which interprets by default the first `;` as the end of the script command. Furthermore, if the script is long, PyTerm seems to get confused (hence the recommended use of minicom). To start playing with PyTerm, first edit `;` line 256 of RIOT/dist/tools/pyterm/pyterm

### How to build

Type `make flash`. Then use your preferred serial communication tool to land in the RIOT shell.
Note: you may have to type `reboot` or to press `RESET` on the board (after the flash).

### Running the example

In the RIOT shell, `help` will provide the list of available commands.

The `script` command will run the test script code that you input in the command line.
Some examples of scripts you can try:
```
script print ('hello');
```
```
script var x = Math.sqrt (64); var txt = \'\'; while (x>1) { txt += --x + \'\\n\';} print (txt);
```
```
script var person = { fname:\'John\', lname:\'Doe\', age:25 }; var text = \'\'; var x; for (x in person) { text += person[x] + \'\\n\'; } print (text);
```

Remark: outside of the print command, you may have to replace single brackets ' with \'.