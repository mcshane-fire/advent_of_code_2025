#include <string>
#include <iostream>
#include <fstream>
#include <regex>
#include <numeric>

class Primes {
    public:
    Primes() {
        primes = new std::vector<uint>;
        primes->push_back(2);
        next = 0;
    }
    ~Primes() {
        delete primes;
    }

    void reset() {
        next = 0;
    }

    uint get() {
        //std::cout << "get() next=" << next << " size()=" << primes->size() << "\n";
        while(next >= primes->size()) {
            int len = 1000;
            bool mask[1000]; // mask[0] is prime true/false for last prime+1, mask[len-1] is prime+len
            uint offset = primes->back()+1;

            //std::cout << "Generating from " << offset;
            for(int i=0; i<len; i++) {
                mask[i] = true;
            }
            
            // eliminate multiples of known primes
            for(int i=0; i<primes->size(); i++) {
                uint prime = (*primes)[i];
                if(prime * prime > offset+len) {
                    break;
                }
                uint n = (prime * prime);
                if(n < offset) {
                    n = (((offset-1) / prime)+1) * prime;
                }
                while(n < offset+len) {
                    mask[n-offset] = false;
                    n += prime;
                }
            }

            // go through mask and add primes to list, eliminating more multiples if neccessary
            for(int i=0; i<len; i++) {
                if(mask[i] == true) {
                    uint prime = i+offset;
                    primes->push_back(prime);
                    //std::cout << " - added " << prime << "\n";
                    if(prime * prime <= offset+len) {
                        int n = prime * prime;
                        if(n < offset) {
                            n = (((offset-1) / prime)+1) * prime;
                        }
                        while(n < len) {
                            if(n >= offset) {
                                mask[n-offset] = false;
                            }
                            n += prime;
                        }                        
                    }
                }
            }
        }
        //std::cout << "Returning generated prime " << (*primes)[next] << "\n";
        return (*primes)[next++];
    }

    int num_primes() {
        return primes->size();
    }

    private:
    std::vector<uint> *primes;
    int next;
};

class Fraction {
    public:
    Fraction() {
        num = 0;
        denom = 1;
    }

    Fraction(int64_t n) {
        num = n;
        denom = 1;
    }

    Fraction(int64_t n, int64_t d) {
        if(d < 0) {
            d *= -1;
            n *= -1;
        }

        int64_t gcd = std::gcd(n, d);
        num = n / gcd;
        denom = d / gcd;
    }

    Fraction operator+(const Fraction& f) {
        return Fraction((num * f.denom) + (denom * f.num), denom * f.denom);
    }

    Fraction operator*(const Fraction& f) {
        return Fraction(num * f.num, denom * f.denom);
    }

    Fraction operator-(const Fraction& f) {
        return Fraction((num * f.denom) - (denom * f.num), denom * f.denom);   
    }

    Fraction operator/(const Fraction& f) {
        return Fraction(num * f.denom, denom * f.num);
    }

    friend std::ostream& operator<<(std::ostream& os, const Fraction& frac) {
        if(frac.denom == 1) {
            os << frac.num;
        } else {
            os << frac.num << "/" << frac.denom;
        }
        return os;
    }

    bool is_integer() {
        return denom == 1;
    }

    int64_t get_val() {
        return num / denom;
    }

    private:
    int64_t num;
    int64_t denom;

};

struct Problem {
    std::pair<Fraction,Fraction> button_A;
    std::pair<Fraction,Fraction> button_B;
    std::pair<Fraction,Fraction> prize;
};

int64_t solve_problem(struct Problem *p) {
    //std::cout << "solve_problem: " << p->button_A.first << "," << p->button_A.second << " + " << p->button_B.first << "," << p->button_B.second << " -> " << p->prize.first << "," << p->prize.second << "\n";

    Fraction mul = p->button_A.second / p->button_A.first;

    Fraction bmul = p->button_B.second - (p->button_B.first * mul);
    Fraction res = p->prize.second - (p->prize.first * mul);
    Fraction b = res / bmul;

    if(b.is_integer()) {
        Fraction a = (p->prize.first - (p->button_B.first * b)) / p->button_A.first;
        //std::cout << "Solution: " << a << ", " << b << "\n\n";
        return (3 * a.get_val()) + b.get_val();
    
    } else {
        //std::cout << "Non-integer: mul " << mul << " res " << b << "\n\n";
        return 0;
    }
}

int main(int argc, char *argv[]) {
    std::ifstream input(argc == 2 ? argv[1] : "test_input.txt");
    std::string line;
    std::vector<struct Problem *> problems;
    std::regex button("Button ([AB]): X\\+(\\d+), Y\\+(\\d+)");
    std::regex prize("Prize: X=(\\d+), Y=(\\d+)");

    struct Problem *p;

    while(std::getline(input, line)) {
        std::smatch res;
        if(line.length() == 0) {
            continue;
        }
        if(std::regex_match(line, res, button)) {
            if(res[1] == 'A') {
                p = new struct Problem();
                p->button_A.first = Fraction(std::stoul(res[2]));
                p->button_A.second = Fraction(std::stoul(res[3]));
            } else {
                p->button_B.first = Fraction(std::stoul(res[2]));
                p->button_B.second = Fraction(std::stoul(res[3]));
            }
        } else if(std::regex_match(line, res, prize)) {
            p->prize.first = Fraction(std::stoul(res[1]));
            p->prize.second = Fraction(std::stoul(res[2]));
            problems.push_back(p);
        }
    }

    int64_t total = 0;
    for(auto p : problems) {
        total += solve_problem(p);
    }
    std::cout << "Part 1: " << total << "\n";

    total = 0;
    for(auto p : problems) {
        p->prize.first = p->prize.first + 10000000000000ULL;
        p->prize.second = p->prize.second + 10000000000000ULL;
        total += solve_problem(p);
    }
    std::cout << "Part 2: " << total << "\n";


    if(false) {
        Primes p;
        std::cout << "Primes: " << p.num_primes() << "\n";
        for(int i=0; i<10; i++) {
            std::cout << p.get() << "\n";
        }
        std::cout << "Primes: " << p.num_primes() << "\n";

        //std::cout << "Part 1: " << cost_fences(map, 0) << "\n";

        Fraction f = Fraction(42);
        Fraction f2 = Fraction(42,63);
        Fraction f3 = Fraction(1,2);
        std::cout << f << " " << f2 << " " << (f-f3) << " " << (f/f3) << "\n";
    }

    return 0;
}