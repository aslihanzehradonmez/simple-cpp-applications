#include <iostream>
#include <vector>
#include <iomanip>
#include <limits>
#include <string>
#include <map>
#include <numeric>

using namespace std;

struct Student {
    string name;
    string surname;
    map<string, double> subjectGrades;
    double averageGrade;
    string letterGrade;
};

class GradeCalculator {
private:
    map<string, double> subjectWeights;
    map<pair<double, double>, string> gradeRanges;
    vector<Student> students;

    void fakeClearScreen() const {
        for (int i = 0; i < 50; ++i) {
            cout << endl;
        }
    }

    double calculateAverage(const Student& student) const {
        double weightedSum = 0.0;
        double totalWeight = 0.0;
        for (const auto& gradePair : student.subjectGrades) {
            const string& subject = gradePair.first;
            double grade = gradePair.second;
            auto it = subjectWeights.find(subject);
            if (it != subjectWeights.end()) {
                weightedSum += grade * it->second;
                totalWeight += it->second;
            }
            else {
                weightedSum += grade;
                totalWeight += 1.0;
            }
        }
        return (totalWeight > 0) ? (weightedSum / totalWeight) : 0.0;
    }

    string assignLetterGrade(double average) const {
        for (const auto& range : gradeRanges) {
            if (average >= range.first.first && average <= range.first.second) {
                return range.second;
            }
        }
        return "FF";
    }

public:
    GradeCalculator() {
        subjectWeights["Midterm"] = 0.4;
        subjectWeights["Final"] = 0.6;

        gradeRanges[{90.0, 100.0}] = "AA";
        gradeRanges[{85.0, 89.99}] = "BA";
        gradeRanges[{80.0, 84.99}] = "BB";
        gradeRanges[{75.0, 79.99}] = "CB";
        gradeRanges[{70.0, 74.99}] = "CC";
        gradeRanges[{65.0, 69.99}] = "DC";
        gradeRanges[{60.0, 64.99}] = "DD";
        gradeRanges[{50.0, 59.99}] = "FD";
        gradeRanges[{0.0, 49.99}] = "FF";
    }

    void setSubjectWeight(const string& subject, double weight) {
        subjectWeights[subject] = weight;
    }

    void setGradeRange(double lowerBound, double upperBound, const string& letter) {
        gradeRanges[{lowerBound, upperBound}] = letter;
    }

    void addStudent(const Student& student) {
        students.push_back(student);
    }

    void processStudent(Student& student) {
        student.averageGrade = calculateAverage(student);
        student.letterGrade = assignLetterGrade(student.averageGrade);
    }

    void calculateClassAverageForSubject(const string& subject) const {
        double sum = 0.0;
        int count = 0;
        for (const auto& student : students) {
            auto it = student.subjectGrades.find(subject);
            if (it != student.subjectGrades.end()) {
                sum += it->second;
                count++;
            }
        }
        if (count > 0) {
            cout << "Class average for " << subject << ": " << fixed << setprecision(2) << (sum / count) << endl;
        }
        else {
            cout << "No grades found for " << subject << " for any student." << endl;
        }
        cout << "\nPress Enter to return to the menu...";
        cin.get();
    }

    void calculateClassOverallAverage() const {
        if (students.empty()) {
            cout << "No student data available." << endl;
        }
        else {
            double totalAverageSum = 0.0;
            for (const auto& student : students) {
                totalAverageSum += student.averageGrade;
            }
            cout << "Class overall average: " << fixed << setprecision(2) << (totalAverageSum / students.size()) << endl;
        }
        cout << "\nPress Enter to return to the menu...";
        cin.get();
    }

    void displayStudentResults(const Student& student) const {
        cout << "\n--- Student Results ---" << endl;
        cout << "Name: " << student.name << " " << student.surname << endl;
        cout << "Subject Grades: ";
        for (const auto& gradePair : student.subjectGrades) {
            cout << gradePair.first << "(" << fixed << setprecision(2) << gradePair.second << ") ";
        }
        cout << endl;
        cout << "Average Grade: " << fixed << setprecision(2) << student.averageGrade << endl;
        cout << "Letter Grade: " << student.letterGrade << endl;
        cout << "\nPress Enter to return to the menu...";
        cin.get();
    }

    void displayAllStudentResults() const {
        cout << "\n--- All Student Results ---" << endl;
        cout << setw(10) << left << "Name"
            << setw(10) << left << "Surname"
            << setw(15) << right << "Average Grade"
            << setw(10) << right << "Letter Grade" << endl;
        cout << setfill('-') << setw(45) << "" << setfill(' ') << endl;

        for (const auto& student : students) {
            cout << setw(10) << left << student.name
                << setw(10) << left << student.surname
                << setw(15) << right << fixed << setprecision(2) << student.averageGrade
                << setw(10) << right << student.letterGrade << endl;
        }
        cout << "\nPress Enter to return to the menu...";
        cin.get();
    }

    Student getStudentInput() {
        Student student;
        cout << "Enter student name: ";
        getline(cin >> ws, student.name);
        cout << "Enter student surname: ";
        getline(cin >> ws, student.surname);

        int numSubjects;
        cout << "Enter the number of subjects for " << student.name << ": ";
        cin >> numSubjects;
        cin.ignore();

        for (int i = 0; i < numSubjects; ++i) {
            string subjectName;
            double grade;
            cout << "Enter subject name #" << i + 1 << ": ";
            getline(cin >> ws, subjectName);
            cout << "Enter grade for " << subjectName << " (0-100): ";
            cin >> grade;
            while (cin.fail() || grade < 0 || grade > 100) {
                cout << "Invalid grade. Please enter a value between 0 and 100: ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cin >> grade;
            }
            cin.ignore();
            student.subjectGrades[subjectName] = grade;
        }
        processStudent(student);
        return student;
    }

    void getClassDataInput() {
        int numStudents;
        cout << "Enter the number of students in the class: ";
        cin >> numStudents;
        cin.ignore();
        for (int i = 0; i < numStudents; ++i) {
            cout << "\n--- Entering data for student #" << i + 1 << " ---" << endl;
            students.push_back(getStudentInput());
        }
        cout << "\nClass data entered successfully. Press Enter to return to the menu...";
        cin.get();
    }

public:
    void run() {
        int choice;
        do {
            fakeClearScreen();
            cout << "\n--- Grade Calculator Menu ---" << endl;
            cout << "1. Enter and Calculate for a Single Student" << endl;
            cout << "2. Enter Class Data" << endl;
            cout << "3. Calculate Class Average for a Subject" << endl;
            cout << "4. Calculate Class Overall Average" << endl;
            cout << "5. Display All Student Results" << endl;
            cout << "6. Exit" << endl;
            cout << "Enter your choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
            case 1: {
                Student singleStudent = getStudentInput();
                displayStudentResults(singleStudent);
                addStudent(singleStudent);
                break;
            }
            case 2:
                getClassDataInput();
                break;
            case 3: {
                if (!students.empty()) {
                    string subjectName;
                    cout << "Enter the subject name to calculate the class average: ";
                    getline(cin >> ws, subjectName);
                    calculateClassAverageForSubject(subjectName);
                }
                else {
                    cout << "No student data available. Please enter class data first." << endl;
                    cout << "\nPress Enter to return to the menu...";
                    cin.get();
                }
                break;
            }
            case 4:
                calculateClassOverallAverage();
                break;
            case 5:
                displayAllStudentResults();
                break;
            case 6:
                cout << "Exiting the Grade Calculator." << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
                cout << "\nPress Enter to return to the menu...";
                cin.get();
            }
        } while (choice != 6);

        cout << "\nAZD" << endl;
    }
};

int main() {
    GradeCalculator calculator;
    calculator.run();
    return 0;
}