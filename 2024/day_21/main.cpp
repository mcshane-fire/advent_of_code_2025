#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <regex>
#include <map>

// 135260 is too high

inline std::string move_x(int dx) {
    return std::string(dx > 0 ? dx : -dx, dx > 0 ? '>' : '<');
}

inline std::string move_y(int dy) {
    return std::string(dy > 0 ? dy : -dy, dy > 0 ? 'v' : '^');
}

std::string move(std::pair<int,int>& from, std::pair<int,int>& to, std::pair<int,int>& avoid) {
    std::string ret = "";
    if(from != to) {
        if(avoid == std::pair<int,int>(to.first,from.second)) {
            ret += move_y(to.second - from.second);
            ret += move_x(to.first - from.first);
        } else {
            ret += move_x(to.first - from.first);
            ret += move_y(to.second - from.second);
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

uint64_t solve_problem(std::string line) {

    std::map<char,std::pair<int,int>> numpad = {{'7', {0,0}}, {'8', {1,0}}, {'9', {2,0}},
                                                {'4', {0,1}}, {'5', {1,1}}, {'6', {2,1}},
                                                {'1', {0,2}}, {'2', {1,2}}, {'3', {2,2}},
                                                              {'0', {1,3}}, {'A', {2,3}}};
    std::pair<int,int> numpad_avoid = {0,3};

    std::map<char,std::pair<int,int>> dpad =                 {{'^', {1,0}}, {'A', {2,0}},
                                                {'<', {0,1}}, {'v', {1,1}}, {'>', {2,1}}};
    std::pair<int,int> dpad_avoid = {0,0};

    std::string robot_1 = construct_moves(numpad, numpad_avoid, line);
    std::string robot_2 = construct_moves(dpad, dpad_avoid, robot_1);
    std::string me = construct_moves(dpad, dpad_avoid, robot_2);
    uint64_t num = std::stoull(line.substr(0,line.length()-1)) * me.length();

    std::cout << "\nTarget: " << line << "\n";
    std::cout << robot_1 << "\n";
    std::cout << robot_2 << "\n";
    std::cout << me << "\n";
    std::cout << num << "\n";

    return num;
}

uint64_t find_solutions(std::vector<std::string> problems) {
    uint64_t total = 0;
    for(auto p : problems) {
        total += solve_problem(p);
    }
    return total;
}

int main(int argc, char *argv[]) {
    std::ifstream input(argc >= 2 ? argv[1] : "input.txt");
    std::string line;
    std::vector<std::string> problems;

    while(std::getline(input, line)) {  
        problems.push_back(line);
    }

    std::cout << "Part 1: " << find_solutions(problems) << "\n";

    return 0;
}