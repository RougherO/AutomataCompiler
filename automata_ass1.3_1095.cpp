#include <iostream>

int DFA()
{
    static int i = 1, j = 0, k = 0;

    if (k == 10) {
        k = 0;
        j++;
    }
    if (j == 10) {
        j = 0;
        i++;
    }
    if (i == 10) {
        return -1;
    }

    return 100 * i + 10 * j + k++;
}

int main()
{
    std::cout << "All 3 digit numbers:\n";
    int nxt;
    while ((nxt = DFA()) != -1) {
        std::cout << nxt << "\n";
    }
}