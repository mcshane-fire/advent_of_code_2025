#!/usr/bin/python3

import sys

def read_input(filename):
    ret = []
    fp = open(filename, 'r')
    for line in fp:
        p = line.strip().split(",")
        if len(p) != 2:
            print("Malformed line")
            continue
        ret.append([int(p[0]), int(p[1])])

    print("Read %d locations" % len(ret))
    return ret

def find_biggest(pos):
    height = 0
    for i in pos:
        if i[1] > height:
            height = i[1]

    # assumes we can have a single pair of left/right edges for each row, each row as a single contiguous set of colour squares
    left = [None] * (height+1)
    right = [None] * (height+1)

    pos.append(pos[0])
    for i in range(len(pos)-1):
        min_x = min(pos[i][0], pos[i+1][0])
        max_x = max(pos[i][0], pos[i+1][0])
        min_y = min(pos[i][1], pos[i+1][1])
        max_y = max(pos[i][1], pos[i+1][1])

        if min_y == max_y:
            if left[min_y] is None or min_x < left[min_y]:
                left[min_y] = min_x
            if right[min_y] is None or max_x > right[min_y]:
                right[min_y] = max_x

        elif min_x == max_x:
            for j in range(min(pos[i][1], pos[i+1][1]), max(pos[i][1], pos[i+1][1])+1):
                if left[j] is None or min_x < left[j]:
                    left[j] = min_x
                if right[j] is None or min_x > right[j]:
                    right[j] = min_x

    best = 0
    for i in range(1, len(pos)):
        for j in range(i):
            size = (abs(pos[i][0] - pos[j][0])+1) * (abs(pos[i][1] - pos[j][1])+1)

            if size > best:
                min_x = min(pos[i][0], pos[j][0])
                max_x = max(pos[i][0], pos[j][0])
                min_y = min(pos[i][1], pos[j][1])
                max_y = max(pos[i][1], pos[j][1])
                fail = False

                while min_y < max_y:
                    if min_x < left[min_y] or right[min_y] < max_x or min_x < left[max_y] or right[max_y] < max_x:
                        fail = True
                        break
                    min_y += 1
                    max_y -= 1

                if fail is False:
                    best = size

    return best

print(find_biggest(read_input(sys.argv[1])))

