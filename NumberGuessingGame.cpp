#include <iostream>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <thread>
#include <chrono>
#include <vector>
#include <iomanip>
#include <map>
#include <algorithm>

using namespace std;
using namespace chrono;

#define RESET       "\033[0m"
#define RED         "\033[31m"
#define GREEN       "\033[32m"
#define YELLOW      "\033[33m"
#define BLUE        "\033[34m"
#define MAGENTA     "\033[35m"
#define CYAN        "\033[36m"
#define BOLDWHITE   "\033[1m\033[37m"

enum Difficulty { EASY = 1, MEDIUM, HARD };

struct PlayerStats {
    string name;
    int attempts;
    double duration;
};

map<Difficulty, vector<PlayerStats>> leaderboards;

string getDifficultyName(Difficulty level) {
    switch (level) {
    case EASY: return "Easy";
    case MEDIUM: return "Medium";
    case HARD: return "Hard";
    default: return "Unknown";
    }
}

void delayPrint(const string& text, int ms = 30) {
    for (size_t i = 0; i < text.size(); ++i) {
        cout << text[i] << flush;
        this_thread::sleep_for(milliseconds(ms));
    }
    cout << endl;
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    cout << "\033[2J\033[1;1H";
#endif
}

void displayBanner() {
    clearScreen();
    cout << BOLDWHITE << "=====================================" << RESET << endl;
    cout << CYAN << "     Simple Number Guessing Game     " << RESET << endl;
    cout << BOLDWHITE << "=====================================" << RESET << endl << endl;
}

int getValidatedInput(const string& prompt, int min = INT_MIN, int max = INT_MAX) {
    int value;
    while (true) {
        cout << CYAN << prompt << RESET;
        if (cin >> value && value >= min && value <= max)
            return value;
        cout << RED << "Invalid input. Try again." << RESET << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

Difficulty chooseDifficulty() {
    delayPrint("Choose difficulty:");
    cout << YELLOW << "1. Easy (1-10)\n2. Medium (1-50)\n3. Hard (1-100)\n4. Random\n" << RESET;
    int choice = getValidatedInput("Select (1-4): ", 1, 4);
    Difficulty selected;

    if (choice == 4) {
        selected = static_cast<Difficulty>(rand() % 3 + 1);
        cout << GREEN << "Randomly selected: " << getDifficultyName(selected) << RESET << endl;
    }
    else {
        selected = static_cast<Difficulty>(choice);
        cout << GREEN << "Selected difficulty: " << getDifficultyName(selected) << RESET << endl;
    }

    return selected;
}

void showLeaderboard() {
    cout << BOLDWHITE << "\n╔════════════════════════════════════════════════════╗" << RESET << endl;
    cout << BOLDWHITE << "║                 🏆 LEADERBOARDS                   ║" << RESET << endl;
    cout << BOLDWHITE << "╚════════════════════════════════════════════════════╝\n" << RESET;

    for (map<Difficulty, vector<PlayerStats>>::iterator it = leaderboards.begin(); it != leaderboards.end(); ++it) {
        Difficulty level = it->first;
        vector<PlayerStats>& players = it->second;

        sort(players.begin(), players.end(), [](const PlayerStats& a, const PlayerStats& b) {
            if (a.attempts == b.attempts)
                return a.duration < b.duration;
            return a.attempts < b.attempts;
            });

        cout << MAGENTA << "\n=== " << getDifficultyName(level) << " Level ===\n" << RESET;
        cout << left << setw(5) << "#"
            << setw(15) << "Name"
            << setw(10) << "Tries"
            << setw(10) << "Time(s)" << endl;

        for (size_t i = 0; i < players.size(); ++i) {
            const PlayerStats& stat = players[i];
            cout << left << setw(5) << (i + 1)
                << setw(15) << stat.name
                << setw(10) << stat.attempts
                << setw(10) << fixed << setprecision(1) << stat.duration << endl;
        }
    }
    cout << endl;
}

void updateLeaderboard(const string& name, Difficulty level, int attempts, double duration) {
    vector<PlayerStats>& lb = leaderboards[level];

    for (size_t i = 0; i < lb.size(); ++i) {
        if (lb[i].name == name) {
            if (attempts < lb[i].attempts || (attempts == lb[i].attempts && duration < lb[i].duration)) {
                lb[i].attempts = attempts;
                lb[i].duration = duration;
            }
            return;
        }
    }

    lb.push_back({ name, attempts, duration });
}

void playGame(const string& playerName) {
    Difficulty level = chooseDifficulty();
    int maxNum = (level == EASY) ? 10 : (level == MEDIUM) ? 50 : 100;
    int secret = rand() % maxNum + 1;
    int guess, attempts = 0;
    auto start = high_resolution_clock::now();

    cout << "\nGuess the number between 1 and " << maxNum << "!" << endl;

    while (true) {
        guess = getValidatedInput("Your guess: ", 1, maxNum);
        attempts++;

        if (guess < secret)
            cout << YELLOW << "Too low!" << RESET << endl;
        else if (guess > secret)
            cout << MAGENTA << "Too high!" << RESET << endl;
        else {
            auto end = high_resolution_clock::now();
            double duration = static_cast<double>(
                duration_cast<seconds>(end - start).count()
                );

            cout << GREEN << "\n🎉 Congratulations, " << playerName << "! You guessed it in "
                << attempts << " tries." << RESET << endl;
            cout << BLUE << "Time taken: " << duration << " seconds." << RESET << endl;

            updateLeaderboard(playerName, level, attempts, duration);

            cout << CYAN << "\n>> Press Enter to return to the menu..." << RESET;
            cin.ignore(); // to clear leftover newline from previous input
            cin.get();    // wait for enter
            break;
        }
    }
}

bool askChangeUser() {
    string input;
    cout << CYAN << "Change user? (y/n): " << RESET;
    cin >> input;
    return input == "y" || input == "Y";
}

string getPlayerName() {
    string name;
    cout << CYAN << "Enter your name: " << RESET;
    cin >> name;
    return name;
}

void showMenu(const string& playerName) {
    bool inMenu = true;

    while (inMenu) {
        displayBanner();
        cout << GREEN << "Current Player: " << playerName << RESET << endl;
        cout << BOLDWHITE << "\n===== MENU =====\n" << RESET;
        cout << "1. Play Game\n";
        cout << "2. Show Leaderboard\n";
        cout << "3. Change User\n";
        cout << "4. Exit\n";

        int choice = getValidatedInput("Select option (1-4): ", 1, 4);
        if (choice == 1) {
            playGame(playerName);
        }
        else if (choice == 2) {
            showLeaderboard();
            cout << CYAN << "\nPress Enter to return to menu..." << RESET;
            cin.ignore();
            cin.get();
        }
        else if (choice == 3) {
            inMenu = false; // break to change user
        }
        else if (choice == 4) {
            clearScreen();
            displayBanner();
            delayPrint(GREEN + string("Thank you for playing, ") + playerName + "!" + RESET);
            delayPrint(YELLOW + string("Exiting the game...") + RESET);
            this_thread::sleep_for(chrono::seconds(2));

            cout << "\n\n" << BOLDWHITE << "=====================================" << RESET << endl;
            cout << BOLDWHITE << "                 AZD                 " << RESET << endl;
            cout << BOLDWHITE << "=====================================" << RESET << endl;
            exit(0);
        }
    }
}

int main() {
    srand(static_cast<unsigned int>(time(0)));

    while (true) {
        string playerName = getPlayerName();
        showMenu(playerName);
    }

    return 0;
}
