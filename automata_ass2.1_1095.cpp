#include <cctype>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <string>

using namespace std;

struct nfa {
    map<string, map<char, set<string>>> table;
    set<string> states;
    set<string> final_states;
    set<char> alphabets;
};

ostream& operator<<(ostream& os, nfa& machine)
{
    for (auto const& state : machine.states) {
        for (char alphabet : machine.alphabets) {
            os << state << " " << alphabet << " [";
            auto const& cell = machine.table[state][alphabet];
            for (auto it = cell.begin(); it != cell.end(); ++it) {
                os << *it << (next(it) != cell.end() ? ", " : "");
            }
            os << "]\n";
        }
    }
    return os;
}

bool check_nfa(nfa& nfa_machine)
{
    for (string const& state : nfa_machine.states) {
        for (char c : nfa_machine.alphabets) {
            if (nfa_machine.table[state][c].size() > 1) {
                return true;
            }
        }
    }
    return false;
}

bool check(nfa& machine, string const& str)
{
    queue<string> q;
    q.push(*machine.states.begin());

    for (char c : str) {
        int sz = q.size();
        for (int i = 0; i < sz; ++i) {
            string const& curr = q.front();
            q.pop();
            for (char a : machine.alphabets) {
                if (c == a && machine.table[curr].find(a) != machine.table[curr].end()) {
                    for (string const& s : machine.table[curr][a]) {
                        q.push(s);
                    }
                }
            }
        }
    }

    while (!q.empty()) {
        if (q.front() == *machine.states.rbegin()) {
            return true;
        }
        q.pop();
    }

    return false;
}

int main()
{
    string input;
    cout << "Enter string: ";
    cin >> input;

    nfa start_with_one;
    start_with_one.alphabets        = set<char> { '0', '1' };
    start_with_one.table["q0"]['1'] = set<string> { "q1" };
    start_with_one.table["q1"]['0'] = set<string> { "q1" };
    start_with_one.table["q1"]['1'] = set<string> { "q1" };
    start_with_one.states           = set<string> { "q0", "q1" };
    start_with_one.final_states     = set<string> { "q1" };

    // cout << (check_nfa(start_with_one) ? "NFA" : "DFA") << "\n";

    cout << (check(start_with_one, input) ? "Valid string" : "Invalid string") << "\n";
}