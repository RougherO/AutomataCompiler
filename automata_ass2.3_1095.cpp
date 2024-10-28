#include <ctime>
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

ostream& operator<<(ostream& os, nfa& machine)
{
    os << (!machine.alphabets.count('_') ? "NFA\n" : "E-NFA\n");
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
            os << state << " " << alphabet << " {";
            auto const& cell = machine.table[state][alphabet];
            for (auto it = cell.begin(); it != cell.end(); ++it) {
                os << *it << (next(it) != cell.end() ? ", " : "");
            }
            os << "}\n";
        }
    }

    os << "final states: {";
    for (auto it = machine.final_states.begin(); it != machine.final_states.end(); ++it) {
        os << *it << (next(it) != machine.final_states.end() ? ", " : "");
    }
    os << "}\n";

    return os;
}

struct dfa {
    map<string, map<char, string>> table;
    set<string> states;
    set<string> final_states;
    set<char> alphabets;
};

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

set<string> epsilon_closure(string const& state, nfa const& machine)
{
    set<string> closure = { state };
    queue<string> q;
    q.push(state);

    while (!q.empty()) {
        string curr_state = q.front();
        q.pop();

        if (machine.table.at(curr_state).count('_')) {
            for (auto const& next_state : machine.table.at(curr_state).at('_')) {
                if (closure.find(next_state) == closure.end()) {
                    closure.insert(next_state);
                    q.push(next_state);
                }
            }
        }
    }

    return closure;
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

    string dead_state = "q" + std::to_string(m.states.size());
    for (char c : machine.alphabets) {
        machine.table[dead_state][c] = dead_state;
    }

    bool need_dead {};
    for (string state : machine.states) {
        for (char c : machine.alphabets) {
            if (machine.table[state][c].size() == 0) {
                machine.table[state][c] = dead_state;
                need_dead               = true;
            }
        }
    }

    if (need_dead) {
        machine.states.insert(dead_state);
    }

    return machine;
}

dfa e_nfa_to_dfa(nfa& m)
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
    for (auto const& state : m.states) {
        for (char c : m.alphabets) {
            for (string const& t_state : m.table[state][c]) {
                set<string> closure = epsilon_closure(t_state, m);
                // for (string const& s : closure) {
                // auto const& s_states = m.table.at(s).at(c);
                m.table.at(state).at(c).insert(closure.begin(), closure.end());
                // if (m.final_states.find(s) != m.final_states.end()) {
                //     m.final_states.insert(state);
                // }
                // }
                for (auto const& s : m.table[state][c]) {
                    transition_map[{ c, state_to_index[state] }] |= state_to_index[s];
                }
            }
        }
    }

    m.alphabets.erase('_');

    cout << m << "\n";

    return nfa_to_dfa(m);
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
            cout << "Enter alphabet and state['_' for epsilon]: ";
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

int main()
{
    nfa epsilon_nfa_machine;
    epsilon_nfa_machine.alphabets        = set<char> { 'a', 'b', '_' };
    epsilon_nfa_machine.states           = set<string> { "q0", "q1", "q2" };
    epsilon_nfa_machine.final_states     = set<string> { "q2" };
    epsilon_nfa_machine.table["q0"]['a'] = set<string> { "q0" };
    epsilon_nfa_machine.table["q0"]['_'] = set<string> { "q1" };
    epsilon_nfa_machine.table["q1"]['b'] = set<string> { "q1" };
    epsilon_nfa_machine.table["q1"]['_'] = set<string> { "q2" };
    epsilon_nfa_machine.table["q2"]['a'] = set<string> { "q2" };
    epsilon_nfa_machine.table["q2"]['b'] = set<string> { "q2" };

    cout << epsilon_nfa_machine << "\n";

    dfa dfa_machine = e_nfa_to_dfa(epsilon_nfa_machine);

    cout << dfa_machine;
}