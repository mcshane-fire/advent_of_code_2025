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

    print("Read %d ranges and %d ingredients" % (len(ranges), len(ingredients)))
    return {'ranges' : ranges, 'ingredients' : ingredients}

def check_freshness(data):
    count = 0
    for i in data['ingredients']:
        for r in data['ranges']:
            if i >= r[0] and i <= r[1]:
                count += 1
                break
    return count

print(check_freshness(read_input(sys.argv[1])))

