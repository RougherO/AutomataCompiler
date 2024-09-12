#include <iomanip>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <vector>

using namespace std;

struct DFA {
    map<int32_t, map<char, int32_t>> table;
    int32_t start_state;
    int32_t final_states;
    set<char> alphabets;
};

struct _DFA {
    map<string, map<char, string>> table;
    set<string> states;
    set<string> final_states;
    string start_state;
    set<char> alphabets;
};

int32_t remove_unreachable_states(DFA& dfa)
{
    int32_t reachable = 0;
    queue<int32_t> q;
    q.push(dfa.start_state);
    reachable |= dfa.start_state;

    while (!q.empty()) {
        int32_t state = q.front();
        q.pop();
        for (char c : dfa.alphabets) {
            int32_t next_state = dfa.table[state][c];
            if (next_state && !(reachable & next_state)) {
                reachable |= next_state;
                q.push(next_state);
            }
        }
    }

    return reachable;
}

DFA minimize_dfa(DFA& dfa)
{
    int32_t reachable = remove_unreachable_states(dfa);

    vector<int32_t> partition(32, -1);
    int32_t final_mask     = dfa.final_states;
    int32_t non_final_mask = reachable & ~final_mask;

    for (int i = 0; i < 32; ++i) {
        if (final_mask & (1 << i)) {
            partition[i] = 1;
        } else if (non_final_mask & (1 << i)) {
            partition[i] = 0;
        }
    }

    int max_partitions { 1 };

    queue<int32_t> W;
    W.push(non_final_mask);
    W.push(final_mask);

    bool changed { true };
    while (!W.empty() && changed) {
        changed   = false;
        int32_t A = W.front();
        W.pop();
        int32_t processed {};
        for (char c : dfa.alphabets) {
            int32_t X = 0;
            int prev_partition { -1 };
            for (int i = 0; i < 32; ++i) {
                if (processed & (1 << i)) continue;
                if (reachable & (1 << i)) {
                    if (dfa.table[1 << i][c] & A) {
                        if (prev_partition == -1) {
                            prev_partition = partition[i];
                        }
                        if (partition[i] == prev_partition) {
                            X         |= (1 << i);
                            processed |= (1 << i);
                        }
                    }
                }
            }

            if (X == 0 || X == A) continue;

            W.push(X);
            for (int i {}; i < 32; ++i) {
                if (X & (1 << i)) {
                    // if (partition[i] != max_partitions + 1) {
                    changed      = true;
                    partition[i] = max_partitions + 1;
                    // }
                }
            }
            max_partitions++;
        }
    }

    DFA minimized_dfa;
    minimized_dfa.alphabets = dfa.alphabets;
    map<int32_t, int32_t> state_map;
    map<int32_t, int32_t> partition_map;

    for (int i {}; i < 32; ++i) {
        if (reachable & (1 << i))
            partition_map[partition[i]] |= (1 << i);
    }

    for (int32_t i {}; i < 32; ++i) {
        if (reachable & (1 << i)) {
            state_map[1 << i] = partition_map[partition[i]];
        }
    }

    for (auto const& p : state_map) {
        int32_t old_state = p.first;
        int32_t new_state = p.second;
        for (char c : dfa.alphabets) {
            minimized_dfa.table[new_state][c] = state_map[dfa.table[old_state][c]];
        }
        if (new_state & final_mask) {
            minimized_dfa.final_states |= new_state;
        }
        if (new_state & dfa.start_state) {
            minimized_dfa.start_state |= new_state;
        }
    }

    return minimized_dfa;
}

// Output operator for DFA with bitwise operations
ostream& operator<<(ostream& os, _DFA& _dfa)
{
    os << "initial state: " << _dfa.start_state << "\n";

    os << "states: {";
    for (auto it = _dfa.states.begin(); it != _dfa.states.end(); ++it) {
        os << *it << (next(it) != _dfa.states.end() ? ", " : "");
    }
    os << "}\n";

    os << "alphabets: {";
    for (auto it = _dfa.alphabets.begin(); it != _dfa.alphabets.end(); ++it) {
        os << *it << (next(it) != _dfa.alphabets.end() ? ", " : "");
    }
    os << "}\n";

    os << "table:\n";
    for (auto const& state : _dfa.states) {
        for (char c : _dfa.alphabets) {
            os << setw(10) << left << state << " " << c << " " << _dfa.table[state][c] << "\n";
        }
    }

    os << "final states: {";
    for (auto it = _dfa.final_states.begin(); it != _dfa.final_states.end(); ++it) {
        os << *it << (next(it) != _dfa.final_states.end() ? ", " : "");
    }
    os << "}\n";

    return os;
}

string get_state(int32_t q, map<int32_t, string>& index_to_state)
{
    string ans {};
    for (int i {}; i < 32; ++i) {
        if (q & (1 << i)) {
            ans += index_to_state[1 << i];
        }
    }
    return ans;
}

DFA create_machine(_DFA& m, map<string, int32_t>& state_to_index)
{
    DFA machine;

    machine.start_state  = state_to_index[m.start_state];
    machine.alphabets    = m.alphabets;
    machine.final_states = 0;

    for (string const& state : m.final_states) {
        machine.final_states |= state_to_index[state];
    }

    for (string const& state : m.states) {
        for (char c : m.alphabets) {
            machine.table[state_to_index[state]][c] = state_to_index[m.table[state][c]];
        }
    }

    return machine;
}

_DFA create_machine(DFA& m, map<int32_t, string>& index_to_state)
{
    _DFA machine;

    machine.alphabets   = m.alphabets;
    machine.start_state = get_state(m.start_state, index_to_state);

    for (auto const& p1 : m.table) {
        machine.states.insert(get_state(p1.first, index_to_state));
        for (auto const& p2 : p1.second) {
            machine.table[get_state(p1.first, index_to_state)][p2.first] = get_state(m.table[p1.first][p2.first], index_to_state);
        }
        if (p1.first & m.final_states) {
            machine.final_states.insert(get_state(p1.first, index_to_state));
        }
    }

    return machine;
}

int main()
{
    _DFA _dfa;
    _dfa.alphabets        = set<char> { 'a', 'b' };
    _dfa.final_states     = set<string> { "q3", "q4", "q5" };
    _dfa.states           = set<string> { "q0", "q1", "q2", "q3", "q4", "q5", "q6" };
    _dfa.start_state      = "q0";
    _dfa.table["q0"]['a'] = "q1";
    _dfa.table["q0"]['b'] = "q2";
    _dfa.table["q1"]['a'] = "q3";
    _dfa.table["q1"]['b'] = "q4";
    _dfa.table["q2"]['a'] = "q3";
    _dfa.table["q2"]['b'] = "q5";
    _dfa.table["q3"]['a'] = "q3";
    _dfa.table["q3"]['b'] = "q1";
    _dfa.table["q4"]['a'] = "q4";
    _dfa.table["q4"]['b'] = "q5";
    _dfa.table["q5"]['a'] = "q5";
    _dfa.table["q5"]['b'] = "q4";
    _dfa.table["q6"]['a'] = "q2";
    _dfa.table["q6"]['b'] = "q6";

    cout << "Original DFA:\n";
    cout << _dfa;

    int i {};
    map<string, int32_t> state_to_index;
    map<int32_t, string> index_to_state;
    for (string const& state : _dfa.states) {
        state_to_index[state]  = 1 << i;
        index_to_state[1 << i] = state;
        i++;
    }

    DFA dfa           = create_machine(_dfa, state_to_index);
    DFA minimized_dfa = minimize_dfa(dfa);

    _DFA _minimized_dfa = create_machine(minimized_dfa, index_to_state);

    cout << "\nMinimized DFA:\n";
    cout << _minimized_dfa;
}