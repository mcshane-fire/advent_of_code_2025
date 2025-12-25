#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>

void part1(std::vector<int> ca, std::vector<int> cb) {
    sort(ca.begin(), ca.end());
    sort(cb.begin(), cb.end());
    int total = 0;

    for (size_t i = 0; i < ca.size(); i++) {
        //std::cout << ca[i] << ", " << cb[i] << ": " << abs(ca[i] - cb[i]) << "\n";
        total += abs(ca[i] - cb[i]);
    }

    std::cout << "Part 1: " << total << "\n";
}

void part2(std::vector<int> ca, std::vector<int> cb) {
    int total = 0;

    for(size_t i = 0; i < ca.size(); i++) {
        total += ca[i] * std::count(cb.begin(), cb.end(), ca[i]);
    }

    std::cout << "Part 2: " << total << "\n";
}

int main(int argc, char *argv[]) {
    std::ifstream input(argv[1]);
    std::string line;
    std::vector<int> ca, cb;

    while( std::getline( input, line ) ) {
        //std::cout<<line<<'\n';
        std::stringstream stream(line);
        int a, b;
        stream >> a;
        stream >> b;
        ca.push_back(a);
        cb.push_back(b);
        //std::cout<<"Found int " <<a<<" and "<<b<<"\n"; 
    }

    part1(ca, cb);
    part2(ca, cb);

    return 0;
}