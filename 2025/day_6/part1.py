#!/usr/bin/python3

import sys, re

def read_input(filename):
    sums = []
    fp = open(filename, 'r')
    for line in fp:
        p = re.split(' +', line.strip())
        if len(sums) == 0:
            for n in p:
                sums.append({'numbers' : [int(n)], 'operator' : None})
        else:
            if len(p) != len(sums):
                print("Different number of values")
                continue

            for i in range(len(p)):
                if p[i] in ['+', '*']:
                    if sums[i]['operator'] is not None:
                        print("Two operators in column %d", i)
                        continue
                    sums[i]['operator'] = p[i]
                else:
                    sums[i]['numbers'].append(int(p[i]))

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

