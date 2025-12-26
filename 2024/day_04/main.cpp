#include <string>
#include <iostream>
#include <fstream>

int count_instances(std::vector<std::string> lines, std::string word) {
    int count = 0;
    int yd[8] = {0, -1, -1, -1, 0, 1, 1, 1};
    int xd[8] = {1, 1, 0, -1, -1, -1, 0, 1};

    for(size_t y = 0; y < lines.size(); y++) {
        for(size_t x = 0; x < lines[y].length(); x++) {
            if(lines[y][x] == word[0]) {
                //std::cout << y << ", " << x << "\n";
                for(size_t d = 0; d < 8; d++) {
                    int i = 1;
                    int nx = x;
                    int ny = y;
                    while(i < word.length()) {
                        ny += yd[d];
                        nx += xd[d];
                        if(ny < 0 || ny >= lines.size() || nx < 0 || nx >= lines[ny].length() || lines[ny][nx] != word[i]) {
                            break;
                        }
                        i++;
                    }
                    if(i == word.length()) {
                        count += 1;
                    }
                }
            }
        }
    }

    return count;
}

int count_crosses(std::vector<std::string> lines, std::string word) {
    int count = 0;

    for(size_t y = 1; y < lines.size()-1; y++) {
        for(size_t x = 1; x < lines[y].length()-1; x++) {
            if(lines[y][x] == word[1]) {
                char tl = lines[y-1][x-1];
                char tr = lines[y-1][x+1];
                char bl = lines[y+1][x-1];
                char br = lines[y+1][x+1];

                if((tl == word[0] && br == word[2]) || (tl== word[2] && br == word[0])) {
                    if ((tr == word[0] && bl == word[2]) || (tr == word[2] && bl == word[0])) {
                        count += 1;
                    }
                }
            }
        }
    }
    return count;
}

int main(int argc, char *argv[]) {
    std::ifstream input(argv[1]);
    std::string line;
    std::vector<std::string> lines;

    while(std::getline(input, line)) {
        lines.push_back(line);
        //std::cout << "'" << line << "'" << "\n";
    }

    std::cout << "Part 1: " << count_instances(lines, "XMAS") << "\n";
    std::cout << "Part 2: " << count_crosses(lines, "MAS") << "\n";

    return 0;
}