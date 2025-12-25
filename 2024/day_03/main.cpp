#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

using namespace std;

int part1(std::string str) {
    std::string::const_iterator start(str.cbegin());
    std::regex rmul("mul\\((\\d+),(\\d+)\\)");
    std::smatch res;

    int total = 0;

    while(std::regex_search(start, str.cend(), res, rmul)) {
        //cout << stoi(res[1]) << " * " << stoi(res[2]) << "\n";
        total += stoi(res[1]) * stoi(res[2]);
        start = res.suffix().first;
    }

    return total;
}

int part2(std::string str) {
    std::string::const_iterator start(str.cbegin());
    std::regex rmul("mul\\((\\d+),(\\d+)\\)|do\\(\\)|don't\\(\\)");
    std::smatch res;

    int total = 0;
    int enable = 1;

    while(std::regex_search(start, str.cend(), res, rmul)) {
        //cout << res[0] << "\n";
        if(res[0] == "do()") {
            enable = 1;
        } else if(res[0] == "don't()") {
            enable = 0;
        }
        else if (enable) {
            total += stoi(res[1]) * stoi(res[2]);
            //std::cout << res[1] << ", " << res[2] << "\n";
        }
            
        start = res.suffix().first;
    }

    return total;
}


int main(int argc, char *argv[]) {
    std::ifstream input(argv[1]);
    std::string str((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());

    std::cout << "Part 1: " << part1(str) << "\n";
    std::cout << "Part 2: " << part2(str) << "\n";

    return 0;
}