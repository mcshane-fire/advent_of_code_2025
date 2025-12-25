#!/usr/bin/python3

import sys

def read_input(filename):
    ret = []
    fp = open(filename, 'r')
    for line in fp:
        if line[0] not in ['L', 'R']:
            print("Bad line '%s'" % line)
            return ret
        mv = int(line[1:])
        if line[0] == 'L':
            mv = -mv

        ret.append(mv)

    print("Read %d moves" % len(ret))
    return ret

def apply_moves(moves, start):
    count = 0
    for m in moves:
        mv = 1
        if m < 0:
            mv = -1
            m = -m

        for j in range(m):
            start += mv
            if start < 0:
                start += 100
            if start >= 100:
                start = 0
            if start == 0:
                count += 1

    return count

print(apply_moves(read_input(sys.argv[1]), 50))

