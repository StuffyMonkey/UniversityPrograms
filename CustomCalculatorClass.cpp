#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <fstream>
#include <sstream>
#include <stack>

using namespace std;
ifstream inf("/home/goodzone/CLionProjects/Calculator/input.txt");
ofstream ouf("/home/goodzone/CLionProjects/Calculator/output.txt");

vector<string> adv_tokenizer(std::string s, char del)
{
    std::stringstream ss(s);
    std::string word;
    std::vector<std::string> v;
    while (!ss.eof()) {
        getline(ss, word, del);
        v.push_back(word);
    }
    return v;
}

void print_queue(std::queue<char> q)
{
    while (!q.empty())
    {
        std::cout << q.front() << " ";
        q.pop();
    }
    std::cout << std::endl;
}

inline int intDivEx (int numerator, int denominator) {
    if (denominator == 0)
        throw std::overflow_error("Divide by zero exception");
    return numerator / denominator;
}

void calculate_expression(queue<char> & outp, map<char, pair<int, char>> map) {
    stack<int> stack;
    while(!outp.empty())
    {
        char c = outp.front();
        outp.pop();
        switch(c)
        {
            case '+':
            {
                int rhs=stack.top();
                stack.pop();
                int lhs=stack.top();
                stack.pop();
                int result=lhs+rhs;
                stack.push(result);
                break;
            }

            case '-':
            {
                int rhs=stack.top();
                stack.pop();
                int lhs=stack.top();
                stack.pop();

                int result;
                result=lhs-rhs;
                stack.push(result);
                break;
            }

            case '*':
            {
                int rhs=stack.top();
                stack.pop();
                int lhs=stack.top();
                stack.pop();
                int result=lhs*rhs;
                stack.push(result);
                break;
            }

            case '/':
            {
                    int rhs = stack.top();
                    stack.pop();
                    int lhs = stack.top();
                    stack.pop();
                    int result;
                    try { result = intDivEx(lhs, rhs); }
                    catch (const overflow_error e) { ouf << "ERROR" << endl;
                        return;}
                    stack.push(result);
                    break;

            }

            default:
                int number=(c-'0');
                stack.push(number);
                break;
        }
    }
    ouf << stack.top() << endl;
}

void Shunting_yard_algorithm(string s, map<char, pair<int, char>> map) {
    stack<char> operations;
    queue<char> outp;
    for (char & it : s) {
        if (isdigit(it))
            outp.push(it);
        else if (it == '+' || it == '-' || it == '*' || it == '/') {
            if (!operations.empty())
            while (operations.top() != '(' && ( (map[operations.top()].first > map[it].first) ||
                                               (map[operations.top()].first == map[it].first and
                                                map[it].second == 'L'))) {
                char value = operations.top();
                operations.pop();
                outp.push(value);
                if (operations.empty())
                    break;
            }
            operations.push(it);
        }
        else if (it == '(') {
            operations.push(it);
        }
        else if (it == ')') {
            if (!operations.empty())
            while (operations.top() != '(') {
                char value = operations.top();
                operations.pop();
                outp.push(value);
                if (operations.empty())
                    break;
            }
            // Удаляем евые скобки
            if (!operations.empty())
                operations.pop();
        }
    }
    while (!operations.empty()) {
        // assert the operator on top of the stack is not a parenthesis
        char value = operations.top();
        operations.pop();
        outp.push(value);
    }
    // print_queue(outp);
    calculate_expression(outp, map);
}

int main() {
    string s;
    map<char, pair<int, char>> operations;

    if (inf.is_open()) {
        // свойства операций
        for (int i=0; i<4; i++) {
            getline(inf, s);
            auto v = adv_tokenizer(s, ' ');
            char oper_code = v[0][0];
            char oper_type = v[2][0]; // R - ассоциативность справа, L - слева
            int oper_priority = atoi(v[1].c_str());
            operations[oper_code] = make_pair(oper_priority, oper_type);
        }
        getline(inf, s);
        Shunting_yard_algorithm(s, operations);
    }
    else {
        cout << "Input file doesn't exists!" << endl;
    }
}
