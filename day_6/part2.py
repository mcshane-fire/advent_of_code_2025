#!/usr/bin/python3

import sys, re

def read_input(filename):
    sums = []
    fp = open(filename, 'r')
    lines = []
    for line in fp:
        lines.append(line.strip("\n"))

    s = {'numbers' : [], 'operator' : None}
    i = len(lines[0])-1
    while i >= 0:
        num = 0
        for j in range(len(lines)-1):
            if lines[j][i] != ' ':
                num = (num * 10) + int(lines[j][i])
        s['numbers'].append(num)
        if lines[-1][i] in ['*', '+']:
            s['operator'] = lines[-1][i]
            sums.append(s)
            s = {'numbers' : [], 'operator' : None}
            i -= 2
        else:
            i -= 1

    print("Read %d sums each with %d values" % (len(sums), len(sums[0]['numbers'])))
    return sums

def calculate_sums(sums):
    total = 0
    for s in sums:
        start = 0 if s['operator'] == '+' else 1
        for n in s['numbers']:
            start = start + n if s['operator'] == '+' else start * n

        total += start
    
    return total

print(calculate_sums(read_input(sys.argv[1])))

