#include <string>
#include <fstream>
#include <iostream>
#include <regex>
#include <map>
#include <algorithm>

uint sum_digits(std::vector<std::string> lines) {
    std::regex digit_start = std::regex("^[^\\d]*(\\d)");
    std::regex digit_end = std::regex(".*(\\d)[^\\d]*$");
    std::smatch res_start, res_end;
    uint total = 0;

    for(auto line : lines) {
        if(std::regex_search(line, res_start, digit_start) && std::regex_search(line, res_end, digit_end)) {
            total += (std::stoi(res_start[1]) * 10) + std::stoi(res_end[1]);
        }
    }
    return total;
}

uint sum_digits_words(std::vector<std::string> lines) {
    std::regex digit = std::regex("(?=(\\d|one|two|three|four|five|six|seven|eight|nine))");
    std::regex rdigit = std::regex("(?=(\\d|eno|owt|eerht|ruof|evif|xis|neves|thgie|enin))");
    std::map<std::string,uint> lu = {{"0",0}, {"1",1}, {"2",2}, {"3",3}, {"4",4}, {"5",5}, {"6",6}, {"7",7}, {"8",8}, {"9",9},
        {"one",1}, {"two",2}, {"three",3}, {"four",4}, {"five",5}, {"six",6}, {"seven",7}, {"eight",8}, {"nine",9},
        {"eno",1}, {"owt",2}, {"eerht",3}, {"ruof",4}, {"evif",5}, {"xis",6}, {"neves",7}, {"thgie",8}, {"enin",9}};
    std::smatch res;
    uint total = 0;

    for(auto line : lines) {
        if(std::regex_search(line, res, digit)) {
            uint n1 = lu[res[1]];
            std::reverse(line.begin(), line.end());
            if(std::regex_search(line, res, rdigit)) {
                total += (n1*10) + lu[res[1]];
            }
        }
    }
    return total;
}

int main(int argc, char *argv[]) {
    std::ifstream input(argc >= 2 ? argv[1] : "test_input2.txt");
    std::string line;

    std::vector<std::string> lines;
    uint total = 0;

    while(std::getline(input, line)) {
        lines.push_back(line);
    }
        
    std::cout << "Part1: " << sum_digits(lines) << "\n";
    std::cout << "Part2: " << sum_digits_words(lines) << "\n";


    return 0;
}

