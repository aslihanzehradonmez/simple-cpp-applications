#include <iostream>
#include <vector>
#include <limits>
#include <string>
#include <iomanip>

#define NOMINMAX
#include <windows.h>


HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

enum ConsoleColor {
    BLUE = 1, GREEN = 2, CYAN = 3, RED = 4, MAGENTA = 5, YELLOW = 6, WHITE = 7,
    GREY = 8, L_BLUE = 9, L_GREEN = 10, L_CYAN = 11, L_RED = 12, L_MAGENTA = 13, L_YELLOW = 14, BRIGHT_WHITE = 15
};

void setConsoleColor(ConsoleColor color) {
    SetConsoleTextAttribute(hConsole, color);
}

void resetConsoleColor() {
    SetConsoleTextAttribute(hConsole, WHITE);
}


unsigned long long calculateFactorial(int n) {
    if (n < 0) {
        return 0;
    }
    if (n == 0) {
        return 1;
    }
    unsigned long long result = 1;
    // Use constexpr for compile-time constant
    constexpr unsigned long long max_val = std::numeric_limits<unsigned long long>::max();

    for (int i = 1; i <= n; ++i) {
        // Check for potential overflow before multiplication
        if (max_val / i < result) {
            return 0; // Indicate overflow
        }
        result *= i;
    }
    return result;
}


unsigned long long calculateFibonacci(int n) {
    if (n < 0) {
        return 0;
    }
    if (n == 0) {
        return 0;
    }
    if (n == 1) {
        return 1;
    }

    unsigned long long a = 0;
    unsigned long long b = 1;
    unsigned long long current_fib = 0;
    // Use constexpr for compile-time constant
    constexpr unsigned long long max_val = std::numeric_limits<unsigned long long>::max();

    for (int i = 2; i <= n; ++i) {
        // Check for potential overflow before addition
        if (max_val - b < a) {
            return 0; // Indicate overflow
        }
        current_fib = a + b;
        a = b;
        b = current_fib;
    }
    return b;
}


void printHeader(const std::string& title) {
    setConsoleColor(L_CYAN);
    std::cout << "\n==================================================\n";
    std::cout << "    " << title << "\n";
    std::cout << "==================================================\n\n";
    resetConsoleColor();
}


void printTableHeader() {
    setConsoleColor(YELLOW);
    std::cout << std::setw(5) << "N" << " | "
        << std::setw(25) << "Factorial(N)" << " | "
        << std::setw(25) << "Fibonacci(N)" << std::endl;
    std::cout << std::setw(5) << "-----" << " | "
        << std::setw(25) << "-------------------------" << " | "
        << std::setw(25) << "-------------------------" << std::endl;
    resetConsoleColor();
}


void printTableRow(int n, unsigned long long factorial, unsigned long long fibonacci) {
    setConsoleColor(BRIGHT_WHITE);
    std::cout << std::setw(5) << n << " | ";

    setConsoleColor(L_GREEN);
    if (factorial == 0 && n > 20) {
        std::cout << std::setw(25) << "Overflow";
    }
    else if (factorial == 0 && n < 0) {
        std::cout << std::setw(25) << "Undefined";
    }
    else {
        std::cout << std::setw(25) << factorial;
    }

    std::cout << " | ";

    setConsoleColor(L_MAGENTA);
    if (fibonacci == 0 && n > 93) {
        std::cout << std::setw(25) << "Overflow";
    }
    else if (fibonacci == 0 && n < 0) {
        std::cout << std::setw(25) << "Undefined";
    }
    else if (fibonacci == 0 && n == 0) {
        std::cout << std::setw(25) << 0;
    }
    else {
        std::cout << std::setw(25) << fibonacci;
    }

    std::cout << std::endl;
    resetConsoleColor();
}


int main() {
    int limit;

    SetConsoleTitle(L"Fibonacci and Factorial Calculation Program");

    printHeader("Fibonacci and Factorial Calculator");

    setConsoleColor(L_YELLOW);
    std::cout << "Enter the upper limit (N) for calculations (e.g., 20): ";
    resetConsoleColor();

    while (!(std::cin >> limit) || limit < 0) {
        setConsoleColor(L_RED);
        std::cout << "Invalid input. Please enter a non-negative integer: ";
        resetConsoleColor();
        std::cin.clear();
        // Note: std::numeric_limits is used here too, but streamsize::max()
        // is not necessarily required to be constexpr on the variable itself.
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    std::cout << "\nCalculating Factorial and Fibonacci numbers up to N = " << limit << "...\n" << std::endl;

    printTableHeader();

    for (int i = 0; i <= limit; ++i) {
        unsigned long long fact = calculateFactorial(i);
        unsigned long long fib = calculateFibonacci(i);

        printTableRow(i, fact, fib);
    }

    setConsoleColor(CYAN);
    std::cout << "\n--------------------------------------------------\n";
    std::cout << "Calculation Complete.\n";

    setConsoleColor(L_YELLOW);
    std::cout << "\n";
    std::cout << "\n                  AZD\n";
    std::cout << "--------------------------------------------------\n";

    resetConsoleColor();

    std::cout << "\nPress Enter to exit...";
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();

    return 0;
}