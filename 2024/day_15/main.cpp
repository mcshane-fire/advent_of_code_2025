#include <string>
#include <iostream>
#include <fstream>
#include <vector>

void scale_map(std::vector<std::string>& map) {
    for(int y=0; y<map.size(); y++) {
        for(int x=map[y].length()-1; x>=0; x--) {
            char c = map[y][x];
            std::string s;
            switch(c) {
                case '#': s = "##"; break;
                case 'O': s = "[]"; break;
                case '.': s = ".."; break;
                case '@': s = "@."; break;
            }
            map[y].replace(x, 1, s);
        }
    }
}

uint64_t follow_instructions(std::vector<std::string> map, std::string& instructions) {
    std::size_t x, y;  
    int xd[] = {1, 0, -1, 0};
    int yd[] = {0, -1, 0, 1};
    char ic[] = {'>', '^', '<', 'v'};
    
    for(y=0; y<map.size(); y++) {
        x = map[y].find('@');
        if(x != std::string::npos) {
            break;
        }
    }

    for(char c : instructions) {
        int dir = 0;
        while(dir < 4 && ic[dir] != c) {
            dir++;
        }

        std::vector<std::pair<int,int>> front;
        std::vector<std::pair<int,int>> back;

        front.push_back({x,y});

        bool can_move = true;
        bool added = true;
        while(can_move && added) {
            // for all tiles in front, this move may push some boxes, so adjust front/back according
            added = false;
            for(int i=0; i<front.size(); i++) {
                auto p = front[i];
                char c = map[p.second + yd[dir]][p.first + xd[dir]];
                //std::cout << "f("<<i<<"/"<<front.size()<<"): "<<p.first<<","<<p.second<<" -> "<<(p.first+xd[dir])<<","<<(p.second+yd[dir])<< " = " << c << "\n";
                if(c == '.') {
                    //std::cout << " - space, move on\n";
                    continue;
                } else if(c == 'O') {
                    front.push_back({p.first + xd[dir], p.second + yd[dir]});
                    added = true;
                    //std::cout << " - standard block, added to front\n";
                } else if(c == '#') {
                    can_move = false;
                    //std::cout << " - hit wall, can't move\n";
                    break;
                } else if(yd[dir] == 0 && (c == '[' || c == ']')) {
                    added = true;
                    front.push_back({p.first + (2*xd[dir]), p.second + (2*yd[dir])});
                    back.push_back({p.first + xd[dir], p.second + yd[dir]});
                } else if(xd[dir] == 0 && (c == '[' || c == ']')) {
                    added = true;
                    int ox = c == '[' ? p.first+1 : p.first-1;
                    //std::cout << " - removing "<<ox<<","<<p.second<<" in case we push on both halves\n";
                    if(std::erase(front, std::pair<int,int>(ox, p.second)) == 1) {
                        back.push_back({ox, p.second});
                    }
                    //std::cout << " - adding "<<(p.first)<<","<<(p.second+yd[dir])<<" and "<<ox<<","<<(p.second+yd[dir])<<"\n";
                    front.push_back({p.first, p.second+yd[dir]});
                    front.push_back({ox, p.second+yd[dir]});
                }

                // move this element into back, and decrement i so we check the next element in front
                //std::cout << " : moving tile " << i << " " << front[i].first<<","<<front[i].second<<" to back (len "<<back.size()<<")\n";
                back.push_back(front[i]);
                front.erase(front.begin() + i);
                i--;
            } 
        }

        if(can_move) {
            front.insert(front.end(), back.begin(), back.end());
            std::sort(front.begin(), front.end(), [dir,xd,yd](const std::pair<int,int> &a, const std::pair<int,int> &b) { 
                if((dir & 1) == 0) { return a.first * xd[dir] > b.first * xd[dir]; }
                else { return a.second * yd[dir] > b.second * yd[dir]; }});

            for(auto p : front) {
                map[p.second + yd[dir]][p.first + xd[dir]] = map[p.second][p.first];
                map[p.second][p.first] = '.';
            }

            x += xd[dir];
            y += yd[dir];
        }
    }

    uint64_t total = 0;
    for(y=0; y<map.size(); y++) {
        for(x=0; x<map[y].length(); x++) {
            if(map[y][x] == 'O' || map[y][x] == '[') {
                total += (y * 100) + x;                
            }
        }
    }

    return total;
}

int main(int argc, char *argv[]) {
    std::ifstream input(argc >= 2 ? argv[1] : "input.txt");
    std::string line;
    std::vector<std::string> map;
    std::string instructions("");

    bool in_map = true;

    while(std::getline(input, line)) {
        if(in_map) {
            if(line.length() == 0) {
                in_map = false;
                continue;
            }
            map.push_back(line);
        } else {
            instructions += line;
        }
    }

    std::cout << "Part 1: " << follow_instructions(map, instructions) << "\n";
    scale_map(map);
    std::cout << "Part 2: " << follow_instructions(map, instructions) << "\n";

    return 0;
}