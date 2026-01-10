#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <set>

std::string find_largest_cluster(std::vector<std::pair<uint,uint>> links, std::unordered_map<uint,std::set<uint>>& adj) {
    std::set<std::set<uint>> clusters;

    for(auto p : links) {
        for(auto it = adj[p.first].begin(); it != adj[p.first].end(); it++) {
            if(adj[(*it)].contains(p.second)) {
                std::set<uint> triple{p.first, p.second, *it};

                if(!clusters.contains(triple)) {
                    clusters.insert(triple);
                }
            }
        }
    }

    std::set<uint> biggest;

    while(true) {
        std::set<std::set<uint>> next_clusters;
        biggest = *clusters.begin();

        //std::cout << "\n" << biggest.size() << " " << clusters.size();

        // iterate through each cluster, and find if we can make it bigger
        for(auto it = clusters.begin(); it != clusters.end(); it++) {
            // pick one member
            auto iit = (*it).begin();
            uint member = *iit++;

            // now iterate through all the adjacencies of this one member
            for(auto cid = adj[member].begin(); cid != adj[member].end(); cid++) {
                // if it's not in the cluster..
                if(!(*it).contains(*cid)) {
                    bool missing = false;
                    // check if it's in all the adjcencies of the othe cluster members
                    for(auto iit = (*it).begin()++; iit != (*it).end(); iit++) {
                        if(!adj[*iit].contains(*cid)) {
                            missing = true;
                            break;
                        }
                    }
                    if(!missing) {
                        std::set<uint> ns = (*it);
                        ns.insert(*cid);
                        next_clusters.insert(ns);
                    }
                }
            }
        }
        clusters = next_clusters;
        if(clusters.size() == 0) {
            break;
        }
    }

    std::string ret;
    std::string sep;

    for(auto it = biggest.begin(); it != biggest.end(); it++) {
        ret += sep;
        ret += (char) ((*it / 100) + 'a' - 1);
        ret += (char) ((*it % 100) + 'a' - 1);
        sep = ",";
    }

    return ret;
}


bool starts_with_t(std::set<uint> triple) {
    for(auto it = triple.begin(); it != triple.end(); it++) {
        if(((*it) / 100) == 20) {
            return true;
        }
    }
    return false;
}

uint count_triples_t(std::vector<std::pair<uint,uint>> links, std::unordered_map<uint,std::set<uint>>& adj) {
    std::set<std::set<uint>> triples;

    for(auto p : links) {
        for(auto it = adj[p.first].begin(); it != adj[p.first].end(); it++) {
            if(adj[(*it)].contains(p.second)) {
                std::set<uint> triple{p.first, p.second, *it};

                if(!triples.contains(triple)) {
                    triples.insert(triple);
                }
            }
        }
    }

    return std::count_if(triples.begin(), triples.end(), starts_with_t);
}

int main(int argc, char *argv[]) {
    std::ifstream input(argc >= 2 ? argv[1] : "test_input.txt");
    std::string line;
    std::vector<std::pair<uint,uint>> links;

    while(std::getline(input, line)) {  
        links.emplace_back((line[1] - 'a' + 1) + ((line[0] - 'a' + 1) * 100), (line[4] - 'a' + 1) + ((line[3] - 'a' + 1) * 100));
    }

    std::unordered_map<uint,std::set<uint>> adj;

    for(auto p : links) {
        if(!adj.contains(p.first)) {
            adj[p.first] = std::set<uint>{p.second};
        } else {
            adj[p.first].insert(p.second);
        }

        if(!adj.contains(p.second)) {
            adj[p.second] = std::set<uint>{p.first};
        } else {
            adj[p.second].insert(p.first);
        }
    }
   
    std::cout << "Part 1: " << count_triples_t(links, adj) << "\n";
    std::cout << "Part 2: " << find_largest_cluster(links, adj) << "\n";

    return 0;
}