#!/usr/bin/python3

import sys

def read_input(filename):
    shapes = {'list' : []}
    problems = []

    in_shape = True
    shape_def = None

    fp = open(filename, 'r')
    for line in fp:
        if in_shape:
            if shape_def is None:
                p = line.strip().split(":")
                if len(p[1]) == 0:
                    if int(p[0]) != len(shapes['list']):
                        print("Bad shape number")
                    shape_def = []
                else:
                    in_shape = False
            else:
                if len(line.strip()) == 0:
                    if len(shape_def) != 3:
                        print("Invalid shape height")
                    shapes['list'].append({'number' : len(shapes['list']), 'def' : shape_def})
                    shape_def = None
                else:
                    if len(line.strip()) != 3:
                        print("Invalid shape width")
                    shape_def.append(line.strip())
        
        if not in_shape:
            if len(line) > 0:
                p = line.strip().split(":")
                size = p[0].split("x")
                num = p[1].strip().split(" ")

                problems.append({'id' : len(problems), 'width' : int(size[0]), 'height' : int(size[1]), 'num' : [int(x) for x in num]})


    shapes['btf'] = []
    shapes['mask'] = []
    for s in shapes['list']:
        count = 0
        btf = []
        for i in range(8):
            btf.append([0, 0, 0])

        for i in range(3):
            for j in range(3):
                if s['def'][i][j] == '#':
                    count += 1

                    btf[0][i] |= 1<<j
                    btf[1][j] |= 1<<(2-i)
                    btf[2][2-i] |= 1<<(2-j)
                    btf[3][2-j] |= 1<<i
                    btf[4][i] |= 1<<(2-j)
                    btf[5][2-j] |= 1<<(2-i)
                    btf[6][2-i] |= 1<<j
                    btf[7][j] |= 1<<i            

        mask = []
        for v in range(8):
            mask.append([0, 0, 0, 0, 0])

        s['btf_c'] = []
        s['mask_c'] = []

        for v in range(8):
            for i in range(3):
                add = btf[v][i] | (btf[v][i]<<1) | (btf[v][i]<<2)
                mask[v][i] |= add
                mask[v][i+1] |= add
                mask[v][i+2] |= add

            for i in range(3):
                mask[v][i+1] &= ~(btf[v][i]<<1)

            s['btf_c'].append(btf[v][0] | (btf[v][1]<<3) | (btf[v][2]<<6))
            s['mask_c'].append(mask[v][0] | (mask[v][1]<<5) | (mask[v][2]<<10) | (mask[v][3]<<15) | (mask[v][4]<<20))

            if s['btf_c'][-1] not in shapes['btf']:
                shapes['mask'].append({'number' : s['number'], 'variant' : v, 'btf' : s['btf_c'][-1], 'mask' : s['mask_c'][-1], 'bits' : btf[v]})
                shapes['btf'].append(s['btf_c'][-1])

        s['count'] = count
        s['btf'] = btf
        s['mask'] = mask

    # pre-calculate which shapes fit into a 3x3 square with bitmast as 0..511
    shapes['fit'] = [None] * 512
    for i in range(512):
        for j in range(len(shapes['btf'])):
            if shapes['btf'][j] & i == 0:
                if shapes['fit'][i] is None:
                    shapes['fit'][i] = []
                shapes['fit'][i].append(j)

    print("Read %d shapes and %d problems" % (len(shapes), len(problems)))
    return {'shapes' : shapes, 'problems' : problems}

def bitcount(x):
    count = 0
    while x != 0:
        count += 1
        x &= x-1
    return count

def score_board(problem,  board):
    # count crossing for each row
    # not a very good scoring algorithm, doesn't take into account length of gaps, or vertical gaps
    total = 0

    #print("score_board")
    for row in board[1:-1]:
        #print(total, row)
        prev = None
        for i in range(problem['width']):
            next = (row >> (i+1)) & 1
            if prev is not None:
                if prev != next:
                    total += 1 
            prev = next
    #print(" - %d changes" % total)
    return total

def place_shape(board, shape, x, y):
    board[y] |= shape['bits'][0] << x
    board[y+1] |= shape['bits'][1] << x
    board[y+2] |= shape['bits'][2] << x

def remove_shape(board, shape, x, y):
    board[y] &= ~(shape['bits'][0] << x)
    board[y+1] &= ~(shape['bits'][1] << x)
    board[y+2] &= ~(shape['bits'][2] << x)

# quicker but very crude way of finding the best fit, just return the first match
def try_place2(problem, shapes, board, x, y, commit, prefix, debug = False):
    grid = (7 & (board[y] >> x)) | ((7 & (board[y+1] >> x))<<3) | ((7 & (board[y+2] >> x))<<6)
    candidates = shapes['fit'][grid]
    if candidates is None:
        return None
    for c in candidates:
        if problem['num'][shapes['mask'][c]['number']] > 0:    
            return {'shape' : c, 'score' : 0, 'x' : x, 'y' : y}
    return None

# slower method to try and pick the best option by maximising number of boarding tiles
def try_place(problem, shapes, board, x, y, commit, prefix, debug = False):
    if debug:
        print("%stry_place: %d %d" % (prefix, x, y))

    grid = (7 & (board[y] >> x)) | ((7 & (board[y+1] >> x))<<3) | ((7 & (board[y+2] >> x))<<6)
    candidates = shapes['fit'][grid]

    if debug:
        print("Grid: {:09b}".format(grid))
        print("Candidates: ", candidates)

    if candidates is None:
        return None

    mx = x-1
    my = y-1

    best = None
    mask = (31 & (board[my] >> mx)) | ((31 & (board[my+1] >> mx))<<5) | ((31 & (board[my+2] >> mx))<<10) | ((31 & (board[my+3] >> mx))<<15) | ((31 & (board[my+4] >> mx))<<20)
    for c in candidates:
        if problem['num'][shapes['mask'][c]['number']] > 0:
            count = bitcount(shapes['mask'][c]['mask'] & mask)

            place_shape(board, shapes['mask'][c], x, y)
            sb = score_board(problem, board)
            remove_shape(board, shapes['mask'][c], x, y)

            if best is None or count - sb > best['score']:
                #print(" - new best for ", c, count, sb)
                best = {'shape' : c, 'score' : count - sb, 'x' : x, 'y' : y}

            # need to discount count when we create spaces that can't be filled
            # could count gaps?
            # or do look ahead in this region with remaining tiles? - or is that likely better done at the higher level?

    if best is None:
        if debug:
            print("All candidates excluded")
        return None

    if debug:
        print("Best fit is %s" % best)

    return best

def print_board(problem, board, prefix):
    bfmt = "%s{:0%db}" % (prefix, problem['width']+2)
    for b in board:
        print(bfmt.format(b))
    
def place_tiles(problem, shapes, board, prefix, debug):
    if problem['max_steps'] <= 0:
        return False
    problem['max_steps'] -= 1

    options = []
    for x in range(1, problem['width']-1):
        for y in range(1, problem['height']-1):
            res = try_place2(problem, shapes, board, x, y, False, prefix, debug = False)
            if res is not None:
                options.append(res)

    options.sort(key = lambda x : x['score'], reverse = True)
    if len(options) == 0:
        return False

    for opt in options:
        place_shape(board, shapes['mask'][opt['shape']], opt['x'], opt['y'])
        problem['num'][shapes['mask'][opt['shape']]['number']] -= 1

        if debug:
            print("%sPicked %s" % (prefix, opt))
            print_board(problem, board, prefix)
            print("%s%s" % (prefix, problem['num']))

        if sum(problem['num']) == 0:
            return True

        if place_tiles(problem, shapes, board, "   %s" % prefix, debug) == True:
            return True
        
        remove_shape(board, shapes['mask'][opt['shape']], opt['x'], opt['y'])
        problem['num'][shapes['mask'][opt['shape']]['number']] += 1

    return False

def try_solve_problem(shapes, problem, debug = False):
    if debug:
        for s in shapes['list']:
            print(s['number'], s['def'], s['count'])
            for v in range(8):
                print(" -> %d %d %d" % (v, s['btf_c'][v], s['mask_c'][v]))
                print("{:09b}".format(s['btf_c'][v]))
                for n in s['btf'][v]:
                    print("{:03b}".format(n))
                print("{:025b}".format(s['mask_c'][v]))
                for n in s['mask'][v]:
                    print("{:05b}".format(n))

    if False and debug:
        print(shapes['btf'])
        for i in range(len(shapes['mask'])):
            print(i, shapes['btf'][i])
            print(shapes['mask'][i])

        #print(shapes['fit'])
        print(problem['width'], problem['height'], problem['num'])

    total = 0
    for i in range(len(shapes['list'])):
        total += shapes['list'][i]['count'] * problem['num'][i]

    print("%d: Total to fit: %d Space: %d Fill rate: %g" % (problem['id'], total, problem['width'] * problem['height'], total / (problem['width'] * problem['height'])))
    problem['max_steps'] = 1000

    if total > problem['width'] * problem['height']:
        return False

    board = []
    board.append((1<<(problem['width']+2))-1)
    for i in range(problem['height']):
        board.append(((((1<<(problem['width']+1))-1) ^ ((1<<problem['width'])-1))<<1) | 1)
    board.append((1<<(problem['width']+2))-1)

    if debug:
        print_board(problem, board, "")

    place_tiles(problem, shapes, board, "", debug)

    if sum(problem['num']) == 0:
        print("Found solution, %d steps remaining" % problem['max_steps'])
        print_board(problem, board, "")
        return True

    print("No solution found")
    return False

def count_solutions(p, num = None):
    count = 0

    if num is None:
        for problem in p['problems']:
            count += try_solve_problem(p['shapes'], problem)
    else:
        count += try_solve_problem(p['shapes'], p['problems'][num], debug = True)

    return count

print(count_solutions(read_input(sys.argv[1]), None if len(sys.argv) == 2 else int(sys.argv[2])))

# create an 8x8 region as a 64bit int, around an area of the space you want to fill - ideally one with some shapes there
# each shape/rotation/flip inside this 8x8 region would be a single int, so 6 shapes is down to 48ish numbers
# saves having to do three comparisons to see if a shape fits.
#  - although potentially just doing three comparisons (or five with border matching) might just be simplier than maintaining tiles
# could you create a tiled version of your space, with a bunch of 8x8 tiles that overlap, and maintain them while placing shapes?
# each shape number may partially overlap with neighbouring tiles?
#  - maybe that's a reason to construct your tile dynamically to find the best fit in that area, and sod trying to update a fixed set of tiles
#
# ideally want to place shapes to minimise gaps - how to find out gaps?  single 0 bits surrounded by 1s?  both horizontally & vertically
# (in 8x8 tile that's adjacent bits and then bits that are stride 8 adjacent)
# or for each shape construct the shape that's the next bit out from the border, and maximise the overlap between this border and existing blocks
#
# for each shape calculate the 9 bit (3x3) value, move around a 3x3 window and use a lookup table to work out which shapes are candidates
# this is 512 lookup table to lists of candidates (around 6 x 8 = 48 potential shapes)
# then do the 5x5 window over the existing board and match with the shadows of all the candidates to identify the best match
#
# opposite approach:
# try and make mega-shapes which combine multiple shapes leaving minimal/low unfilled squares
# use these mega-shapes to fill up the large recangtles, with fewer shapes needed the exhaustive search is easier
# gaps around the edge would need to be filled with smaller (the original?) shapes
#
# cache solutions
# create directory which represents a set of shapes
# inside directory create files which represent a problem (size & number of each shape) to contain things like whether a solution(s) was found,
# and a representation of those solutions