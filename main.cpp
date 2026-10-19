#include<iostream>
#include<vector>
#include<string>
#include<unordered_map>

#include "models.h"
#include "greedy.h"
#include "backtracking.h"
#include "conflicts.h"

using namespace std;

class ExamSeatingSystem{
private:
    vector<Student> students;
    vector<Hall> halls;
    vector<Seat> seats;
    vector<int> lastAllocation;

    string lastAlgorithm="None";

    unordered_map<string,Student> studentMap;

public:
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

    // read a full line
    string readText(string message){
        string text;
        cout<<message;
        getline(cin,text);
        return text;
    }

    // create a seat for every position in each hall
    void createSeats(){
        seats.clear();

        for(int i=0;i<halls.size();i++){
            for(int row=1;row<=halls[i].rows;row++){
                for(int col=1;col<=halls[i].columns;col++){
                    Seat seat;

                    seat.hallId=halls[i].id;
                    seat.row=row;
                    seat.column=col;
                    seat.accessible=(col==halls[i].accessibleColumn);
                    seat.occupied=false;

                    seats.push_back(seat);
                }
            }
        }
    }

    // enter student details and avoid duplicate IDs using hashing
    void enterStudents(){
        int count;

        do{
            count=readInt("Number of students: ");

            if(count<=0) cout<<"Enter at least one student."<<endl;

        }while(count<=0);

        for(int i=0;i<count;i++){
            Student student;
            bool duplicate;

            do{
                cout<<"\nStudent "<<i+1<<endl;
                student.id=readText("Student ID: ");

                if(student.id.empty()){
                    cout<<"Student ID cannot be empty."<<endl;
                    duplicate=true;
                    continue;
                }

                duplicate=(studentMap.find(student.id)!=studentMap.end());

                if(duplicate==true){
                    cout<<"That ID already exists. Please try again."<<endl;
                }

            }while(duplicate==true);

            student.name=readText("Student name: ");
            student.className=readText("Class: ");

            int accessible;

            do{
                accessible=readInt("Needs accessible seating? (1 = Yes, 0 = No): ");

                if(accessible!=0 && accessible!=1) cout<<"Enter 1 for Yes or 0 for No."<<endl;

            }while(accessible!=0 && accessible!=1);

            student.needsAccessible=(accessible==1);

            students.push_back(student);
            studentMap[student.id]=student;
        }

        lastAllocation.clear();
        lastAlgorithm="None";
    }

    // enter hall details and avoid duplicate hall IDs
    void enterHalls(){
        int count;

        do{
            count=readInt("Number of halls: ");

            if(count<=0) cout<<"Enter at least one hall."<<endl;

        }while(count<=0);

        for(int i=0;i<count;i++){
            Hall hall;
            bool duplicate;

            do{
                cout<<"\nHall "<<i+1<<endl;
                hall.id=readText("Hall ID: ");

                if(hall.id.empty()){
                    cout<<"Hall ID cannot be empty."<<endl;
                    duplicate=true;
                    continue;
                }

                duplicate=false;

                for(int j=0;j<halls.size();j++){
                    if(halls[j].id==hall.id){
                        duplicate=true;
                        break;
                    }
                }

                if(duplicate==true) cout<<"That hall ID already exists. Try another one."<<endl;

            }while(duplicate==true);

            hall.name=readText("Hall name: ");

            do{
                hall.rows=readInt("Number of rows: ");
                hall.columns=readInt("Number of columns: ");

                if(hall.rows<=0 || hall.columns<=0) cout<<"Rows and columns must be greater than zero."<<endl;

            }while(hall.rows<=0 || hall.columns<=0);

            do{
                hall.accessibleColumn=readInt("Accessible column (1-based): ");

                if(hall.accessibleColumn<1 || hall.accessibleColumn>hall.columns){
                    cout<<"Enter a column between 1 and "<<hall.columns<<"."<<endl;
                }

            }while(hall.accessibleColumn<1 || hall.accessibleColumn>hall.columns);

            halls.push_back(hall);
        }

        createSeats();
        lastAllocation.clear();
        lastAlgorithm="None";
    }

    // search for a student using hashing
    void searchStudent(){
        if(students.empty()){
            cout<<"No student records available."<<endl;
            return;
        }

        string id=readText("Enter Student ID to search: ");

        auto it=studentMap.find(id);

        if(it!=studentMap.end()){
            Student student=it->second;

            cout<<"\nStudent Found!"<<endl;
            cout<<"Student ID: "<<student.id<<endl;
            cout<<"Student Name: "<<student.name<<endl;
            cout<<"Class: "<<student.className<<endl;
            cout<<"Needs Accessible Seating: "<<(student.needsAccessible ? "Yes" : "No")<<endl;
        }
        else{
            cout<<"Student not found."<<endl;
        }
    }

    // display all student and hall details
    void displayData(){
        cout<<"\n \t\t\t\t STUDENTS "<<endl;

        for(int i=0;i<students.size();i++){
            cout<<"ID:"<<students[i].id<<"\tName:" <<students[i].name<<"\t Class:" <<students[i].className<<"\t Needs Accessible?: "<<(students[i].needsAccessible ? "Yes" : "No")<<endl;
        }

        cout<<"\n \t\t\t\t HALLS "<<endl;

        for(int i=0;i<halls.size();i++){
            cout<<halls[i].id<<" \t " <<halls[i].name<<" \t " <<halls[i].rows<<" rows x "<<halls[i].columns<<" columns \t Accessible column: "<<halls[i].accessibleColumn<<endl;
        }
    }

    // count students who have been assigned a seat
    int countAllocated(){
        int count=0;

        for(int i=0;i<lastAllocation.size();i++){
            if(lastAllocation[i]!=-1){
                count++;
            }
        }

        return count;
    }

    // show seat assignments and check for conflicts
    void displayResults(){
        cout<<"\n \t\t\t\t SEATING ALLOCATION "<<endl;

        for(int i=0;i<students.size();i++){
            cout<<students[i].id<<" - "<<students[i].name<<": ";

            if(i>=lastAllocation.size() || lastAllocation[i]==-1){
                cout<<"UNALLOCATED"<<endl;
            }
            else if(lastAllocation[i]<0 || lastAllocation[i]>=seats.size()){
                cout<<"INVALID SEAT INDEX"<<endl;
            }
            else{
                int seatIndex=lastAllocation[i];

                cout<<"Hall "<<seats[seatIndex].hallId<<", Row "<<seats[seatIndex].row <<", Column "<<seats[seatIndex].column<<endl;
            }
        }

        int allocated=countAllocated();

        cout<<"\nAllocated: "<<allocated<<" / "<<students.size()<<endl;
        cout<<"Unallocated: "<<students.size()-allocated<<endl;

        vector<string> conflicts=detectConflicts(students,seats,lastAllocation,halls);

        cout<<"\n \t\t\t\t CONFLICT CHECK "<<endl;

        if(conflicts.empty()) cout<<"No conflicts detected."<<endl;
        else{
            for(int i=0;i<conflicts.size();i++){
                cout<<"- "<<conflicts[i]<<endl;
            }
        }
    }

    // run greedy algorithm
    void runGreedy(){
        if(halls.empty() || students.empty()){
            cout<<"Please enter both hall and student details first."<<endl;
            return;
        }

        lastAllocation=allocateGreedy(students,seats);
        lastAlgorithm="Greedy";
        displayResults();
    }

    // run backtracking algorithm
    void runBacktracking(){
        if(halls.empty() || students.empty()){
            cout<<"Please enter both hall and student details first."<<endl;
            return;
        }

        lastAllocation=allocateBacktracking(students,seats);
        lastAlgorithm="Backtracking";
        displayResults();
    }

    // show the last allocation
    void showLastAllocation(){
        if(lastAllocation.empty()){
            cout<<"No previous allocation available. Run an algorithm first."<<endl;
            return;
        }

        cout<<"Last Algorithm Used: "<<lastAlgorithm<<endl;
        displayResults();
    }

    // check whether student and hall details are available
    bool hasData(){
        return !halls.empty() && !students.empty();
    }
};

int main(){
    ExamSeatingSystem system;
    int choice;

    do{
        cout<<"\n \t\t\t\t EXAM SEATING SYSTEM "<<endl;
        cout<<"1. Enter Hall Details"<<endl;
        cout<<"2. Enter Student Details"<<endl;
        cout<<"3. Display Data"<<endl;
        cout<<"4. Run Greedy Algorithm"<<endl;
        cout<<"5. Run Backtracking Algorithm"<<endl;
        cout<<"6. Show Last Allocation"<<endl;
        cout<<"7. Search Student by ID"<<endl;
        cout<<"8. Exit"<<endl;

        choice=system.readInt("Enter your choice: ");

        switch(choice){
            case 1:
                system.enterHalls();
                cout<<"Hall details saved successfully."<<endl;
                break;

            case 2:
                system.enterStudents();
                cout<<"Student details saved successfully."<<endl;
                break;

            case 3:
                if(system.hasData()==false){
                    cout<<"Please enter both hall and student details first."<<endl;
                    break;
                }

                system.displayData();
                break;

            case 4:
                system.runGreedy();
                break;

            case 5:
                system.runBacktracking();
                break;

            case 6:
                system.showLastAllocation();
                break;

            case 7:
                system.searchStudent();
                break;

            case 8:
                cout<<"Exiting program..."<<endl;
                break;

            default:
                cout<<"Invalid choice. Please try again."<<endl;
        }

    }while(choice!=8);

    return 0;
}
