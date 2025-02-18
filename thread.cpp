#include <iostream>
#include <thread>

using namespace std;

void f1(char c) {
    for (int i = 0; i < 20000; i++) {
        cout << c;
    }
}

// void f2() {
//     for (int i = 0; i < 20000; i++) {
//         cout << "-";
//     }
// }

int main() {
    thread one(f1, '+');
    thread two(f1, '-');

    one.join();
    two.join();
    return 0;
}
