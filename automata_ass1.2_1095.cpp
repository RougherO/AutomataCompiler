#include <string>
#include <iostream>

bool DFA(std::string const& str)
{
    return std::equal(str.begin(), str.begin() + str.size() / 2, str.rbegin());
}

int main()
{
    std::cout << "Enter string: ";
    std::string str;
    std::cin >> str;
    std::cout << (DFA(str) ? "Valid" : "Invalid") << "\n";
}