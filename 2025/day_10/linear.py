#!/usr/bin/python3

from multiprocessing.util import debug


class Linear:
    def __init__(self, equations, debug = False):
        self.equations = equations
        self.solutions = []
        self.debug = debug
        self.min_set = None
        self.valid_range = []
        for i in range(len(equations[0]['coef'])):
            self.valid_range.append([0, None])  # min, max

    def __str__(self):
        ret = "Linear: %d equations, %d unknowns\n" % (len(self.equations), 0 if len(self.equations) == 0 else len(self.equations[0]['coef']))
        for i in self.equations:
            ret += "   [ "
            for j in range(len(i['coef'])):
                ret += " %3s " % i['coef'][j]
            ret += " ] -> %s\n" % (i['result'])
        ret += "Solutions: %s\n" % self.solutions
        ret += "Min set: %s\n" % self.min_set
        ret += "Valid ranges: %s\n" % self.valid_range

        return ret

    def add_equations(self, equations):
        self.equations += equations

    def copy(self):
        neq = []
        for eq in self.equations:
            neq.append({'coef' : eq['coef'].copy(), 'result' : eq['result']})

        nlin = Linear(neq, debug = self.debug)
        nlin.valid_range = [vr.copy() for vr in self.valid_range]
        return nlin

    def zero(self, num):
        if num > -0.00001 and num < 0.00001:
            return True
        return False  

    def round(self, num):
        if self.zero(num - round(num)):
            return round(num)
        return num

    def simplify(self):
        # remove zero equations
        rem = set()
        for i in range(len(self.equations)):
            all_zero = True
            for j in self.equations[i]['coef']:
                if not self.zero(j):
                    all_zero = False
                    break

            if all_zero:
                if self.zero(self.equations[i]['result']):
                    rem.add(i)
                else:
                    self.min_set = None
                    if self.debug:
                        print("Impossible solution - zero row %d gets non zero result %s" % (i, self.equations[i]['result']))
                        print(self)
                    return

        for i in sorted(list(rem), reverse = True):
            self.equations.pop(i)

        # remove duplicate/multiple equations
        rem = set()
        for i in range(len(self.equations)):
            for j in range(i+1, len(self.equations)):
                mult = None
                for k in range(len(self.equations[i]['coef'])):
                    if not self.zero(self.equations[i]['coef'][k]) and not self.zero(self.equations[j]['coef'][k]) and mult is None:
                        mult = self.equations[j]['coef'][k] / self.equations[i]['coef'][k]
                        break
                
                if mult is not None:
                    same = True
                    for k in range(len(self.equations[i]['coef'])):
                        if self.zero(self.equations[j]['coef'][k] - self.equations[i]['coef'][k] * mult):
                            same = False
                            break
                    
                    if same:
                        if self.zero(self.equations[j]['result'] - self.equations[i]['result'] * mult):
                            rem.add(j)
                        else:
                            self.min_set = None
                            if self.debug:
                                print("Impossible solution - similar rows %d & %d get non-similar result" % (i, j))
                                print(self)
                            return

        for i in sorted(list(rem), reverse = True):
            self.equations.pop(i)

        # see if maximum values for each parameter have changed
        if len(self.equations) > 0:
            for i in range(len(self.equations[0]['coef'])):
                min_val = None
                max_val = None
                for j in self.equations:
                    # work out whether each coefficient and the result have the same sign
                    signs = [0, 0]
                    for k in j['coef']:
                        if not self.zero(k):
                            signs[0 if k > 0 else 1] += 1
                    signs[0 if j['result'] > 0 else 1] += 1

                    if signs[0] == 0 or signs[1] == 0:
                        for k in range(len(j['coef'])):
                            if not self.zero(j['coef'][k]):
                                val = j['result'] / j['coef'][k]
                                if self.zero(val - round(val)):
                                    val = round(val)
                                if self.valid_range[k][1] is None or val < self.valid_range[k][1]:
                                    self.valid_range[k][1] = round(val)

        # recalculate min_set
        self.min_set = None
        for i in self.equations:
            pos = {}
            for j in range(len(i['coef'])):
                if not self.zero(i['coef'][j]):
                    pos[j] = i['coef'][j]
            if self.min_set is None or len(pos) < len(self.min_set['set']):
                self.min_set = {'set' : pos, 'result' : i['result']}

        if self.debug:
            print(self)

    # does i -= j * m, so that coef i[k] is zero
    def subtract_rows(self, i, j, k, mult = None):
        if mult is None:
            mult = self.equations[i]['coef'][k] / self.equations[j]['coef'][k]
        for c in range(len(self.equations[i]['coef'])):
            self.equations[i]['coef'][c] = self.round(self.equations[i]['coef'][c] - (self.equations[j]['coef'][c] * mult))

        self.equations[i]['result'] = self.round(self.equations[i]['result'] - (self.equations[j]['result'] * mult))

    def make_diagonal(self):
        i=0
        self.simplify()
        while i < min(len(self.equations), len(self.equations[0]['coef'])):
            if self.min_set is None:
                return
            if self.zero(self.equations[i]['coef'][i]):
                # find a lower row with non-zero coef
                best = None
                for j in range(i+1, len(self.equations)):
                    if not self.zero(self.equations[j]['coef'][i]) and (best is None or abs(self.equations[j]['coef'][i]) > abs(self.equations[best]['coef'][i])):
                        best = j

                if best is None:
                    # no non-zero coefficients in this column, skip to next
                    i += 1
                    continue

                repl = self.equations.pop(best)
                self.equations.insert(i, repl)

            # now subtract multiples of this row from lower down rows
            for j in range(i+1, len(self.equations)):
                self.subtract_rows(j, i, i)

            self.simplify()
            i += 1

    def reduce_rows(self):
        for i in range(len(self.equations)):
            for j in range(i+1, len(self.equations)):
                both_zero = True
                match = {}
                extra = 0
                for k in range(len(self.equations[i]['coef'])):
                    if both_zero:
                        if self.zero(self.equations[i]['coef'][k]) and self.zero(self.equations[j]['coef'][k]):
                            continue

                        if not self.zero(self.equations[i]['coef'][k]):
                            both_zero = False
                            continue

                        # no match
                        break
                    else:
                        if not self.zero(self.equations[j]['coef'][k]):
                            if not self.zero(self.equations[i]['coef'][k]):                                
                                mult = self.round(self.equations[i]['coef'][k] / self.equations[j]['coef'][k])
                                if mult not in match:
                                    match[mult] = 0
                                match[mult] += 1
                            else:
                                extra += 1

                if len(match) > 0:
                    bc = sorted(match.keys(), key = lambda x : match[x], reverse = True)
                    if match[bc[0]] > extra:
                        self.subtract_rows(i, j, None, mult = bc[0])
        self.simplify()

    def spot_value(self):
        found = None
        sln = None
        for i in self.equations:
            for j in range(len(i['coef'])):
                if i['coef'][j] != 0:
                    if found is not None:
                        found = None
                        break
                    
                    found = j

            if found is not None:
                sln = i['result'] / i['coef'][found]
                if self.zero(sln - round(sln)):
                    sln = round(sln)
                else:
                    if self.debug:
                        print("Error:non-integer solution")
                    return None

                if sln < self.valid_range[found][0] or sln > self.valid_range[found][1]:
                    if self.debug:
                        print("Error:out of range")
                    return None

                break      

        if found is not None:
            self.solutions.append([found, sln])
            for i in self.equations:
                i['result'] -= (i['coef'].pop(found) * sln)
                if self.zero(i['result'] - round(i['result'])):
                    i['result'] = round(i['result'])

            self.valid_range.pop(found)

            self.simplify()
            return 1

        return 0
    
    def solve(self):
        self.make_diagonal()
        self.reduce_rows()

        while True:
            sv = self.spot_value()            
            if sv is None:
                return None
            if sv == 0:
                break

        presses = 0
        for s in self.solutions:
            presses += s[1]

        if len(self.equations) == 0:
            if self.debug:
                print("Solved with %d presses" % presses)
            return presses

        if self.min_set is None:    
            if self.debug:
                print("min_set is None, failed to solve")
            return None

        b = list(self.min_set['set'].keys())

        # could try and find the variable with the smallest range here, would need to check candidates with same set as b[0]
        if self.debug:
            print("Doing speculative solve on variable %s" % (b[0]))
        best_solution = None
        for i in range(self.valid_range[b[0]][0], self.valid_range[b[0]][1] + 1):
            if self.debug:
                print("Trying speculative b%d = %d" % (b[0], i))
            c = [0] * len(self.equations[0]['coef'])
            c[b[0]] = 1
            spec = [{'coef' : c, 'result' : i}]
            nlin = self.copy()
            nlin.add_equations(spec)
            if self.debug:
                print(nlin)

            npresses = nlin.solve()
            if npresses is not None:
                if self.debug:
                    print("Guess b%d = %d found speculative solution with %d presses" % (b[0], i, npresses))
                if best_solution is None or npresses < best_solution[0]:
                    best_solution = [npresses, nlin.solutions]
                    if self.debug:
                        print("New best solution with b%d = %d" % (b[0], i))
                   
        if best_solution is not None:
            self.solutions += best_solution[1]
            if self.debug:
                print("Returning combined best solution: %s with %d presses" % (self.solutions, best_solution[0] + presses))
            return presses + best_solution[0]

        if self.debug:
            print("Failed to solve")
        return None
