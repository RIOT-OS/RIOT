#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# SPDX-FileCopyrightText: 2017 Eistec AB
# SPDX-License-Identifier: LGPL-3.0-or-later

import sys
import re
import logging
import argparse
import serial
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation


class SpectrumEmitter(object):
    def __init__(self, port):
        self.port = port

    def data_gen(self):
        logging.info("Begin collecting data from serial port")
        while True:
            # Read one line from the spectrum device
            line = self.port.readline().rstrip()
            pkt_data = re.match(
                r"\[([-+]?\d+),\s*([-+]?\d+),\s*([-+]?\d+)\]\s*(.*)",
                line.decode(errors="replace"),
            )
            if pkt_data:
                ed = {}
                try:
                    iface_id = int(pkt_data.group(1))
                    timestamp = int(pkt_data.group(2))
                    count = int(pkt_data.group(3))
                except ValueError:
                    # Incorrect data received, probably UART noise or debugging
                    # messages from the device, not much else we can do other
                    # than try again with the next line
                    continue
                logging.debug("data: if=%d cnt=%d t=%d", iface_id, count, timestamp)
                raw = pkt_data.group(4)
                for ch_ed in raw.split(","):
                    try:
                        pair = ch_ed.split(":")
                        ch = int(pair[0])
                        ed[ch] = float(pair[1])
                    except (ValueError, IndexError):
                        continue
                yield ed


class RSSIPlot(object):
    def __init__(self, ax, *args, tlen=120, dt=0.5, nchannels=27):
        self.ax = ax
        self.count = 0
        self.dt = dt
        self.tlen = tlen
        # Generate mesh for plotting, this creates a grid of nchannel rows and
        # (tlen / dt) columns
        self.Y, self.X = np.mgrid[
            slice(0 - 0.5, nchannels + 0.5, 1),
            slice(-self.tlen - self.dt / 2, 0 + 1 - self.dt / 2, self.dt),
        ]
        Z = np.zeros_like(self.X)
        # X and Y are the bounds, so Z should be the value *inside* those bounds.
        # Therefore, remove the last row and column from the Z array.
        self.Z = Z[:-1, :-1]
        self.pcm = self.ax.pcolormesh(
            self.X, self.Y, self.Z, vmin=-100, vmax=-20, cmap=plt.cm.get_cmap("jet")
        )
        self.ax.get_figure().colorbar(self.pcm, label="Measured signal level [dB]")
        self.ax.set_ylabel("Channel number")
        self.ax.set_xlabel("Time [s]")
        self.ch_min = nchannels
        self.ch_max = 0

    def update(self, ed):
        resize = False
        for ch in ed.keys():
            if ch < self.ch_min:
                self.ch_min = ch
                resize = True
            if ch > self.ch_max:
                self.ch_max = ch
                resize = True
        col = np.zeros((self.Z.shape[0], 1))
        for ch in ed.keys():
            col[ch, 0] = ed[ch]
        self.Z = np.hstack((self.Z[:, 1:], col))
        if resize:
            self.ax.set_ylim([self.ch_min - 0.5, self.ch_max + 0.5])
            self.ax.set_yticks(range(self.ch_min, self.ch_max + 1))
        self.pcm.set_array(self.Z.ravel())
        return (self.pcm,)


def main(argv):
    loglevels = [
        logging.CRITICAL,
        logging.ERROR,
        logging.WARN,
        logging.INFO,
        logging.DEBUG,
    ]
    parser = argparse.ArgumentParser(argv)
    parser.add_argument(
        "-v",
        "--verbosity",
        type=int,
        default=4,
        help="set logging verbosity, 1=CRITICAL, 5=DEBUG",
    )
    parser.add_argument("tty", help="Serial port device file name")
    parser.add_argument(
        "-b", "--baudrate", default=115200, type=int, help="Serial port baudrate"
    )
    args = parser.parse_args()
    # logging setup
    logging.basicConfig(level=loglevels[args.verbosity - 1])

    # open serial port
    try:
        logging.debug("Open serial port %s, baud=%d", args.tty, args.baudrate)
        port = serial.Serial(
            port=args.tty, baudrate=9600, dsrdtr=0, rtscts=0, timeout=0.3
        )
        # This baudrate reconfiguration is necessary for certain USB to serial
        # adapters, the Linux cdc_acm driver will keep repeating stale buffer
        # contents otherwise. No idea about the cause, but this fixes the symptom.
        port.baudrate = args.baudrate
    except IOError:
        logging.critical("error opening serial port", file=sys.stderr)
        sys.exit(2)

    try:
        logging.debug("Creating figure")
        fig, ax = plt.subplots()
        graph = RSSIPlot(ax)
        emitter = SpectrumEmitter(port)
        animation.FuncAnimation(
            fig, graph.update, emitter.data_gen, interval=10, blit=True
        )
        plt.show()
    except KeyboardInterrupt:
        port.close()
        sys.exit(2)


if __name__ == "__main__":
    main(sys.argv)
