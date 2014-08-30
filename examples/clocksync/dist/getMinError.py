#!/usr/bin/env python2
# -*- coding: utf-8 -*-

# Copyright (C) 2014 Daniel Jentsch <d.jentsch@fu-berlin.de>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301 USA

import os, re, math, sys, getopt
import time, calendar, collections

def print_help():
  supportedProtocols = ["gtsp", "ftsp", "pulsesync"]
  print "getMinError.py [-m error [-p]] <path-to-log-files>"
  print "-m error: maximal allowed error in µs"
  print "    the program exits with 1 if a no error was found below error"

def main(argv):
  logdirs, allowedError = getOptions(argv)

  minErrors = dict()

  for logdir in logdirs:
    beacons = getBeacons(logdir)
    minError = getMinDiff(beacons.values())
    minErrors[logdir] = minError

  for log in minErrors:
    # It's implicitly clear what log is meant,because there is only one
    name = "" if len(minErrors) == 1 else log + ": "
    state = "[ ok ]" if minErrors[log] <= allowedError else "[fail]"
    print state + " " + name + str(minErrors[log]) + "µs"


  failed = any(errors > allowedError for errors in minErrors.values())
  if failed:
    sys.exit(1)
  else:
    sys.exit(0)

# Returns the smallest difference between all tuple members
def getMinDiff(tuples):
  return min([abs(m - n) for m, n in tuples])

# returns a dictionary from beacon sender and its ID to min and max estimated
# global time
def getBeacons(logdir):
  result = dict()
  logpats = r'(\S+)\s+(\S+).*\#et, a: (\S+), c: (\S+), tl: (\S+), tg: (\S+)'
  logpat = re.compile(logpats)

  for root, dirs, files in os.walk(logdir):
    for file in files:
      fileName = os.path.join(root, file)
      with open(fileName) as openFile:
        for line in openFile:
          match = logpat.match(line)
          if match:
            tuple = match.groups()
            sender = int(tuple[2])
            id = int(tuple[3])
            globalTime = int(tuple[5])

            mi, ma = result.get((sender, id), (sys.maxint, -1))
            mi = min(mi, globalTime)
            ma = max(ma, globalTime)
            result[sender, id] = mi, ma

  return result

# Returns commandline arguments: logdirs and maximal allowd error
def getOptions(args):
  allowedError = sys.maxint

  try:
    opts, logdirs = getopt.getopt(args,"hm:",["max="])
  except getopt.GetoptError:
    print_help()
    sys.exit(2)
  for opt, arg in opts:
    if opt == '-h':
      print_help()
      sys.exit()
    elif opt in ("-m", "--max"):
      allowedError = int(arg)

  if not logdirs:
    print("No log folders")
    print_help()
    sys.exit(2)

  return logdirs, allowedError


if __name__ == "__main__":
  main(sys.argv[1:])
