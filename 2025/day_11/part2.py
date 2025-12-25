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
            all[source] = {'name' : source, 'paths' : [0,0,0], 'dest_met' : 0, 'destnames' : dest, 'src' : [], 'dest' : []}

    # link all sources and destinations
    if 'out' not in all:
        all['out'] = {'name' : 'out', 'paths' : [1,0,0], 'dest_met' : 0, 'destnames' : [], 'src' : [], 'dest' : []}

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

def count_paths(all):
    candidates = [all['out']]
    svr = all['svr']

    while svr['dest_met'] != len(svr['dest']):
        rem = []
        add = []
        for n in candidates:
            if n['dest_met'] == len(n['dest']):
                #print(" - Completed node %s, found %d paths" % (n['name'], n['paths']))

                rem.append(n)

                shift = 1 if n['name'] in ['dac', 'fft'] else 0
                for s in n['src']:
                    for i in range(3-shift):
                        s['paths'][i+shift] += n['paths'][i]
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

    return svr['paths'][2]

print(count_paths(read_input(sys.argv[1])))

