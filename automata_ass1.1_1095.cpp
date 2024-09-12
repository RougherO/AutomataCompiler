#include <string>
#include <iostream>

enum class state {
    q0, /* Even zeroes even ones */
    q1, /* Even zeroes odd ones */
    q2, /* Odd zereos odd ones */
    q3  /* Odd zeroes even ones */
};

bool DFA(std::string const& str)
{
    state s = state::q0;

    for (char c : str) {
        switch (c) {
            case '0':
                switch (s) {
                    case state::q0:
                        s = state::q3;
                        break;
                    case state::q1:
                        s = state::q2;
                        break;
                    case state::q2:
                        s = state::q1;
                        break;
                    case state::q3:
                        s = state::q0;
                        break;
                }
                break;
            case '1':
                switch (s) {
                    case state::q0:
                        s = state::q1;
                        break;
                    case state::q1:
                        s = state::q0;
                        break;
                    case state::q2:
                        s = state::q3;
                        break;
                    case state::q3:
                        s = state::q2;
                        break;
                }
                break;
        }
    }

    return s == state::q0 || s == state::q1 || s == state::q3;
}

int main()
{
    std::string s;
    std::cout << "Enter string: ";
    std::cin >> s;
    std::cout << (DFA(s) ? "Valid string" : "Invalid string") << "\n";
}