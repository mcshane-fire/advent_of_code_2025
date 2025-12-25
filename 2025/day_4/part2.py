#!/usr/bin/python3

import sys

def read_input(filename):
    lines = []
    fp = open(filename, 'r')
    max = 0
    for line in fp:
        lines.append(line.strip())
        if len(lines[-1]) > max:
            max = len(lines[-1])

    ret = []
    ret.append([0] * (max+2))
    for line in lines:
        ll = [0] * (max+2)
        for i in range(len(line)):
            if line[i] == '@':
                ll[i+1] = 1
            elif line[i] != '.':
                print("Unexpected character '%s'" % line[i])

        ret.append(ll)

    ret.append([0] * (max+2))

    print("Read %d lines, width %d" % (len(lines), max))
    return ret

def count_remove_neighbours(arr, lim):
    count = 0

    while True:
        this_go = 0
        for i in range(1, len(arr)-1):
            for j in range(1, len(arr[0])-1):
                if arr[i][j] == 1:
                    total = arr[i-1][j-1] + arr[i-1][j] + arr[i-1][j+1] + arr[i][j-1] + arr[i][j+1] + arr[i+1][j-1] + arr[i+1][j] + arr[i+1][j+1]
                    if total < lim:
                        this_go += 1
                        arr[i][j] = 0
        if this_go == 0:
            break
        
        count += this_go

    return count

print(count_remove_neighbours(read_input(sys.argv[1]), 4))

