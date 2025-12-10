#!/usr/bin/python3

import sys, math

def read_input(filename):
    ret = []
    fp = open(filename, 'r')
    for line in fp:
        p = line.strip().split(" ")
        li = p[0].strip("[]")

        problem = {'values' : len(li), 'buttons' : [], 'rev' : [], 'power' : p[-1].strip("{}").split(",")}
        
        for i in range(len(problem['power'])):
            problem['power'][i] = int(problem['power'][i])
            problem['rev'].append([])

        for i in range(1,len(p)-1):
            but = []
            for j in p[i].strip("()").split(","):
                but.append(int(j))
                problem['rev'][int(j)].append(i-1)

            problem['buttons'].append(but)

        ret.append(problem)

    print("Read %d problems" % len(ret))

    return ret

def count_n_from_s(n, s):
    return math.factorial(s+n-1)/(math.factorial(n)*math.factorial(s-1))

def fill_n_from_s(n, s, ret, sln, left, i):
    if i == s-1:
        sln[i] = left
        ret.append(sln.copy())
    else:
        for p in range(left+1):
            sln[i] = p
            fill_n_from_s(n, s, ret, sln, left-p, i+1)

def make_n_from_s(n, s):
    ret = []
    sln = [0] * s
    fill_n_from_s(n, s, ret, sln, n, 0)
    return ret

def make_up_one_power(p, power, cnt):
    if p['count'] > 10000:
        return None

    best = None
    for i in range(p['values']):
        if power[i] > 0:
            combinations = count_n_from_s(power[i], len(p['rev'][i]))
            if best is None or combinations < best[1]:
                best = (i, combinations)

    if cnt == 0:
        print("%d: %s %s" % (cnt, power, best))

    if best is None:
        #print("No best")
        #print(power)
        #print("-----------")
        return cnt

    #print(best)
    slns = make_n_from_s(power[best[0]], len(p['rev'][best[0]]))
    #print(slns)
    lowest = None
    for s in slns:
        npower = power.copy()
        fail = False
        for i in range(len(s)):
            for j in p['buttons'][p['rev'][best[0]][i]]:
                npower[j] -= s[i]
                if npower[j] < 0:
                    # invalid combination
                    fail = True
                    break
            if fail:
                break
        
        if not fail:
            p['count'] += 1
            #if cnt == 0:
                #print("%d->%d: Pressing combinations %s from rev %s" % (cnt, cnt+power[best[0]], s, p['rev'][best[0]]))
                #print(npower)
            nc = make_up_one_power(p, npower, cnt + power[best[0]])

            if nc is not None and (lowest is None or nc < lowest):
                lowest = nc

    return lowest

def sum_presses(problems):

    total = 0
    for i in range(0, len(problems)):
        p = problems[i]
        p['count'] = 0
        print(p)
        presses = make_up_one_power(p, p['power'], 0)
        print(presses, p['count'])
        if presses is not None:
            total += presses

    return total


print(sum_presses(read_input(sys.argv[1])))