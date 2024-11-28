#include <iostream>
#include <sqlite3.h>
#include <string>
#include <vector>

// Base class for database operations
class DatabaseManager {
protected:
    sqlite3 *db;
    const std::string dbname = "company.db";

    void executeSQL(const std::string &sql) {
        char *errMsg = nullptr;
        int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            std::cerr << "SQL error: " << errMsg << std::endl;
            sqlite3_free(errMsg);
        }
    }

public:
    DatabaseManager() {
        if (sqlite3_open(dbname.c_str(), &db)) {
            std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
            exit(1);
        }
    }

    virtual ~DatabaseManager() {
        sqlite3_close(db);
    }
};

class EmployeeManager : public DatabaseManager {
public:
    EmployeeManager() {
        const std::string createTableSQL = 
            "CREATE TABLE IF NOT EXISTS Employee (" 
            "id INTEGER PRIMARY KEY, "
            "name TEXT NOT NULL, "
            "position TEXT NOT NULL);";
        executeSQL(createTableSQL);
    }

    void addEmployee(int id, const std::string &name, const std::string &position) {
        const std::string insertSQL = 
            "INSERT INTO Employee (id, name, position) VALUES (" + 
            std::to_string(id) + ", '" + name + "', '" + position + "');";
        executeSQL(insertSQL);
        std::cout << "Employee added successfully!\n";
    }

    void displayEmployees() {
        const std::string selectSQL = "SELECT * FROM Employee;";
        sqlite3_stmt *stmt;

        if (sqlite3_prepare_v2(db, selectSQL.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "Failed to fetch employees: " << sqlite3_errmsg(db) << std::endl;
            return;
        }

        std::cout << "ID\tName\t\tPosition\n";
        std::cout << "-----------------------------------\n";
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const char *name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
            const char *position = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));

            std::cout << id << "\t" << name << "\t\t" << position << "\n";
        }

        sqlite3_finalize(stmt);
    }
};

class ProjectManager : public DatabaseManager {
public:
    ProjectManager() {
        const std::string createTableSQL = 
            "CREATE TABLE IF NOT EXISTS Project (" 
            "id INTEGER PRIMARY KEY, "
            "name TEXT NOT NULL, "
            "deadline TEXT NOT NULL);";
        executeSQL(createTableSQL);
    }

    void addProject(int id, const std::string &name, const std::string &deadline) {
        const std::string insertSQL = 
            "INSERT INTO Project (id, name, deadline) VALUES (" + 
            std::to_string(id) + ", '" + name + "', '" + deadline + "');";
        executeSQL(insertSQL);
        std::cout << "Project added successfully!\n";
    }

    void displayProjects() {
        const std::string selectSQL = "SELECT * FROM Project;";
        sqlite3_stmt *stmt;

        if (sqlite3_prepare_v2(db, selectSQL.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "Failed to fetch projects: " << sqlite3_errmsg(db) << std::endl;
            return;
        }

        std::cout << "ID\tName\t\tDeadline\n";
        std::cout << "-----------------------------------\n";
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const char *name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
            const char *deadline = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));

            std::cout << id << "\t" << name << "\t\t" << deadline << "\n";
        }

        sqlite3_finalize(stmt);
    }
};

class AssignmentManager : public DatabaseManager {
public:
    AssignmentManager() {
        const std::string createTableSQL = 
            "CREATE TABLE IF NOT EXISTS Assignment (" 
            "employee_id INTEGER, "
            "project_id INTEGER, "
            "FOREIGN KEY(employee_id) REFERENCES Employee(id), "
            "FOREIGN KEY(project_id) REFERENCES Project(id));";
        executeSQL(createTableSQL);
    }

    void assignEmployeeToProject(int employee_id, int project_id) {
        const std::string insertSQL = 
            "INSERT INTO Assignment (employee_id, project_id) VALUES (" + 
            std::to_string(employee_id) + ", " + std::to_string(project_id) + ");";
        executeSQL(insertSQL);
        std::cout << "Employee assigned to project successfully!\n";
    }

    void displayAssignments() {
        const std::string selectSQL = 
            "SELECT Employee.name, Project.name FROM Assignment "
            "JOIN Employee ON Assignment.employee_id = Employee.id "
            "JOIN Project ON Assignment.project_id = Project.id;";
        sqlite3_stmt *stmt;

        if (sqlite3_prepare_v2(db, selectSQL.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "Failed to fetch assignments: " << sqlite3_errmsg(db) << std::endl;
            return;
        }

        std::cout << "Employee\t\tProject\n";
        std::cout << "-----------------------------------\n";
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const char *employeeName = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
            const char *projectName = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));

            std::cout << employeeName << "\t\t" << projectName << "\n";
        }

        sqlite3_finalize(stmt);
    }
};

int main() {
    EmployeeManager empManager;
    ProjectManager projManager;
    AssignmentManager assignManager;

    int choice;

    do {
        std::cout << "\nCompany Management System\n";
        std::cout << "1. Add Employee\n";
        std::cout << "2. Display Employees\n";
        std::cout << "3. Add Project\n";
        std::cout << "4. Display Projects\n";
        std::cout << "5. Assign Employee to Project\n";
        std::cout << "6. Display Assignments\n";
        std::cout << "7. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: {
                int id;
                std::string name, position;
                std::cout << "Enter Employee ID: ";
                std::cin >> id;
                std::cin.ignore();
                std::cout << "Enter Name: ";
                std::getline(std::cin, name);
                std::cout << "Enter Position: ";
                std::getline(std::cin, position);
                empManager.addEmployee(id, name, position);
                break;
            }
            case 2:
                empManager.displayEmployees();
                break;
            case 3: {
                int id;
                std::string name, deadline;
                std::cout << "Enter Project ID: ";
                std::cin >> id;
                std::cin.ignore();
                std::cout << "Enter Project Name: ";
                std::getline(std::cin, name);
                std::cout << "Enter Deadline: ";
                std::getline(std::cin, deadline);
                projManager.addProject(id, name, deadline);
                break;
            }
            case 4:
                projManager.displayProjects();
                break;
        }
    }
}
