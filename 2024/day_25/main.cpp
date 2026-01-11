#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <regex>
#include <map>
#include <numeric>
#include <algorithm>

int count_combinations(std::vector<std::vector<uint>> keys, std::vector<std::vector<uint>> locks, uint max_value) {
    int total = 0;

    for(auto kit = keys.begin(); kit != keys.end(); kit++) {
        for(auto lit = locks.begin(); lit != locks.end(); lit++) {
            std::vector<uint> result((*kit).size());
            std::transform((*kit).begin(), (*kit).end(), (*lit).begin(), result.begin(), std::plus<uint>());
            if((*std::max_element(result.begin(), result.end())) <= max_value) {
                total++;
            }
        }
    }

    return total;
}

int main(int argc, char *argv[]) {
    std::ifstream input(argc >= 2 ? argv[1] : "test_input.txt");
    std::string line;
    std::vector<std::string> problems;

    std::vector<std::vector<uint>> keys;
    std::vector<std::vector<uint>> locks;

    bool in_pattern = false;
    bool in_lock = false;
    uint max_value = 0;
    uint num_lines = 0;

    while(std::getline(input, line)) {  
        if(!in_pattern && line.length() > 0) {
            if((in_lock = line.find('.') == line.npos)) {
                locks.push_back(std::vector<uint>(line.length()));
            } else {
                keys.push_back(std::vector<uint>(line.length()));
            }
            in_pattern = true;
            num_lines = 0;
        }

        if(line.length() == 0) {
            if(num_lines > max_value) {
                max_value = num_lines;
            }
            in_pattern = false;
        } else {
            num_lines++;
            for(uint i=0; i<line.length(); i++) {
                if(in_lock) {
                    locks.back()[i] += line[i] == '.' ? 0 : 1;
                } else {
                    keys.back()[i] += line[i] == '.' ? 0 : 1;
                }
            }
        }
    }

    std::cout << "Part 1: " << count_combinations(keys, locks, max_value) << "\n";

    return 0;
}