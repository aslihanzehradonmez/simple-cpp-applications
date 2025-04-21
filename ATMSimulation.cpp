#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX

#include <iostream>
#include <iomanip>
#include <map>
#include <vector>
#include <string>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <unistd.h>
#endif

using namespace std;

const string SECRET_KEY = "@zd2025key";

string xorEncryptDecrypt(const string& input) {
    string output = input;
    for (size_t i = 0; i < input.size(); ++i) {
        output[i] = input[i] ^ SECRET_KEY[i % SECRET_KEY.size()];
    }
    return output;
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pause() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

string getHiddenInput(const string& prompt) {
    string input;
    char ch;
    cout << prompt;
#ifdef _WIN32
    while ((ch = _getch()) != '\r') {
        if (ch == '\b' && !input.empty()) {
            input.pop_back();
            cout << "\b \b";
        }
        else if (isdigit(ch) && input.size() < 4) {
            input.push_back(ch);
            cout << '*';
        }
    }
#else
    system("stty -echo");
    getline(cin, input);
    system("stty echo");
    cout << string(input.size(), '*') << endl;
#endif
    return input;
}

void printHeader(const string& title) {
    cout << "\033[1;36m\n====== " << title << " ======\033[0m\n";
}

void printFooter() {
    cout << "\n\033[1;33m==============================\033[0m" << endl;
    cout << "\033[1;35m             AZD             \033[0m" << endl;
    cout << "\033[1;33m==============================\033[0m\n" << endl;
}

struct Transaction {
    string type;
    double amount{};
    time_t timestamp{};
};

struct Account {
    string username;
    string pin;
    double balance = 0.0;
    vector<Transaction> history;
};

class ATM {
private:
    map<string, Account> accounts;
    Account* currentUser = nullptr;
    const string dataFile = "accounts.db";

    string secureInput(const string& prompt) {
        string input;
        cout << prompt;
        getline(cin, input);
        return input;
    }

    void saveAccountsToFile() {
        ofstream out(dataFile);
        for (const auto& pair : accounts) {
            const Account& acc = pair.second;
            out << xorEncryptDecrypt(acc.username) << ','
                << xorEncryptDecrypt(acc.pin) << ','
                << acc.balance << '\n';
            for (const auto& txn : acc.history) {
                out << xorEncryptDecrypt("T") << ','
                    << xorEncryptDecrypt(txn.type) << ','
                    << txn.amount << ',' << txn.timestamp << '\n';
            }
            out << "#\n";
        }
        out.close();
    }

    void loadAccountsFromFile() {
        ifstream in(dataFile);
        if (!in.is_open()) return;

        string line;
        Account acc;
        while (getline(in, line)) {
            if (line == "#") {
                accounts[acc.username] = acc;
                acc = Account();
                continue;
            }
            stringstream ss(line);
            string part;
            vector<string> parts;
            while (getline(ss, part, ',')) parts.push_back(part);

            if (parts.size() == 3) {
                acc.username = xorEncryptDecrypt(parts[0]);
                acc.pin = xorEncryptDecrypt(parts[1]);
                acc.balance = stod(parts[2]);
            }
            else if (parts.size() == 4 && xorEncryptDecrypt(parts[0]) == "T") {
                Transaction txn;
                txn.type = xorEncryptDecrypt(parts[1]);
                txn.amount = stod(parts[2]);
                txn.timestamp = stol(parts[3]);
                acc.history.push_back(txn);
            }
        }
        in.close();
    }

    void createAccount() {
        clearScreen();
        printHeader("Create New Account");
        string username = secureInput("Enter username: ");
        if (accounts.count(username)) {
            cout << "\n\033[1;31mUsername already exists.\033[0m\n";
            pause();
            return;
        }
        string pin = getHiddenInput("Set a 4-digit PIN: ");
        accounts[username] = { username, pin, 0.0, {} };
        cout << "\n\033[1;32mAccount created successfully!\033[0m\n";
        saveAccountsToFile();
        pause();
    }

    void login() {
        clearScreen();
        printHeader("User Login");
        string username = secureInput("Enter username: ");
        string pin = getHiddenInput("Enter PIN: ");
        if (accounts.count(username) && accounts[username].pin == pin) {
            currentUser = &accounts[username];
            cout << "\n\033[1;32mLogin successful!\033[0m\n";
        }
        else {
            cout << "\n\033[1;31mInvalid credentials.\033[0m\n";
        }
        pause();
    }

    void logout() {
        currentUser = nullptr;
    }

    void checkBalance() {
        clearScreen();
        printHeader("Balance Inquiry");
        cout << "Current Balance: $" << fixed << setprecision(2) << currentUser->balance << endl;
        pause();
    }

    void deposit() {
        clearScreen();
        printHeader("Deposit Money");
        double amount;
        cout << "Enter amount to deposit: $";
        cin >> amount;
        cin.ignore();
        if (amount > 0) {
            currentUser->balance += amount;
            currentUser->history.push_back({ "Deposit", amount, time(nullptr) });
            cout << "\n\033[1;32mDeposit successful!\033[0m\n";
            saveAccountsToFile();
        }
        else {
            cout << "\n\033[1;31mInvalid amount.\033[0m\n";
        }
        pause();
    }

    void withdraw() {
        clearScreen();
        printHeader("Withdraw Money");
        double amount;
        cout << "Enter amount to withdraw: $";
        cin >> amount;
        cin.ignore();
        if (amount > 0 && amount <= currentUser->balance) {
            currentUser->balance -= amount;
            currentUser->history.push_back({ "Withdrawal", amount, time(nullptr) });
            cout << "\n\033[1;32mWithdrawal successful!\033[0m\n";
            saveAccountsToFile();
        }
        else {
            cout << "\n\033[1;31mInsufficient balance or invalid amount.\033[0m\n";
        }
        pause();
    }

    void showTransactionHistory() {
        clearScreen();
        printHeader("Transaction History");
        if (currentUser->history.empty()) {
            cout << "No transactions to display.\n";
        }
        else {
            for (const auto& txn : currentUser->history) {
                cout << txn.type << " of $" << fixed << setprecision(2) << txn.amount
                    << " on " << ctime(&txn.timestamp);
            }
        }
        pause();
    }

    void userMenu() {
        int choice = 0;
        while (currentUser) {
            clearScreen();
            printHeader("Welcome, " + currentUser->username);
            cout << "1. Check Balance\n2. Deposit\n3. Withdraw\n4. Transaction History\n5. Logout\n\nEnter choice: ";
            cin >> choice;
            cin.ignore();
            switch (choice) {
            case 1: checkBalance(); break;
            case 2: deposit(); break;
            case 3: withdraw(); break;
            case 4: showTransactionHistory(); break;
            case 5: logout(); break;
            default: cout << "\n\033[1;31mInvalid choice.\033[0m\n"; pause(); break;
            }
        }
    }

public:
    void viewAllUsers() {
        clearScreen();
        printHeader("Registered Users");
        if (accounts.empty()) {
            cout << "No registered users found.\n";
        }
        else {
            for (const auto& pair : accounts) {
                cout << "- " << pair.first << endl;
            }
        }
        pause();
    }

    void deleteUser() {
        clearScreen();
        printHeader("Delete User Account");
        string username = secureInput("Enter username to delete: ");
        if (accounts.erase(username)) {
            cout << "\n\033[1;32mUser deleted successfully!\033[0m\n";
            saveAccountsToFile();
        }
        else {
            cout << "\n\033[1;31mUser not found.\033[0m\n";
        }
        pause();
    }

    void run() {
        loadAccountsFromFile();
        int option;
        while (true) {
            clearScreen();
            printHeader("ATM Simulation");
            cout << "1. Create Account\n2. Login\n3. View All Users\n4. Delete User\n5. Exit\n\nYour choice: ";
            cin >> option;
            cin.ignore();
            switch (option) {
            case 1: createAccount(); break;
            case 2: login(); if (currentUser) userMenu(); break;
            case 3: viewAllUsers(); break;
            case 4: deleteUser(); break;
            case 5: clearScreen(); printFooter(); saveAccountsToFile(); return;
            default: cout << "\n\033[1;31mInvalid option.\033[0m\n"; pause(); break;
            }
        }
    }
};

int main() {
    ATM atm;
    atm.run();
    return 0;
}
