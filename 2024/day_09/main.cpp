#include <string>
#include <iostream>
#include <fstream>
#include <map>

uint64_t move_files(std::string &line) {
    uint64_t total = 0;

    std::map<int,std::pair<int,int>> list;

    int i=0;
    int file_number = 0;
    int pos = 0;
    while(i < line.length()) {
        int len = line[i++] - '0';
        if(len > 0) {
            //std::cout << "Insert " << pos << ", " << len << "\n";
            list.insert({pos, std::pair<int,int>(file_number, len)});
            pos += len;
        }
        file_number++;
        if(i < line.length()) {
            len = line[i++] - '0';
            if(len > 0) {
                //std::cout << " - gap " << pos << ", " << len << "\n";
                list.insert({pos, std::pair<int,int>(-1, len)});
                pos += len;
            }
        }
    }

    auto files = list.end();
    files--;
    int last_file = files->second.first;
    while (files != list.begin()) {
        if(files->second.first >= 0 and files->second.first <= last_file) {
            //std::cout << "Looking for first gap for file " << files->second.first << ", currently at " << files->first << " with length " << files->second.second << "\n";
            last_file = files->second.first;
            auto gaps = list.begin();
            while(gaps != files) {
                if(gaps->second.first == -1) {
                    //std::cout << " - found gap of len " << gaps->second.second << "\n";
                    if(gaps->second.second >= files->second.second) {
                        //std::cout << " - moving into gap" << "\n";
                        gaps->second.first = files->second.first;
                        files->second.first = 0;

                        if (gaps->second.second > files->second.second) {
                            //std::cout << " - adding partial gap at " << gaps->first+files->second.second << " length " << gaps->second.second - files->second.second << "\n";
                            list.insert({gaps->first+files->second.second, std::pair<int,int>(-1, gaps->second.second - files->second.second)});
                            gaps->second.second = files->second.second;
                        }
                        break;
                    }
                }
                gaps++;
            }
        }
        files--;
    }

    for(files = list.begin(); files != list.end(); files++) {        
        //std::cout << files->first << " : " << files->second.first << ", " << files->second.second << "\n";

        if(files->second.first > 0) {
            for(int i=0; i<files->second.second; i++) {
                total += (files->first + i) * files->second.first;
            }
        }
    }

    return total;
}

uint64_t defrag(std::string line) {
    uint64_t total = 0;
    auto start = line.cbegin();
    auto end = line.cend()-1;
    int file_number = 0;
    int pos = 0;
    int end_file_length = *end - '0';
    int end_file_number = (line.length()-1)/2;

    while(start < end) {
        // starting a file from the left
        int file_length = *start++ - '0';
        //std::cout << "File: "<< file_length << "\n";
        for(int i=0; i<file_length; i++) {
            total += pos * file_number;
            //std::cout << pos << " Fill left: " << file_number << "\n";
            pos++;
        }
        file_number++;

        // now have a gap, fill it in from the files from the right
        int gap_length = *start++ - '0';
        //std::cout << "Gap: " << gap_length << " End file: " << end_file_length << "\n";
        while(gap_length > 0) {
            for(int i=0; i<std::min(gap_length, end_file_length); i++) {
                total += pos * end_file_number;
                //std::cout << pos << " Fill right: " << end_file_number << "\n";
                pos++;
                end_file_length--;
                gap_length--;
            }
            if(end_file_length == 0) {
                end -= 2;
                end_file_length = *end - '0';
                end_file_number--;
            }
        }

        //std::cout << "end of loop, gap is " << (end-start) << " with values " << *start << " and " << *end << "\n";
        //std::cout << "end_file_length " << end_file_length << " total " << total << "\n";
    }

    while(end_file_length > 0) {
        total += pos * end_file_number;
        //std::cout << pos << " terminal fill right: " << end_file_number << "\n";
        pos++;
        end_file_length--;
    }

    return total;
}

int main(int argc, char *argv[]) {
    std::ifstream input(argv[1]);
    std::string line;
    std::vector<std::string> map;

    std::getline(input, line);

    std::cout << "Part 1: " << defrag(line) << "\n";
    std::cout << "Part 2: " << move_files(line) << "\n";

    return 0;
}