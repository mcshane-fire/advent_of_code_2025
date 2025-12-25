#!/usr/bin/python

# class SortingTree
#
# constructor: no arguments
# methods:
#
# insert(value, data)
#    - adds new record, sorted by value with extra value data associated with record
#    - clears current record
#
# printTree()
#    - pretty print of tree, with included diagnostics for tree errors
#
# contains(value, data)
#    - checks whether tree contains record with given value and data
#    - if found, sets current record to that found
#    - returns boolean
#
# greater_equal(value)
#    - finds the record with the smallest value which is greater or equal to the value passed in
#    - if found, sets current record to that found
#    - returns None if no records are found
#    - returns (value, data) tuple of record found
#
# next()
#    - if current record is set, iterates upwards by value to the next record
#    - sets next record as current
#    - returns None if no more records exist
#    - returns (value, data) tuple of next record
#
# depth()
#    - returns depth of current record if set (root as depth=1)
#    - returns 0 if no current record set
#
# delete(value, data)
#    - deletes a record with the specified value and data
#    - return boolean if record was found & deleted

import sys, math

class TreeNode:
    def __init__(self, value, data, parent):
        self.value = value
        self.data = data
        self.parent = parent
        self.children = [None, None]
        self.count = 1
        self.test = 0

    def insert(self, value, data):
        parent = self
        rebalance = None
        best = 3
        
        while True:
            parent.count += 1

            # calculate potential for rebalancing
            score = 0
            swing = 0
            if parent.children[0] != None:
                score += parent.children[0].count
            if parent.children[1] != None:
                score -= parent.children[1].count
            if score < 0 and parent.children[1].children[0] != None:
                swing += parent.children[1].children[0].count
            if score > 0 and parent.children[0].children[1] != None:
                swing += parent.children[0].children[1].count

            if score < 0:
                score = -score
            score = score - swing

            if parent.test > 0:
                parent.test -= 1
                score = 0
                
            if score > best:
                rebalance = parent
                best = score

            # work out which way to go
            index = 1
            if value < parent.value:
                index = 0

            # insert or go down the tree
            if parent.children[index] == None:
                parent.children[index] = TreeNode(value, data, parent)
                break
            else:
                parent = parent.children[index]

        if rebalance != None:
            count_left = 0
            if rebalance.children[0] != None:
                count_left = rebalance.children[0].count
            count_right = 0
            if rebalance.children[1] != None:
                count_right = rebalance.children[1].count
            promote = 0
            if count_left < count_right:
                promote = 1

            #print "Rebalance %x/%x (promote %d, score %d)" % (rebalance.value, rebalance.data, promote, best)

            value = rebalance.value
            data = rebalance.data
            rescue = rebalance.children[1-promote]
            rebalance.value = rebalance.children[promote].value
            rebalance.data = rebalance.children[promote].data
            rebalance.children[1-promote] = rebalance.children[promote]
            rebalance.children[1-promote].value = value
            rebalance.children[1-promote].data = data
            rebalance.children[promote] = rebalance.children[1-promote].children[promote]
            if rebalance.children[promote] != None:
                rebalance.children[promote].parent = rebalance
            rebalance.children[1-promote].children[promote] = rebalance.children[1-promote].children[1-promote]
            rebalance.children[1-promote].children[1-promote] = rescue
            if rescue != None:
                rescue.parent = rebalance.children[1-promote]
            rebalance.children[1-promote].count = rebalance.count - rebalance.children[1-promote].count
            if rebalance.children[1-promote].children[promote] != None:
                rebalance.children[1-promote].count += rebalance.children[1-promote].children[promote].count
            

    def depth(self):
        if self.parent == None:
            return 1
        return 1 + self.parent.depth()
                
    def printTree(self, indent, bits):
        if self.children[1] != None:
            self.children[1].printTree(indent+1, bits)

        prefix = ""
        for i in range(0, indent):
            ch = ' '
            if bits & (1<<i) != 0:
                ch = '|'
            prefix += "%c   " % ch

        suffix = ""
        count = 1
        for i in [0, 1]:
            if self.children[i] != None:
                count += self.children[i].count
                if self.children[i].parent != self:
                    suffix += "(%d parent wrong)" % i
        if count != self.count:
            suffix += "(Count wrong %d != %d)" % (count, self.count)
        if self.children[0] != None and self.children[0].value >= self.value:
            suffix += "(Left misordered %d >= %d)" % (self.children[0].value, self.value)
        if self.children[1] != None and self.children[1].value < self.value:
            suffix += "(Right misordered %d < %d)" % (self.children[1].value, self.value)

        print("%s-- %x/%s/%d %s" % (prefix, self.value, self.data, self.count, suffix))
            
        if indent > 0:
            bits ^= (1<<indent)

        if self.children[0] != None:
            self.children[0].printTree(indent+1, bits | (1<<(indent+1)))
                

class SortingTree:
    def __init__(self):
        self.root = None
        self.current = None

    def insert(self, value, data):
        self.current = None
        if self.root == None:
            self.root = TreeNode(value, data, None)
        else:
            self.root.insert(value, data)

    def printTree(self):
        if self.root == None:
            print("Empty Tree")
        else:
            self.root.printTree(0, 0)

    def contains(self, value, data):
        node = self.root
        while node != None:
            if value == node.value and data == node.data:
                self.current = node
                return True
            if value < node.value:
                node = node.children[0]
            else:
                node = node.children[1]
            
        return False

    # returns details of the node with the smallest value in the tree
    # that is greater or equal to the value passed in
    def greater_equal(self, value):
        node = self.root
        best = None
        while node != None:
            if node.value < value:
                node = node.children[1]
            elif node.value == value:
                best = node
                break
            else:
                if best == None or node.value < best.value:
                    best = node
                node = node.children[0]

        self.current = best
        if best != None:
            return (best.value, best.data)
        return None

    def next(self):
        if self.current == None:
            return None

        if self.current.children[1] != None:
            self.current = self.current.children[1]
            while self.current.children[0] != None:
                self.current = self.current.children[0]
        else:
            carry_on = True
            while carry_on and self.current.parent != None:
                carry_on = self.current.parent.children[0] != self.current
                self.current = self.current.parent
            if carry_on:
                self.current = None

        if self.current:
            return (self.current.value, self.current.data)
        return None

    def depth(self):
        if self.current == None:
            return 0
        return self.current.depth()
    
    def delete(self, value, data):
        if self.contains(value, data) == False:
            return False

        # go up and reduce node counts by 1
        node = self.current
        while node != None:
            node.count -= 1
            node = node.parent

        # work out which child of the parent node we are deleting
        node = self.current
        index = None
        if node.parent != None:
            if node.parent.children[0] == node:
                index = 0
            else:
                index = 1

        # work out which child node to put in place of the one we're deleting
        # XXX could think about which to promote to maintain balance of tree
        promote = None
        if node.children[0] != None:
            promote = 0
        elif node.children[1] != None:
            promote = 1

        # put the other child node of the one we're deleting into the right place
        if promote != None:
            attach = node.children[promote]
            rescue = node.children[1-promote]
            if rescue != None:
                while attach.children[1-promote] != None:
                    attach.count += rescue.count
                    attach = attach.children[1-promote]

                attach.children[1-promote] = rescue
                attach.count += rescue.count
                rescue.parent = attach
            
            node.children[promote].parent = node.parent
            
            if index != None:
                node.parent.children[index] = node.children[promote]
            else:
                self.root = node.children[promote]
        else:
            if index != None:
                node.parent.children[index] = None
            else:
                self.root = None
                
        self.current = None
        return True
