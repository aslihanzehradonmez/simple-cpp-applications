#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <limits>

#ifdef _WIN32
#define CLEAR "cls"
#else
#define CLEAR "clear"
#endif

std::string caesarCipher(std::string text, int shift, bool encrypt) {
    std::string result = "";
    for (char& c : text) {
        if (isalpha(c)) {
            char base = isupper(c) ? 'A' : 'a';
            int shiftedChar = (c - base + (encrypt ? shift : (26 - shift))) % 26;
            result += (char)(base + shiftedChar);
        }
        else {
            result += c;
        }
    }
    return result;
}

std::string vigenereCipher(std::string text, std::string key, bool encrypt) {
    std::string result = "";
    int keyIndex = 0;
    for (char& c : text) {
        if (isalpha(c)) {
            char base = isupper(c) ? 'A' : 'a';
            char keyChar = key[keyIndex % key.length()];
            char keyBase = isupper(keyChar) ? 'A' : 'a';
            int shift = keyChar - keyBase;
            int shiftedChar = (c - base + (encrypt ? shift : (26 - shift))) % 26;
            result += (char)(base + shiftedChar);
            keyIndex++;
        }
        else {
            result += c;
        }
    }
    return result;
}

std::string transpositionCipher(std::string text, std::vector<int> key, bool encrypt) {
    std::string result = "";
    int numCols = static_cast<int>(key.size());
    int numRows = (text.length() + numCols - 1) / numCols;
    std::vector<std::vector<char>> grid(numRows, std::vector<char>(numCols, ' '));
    int textIndex = 0;

    if (encrypt) {
        for (int i = 0; i < numRows; ++i) {
            for (int j = 0; j < numCols; ++j) {
                if (textIndex < text.length()) {
                    grid[i][j] = text[textIndex++];
                }
            }
        }
        for (int k : key) {
            if (k > 0 && k <= numCols) {
                for (int i = 0; i < numRows; ++i) {
                    result += grid[i][k - 1];
                }
            }
        }
    }
    else {
        std::vector<std::vector<char>> decryptedGrid(numRows, std::vector<char>(numCols));
        int resultIndex = 0;
        std::vector<int> decryptedOrder(numCols);
        for (int i = 0; i < numCols; ++i) {
            if (key[i] > 0 && key[i] <= numCols) {
                decryptedOrder[key[i] - 1] = i;
            }
        }
        for (int k : decryptedOrder) {
            for (int i = 0; i < numRows; ++i) {
                if (resultIndex < text.length()) {
                    decryptedGrid[i][k] = text[resultIndex++];
                }
            }
        }
        for (int i = 0; i < numRows; ++i) {
            for (int j = 0; j < numCols; ++j) {
                result += decryptedGrid[i][j];
            }
        }
        while (!result.empty() && result.back() == ' ') {
            result.pop_back();
        }
    }
    return result;
}

int getIntegerInput(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.good()) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            break;
        }
        else {
            std::cout << "Invalid input. Please enter an integer.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
    return value;
}

std::string getStringInput(const std::string& prompt) {
    std::string text;
    std::cout << prompt;
    std::getline(std::cin >> std::ws, text);
    return text;
}

bool getEncryptionChoice() {
    char choice;
    while (true) {
        std::cout << "Enter 'e' to encrypt or 'd' to decrypt: ";
        std::cin >> choice;
        if (choice == 'e' || choice == 'd') {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return (choice == 'e');
        }
        else {
            std::cout << "Invalid input. Please enter 'e' or 'd'.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

std::vector<int> getTranspositionKey() {
    std::string keyStr = getStringInput("Enter the transposition key (space-separated integers, e.g., 3 1 2): ");
    std::vector<int> key;
    std::stringstream ss(keyStr);
    int temp;
    while (ss >> temp) {
        key.push_back(temp);
    }
    return key;
}

int main() {
    system(CLEAR);
    std::cout << "*******************************************************\n";
    std::cout << "** Simple Text Encryption Program v3.0               **\n";
    std::cout << "*******************************************************\n\n";

    std::cout << "Choose the operation you want to perform:\n";
    std::cout << "1. Caesar Cipher\n";
    std::cout << "2. Vigenere Cipher\n";
    std::cout << "3. Transposition Cipher\n";
    int choice = getIntegerInput("Enter your choice (1-3): ");

    std::string inputText = getStringInput("\nEnter the text to process: ");
    bool encryptChoice = getEncryptionChoice();
    std::string outputText;

    switch (choice) {
    case 1: {
        int shiftValue = getIntegerInput("Enter the shift value for Caesar Cipher: ");
        outputText = caesarCipher(inputText, shiftValue, encryptChoice);
        break;
    }
    case 2: {
        std::string key = getStringInput("Enter the key for Vigenere Cipher: ");
        outputText = vigenereCipher(inputText, key, encryptChoice);
        break;
    }
    case 3: {
        std::vector<int> key = getTranspositionKey();
        outputText = transpositionCipher(inputText, key, encryptChoice);
        break;
    }
    default:
        std::cout << "Invalid choice.\n";
        return 1;
    }

    std::cout << "\nResult: " << outputText << "\n\n";

    std::cout << "-------------------------------------------------------\n";
    std::cout << "                          AZD                          \n";
    std::cout << "-------------------------------------------------------\n";

    return 0;
}