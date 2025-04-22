#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <limits>
#include <cstdlib>
#ifdef _WIN32
#include <conio.h>
#endif

#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_RESET   "\033[0m"

const std::string CONFIG_FILE = "users.config";
bool isLoggedIn = false;
std::string loggedInUser = "";

void clearScreen() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

std::string simpleHash(const std::string& data) {
    size_t hash = std::hash<std::string>{}(data);
    std::stringstream ss;
    ss << std::hex << hash;
    return ss.str();
}

std::string getHiddenPassword() {
    std::string password = "";
    char ch;
    while ((ch = _getch()) != '\r') { // Enter tuþuna basýlana kadar karakter oku
        if (ch == '\b') { // Backspace kontrolü
            if (!password.empty()) {
                password.pop_back();
                std::cout << "\b \b"; // Ýmleci geri al ve boþluk yaz
            }
        }
        else if (ch != '\n' && ch != '\r') {
            password += ch;
            std::cout << "*";
        }
    }
    std::cout << std::endl; // Yeni satýra geç
    return password;
}

bool registerUser() {
    clearScreen();
    std::cout << COLOR_CYAN << "\n--- " << COLOR_YELLOW << "User Registration" << COLOR_CYAN << " ---" << COLOR_RESET << std::endl;
    std::string username, password;
    std::cout << "Username: ";
    std::cin >> username;
    std::cout << "Password: ";
#ifdef _WIN32
    password = getHiddenPassword();
#else
    std::cin >> password;
#endif

    std::string hashedPassword = simpleHash(password);

    std::ofstream configFile(CONFIG_FILE, std::ios::app);
    if (configFile.is_open()) {
        configFile << username << ":" << hashedPassword << std::endl;
        configFile.close();
        std::cout << COLOR_GREEN << "User registered successfully." << COLOR_RESET << std::endl;
    }
    else {
        std::cerr << COLOR_RED << "Unable to open configuration file." << COLOR_RESET << std::endl;
        return false;
    }
    std::cout << "\nPress Enter to return to the main menu...";
    std::cin.ignore();
    std::cin.get();
    return true;
}

bool loginUser() {
    clearScreen();
    std::cout << COLOR_CYAN << "\n--- " << COLOR_YELLOW << "User Login" << COLOR_CYAN << " ---" << COLOR_RESET << std::endl;
    std::string username, password;
    std::cout << "Username: ";
    std::cin >> username;
    std::cout << "Password: ";
#ifdef _WIN32
    password = getHiddenPassword();
#else
    std::cin >> password;
#endif

    std::ifstream configFile(CONFIG_FILE);
    if (configFile.is_open()) {
        std::string line;
        while (getline(configFile, line)) {
            std::stringstream ss(line);
            std::string storedUsername, storedHash;
            std::getline(ss, storedUsername, ':');
            std::getline(ss, storedHash, ':');

            if (username == storedUsername) {
                if (simpleHash(password) == storedHash) {
                    std::cout << COLOR_GREEN << "Login successful!" << COLOR_RESET << std::endl;
                    configFile.close();
                    isLoggedIn = true;
                    loggedInUser = username;
                    return true;
                }
                else {
                    std::cout << COLOR_RED << "Incorrect password." << COLOR_RESET << std::endl;
                    configFile.close();
                    return false;
                }
            }
        }
        std::cout << COLOR_RED << "Username not found." << COLOR_RESET << std::endl;
        configFile.close();
        return false;
    }
    else {
        std::cerr << COLOR_RED << "Unable to open configuration file." << COLOR_RESET << std::endl;
        return false;
    }
    std::cout << "\nPress Enter to return to the main menu...";
    std::cin.ignore();
    std::cin.get();
    return false;
}

void loggedInMenu() {
    clearScreen();
    std::cout << COLOR_MAGENTA << "\n--- " << COLOR_GREEN << "Welcome, " << loggedInUser << "!" << COLOR_MAGENTA << " ---" << COLOR_RESET << std::endl;
    std::cout << COLOR_YELLOW << "You are now logged in." << COLOR_RESET << std::endl;
    std::cout << "0. Logout" << std::endl;
    std::cout << "Enter your choice: ";
    int choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (choice == 0) {
        isLoggedIn = false;
        loggedInUser = "";
        std::cout << COLOR_YELLOW << "Logged out successfully." << COLOR_RESET << std::endl;
    }
    else {
        std::cout << COLOR_RED << "Invalid choice." << COLOR_RESET << std::endl;
    }
    std::cout << "\nPress Enter to return to the main menu...";
    std::cin.get();
}

int main() {
    int choice;
    do {
        clearScreen();
        if (!isLoggedIn) {
            std::cout << COLOR_MAGENTA << "\n--- " << COLOR_GREEN << "User Login System" << COLOR_MAGENTA << " ---" << COLOR_RESET << std::endl;
            std::cout << "1. Register" << std::endl;
            std::cout << "2. Login" << std::endl;
            std::cout << "0. Exit" << std::endl;
            std::cout << "Enter your choice: ";
            std::cin >> choice;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            switch (choice) {
            case 1:
                registerUser();
                break;
            case 2:
                loginUser();
                break;
            case 0:
                std::cout << COLOR_YELLOW << "Exiting..." << COLOR_RESET << std::endl;
                break;
            default:
                std::cout << COLOR_RED << "Invalid choice. Please try again." << COLOR_RESET << std::endl;
            }
        }
        else {
            loggedInMenu();
            choice = -1; // Ana menüye hemen dönmemek için
        }
    } while (choice != 0);

    clearScreen();
    std::cout << COLOR_BLUE << "\n-------------------------" << COLOR_RESET << std::endl;
    std::cout << COLOR_BLUE << "           AZD           " << COLOR_RESET << std::endl;
    std::cout << COLOR_BLUE << "-------------------------" << COLOR_RESET << std::endl;

    return 0;
}