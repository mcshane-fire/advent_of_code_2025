#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <numeric>
#include <regex>

int count_winnings(std::map<uint,std::pair<std::set<uint>,std::set<uint>>>& cards) {
    int total = 0;

    for(auto card : cards) {
        int matched = 0;
        for(uint num : card.second.first) {
            if(card.second.second.contains(num)) {
                matched++;
            }
        }

        if(matched > 0) {
            total += 1<<(matched-1);
        }
    }

    return total;
}

int64_t count_cards(std::map<uint,std::pair<std::set<uint>,std::set<uint>>>& cards) {
    int64_t total = 0;

    std::map<uint,int64_t> count;

    for(auto card : cards) {
        count[card.first] = 1;
    }

    for(auto card : cards) {
        int matched = 0;
        for(uint num : card.second.first) {
            if(card.second.second.contains(num)) {
                matched++;
            }
        }

        for(uint i=1; i<=matched; i++) {
            if(count.contains(card.first+i)) {
                count[card.first+i] += count[card.first];
            }
        }
    }

    return std::accumulate(count.begin(), count.end(), 0, [](const int64_t prev, const std::pair<uint,int64_t>& p) { return prev + p.second; });
}

int main(int argc, char *argv[]) {
    std::ifstream input(argc >= 2 ? argv[1] : "test_input.txt");
    std::string line;
    std::map<uint,std::pair<std::set<uint>,std::set<uint>>> cards;
    std::regex card = std::regex("^Card[ ]+(\\d+): (.*)$");
    std::smatch res;

    while(std::getline(input, line)) {        
        if(std::regex_search(line, res, card)) {            
            uint number = stoi(res[1]);
            std::set<uint> winning;
            std::set<uint> hand;
            std::stringstream ss(res[2]);
            std::string word;
            bool in_hand = false;
            while(getline(ss, word, ' ')) {
                if(word == "|") {
                    in_hand = true;
                }
                else if(word.length() > 0) {
                    if(in_hand) {
                        hand.insert(stoi(word));
                    } else {
                        winning.insert(stoi(word));
                    }
                }
            }
            cards[number] = {winning, hand};
        }
    }

    std::cout << "Part 1: " << count_winnings(cards) << "\n";
    std::cout << "Part 2: " << count_cards(cards) << "\n";
        
    return 0;
}