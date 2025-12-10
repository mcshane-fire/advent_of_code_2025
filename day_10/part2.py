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

def fill_n_from_s(n, s, ret, sln, left, i, max_values):
    if i == len(s)-1:
        if left > max_values[s[i]]:
            return
        sln[i] = left
        ret.append(sln.copy())
    else:
        for p in range(min(left+1, max_values[s[i]]+1)):
            sln[i] = p
            fill_n_from_s(n, s, ret, sln, left-p, i+1, max_values)

def make_n_from_s(n, s, max_values):
    ret = []
    sln = [0] * len(s)
    fill_n_from_s(n, s, ret, sln, n, 0, max_values)
    return ret

def make_up_one_power(p, power, cnt):
    if p['count'] > 10000:
        return None



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

def bitcount(n):
    c = 0
    while n > 0:
        n &= n-1
        c += 1
    return c

def find_min(p):
    print("Finding min for problem:", p)
    max_values = []
    for i in range(len(p['buttons'])):
        max_val = None
        for j in p['buttons'][i]:
            if max_val is None or p['power'][j] < max_val:
                max_val = p['power'][j]

        max_values.append(max_val)

    best = None
    for i in range(p['values']):
        if p['power'][i] > 0:
            combinations = count_n_from_s(p['power'][i], len(p['rev'][i]))
            if best is None or combinations < best['num']:
                best = {'target' : i, 'num' : combinations}   

    bfs = []
    min_count = None
    for i in range(len(p['rev'])):
        bf = 0
        for j in p['rev'][i]:
            bf |= (1 << j)
        bfs.append(bf)

    print("bitfields:", bfs)

    for i in range(len(p['rev'])):
        for j in range(i):
            nbf = bfs[i] ^ bfs[j]
            c = bitcount(nbf)
            if c > 0 and (min_count is None or c < min_count[0]):
                min_count = [c, i, j]

    ret = {}

    print("Best", best)

    i = min_count[1]
    j = min_count[2]
    if min_count[0] == 2:
        ipos = None
        ineg = None
        for k in range(p['values']):
            if k in p['rev'][i] and k not in p['rev'][j]:
                ipos = k
            elif k in p['rev'][j] and k not in p['rev'][i]:
                ineg = k
        sum = p['power'][i]-p['power'][j]

        irange = [max(0, sum), max_values[ipos]]
        if irange[1] - sum > max_values[ineg]:
            irange[1] = max_values[ineg] + sum
        
        ret['buttons'] = [ipos, ineg]
        ret['values'] = []
        for k in range(irange[0], irange[1]+1):
            ret['values'].append([k, k - sum])

        print("Combo", min_count, len(ret['values']))
    else:
        print("Error case:", min_count)
        ret = None 

    if ret is None or best['num'] < len(ret['values']):
        return best
    else:
        return ret

def apply_trial(p, trial):
    print(trial)
    np = {'values' : p['values'], 'buttons' : [], 'rev' : [], 'power' : p['power'].copy()}
    for i in range(np['values']):
        np['rev'].append([])
    for i in range(len(p['buttons'])):
        if i not in trial['buttons']:
            np['buttons'].append(p['buttons'][i])
            for j in p['buttons'][i]:
                np['rev'][j].append(i)
    for v in trial['values']:
        for i in p['buttons'][trial['buttons'][0]]:
            np['power'][i] -= v[0]
        for i in p['buttons'][trial['buttons'][1]]:
            np['power'][i] -= v[1]

        print("Applied buttons") 
        t = find_min(np)
        print("Got next move:", t)
        if t is not None and 'buttons' in t:
            apply_trial(np, t)  
        else:
            print("Ending recursion")

        for i in p['buttons'][trial['buttons'][0]]:
            np['power'][i] += v[0]
        for i in p['buttons'][trial['buttons'][1]]:
            np['power'][i] += v[1]
    
         

def sum_presses(problems):
    total = 0
    for i in range(0, len(problems)):
        p = problems[i]
        t = find_min(p)
        if t is not None and 'buttons' in t:
            apply_trial(p, t)
        else:
            print("Can't solve")

        #presses = make_up_one_power(p, p['power'], 0)
        #print(presses, p['count'])
        #if presses is not None:
        #    total += presses
        break

    return total


sum_presses(read_input(sys.argv[1]))