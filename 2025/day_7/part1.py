#!/usr/bin/python3

import sys, re

def read_input(filename):
    ret = []
    fp = open(filename, 'r')
    n = None
    for line in fp:
        ret.append(list(line.strip("\n")))
        if n is None:
            n = len(ret[-1])
        elif n != len(ret[-1]):
            print("Different line lengths")

    print("Read %d lines each with %s length" % (len(ret), n))
    return ret

def count_splits(layout):
    splits = 0

    for i in range(len(layout)-1):
        for j in range(len(layout[i])):
            if layout[i][j] in ['S', '|']:
                if layout[i+1][j] == '.':
                    layout[i+1][j] = '|'
                elif layout[i+1][j] == '^':
                    splits += 1
                    if j > 0 and layout[i+1][j-1] == '.':
                        layout[i+1][j-1] = '|'
                    if j < len(layout[i])-1 and layout[i+1][j+1] == '.':
                        layout[i+1][j+1] = '|'

    #for l in layout:
    #    print(''.join(l))

    return splits

print(count_splits(read_input(sys.argv[1])))

