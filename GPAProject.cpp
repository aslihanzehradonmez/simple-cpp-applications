#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <map>
#include <fstream>
#include <algorithm>
#include <sstream>
#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"

struct Course {
    string code;
    string name;
    double credit;
    string letterGrade;
};

map<string, double> gradePointMap = {
    {"AA", 4.0}, {"BA", 3.5}, {"BB", 3.0}, {"CB", 2.5},
    {"CC", 2.0}, {"DC", 1.5}, {"DD", 1.0}, {"FD", 0.5}, {"FF", 0.0}
};

const vector<string> validGrades = { "AA", "BA", "BB", "CB", "CC", "DC", "DD", "FD", "FF" };

string line(char symbol = '=', int length = 60) {
    return string(length, symbol);
}

bool isValidGrade(const string& g) {
    return gradePointMap.find(g) != gradePointMap.end();
}

string formatCourse(const Course& c) {
    string color = c.letterGrade == "AA" ? GREEN :
        c.letterGrade == "BA" ? CYAN :
        c.letterGrade == "BB" ? YELLOW :
        c.letterGrade == "CB" ? MAGENTA : RED;
    ostringstream oss;
    oss << color << left << setw(10) << c.code
        << setw(25) << c.name
        << setw(7) << c.credit
        << setw(10) << c.letterGrade << RESET;
    return oss.str();
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    cout << "\033[2J\033[1;1H";
#endif
}

vector<Course> loadCoursesFromFile(const string& filename) {
    vector<Course> courses;
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        Course c;
        size_t pos1 = line.find(',');
        size_t pos2 = line.find(',', pos1 + 1);
        size_t pos3 = line.find(',', pos2 + 1);
        c.code = line.substr(0, pos1);
        c.name = line.substr(pos1 + 1, pos2 - pos1 - 1);
        c.credit = stod(line.substr(pos2 + 1, pos3 - pos2 - 1));
        c.letterGrade = line.substr(pos3 + 1);
        courses.push_back(c);
    }
    file.close();
    return courses;
}

void saveCoursesToFile(const vector<Course>& courses, const string& filename) {
    ofstream file(filename);
    for (const auto& c : courses) {
        file << c.code << "," << c.name << "," << c.credit << "," << c.letterGrade << endl;
    }
    file.close();
}

vector<Course> getCourses() {
    vector<Course> courses;
    string input;
    int n;
    while (true) {
        cout << BOLD << "Enter number of courses: " << RESET;
        getline(cin, input);
        try {
            n = stoi(input);
            if (n > 0) break;
        }
        catch (...) {}
        cerr << RED << "Invalid number. Please enter a positive integer." << RESET << endl;
    }

    for (int i = 0; i < n; ++i) {
        Course c;
        cout << BLUE << "\nCourse " << (i + 1) << RESET << endl;
        cout << "Course code: ";
        getline(cin, c.code);
        cout << "Course name: ";
        getline(cin, c.name);

        while (true) {
            cout << "Course credit: ";
            getline(cin, input);
            try {
                c.credit = stod(input);
                if (c.credit > 0) break;
            }
            catch (...) {}
            cerr << RED << "Invalid credit. Please enter a valid number." << RESET << endl;
        }

        while (true) {
            cout << "Letter grade (";
            for (const auto& g : validGrades) cout << g << (g == "FF" ? "): " : "/");
            getline(cin, c.letterGrade);
            transform(c.letterGrade.begin(), c.letterGrade.end(), c.letterGrade.begin(), ::toupper);
            if (isValidGrade(c.letterGrade)) break;
            cerr << RED << "Invalid grade. Try again." << RESET << endl;
        }

        courses.push_back(c);
    }
    return courses;
}

void interactiveMode(vector<Course>& courses) {
    string input;
    while (true) {
        cout << BOLD << CYAN << "\nChoose an option:" << RESET << endl;
        cout << " 1. Enter new courses" << endl;
        cout << " 2. View previously saved courses" << endl;
        cout << " 3. Exit" << endl;
        cout << "Enter choice [1-3]: ";
        getline(cin, input);

        if (input == "1") {
            courses = getCourses();
            saveCoursesToFile(courses, "courses.txt");
            break;
        }
        else if (input == "2") {
            courses = loadCoursesFromFile("courses.txt");
            break;
        }
        else if (input == "3") {
            cout << RED << "Exiting program..." << RESET << endl;
            exit(0);
        }
        else {
            cerr << RED << "Invalid input. Please select 1, 2 or 3." << RESET << endl;
        }
    }
}

double calculateGPA(const vector<Course>& courses) {
    double totalPoints = 0.0, totalCredits = 0.0;
    for (const auto& c : courses) {
        totalPoints += gradePointMap[c.letterGrade] * c.credit;
        totalCredits += c.credit;
    }
    return (totalCredits > 0) ? (totalPoints / totalCredits) : 0.0;
}

string gpaClassification(double gpa) {
    if (gpa >= 3.5) return "High Honors";
    if (gpa >= 3.0) return "Honors";
    if (gpa >= 2.0) return "Pass";
    return "Fail";
}

void renderCourseTable(const vector<Course>& courses) {
    cout << line('-') << endl;
    cout << BOLD << left << setw(10) << "Code"
        << setw(25) << "Name"
        << setw(7) << "Credit"
        << setw(10) << "Grade" << RESET << endl;
    cout << line('-') << endl;
    for (const auto& c : courses) {
        cout << formatCourse(c) << endl;
    }
    cout << line('-') << endl;
}

int main() {
    clearScreen();
    cout << BOLD << GREEN << "\nWelcome to the Grade Point Average Calculation System" << RESET << endl;
    cout << line('=') << endl;

    vector<Course> courses;
    interactiveMode(courses);

    double gpa = calculateGPA(courses);

    clearScreen();
    cout << BOLD << CYAN << "\nCourse Summary" << RESET << endl;
    renderCourseTable(courses);
    cout << fixed << setprecision(2);
    cout << BOLD << YELLOW << "\nYour GPA: " << gpa << RESET << endl;
    cout << BOLD << GREEN << "GPA Classification: " << gpaClassification(gpa) << RESET << endl;

    cout << BOLD << RED << "\n\nAZD" << RESET << endl;
    return 0;
}
