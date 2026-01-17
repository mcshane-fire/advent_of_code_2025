#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>

int64_t count_parts(const std::vector<std::string>& map) {
    int64_t total = 0;

    for(int y=0; y<map.size(); y++) {
        int num = 0;
        bool symbol = false;
        for(int x=0; x<map[y].length(); x++) {
            if(map[y][x] >= '0' && map[y][x] <= '9') {
                num = num*10 + (map[y][x] - '0');

                for(int dy=-1; dy<=1; dy++) {
                    for(int dx=-1; dx<=1; dx++) {
                        if(y+dy >= 0 && y+dy < map.size() && x+dx >= 0 && x+dx < map[y].length()) {
                            char c = map[y+dy][x+dx];
                            if(c != '.' && !(c >= '0' && c <= '9')) {
                                symbol = true;
                            }
                        }
                    }
                }
            } else {
                if(num > 0 && symbol) {
                    total += num;
                }
                symbol = false;
                num = 0;
            }
        }
        if(num > 0 && symbol) {
            total += num;
        }
    }

    return total;
}

int64_t calculate_value(std::map<std::pair<int,int>,int>& gears, const std::pair<int,int>& p, int value) {
    if(gears.contains(p)) {
        return gears[p] * value;
    } else {
        gears[p] = value;
    }
    return 0;
}

int64_t count_gears(const std::vector<std::string>& map) {
    int64_t total = 0;
    std::map<std::pair<int,int>,int> gears;

    for(int y=0; y<map.size(); y++) {
        int num = 0;
        bool symbol = false;
        std::pair<int,int> star;
        for(int x=0; x<map[y].length(); x++) {
            if(map[y][x] >= '0' && map[y][x] <= '9') {
                num = num*10 + (map[y][x] - '0');

                for(int dy=-1; dy<=1; dy++) {
                    for(int dx=-1; dx<=1; dx++) {
                        if(y+dy >= 0 && y+dy < map.size() && x+dx >= 0 && x+dx < map[y].length()) {
                            char c = map[y+dy][x+dx];
                            if(c == '*') {
                                star = {y+dy,x+dx};
                                symbol = true;
                            }
                        }
                    }
                }
            } else {
                if(num > 0 && symbol) {
                    total += calculate_value(gears, star, num);
                }
                symbol = false;
                num = 0;
            }
        }
        if(num > 0 && symbol) {
            total += calculate_value(gears, star, num);
        }
    }

    return total;
}


int main(int argc, char *argv[]) {
    std::ifstream input(argc >= 2 ? argv[1] : "test_input.txt");
    std::string line;
    std::vector<std::string> map;

    while(std::getline(input, line)) {
        map.push_back(line);
    }

    std::cout << "Part 1: " << count_parts(map) << "\n";
    std::cout << "Part 2: " << count_gears(map) << "\n";
        
    return 0;
}