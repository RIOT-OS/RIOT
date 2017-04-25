#!/usr/bin/env python3

import json, fileinput

lines = []
for line in fileinput.input():
    lines.append(line.rstrip())

print(json.dumps(lines, indent=4))
