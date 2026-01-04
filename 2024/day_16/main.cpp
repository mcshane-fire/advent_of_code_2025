#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <ranges>
#include <set>

struct position {
    int x, y, dir;
    int prev_x, prev_y, prev_dir;
};

struct costing {
    int cost[4];
    std::vector<struct position> prev;
};

void add_explore(std::map<int,std::vector<struct position>>& explore, int cost, int x, int y, int dir, int prev_x, int prev_y, int prev_dir) {
    if(!explore.contains(cost)) {
        explore.insert(std::pair<int,std::vector<struct position>> (cost, std::vector<struct position>()));
    }
    explore[cost].emplace_back(x, y, dir, prev_x, prev_y, prev_dir);
}

void backtrack(std::vector<std::vector<struct costing>>& best_cost, std::set<std::pair<int,int>>& visited, int x, int y, int dir) {
    if(x == -1) {
        return;
    }

    visited.insert({x, y});

    //std::cout << "Backtrack at " << x << "," << y << "[" << dir << "]\n";
    for(auto p : best_cost[y][x].prev | std::views::filter([dir](struct position p){ return p.dir == dir; })) {
        //std::cout << "FV: " << p.x << "," << p.y << "[" << p.dir << "]  prev:" << p.prev_x << "," << p.prev_y << "[" << p.prev_dir << "]\n";
        backtrack(best_cost, visited, p.prev_x, p.prev_y, p.prev_dir);
    }
} 

std::pair<int,int> find_shortest_route(std::vector<std::string>& map) {
    int xd[] = {1, 0, -1, 0};
    int yd[] = {0, -1, 0, 1};

    if(map[map.size()-2][1] != 'S') {
        std::cout << "Start not found\n";
    }
    if(map[1][map[1].length()-2] != 'E') {
        std::cout << "End not found\n";
    }

    std::vector<std::vector<struct costing>> best_cost;
    for(int i=0; i<map.size(); i++) {
        std::vector<struct costing> row;
        for(int j=0; j<map[i].length(); j++) {
            row.push_back({{-1, -1, -1, -1}});
        }
        best_cost.push_back(row);
    }

    std::map<int,std::vector<struct position>> explore;
    add_explore(explore, 0, 1, map.size()-2, 0, -1, -1, -1);

    int minimum_cost = 0;

    while(explore.size() > 0 && minimum_cost == 0) {
        auto cp = *explore.begin();
        explore.erase(explore.begin());

        //std::cout << cp.first << "\n";
        for(auto p : cp.second) {
            struct costing& bc = best_cost[p.y][p.x];
            //std::cout << " - " << p.x << "," << p.y << "," << p.dir << ", costs: [" << bc.cost[0] << "," << bc.cost[1] << "," << bc.cost[2] << "," << bc.cost[3] << "], prev = " << p.prev_x << "," << p.prev_y << "[" << p.prev_dir << "]\n";

            if(map[p.y][p.x] == 'E') {
                //std::cout << "    - found exit\n";
                minimum_cost = cp.first;
            }

            if(bc.cost[p.dir] == -1) {
                bc.cost[p.dir] = cp.first;
                bc.prev.push_back(p);

                //std::cout << "   - best cost["<<p.dir<<"] to here now " << cp.first << " (prev:" << p.prev_x << "," << p.prev_y << ")\n";

                char c = map[p.y + yd[p.dir]][p.x + xd[p.dir]];
                //std::cout << "    - next char " << c << "\n";
                if(c == '.' || c == 'E') {
                    add_explore(explore, cp.first+1, p.x + xd[p.dir], p.y + yd[p.dir], p.dir, p.x, p.y, p.dir);
                }

                for(int d = 1; d < 4; d+=2) {
                    int nd = (p.dir + d) & 0x3;
                    char c = map[p.y + yd[nd]][p.x + xd[nd]];
                    //std::cout << "    - look around, next char " << c << "\n";
                    if(c == '.' || c == 'E') {
                        add_explore(explore, cp.first+1+(d == 2 ? 2000 : 1000), p.x + xd[nd], p.y + yd[nd], nd, p.x, p.y, p.dir);
                    }
                }
            }
            else if (bc.cost[p.dir] == cp.first) {
                //std::cout << "  - matched best cost, joining routes of equal cost\n";
                bc.prev.push_back(p);
            }
            //else {
            //    std::cout << "  - ignoring point, reached here already\n";
            //}
        }
    }

    // calculate number of tiles on best routes
    std::set<std::pair<int,int>> visited;
    visited.insert({map[1].length()-2,1});
    for(auto p : best_cost[1][map[1].length()-2].prev) {
        backtrack(best_cost, visited, p.prev_x, p.prev_y, p.prev_dir);
    }

    return std::make_pair(minimum_cost, visited.size());
}

int main(int argc, char *argv[]) {
    std::ifstream input(argc >= 2 ? argv[1] : "test_input1.txt");
    std::string line;
    std::vector<std::string> map;

    while(std::getline(input, line)) {  
        map.push_back(line);
    }

    auto r = find_shortest_route(map);
    std::cout << "Part 1: " << r.first << "\n";
    std::cout << "Part 2: " << r.second << "\n";

    return 0;
}