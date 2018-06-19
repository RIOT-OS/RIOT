import test_shell_if
import serial.tools.list_ports
import logging


class PeriphI2CIf(test_shell_if.TestShellIf):

    def __init__(self, port=None, baud=115200):

        if (port is None):
            self.autoconnect()
        else:
            self.connect(port, baud)

    def autoconnect(self):
        found_connection = False
        comlist = serial.tools.list_ports.comports()
        connected = []
        logging.debug("Autoconnecting")
        for element in comlist:
            connected.append(element.device)
        for port in connected:
            logging.debug("Port: " + port)
            self.connect(port)
            ret = self.get_id().data
            if (isinstance(ret, list)):
                if (len(ret) is 1):
                    if (isinstance(ret[0], str)):
                        logging.debug("ID: %s" % ret[0])
                        if ('periph_i2c' in ret[0]):
                            logging.debug("Found connection")
                            found_connection = True
                            break
            self.disconnect()
        return found_connection

    def assign_dev(self, dev_num):
        return self.send_cmd('i2c_assign_dev %d' % (dev_num))

    def acquire(self):
        return self.send_cmd('i2c_acquire')

    def release(self):
        return self.send_cmd('i2c_release')

    def read_reg(self, addr, reg, flag=None):
        if (flag is None):
            return self.send_cmd('i2c_read_reg %d %d' % (addr, reg))
        else:
            return self.send_cmd('i2c_read_reg %d %d %d' % (addr, reg, flag))

    def read_regs(self, addr, reg, len, flag=None):
        if (flag is None):
            return self.send_cmd('i2c_read_regs %d %d %d' % (addr, reg, len))
        else:
            return self.send_cmd('i2c_read_regs \
            %d %d %d %d' % (addr, reg, len, flag))

    def read_byte(self, addr, flag=None):
        if (flag is None):
            return self.send_cmd('i2c_read_byte %d' % (addr))
        else:
            return self.send_cmd('i2c_read_byte %d %d' % (addr, flag))

    def read_bytes(self, addr, len, flag=None):
        if (flag is None):
            return self.send_cmd('i2c_read_bytes %d %d' % (addr, len))
        else:
            return self.send_cmd('i2c_read_bytes %d %d %d' % (addr, len, flag))

    def write_reg(self, addr, reg, data, flag=None):
        if (flag is None):
            return self.send_cmd('i2c_write_reg %d %d %d' % (addr, reg, data))
        else:
            return self.send_cmd('i2c_write_reg \
            %d %d %d %d' % (addr, reg, data, flag))

    def write_regs(self, addr, reg, data, flag=None):
        str = ''
        for val in data:
            str += ' %d' % (val)
        if (flag is None):
            return self.send_cmd('i2c_write_regs %d %d 0' % (addr, reg) + str)
        else:
            return self.send_cmd('i2c_write_regs \
            %d %d %d' % (addr, reg, flag) + str)

    def write_byte(self, addr, data, flag=None):
        if (flag is None):
            return self.send_cmd('i2c_write_byte %d %d' % (addr, data))
        else:
            return self.send_cmd('i2c_write_byte \
            %d %d %d' % (addr, flag, data))

    def write_bytes(self, addr, data, flag=None):
        str = ''
        for val in data:
            str += ' %d' % (val)
        if (flag is None):
            return self.send_cmd('i2c_write_bytes %d 0' % (addr) + str)
        else:
            return self.send_cmd('i2c_write_bytes %d %d' % (addr, flag) + str)

    def get_devs(self):
        return self.send_cmd('i2c_get_devs')

    def get_speed(self):
        return self.send_cmd('i2c_get_speed')

    def get_id(self):
        return self.send_cmd('i2c_get_id')
