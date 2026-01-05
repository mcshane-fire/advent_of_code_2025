#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <ranges>
#include <set>
#include <regex>

struct position {
    int x, y;
    int prev_x, prev_y;
};

struct costing {
    int cost;
    std::vector<struct position> prev;
};

void add_explore(std::map<int,std::vector<struct position>>& explore, int cost, int x, int y, int prev_x, int prev_y) {
    if(!explore.contains(cost)) {
        explore.insert(std::pair<int,std::vector<struct position>> (cost, std::vector<struct position>()));
    }
    explore[cost].emplace_back(x, y, prev_x, prev_y);
}

void backtrack(std::vector<std::vector<struct costing>>& best_cost, std::set<std::pair<int,int>>& visited, int x, int y) {
    if(x == -1) {
        return;
    }

    visited.insert({x, y});

    //std::cout << "Backtrack at " << x << "," << y << "\n";
    for(auto p : best_cost[y][x].prev) {
        //std::cout << "FV: " << p.x << "," << p.y <<  " prev:" << p.prev_x << "," << p.prev_y << "\n";
        backtrack(best_cost, visited, p.prev_x, p.prev_y);
    }
} 

std::pair<int,int> find_shortest_route(std::vector<std::string>& map) {
    int xd[] = {1, 0, -1, 0};
    int yd[] = {0, -1, 0, 1};

    std::vector<std::vector<struct costing>> best_cost;
    for(int i=0; i<map.size(); i++) {
        std::vector<struct costing> row;
        for(int j=0; j<map[i].length(); j++) {
            row.push_back({-1});
        }
        best_cost.push_back(row);
    }

    std::map<int,std::vector<struct position>> explore;
    add_explore(explore, 0, 0, 0, -1, -1);

    int minimum_cost = 0;

    while(explore.size() > 0 && minimum_cost == 0) {
        auto cp = *explore.begin();
        explore.erase(explore.begin());

        //std::cout << cp.first << "\n";
        for(auto p : cp.second) {
            struct costing& bc = best_cost[p.y][p.x];
            //std::cout << " - " << p.x << "," << p.y << " " << ", cost: " << bc.cost << ", prev = " << p.prev_x << "," << p.prev_y << "]\n";

            if(p.y == map.size()-1 && p.x == map[map.size()-1].length()-1) {
                //std::cout << "    - found exit\n";
                minimum_cost = cp.first;
            }

            if(bc.cost == -1) {
                bc.cost = cp.first;
                bc.prev.push_back(p);

                //std::cout << "   - best cost to here now " << cp.first << " (prev:" << p.prev_x << "," << p.prev_y << ")\n";
                for(int d = 0; d < 4; d++) {
                    if(p.y + yd[d] >= 0 && p.y + yd[d] < map.size() && p.x + xd[d] >= 0 && p.x + xd[d] < map[p.y+yd[d]].length()) {
                        char c = map[p.y + yd[d]][p.x + xd[d]];
                        //std::cout << "    - dir " << d << ", next char " << c << "\n";
                        if(c == '.' ) {
                            add_explore(explore, cp.first+1, p.x + xd[d], p.y + yd[d], p.x, p.y);
                        }
                    }
                }
            }
            else if (bc.cost == cp.first) {
                //std::cout << "  - matched best cost, joining routes of equal cost\n";
                bc.prev.push_back(p);
            }
        }
    }

    // calculate number of tiles on best routes
    std::set<std::pair<int,int>> visited;
    visited.insert({map[map.size()-1].length()-1,map.size()-1});
    for(auto p : best_cost[map.size()-1][map[map.size()-1].length()-1].prev) {
        backtrack(best_cost, visited, p.prev_x, p.prev_y);
    }

    return std::make_pair(minimum_cost, visited.size());
}

int find_steps(std::vector<std::pair<int,int>> locations, int width, int height, int to_fall) {
    std::vector<std::string> map = std::vector<std::string>();
    for(int y=0; y<height; y++) {
        map.push_back(std::string(width, '.'));
    }
    for(int i=0; i<to_fall; i++) {
        map[locations[i].second][locations[i].first] = '#';
    }

    return find_shortest_route(map).first;
}

int main(int argc, char *argv[]) {
    std::ifstream input(argc >= 2 ? argv[1] : "input.txt");
    std::string line;
    std::vector<std::pair<int,int>> locations;
    std::regex numbers = std::regex("(\\d+),(\\d+)");
    std::smatch res;

    while(std::getline(input, line)) {  
        if(std::regex_match(line, res, numbers)) {
            locations.push_back({std::stoi(res[1]), std::stoi(res[2])});
        }
    }

    std::cout << "Part 1: " << find_steps(locations, 71, 71, 1024) << "\n";

    for(int i=locations.size(); i>=0; i--) {
        if(find_steps(locations, 71, 71, i) != 0) {
            std::cout << "Part 2: " << locations[i].first << "," << locations[i].second << "\n";
            break;
        }
    }

    return 0;
}