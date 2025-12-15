#!/usr/bin/python3

import sys, math, copy
import linear

# 16451 too low
# 16515 too high

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

def bitcount(n):
    c = 0
    while n > 0:
        n &= n-1
        c += 1
    return c

def gen_values(t, pre, max_values, pos, neg, sum):
    irange = [max(0, sum), max_values[pos]]
    if irange[1] - sum > max_values[neg]:
        irange[1] = max_values[neg] + sum
    
    for k in range(irange[0], irange[1]+1):
        t.append(pre + [k, k - sum])

def find_min(p):
    print("Finding min for problem:", p)

    if len(p['buttons']) == 0:
        print("No buttons left, no next move")
        return None

    for i in range(p['values']):
        if len(p['rev'][i]) == 0 and p['power'][i] > 0:
            print("No buttons left to generate power[%d] = %d" % (i, p['power'][i]))
            return None

    max_values = []
    for i in range(len(p['buttons'])):
        max_val = None
        for j in p['buttons'][i]:
            if max_val is None or p['power'][j] < max_val:
                max_val = p['power'][j]

        max_values.append(max_val)

    print("max_values:", max_values)

    bfs = []
    min_count = None
    for i in range(len(p['rev'])):
        bf = 0
        for j in p['rev'][i]:
            if max_values[j] > 0:
                bf |= (1 << j)
        bfs.append(bf)

    print("bitfields:", bfs)

    backup = None
    for i in range(len(p['rev'])):
        if bitcount(bfs[i]) == 1:
            min_count = [1, i]
            break
        elif bitcount(bfs[i]) == 2:
            if backup is None or p['power'][i] < p['power'][backup]:
                backup = i
        for j in range(i):
            if bfs[i] != 0 and bfs[j] != 0:
                nbf = bfs[i] ^ bfs[j]
                c = bitcount(nbf)
                pe = abs(p['power'][i]-p['power'][j])
                print("Candidate: %d (%d,%d)->%d pd = %d" % (c, i, j, nbf, pe))
                if c > 0 and (min_count is None or c < min_count[0] or (c == min_count[0] and pe < min_count[1])):
                    if bitcount(bfs[i]) > bitcount(bfs[j]):
                        min_count = [c, pe, i, j]
                    else:
                        min_count = [c, pe, j, i]

    ret = {}

    print("min_count", min_count)
    if min_count is None:
        print("No next move found")
        return None

    if len(min_count) == 4 and min_count[0] < 5:
        i = min_count[2]
        j = min_count[3]
        ipos = []
        ineg = []
        for k in range(len(p['buttons'])):
            if k in p['rev'][i] and k not in p['rev'][j] and max_values[k] > 0:
                ipos.append(k)
            elif k in p['rev'][j] and k not in p['rev'][i] and max_values[k] > 0:
                ineg.append(k)
        sum = p['power'][i]-p['power'][j]

        print("ipos", ipos, "ineg", ineg, "sum", sum)

        if min_count[0] == 1:
            if len(ipos) == 0:
                ipos.append(ineg[0])
                sum = -sum
            if sum < 0:
                print("Odd button press count")
                return None
            else:
                ret['buttons'] = [ipos[0]]
                ret['values'] = [[sum]]
        elif min_count[0] == 2:
            if len(ipos) == 1 and len(ineg) == 1:
                ret['buttons'] = [ipos[0], ineg[0]]
                ret['values'] = []
                gen_values(ret['values'], [], max_values, ipos[0], ineg[0], sum)
            else:
                irange = [sum - min(sum, max_values[ipos[1]]), min(sum, max_values[ipos[0]])]
                ret['buttons'] = [ipos[0], ipos[1]]
                ret['values'] = []
                for k in range(irange[0], irange[1]+1):
                    ret['values'].append([k, sum - k])
        elif min_count[0] == 3:
            if len(ipos) != 2 or len(ineg) != 1:
                print("Error case:", ipos, ineg)
                return None
            ret['buttons'] = ipos + ineg
            ret['values'] = []
            for i in range(max_values[ipos[0]]+1):
                gen_values(ret['values'], [i], max_values, ipos[1], ineg[0], sum-i)
        else:
            if len(ipos) != 2 or len(ineg) != 2:
                print("Error case:", min_count)
                return None
            ret['buttons'] = ipos + ineg
            ret['values'] = []

            for k in range(max_values[ipos[0]]+1):
                for m in range(max_values[ineg[0]]+1):
                    gen_values(ret['values'], [k, m], max_values, ipos[1], ineg[1], sum - k + m)       
    elif len(min_count) == 2 and min_count[0] == 1:
        for j in range(len(p['buttons'])):
            if (1<<j) == bfs[min_count[1]]:
                ret['buttons'] = [j]
                break
        ret['values'] = [[p['power'][i]]]
    elif backup is not None:
        print("Backup")
        print(p['rev'][backup], p['power'][backup])

        ipos = p['rev'][backup].copy()
        sum = p['power'][backup]
        irange = [sum - min(sum, max_values[ipos[1]]), min(sum, max_values[ipos[0]])]

        ret['buttons'] = ipos
        ret['values'] = []
        for k in range(irange[0], irange[1]+1):
            ret['values'].append([k, sum - k])
    else:
        print("Error case:", min_count)
        ret = None 
               
    return ret

def replay_history(p, history):
    test = [0] * p['values']
    for b in history:
        for ti in b[1]:
            test[ti] += b[0]

    return test


def apply_trial(p, trial, presses, history):   
    if len(trial['values']) < 50:
        print(presses, trial)
    else:
        print("%s Buttons:%s Values:%d count: %s ..." % (presses, trial['buttons'], len(trial['values']), trial['values'][0:25]))

    print("History", history)
    test = replay_history(p, history)
    print("Replay:", test)
    for i in range(len(test)):
        test[i] += p['power'][i]
    print("Sum:", test)


    np = {'values' : p['values'], 'buttons' : [], 'rev' : [], 'power' : p['power'].copy()}
    for i in range(np['values']):
        np['rev'].append([])
    for i in range(len(p['buttons'])):
        if i not in trial['buttons']:
            np['buttons'].append(p['buttons'][i])
            for j in p['buttons'][i]:
                np['rev'][j].append(len(np['buttons'])-1)
    
    best_solution = None

    for v in trial['values']:
        new_presses = 0
        skip = False
        for i in range(len(trial['buttons'])):
            new_presses += v[i]
            if v[i] > 0:
                history.append([v[i], p['buttons'][trial['buttons'][i]].copy()])
            for j in p['buttons'][trial['buttons'][i]]:
                np['power'][j] -= v[i]
                if np['power'][j] < 0:
                    print("After pressing %s %d times, got negative power[%d] = %d" % (p['buttons'][trial['buttons'][i]], v[i], j, np['power'][j]))
                    skip = True

        if not skip:
            print("Applied buttons %s with reps %s (new_presses %d)" % (trial['buttons'], v, new_presses)) 

            if sorted(np['power'])[-1] == 0:
                print("Zero max power, we're all done in %d" % (presses + new_presses))
                print("History:", history)
                if best_solution is None or best_solution[0] > presses + new_presses:
                    best_solution = [presses + new_presses, history.copy()]
            else:
                t = find_min(np)
                print("Got next move:", t)
                if t is not None and 'buttons' in t:
                    solution = apply_trial(np, t, presses + new_presses, history)
                    if solution is not None and (best_solution is None or best_solution[0] > solution[0]):
                        best_solution = solution
                else:
                    print("History:", history)
                    print("Ending recursion")

        for i in range(len(trial['buttons'])):
            if v[i] > 0:
                history.pop(-1)
            for j in p['buttons'][trial['buttons'][i]]:
                np['power'][j] += v[i]

    return best_solution
    
def sum_presses(problems):
    total = 0
    # 38 is the hard one
    for i in range(38, len(problems)):
        p = problems[i]
        t = find_min(p)
        c = None
        history = []
        if t is not None and 'buttons' in t:
            c = apply_trial(p, t, 0, history)
            if c is not None:
                total += c[0]

                test = replay_history(p, c[1])
                found_mistake = False
                for j in range(p['values']):
                    if test[j] != p['power'][j]:
                        found_mistake = True 

                if found_mistake:
                    print("Error: mistake in solution for problem ", i)
                    print(p['power'])
                    print(test)
                    print(p)
                    print(c)
                    c = "Error"

        else:
            print("Can't solve")

        print("finished problem %d, presses %s, total now %d" % (i, c, total))
        print("\n\n\n")
        break

    return total

def direct_solve(problems):
    total = 0
    just = None
    for i in range(0 if just is None else just, len(problems)):
        p = problems[i]

        spec = []
        for j in range(p['values']):
            c = [0] * len(p['buttons'])
            for k in p['rev'][j]:
                c[k] = 1
            spec.append({'coef' : c, 'result' : p['power'][j]})

        print(i, p)

        lin = linear.Linear(spec, debug = False if just is None else True)
        lin.make_diagonal()
        lin.reduce_rows()

        v = lin.spot_value()
        while v is not None:
            v = lin.spot_value()

        print(lin)

        if len(lin.equations) == 0:
            presses = 0
            for s in lin.solutions:
                presses += s[1]

            print("Problem %d in %d presses" % (i, presses))
            total += 1
        else:
            print(lin.min_set)
            if len(lin.min_set['set']) == 2:
                values = []
                b = list(lin.min_set['set'].keys())
                for j in range(20):
                    ov = (lin.min_set['result'] - (j * lin.min_set['set'][b[0]])) / lin.min_set['set'][b[1]]
                    if ov >= 0 and ov <= 20 and round(ov) - ov < 0.00001:
                        print(j, ov)
                        spec = []
                        c = [0] * len(lin.equations[0]['coef'])
                        c[b[0]] = 1
                        spec.append({'coef' : c, 'result' : j})
                        c = [0] * len(lin.equations[0]['coef'])
                        c[b[1]] = 1
                        spec.append({'coef' : c, 'result' : ov})
                        nlin = lin.copy()
                        nlin.add_equations(spec)
 
                        nlin.make_diagonal()
                        nlin.reduce_rows()
                        v = nlin.spot_value()
                        while v is not None:
                            v = nlin.spot_value()

                        print(nlin)
                        if len(nlin.equations) == 0:
                            presses = 0
                            for s in lin.solutions:
                                presses += s[1]
                            print("Problem %d found speculative solution with %d presses" % (i, presses))

        

        if just is not None:
            break

    return total


#print(sum_presses(read_input(sys.argv[1])))
print(direct_solve(read_input(sys.argv[1])))