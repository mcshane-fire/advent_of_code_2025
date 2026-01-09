#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <regex>
#include <map>
#include <numeric>

// part 2: 190604550402542 is too high
// part 2:  76144653735612 is too low (same algorithm as above, but 24 steps)

inline std::string move_x(int dx) {
    return std::string(dx > 0 ? dx : -dx, dx > 0 ? '>' : '<');
}

inline std::string move_y(int dy) {
    return std::string(dy > 0 ? dy : -dy, dy > 0 ? 'v' : '^');
}

std::string move(std::pair<int,int>& from, std::pair<int,int>& to, std::pair<int,int>& avoid, bool invert = false) {
    std::string ret = "";
    if((from != to)) {
        if((to.second > from.second && to.first > from.first) ^ invert) {
            if(avoid == std::pair<int,int>(from.first,to.second)) {
                ret += move_x(to.first - from.first);
                ret += move_y(to.second - from.second);
            } else {
                ret += move_y(to.second - from.second);
                ret += move_x(to.first - from.first);
            }
        } else {
            if(avoid == std::pair<int,int>(to.first,from.second)) {
                ret += move_y(to.second - from.second);
                ret += move_x(to.first - from.first);
            } else {
                ret += move_x(to.first - from.first);
                ret += move_y(to.second - from.second);
            }
        }
    }
    return ret;
}

std::string construct_moves(std::map<char,std::pair<int,int>>& keys, std::pair<int,int>& avoid, std::string input) {
    std::string moves = "";
    input.insert(0,1,'A');

    for(uint i=0; i<input.length()-1; i++) {
        moves += move(keys[input[i]], keys[input[i+1]], avoid);
        moves += "A";
    }

    return moves;
}

uint64_t solve_problems(std::vector<std::string> problems, bool fast, int num_robots) {
    uint64_t total = 0;
    std::map<char,std::pair<int,int>> numpad = {{'7', {0,0}}, {'8', {1,0}}, {'9', {2,0}},
                                                {'4', {0,1}}, {'5', {1,1}}, {'6', {2,1}},
                                                {'1', {0,2}}, {'2', {1,2}}, {'3', {2,2}},
                                                              {'0', {1,3}}, {'A', {2,3}}};
    std::pair<int,int> numpad_avoid = {0,3};

    std::map<char,std::pair<int,int>> dpad =                 {{'^', {1,0}}, {'A', {2,0}},
                                                {'<', {0,1}}, {'v', {1,1}}, {'>', {2,1}}};
    std::pair<int,int> dpad_avoid = {0,0};

    std::vector<char> convert = {'A', '>', 'v', '<', '^'};
    std::map<char,int> convert_back = {{'A',0}, {'>',1}, {'v',2}, {'<',3}, {'^',4}};
    std::vector<std::vector<uint>> next_step(convert.size()*convert.size(), std::vector<uint>());
    for(uint i=0; i<convert.size(); i++) {
        for(uint j=0; j<convert.size(); j++) {
            uint code = (i*convert.size()) + j;
            std::string moves = 'A' + move(dpad[convert[i]], dpad[convert[j]], dpad_avoid) + 'A';

            for(uint k=0; k<moves.length()-1; k++) {
                uint to = (convert_back[moves[k]]*convert.size()) + convert_back[moves[k+1]];
                //std::cout << "   -> " << to << " " << moves[k] << moves[k+1] << "\n";
                next_step[code].push_back(to);
            }
        }
    }

    for(std::string line : problems) {
        std::string sequence = construct_moves(numpad, numpad_avoid, line);
        uint64_t num = stoull(line.substr(0,line.length()-1));

        //std::cout << "\n" << line << "\n";
        //std::cout << sequence.length() << " " << sequence << "\n";

        if(fast) {
            sequence.insert(0,1,'A');
            std::vector<uint64_t> element_count(convert.size() * convert.size(), 0);
            for(uint i=0; i<sequence.length()-1; i++) {
                element_count[(convert_back[sequence[i]]*convert.size()) + convert_back[sequence[i+1]]]++;
            }

            for(uint n=0; n<num_robots; n++) {
                //std::cout << "Count: " << std::reduce(element_count.begin(), element_count.end()) << "\n";
                std::vector<uint64_t> next_element_count(convert.size() * convert.size(), 0);
                for(uint i=0; i<element_count.size(); i++) {
                    for(uint p : next_step[i]) {
                        next_element_count[p] += element_count[i];
                    }
                }

                element_count = next_element_count;
            }

            //std::cout << "Count: " << std::reduce(element_count.begin(), element_count.end()) << "\n";
            num *= std::reduce(element_count.begin(), element_count.end());

        } else {
            for(int i=0; i<num_robots; i++) {
                sequence = construct_moves(dpad, dpad_avoid, sequence);
                //std::cout << sequence.length() << " " << sequence.substr(0,80) << "\n";
            }   
            num *= sequence.length();
        }
        total += num;
    }

    return total;
}

int main(int argc, char *argv[]) {
    std::ifstream input(argc >= 2 ? argv[1] : "test_input2.txt");
    std::string line;
    std::vector<std::string> problems;

    while(std::getline(input, line)) {  
        problems.push_back(line);
    }

    std::cout << "Part 1: " << solve_problems(problems, false, 2) << "\n";
    std::cout << "Part 2: " << solve_problems(problems, true, 25) << "\n";

    return 0;
}