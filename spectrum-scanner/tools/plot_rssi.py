#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Copyright (C) 2017  Eistec AB
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import sys
import re
import time
import logging
import argparse
from serial import Serial
import numpy as np
import matplotlib.pylab as plt


class rssi_plot(object):

    def plot_rssi(self, port):
        self.count = 0
        self.dt = 0.5
        self.tlen = 120
        # Generate mesh for plotting
        Y, X = np.mgrid[slice(0 - .5, 26 + 1.5, 1), slice(0 - self.dt / 2, self.tlen + 1 - self.dt / 2, self.dt)]
        Z = np.zeros_like(X)
        # X and Y are bounds, so Z should be the value *inside* those bounds.
        # Therefore, remove the last value from the Z array.
        Z = Z[:, :-1]
        logging.debug("Creating figure")
        plt.figure()
        pcm = plt.pcolormesh(X, Y, Z, vmin=-128, vmax=0, cmap=plt.cm.get_cmap('jet'))
        plt.xlim([0, self.tlen])
        plt.ylim([0, 26])
        plt.colorbar(label="Measured signal level [dB]")
        plt.ylabel("Channel number")
        plt.xlabel("Time [s]")
        plt.ion()
        logging.debug("Show plot window")
        plt.show()
        ch_min = 26
        ch_max = 0
        last_update = time.time()

        logging.info("Begin collecting data from serial port")
        while True:
            line = port.readline().rstrip()

            pkt_data = re.match(r"\[([-+]?\d+),\s*([-+]?\d+),\s*([-+]?\d+)\]\s*(.*)", line.decode(errors='replace'))
            if pkt_data:
                now = time.time()
                try:
                    iface_id = int(pkt_data.group(1))
                    timestamp = int(pkt_data.group(2))
                    count = int(pkt_data.group(3))
                    tidx = int(timestamp / (self.dt * 1000000)) % (Z.shape[1])
                except ValueError:
                    continue
                logging.debug("data: tidx=%d if=%d cnt=%d t=%d", tidx, iface_id, count, timestamp)
                raw = pkt_data.group(4)
                resize = False
                for ch_ed in raw.split(","):
                    try:
                        pair = ch_ed.split(":")
                        ch = int(pair[0])
                        ed = float(pair[1])
                        if ch < ch_min:
                            ch_min = ch
                            resize = True
                        if ch > ch_max:
                            ch_max = ch
                            resize = True
                        Z[ch, tidx] = ed
                    except (ValueError, IndexError):
                        continue
                if resize:
                    logging.debug("resize: %d %d" % (ch_min, ch_max))
                    plt.ylim([ch_min - .5, ch_max + .5])
                if now > last_update + 1:
                    last_update = now
                    pcm.set_array(Z.ravel())
                    pcm.autoscale()
                    pcm.changed()
            plt.pause(0.01)


def main(argv):
    loglevels = [logging.CRITICAL, logging.ERROR, logging.WARN, logging.INFO, logging.DEBUG]
    parser = argparse.ArgumentParser(argv)
    parser.add_argument('-v', '--verbosity', type=int, default=4,
                        help='set logging verbosity, 1=CRITICAL, 5=DEBUG')
    parser.add_argument('tty',
                        help='Serial port device file name')
    parser.add_argument('-b', '--baudrate', default=115200, type=int,
                        help='Serial port baudrate')
    args = parser.parse_args()
    # logging setup
    logging.basicConfig(level=loglevels[args.verbosity-1])

    # open serial port
    try:
        logging.debug("Open serial port %s, baud=%d", args.tty, args.baudrate)
        port = Serial(args.tty, args.baudrate, dsrdtr=0, rtscts=0, timeout=0.3)
    except IOError:
        logging.critical("error opening serial port", file=sys.stderr)
        sys.exit(2)

    try:
        app = rssi_plot()
        app.plot_rssi(port)
    except KeyboardInterrupt:
        port.close()
        sys.exit(2)


if __name__ == "__main__":
    main(sys.argv)
