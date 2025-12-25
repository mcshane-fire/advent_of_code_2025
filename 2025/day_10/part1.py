#!/usr/bin/python3

import sys

def read_input(filename):
    ret = []
    fp = open(filename, 'r')
    for line in fp:
        p = line.strip().split(" ")
        li = p[0].strip("[]")

        problem = {'target' : 0, 'states' : 2**len(li), 'buttons' : []}       

        for i in range(len(li)):
            if li[i] == '#':
                problem['target'] += 1<<i

        for i in range(1,len(p)-1):
            but = 0
            for j in p[i].strip("()").split(","):
                but += 1<<int(j)

            problem['buttons'].append(but)

        ret.append(problem)

    print("Read %d problems" % len(ret))
    return ret

def next_move(states, n, dir, buttons):
    nset = []
    for s in n:
        for b in buttons:
            ns = s ^ b
            if states[ns] is None:
                states[ns] = states[s] + dir
                nset.append(ns)
            elif states[ns]*dir < 0:
                return (((states[s] - states[ns]) * dir) - 1, None)

    return (None, nset)

def sum_presses(problems):
    total = 0
    for p in problems:
        if p['target'] == 0:
            continue

        states = [None] * p['states']
        states[0] = 1
        states[p['target']] = -1

        inc = [0]
        dec = [p['target']]

        while True:
            (r, inc) = next_move(states, inc, 1, p['buttons'])
            if r is not None:
                total += r
                break
            (r, dec) = next_move(states, dec, -1, p['buttons'])
            if r is not None:
                total += r
                break

    return total

        

print(sum_presses(read_input(sys.argv[1])))

