// Algorithm for creating DFA according with regular expression was taken from: http://esyr.org/wiki/%D0%9A%D0%BE%D0%BD%D1%81%D1%82%D1%80%D1%83%D0%B8%D1%80%D0%BE%D0%B2%D0%B0%D0%BD%D0%B8%D0%B5_%D0%9A%D0%BE%D0%BC%D0%BF%D0%B8%D0%BB%D1%8F%D1%82%D0%BE%D1%80%D0%BE%D0%B2%2C_%D0%90%D0%BB%D0%B3%D0%BE%D1%80%D0%B8%D1%82%D0%BC%D1%8B_%D1%80%D0%B5%D1%88%D0%B5%D0%BD%D0%B8%D1%8F_%D0%B7%D0%B0%D0%B4%D0%B0%D1%87#.D0.9F.D0.BE.D1.81.D1.82.D1.80.D0.BE.D0.B5.D0.BD.D0.B8.D0.B5_.D0.94.D0.9A.D0.90_.D0.BF.D0.BE_.D0.A0.D0.92
// Also I used this sourse: http://cmcstuff.esyr.org/vmkbotva-r15/2%20%D0%BA%D1%83%D1%80%D1%81/4%20%D0%A1%D0%B5%D0%BC%D0%B5%D1%81%D1%82%D1%80/%D0%9F%D1%80%D0%B0%D0%BA/%D0%94%D0%B7/regexp.pdf
#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <set>
#include <unordered_map>
#include <stack>
#include <cassert>
using namespace std;
ifstream inf("input.txt");
ofstream ouf("output.txt");

string r_polish_notation(string& exp);
string polish_notation(string& exp);

class vertex {
    friend class binary_tree;
    char letter = '~';
    int number = -1;
    vertex* left = nullptr;
    vertex* right = nullptr;

    vertex() {}

    vertex(char buff, int n) {
        letter = buff;
        number = n;
    }
};

bool search(set<pair<int, char>> temp, int elem) {
    for(auto it : temp) {
        if (it.first == elem)
            return true;
    }
        return false;
}

bool l_search(set<pair<int, char>> temp, char elem) {
    for(auto it : temp) {
        if (it.second == elem)
            return true;
    }
    return false;
}

class binary_tree {
private:
    vertex* root;
    int count = 0;

    void raw_clean(vertex* step) {
        if(step->left != nullptr)
            raw_clean(step->left);
        if(step->right != nullptr)
            raw_clean(step->right);
        delete(step);
    }
public:
    binary_tree(string& temp) {
        root = new vertex;
        temp = polish_notation(temp);
        create_tree(temp, root);
    }

    // recursive parse of regular expression with creating binary sintaxis tree
    void create_tree(string& temp, vertex* step) {
        static auto it = temp.begin();
        if(it == temp.end() || step == nullptr)
            return;
        if(step == root) {
            step->letter = '.';
            step->right = new vertex('#', count++);
            step->left = new vertex;
            create_tree(temp, step->left);
        }
        else {
            if(*it == '*') {
                step->letter = *it;
                it++;
                step->left = new vertex;
                create_tree(temp, step->left);
            }
            else if(*it == '.' || *it == '|') {
                step->letter = *it;
                it++;
                step->left = new vertex;
                step->right = new vertex;
                create_tree(temp, step->left);
                create_tree(temp, step->right);
            }
            else if(isalpha(*it)) {
                step->letter = *it;
                step->number = count++;
                it++;
            }
        }
    }

    // That's really sophisticated way of bypass tree, but in the regular expression multiplication isn't commutative
    // so there is no way to make it more simple (I guess)
    void go_through_tree(vertex* step) {
        if(step->left != nullptr)
            go_through_tree(step->left);
        if(step->right != nullptr)
            go_through_tree(step->right);
        cout << step->letter;
    }

    set<pair<int, char>> firstpos(vertex* step, set<pair<int, char>>& res) {
        if(step != nullptr) {
            if(isalpha(step->letter) || step->letter == '#') {
                res.insert(pair<int, char>(step->number, step->letter));
            }
            else if(step->letter == '|') {
                for(auto it : firstpos(step->left, res))
                    res.insert(it);
                for(auto it: firstpos(step->right, res))
                    res.insert(it);
            }
            else if(step->letter == '*') {
                for(auto it: firstpos(step->left, res))
                    res.insert(it);
            }
            else if(step->letter == '.') {
                if(nullable(step->left)) {
                    for(auto it : firstpos(step->left, res))
                        res.insert(it);
                    for(auto it: firstpos(step->right, res))
                        res.insert(it);
                }
                else
                    for(auto it : firstpos(step->left, res))
                        res.insert(it);
            }
        }
        return res;
    }

    set<pair<int, char>> lastpos(vertex* step, set<pair<int, char>>& res) {
        if(step != nullptr) {
            if(isalpha(step->letter) || step->letter == '#') {
                res.insert(pair<int, char>(step->number, step->letter));
            }
            else if(step->letter == '|') {
                for(auto it : lastpos(step->left, res))
                    res.insert(it);
                for(auto it: lastpos(step->right, res))
                    res.insert(it);
            }
            else if(step->letter == '*') {
                for(auto it: lastpos(step->left, res))
                    res.insert(it);
            }
            else if(step->letter == '.') {
                if(nullable(step->right)) {
                    for(auto it : lastpos(step->left, res))
                        res.insert(it);
                    for(auto it: lastpos(step->right, res))
                        res.insert(it);
                }
                else
                    for(auto it: lastpos(step->right, res))
                        res.insert(it);
            }
        }
        return res;
    }

    bool nullable(vertex* step) {
        if(step != nullptr) {
            if(isalpha(step->letter))
                return false;
            else if(step->letter == '*')
                return true;
            else if(step->letter == '|')
                return (nullable(step->left) || nullable(step->right));
            else if(step->letter == '.')
                return (nullable(step->left) && nullable(step->right));
        }
        return false;
    }

    // This function compute set of positions according to the following rule:
    // If i_x is position and there's set of j_x positions such that there's some string as ...cd..., which could be described by language of regular expression and
    // If that string must be satisfy the rules:
    // - that i_x is correspond entry of c
    // - that j_x is correspond entry of d
    // then followpos return that set of positions
    set<pair<int, char>> followpos(int i, vertex* step, set<pair<int, char>>& res) {
        set<pair<int, char>> temp1;
        set<pair<int, char>> temp2;
        if(step->left != nullptr)
            followpos(i, step->left, res);
        if(step->right != nullptr)
            followpos(i, step->right, res);

        if (step->letter == '*' && step->left != nullptr) {
            if (search(lastpos(step->left, temp1), i))
                for (auto it : firstpos(step->left, temp2))
                    res.insert(it);
        } else if (step->letter == '.') {
            if (search(lastpos(step->left, temp1), i))
                for (auto it : firstpos(step->right, temp2))
                    res.insert(it);
        }
        return res;
    }

    vertex* get_root() const { return root;}

    ~binary_tree() {
        raw_clean(root);
    }
};

template<>
struct std::hash<set<int>> {
    size_t operator() (const set<int>& A) const noexcept {
        size_t val = 0;
        std::hash<int> hash_f;
        for(auto it: A) {
            val += hash_f(it) % A.size();
        }
        return val;
    }
};

template<typename T>
bool find_set(const set<T>& temp, T k) {
    for(auto it : temp)
        if(it == k)
            return true;
    return false;
}

template<typename T>
using Aut_simulation = unordered_map<T, unordered_map<T, set<char>>>;

template<typename T>
class automat {
protected:
    friend int main();
    Aut_simulation<T> state;
    set<T> ends;
    int size = 0;
    T start;
    set<char> signature;

    bool _find_word_impl_(const string& temp, T key, unsigned int index = 0) {
        set<T> t;
        for(auto it = state[key].begin(); it != state[key].end(); it++) {
            if(it->second.find(temp[index]) != it->second.end()) {
                t.insert(it->first);
            }
        }
        if(index < temp.length())
            for(auto it : t) {
                if(_find_word_impl_(temp, it, index + 1))
                    return true;
            }
        if(index == temp.length() && find_set<T>(ends, key))
            return true;
        return false;
    }
public:
    explicit automat() {}

    automat(const Aut_simulation<T>& aut,const set<T>& ends_t, int size_tmp, T start_t,const set<char>& signature_t) {
        state = aut;
        ends = ends_t;
        size = size_tmp;
        start = start_t;
        signature = signature_t;
    }

    bool find_word(string& temp) {
        return _find_word_impl_(temp, start);
    }

    T get_start() {return start;}
};

struct NFA : public automat<int> {
    friend class DFA;
    NFA(const Aut_simulation<int>& aut,const set<int>& ends_t, int size_tmp, int start_t,const set<char>& sign_t) :
    automat<int>(aut, ends_t, size_tmp, start_t, sign_t) {}
};

class DFA : public automat<set<int>> {
public:
    explicit DFA(NFA& temp) : automat<set<int>>() {
        signature = temp.signature;
        start = {temp.start};
        determinization(temp);
    }

    explicit DFA(string& temp) {
        // parse regular expression to make alphabet
        for(auto it : temp)
            if(isalpha(it))
                signature.insert(it);
        binary_tree tree(temp);
        // initializing initial statement
        set<pair<int, char>> buff;
        set<pair<int, char>> step;
        for(auto it : tree.firstpos(tree.get_root(), buff)) {
            start.insert(it.first);
            step.insert(it);
        }
        set<set<pair<int, char>>> que;
        que.insert(step);
        queue<set<pair<int, char>>> real;
        real.push(step);
        while(!real.empty()) {
            real.pop();
            for(auto let : signature) {
                if(l_search(step, let)) {
                    // set of position which corresponds letter let
                    set<int> positions;
                    for(auto tmp: step) {
                        if(tmp.second == let)
                            positions.insert(tmp.first);
                    }
                    if(!positions.empty()) {
                        // new_state in union of result computing followpos on every element from positions
                        set<pair<int, char>> new_state;
                        for(auto tmp: positions) {
                            set<pair<int, char>> someth;
                            for(auto pairs : tree.followpos(tmp, tree.get_root(), someth))
                                new_state.insert(pairs);
                            someth.clear();
                        }
                        if(!new_state.empty()) {
                            // check if new_state there isn't in queue
                            if(que.find(new_state) == que.end()) {
                                que.insert(new_state);
                                real.push(new_state);
                            }
                            // there we must define transition function
                            set<int> from;
                            set<int> to;
                            for(auto f : step)
                                from.insert(f.first);
                            for(auto t : new_state)
                                to.insert(t.first);
                            state[from][to].insert(let);
                            state[to];
                            // also I should come up with idea how to define step
                        }
                    }
                }
            }
            step = real.front();
        }
        // then we must define set of ends()
        for(auto tmp : state)
            if(tmp.first.find(0) != tmp.first.end())
                ends.insert(tmp.first);
    }

    void determinization(NFA& temp) {
        // queue which is used to make iterations for determinization
        queue<set<int>> que;
        que.push(start);
        while(!que.empty()) {
            set<int> p = que.front();
            que.pop();
            for(auto c : signature) {
                set<int> buff;
                for(auto it : p) {
                    for(auto it1 : temp.state[it]) {
                        if(it1.second.find(c) != it1.second.end())
                            buff.insert(it1.first);
                    }
                }
                state[p][buff].insert(c);
                if(state.count(buff) == 0) {
                    que.push(buff);
                    state[buff];
                }
            }
        }
        // create ends statements
        for(auto it : state) {
            for(auto p : temp.ends) {
                if(it.first.find(p) != it.first.end())
                    ends.insert(it.first);
            }
        }
        // empty set = rubbish bin
    }
};

NFA init() {
    int n;      // number of states
    int start;      // number of initial state
    set<int> ends;  // end states
    set<char> signature;    // kit of symbols for automat's alphabet
    Aut_simulation<int> aut;
    // read data
    inf >> n;
    inf >> start;
    // number of end states
    int k;
    inf >> k;
    while(k-- > 0) {
        int buff;
        inf >> buff;
        ends.insert(buff);
    }
    // number of functional transitions
    inf >> k;
    while(k-- > 0) {
        int beg, end;
        char let;
        inf >> beg >> end >> let;
        aut[beg][end].insert(let);
        signature.insert(let);
    }
    return NFA(aut, ends, n, start, signature);
}


// create equivalent regular expression in reverse polish notation
// WARNING!!!
// This function used to create expression for polish notation, so that not reversed for reg expressions
string r_polish_notation(string& exp) {
    string result;
    stack<char> temp;
    for(auto it : exp) {
        if(isalpha(it))
            result += it;
        else if (it == '(' || it == '*')
            temp.push(it);
        else if(it == ')') {
            while(temp.top() != '(') {
                result += temp.top();
                temp.pop();
                assert(!temp.empty());
            }
            temp.pop();
        }
        else if(it == '|') {
            while(!temp.empty() && (temp.top() == '|' || temp.top() != '(')) {
                result += temp.top();
                temp.pop();
            }
            temp.push(it);
        }
        else if(it == '.') {
            while(!temp.empty() && temp.top() != '(') {
                result += temp.top();
                temp.pop();
            }
            temp.push(it);
        }
    }
    while(!temp.empty()) {
        result += temp.top();
        temp.pop();
    };
    return result;
}

char brackets_converter(char symbol) {
    if(symbol == '(')
        return ')';
    else if(symbol == ')')
        return '(';
    return symbol;
}

string polish_notation(string& exp) {
    string result;
    string temp;
    for(auto it = exp.rbegin(); it != exp.rend(); it++)
        temp += brackets_converter(*it);
    temp = r_polish_notation(temp);
    for(auto it = temp.rbegin(); it != temp.rend(); it++)
        result += brackets_converter(*it);
    return result;
}

// k is the number of string which will be checked for recognition of automat
template<typename T>
void check_words(automat<T>& inp, int k) {
    while(k-- > 0) {
        string temp;
        inf >> temp;
        if(inp.find_word(temp))
            ouf << "YES" << endl;
        else
            ouf << "NO" << endl;
    }
}

int main() {
    // creating NFA
    NFA aut = init();
    int k;
    inf >> k;
    check_words(aut, k);
    // this part for check correctness on additional task
    /* string stmp;
    inf >> stmp;
    DFA hor(stmp);
    inf >> k;
    check_words(hor, k); */
    return 0;
}
