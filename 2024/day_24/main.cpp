#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <regex>
#include <set>
#include <format>

enum gate_type {
    AND,
    OR,
    XOR,
    NOT,
    EQUAL
};

uint64_t run_gate_logic(std::unordered_map<std::string,int> values,
                        std::unordered_map<std::string,std::set<std::string>>& outputs,
                        std::unordered_map<std::string,gate_type> gates,
                        std::unordered_map<std::string,int>& final) {

    while(gates.size() > 0 || values.size() > 0) {
        std::unordered_map<std::string,int> next_values;

        for(auto it = values.begin(); it != values.end(); it++) {
            final[(*it).first] = (*it).second;
            if(outputs.contains((*it).first)) {
                //std::cout << " Applying " << (*it).first << " = " << (*it).second << "\n";
                for(auto n : outputs[(*it).first]) {
                    if(!gates.contains(n)) {
                        //std::cout << "   skip\n";
                        continue;
                    }
                    //std::cout << "  -> " << n << " " << gates[n] << "\n";
                    switch(gates[n]) {
                        case AND:
                            if((*it).second) {
                                gates[n] = EQUAL;
                            } else {
                                gates.erase(n);
                                next_values[n] = 0;
                            }
                            break;
                        case OR:
                            if((*it).second) {
                                gates.erase(n);
                                next_values[n] = 1;
                            } else {
                                gates[n] = EQUAL;
                            }
                            break;
                        case XOR:
                            gates[n] = (*it).second ? NOT : EQUAL;
                            break;
                        case NOT:
                            next_values[n] = (*it).second ? 0 : 1;
                            gates.erase(n);
                            break;
                        case EQUAL:
                            next_values[n] = (*it).second;
                            gates.erase(n);
                            break;
                    }
                    //std::cout << "     : " << (gates.contains(n) ? gates[n] : -1) << "\n";
                }
            }
        }
        values = next_values;
    }

    uint64_t ret = 0;
    for(auto p : final) {
        //std::cout << p.first << " " << p.second << "\n";
        if(p.first[0] == 'z') {
            ret |= (uint64_t) p.second << stoull(p.first.substr(1));
        }
    }

    return ret;
}

void ps(std::string pre, std::set<std::string> s) {
    std::cout << pre;
    for(auto n : s) { 
        std::cout << n << ", ";
    }
    std::cout << "\n";
}

std::string find_errors(std::unordered_map<std::string,int>& values,
                    std::unordered_map<std::string,std::set<std::string>>& outputs,
                    std::unordered_map<std::string,gate_type>& gates) {

    std::vector<std::pair<std::string,std::string>> swaps = {{"cmv", "z17"}, {"rdg", "z30"}, {"rmj", "z23"}, {"mwp", "btb"}};

    for(auto swap : swaps) {
        gate_type t1 = gates[swap.first];
        gate_type t2 = gates[swap.second];

        gates.erase(swap.first);
        gates.erase(swap.second);

        gates[swap.second] = t1;
        gates[swap.first] = t2;

        for(auto p : outputs) {
            if(outputs[p.first].contains(swap.first) && outputs[p.first].contains(swap.second)) {
                continue;
            }

            if(outputs[p.first].contains(swap.first)) {
                outputs[p.first].erase(swap.first);
                outputs[p.first].insert(swap.second);
            } else if(outputs[p.first].contains(swap.second)) {
                outputs[p.first].erase(swap.second);
                outputs[p.first].insert(swap.first);
            }
        }
    }

    std::unordered_map<std::string,std::set<std::string>> inputs;
    for(auto p : outputs) {
        for(auto n : p.second) {
            if(!inputs.contains(n)) {
                inputs[n] = std::set<std::string>();
            }
            inputs[n].insert(p.first);
        }
    }

    std::map<std::string,std::set<std::string>> zs;
    for(auto p : gates) {
        if(p.first[0] == 'z') {
            zs[p.first] = std::set<std::string>();
        }
    }

    for(auto z : zs) {
        std::vector<std::string> explore;

        explore.push_back(z.first);
        while(explore.size() > 0) {
            std::string e = explore.back();
            explore.pop_back();
            zs[z.first].insert(e);
            for(auto n : inputs[e]) {
                explore.push_back(n);
            }
        }

        if(false) {
            std::cout << z.first << ": ";
            for(auto n : zs[z.first]) {
                std::cout << n << " ";
            }
            std::cout << "\n";
        }
    }

    for(auto p : values) {
        values[p.first] = 0;
    }

    for(uint b=0; b<44; b++) {
        bool success = true;
        //std::cout << "Checking bit " << b << "\n";

        std::vector<std::unordered_map<std::string,int>> final(8, std::unordered_map<std::string,int>());

        for(uint i=0; i<2; i++) {
            for(uint j=0; j<2; j++) {
                for(uint k=0; k<(b > 0 ? 2 : 1); k++) {
                    values[std::format("x{:02d}", b)] = i;
                    values[std::format("y{:02d}", b)] = j;

                    if(b > 0) {
                        values[std::format("x{:02d}", b-1)] = k;
                        values[std::format("y{:02d}", b-1)] = k; 
                    }                      

                    uint exp = i | (j<<1) | (k<<2);

                    auto answer = run_gate_logic(values, outputs, gates, final[exp]) >> b;

                    if((answer & 1) != ((i+j+k) & 1)) {
                        std::cout << "Failed: " << i << "," << j << "," << k << " -> " << (answer & 1) << "\n";
                        success = false;
                    }
                }
            }
        }

        if(!success) {
            std::string z = std::format("z{:02d}", b);
            std::string zp = std::format("z{:02d}", b-1);
            for(auto n : zs[z]) {
                if(!zs[zp].contains(n)) {
                    std::cout << "  additional " << n << ": ";
                    for(uint i=0; i<8; i++) {
                        std::cout << final[i][n] << " ";
                    }
                    std::cout << " : ";
                    for(auto p : inputs[n]) {
                        std::cout << p << " ";
                    }
                    std::cout << "\n";
                }
            }
            
        }
    }

    std::set<std::string> ss;
    for(auto p : swaps) {
        ss.insert(p.first);
        ss.insert(p.second);
    }
    std::string ret;
    std::string sep;
    for(auto n : ss) {
        ret += sep;
        ret += n;
        sep = ",";
    }

    return ret;

}

int main(int argc, char *argv[]) {
    std::ifstream input(argc >= 2 ? argv[1] : "test_input.txt");
    std::string line;
    std::vector<std::pair<uint,uint>> links;
    std::regex r_input = std::regex("(\\w+): ([01])");
    std::regex r_gate = std::regex("(\\w+) (AND|OR|XOR) (\\w+) -> (\\w+)");
    std::smatch res;
    std::unordered_map<std::string,int> values;
    std::unordered_map<std::string,std::set<std::string>> outputs;
    std::unordered_map<std::string,gate_type> gates;

    std::map<std::string,gate_type> gate_type_map = {{"AND",AND}, {"OR",OR}, {"XOR",XOR}};
  

    while(std::getline(input, line)) {  
        if(std::regex_match(line, res, r_input)) {
            //std::cout << "Input: " << res[1] << " " << res[2] << "\n";
            values[res[1]] = stoi(res[2]);
        } else if(std::regex_match(line, res, r_gate)) {
            //std::cout << "Gate: " << res[1] << " " << res[2] << " " << res[3] << " " << res[4] << "\n";
            if(!outputs.contains(res[1])) {
                outputs[res[1]] = std::set<std::string>();
            }
            outputs[res[1]].insert(res[4]);
            if(!outputs.contains(res[3])) {
                outputs[res[3]] = std::set<std::string>();
            }
            outputs[res[3]].insert(res[4]);

            gates[res[4]] = gate_type_map[res[2]];
        }
    }

    std::unordered_map<std::string,int> final;

    std::cout << "Part 1: " << run_gate_logic(values, outputs, gates, final) << "\n";
    std::cout << "Part 2: " << find_errors(values, outputs, gates) << "\n";

    return 0;
}