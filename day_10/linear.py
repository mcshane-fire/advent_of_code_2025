#!/usr/bin/python3

class Linear:
    def __init__(self, equations, debug = False):
        self.equations = equations
        self.solutions = []
        self.debug = debug
        self.min_set = None

    def __str__(self):
        ret = "Linear: %d equations, %d unknowns\n" % (len(self.equations), 0 if len(self.equations) == 0 else len(self.equations[0]['coef']))
        min_set = None
        for i in self.equations:
            ret += "   [ "
            pos = {}
            for j in range(len(i['coef'])):
                ret += " %3s " % i['coef'][j]
                if not self.zero(i['coef'][j]):
                    pos[j] = i['coef'][j]
            if min_set is None or len(pos) < len(min_set['set']):
                min_set = {'set' : pos, 'result' : i['result']}
            ret += " ] -> %s\n" % (i['result'])
        ret += "Solutions: %s\n" % self.solutions
        ret += "Min set: %s\n" % min_set

        self.min_set = min_set

        return ret

    def add_equations(self, equations):
        self.equations += equations

    def copy(self):
        neq = []
        for eq in self.equations:
            neq.append({'coef' : eq['coef'].copy(), 'result' : eq['result']})

        return Linear(neq)

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
                            print("Impossible solution - similar rows %d & %d get non-similar result" % (i, j))
                            print(self)
                            return

        for i in sorted(list(rem), reverse = True):
            self.equations.pop(i)

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
            #print(self)
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
                break

        if found is not None:
            self.solutions.append([found, sln])
            for i in self.equations:
                i['result'] -= (i['coef'].pop(found) * sln)
                if self.zero(i['result'] - round(i['result'])):
                    i['result'] = round(i['result'])

            self.simplify()
            return found

        return None
                
if __name__ == "__main__":
    print("linear library")

    linear = Linear([{'coef' : [0,1,0,1,0,0], 'result' : 2},
                    {'coef' : [0,1,0,1,1,1], 'result' : 16},
                    {'coef' : [0,0,1,1,1,1], 'result' : 21},
                    {'coef' : [0,1,1,0,0,1], 'result' : 21},
                    {'coef' : [1,1,0,1,1,0], 'result' : 7}])

    print(linear)

    linear.make_diagonal()  

    print(linear)