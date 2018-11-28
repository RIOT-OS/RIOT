from periph_uart_if import PeriphUartIf
from robot.version import get_version


class UartDevice(PeriphUartIf):

    ROBOT_LIBRARY_SCOPE = 'TEST SUITE'
    ROBOT_LIBRARY_VERSION = get_version()
