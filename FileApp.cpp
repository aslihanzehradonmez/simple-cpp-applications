#define NOMINMAX

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits>
#include <iomanip>
#include <stdexcept>
#include <algorithm>
#include <sstream>
#include <cstdio>
#include <windows.h>
#include <cstdint>
#include <cctype>


HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

const WORD FOREGROUND_WHITE = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
const WORD FOREGROUND_YELLOW = FOREGROUND_RED | FOREGROUND_GREEN;
const WORD FOREGROUND_CYAN = FOREGROUND_GREEN | FOREGROUND_BLUE;
const WORD FOREGROUND_MAGENTA = FOREGROUND_RED | FOREGROUND_BLUE;
const WORD FOREGROUND_LIGHT_BLUE = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
const WORD FOREGROUND_LIGHT_GREEN = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
const WORD FOREGROUND_LIGHT_RED = FOREGROUND_RED | FOREGROUND_INTENSITY;
const WORD FOREGROUND_LIGHT_YELLOW = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
const WORD FOREGROUND_BRIGHT_WHITE = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;


struct FileInfo {
    std::string name;
    uint64_t size;
    FILETIME lastWriteTime;
    bool isDirectory;
};

void setConsoleColor(WORD color) {
    SetConsoleTextAttribute(hConsole, color);
}

void resetConsoleColor() {
    setConsoleColor(FOREGROUND_WHITE);
}

void clearScreen() {
    system("cls");
}

std::string getLastErrorAsString() {
    DWORD errorCode = GetLastError();
    if (errorCode == 0) return "No error.";
    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
    std::string message(messageBuffer, size);
    LocalFree(messageBuffer);
    while (!message.empty() && (message.back() == '\r' || message.back() == '\n')) {
        message.pop_back();
    }
    return "Error " + std::to_string(errorCode) + ": " + message;
}

// Basic pause function for displaying messages before returning to menu or exiting.
void pauseForUser(const std::string& message = "Press Enter to continue...") {
    std::cout << "\n";
    setConsoleColor(FOREGROUND_YELLOW);
    std::cout << message;
    resetConsoleColor();
    // Clear the buffer fully before waiting for Enter
    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
}


std::string getStringInput(const std::string& prompt) {
    std::string input;
    setConsoleColor(FOREGROUND_CYAN);
    std::cout << prompt;
    resetConsoleColor();
    if (std::cin.peek() == '\n') {
        std::cin.ignore();
    }
    std::getline(std::cin, input);
    input.erase(0, input.find_first_not_of(" \t\n\r\f\v\""));
    input.erase(input.find_last_not_of(" \t\n\r\f\v\"") + 1);
    return input;
}

int getIntInput(const std::string& prompt) {
    int choice;
    while (true) {
        setConsoleColor(FOREGROUND_CYAN);
        std::cout << prompt;
        resetConsoleColor();
        std::cin >> choice;
        if (std::cin.fail()) {
            setConsoleColor(FOREGROUND_LIGHT_RED);
            std::cout << "Invalid input. Please enter a whole number.\n";
            resetConsoleColor();
            std::cin.clear();
            std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
        }
        else {
            std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
            return choice;
        }
    }
}

bool getConfirmation(const std::string& prompt) {
    std::string input;
    while (true) {
        input = getStringInput(prompt + " (y/n): ");
        std::transform(input.begin(), input.end(), input.begin(),
            [](unsigned char c) { return std::tolower(c); });
        if (input == "y" || input == "yes") return true;
        if (input == "n" || input == "no") return false;
        setConsoleColor(FOREGROUND_LIGHT_RED);
        std::cout << "Invalid input. Please enter 'y' or 'n'.\n";
        resetConsoleColor();
    }
}

// New function to ask user whether to continue (return to menu) or exit.
// Returns true if the user wants to exit, false otherwise.
bool askContinueOrExit() {
    setConsoleColor(FOREGROUND_YELLOW);
    std::cout << "\nOperation complete.\n";
    std::string input = getStringInput("Press Enter to return to menu, or type 'exit' to quit: ");
    resetConsoleColor();
    std::transform(input.begin(), input.end(), input.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return (input == "exit");
}


std::string formatFileSize(uint64_t size) {
    std::stringstream ss;
    const double KB = 1024.0;
    const double MB = 1024.0 * 1024.0;
    const double GB = 1024.0 * 1024.0 * 1024.0;
    if (size < KB) ss << size << " B";
    else if (size < MB) ss << std::fixed << std::setprecision(1) << (static_cast<double>(size) / KB) << " KB";
    else if (size < GB) ss << std::fixed << std::setprecision(1) << (static_cast<double>(size) / MB) << " MB";
    else ss << std::fixed << std::setprecision(1) << (static_cast<double>(size) / GB) << " GB";
    return ss.str();
}

std::string formatFileTime(const FILETIME& ft) {
    SYSTEMTIME stUTC, stLocal;
    char buffer[100];
    if (!FileTimeToSystemTime(&ft, &stUTC) || !SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal)) return "Invalid Time";
    GetDateFormatA(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &stLocal, NULL, buffer, sizeof(buffer));
    std::string dateStr = buffer;
    GetTimeFormatA(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &stLocal, NULL, buffer, sizeof(buffer));
    std::string timeStr = buffer;
    return dateStr + " " + timeStr;
}

uint64_t combineFileSize(DWORD high, DWORD low) {
    return (static_cast<uint64_t>(high) << 32) | low;
}

bool fileExists(const std::string& filePath) {
    DWORD fileAttr = GetFileAttributesA(filePath.c_str());
    return (fileAttr != INVALID_FILE_ATTRIBUTES && !(fileAttr & FILE_ATTRIBUTE_DIRECTORY));
}

bool directoryExists(const std::string& dirPath) {
    DWORD fileAttr = GetFileAttributesA(dirPath.c_str());
    return (fileAttr != INVALID_FILE_ATTRIBUTES && (fileAttr & FILE_ATTRIBUTE_DIRECTORY));
}


void readFileContent(const std::string& filePath) {
    setConsoleColor(FOREGROUND_YELLOW);
    std::cout << "Hint: Reading PLAIN TEXT file. Binary files (.docx, .exe) will show garbled text.\n";
    resetConsoleColor();
    std::ifstream inputFile(filePath);
    if (!inputFile.is_open()) {
        setConsoleColor(FOREGROUND_LIGHT_RED);
        std::cerr << "Error: Could not open file '" << filePath << "' for reading.\n";
        std::cerr << "       Reason: Check path, existence, and permissions.\n";
        resetConsoleColor(); return;
    }
    std::string line; int lineNumber = 1;
    setConsoleColor(FOREGROUND_LIGHT_GREEN);
    size_t lastSlash = filePath.find_last_of("/\\");
    std::string filename = (lastSlash == std::string::npos) ? filePath : filePath.substr(lastSlash + 1);
    std::cout << "\n--- Content of " << filename << " ---\n";
    resetConsoleColor();
    while (getline(inputFile, line)) { std::cout << std::setw(5) << lineNumber++ << ": " << line << std::endl; }
    inputFile.close();
    std::cout << "\n--- End of Content ---\n";
}

void writeFileContent(const std::string& filePath) {
    setConsoleColor(FOREGROUND_YELLOW); std::cout << "Hint: Writes text. Can append or overwrite existing files.\n"; resetConsoleColor();
    std::ios_base::openmode mode = std::ios::out; bool fileDidExist = fileExists(filePath);
    if (fileDidExist) {
        std::string choiceStr = getStringInput("File exists. Append (a) or Overwrite (o)? ");
        std::transform(choiceStr.begin(), choiceStr.end(), choiceStr.begin(), [](unsigned char c) { return std::tolower(c); });
        if (choiceStr == "a" || choiceStr == "append") { mode = std::ios::app; setConsoleColor(FOREGROUND_YELLOW); std::cout << "Mode set to APPEND.\n"; resetConsoleColor(); }
        else if (choiceStr == "o" || choiceStr == "overwrite") { setConsoleColor(FOREGROUND_YELLOW); std::cout << "Mode set to OVERWRITE.\n"; resetConsoleColor(); }
        else { setConsoleColor(FOREGROUND_LIGHT_RED); std::cerr << "Invalid choice. Operation cancelled.\n"; resetConsoleColor(); return; }
    }
    else { setConsoleColor(FOREGROUND_YELLOW); std::cout << "File does not exist. It will be created.\n"; resetConsoleColor(); }
    std::ofstream outputFile(filePath, mode);
    if (!outputFile.is_open()) {
        setConsoleColor(FOREGROUND_LIGHT_RED); std::cerr << "Error: Could not open file '" << filePath << "' for writing.\n"; std::cerr << "       Reason: Check path validity and write permissions.\n"; resetConsoleColor(); return;
    }
    setConsoleColor(FOREGROUND_CYAN); std::cout << "Enter text. Type '!EOF!' on a new line when finished:\n"; resetConsoleColor();
    std::string line;
    while (true) { if (std::cin.peek() == '\n') std::cin.ignore(); std::getline(std::cin, line); if (line == "!EOF!") break; outputFile << line << std::endl; }
    outputFile.close();
    setConsoleColor(FOREGROUND_LIGHT_GREEN);
    size_t lastSlash = filePath.find_last_of("/\\"); std::string filename = (lastSlash == std::string::npos) ? filePath : filePath.substr(lastSlash + 1);
    std::cout << "Content successfully written to " << filename << std::endl; resetConsoleColor();
}

void listDirectoryContents(const std::string& dirPath) {
    setConsoleColor(FOREGROUND_YELLOW); std::cout << "Hint: Listing contents of '" << dirPath << "'.\n"; resetConsoleColor();
    std::string searchPath = dirPath; if (!searchPath.empty() && searchPath.back() != '\\' && searchPath.back() != '/') searchPath += "\\"; searchPath += "*";
    WIN32_FIND_DATAA findData; HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        setConsoleColor(FOREGROUND_LIGHT_RED); std::cerr << "Error listing directory: " << dirPath << "\n"; std::cerr << "       " << getLastErrorAsString() << "\n"; std::cerr << "       Reason: Check existence and permissions.\n"; resetConsoleColor(); return;
    }
    std::vector<FileInfo> items;
    do { if (strcmp(findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0) { FileInfo info; info.name = findData.cFileName; info.isDirectory = (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY); info.size = info.isDirectory ? 0 : combineFileSize(findData.nFileSizeHigh, findData.nFileSizeLow); info.lastWriteTime = findData.ftLastWriteTime; items.push_back(info); } } while (FindNextFileA(hFind, &findData) != 0);
    FindClose(hFind); DWORD lastError = GetLastError(); if (lastError != ERROR_NO_MORE_FILES) { setConsoleColor(FOREGROUND_LIGHT_RED); std::cerr << "Warning: Error during traversal. " << getLastErrorAsString() << std::endl; resetConsoleColor(); }
    if (items.empty()) { setConsoleColor(FOREGROUND_YELLOW); std::cout << "Directory appears to be empty.\n"; resetConsoleColor(); return; }
    int sortChoice = 0; std::cout << "\nSort by: (0) Name [Default], (1) Size, (2) Date Modified\n"; sortChoice = getIntInput("Enter sort choice (0-2): ");
    switch (sortChoice) { case 1: std::sort(items.begin(), items.end(), [](const FileInfo& a, const FileInfo& b) { if (a.isDirectory != b.isDirectory) return a.isDirectory; return a.size < b.size; }); break; case 2: std::sort(items.begin(), items.end(), [](const FileInfo& a, const FileInfo& b) { return CompareFileTime(&a.lastWriteTime, &b.lastWriteTime) < 0; }); break; default: std::sort(items.begin(), items.end(), [](const FileInfo& a, const FileInfo& b) { return _stricmp(a.name.c_str(), b.name.c_str()) < 0; }); break; }
                                setConsoleColor(FOREGROUND_LIGHT_GREEN); std::cout << "\n--- Contents of " << dirPath << " ---\n"; resetConsoleColor(); std::cout << std::left << std::setw(45) << "Name" << std::setw(12) << "Type" << std::setw(15) << "Size" << "Last Modified\n"; std::cout << std::string(85, '-') << std::endl;
                                for (const auto& item : items) { std::string typeStr; WORD color; if (item.isDirectory) { color = FOREGROUND_LIGHT_BLUE; typeStr = "Directory"; } else { color = FOREGROUND_WHITE; typeStr = "File"; } setConsoleColor(color); std::cout << std::left << std::setw(45) << item.name.substr(0, 43) << std::setw(12) << typeStr << std::setw(15) << (item.isDirectory ? "-" : formatFileSize(item.size)) << formatFileTime(item.lastWriteTime) << std::endl; resetConsoleColor(); } std::cout << std::string(85, '-') << std::endl;
}

void deleteFile(const std::string& filePath) {
    setConsoleColor(FOREGROUND_YELLOW); std::cout << "Hint: Attempting to delete file: " << filePath << std::endl; resetConsoleColor();
    if (!fileExists(filePath)) { setConsoleColor(FOREGROUND_LIGHT_RED); std::cerr << "Error: File not found. Cannot delete.\n"; resetConsoleColor(); return; }
    if (directoryExists(filePath)) { setConsoleColor(FOREGROUND_LIGHT_RED); std::cerr << "Error: Path is a directory. Cannot delete directories this way.\n"; resetConsoleColor(); return; }
    setConsoleColor(FOREGROUND_LIGHT_RED); std::cout << "WARNING: This will PERMANENTLY delete the file!\n"; resetConsoleColor();
    if (getConfirmation("Are you sure?")) { if (std::remove(filePath.c_str()) == 0) { setConsoleColor(FOREGROUND_LIGHT_GREEN); std::cout << "File successfully deleted.\n"; resetConsoleColor(); } else { setConsoleColor(FOREGROUND_LIGHT_RED); std::string errorMsg = "Error deleting file '" + filePath + "'"; perror(errorMsg.c_str()); std::cerr << "       Reason: File in use, or permissions issue.\n"; resetConsoleColor(); } }
    else { setConsoleColor(FOREGROUND_YELLOW); std::cout << "File deletion cancelled.\n"; resetConsoleColor(); }
}

void copyFile(const std::string& sourcePath, const std::string& destPath) {
    setConsoleColor(FOREGROUND_YELLOW); std::cout << "Hint: Copying from '" << sourcePath << "' to '" << destPath << "'.\n"; resetConsoleColor();
    if (!fileExists(sourcePath)) { setConsoleColor(FOREGROUND_LIGHT_RED); std::cerr << "Error: Source file not found.\n"; resetConsoleColor(); return; }
    if (directoryExists(sourcePath)) { setConsoleColor(FOREGROUND_LIGHT_RED); std::cerr << "Error: Source path is a directory.\n"; resetConsoleColor(); return; }
    if (sourcePath == destPath) { setConsoleColor(FOREGROUND_LIGHT_RED); std::cerr << "Error: Source and destination are the same.\n"; resetConsoleColor(); return; }
    std::ifstream sourceFile(sourcePath, std::ios::binary); if (!sourceFile.is_open()) { setConsoleColor(FOREGROUND_LIGHT_RED); std::cerr << "Error: Could not open source file '" << sourcePath << "'.\n"; resetConsoleColor(); return; }
    std::ios_base::openmode destMode = std::ios::binary | std::ios::out | std::ios::trunc;
    if (fileExists(destPath)) { setConsoleColor(FOREGROUND_YELLOW); std::cout << "Destination file '" << destPath << "' exists.\n"; resetConsoleColor(); if (!getConfirmation("Overwrite?")) { setConsoleColor(FOREGROUND_YELLOW); std::cout << "Copy cancelled.\n"; resetConsoleColor(); sourceFile.close(); return; } }
    else if (directoryExists(destPath)) { setConsoleColor(FOREGROUND_LIGHT_RED); std::cerr << "Error: Destination '" << destPath << "' is a directory.\n"; resetConsoleColor(); sourceFile.close(); return; }
    std::ofstream destFile(destPath, destMode); if (!destFile.is_open()) { setConsoleColor(FOREGROUND_LIGHT_RED); std::cerr << "Error: Could not open destination file '" << destPath << "'.\n"; std::cerr << "       Reason: Check path and permissions.\n"; resetConsoleColor(); sourceFile.close(); return; }
    char buffer[8192]; bool copyError = false;
    try { while (sourceFile.read(buffer, sizeof(buffer))) { destFile.write(buffer, sourceFile.gcount()); if (destFile.fail()) throw std::runtime_error("Write error"); } if (sourceFile.gcount() > 0) { destFile.write(buffer, sourceFile.gcount()); if (destFile.fail()) throw std::runtime_error("Final write error"); } }
    catch (const std::exception& e) { setConsoleColor(FOREGROUND_LIGHT_RED); std::cerr << "Error during copy: " << e.what() << ".\n"; resetConsoleColor(); copyError = true; }
    sourceFile.close(); destFile.close();
    if (!copyError && !sourceFile.bad() && !destFile.bad()) { setConsoleColor(FOREGROUND_LIGHT_GREEN); std::cout << "File successfully copied.\n"; resetConsoleColor(); }
    else { if (!copyError) { setConsoleColor(FOREGROUND_LIGHT_RED); std::cerr << "Error: Stream error during copy.\n"; resetConsoleColor(); } std::remove(destPath.c_str()); }
}

void moveOrRenameFile(const std::string& sourcePath, const std::string& destPath) {
    setConsoleColor(FOREGROUND_YELLOW); std::cout << "Hint: Moving/Renaming '" << sourcePath << "' to '" << destPath << "'.\n"; resetConsoleColor();
    if (!fileExists(sourcePath) && !directoryExists(sourcePath)) { setConsoleColor(FOREGROUND_LIGHT_RED); std::cerr << "Error: Source not found: " << sourcePath << "\n"; resetConsoleColor(); return; }
    if (sourcePath == destPath) { setConsoleColor(FOREGROUND_LIGHT_RED); std::cerr << "Error: Paths are the same.\n"; resetConsoleColor(); return; }
    if (fileExists(destPath) || directoryExists(destPath)) { setConsoleColor(FOREGROUND_LIGHT_RED); std::cerr << "Error: Destination path '" << destPath << "' already exists.\n"; resetConsoleColor(); return; }
    if (std::rename(sourcePath.c_str(), destPath.c_str()) == 0) { setConsoleColor(FOREGROUND_LIGHT_GREEN); std::cout << "Successfully moved/renamed.\n"; resetConsoleColor(); }
    else { setConsoleColor(FOREGROUND_LIGHT_RED); std::string errorMsg = "Error moving/renaming from '" + sourcePath + "' to '" + destPath + "'"; perror(errorMsg.c_str()); std::cerr << "       Reason: Permissions, file in use, different drives?\n"; resetConsoleColor(); }
}

void viewFileProperties(const std::string& path) {
    setConsoleColor(FOREGROUND_YELLOW); std::cout << "Hint: Showing properties for: " << path << std::endl; resetConsoleColor();
    WIN32_FILE_ATTRIBUTE_DATA fileInfo;
    if (GetFileAttributesExA(path.c_str(), GetFileExInfoStandard, &fileInfo)) {
        bool isDir = (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY); uint64_t size = isDir ? 0 : combineFileSize(fileInfo.nFileSizeHigh, fileInfo.nFileSizeLow);
        std::cout << "\n--- Properties ---\n"; std::cout << "Path:          " << path << "\n"; std::cout << "Type:          " << (isDir ? "Directory" : "File") << "\n"; if (!isDir) std::cout << "Size:          " << formatFileSize(size) << " (" << size << " bytes)\n"; std::cout << "Created:       " << formatFileTime(fileInfo.ftCreationTime) << "\n"; std::cout << "Last Modified: " << formatFileTime(fileInfo.ftLastWriteTime) << "\n"; std::cout << "Last Accessed: " << formatFileTime(fileInfo.ftLastAccessTime) << "\n"; std::cout << "Attributes:    ";
        if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_READONLY) std::cout << "Read-Only "; if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) std::cout << "Hidden "; if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) std::cout << "System "; if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) std::cout << "Archive "; if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) std::cout << "Directory "; if (fileInfo.dwFileAttributes == 0 || fileInfo.dwFileAttributes == FILE_ATTRIBUTE_NORMAL) std::cout << "Normal"; std::cout << "\n------------------\n";
    }
    else { setConsoleColor(FOREGROUND_LIGHT_RED); std::cerr << "Error getting properties for: " << path << "\n"; std::cerr << "       " << getLastErrorAsString() << "\n"; std::cerr << "       Reason: Check existence and permissions.\n"; resetConsoleColor(); }
}

void transformFileContent(const std::string& sourcePath, const std::string& destPath, bool toUpper) {
    setConsoleColor(FOREGROUND_YELLOW); std::cout << "Hint: Reading '" << sourcePath << "', transforming to " << (toUpper ? "UPPERCASE" : "lowercase") << ", writing to '" << destPath << "'.\n"; resetConsoleColor();
    if (!fileExists(sourcePath)) { setConsoleColor(FOREGROUND_LIGHT_RED); std::cerr << "Error: Source file not found.\n"; resetConsoleColor(); return; }
    if (directoryExists(sourcePath)) { setConsoleColor(FOREGROUND_LIGHT_RED); std::cerr << "Error: Source path is a directory.\n"; resetConsoleColor(); return; }
    if (sourcePath == destPath) { setConsoleColor(FOREGROUND_LIGHT_RED); std::cerr << "Error: Source and destination cannot be the same.\n"; resetConsoleColor(); return; }
    std::ifstream sourceFile(sourcePath); if (!sourceFile.is_open()) { setConsoleColor(FOREGROUND_LIGHT_RED); std::cerr << "Error: Could not open source file '" << sourcePath << "'.\n"; resetConsoleColor(); return; }
    std::ios_base::openmode destMode = std::ios::out | std::ios::trunc;
    if (fileExists(destPath)) { setConsoleColor(FOREGROUND_YELLOW); std::cout << "Destination file '" << destPath << "' exists.\n"; resetConsoleColor(); if (!getConfirmation("Overwrite?")) { setConsoleColor(FOREGROUND_YELLOW); std::cout << "Transformation cancelled.\n"; resetConsoleColor(); sourceFile.close(); return; } }
    else if (directoryExists(destPath)) { setConsoleColor(FOREGROUND_LIGHT_RED); std::cerr << "Error: Destination path '" << destPath << "' is a directory.\n"; resetConsoleColor(); sourceFile.close(); return; }
    std::ofstream destFile(destPath, destMode); if (!destFile.is_open()) { setConsoleColor(FOREGROUND_LIGHT_RED); std::cerr << "Error: Could not open destination file '" << destPath << "'.\n"; resetConsoleColor(); sourceFile.close(); return; }
    std::string line; unsigned long long linesProcessed = 0; bool writeError = false;
    while (getline(sourceFile, line)) { if (toUpper) std::transform(line.begin(), line.end(), line.begin(), ::toupper); else std::transform(line.begin(), line.end(), line.begin(), ::tolower); destFile << line << std::endl; linesProcessed++; if (destFile.fail()) { writeError = true; break; } }
    sourceFile.close(); destFile.close();
    if (writeError) { setConsoleColor(FOREGROUND_LIGHT_RED); std::cerr << "Error: Failed writing to destination. Aborted.\n"; resetConsoleColor(); std::remove(destPath.c_str()); return; }
    if (sourceFile.bad() || destFile.bad()) { setConsoleColor(FOREGROUND_LIGHT_RED); std::cerr << "Error: Stream error during transformation.\n"; resetConsoleColor(); std::remove(destPath.c_str()); }
    else { setConsoleColor(FOREGROUND_LIGHT_GREEN); std::cout << "Transformation complete. Processed " << linesProcessed << " lines.\n"; resetConsoleColor(); }
}

void analyzeTextFile(const std::string& filePath) {
    setConsoleColor(FOREGROUND_YELLOW); std::cout << "Hint: Analyzing PLAIN TEXT file: " << filePath << "\n"; std::cout << "      Results may be inaccurate for binary files.\n"; resetConsoleColor();
    std::ifstream inputFile(filePath); if (!inputFile.is_open()) { setConsoleColor(FOREGROUND_LIGHT_RED); std::cerr << "Error: Could not open file '" << filePath << "' for analysis.\n"; resetConsoleColor(); return; }
    unsigned long long lineCount = 0, wordCount = 0, charCount = 0; std::string line; bool inWord = false;
    while (getline(inputFile, line)) { lineCount++; charCount += line.length() + 1; for (char c : line) { if (std::isspace(static_cast<unsigned char>(c))) { if (inWord) wordCount++; inWord = false; } else inWord = true; } if (inWord) { wordCount++; inWord = false; } }
    inputFile.close();
    std::cout << "\n--- Analysis Results ---\n"; std::cout << "File:         " << filePath << "\n"; std::cout << "Lines:        " << lineCount << "\n"; std::cout << "Words:        " << wordCount << "\n"; std::cout << "Characters:   " << charCount << " (incl. newlines)\n"; std::cout << "------------------------\n";
}


void displayMenu() {
    clearScreen(); setConsoleColor(FOREGROUND_MAGENTA | FOREGROUND_INTENSITY); std::cout << "==================================================\n"; setConsoleColor(FOREGROUND_BRIGHT_WHITE); std::cout << "=== Advanced File Reading/Writing Application ===\n"; setConsoleColor(FOREGROUND_MAGENTA | FOREGROUND_INTENSITY); std::cout << "==================================================\n\n"; resetConsoleColor();
    setConsoleColor(FOREGROUND_YELLOW); std::cout << "--- Main Menu ---\n"; resetConsoleColor();
    std::cout << " 1. Read File Content           (View text file content)\n";
    std::cout << " 2. Write to File               (Create/Append/Overwrite text file)\n";
    std::cout << " 3. List Directory Contents     (List files/folders - Win Only)\n";
    std::cout << " 4. View File/Dir Properties    (Show size, dates, attributes)\n";
    std::cout << " 5. Analyze Text File           (Count lines, words, chars)\n";
    std::cout << " 6. Transform Text File Case    (Convert to UPPER/lower)\n";
    std::cout << " 7. Delete File                 (Permanently remove a file)\n";
    std::cout << " 8. Copy File                   (Duplicate a file)\n";
    std::cout << " 9. Move/Rename File/Directory  (Change name or location)\n";
    std::cout << " 0. Exit                        (Close the application)\n";
    std::cout << "--------------------------------------------------\n";
}

int main() {
    SetConsoleTitle(TEXT("Advanced File Reading and Writing Application - AZD"));
    int choice;
    bool shouldExit = false; // Flag to control exiting from within actions

    do {
        displayMenu();
        choice = getIntInput("Enter your choice (0-9): ");
        std::string path1, path2;
        bool transformToUpper = false;

        // Handle actions based on choice
        switch (choice) {
        case 1: path1 = getStringInput("Enter full path of TEXT file to read: "); readFileContent(path1); break;
        case 2: path1 = getStringInput("Enter full path of file to write to: "); writeFileContent(path1); break;
        case 3: path1 = getStringInput("Enter full path of directory to list: "); listDirectoryContents(path1); break;
        case 4: path1 = getStringInput("Enter full path of file/directory to view props: "); viewFileProperties(path1); break;
        case 5: path1 = getStringInput("Enter full path of TEXT file to analyze: "); analyzeTextFile(path1); break;
        case 6: path1 = getStringInput("Enter SOURCE text file path: "); path2 = getStringInput("Enter DESTINATION file path: "); transformToUpper = getConfirmation("Convert to UPPERCASE? (y/n)"); transformFileContent(path1, path2, transformToUpper); break;
        case 7: path1 = getStringInput("Enter full path of file to DELETE: "); deleteFile(path1); break;
        case 8: path1 = getStringInput("Enter SOURCE file path to copy: "); path2 = getStringInput("Enter DESTINATION file path for copy: "); copyFile(path1, path2); break;
        case 9: path1 = getStringInput("Enter CURRENT full path to move/rename: "); path2 = getStringInput("Enter the NEW full path/name: "); moveOrRenameFile(path1, path2); break;
        case 0: setConsoleColor(FOREGROUND_LIGHT_GREEN); std::cout << "\nExiting application...\n"; resetConsoleColor(); shouldExit = true; break; // Set flag to exit loop
        default: setConsoleColor(FOREGROUND_LIGHT_RED); std::cout << "Invalid choice.\n"; resetConsoleColor(); pauseForUser("Press Enter to return to menu..."); break; // Simple pause for invalid choice
        }

        // After a valid action (not 0 or default), ask user if they want to continue or exit
        if (choice > 0 && choice <= 9) {
            shouldExit = askContinueOrExit(); // Update exit flag based on user input
        }

    } while (!shouldExit); // Loop continues until user chooses 0 or types 'exit'

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(hConsole, &csbi)) { COORD bottom_left = { 0, csbi.srWindow.Bottom }; SetConsoleCursorPosition(hConsole, bottom_left); }
    std::cout << "\n\n"; setConsoleColor(FOREGROUND_BRIGHT_WHITE | BACKGROUND_BLUE); std::cout << "    AZD    "; resetConsoleColor(); std::cout << std::endl;
    return 0;
}