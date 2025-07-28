#include <iostream>
#define print(x) std::cout << x << std::endl

int main() {
    // bitwise operators
    // &
    // |
    // ^
    // <<
    // >>

    int a = 12; // 00000110
    int b = 4; //  00000100
    int c = 8; //  00001000

    c = a >> 1;
    print(c); // OUTPUT: 8

    return 0;
}