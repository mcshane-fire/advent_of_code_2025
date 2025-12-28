#include <string>
#include <iostream>
#include <fstream>
#include <map>

int count_trailheads(std::vector<std::string> map, int sum_distinct) {
    int total = 0;
    int xd[] = {1, 0, -1, 0};
    int yd[] = {0, -1, 0, 1};

    std::vector<std::vector<int>> opt;

    for(int y=0; y<map.size(); y++) {
        //std::cout << map[y] << "\n";

        std::vector<int> line;
        for(int x=0; x<map[y].length(); x++) {
            if(map[y][x] == '9') {
                line.push_back(1<<5);
            } else {
                int val = map[y][x] == '0' ? 1 << 4 : 0;

                for(int dir=0; dir<4; dir++) {
                    int nx = x + xd[dir];
                    int ny = y + yd[dir];
                    if(ny >= 0 && ny < map.size() && nx >= 0 && nx < map[ny].length()) {
                        if(map[ny][nx] == map[y][x]+1) {
                            val |= 1<<dir;
                        }
                    }
                }
                line.push_back(val);
            }
            //std::cout << "Seen " << x << "," << y << " : " << map[y][x] << " to val " << line.back() << "\n";
        }
        opt.push_back(line);
    }

    for(int y=0; y<map.size(); y++) {
        for(int x=0; x<map[y].length(); x++) {
            if(opt[y][x] & 1<<4) {
                //std::cout << "Trail head at " << x << "," << y << "\n";
                std::map<std::pair<int, int>,int> border;
                border.insert({std::pair<int,int>(x,y), 1});
                while(border.size() > 0) {
                    //std::cout << " - tracking border:\n";
                    std::map<std::pair<int,int>,int> next;
                    for(auto const &[p,count] : border) {
                        int v = opt[p.second][p.first];
                        //std::cout << "     : " << p.second << "," << p.first << ":" << v << " original " << map[p.second][p.first] << "\n";
                        if(v & 1<<5) {
                            if(sum_distinct) {
                                total += count;
                            } else {
                                total++;
                            }
                        } else {
                            for(int dir=0; dir<4; dir++) {
                                if(v & 1<<dir) {
                                    std::pair<int,int> nb(p.first+xd[dir], p.second+yd[dir]);
                                    if(!next.contains(nb)) {
                                        next.insert({nb,count});
                                        //std::cout << "        -> " << dir << " : " << (p.first+xd[dir]) << "," << (p.second+yd[dir]) << "\n";
                                    } else {
                                        next[nb] = next[nb] + count;
                                    }
                                }
                            }
                        }
                    }
                    border = next;
                }
            }
        }
    }

    return total;
}

int main(int argc, char *argv[]) {
    std::ifstream input(argv[1]);
    std::string line;
    std::vector<std::string> map;

    while(std::getline(input, line)) {
        map.push_back(line);
    }

    std::cout << "Part 1: " << count_trailheads(map, 0) << "\n";
    std::cout << "Part 2: " << count_trailheads(map, 1) << "\n";

    return 0;
}