// File: calculator.cpp
#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <functional>
#include <stdexcept>
#include <fstream>
#include <ctime>
#include <stack>
#include <cctype>
#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

double lastResult = 0.0;

void applyConsoleColor(int colorCode) {
#ifdef _WIN32
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorCode);
#else
    cout << "\033[1;3" << colorCode << "m";
#endif
}

void resetConsoleColor() {
#ifdef _WIN32
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
#else
    cout << "\033[0m";
#endif
}

void renderInterfaceHeader() {
    applyConsoleColor(3);
    cout << "===============================================\n";
    cout << "         ADVANCED MULTIFUNCTION CALCULATOR       \n";
    cout << "===============================================\n";
    resetConsoleColor();
}

void renderInterfaceFooter() {
    applyConsoleColor(5);
    cout << "\n-----------------------------------------------\n";
    cout << "                  AZD                           \n";
    cout << "-----------------------------------------------\n";
    resetConsoleColor();
}

int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

double applyOperator(double a, double b, char op) {
    switch (op) {
    case '+': return a + b;
    case '-': return a - b;
    case '*': return a * b;
    case '/':
        if (b == 0) throw runtime_error("Division by zero");
        return a / b;
    default: throw runtime_error("Unsupported operator");
    }
}

double parseFunction(const string& name, double arg) {
    if (name == "sin") return sin(arg);
    if (name == "cos") return cos(arg);
    if (name == "tan") return tan(arg);
    if (name == "sqrt") return sqrt(arg);
    if (name == "log") return log(arg);
    throw runtime_error("Unknown function: " + name + "\nTry: sin, cos, tan, sqrt, log");
}

double evaluateExpression(string expr) {
    size_t pos;
    while ((pos = expr.find("ans")) != string::npos) {
        expr.replace(pos, 3, to_string(lastResult));
    }

    stack<double> values;
    stack<char> ops;
    size_t i = 0;
    while (i < expr.size()) {
        if (isspace(expr[i])) {
            ++i;
            continue;
        }
        if (isalpha(expr[i])) {
            string func;
            while (i < expr.size() && isalpha(expr[i])) func += expr[i++];
            if (expr[i] != '(') throw runtime_error("Expected '(' after function name");
            int start = ++i, count = 1;
            while (i < expr.size() && count > 0) {
                if (expr[i] == '(') count++;
                else if (expr[i] == ')') count--;
                ++i;
            }
            if (count != 0) throw runtime_error("Mismatched parentheses in function argument");
            string argStr = expr.substr(start, i - start - 1);
            double argVal = evaluateExpression(argStr);
            values.push(parseFunction(func, argVal));
        }
        else if (isdigit(expr[i]) || expr[i] == '.') {
            string val;
            while (i < expr.size() && (isdigit(expr[i]) || expr[i] == '.')) val += expr[i++];
            values.push(stod(val));
        }
        else if (expr[i] == '(') {
            ops.push(expr[i++]);
        }
        else if (expr[i] == ')') {
            while (!ops.empty() && ops.top() != '(') {
                double b = values.top(); values.pop();
                double a = values.top(); values.pop();
                char op = ops.top(); ops.pop();
                values.push(applyOperator(a, b, op));
            }
            if (!ops.empty()) ops.pop();
            ++i;
        }
        else {
            while (!ops.empty() && precedence(ops.top()) >= precedence(expr[i])) {
                double b = values.top(); values.pop();
                double a = values.top(); values.pop();
                char op = ops.top(); ops.pop();
                values.push(applyOperator(a, b, op));
            }
            ops.push(expr[i++]);
        }
    }
    while (!ops.empty()) {
        double b = values.top(); values.pop();
        double a = values.top(); values.pop();
        char op = ops.top(); ops.pop();
        values.push(applyOperator(a, b, op));
    }
    return values.top();
}

void logResult(const string& expression, double result) {
    ofstream logFile("calc_history.log", ios::app);
    time_t now = time(nullptr);
    tm localTm{};
    localtime_s(&localTm, &now);
    logFile << put_time(&localTm, "%Y-%m-%d %H:%M:%S") << " | "
        << expression << " = " << result << endl;
    logFile.close();
}


void displayHistory() {
    ifstream logFile("calc_history.log");
    if (!logFile) {
        cout << "\nNo history available.\n";
        return;
    }
    cout << "\n------ History ------\n";
    string line;
    while (getline(logFile, line)) {
        cout << line << endl;
    }
    logFile.close();
}

void clearHistory() {
    ofstream logFile("calc_history.log", ios::trunc);
    logFile.close();
    cout << "\nHistory cleared.\n";
}

void processExpressionDirect() {
    string expression;
    cout << "\nType expression (e.g. 2+3*(5-2), sin(1.57), history, clear, exit):\n> ";
    getline(cin, expression);
    if (expression == "exit") {
        renderInterfaceFooter();
        cout << "Press Enter to exit...";
        cin.get();
        exit(0);
    }
    else if (expression == "history") {
        displayHistory();
        return;
    }
    else if (expression == "clear") {
        clearHistory();
        return;
    }
    try {
        double result = evaluateExpression(expression);
        lastResult = result;
        applyConsoleColor(2);
        cout << "Result: " << fixed << setprecision(6) << result << endl;
        resetConsoleColor();
        logResult(expression, result);
    }
    catch (const exception& ex) {
        applyConsoleColor(4);
        cerr << "Error: " << ex.what() << endl;
        resetConsoleColor();
    }
}

int main() {
    renderInterfaceHeader();
    while (true) {
        processExpressionDirect();
    }
    renderInterfaceFooter();
    return 0;
}
