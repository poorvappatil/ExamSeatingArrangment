
#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <chrono>

#include "models.h"
#include "greedy.h"
#include "backtracking.h"
#include "conflicts.h"

using namespace std;

// Read an integer safely.
int readInt(string message)
{
    int value;

    while (true)
    {
        cout << message;

        if (cin >> value)
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }

        cout << "Please enter a valid number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

// Read a line of text.
string readText(string message)
{
    string value;
    cout << message;
    getline(cin, value);
    return value;
}

// Create all individual seats from hall dimensions.
vector<Seat> createSeats(const vector<Hall>& halls)
{
    vector<Seat> seats;

    for (int i = 0; i < (int)halls.size(); i++)
    {
        for (int r = 1; r <= halls[i].rows; r++)
        {
            for (int c = 1; c <= halls[i].columns; c++)
            {
                Seat seat;

                seat.hallId = halls[i].id;
                seat.row = r;
                seat.column = c;
                seat.accessible =
                    (c == halls[i].accessibleColumn);
                seat.occupied = false;

                seats.push_back(seat);
            }
        }
    }

    return seats;
}

// Enter student details.
void enterStudents(vector<Student>& students)
{
    int count;

    do
    {
        count = readInt("Number of students: ");

        if (count <= 0)
            cout << "Enter at least one student.\n";

    } while (count <= 0);

    students.clear();

    for (int i = 0; i < count; i++)
    {
        Student student;

        cout << "\nStudent " << i + 1 << "\n";

        student.id = readText("Student ID: ");
        student.name = readText("Student name: ");
        student.className = readText("Class: ");

        int accessible;

        do
        {
            accessible = readInt(
                "Needs accessible seating? (1 = Yes, 0 = No): "
            );

            if (accessible != 0 && accessible != 1)
                cout << "Enter 1 or 0.\n";

        } while (accessible != 0 && accessible != 1);

        student.needsAccessible = (accessible == 1);

        bool duplicate = false;

        for (int j = 0; j < (int)students.size(); j++)
        {
            if (students[j].id == student.id)
                duplicate = true;
        }

        if (duplicate)
        {
            cout << "Duplicate student ID. Enter this student again.\n";
            i--;
        }
        else
        {
            students.push_back(student);
        }
    }
}

// Enter hall details.
void enterHalls(vector<Hall>& halls)
{
    int count;

    do
    {
        count = readInt("Number of halls: ");

        if (count <= 0)
            cout << "Enter at least one hall.\n";

    } while (count <= 0);

    halls.clear();

    for (int i = 0; i < count; i++)
    {
        Hall hall;

        cout << "\nHall " << i + 1 << "\n";

        hall.id = readText("Hall ID: ");
        hall.name = readText("Hall name: ");

        int duplicate;

        do
        {
            duplicate = 0;

            for (int j = 0; j < (int)halls.size(); j++)
            {
                if (halls[j].id == hall.id)
                    duplicate = 1;
            }

            if (duplicate)
            {
                cout << "Duplicate hall ID. Enter a different ID.\n";
                hall.id = readText("Hall ID: ");
            }

        } while (duplicate);

        do
        {
            hall.rows = readInt("Number of rows: ");
            hall.columns = readInt("Number of columns: ");

            if (hall.rows <= 0 || hall.columns <= 0)
                cout << "Rows and columns must be positive.\n";

        } while (hall.rows <= 0 || hall.columns <= 0);

        do
        {
            hall.accessibleColumn =
                readInt("Accessible column (1-based): ");

            if (hall.accessibleColumn < 1 ||
                hall.accessibleColumn > hall.columns)
            {
                cout << "Column must be between 1 and "
                     << hall.columns << ".\n";
            }

        } while (hall.accessibleColumn < 1 ||
                 hall.accessibleColumn > hall.columns);

        halls.push_back(hall);
    }
}

// Display entered students and halls.
void displayData(
    const vector<Student>& students,
    const vector<Hall>& halls
)
{
    cout << "\n--- STUDENTS ---\n";

    for (int i = 0; i < (int)students.size(); i++)
    {
        cout << students[i].id << " | "
             << students[i].name << " | "
             << students[i].className << " | Accessible: "
             << (students[i].needsAccessible ? "Yes" : "No")
             << "\n";
    }

    cout << "\n--- HALLS ---\n";

    for (int i = 0; i < (int)halls.size(); i++)
    {
        cout << halls[i].id << " | "
             << halls[i].name << " | "
             << halls[i].rows << " rows x "
             << halls[i].columns << " columns | Accessible column: "
             << halls[i].accessibleColumn << "\n";
    }
}

// Display allocation results and check for conflicts.
void displayResults(
    const vector<Student>& students,
    const vector<Seat>& seats,
    const vector<int>& allocation
)
{
    int allocated = 0;

    cout << "\n--- SEATING ALLOCATION ---\n";

    for (int i = 0; i < (int)students.size(); i++)
    {
        cout << students[i].id << " - "
             << students[i].name << ": ";

        if (allocation[i] == -1)
        {
            cout << "UNALLOCATED\n";
        }
        else
        {
            int seatIndex = allocation[i];

            cout << "Hall " << seats[seatIndex].hallId
                 << ", Row " << seats[seatIndex].row
                 << ", Column " << seats[seatIndex].column
                 << "\n";

            allocated++;
        }
    }

    cout << "\nAllocated students: " << allocated
         << " / " << students.size() << "\n";

    cout << "Unallocated students: "
         << students.size() - allocated << "\n";

    vector<string> conflicts =
        detectConflicts(students, seats, allocation);

    cout << "\n--- CONFLICT CHECK ---\n";

    if (conflicts.empty())
    {
        cout << "No conflicts detected.\n";
    }
    else
    {
        for (int i = 0; i < (int)conflicts.size(); i++)
            cout << "- " << conflicts[i] << "\n";
    }
}

int main()
{
    vector<Student> students;
    vector<Hall> halls;
    vector<Seat> seats;

    cout << "EXAMINATION SEATING ALLOCATION\n";
    cout << "Enter data for this session.\n\n";

    enterStudents(students);
    enterHalls(halls);

    seats = createSeats(halls);

    vector<int> lastAllocation;
    string lastAlgorithm = "None";

    int choice;

    do
    {
        cout << "\n========== MENU ==========\n";
        cout << "1. Display students and halls\n";
        cout << "2. Run Greedy allocation\n";
        cout << "3. Run Backtracking allocation\n";
        cout << "4. Compare both algorithms\n";
        cout << "5. Display most recent allocation\n";
        cout << "6. Exit\n";

        choice = readInt("Enter choice: ");

        if (choice == 1)
        {
            displayData(students, halls);
        }
        else if (choice == 2)
        {
            lastAllocation = allocateGreedy(students, seats);
            lastAlgorithm = "Greedy";

            displayResults(students, seats, lastAllocation);
        }
        else if (choice == 3)
        {
            lastAllocation =
                allocateBacktracking(students, seats);

            lastAlgorithm = "Backtracking";

            displayResults(students, seats, lastAllocation);
        }
        else if (choice == 4)
        {
            auto startGreedy = chrono::steady_clock::now();

            vector<int> greedyResult =
                allocateGreedy(students, seats);

            auto endGreedy = chrono::steady_clock::now();

            auto startBacktracking = chrono::steady_clock::now();

            vector<int> backtrackingResult =
                allocateBacktracking(students, seats);

            auto endBacktracking = chrono::steady_clock::now();

            long long greedyTime =
                chrono::duration_cast<chrono::microseconds>(
                    endGreedy - startGreedy
                ).count();

            long long backtrackingTime =
                chrono::duration_cast<chrono::microseconds>(
                    endBacktracking - startBacktracking
                ).count();

            int greedyCount = 0;
            int backtrackingCount = 0;

            for (int i = 0; i < (int)students.size(); i++)
            {
                if (greedyResult[i] != -1)
                    greedyCount++;

                if (backtrackingResult[i] != -1)
                    backtrackingCount++;
            }

            cout << "\n--- ALGORITHM COMPARISON ---\n";
            cout << "Greedy allocated: "
                 << greedyCount << " / " << students.size() << "\n";
            cout << "Greedy time: " << greedyTime << " microseconds\n";

            cout << "Backtracking allocated: "
                 << backtrackingCount << " / " << students.size() << "\n";
            cout << "Backtracking time: "
                 << backtrackingTime << " microseconds\n";

            cout << "\nGreedy conflict check:\n";
            vector<string> greedyConflicts =
                detectConflicts(students, seats, greedyResult);

            if (greedyConflicts.empty())
                cout << "No conflicts detected.\n";
            else
                for (int i = 0; i < (int)greedyConflicts.size(); i++)
                    cout << "- " << greedyConflicts[i] << "\n";

            cout << "\nBacktracking conflict check:\n";
            vector<string> backtrackingConflicts =
                detectConflicts(students, seats, backtrackingResult);

            if (backtrackingConflicts.empty())
                cout << "No conflicts detected.\n";
            else
                for (int i = 0; i < (int)backtrackingConflicts.size(); i++)
                    cout << "- " << backtrackingConflicts[i] << "\n";
        }
        else if (choice == 5)
        {
            if (lastAllocation.empty())
            {
                cout << "No allocation has been run yet.\n";
            }
            else
            {
                cout << "Most recent algorithm: "
                     << lastAlgorithm << "\n";

                displayResults(students, seats, lastAllocation);
            }
        }
        else if (choice == 6)
        {
            cout << "Exiting program. Goodbye!\n";
        }
        else
        {
            cout << "Invalid choice. Select 1 to 6.\n";
        }

    } while (choice != 6);

    return 0;
}

