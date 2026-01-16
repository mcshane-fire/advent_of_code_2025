#include <string>
#include <fstream>
#include <iostream>
#include <regex>
#include <map>
#include <algorithm>

struct triple {
    uint red;
    uint green;
    uint blue;

    bool operator>(struct triple& n) {
        return red > n.red || green > n.green || blue > n.blue;
    }

    friend std::ostream& operator<< (std::ostream& stream, const struct triple &t) {
        stream << "(" << t.red << "," << t.green << "," << t.blue << ")";
        return stream;
    }
};

int count_possible(std::map<int,std::vector<struct triple>> games, struct triple possible) {
    int total = 0;

    for(auto game : games) {
        bool pos = true;
        for(auto p : game.second) {

            if(p > possible) {
                pos = false;
                break;
            }
        }
        if(pos) {
            total += game.first;
        }
    }
    return total;
}

int64_t count_powers(std::map<int,std::vector<struct triple>> games) {
    int64_t total = 0;

    for(auto game: games) {
        struct triple needed = {0, 0, 0};
        for(auto p : game.second) {
            needed.red = std::max(needed.red, p.red);
            needed.green = std::max(needed.green, p.green);
            needed.blue = std::max(needed.blue, p.blue);
        }
        total += needed.red * needed.green * needed.blue;
    }

    return total;
}

int main(int argc, char *argv[]) {
    std::ifstream input(argc >= 2 ? argv[1] : "test_input.txt");
    std::string line;
    std::regex number = std::regex("^Game (\\d+): ");
    std::regex game = std::regex("^[; ]*([^;]+)[; ]*");
    std::regex result = std::regex("^(\\d+) (red|green|blue)[, ]*");
    std::smatch res;

    std::map<int,std::vector<struct triple>> games;

    while(std::getline(input, line)) {
        std::string::const_iterator search(line.begin());
        if(std::regex_search(search, line.cend(), res, number)) {
            //std::cout << "Number " << res[1] << "\n";
            int game_number = stoi(res[1]);
            std::vector<struct triple> g;

            search = res.suffix().first;
            while(std::regex_search(search, line.cend(), res, game)) {
                //std::cout << "  Game '" << res[1] << "'\n";
                search = res[1].first;
                struct triple t = {0, 0, 0};

                while(std::regex_search(search, line.cend(), res, result)) {
                    //std::cout << "    Result " << res[1] << " : " << res[2] << "\n";
                    if(res[2] == "red") {
                        t.red = stoi(res[1]);
                    } else if (res[2] == "green") {
                        t.green = stoi(res[1]);
                    } else if (res[2] == "blue") {
                        t.blue = stoi(res[1]);
                    }

                    search = res.suffix().first;
                }  
                g.push_back(t);
            }

            games[game_number] = g;
        }
    }

    std::cout << "Part 1: " << count_possible(games, {12,13,14}) << "\n";
    std::cout << "Part 2: " << count_powers(games) << "\n";
        
    return 0;
}