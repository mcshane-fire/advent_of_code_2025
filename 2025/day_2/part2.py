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

def find_repeated(start, stop):
    total = 0
    ss = str(start)
    es = str(stop)
    sl = len(ss)

    if sl != len(es):
        print("Invalid call")
        return total

    found = []

    for num in range(1, int(sl/2)+1):
        if sl % num == 0:
            #print("Checking %d -> %d, len %d" % (start, stop, num))

            s = int(ss[0:num])
            e = int(es[0:num])

            mul = int(sl / num)

            for t in range(s, e+1):
                c = int(str(t) * mul)
                if c >= start and c <= stop and c not in found:
                    #print("Invalid ID: %d" % c)
                    total += c            
                    found.append(c)

    return total

def find_invalid_ids(ranges):
    total = 0
    for r in ranges:
        if r[1] < r[0]:
            print("Reverse range")
            return total

        ss = str(r[0])
        es = str(r[1])

        #print(r)

        if len(es) == len(ss):
            total += find_repeated(r[0], r[1])
        else:
            if len(es) > len(ss)+1:
                print("Can only copy with difference of one")
                continue

            total += find_repeated(r[0], int('9' * len(ss)))
            total += find_repeated(int('1' + ('0' * len(ss))), r[1])

    return total

print(find_invalid_ids(read_input(sys.argv[1])))

