#include <iostream>
#include <iomanip>
#include <limits>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;

class Employee {
public:
    string name;
    int emp_id;
    double hourlyRate;
    double hoursWorked;

    Employee(string n, int i, double rate, double hours) 
        : name(n), emp_id(i), hourlyRate(rate), hoursWorked(hours) {}

    double calculateSalary() const {
        return hourlyRate * min(hoursWorked, 40.0);
    }

    double calculateOvertimePay() const {
        return (hoursWorked > 40) ? (hoursWorked - 40) * (hourlyRate * 1.5) : 0.0;
    }

    double calculateGrossPay() const {
        return calculateSalary() + calculateOvertimePay();
    }

    double calculateNetPay() const {
        const double taxRate = 0.15;
        return calculateGrossPay() - (calculateGrossPay() * taxRate);
    }

    void display() const {
        cout << left << setw(10) << emp_id
             << setw(20) << name 
             << setw(12) << fixed << setprecision(2) << hourlyRate 
             << setw(12) << hoursWorked 
             << setw(12) << calculateSalary() 
             << setw(12) << calculateOvertimePay()
             << setw(12) << calculateGrossPay() 
             << setw(12) << calculateNetPay() << endl;
    }

    void saveToFile(ofstream& outFile) const {
        if (!outFile) {
            cerr << "Error writing to file!" << endl;
            return;
        }
        outFile << emp_id << "," << name << "," << hourlyRate << "," << hoursWorked << endl;
    }

    static bool loadFromFile(const string& line, Employee& emp) {
        stringstream ss(line);
        string idStr, name, rateStr, hoursStr;
        
        if (!getline(ss, idStr, ',') || !getline(ss, name, ',') || 
            !getline(ss, rateStr, ',') || !getline(ss, hoursStr)) {
            return false;
        }

        try {
            int id = stoi(idStr);
            double rate = stod(rateStr);
            double hours = stod(hoursStr);
            emp = Employee(name, id, rate, hours);
            return true;
        } catch (const exception& e) {
            cerr << "Error parsing employee data: " << e.what() << endl;
            return false;
        }
    }
};

void saveEmployeesToFile(const vector<Employee>& employees) {
    ofstream outFile("employees.txt");
    if (!outFile) {
        cerr << "Error opening file for writing!" << endl;
        return;
    }

    for (const auto& emp : employees) {
        emp.saveToFile(outFile);
    }

    outFile.close();
    cout << "Employee data saved successfully!" << endl;
}

void loadEmployeesFromFile(vector<Employee>& employees) {
    ifstream inFile("employees.txt");
    if (!inFile) {
        cerr << "Error opening file for reading or file does not exist." << endl;
        return;
    }

    string line;
    while (getline(inFile, line)) {
        Employee emp("", 0, 0, 0);
        if (Employee::loadFromFile(line, emp)) {
            employees.push_back(emp);
        }
    }
    sort(employees.begin(), employees.end(), [](const Employee& a, const Employee& b) {
        return a.emp_id < b.emp_id;
    });
}

template <typename T>
void getUserInput(const string& prompt, T& value, bool (*validate)(T) = nullptr) {
    while (true) {
        cout << prompt;
        if (cin >> value && (!validate || validate(value))) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }
        cerr << "Invalid input! Please try again." << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

bool validatePositive(double value) { return value >= 0; }
bool validatePositiveInt(int value) { return value >= 0; }

bool isEmployeeIdUnique(const vector<Employee>& employees, int emp_id) {
    return find_if(employees.begin(), employees.end(), [emp_id](const Employee& emp) {
        return emp.emp_id == emp_id;
    }) == employees.end();
}

void addEmployee(vector<Employee>& employees) {
    string name;
    int emp_id;
    double rate, hours;

    do {
        getUserInput("\nEnter Employee ID: ", emp_id, validatePositiveInt);
        if (!isEmployeeIdUnique(employees, emp_id)) {
            cerr << "Employee ID already exists! Please enter a unique ID." << endl;
        }
    } while (!isEmployeeIdUnique(employees, emp_id));

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Enter Employee Name: ";
    getline(cin, name);

    getUserInput("Enter Hourly Rate: ", rate, validatePositive);
    getUserInput("Enter Hours Worked: ", hours, validatePositive);

    employees.emplace_back(name, emp_id, rate, hours);
    sort(employees.begin(), employees.end(), [](const Employee& a, const Employee& b) {
        return a.emp_id < b.emp_id;
    });
    cout << "Employee added successfully!" << endl;
}

void removeEmployee(vector<Employee>& employees) {
    int emp_id;
    cout << "Enter Employee ID to remove: ";
    cin >> emp_id;

    auto it = remove_if(employees.begin(), employees.end(), [emp_id](const Employee& emp) {
        return emp.emp_id == emp_id;
    });

    if (it != employees.end()) {
        employees.erase(it, employees.end());         
        cout << "Employee with ID " << emp_id << " has been removed." << endl;
    } else {
        cerr << "Employee ID not found!" << endl;
    }
}

void displayEmployees(const vector<Employee>& employees) {
    if (employees.empty()) {
        cout << "No employees to display." << endl;
        return;
    }

    cout << "\nPayroll Details:\n";
    cout << left << setw(10) << "Emp_ID" 
         << setw(20) << "Name" 
         << setw(12) << "Rate" 
         << setw(12) << "Hours" 
         << setw(12) << "Salary" 
         << setw(12) << "Overtime" 
         << setw(12) << "Gross Pay" 
         << setw(12) << "Net Pay" << endl;
    cout << string(100, '-') << endl;

    for (const auto& emp : employees) {
        emp.display();
    }
}

void editRecord(vector<Employee>& employees) {
    int emp_id;
    cout << "Enter Employee ID to edit: ";
    cin >> emp_id;

    auto it = find_if(employees.begin(), employees.end(), [emp_id](const Employee& emp) {
        return emp.emp_id == emp_id;
    });

    if (it != employees.end()) {
        Employee& emp = *it;
        cout << "Editing Employee: " << emp.name << endl;

        string name;
        double rate, hours;

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter new Employee Name (leave blank to keep current): ";
        getline(cin, name);
        if (!name.empty()) {
            emp.name = name;
        }

        getUserInput("Enter new Hourly Rate: ", rate, validatePositive);
        emp.hourlyRate = rate;
        
        getUserInput("Enter new Hours Worked: ", hours, validatePositive);
        emp.hoursWorked = hours;

        cout << "Employee details updated successfully!" << endl;
    } else {
        cerr << "Employee ID not found!" << endl;
    }
}

int main() {
    vector<Employee> employees;
    loadEmployeesFromFile(employees);

    int choice;
    do {
        cout << "\nMenu:\n"
             << "1. Add Employee\n"
             << "2. Display Employees\n"
             << "3. Edit Employee\n"
             << "4. Remove Employee\n"
             << "5. Save Employees to File\n"
             << "6. Exit\n"
             << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: addEmployee(employees); break;
            case 2: displayEmployees(employees); break;
            case 3: editRecord(employees); break;
            case 4: removeEmployee(employees); break; 
            case 5: saveEmployeesToFile(employees); break;
            case 6: cout << "Exiting program. Goodbye!" << endl; break;
            default: cerr << "Invalid choice! Please try again." << endl;
        }
    } while (choice != 6);

    return 0;
}
