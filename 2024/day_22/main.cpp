#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <set>

uint capture_sequences(std::vector<int64_t> seeds, uint count) {
    std::unordered_map<uint,uint> scores;
    uint best_score = 0;
    //uint best_sequence = 0;

    for(uint si = 0; si < seeds.size(); si++) {
        int64_t s = seeds[si];
        int64_t last = 0;
        uint changes[4] = {0, 0, 0, 0};
        std::set<uint> seen;
        for(uint i=0; i<count; i++) {

            s ^= s<<6;
            s %= 16777216;
            s ^= s>>5;
            s %= 16777216;
            s ^= s<<11;
            s %= 16777216;
            
            if(i > 0) {
                changes[i&3] = (s%10)-last+10;;
            }

            if(i > 3) {
                uint pat = changes[(i+1)&3] + (changes[(i+2)&3] * 20) + (changes[(i+3)&3] * 20*20) + (changes[i&3] * 20*20*20);

                if(!seen.contains(pat)) {
                    seen.insert(pat);
                    if(!scores.contains(pat)) {
                        scores[pat] = s%10;
                    } else {
                        scores[pat] += s%10;

                        if(scores[pat] > best_score) {
                            best_score = scores[pat];
                            //best_sequence = pat;
                        }
                    }
                }
            }

            last = s%10;
        }    
    }

    //std::cout << best_score << " " << ((int) (best_sequence % 20)-10) << "," << ((int)((best_sequence/20)%20)-10) << "," << ((int)((best_sequence/400)%20)-10) << "," << ((int)((best_sequence/8000)%20)-10) << ","<< "\n";
    return best_score;
}

int64_t iterate_seeds(std::vector<int64_t> seeds, uint count) {
    int64_t total = 0;

    for(int64_t s : seeds) {
        for(uint i=0; i<count; i++) {
            s ^= s<<6;
            s %= 16777216;
            s ^= s>>5;
            s %= 16777216;
            s ^= s<<11;
            s %= 16777216;
        }
        //std::cout << s << "\n";
        total += s;
    }

    return total;
}

int main(int argc, char *argv[]) {
    std::ifstream input(argc >= 2 ? argv[1] : "test_input.txt");
    std::string line;
    std::vector<int64_t> seeds;

    while(std::getline(input, line)) {  
        seeds.push_back(stoll(line));
    }
        
    std::cout << "Part 1: " << iterate_seeds(seeds, 2000) << "\n";
    std::cout << "Part 2: " << capture_sequences(seeds, 2000) << "\n";

    return 0;
}