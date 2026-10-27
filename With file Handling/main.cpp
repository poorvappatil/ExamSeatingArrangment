#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <iomanip>
#include <fstream>

#include "models.h"
#include "greedy.h"
#include "conflicts.h"

using namespace std;

class ExamSeatingSystem {
private:
    vector<Student> students;
    vector<Hall> halls;
    vector<Seat> seats;
    vector<int> lastAllocation;
    unordered_map<string, Student> studentMap;

    // save all student records to a text file
    void saveStudents() {
        ofstream file("students.txt");
        if (!file) {
            cout << "Could not save students.txt" << endl;
            return;
        }

        for (Student& student : students) {
            file << student.id << "|"
                 << student.name << "|"
                 << student.className << "|"
                 << student.needsAccessible << "\n";
        }

        file.close();
    }

    // save all hall records to a text file
    void saveHalls() {
        ofstream file("halls.txt");
        if (!file) {
            cout << "Could not save halls.txt" << endl;
            return;
        }

        for (Hall& hall : halls) {
            file << hall.id << "|"
                 << hall.name << "|"
                 << hall.rows << "|"
                 << hall.columns << "|"
                 << hall.accessibleColumn << "\n";
        }

        file.close();
    }

    // load student records when the program starts
    void loadStudents() {
        ifstream file("students.txt");
        if (!file) return;

        string id, name, className, accessibleText;
        while (getline(file, id, '|')) {
            if (!getline(file, name, '|')) break;
            if (!getline(file, className, '|')) break;
            if (!getline(file, accessibleText)) break;

            Student student;
            student.id = id;
            student.name = name;
            student.className = className;
            student.needsAccessible = (accessibleText == "1");

            // ignore empty ids and duplicate ids in the file
            if (student.id.empty() || studentMap.count(student.id) > 0) continue;

            students.push_back(student);
            studentMap[student.id] = student;
        }

        file.close();
    }

    // load hall records when the program starts
    void loadHalls() {
        ifstream file("halls.txt");
        if (!file) return;

        string id, name, rowsText, columnsText, accessibleText;
        while (getline(file, id, '|')) {
            if (!getline(file, name, '|')) break;
            if (!getline(file, rowsText, '|')) break;
            if (!getline(file, columnsText, '|')) break;
            if (!getline(file, accessibleText)) break;

            try {
                Hall hall;
                hall.id = id;
                hall.name = name;
                hall.rows = stoi(rowsText);
                hall.columns = stoi(columnsText);
                hall.accessibleColumn = stoi(accessibleText);

                if (hall.id.empty() || hall.rows <= 0 || hall.columns <= 0 || hall.accessibleColumn < 1 || hall.accessibleColumn > hall.columns) {
                    continue;
                }

                bool duplicate = false;
                for (Hall& existing : halls) {
                    if (existing.id == hall.id) duplicate = true;
                }
                if (!duplicate) halls.push_back(hall);
            } 
            catch (...) {}
        }

        file.close();
    }

public:
    ExamSeatingSystem() {
        loadStudents();
        loadHalls();
        createSeats();

        if (!students.empty() || !halls.empty()) {
            cout << "Saved data loaded from text files." << endl;
        }
    }

    // read a number and handle invalid input
    int readInt(string message){
        int number;

        while(true){
            cout<<message;

            if(cin>>number){
                cin.ignore(1000,'\n');
                return number;
            }

            cout<<"Invalid input. Please enter a valid number."<<endl;
            cin.clear();
            cin.ignore(1000,'\n');
        }
    }

    /// read a full line
    string readText(string message){
        string text;
        cout<<message;
        getline(cin,text);
        return text;
    }

    // create a seat for every position in each hall
    void createSeats() {
        seats.clear();
        for (int i = 0; i < halls.size(); i++) {
            for (int row = 1; row <= halls[i].rows; row++) {
                for (int col = 1; col <= halls[i].columns; col++) {
                    Seat seat;
                    seat.hallId = halls[i].id;
                    seat.row = row;
                    seat.column = col;
                    seat.accessible = (col == halls[i].accessibleColumn);
                    seat.occupied = false;
                    seats.push_back(seat);
                }
            }
        }
    }

    // enter student details and avoid duplicate ids
    void enterStudents() {
        int count;
        do {
            count = readInt("Number of new students: ");
            if (count <= 0) cout << "Enter at least one student." << endl;
        } while (count <= 0);

        for (int i = 0; i < count; i++) {
            Student student;
            bool duplicate;
            do {
                cout << "\nStudent " << i + 1 << endl;
                student.id = readText("Student ID: ");

                if (student.id.empty()) {
                    cout << "Student ID cannot be empty." << endl;
                    duplicate = true;
                    continue;
                }

                duplicate = (studentMap.find(student.id) != studentMap.end());

                if (duplicate) cout << "That ID already exists. Please try again." << endl;
            } while (duplicate);

            student.name = readText("Student name: ");
            student.className = readText("Class: ");

            int accessible;
            do {
                accessible = readInt("Needs accessible seating? (1 = Yes, 0 = No): ");
                if (accessible != 0 && accessible != 1) cout << "Enter 1 for Yes or 0 for No." << endl;
            } while (accessible != 0 && accessible != 1);

            student.needsAccessible = (accessible == 1);
            students.push_back(student);
            studentMap[student.id] = student;
        }

        lastAllocation.clear();
        saveStudents();
        cout << "Student details saved to students.txt" << endl;
    }

    // enter hall details and avoid duplicate hall ids
    void enterHalls() {
        int count;
        do {
            count = readInt("Number of new halls: ");
            if (count <= 0) cout << "Enter at least one hall." << endl;
        } while (count <= 0);

        for (int i = 0; i < count; i++) {
            Hall hall;
            bool duplicate;
            do {
                cout << "\nHall " << i + 1 << endl;
                hall.id = readText("Hall ID: ");

                if (hall.id.empty()) {
                    cout << "Hall ID cannot be empty." << endl;
                    duplicate = true;
                    continue;
                }

                duplicate = false;
                for (Hall& existing : halls) {
                    if (existing.id == hall.id) duplicate = true;
                }

                if (duplicate) cout << "That hall ID already exists. Try another one." << endl;
            } while (duplicate);

            hall.name = readText("Hall name: ");
            do {
                hall.rows = readInt("Number of rows: ");
                hall.columns = readInt("Number of columns: ");
                if (hall.rows <= 0 || hall.columns <= 0) cout << "Rows and columns must be greater than zero." << endl;
            } while (hall.rows <= 0 || hall.columns <= 0);

            do {
                hall.accessibleColumn = readInt("Accessible column (1-based): ");
                if (hall.accessibleColumn < 1 || hall.accessibleColumn > hall.columns) cout << "Enter a column between 1 and " << hall.columns << "." << endl;
            } while (hall.accessibleColumn < 1 || hall.accessibleColumn > hall.columns);

            halls.push_back(hall);
        }

        createSeats();
        lastAllocation.clear();
        saveHalls();
        cout << "Hall details saved to halls.txt" << endl;
    }

    // search for a student using the hash map
    void searchStudent() {
        if (students.empty()) {
            cout << "No student records available." << endl;
            return;
        }

        string id = readText("Enter Student ID to search: ");
        auto it = studentMap.find(id);
        if (it != studentMap.end()) {
            Student student = it->second;
            cout << "\nStudent found!" << endl;
            cout << "Student ID: " << student.id << endl;
            cout << "Student Name: " << student.name << endl;
            cout << "Class: " << student.className << endl;
            cout << "Needs accessible seating: "<< (student.needsAccessible ? "Yes" : "No") << endl;
        }
        else {
            cout << "Student not found." << endl;
        }
    }

    // display all student and hall details
    void displayData() {
        cout << "\n \t\t\t STUDENTS" << endl;
        for (Student& student : students) {
            cout << "ID: " << student.id << "\tName: " << student.name
                 << "\tClass: " << student.className
                 << "\tNeeds accessible seating: "
                 << (student.needsAccessible ? "Yes" : "No") << endl;
        }

        cout << "\n \t\t\t HALLS" << endl;
        for (Hall& hall : halls) {
            cout << hall.id << "\t" << hall.name << "\t"
                 << hall.rows << " rows x " << hall.columns
                 << " columns\tAccessible column: " << hall.accessibleColumn << endl;
        }
    }

    // count students who have a valid seat
    int countAllocated() {
        int count = 0;
        for (int seatIndex : lastAllocation) {
            if (seatIndex >= 0 && seatIndex < seats.size()) count++;
        }
        return count;
    }

    // show seats like a hall layout
    void showSeatMap() {
        cout << "\n \t\t\t SEATING MAP \n";
        for (Hall& hall : halls) {
            cout << "\nHall " << hall.id << " - " << hall.name << endl;
            cout << setw(10) << " ";
            for (int col = 1; col <= hall.columns; col++)
                cout << setw(12) << ("Col " + to_string(col));
            cout << endl;

            for (int row = 1; row <= hall.rows; row++) {
                cout << setw(10) << ("Row " + to_string(row));
                for (int col = 1; col <= hall.columns; col++) {
                    string cell = "----";
                    int seatIndex = -1;

                    // find the seat at this position
                    for (int s = 0; s < seats.size(); s++) {
                        if (seats[s].hallId == hall.id &&
                            seats[s].row == row && seats[s].column == col) {
                            seatIndex = s;
                            break;
                        }
                    }

                    // find the student assigned to this seat
                    if (seatIndex != -1) {
                        for (int i = 0; i < lastAllocation.size(); i++) {
                            if (lastAllocation[i] == seatIndex) {
                                cell = students[i].id;
                                break;
                            }
                        }
                        if (seats[seatIndex].accessible) cell += "*";
                    }
                    cout << setw(12) << ("[" + cell + "]");
                }
                cout << endl;
            }
        }
        cout << "\n* = accessible seat" << endl;
        cout << "---- = empty seat" << endl;
    }

    // show student assignments and check for conflicts
    void displayResults() {
        cout << "\n \t\t\t SEATING ALLOCATION " << endl;
        for (int i = 0; i < students.size(); i++) {
            cout << students[i].id << " - " << students[i].name << ": ";
            if (i >= lastAllocation.size() || lastAllocation[i] == -1) {
                cout << "UNALLOCATED" << endl;
            }
            else if (lastAllocation[i] < 0 ||
                       lastAllocation[i] >= seats.size()) {
                cout << "INVALID SEAT INDEX" << endl;
            }
            else {
                int seatIndex = lastAllocation[i];
                cout << "Hall " << seats[seatIndex].hallId
                     << ", Row " << seats[seatIndex].row
                     << ", Column " << seats[seatIndex].column << endl;
            }
        }

        int allocated = countAllocated();
        cout << "\nAllocated: " << allocated << " / " << students.size() << endl;
        cout << "Unallocated: " << students.size() - allocated << endl;

        ConflictReport report = generateConflictReport(students, seats, lastAllocation, halls);
        cout << "\n \t\t\t CONFLICT CHECK " << endl;
        if (report.hardViolations.empty()) {
            cout << "No hard violations detected." << endl;
        } else {
            cout << "\nHard violations:" << endl;
            for (string& item : report.hardViolations) cout << "- " << item << endl;
        }

        if (report.unallocatedStudents.empty()) {
            cout << "\nAll students have been allocated." << endl;
        }
        else {
            cout << "\nUnallocated students:" << endl;
            for (string& item : report.unallocatedStudents) cout << "- " << item << endl;
        }
        showSeatMap();
    }

    // run the greedy algorithm
    void runGreedy() {
        if (halls.empty() || students.empty()) {
            cout << "Please enter both hall and student details first." << endl;
            return;
        }
        lastAllocation = allocateGreedy(students, seats);
        displayResults();
    }

    // check if student and hall details are available
    bool hasData()  {
        return !halls.empty() && !students.empty();
    }
};

int main() {
    ExamSeatingSystem system;
    int choice;

    do {
        cout << "\n \t\t\t EXAM SEATING SYSTEM " << endl;
        cout << "1. Enter hall details" << endl;
        cout << "2. Enter student details" << endl;
        cout << "3. Display saved data" << endl;
        cout << "4. Run greedy algorithm" << endl;
        cout << "5. Search student by ID" << endl;
        cout << "6. Exit" << endl;
        choice = system.readInt("Enter your choice: ");

        switch (choice) {
            case 1:
                system.enterHalls();
                break;

            case 2:
                system.enterStudents();
                break;

            case 3:
                if (!system.hasData()) {
                    cout << "No saved student or hall data is available yet." << endl;
                    break;
                }
                system.displayData();
                break;

            case 4:
                system.runGreedy();
                break;

            case 5:
                system.searchStudent();
                break;

            case 6:
                cout << "Exiting program..." << endl;
                break;

            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    } while (choice != 6);

    cout << "Program ended successfully." << endl;
    return 0;
}
