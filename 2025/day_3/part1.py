#!/usr/bin/python3

import sys

def read_input(filename):
    ret = []
    fp = open(filename, 'r')
    for line in fp:
        v = []
        for i in range(len(line.strip())):
            v.append(int(line[i]))

        ret.append(v)

    print("Read %d ranges" % len(ret))
    return ret

def find_best_two(r):
    #print(r)

    p1 = None
    for i in range(len(r)-1):
        if p1 is None or r[i] > r[p1]:
            p1 = i
    
    p2 = None
    for i in range(p1+1, len(r)):
        if p2 is None or r[i] > r[p2]:
            p2 = i

    return r[p1] * 10 + r[p2]


def calculate_max(ranges):
    total = 0
    for r in ranges:
        total += find_best_two(r)
    return total

print(calculate_max(read_input(sys.argv[1])))

