#!/usr/bin/python3

import sys

def read_input(filename):
    ranges = []
    ingredients = []
    fp = open(filename, 'r')
    done_ranges = False
    for line in fp:
        line = line.strip()
        if done_ranges:
            ingredients.append(int(line))
        else:
            if len(line) == 0:
                done_ranges = True
            else:
                p = line.split("-")
                ranges.append([int(p[0]), int(p[1])])
                if ranges[-1][1] < ranges[-1][0]:
                    print("Inverted range")

    print("Read %d ranges and %d ingredients" % (len(ranges), len(ingredients)))
    return {'ranges' : ranges, 'ingredients' : ingredients}

def number_fresh(ranges):
    ranges.sort(key = lambda x : x[0])

    count = 0
    cur = None
    for r in ranges:
        if cur is None or cur < r[0]:
            # previous range entirely lower than new range
            count += r[1] - r[0] + 1
            cur = r[1]
        else:
            if cur <= r[1]:
                # previous range overlapping with new range
                count += r[1] - cur
                cur = r[1]
            #else: previous range entirely covers new range, nothing to do

    return count

print(number_fresh(read_input(sys.argv[1])['ranges']))

