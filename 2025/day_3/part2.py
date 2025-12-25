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

def find_best_n(r, num):
    #print(r)

    res = 0
    used = 0

    for c in range(num):
        p = None
        for i in range(used, len(r)-num+c+1):
            if p is None or r[i] > r[p]:
                p = i
        res = (res * 10) + r[p]
        used = p+1
    
    #print(res)
    return res


def calculate_max(ranges):
    total = 0
    for r in ranges:
        total += find_best_n(r, 12)
    return total

print(calculate_max(read_input(sys.argv[1])))

