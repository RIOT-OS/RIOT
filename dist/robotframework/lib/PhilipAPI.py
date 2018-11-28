import time
from enum import Enum
import serial
from if_lib import PhilipIf
from robot.version import get_version


class BptUartModes(Enum):
    '''BPT UART test modes.'''
    ECHO = 0
    ECHO_EXT = 1
    REG_ACCESS = 2
    TX = 3


class PhilipAPI(PhilipIf):

    ROBOT_LIBRARY_SCOPE = 'TEST SUITE'
    ROBOT_LIBRARY_VERSION = get_version()

    def reset_dut(self):
        ret = list()
        ret.append(self.set_sys_cr(0xff))
        ret.append(self.execute_changes())
        time.sleep(1)
        ret.append(self.set_sys_cr(0x00))
        ret.append(self.execute_changes())
        time.sleep(1)
        return ret

    def setup_uart(self, mode=BptUartModes.ECHO.value, baudrate=115200,
                   parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE,
                   rts=True):
        '''Setup tester's UART.'''

        # setup testing mode
        cmd_info = self.set_uart_mode(int(mode))
        if cmd_info['result'] != 'Success':
            return cmd_info

        # setup baudrate
        cmd_info = self.set_uart_baud(int(baudrate))
        if cmd_info['result'] != 'Success':
            return cmd_info

        # setup UART control register
        ctrl = 0
        if parity == serial.PARITY_EVEN:
            ctrl = ctrl | 0x02
        elif parity == serial.PARITY_ODD:
            ctrl = ctrl | 0x04

        if stopbits == serial.STOPBITS_TWO:
            ctrl = ctrl | 0x01

        # invert RTS level as it is a low active signal
        if not rts:
            ctrl = ctrl | 0x08

        cmd_info = self.set_uart_ctrl(ctrl)
        if cmd_info['result'] != 'Success':
            return cmd_info

        # reset status register
        cmd_info = self.set_uart_status(0)
        if cmd_info['result'] != 'Success':
            return cmd_info

        # apply changes
        cmd_info = self.execute_changes()
        return cmd_info
