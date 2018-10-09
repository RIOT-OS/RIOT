import time

from if_lib import PhilipIf
from robot.version import get_version


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
