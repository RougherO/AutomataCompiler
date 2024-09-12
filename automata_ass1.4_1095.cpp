#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iterator>

auto print_vec = [](std::vector<std::string> const& v) {
    if (v.size() == 0) {
        std::cout << "[]\n";
        return;
    }
    std::cout << "[";
    for (auto it = v.begin(); it != v.end() - 1; ++it) {
        std::cout << *it << ", ";
    }
    std::cout << v.back() << "]\n";
};
int main()
{
    std::vector<std::string> v1, v2;

    std::string line, word;
    std::stringstream linestream;

    std::cout << "Enter strings separated by space for 1st set: ";
    std::getline(std::cin, line);
    for (linestream.str(std::move(line)); linestream >> word;) {
        v1.emplace_back(std::move(word));
    }

    linestream.clear();
    linestream.str("");

    std::cout << "Enter strings separated by space for 2nd set: ";
    std::getline(std::cin, line);
    for (linestream.str(std::move(line)); linestream >> word;) {
        v2.emplace_back(std::move(word));
    }

    std::sort(v1.begin(), v1.end());
    std::sort(v2.begin(), v2.end());

    std::cout << "A: ";
    print_vec(v1);

    std::cout << "B: ";
    print_vec(v2);

    std::vector<std::string> result;

    std::set_union(v1.begin(), v1.end(), v2.begin(), v2.end(), std::back_inserter(result));
    std::cout << "A U B: ";
    print_vec(result);

    result.clear();

    if (v1.size() == 0) {
        result = v2;
    } else if (v2.size() == 0) {
        result = v1;
    } else {
        for (auto const& s1 : v1) {
            for (auto const& s2 : v2) {
                result.emplace_back(s1 + s2);
            }
        }
    }

    std::cout << "A o B: ";
    print_vec(result);
}