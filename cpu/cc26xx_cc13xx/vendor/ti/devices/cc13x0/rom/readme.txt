Debugging sessions in IAR Embedded Workbench (IAR) and Code Composer Studio (CCS)
can load symbol information covering the code in ROM.
Symbols are loaded by selecting the ELF files found in the same folder as this
readme.txt file.
In addition the source code for the driverlib functions in ROM is found in the
driverlib.c file in this folder.

Loading ROM code symbols in CCS debug session:
- Start a debug session in your project
- Select Run > Load > Add Symbols to create additional symbols
- Browse to and select each ELF file in this folder in the 'Program file' field
- Set the value of 0 in the 'Code offset' field for each ELF file
- If you enter a driverlib function in ROM during your debuging session and
  get this information:
  'Can't find a source file at "../<path>/driverlib.c"'
  you can navigate to the driverlib.c file in this folder by selecting
  the 'Locate File..' button. 

Loading ROM code symbols for use in IAR debug session:
- In your project select the following before starting debug session:
  Project > Options.. > Debugger and then select the 'Images'-tab
- In the 'Images'-tab do the following for each of the ELF files
  located in the same folder as this reame.txt file:
  -- Select the 'Download extra image' box
  -- Browse to the ELF file in the 'Path:' field
  -- Set the value of 0 in the 'Offset:' field
  -- Select the 'Debug info only' box
- If you during a debug session enters a driverlib function in ROM you will
  be notified by this message:
  'Could not find following file: ../<path>/driverlib.c'
  Select the browse button and select the driverlib.c file located in this
  folder.
