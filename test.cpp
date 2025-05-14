#include <iostream>
#include <map>

#define print(x) std::cout << x << std::endl

int main() {
    std::map<char, int> mp;
    mp = {
        {'a', 0},
        {'b', 1},
        {'c', 2},
    };

    for (auto itr = mp.begin(); itr != mp.end(); ++itr ) {
        print(itr->second);
    }
}