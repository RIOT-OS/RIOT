from periph_i2c_if import PeriphI2CIf
from robot.version import get_version


class I2Cdevice(PeriphI2CIf):

    ROBOT_LIBRARY_SCOPE = 'TEST SUITE'
    ROBOT_LIBRARY_VERSION = get_version()
