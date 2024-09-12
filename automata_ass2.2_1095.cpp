#include <iomanip>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <cstdint>

using namespace std;

struct nfa {
    map<string, map<char, set<string>>> table;
    set<string> states;
    set<string> final_states;
    set<char> alphabets;
};

struct dfa {
    map<string, map<char, string>> table;
    set<string> states;
    set<string> final_states;
    set<char> alphabets;
};

pair<string, bool> get_state(int32_t q, map<int32_t, string>& index_to_state, nfa const& nfa_machine)
{
    string ans {};
    bool is_final {};
    for (int i {}; i < 32; ++i) {
        if ((1 << i) & q) {
            if (nfa_machine.final_states.find(index_to_state[1 << i]) != nfa_machine.final_states.end()) {
                is_final = true;
            }
            ans += index_to_state[1 << i];
        }
    }

    return { ans, is_final };
}

int32_t f(int32_t q, char c, map<pair<char, int32_t>, int32_t> transition_map)
{
    if ((q & (q - 1)) == 0) {
        return transition_map[{ c, q }];
    }

    int32_t ans {};
    for (int i {}; i < 32; ++i) {
        if ((1 << i) & q) {
            ans |= f(1 << i, c, transition_map);
        }
    }

    return ans;
}

dfa nfa_to_dfa(nfa& m)
{
    map<string, int32_t> state_to_index;
    map<int32_t, string> index_to_state;

    int i {};

    for (auto it = m.states.begin(); it != m.states.end(); ++it) {
        state_to_index[*it]    = (1 << i);
        index_to_state[1 << i] = *it;
        ++i;
    }

    map<pair<char, int32_t>, int32_t> transition_map;
    for (char c : m.alphabets) {
        for (auto const& state : m.states) {
            for (auto const& s : m.table[state][c]) {
                transition_map[{ c, state_to_index[state] }] |= state_to_index[s];
            }
        }
    }

    dfa machine;
    machine.alphabets = m.alphabets;

    queue<int32_t> q;
    q.push(state_to_index[*m.states.begin()]);

    map<int32_t, bool> is_done;
    while (!q.empty()) {
        int32_t curr = q.front();
        q.pop();

        if (is_done[curr]) continue;
        is_done[curr] = true;

        auto const& p     = get_state(curr, index_to_state, m);
        string curr_state = p.first;
        bool is_final     = p.second;
        if (is_final) {
            machine.final_states.insert(curr_state);
        }
        machine.states.insert(curr_state);
        for (char c : m.alphabets) {
            int32_t q1 = f(curr, c, transition_map);
            if (q1 == 0) continue;
            string next_state            = get_state(q1, index_to_state, m).first;
            machine.table[curr_state][c] = next_state;
            q.push(q1);
        }
    }

    return machine;
}

[[nodiscard]] nfa create_machine()
{
    nfa nfa_machine;

    int n;
    cout << "Enter number of states: ";
    cin >> n;

    for (int i {}; i < n; ++i) {
        cout << "Enter state: ";
        string state;
        cin >> state;
        nfa_machine.states.insert(state);
        cout << "Enter outdegree: ";
        int outdegree;
        cin >> outdegree;
        for (int o {}; o < outdegree; ++o) {
            cout << "Enter alphabet and state: ";
            char c;
            cin >> c;
            nfa_machine.alphabets.insert(c);
            string tstate;
            cin >> tstate;
            nfa_machine.table[state][c].insert(tstate);
        }
    }

    string states;
    cout << "Enter final states: ";

    while (!isalnum(cin.peek())) {
        cin.ignore();
    }

    getline(cin, states);

    stringstream line(states);
    while (line >> states)
        nfa_machine.final_states.insert(states);

    return nfa_machine;
}

ostream& operator<<(ostream& os, nfa& machine)
{
    os << "NFA\n";
    os << "initial state: " << *machine.states.begin() << "\n";

    os << "states: {";
    for (auto it = machine.states.begin(); it != machine.states.end(); ++it) {
        os << *it << (next(it) != machine.states.end() ? ", " : "");
    }
    os << "}\n";

    os << "alphabets: {";
    for (auto it = machine.alphabets.begin(); it != machine.alphabets.end(); ++it) {
        os << *it << (next(it) != machine.alphabets.end() ? ", " : "");
    }
    os << "}\n";

    os << "table:\n";
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

    os << "final states: {";
    for (auto it = machine.final_states.begin(); it != machine.final_states.end(); ++it) {
        os << *it << (next(it) != machine.final_states.end() ? ", " : "");
    }
    os << "}\n";

    return os;
}

ostream& operator<<(ostream& os, dfa& machine)
{
    os << "DFA\n";
    os << "initial state: " << *machine.states.begin() << "\n";

    os << "states: {";
    for (auto it = machine.states.begin(); it != machine.states.end(); ++it) {
        os << *it << (next(it) != machine.states.end() ? ", " : "");
    }
    os << "}\n";

    os << "alphabets: {";
    for (auto it = machine.alphabets.begin(); it != machine.alphabets.end(); ++it) {
        os << *it << (next(it) != machine.alphabets.end() ? ", " : "");
    }
    os << "}\n";

    os << "table:\n";
    for (auto const& state : machine.states) {
        for (char alphabet : machine.alphabets) {
            os << setw(10) << left << state << " " << alphabet << " " << machine.table[state][alphabet] << "\n";
        }
    }

    os << "final states: {";
    for (auto it = machine.final_states.begin(); it != machine.final_states.end(); ++it) {
        os << *it << (next(it) != machine.final_states.end() ? ", " : "");
    }
    os << "}\n";

    return os;
}

int main()
{
    // nfa nfa_machine { create_machine() };

    nfa nfa_machine;
    nfa_machine.alphabets        = set<char> { 'a', 'b' };
    nfa_machine.states           = set<string> { "q0", "q1" };
    nfa_machine.final_states     = set<string> { "q1" };
    nfa_machine.table["q0"]['a'] = set<string> { "q0", "q1" };
    nfa_machine.table["q0"]['b'] = set<string> { "q0" };

    cout << nfa_machine << "\n";

    dfa dfa_machine = nfa_to_dfa(nfa_machine);

    cout << dfa_machine;
}
// 3 q0 2 a q0 a q1 q1 2 b q0 b q2 q2 0 q1