#include <string>
#include <iostream>
#include <fstream>
#include <regex>
#include <map>

uint sum(std::vector<int>& v, int offset, size_t len) {
    uint total = 0;
    uint mul = 1;
    for(uint i=0; i<std::min(len, v.size() - offset); i++) {
        total += v[i+offset] * mul;
        mul *= 5;
    }
    return total;
}

int64_t find_next_towel(std::vector<int64_t>& solutions, std::vector<std::vector<uint>>& fit, std::map<int,std::vector<std::pair<uint, uint>>>& lut, uint pos) {
    int64_t solutions_found = 0;

    if(pos == fit[1].size()) {
        //std::cout << pos << " = completed!\n";
        return 1;
    }
    if(solutions[pos] >= 0) {
        //std::cout << pos << " - already know there are " << solutions[pos] << " solutions from this position\n";
        return solutions[pos];
    }

    for(auto p : lut[fit[2][pos]]) {
        //std::cout << pos << " " << fit[2][pos] << " : " << p.first << "," << p.second << "\n";
        if(fit[1].size() >= pos + p.first && fit[p.first][pos] == p.second) {
            //std::cout << " - match" << "\n";
            solutions_found += find_next_towel(solutions, fit, lut, pos + p.first);
        }
    }

    solutions[pos] = solutions_found;
    //std::cout << pos << " found " << solutions_found << "\n";

    return solutions_found;
}

int64_t solve_problem(std::map<int,std::vector<std::pair<uint, uint>>>& lut, size_t max_towel_size, std::vector<int>& problem) {
    //std::cout << "\n\nProblem: " << problem.size() << " " << max_towel_size << "\n";

    std::vector<std::vector<uint>> fit;
    for(uint i=0; i<max_towel_size+1; i++) {
        fit.push_back(std::vector<uint>());
    }
    std::vector<int64_t> solutions;

    for(uint i=0; i<problem.size(); i++) {
        solutions.push_back(-1);
        for(uint j=1; j<=max_towel_size; j++) {
            fit[j].push_back(sum(problem, i, j));
        }
    }

    return find_next_towel(solutions, fit, lut, 0);
}

std::pair<int,int64_t> find_solutions(std::vector<std::vector<int>>& problems, std::vector<std::vector<int>>& towels) {
    int solvable = 0;
    int64_t total = 0;
    std::map<int,std::vector<std::pair<uint, uint>>> lut;
    size_t max_towel_size = 0;

    for(std::vector<int> towel : towels) {
        uint front = sum(towel, 0, 2);
        uint all = sum(towel, 0, towel.size());
        lut[front].emplace_back(towel.size(), all);

        if(towel.size() == 1) {
            for(uint i=1; i<5; i++) {
                lut[front + (i * 5)].emplace_back(1, all);
            }
        }
        max_towel_size = std::max(max_towel_size, towel.size());
    }

    for(std::vector<int> problem : problems) {
        int64_t found = solve_problem(lut, max_towel_size, problem);
        solvable += found > 0;
        total += found;
    }

    return std::make_pair(solvable, total);
}

int main(int argc, char *argv[]) {
    std::ifstream input(argv[1]);
    std::string line;
    std::vector<std::vector<int>> problems;
    std::vector<std::vector<int>> towels;
    std::regex num("[wubrg]+");
    std::smatch res;
    std::map<char,int> map = {{'w', 0}, {'u', 1}, {'b', 2}, {'r', 3}, {'g', 4}};

    std::getline(input, line);
    std::string::const_iterator start(line.cbegin());

    while(std::regex_search(start, line.cend(), res, num)) {
        std::vector<int> towel;
        std::string s(res[0]);

        for(auto c : s) {
            towel.push_back(map[c]);
        }

        towels.push_back(towel);
        start = res.suffix().first;
    }
    
    std::getline(input, line);

    while(std::getline(input, line)) {
        std::vector<int> problem;

        for(auto c : line) {
            problem.push_back(map[c]);   
        }
        problems.push_back(problem);
    }

    auto p = find_solutions(problems, towels);

    std::cout << "Part 1: " << p.first << "\n";
    std::cout << "Part 2: " << p.second << "\n";

    return 0;
}