#!/usr/bin/python3

import sys
import sorting_tree

def read_input(filename):
    ret = []
    fp = open(filename, 'r')
    for line in fp:
        p = line.strip().split(",")
        if len(p) != 3:
            print("Malformed line")
            continue
        ret.append([int(p[0]), int(p[1]), int(p[2])])

    print("Read %d locations" % len(ret))
    return ret

def join_nearest(loc, join):
    tree = sorting_tree.SortingTree()
    which = {0 : 0}
    circuits = {0 : [0]}

    for i in range(1, len(loc)):
        which[i] = i
        circuits[i] = [i]
        for j in range(i):
            d = (loc[i][0] - loc[j][0])**2 + (loc[i][1] - loc[j][1])**2 + (loc[i][2] - loc[j][2])**2
            tree.insert(d, [i, j])

    (score,b) = tree.greater_equal(0)

    for i in range(join):
        if which[b[0]] != which[b[1]]:
            circuits[which[b[0]]] += circuits[which[b[1]]]
            todrop = which[b[1]]
            for j in circuits[which[b[1]]]:
                which[j] = which[b[0]]
            del circuits[todrop]
        (score, b) = tree.next()

    sl = list(circuits.keys())
    sl.sort(key = lambda x : len(circuits[x]), reverse = True)
    return len(circuits[sl[0]]) * len(circuits[sl[1]]) * len(circuits[sl[2]])

print(join_nearest(read_input(sys.argv[1]), int(sys.argv[2])))

