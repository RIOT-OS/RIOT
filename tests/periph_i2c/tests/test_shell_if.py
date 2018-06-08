import serial
import logging


class TestShellParams:
    cmd = ''
    result = ''
    msg = ''
    data = ''


class TestShellIf:
    __COMMAND = 'Command: '
    __SUCCESS = 'Success: '
    __ERROR = 'Error: '
    __TIMEOUT = 'Timeout: '
    __RESULT_SUCCESS = 'Success'
    __RESULT_ERROR = 'Error'
    __RESULT_TIMEOUT = 'Timeout'

    def __init__(self, port='/dev/ttyACM0', baud=115200):

        self.connect(port, baud)

    def connect(self, port, baud=115200, timeout=1):
        logging.debug("Connecting to " + port)
        self.__dev = serial.Serial(port, baud, timeout=timeout)
        if(self.__dev.isOpen() is False):
            self.__dev.open()

    def disconnect(self):
        logging.debug("Disconnecting")
        if (isinstance(self.__dev, serial.Serial)):
            if (self.__dev.isOpen()):
                self.__dev.close()

    def send_cmd(self, send_cmd):
        cmd_info = TestShellParams()
        logging.debug("Sending: " + send_cmd)
        self.__dev.write(send_cmd + '\n')
        response = 'no_exit'
        while (response != ''):
            response = self.__dev.readline()
            logging.debug("Response: " + response.replace('\n', ''))
            if (self.__COMMAND in response):
                cmd_info.msg = response.replace(self.__COMMAND, '')
                cmd_info.cmd = cmd_info.msg.replace('\n', '')

            if (self.__SUCCESS in response):
                cmd_info.msg = response.replace(self.__SUCCESS, '')
                cmd_info.msg = cmd_info.msg.replace('\n', '')
                if ('[' in cmd_info.msg) and (']' in cmd_info.msg):
                    data = cmd_info.msg
                    data = data[data.find("[")+1:data.find("]")]
                    data = data.split(', ')
                    cmd_info.data = list()
                    for value in data:
                        try:
                            cmd_info.data.append(int(value, 0))
                        except ValueError:
                            cmd_info.data.append(value)
                    logging.debug(cmd_info.data)
                cmd_info.result = self.__RESULT_SUCCESS
                break

            if (self.__ERROR in response):
                cmd_info.msg = response.replace(self.__ERROR, '')
                cmd_info.msg = cmd_info.msg.replace('\n', '')
                cmd_info.result = self.__RESULT_ERROR
                break

        if (response == ''):
            cmd_info.result = self.__RESULT_TIMEOUT
            logging.debug(self.__RESULT_TIMEOUT)
        return cmd_info

    def __del__(self):
        self.disconnect()
