#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <regex>
#include <map>

inline void add_explore(std::map<int,std::vector<std::pair<int,int>>>& explore, int cost, int x, int y) {
    if(!explore.contains(cost)) {
        explore.insert(std::pair<int,std::vector<std::pair<int,int>>> (cost, std::vector<std::pair<int,int>>()));
    }
    explore[cost].emplace_back(x, y);
}

inline int shortcut_length(std::vector<std::vector<int>>& best_cost, int x, int y, int xd, int yd) {
    if(y+yd >= 0 && y+yd < (int) best_cost.size() && x+xd >= 0 && x+xd < (int) best_cost[y+yd].size()) {
        if(best_cost[y+yd][x+xd] > best_cost[y][x]) {
            int saving = best_cost[y+yd][x+xd] - best_cost[y][x] - std::abs(xd) - std::abs(yd);
            return saving;
        }
    }
    return 0;
}

int find_shortcuts_variable(std::vector<std::string>& map, int skip_length, int min_cheat_length) {
    int xd[] = {1, 0, -1, 0};
    int yd[] = {0, -1, 0, 1};
    std::pair<int,int> start;
    std::pair<int,int> end;
    std::vector<std::vector<int>> best_cost;

    for(size_t y=0; y<map.size(); y++) {
        std::vector<int> cost_row;
        for(size_t x=0; x<map[y].length(); x++) {
            cost_row.push_back(-1);
        }
        best_cost.push_back(cost_row);

        size_t x = map[y].find('S');
        if(x != map[y].npos) {
            start = {x, y};
        }
        x = map[y].find('E');
        if(x != map[y].npos) {
            end = {x, y};
        }
    }

    std::map<int,std::vector<std::pair<int,int>>> explore;
    add_explore(explore, 0, start.first, start.second);

    int minimum_cost = 0;

    while(explore.size() > 0 && minimum_cost == 0) {
        auto cp = *explore.begin();
        explore.erase(explore.begin());

        //std::cout << cp.first << "\n";
        for(auto p : cp.second) {
            int bc = best_cost[p.second][p.first];
            //std::cout << " - " << p.x << "," << p.y << " " << ", cost: " << bc << "\n";

            if(p == end) {
                //std::cout << "    - found exit\n";
                minimum_cost = cp.first;
            }

            if(bc == -1) {
                bc = cp.first;
                best_cost[p.second][p.first] = bc;

                //std::cout << "   - best cost to here now " << cp.first << "\n";
                for(int d = 0; d < 4; d++) {
                    char c = map[p.second + yd[d]][p.first + xd[d]];
                    //std::cout << "    - dir " << d << ", next char " << c << "\n";
                    if(c == '.' || c == 'E') {
                        add_explore(explore, cp.first+1, p.first + xd[d], p.second + yd[d]);
                    }
                }
            }
        }
    }

    std::vector<uint64_t> found(minimum_cost+1, 0);
    for(int y=0; y<(int) best_cost.size(); y++) {
        for(int x=0; x<(int) best_cost[y].size(); x++) {
            //std::cout << x << "," << y << " " << best_cost[y][x] << "\n";
            if(best_cost[y][x] != -1) {
                for(int d=2; d<=skip_length; d++) {
                    int d1 = 0;
                    int d2 = d;
                    for(int p=0; p<d; p++) {
                        found[shortcut_length(best_cost, x, y, d1, d2)]++;
                        found[shortcut_length(best_cost, x, y, -d1, -d2)]++;
                        found[shortcut_length(best_cost, x, y, d2, -d1)]++;
                        found[shortcut_length(best_cost, x, y, -d2, d1)]++;
                        d1++;
                        d2--;
                    }                
                }
            }
        }
    }

    uint64_t total = 0;
    for(int i=0; i<=minimum_cost; i++) {
        if(i >= min_cheat_length) {
            total += found[i];
        }
    }

    return total;
}

int main(int argc, char *argv[]) {
    std::ifstream input(argc >= 2 ? argv[1] : "input.txt");
    std::string line;
    std::vector<std::string> map;
    std::regex numbers = std::regex("(\\d+),(\\d+)");
    std::smatch res;

    while(std::getline(input, line)) {  
        map.push_back(line);
    }

    std::cout << "Part 1: " << find_shortcuts_variable(map, 2, 100) << "\n";
    std::cout << "Part 2: " << find_shortcuts_variable(map, 20, 100) << "\n";

    return 0;
}