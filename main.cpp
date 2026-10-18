#include <iostream>
#include <vector>
#include <string>

#include "models.h"
#include "greedy.h"
#include "backtracking.h"
#include "conflicts.h"

using namespace std;

//to read a number and handle invalid input
int readInt(string message){

    int number;

    while(true){
        cout<<message;

        if(cin>>number){
            cin.ignore(1000,'\n');
            return number;
        }

        cout<<"Invalid input. Please enter a valid number.\n";
        cin.clear();
        cin.ignore(1000,'\n');
    }
}

// Read a full line including spaces
string readText(string message){
    string text;
    cout << message;
    getline(cin, text);
    return text;
}

// Create a separate Seat object for every position in every hall
vector<Seat> createSeats(const vector<Hall>& halls){
    vector<Seat> seats;

    for (int i = 0; i < (int)halls.size(); i++){
        for (int row = 1; row <= halls[i].rows; row++){
            for (int col = 1; col <= halls[i].columns; col++){
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

    return seats;
}

// Enter student information and prevent duplicate IDs
void enterStudents(vector<Student>& students){
    int count;

    do{
        count = readInt("Number of students: ");

        if (count <= 0)
            cout << "Enter at least one student.\n";

    } while (count <= 0);

    students.clear();

    for (int i = 0; i < count; i++)
    {
        Student student;
        bool duplicate;

        do{
            cout << "\nStudent " << i + 1 << "\n";

            student.id = readText("Student ID: ");

            duplicate = false;

            //check for duplicates
            for (int j = 0; j < (int)students.size(); j++){
                if (students[j].id == student.id) duplicate = true;
            }

            if (duplicate==true)
                cout << "That ID already exists. Please try again.\n";

        } while (duplicate==true);

        student.name = readText("Student name: ");
        student.className = readText("Class: ");

        int accessible;

        do{
            accessible = readInt("Needs accessible seating? (1 = Yes, 0 = No): ");

            if(accessible != 0 && accessible != 1) cout << "Enter 1 for Yes or 0 for No.\n";

        } while(accessible != 0 && accessible != 1);

        student.needsAccessible = (accessible == 1);

        students.push_back(student);
    }
}

// Enter hall information and prevent duplicate hall IDs
void enterHalls(vector<Hall>& halls){
    int count;

    do{
        count = readInt("Number of halls: ");

        if (count <= 0)
            cout << "Enter at least one hall.\n";

    } while (count <= 0);

    halls.clear();

    for (int i = 0; i < count; i++){
        Hall hall;
        bool duplicate;

        do{
            cout << "\nHall " << i + 1 << "\n";

            hall.id = readText("Hall ID: ");

            duplicate = false;

            //check for duplicates
            for (int j = 0; j < (int)halls.size(); j++)
            {
                if (halls[j].id == hall.id)
                    duplicate = true;
            }

            if (duplicate==true)
                cout << "That hall ID already exists. Try another one.\n";

        } while (duplicate==true);

        hall.name = readText("Hall name: ");

        do{
            hall.rows = readInt("Number of rows: ");
            hall.columns = readInt("Number of columns: ");

            if(hall.rows <= 0 || hall.columns <= 0) cout << "Rows and columns must be greater than zero.\n";

        } while(hall.rows <= 0 || hall.columns <= 0);

        do{
            hall.accessibleColumn = readInt("Accessible column (1-based): ");

            if(hall.accessibleColumn < 1 || hall.accessibleColumn > hall.columns){
                cout << "Enter a column between 1 and "
                     << hall.columns << ".\n";
            }

        } while(hall.accessibleColumn < 1 || hall.accessibleColumn > hall.columns);

        halls.push_back(hall);
    }
}

// Show the student and hall details entered by the user
void displayData(const vector<Student>& students, const vector<Hall>& halls){
    cout << "\n \t\t\t\t STUDENTS \n";

    for (int i = 0; i < (int)students.size(); i++){
        cout << students[i].id << " | "
             << students[i].name << " | "
             << students[i].className << " | Needs accessible seat: "
             << (students[i].needsAccessible ? "Yes" : "No") << "\n";
    }

    cout << "\n \t\t\t\t HALLS \n";

    for (int i = 0; i < (int)halls.size(); i++){
        cout << halls[i].id << " | "
             << halls[i].name << " | "
             << halls[i].rows << " rows x "
             << halls[i].columns << " columns | Accessible column: "
             << halls[i].accessibleColumn << "\n";
    }
}

// Count how many students received seats
int countAllocated(const vector<int>& allocation){
    int count = 0;

    for (int i = 0; i < (int)allocation.size(); i++){
        if (allocation[i] != -1) count++;
    }

    return count;
}

// Display seat assignments and run the conflict checker.
void displayResults(const vector<Student>& students,const vector<Seat>& seats,const vector<int>& allocation){
    cout << "\n \t\t\t\t SEATING ALLOCATION \n";

    for (int i = 0; i < (int)students.size(); i++){
        cout << students[i].id << " - "<< students[i].name << ": ";

        if (i >= (int)allocation.size() || allocation[i] == -1)
        {
            cout << "UNALLOCATED\n";
        }
        else if (allocation[i] < 0 ||
                 allocation[i] >= (int)seats.size())
        {
            cout << "INVALID SEAT INDEX\n";
        }
        else
        {
            int seatIndex = allocation[i];

            cout << "Hall " << seats[seatIndex].hallId
                 << ", Row " << seats[seatIndex].row
                 << ", Column " << seats[seatIndex].column << "\n";
        }
    }

    int allocated = countAllocated(allocation);

    cout << "\nAllocated: " << allocated << " / " << students.size() << "\n";

    cout << "Unallocated: " << students.size() - allocated << "\n";

    vector<string> conflicts = detectConflicts(students, seats, allocation);

    cout << "\n \t\t\t\t CONFLICT CHECK \n";

    if (conflicts.empty()){
        cout << "No conflicts detected.\n";
    }

    else{
        for (int i = 0; i <conflicts.size(); i++)
            cout << "- " << conflicts[i] << "\n";
    }
}

int main()
{
    vector<Student> students;
    vector<Hall> halls;
    vector<Seat> seats;
    vector<int> lastAllocation;

    string lastAlgorithm = "None";
    int choice;

    do
    {
        cout << "\n \t\t\t\t EXAM SEATING SYSTEM \n";
        cout << "1. Display Data\n";
        cout << "2. Run Greedy Algorithm\n";
        cout << "3. Run Backtracking Algorithm\n";
        cout << "4. Show Last Allocation\n";
        cout << "5. Exit\n";

        choice = readInt("Enter your choice: ");

        switch (choice)
        {
            case 1:
                displayData(students, halls);
                break;

            case 2:
                lastAllocation = allocateGreedy(students, seats);
                lastAlgorithm = "Greedy";
                displayResults(students, seats, lastAllocation);
                break;

            case 3:
                lastAllocation = allocateBacktracking(students, seats);
                lastAlgorithm = "Backtracking";
                displayResults(students, seats, lastAllocation);
                break;

            case 4:
                displayResults(students, seats, lastAllocation);
                break;

            case 5:
                cout << "Exiting program...\n";
                break;

            default:
                cout << "Invalid choice. Please try again.\n";
        }

    } while (choice != 5);

    return 0;
}
