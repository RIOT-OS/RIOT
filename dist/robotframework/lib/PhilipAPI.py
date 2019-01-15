from riot_pal import LLMemMapIf, PHILIP_MEM_MAP_PATH
from robot.version import get_version
from time import sleep

class PhilipAPI(LLMemMapIf):

    ROBOT_LIBRARY_SCOPE = 'TEST SUITE'
    ROBOT_LIBRARY_VERSION = get_version()

    def __init__(self, port, baudrate):
        super(PhilipAPI, self).__init__(PHILIP_MEM_MAP_PATH, 'serial', port, baudrate)

    def reset_dut(self):
        ret = list()
        ret.append(self.write_reg('sys.cr', 0xff))
        ret.append(self.execute_changes())
        sleep(1)
        ret.append(self.write_reg('sys.cr', 0x00))
        ret.append(self.execute_changes())
        sleep(1)
        return ret
