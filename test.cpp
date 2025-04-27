#include <iostream>
#include <vector>

#define print(x) std::cout << x << std::endl

int main() {
    std::vector<int> v1 = {1,2,3,4};
    
    for (int i = 0; i < v1.size(); ++i) {
        print(v1[i]);
    }

    for (auto itr = v1.begin(); itr != v1.end(); ++itr) { // itirate through memory locations
        print(*itr);
    }
}