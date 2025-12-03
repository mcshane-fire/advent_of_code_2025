#!/usr/bin/python3

import sys

def read_input(filename):
    ret = []
    fp = open(filename, 'r')
    for line in fp:
        parts = line.split(",")
        for p in parts:
            ps = p.split("-")
            if len(ps) != 2:
                print("Bad line '%s'" % line)
                return ret
            start = int(ps[0])
            end = int(ps[1])
            ret.append([start, end])

    print("Read %d ranges" % len(ret))
    return ret

def find_invalid_ids(ranges):
    total = 0
    for r in ranges:
        ss = str(r[0])
        es = str(r[1])

        if len(ss) % 2 == 1:
            r[0] = int('1' + ('0' * len(ss)))
            ss = str(r[0])
        if len(es) % 2 == 1:
            r[1] = int('9' * (len(es)-1))
            es = str(r[1])

        if r[1] < r[0]:
            continue

        num = int(len(ss)/2)
        if len(es) != len(ss):
            print("Can't cope with different lengths")
            continue

        s = int(ss[0:num])
        e = int(es[0:num])

        for t in range(s, e+1):
            c = int(str(t) + str(t))
            if c >= r[0] and c <= r[1]:
                total += c

    return total

print(find_invalid_ids(read_input(sys.argv[1])))

