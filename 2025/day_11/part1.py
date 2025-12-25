#!/usr/bin/python3

import sys

def read_input(filename):
    all = {}
    fp = open(filename, 'r')
    for line in fp:
        p = line.strip().split(":")
        if len(p) == 2:
            source = p[0]
            dest = p[1].strip().split(" ")
            #print(source, dest)

            if source in all:
                print("source listed twice")
            all[source] = {'name' : source, 'paths' : 0, 'dest_met' : 0, 'destnames' : dest, 'src' : [], 'dest' : []}

    # link all sources and destinations
    if 'out' not in all:
        all['out'] = {'name' : 'out', 'paths' : 0, 'dest_met' : 0, 'destnames' : [], 'src' : [], 'dest' : []}

    for s in all:
        if len(all[s]['destnames']) == 0 and s != 'out':
            print("non-out node with no destinations")

        for d in all[s]['destnames']:
            if d not in all:
                print("destination %s missing" % d)

            all[s]['dest'].append(all[d])
            all[d]['src'].append(all[s])

    print("Read %d descriptions" % len(all))
    return all

def count_paths(all, start, end):
    candidates = [all[end]]
    start = all[start]

    candidates[0]['paths'] = 1

    while start['dest_met'] != len(start['dest']):
        rem = []
        add = []
        for n in candidates:
            if n['dest_met'] == len(n['dest']):
                #print(" - Completed node %s, found %d paths" % (n['name'], n['paths']))

                rem.append(n)

                for s in n['src']:
                    s['paths'] += n['paths']
                    s['dest_met'] += 1
                    if s['dest_met'] == 1:
                        add.append(s)


        for n in rem:
            candidates.remove(n)
        for n in add:
            candidates.append(n)

        #out = "Candidate list: "
        #for n in candidates:
        #    out += "%s, " % n['name']
        #print(out)

    return start['paths']

print(count_paths(read_input(sys.argv[1]), 'you' if len(sys.argv) == 2 else sys.argv[2], 'out' if len(sys.argv) == 2 else sys.argv[3]))

