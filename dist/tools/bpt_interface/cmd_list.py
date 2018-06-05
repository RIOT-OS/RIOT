import serial

PORT = '/dev/ttyACM0'
BAUD = 115200
dev = serial.Serial(PORT, BAUD, timeout=1)


def open():
    dev = serial.Serial(PORT, BAUD, timeout=1)
    if(dev.isOpen() is False):
        dev.open()


def close():
    dev.close()


def read_bits(index, size):
    if(dev.isOpen() is False):
        dev.open()
    bits = (2 ^ size) - 1
    rx_data = read_byte(index/8, 1)
    if (not isinstance(rx_data, long)):
        return rx_data
    rx_data = (rx_data >> (index & 0x7)) & bits
    print('bits: 0x%X' % (rx_data))
    return rx_data


def write_bits(index, size, data):
    if(dev.isOpen() is False):
        dev.open()
    bits = (2 ** (size)) - 1
    rx_data = read_byte(index/8, 1)
    if (not isinstance(rx_data, long)):
        return rx_data
    rx_data = rx_data & (0b11111111 ^ (bits << (index & 0x7)))
    rx_data = rx_data | ((data & bits) << (index & 0x7))
    rx_data = write_bytes(index/8, 1, rx_data)
    return rx_data


def read_byte(index, size):
    if(dev.isOpen() is False):
        dev.open()
    cmd = 'rr %d %d\n' % (index, size)
    print(cmd)
    dev.write(cmd)
    raw_response = dev.readline()
    response = raw_response.split(',')
    print([x for x in raw_response.split(',') if x])
    if (len(response) is 0):
        return "no response"
    if (response[0] is not "0"):
        return "error code: " + response[0]
    if (len(response) is 1):
        return "unexpected error"

    if (size < 8):
        data = long(response[1], 0)
    else:
        data = bytearray.fromhex(response[1][2:len(response[1]) - 1])
    print("data: " + response[1])
    return data


def write_bytes(index, size, data):
    if(dev.isOpen() is False):
        dev.open()
    cmd = 'wr %d' % (index)
    if (isinstance(data, list)):
        for i in range(size):
            if (size - i < len(data)):
                cmd += ' %d' % (data[size - i])
            else:
                cmd += ' 0'
        cmd += '\n'
    else:
        for i in range(size):
            cmd += ' %d' % ((data >> ((i) * 8)) & 0xFF)
        cmd += '\n'

    print(cmd)
    dev.write(cmd)
    raw_response = dev.readline()
    response = raw_response.split(',')
    print([x for x in raw_response.split(',') if x])
    return "error code: " + response[0]


class reg:
    def __init__(self, name, index, bits):
        self.name = name
        self.index = index
        self.bits = bits

    def read_cmd(self):
        if ((self.index & 0x7) is 0 and (self.bits & 0x7) is 0):
            return read_byte(self.index/8, self.bits/8)
        else:
            return read_bits(self.index, self.bits)

    def write_cmd(self, data):
        if ((self.index & 0x7) is 0 and (self.bits & 0x7) is 0):
            return write_bytes(self.index/8, self.bits/8, data)
        else:
            return write_bits(self.index, self.bits, data)


sn = reg("sn", 0, 12*8)
fw_rev = reg("fw_rev", 12*8, 4*8)
build_time = reg("build_time", 16*8, 8*8)
i2c_addr_1 = reg("i2c_addr_1", 32*8 + 6*8, 2*8)
i2c_addr_2 = reg("i2c_addr_2", 32*8 + 8*8, 2*8)
i2c_clk_stretch_delay = reg("i2c_clk_stretch_delay", 32*8 + 6*8, 2*8)
i2c_gen_call = reg("i2c_gen_call", 32*8 + 1*1, 1)
i2c_clk_stretch_dis = reg("i2c_clk_stretch_dis", 32*8 + 2*1, 1)
i2c_10_bit_addr = reg("i2c_10_bit_addr", 32*8 + 0*1, 1)
i2c_disable = reg("i2c_disable", 32*8 + 3*1, 1)
