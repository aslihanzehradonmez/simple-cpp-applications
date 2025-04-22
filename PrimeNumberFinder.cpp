#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <limits>
#include <iomanip>
#include <thread>
#include <chrono>

#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"

bool isPrime(long long n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (long long i = 5; i * i <= n; i = i + 6) {
        if (n % i == 0 || n % (i + 2) == 0) {
            return false;
        }
    }
    return true;
}

std::vector<long long> findPrimesInRange(long long start, long long end) {
    std::vector<long long> primes;
    if (start < 2) start = 2;

    if (end - start <= 100000 || end <= 1000) {
        for (long long num = start; num <= end; ++num) {
            if (isPrime(num)) {
                primes.push_back(num);
            }
        }
    }
    else {
        std::cout << YELLOW << "Note: Searching in a large range (>100k) with this method might take time." << RESET << std::endl;
        for (long long num = start; num <= end; ++num) {
            if (isPrime(num)) {
                primes.push_back(num);
            }
        }
    }
    return primes;
}

std::vector<long long> findFirstNPrimes(int n) {
    std::vector<long long> primes;
    long long num = 2;
    while (primes.size() < n) {
        if (isPrime(num)) {
            primes.push_back(num);
        }
        if (num == std::numeric_limits<long long>::max()) {
            std::cerr << RED << "Error: Reached maximum checkable number." << RESET << std::endl;
            break;
        }
        num++;
    }
    return primes;
}

std::vector<long long> primeFactorization(long long n) {
    std::vector<long long> factors;
    if (n <= 1) return factors;

    while (n % 2 == 0) {
        factors.push_back(2);
        n /= 2;
    }

    for (long long i = 3; i * i <= n; i += 2) {
        while (n % i == 0) {
            factors.push_back(i);
            n /= i;
        }
    }

    if (n > 1) {
        factors.push_back(n);
    }

    return factors;
}

int displayMenu() {
    std::cout << BLUE << "\n+---------------------------------------+" << RESET << std::endl;
    std::cout << BLUE << "| " << BOLD << MAGENTA << "     Prime Number Finder Menu        " << RESET << BLUE << "|" << RESET << std::endl;
    std::cout << BLUE << "+---------------------------------------+" << RESET << std::endl;
    std::cout << CYAN << "| 1. Check if a number is prime         |" << RESET << std::endl;
    std::cout << CYAN << "| 2. Find primes in a range [a, b]      |" << RESET << std::endl;
    std::cout << CYAN << "| 3. Find the first N prime numbers     |" << RESET << std::endl;
    std::cout << CYAN << "| 4. Find prime factorization of N      |" << RESET << std::endl;
    std::cout << CYAN << "| 5. Exit                               |" << RESET << std::endl;
    std::cout << BLUE << "+---------------------------------------+" << RESET << std::endl;
    std::cout << YELLOW << "Enter your choice (1-5): " << RESET;

    int choice;
    while (!(std::cin >> choice) || choice < 1 || choice > 5) {
        std::cout << RED << "Invalid input. Please enter a number between 1 and 5: " << RESET;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return choice;
}

long long getPositiveLongLongInput(const std::string& prompt) {
    long long num;
    while (true) {
        std::cout << YELLOW << prompt << RESET;
        if (std::cin >> num && num > 0) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return num;
        }
        else {
            std::cout << RED << "Invalid input. Please enter a positive whole number." << RESET << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

int getPositiveIntInput(const std::string& prompt) {
    int num;
    while (true) {
        std::cout << YELLOW << prompt << RESET;
        if (std::cin >> num && num > 0) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return num;
        }
        else {
            std::cout << RED << "Invalid input. Please enter a positive whole number." << RESET << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

void printVector(const std::vector<long long>& vec, const std::string& label) {
    std::cout << GREEN << label << ": " << RESET;
    if (vec.empty()) {
        std::cout << "None found." << std::endl;
        return;
    }
    const int perLine = 10;
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i] << (((i + 1) % perLine == 0 || i == vec.size() - 1) ? "" : ", ");
        if ((i + 1) % perLine == 0 && i != vec.size() - 1) {
            std::cout << "\n          ";
        }
    }
    std::cout << std::endl;
}


int main() {
    int choice;
    bool firstRun = true;

    do {
        if (!firstRun) {
            std::cout << "\033[2J\033[H";
        }
        else {
            firstRun = false;
        }

        choice = displayMenu();

        switch (choice) {
        case 1: {
            long long num = getPositiveLongLongInput("Enter a number to check: ");
            auto start_time = std::chrono::high_resolution_clock::now();
            bool result = isPrime(num);
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

            if (result) {
                std::cout << GREEN << num << " is a prime number." << RESET << std::endl;
            }
            else {
                std::cout << RED << num << " is not a prime number." << RESET << std::endl;
            }
            std::cout << MAGENTA << "Time taken: " << duration.count() << " microseconds." << RESET << std::endl;
            break;
        }
        case 2: {
            long long start_range = getPositiveLongLongInput("Enter the start of the range (a): ");
            long long end_range = getPositiveLongLongInput("Enter the end of the range (b): ");

            if (start_range > end_range) {
                std::cout << RED << "Error: Start of range cannot be greater than the end of range." << RESET << std::endl;
                break;
            }

            auto start_time = std::chrono::high_resolution_clock::now();
            std::vector<long long> primes = findPrimesInRange(start_range, end_range);
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

            printVector(primes, "Primes found");
            std::cout << MAGENTA << "Total primes found: " << primes.size() << RESET << std::endl;
            std::cout << MAGENTA << "Time taken: " << duration.count() << " milliseconds." << RESET << std::endl;
            break;
        }
        case 3: {
            int n = getPositiveIntInput("Enter the number of primes to find (N): ");

            auto start_time = std::chrono::high_resolution_clock::now();
            std::vector<long long> primes = findFirstNPrimes(n);
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

            printVector(primes, "First " + std::to_string(n) + " primes");
            std::cout << MAGENTA << "Time taken: " << duration.count() << " milliseconds." << RESET << std::endl;
            break;
        }
        case 4: {
            long long num_factor = getPositiveLongLongInput("Enter a number to factorize: ");

            auto start_time = std::chrono::high_resolution_clock::now();
            std::vector<long long> factors = primeFactorization(num_factor);
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

            printVector(factors, "Prime factors");
            std::cout << MAGENTA << "Time taken: " << duration.count() << " microseconds." << RESET << std::endl;
            break;
        }
        case 5:
            std::cout << GREEN << "Exiting Prime Number Finder." << RESET << std::endl;
            break;
        default:
            std::cout << RED << "Invalid choice." << RESET << std::endl;
            break;
        }

        if (choice != 5) {
            std::cout << YELLOW << "\nPress Enter to continue..." << RESET;
            std::cin.get();
        }

    } while (choice != 5);

    std::cout << "\n" << BOLD << CYAN << "AZD" << RESET << std::endl;

    return 0;
}