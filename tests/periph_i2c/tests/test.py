import periph_i2c_if
import logging
import sys
import argparse

sys.stdout = open('file', 'w')

num_of_dev = 0
BPT_ADDR = 85


def kwexpect(val1, val2):
    err = True
    if (isinstance(val1, basestring) and isinstance(val2, basestring)):
        if (val1.lower() == val2.lower()):
            err = False
    elif (isinstance(val1, list)):
        err = False
        for x in range(0, len(val1)):
            if (val1[x] != val2[x]):
                err = True
    elif ((val1 == val2) or (val1 is val2)):
        err = False
    if err:
        print('ERROR')
        print(val1)
        print(val2)
        raise Exception('error: ' + val1 + ' != ' + val2)


class TEST:
    name = ""
    desc = ""
    notes = ""
    result = ""
    cmd_log = list()

    def __init__(self, name, desc):
        self.name = name
        self.desc = desc
        del self.cmd_log[:]


def init_test():
    global num_of_dev

    t = TEST('init test', 'Tests the initialization of the i2c periph')
    t.cmd_log.append(i2c.get_devs())
    kwexpect(t.cmd_log[-1].result, 'Success')
    num_of_dev = int(t.cmd_log[-1].data[0])

    for d_num in range(0, num_of_dev):
        t.cmd_log.append(i2c.assign_dev(d_num))
        kwexpect(t.cmd_log[-1].result, 'Success')

    t.cmd_log.append(i2c.assign_dev(num_of_dev))
    kwexpect(t.cmd_log[-1].result, 'Error')

    t.result = 'PASS'
    return t


def mutex_test():
    global num_of_dev

    t = TEST('mutex test', 'Tests mutex aquire/release functionality \
(not full)')
    t.notes += 'Cannot test since it can still be used without mutex\n'
    t.notes += 'Cannot test expected timeout due to no reset\n'
    t.notes += 'Do we need double release failure check?\n'
    t.notes += 'Does not check multiple busses acquired\n'
    for d_num in range(0, num_of_dev):
        t.cmd_log.append(i2c.assign_dev(d_num))
        kwexpect(t.cmd_log[-1].result, 'Success')

        t.cmd_log.append(i2c.release())

        t.cmd_log.append(i2c.acquire())
        kwexpect(t.cmd_log[-1].result, 'Success')

        if (d_num == 0):
            t.cmd_log.append(i2c.read_byte(85))
            kwexpect(t.cmd_log[-1].result, 'Success')

        t.cmd_log.append(i2c.release())
        kwexpect(t.cmd_log[-1].result, 'Success')

        # if (d_num == 0):
        # t.cmd_log.append(i2c.read_byte(85))
        # kwexpect(t.cmd_log[-1].result, 'Error')

        # t.cmd_log.append(i2c.acquire())
        # kwexpect(t.cmd_log[-1].result, 'Success')

        # t.cmd_log.append(i2c.acquire())
        # kwexpect(t.cmd_log[-1].result, 'Timeout')
        # Cannot test due to no reset
    t.result = 'PASS'
    return t


def read_test():

    t = TEST('read test', 'Tests the read functionality for default i2c bus \
(0) with the bpt')
    t.notes += 'Does not check flags\n'

    t.cmd_log.append(i2c.assign_dev(0))
    kwexpect(t.cmd_log[-1].result, 'Success')

    t.cmd_log.append(i2c.acquire())
    kwexpect(t.cmd_log[-1].result, 'Success')

    t.cmd_log.append(i2c.read_reg(85, 152))
    kwexpect(t.cmd_log[-1].result, 'Success')
    kwexpect(t.cmd_log[-1].data, [0])

    t.cmd_log.append(i2c.read_reg(86, 152))
    kwexpect(t.cmd_log[-1].result, 'Error')

    t.cmd_log.append(i2c.read_reg(85, 153))
    kwexpect(t.cmd_log[-1].result, 'Success')
    kwexpect(t.cmd_log[-1].data, [1])

    t.cmd_log.append(i2c.read_regs(85, 152, 1))
    kwexpect(t.cmd_log[-1].result, 'Success')
    kwexpect(t.cmd_log[-1].data, [0])

    t.cmd_log.append(i2c.read_regs(85, 153, 1))
    kwexpect(t.cmd_log[-1].result, 'Success')
    kwexpect(t.cmd_log[-1].data, [1])

    t.cmd_log.append(i2c.read_regs(85, 152, 2))
    kwexpect(t.cmd_log[-1].result, 'Success')
    kwexpect(t.cmd_log[-1].data, [0, 1])

    t.cmd_log.append(i2c.read_regs(85, 152, 3))
    kwexpect(t.cmd_log[-1].result, 'Success')
    kwexpect(t.cmd_log[-1].data, [0, 1, 2])

    t.cmd_log.append(i2c.read_regs(84, 152, 1))
    kwexpect(t.cmd_log[-1].result, 'Error')

    t.cmd_log.append(i2c.read_byte(85))
    kwexpect(t.cmd_log[-1].result, 'Success')
    kwexpect(t.cmd_log[-1].data, [0])

    t.cmd_log.append(i2c.read_byte(83))
    kwexpect(t.cmd_log[-1].result, 'Error')

    t.cmd_log.append(i2c.read_bytes(85, 1))
    kwexpect(t.cmd_log[-1].result, 'Success')
    kwexpect(t.cmd_log[-1].data, [0])

    t.cmd_log.append(i2c.read_byte(82, 1))
    kwexpect(t.cmd_log[-1].result, 'Error')

    t.cmd_log.append(i2c.read_bytes(85, 2))
    kwexpect(t.cmd_log[-1].result, 'Success')
    kwexpect(t.cmd_log[-1].data, [0, 1])

    t.cmd_log.append(i2c.read_bytes(85, 3))
    kwexpect(t.cmd_log[-1].result, 'Success')
    kwexpect(t.cmd_log[-1].data, [0, 1, 2])

    t.cmd_log.append(i2c.release())
    kwexpect(t.cmd_log[-1].result, 'Success')
    t.result = 'PASS'
    return t


def check_write_reg(addr, reg, val):
    cmd_list = list()
    cmd_list.append(i2c.write_reg(addr, reg, val[0]))
    kwexpect(cmd_list[-1].result, 'Success')

    cmd_list.append(i2c.read_reg(addr, reg))
    kwexpect(cmd_list[-1].result, 'Success')
    kwexpect(cmd_list[-1].data, val)
    return cmd_list


def check_write_regs(addr, reg, val):
    cmd_list = list()
    cmd_list.append(i2c.write_regs(addr, reg, val))
    kwexpect(cmd_list[-1].result, 'Success')

    cmd_list.append(i2c.read_regs(addr, reg, len(val)))
    kwexpect(cmd_list[-1].result, 'Success')
    kwexpect(cmd_list[-1].data, val)
    return cmd_list


def write_test():
    t = TEST('write test', 'Tests the write functionality for default i2c bus \
(0) with the bpt')
    t.notes += 'Does not check flags\n'

    t.cmd_log.append(i2c.assign_dev(0))
    kwexpect(t.cmd_log[-1].result, 'Success')

    t.cmd_log.append(i2c.acquire())
    kwexpect(t.cmd_log[-1].result, 'Success')

    t.cmd_log.extend(check_write_reg(85, 152, [42]))
    t.cmd_log.extend(check_write_reg(85, 152, [0]))

    t.cmd_log.extend(check_write_reg(85, 153, [41]))
    t.cmd_log.extend(check_write_reg(85, 153, [1]))

    t.cmd_log.append(i2c.write_reg(81, 152, 0))
    kwexpect(t.cmd_log[-1].result, 'Error')

    t.cmd_log.extend(check_write_regs(85, 152, [44]))
    t.cmd_log.extend(check_write_regs(85, 152, [0]))

    t.cmd_log.extend(check_write_regs(85, 153, [45, 46]))
    t.cmd_log.extend(check_write_regs(85, 153, [1, 2]))

    t.cmd_log.extend(check_write_regs(85, 152, [47, 48, 49]))
    t.cmd_log.extend(check_write_regs(85, 152, [0, 1, 2]))

    t.cmd_log.append(i2c.write_regs(81, 152, [0]))
    kwexpect(t.cmd_log[-1].result, 'Error')

    t.cmd_log.append(i2c.write_byte(85, 153))
    kwexpect(t.cmd_log[-1].result, 'Success')

    t.cmd_log.append(i2c.read_byte(85))
    kwexpect(t.cmd_log[-1].result, 'Success')
    kwexpect(t.cmd_log[-1].data, [1])

    t.cmd_log.append(i2c.write_byte(85, 152))
    kwexpect(t.cmd_log[-1].result, 'Success')

    t.cmd_log.append(i2c.read_byte(85))
    kwexpect(t.cmd_log[-1].result, 'Success')
    kwexpect(t.cmd_log[-1].data, [0])

    t.cmd_log.append(i2c.write_byte(80, 0))
    kwexpect(t.cmd_log[-1].result, 'Error')

    t.cmd_log.append(i2c.write_bytes(85, [153]))
    kwexpect(t.cmd_log[-1].result, 'Success')

    t.cmd_log.append(i2c.read_byte(85))
    kwexpect(t.cmd_log[-1].result, 'Success')
    kwexpect(t.cmd_log[-1].data, [1])

    t.cmd_log.append(i2c.write_bytes(85, [152, 50]))
    kwexpect(t.cmd_log[-1].result, 'Success')

    t.cmd_log.append(i2c.read_byte(85))
    kwexpect(t.cmd_log[-1].result, 'Success')
    kwexpect(t.cmd_log[-1].data, [50])

    t.cmd_log.append(i2c.write_bytes(85, [153, 51, 52]))
    kwexpect(t.cmd_log[-1].result, 'Success')

    t.cmd_log.append(i2c.read_bytes(85, 2))
    kwexpect(t.cmd_log[-1].result, 'Success')
    kwexpect(t.cmd_log[-1].data, [51, 52])

    t.cmd_log.append(i2c.write_bytes(85, [152, 0, 1, 2]))
    kwexpect(t.cmd_log[-1].result, 'Success')

    t.cmd_log.append(i2c.read_bytes(85, 3))
    kwexpect(t.cmd_log[-1].result, 'Success')
    kwexpect(t.cmd_log[-1].data, [0, 1, 2])

    t.cmd_log.append(i2c.write_bytes(81, [47, 48, 49]))
    kwexpect(t.cmd_log[-1].result, 'Error')

    t.cmd_log.append(i2c.release())
    kwexpect(t.cmd_log[-1].result, 'Success')
    t.result = 'PASS'
    return t


def read_flag_test():

    t = TEST('read test', 'Tests the read flag functionality')
    t.notes += 'Not a comprehesive test but tests a proper frame\n'

    t.cmd_log.append(i2c.assign_dev(0))
    kwexpect(t.cmd_log[-1].result, 'Success')

    t.cmd_log.append(i2c.acquire())
    kwexpect(t.cmd_log[-1].result, 'Success')

    t.cmd_log.append(i2c.read_reg(85, 152))
    kwexpect(t.cmd_log[-1].result, 'Success')
    kwexpect(t.cmd_log[-1].data, [0])

    t.cmd_log.append(i2c.read_bytes(85, 1, 4))
    kwexpect(t.cmd_log[-1].result, 'Success')
    kwexpect(t.cmd_log[-1].data, [0])

    t.cmd_log.append(i2c.read_bytes(85, 1, 12))
    kwexpect(t.cmd_log[-1].result, 'Success')
    kwexpect(t.cmd_log[-1].data, [1])

    t.cmd_log.append(i2c.read_bytes(85, 1, 8))
    kwexpect(t.cmd_log[-1].result, 'Success')
    kwexpect(t.cmd_log[-1].data, [2])

    t.cmd_log.append(i2c.read_bytes(85, 1, 8))
    kwexpect(t.cmd_log[-1].result, 'Error')

    t.cmd_log.append(i2c.read_bytes(85, 1, 12))
    kwexpect(t.cmd_log[-1].result, 'Error')

    # t.cmd_log.append(i2c.read_bytes(85, 1, 4))
    # kwexpect(t.cmd_log[-1].result, 'Success')
    # kwexpect(t.cmd_log[-1].data, [0])
    # t.cmd_log.append(i2c.read_bytes(85, 1, 4))
    # kwexpect(t.cmd_log[-1].result, 'Timeout')

    t.cmd_log.append(i2c.release())
    kwexpect(t.cmd_log[-1].result, 'Success')
    t.result = 'PASS'
    return t


def print_full_result(test):
    print('===================================================================\
=============')
    print('Name:\t\t' + test.name)
    print('Desc:\t\t' + test.desc)
    print('Result:\t\t' + test.result)
    print('Notes:\t\t' + test.notes)
    print('-------------------------------------------------------------------\
-------------')
    for cmds in test.cmd_log:
        print('Command:\t' + cmds.cmd)
        print('Message:\t' + cmds.msg)
        print('Data:\t\t[%s]' % ', '.join(map(str, cmds.data)))
        print('Result:\t\t' + cmds.result)
        print('')


def print_results(test_list):
    print('')
    print('===================================================================\
=============')
    for x in test_list:
        print('Name:\t\t' + x.name)
        print('Result:\t\t' + x.result + '\n')


# START THE TEST
parser = argparse.ArgumentParser()
parser.add_argument("--log", help='Set the logging level (DEBUG, INFO, WARN)')
args = parser.parse_args()

if args.log is not None:
    loglevel = args.log
    numeric_level = getattr(logging, loglevel.upper(), None)
    if not isinstance(numeric_level, int):
        raise ValueError('Invalid log level: %s' % loglevel)
    logging.basicConfig(level=loglevel)

i2c = periph_i2c_if.PeriphI2CIf()

print('Starting Test periph_i2c')
test_list = list()
test_list.append(init_test())
print_full_result(test_list[-1])
test_list.append(mutex_test())
print_full_result(test_list[-1])
test_list.append(read_test())
print_full_result(test_list[-1])
test_list.append(write_test())
print_full_result(test_list[-1])
test_list.append(read_flag_test())
print_full_result(test_list[-1])

print_results(test_list)
