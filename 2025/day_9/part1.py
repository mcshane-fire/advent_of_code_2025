#!/usr/bin/python3

import sys

def read_input(filename):
    ret = []
    fp = open(filename, 'r')
    for line in fp:
        p = line.strip().split(",")
        if len(p) != 2:
            print("Malformed line")
            continue
        ret.append([int(p[0]), int(p[1])])

    print("Read %d locations" % len(ret))
    return ret

def find_biggest(pos):
    best = 0
    for i in range(1, len(pos)):
        for j in range(i):
            size = (abs(pos[i][0] - pos[j][0])+1) * (abs(pos[i][1] - pos[j][1])+1)
            if size > best:
                best = size
    return best


print(find_biggest(read_input(sys.argv[1])))

