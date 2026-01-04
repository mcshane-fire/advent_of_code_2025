#include <string>
#include <iostream>
#include <fstream>
#include <format>
#include <sstream>

std::vector<int> run_program(std::vector<uint64_t> reg, std::vector<int>& program, int max_results = 0) {
    std::vector<int> output;

    int pc = 0;

    while(pc < program.size()) {
        int op = program[pc+1];
        int cop = op < 4 ? op : reg[op-4];

        switch(program[pc]) {
            case 0: reg[0] >>= cop; break;
            case 1: reg[1] ^= op; break;
            case 2: reg[1] = cop & 7; break;
            case 3: pc = reg[0] > 0 ? op-2 : pc; break;
            case 4: reg[1] ^= reg[2]; break;
            case 5: output.push_back(cop & 7); break;
            case 6: reg[1] = reg[0] >> cop; break;
            case 7: reg[2] = reg[0] >> cop; break;
        }

        if(max_results > 0 and output.size() > max_results) {
            break;
        }

        pc += 2;
    }

    return output;
}

std::vector<std::vector<std::string>> create_masks(std::vector<int>& program) {
    std::vector<std::vector<std::string>> masks;
    std::vector<std::vector<std::vector<int>>> options;

    for(int i=0; i<8; i++) {
        masks.push_back(std::vector<std::string>());
        options.push_back(std::vector<std::vector<int>>());
        for(int j=0; j<8; j++) {
            options[i].push_back(std::vector<int>());
        }
    }
    for(uint64_t i=0; i<1024; i++) {
        std::vector<uint64_t> reg = {i, 0ULL, 0ULL};
        std::vector<int> output = run_program(reg, program, 1);

        options[output[0]][i&7].push_back(i>>3);
    }
    for(int m=0; m<8; m++) {
        //std::cout << "\nTo get "<< m << "\n";
    
        for(int i=0; i<8; i++) {
            std::string fmt = "";
            for(int j=0; j<3; j++) {
                if((i>>j) & 1) {
                    fmt.push_back('1');
                } else {
                    fmt.push_back('0');
                }
            }
            for(int j=0; j<7; j++) {
                int zeros = 0;
                int ones = 0;
                for(int k : options[m][i]) {
                    if((k >> j) & 1) {
                        ones++;
                    } else {
                        zeros++;
                    }
                }
                if(ones == 0 && zeros > 0) {
                    fmt.push_back('0');

                } else if(ones > 0 && zeros == 0) {
                    fmt.push_back('1');
                } else {
                    fmt.push_back('x');
                }
            }
            if(options[m][i].size() > 0) {
                //std::cout << fmt << "\n";
                masks[m].push_back(fmt);
            }
        }
    }

    return masks;
}

std::string combine_masks(std::string& base, std::string& step, int offset) {
    //std::cout << base << " + " << step << " at " << offset << ":";

    std::string ret = base.substr(0,offset);
    for(int i=0; i<std::min(step.length(), base.length()-offset); i++) {
        char bc = base[i+offset];
        char sc = step[i];
        if((bc == '0' && sc == '1') || (bc == '1' && sc == '0')) {
            return std::string();
        } else {
            if(bc == 'x') {
                ret.push_back(sc);
            } else {
                ret.push_back(bc);
            }
        }
    }
    if(step.length() < base.length() - offset) {
        ret.append(base.substr(offset+step.length()));
    } else {
        ret.append(step.substr(base.length() - offset));
    }

    return ret;
}

uint64_t find_replicant(std::vector<int>& program) {
    std::vector<std::vector<std::string>> masks = create_masks(program);

    std::vector<std::string> opt;
    opt.push_back("xxx");
    int offset = 0;

    for(int p : program) {
        std::vector<std::string> new_opt;
        for(std::string m : opt) {
            for(std::string n : masks[p]) {
                std::string no = combine_masks(m, n, offset);
                if(no.length() > 0) {
                    //std::cout << m << " + " << n << " = " << no << "\n";
                    new_opt.push_back(no);
                }
            }
        }
        opt = new_opt;
        offset += 3;
    }

    uint64_t min_solution = 0;
    for(std::string n : opt) {
        uint64_t val = 0;
        for(uint64_t i=0; i<n.length(); i++) {
            if(n[i] == '1') {
                val |= 1ULL<<i;
            }
        }
        if(min_solution == 0 || val < min_solution) {
            min_solution = val;
        }
    }

    return min_solution;
}

int main(int argc, char *argv[]) {
    std::ifstream input(argc >= 2 ? argv[1] : "input.txt");
    std::string line;
    std::vector<uint64_t> reg;
    std::vector<int> program;

    for(int i=0; i<3; i++) {
        std::getline(input, line);
        if(line.find(std::format("Register {:c}: ", 'A' + i)) != std::string::npos) {
            reg.push_back(std::stoull(line.substr(12)));
        }
    }

    std::getline(input, line);
    std::getline(input, line);

    if(line.find("Program: ") == 0) {        
        std::stringstream ss;
        std::string segment;
        ss << line.substr(9);
        while(std::getline(ss, segment, ',')) {
            program.push_back(stoi(segment));
        }        
    }

    std::vector<int> output = run_program(reg, program);
    std::string out;
    for(auto it = output.begin(); it != output.end(); it++) {
        if(it != output.begin()) {
            out.push_back(',');
        }
        out.append(std::format("{:d}", *it));
    }

    std::cout << "Part 1: " << out << "\n";
    std::cout << "Part 2: " << find_replicant(program) << "\n";

    return 0;
}