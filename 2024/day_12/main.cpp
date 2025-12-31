#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <set>

struct region {
    char letter;
    int area;
    int perimeter;
    int fences;
};

int count_fences(std::vector<std::set<int>> &fence) {
    int ret = 0;
    for(auto s : fence) {
        int prev, first = 1;
        for(auto v : s) {
            //std::cout << v << ",";
            if(first) {
                prev = v;
                first = 0;
            } else {
                if(v != prev+1) {
                    ret++;
                }
                prev = v;
            }
        }
        if(!first) {
            ret++;
        }
    }
    return ret;
}

struct region discover_region(std::vector<std::string> &map, int y, int x) {
    struct region r;
    int xd[] = {1, 0, -1, 0};
    int yd[] = {0, -1, 0, 1};
    int fa[] = {1, 0, 0, 1};
    std::vector<std::set<int>> h_fence(map.size()+1, std::set<int>());
    std::vector<std::set<int>> v_fence(map[0].length()+1, std::set<int>());

    r.letter = map[y][x];
    r.area = 0;
    r.perimeter = 0;

    char to_letter = r.letter - 'A' + 'a';

    std::vector<std::pair<int,int>> explore = std::vector<std::pair<int,int>>();
    explore.push_back({x,y});

    //std::cout << r.letter << "->" << to_letter << "\n";
    while(explore.size() > 0) {
        auto p = explore.back();
        explore.pop_back();
        if(map[p.second][p.first] == to_letter) {
            continue;
        }
        r.area++;
        r.perimeter+=4;
        map[p.second][p.first] = to_letter;
        h_fence[p.second].insert(-p.first);
        h_fence[p.second+1].insert(p.first);
        v_fence[p.first].insert(-p.second);
        v_fence[p.first+1].insert(p.second);

        //std::cout << r.area << " " << r.perimeter << " " << map[y] << "\n";        
        for(int dir=0; dir<4; dir++) {
            int nx = p.first + xd[dir];
            int ny = p.second + yd[dir];
            if(ny >= 0 && ny < map.size() && nx >= 0 && nx < map[ny].length())
            {
                if(map[ny][nx] == r.letter) {
                    explore.push_back({nx,ny});
                } else if(map[ny][nx] == to_letter) {
                    r.perimeter -= 2;
                    if((dir & 1) == 0) {
                        v_fence[p.first+fa[dir]].erase(p.second);
                        v_fence[p.first+fa[dir]].erase(-p.second);
                    } else {
                        h_fence[p.second+fa[dir]].erase(p.first);
                        h_fence[p.second+fa[dir]].erase(-p.first);
                    }
                }
            }
        }
        //std::cout << "  After doing " << p.first << "," << p.second << " now A=" << r.area << " P=" << r.perimeter << "\n";
    }

    r.fences = count_fences(h_fence) + count_fences(v_fence);
    //std::cout << to_letter  << " " << r.fences << "\n";

    return r;
}

int cost_fences(std::vector<std::string> map, int count_fences) {
    int total = 0;
    std::vector<struct region> regions;

    for(int y=0; y<map.size(); y++) {
        for(int x=0; x<map[y].length(); x++) {
            if(map[y][x] >= 'A' && map[y][x] <= 'Z') {
                regions.push_back(discover_region(map, y, x));
            }
        }
    }

    for(struct region r : regions) {
        //std::cout << r.letter << ": A=" << r.area << " P=" << r.perimeter << "\n";
        if(count_fences) {
            total += r.area * r.fences;
        } else {
            total += r.area * r.perimeter;
        }
    }

    return total;
}

int main(int argc, char *argv[]) {
    std::ifstream input(argc == 2 ? argv[1] : "input.txt");
    std::string line;
    std::vector<std::string> map;

    while(std::getline(input, line)) {
        map.push_back(line);
    }

    std::cout << "Part 1: " << cost_fences(map, 0) << "\n";
    std::cout << "Part 2: " << cost_fences(map, 1) << "\n";

    return 0;
}