#!/usr/bin/env python2

import gzip
import random
import sys

sizeOfA = 10 * 1000
sizeOfB = 20

print sys.argv
if len(sys.argv) == 5:
	sizeOfA = int(sys.argv[3])
	sizeOfB = int(sys.argv[4])

# read all words
lines = [line.strip() for line in gzip.open(sys.argv[1])]

# get A lines
A = random.sample(lines, sizeOfA + sizeOfB)

# get B from the first sieOfB element
B = A[:sizeOfB]
A = A[sizeOfB:]

SetsFile = open(sys.argv[2], 'w')
SetsFile.write('const int lenA = ' + str(sizeOfA) + ';\n')
SetsFile.write('const char* A[' + str(sizeOfA) + '] = {')
SetsFile.writelines(",".join('"' + x + '"\n' for x in A))
SetsFile.write('};\n')

SetsFile.write('const int lenB = ' + str(sizeOfB) + ';\n')
SetsFile.write('const char* B[' + str(sizeOfB) + '] = {')
SetsFile.writelines(",".join('"' + x + '"\n' for x in B))
SetsFile.write('};\n')

print("sets.h: sizeOfA = " + str(len(A) + len(B)) +  " generated...")
