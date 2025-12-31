#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

class Node {
    public:
    Node(uint64_t initial_value) {
        value = initial_value;
    }

    int blink() {
        int ret = -1;
        if(value == 0) {
            value = 1;
        } else {
            std::string vs = std::to_string(value);
            if(vs.length() % 2 == 0) {
                value = std::stoull(vs.substr(0,vs.length()/2));
                ret = std::stoi(vs.substr(vs.length()/2,vs.length()/2));
            } else {
                value *= 2024;
            }
        }
        return ret;
    }

    uint64_t value;
    Node *next;
    Node *prev;
};

class NodeList {
    public:
    NodeList() {
        head = nullptr;
        tail = nullptr;
        count = 0;
    }

    void append(uint64_t val) {
        if(count == 0) {
            head = tail = new Node(val);
        } else {
            tail->next = new Node(val);
            tail->next->prev = tail;
            tail = tail->next;
        }
        count++;
    }

    void insert(uint64_t val) {
        if(count == 0) {
            head = tail = new Node(val);
        } else {
            head->prev = new Node(val);
            head->prev->next = head;
            head = head->prev;
        }
        count++;
    }

    void blink() {
        Node *n = head;
        while(n != nullptr) {
            int add = n->blink();
            if(add >= 0) {
                Node *nn = new Node(add);
                if(n->next != nullptr) {
                    n->next->prev = nn;
                } else {
                    tail = nn;
                }
                nn->next = n->next;
                nn->prev = n;
                n->next = nn;
                count++;
                n = nn;
            }
            n = n->next;
        }
    }

    int length() {
        return count;
    }

    void print() {
        Node *n = head;
        while(n != nullptr) {
            std::cout << n->value << "\n";
            n = n->next;
        }
    }

    private:
    Node *head;
    Node *tail;
    int count;
};

class NodeSet {
    public:
        NodeSet() {
            values = std::unordered_map<uint64_t,uint64_t>();
            count = 0;
        }

        void append(uint64_t n) {
            add(values, n, 1);
            count++;
        }

        void blink() {
            std::unordered_map<uint64_t,uint64_t> new_values = std::unordered_map<uint64_t,uint64_t>();
            for(auto p : values) {
                if(p.first == 0) {
                    add(new_values, 1, p.second);
                } else {
                    std::string vs = std::to_string(p.first);
                    if(vs.length() % 2 == 0) {
                        uint64_t v2 = std::stoull(vs.substr(0,vs.length()/2));
                        uint64_t v1 = std::stoi(vs.substr(vs.length()/2,vs.length()/2));
                        add(new_values, v1, p.second);
                        add(new_values, v2, p.second);
                        count += p.second;
                    } else {
                        add(new_values, p.first * 2024, p.second);
                    }
                }
            }
            values = new_values;
        }

        uint64_t length() {
            return count;
        }

        void print() {
            std::cout << "\nNodeSet:\n";
            for(auto p : values) {
                std::cout << p.first << " : " << p.second << "\n";
            }
        }

    private:
        std::unordered_map<uint64_t,uint64_t> values;
        uint64_t count;

        void add(std::unordered_map<uint64_t,uint64_t> &v, uint64_t key, uint64_t num) {
            if(v.contains(key)) {
                v[key] += num;
            } else {
                v[key]=num;
            }
        }
};

uint64_t count_stones(std::vector<int> numbers, int blinks) {

    //NodeList list;
    NodeSet list;
    for(int n : numbers) {
        list.append(n);
    }

    for(int i=0; i<blinks; i++) {
        list.blink();
        //std::cout << i << " " << list.length() << "\n";
        //list.print();
    }

    //list.print();

    return list.length();
}

int main(int argc, char *argv[]) {
    std::ifstream input(argc > 1 ? argv[1] : "input.txt");
    std::string line;

    std::getline(input, line);

    std::stringstream ss;
    ss << line;

    std::vector<int> numbers;

    while(!ss.eof()) {
        std::string p;
        ss >> p;
        numbers.push_back(stoi(p));
    }

    std::cout << "Part 1: " << count_stones(numbers, 25) << "\n";
    std::cout << "Part 2: " << count_stones(numbers, 75) << "\n";

    return 0;
}
