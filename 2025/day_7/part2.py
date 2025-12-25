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

def calculate_options(layout):
    n = len(layout[0])
    prev = [1] * n

    for i in range(len(layout)):
        options = [0] * n
        line = layout[-(i+1)]

        if i == len(layout)-1:
            return prev[line.index('S')]

        for j in range(len(line)):
            if line[j] == '^':
                if j > 0 and line[j-1] == '.':
                    options[j] += prev[j-1]
                if j < n-1 and line[j+1] == '.':
                    options[j] += prev[j+1]
            elif line[j] == '.':
                options[j] = prev[j]
            else:
                print("Unexpected character")
        
        prev = options

print(calculate_options(read_input(sys.argv[1])))

