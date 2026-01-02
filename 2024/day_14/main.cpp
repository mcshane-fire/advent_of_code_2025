#include <string>
#include <iostream>
#include <fstream>
#include <regex>

struct robot {
    int x, y;
    int dx, dy;
};

int score_quadrants(std::vector<struct robot> robots, int width, int height, int seconds, bool tree, bool print) {
    int q[4];

    std::vector<std::string> map;

    for(int i=0; i<height; i++)  {
        map.push_back(std::string(width, ' '));
    }

    q[0] = q[1] = q[2] = q[3] = 0;

    for(struct robot r : robots) {
        int x = (r.x + (r.dx * seconds)) % width;
        int y = (r.y + (r.dy * seconds)) % height;

        if(x < 0) {
            x += width;
        }
        if(y < 0) {
            y += height;
        }

        map[y][x] = '#';

        if(tree) {
            // assume a tree has very few points top left and top right triangles
            // turns out this is very incorrect, but got the right result with score=40
            if(x + y < width/2) {
                q[0]++;
            }
            if(y + width - x < width/2) {
                q[1]++;
            }
        } else {
            if(x != (width/2) && y != (height/2)) {
                int qx = x > (width/2);
                int qy = y > (height/2);
                q[qx + (2*qy)]++;
            }
        }
    }

    if(print) {
        for(auto s : map) {
            std::cout << s << "\n";
        }
    }

    int total = 1;

    for(int i=0; i<4; i++) {
        if(tree) {
            total += q[i];
        } else {
            total *= q[i];
        }
    }

    return total;
}

int main(int argc, char *argv[]) {
    std::ifstream input(argc >= 2 ? argv[1] : "input.txt");
    std::string line;
    std::regex desc("p=(\\d+),(\\d+) v=(-?\\d+),(-?\\d+)");
    std::vector<struct robot> robots;

    while(std::getline(input, line)) {
        std::smatch res;
        if(line.length() == 0) {
            continue;
        }
        if(std::regex_match(line, res, desc)) {
            robots.emplace_back(stoi(res[1]), stoi(res[2]), stoi(res[3]), stoi(res[4]));
        }
    }

    std::cout << "Part 1: " << score_quadrants(robots, 101, 103, 100, false, false) << "\n";

    for(int i=0; i<(argc >= 3 ? std::stoi(argv[2]) : 10); i++) {
        std::cout << i << " " << score_quadrants(robots, 101, 103, i, true, false) << "\n";
    }

    score_quadrants(robots, 101, 103, 8159, true, true);

    return 0;
}