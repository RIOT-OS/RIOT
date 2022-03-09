/*
 * Copyright (C) 2021 J. David Ibáñez <jdavid.ibp@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief    Tests the Arduino-SDI-12 package
 *
 * @author   J. David Ibáñez <jdavid.ibp@gmail.com>
 *
 * @}
 */

#include <fmt.h>
#include <SDI12.h>

#ifndef SDI12_DATA_PIN
#define SDI12_DATA_PIN 13
#endif

SDI12 sdi12(SDI12_DATA_PIN);

void sendCommand(const char *cmd)
{
    print_str("Send: ");
    print_str(cmd);
    print_str("\n");
    sdi12.clearBuffer();
    sdi12.sendCommand(cmd);
    delay(300);
}

void readResponse(char *buffer)
{
    int i = 0;
    while (sdi12.available()) {
        char c = sdi12.read();
        if (c == '\0') {
            continue;
        }

        buffer[i++] = c;
    }
    buffer[i] = '\0';

    print_str("Recv: ");
    print_str(buffer);

    // Responses from SDI-12 end by \r\n
    if (buffer[0] == '\0') {
        print_str("\n");
    }
}

int main(void)
{
    char cmd[10];
    char out[50];

    print_str("Testing the Arduino-SDI-12 package\n");

    sdi12.begin();
    delay(500);  // allow things to settle

    while (1) {
        print_str("\n");

        // Address query command (?!)
        sendCommand("?!");
        readResponse(out);
        if (strlen(out) == 0) {
            continue;
        }

        // For the rest of the loop the first command char will always be the
        // device address
        cmd[0] = out[0];

        // Identification command (aI!)
        cmd[1] = 'I';
        cmd[2] = '!';
        cmd[3] = '\0';
        sendCommand(cmd);
        readResponse(out);

        // Start measurement (aM!)
        cmd[1] = 'M';
        sendCommand(cmd);
        readResponse(out);  // atttn

        // Wait ttt seconds
        unsigned int ttt;
        ttt = (out[1] - '0') * 100 + (out[2] - '0') * 10 + (out[3] - '0');
        delay(ttt * 1000);

        // Data command (aD0!)
        cmd[1] = 'D';
        cmd[2] = '0';
        cmd[3] = '!';
        cmd[4] = '\0';
        sendCommand(cmd);
        readResponse(out);

        // Repeat in 5 seconds
        delay(5000);
    }
}
