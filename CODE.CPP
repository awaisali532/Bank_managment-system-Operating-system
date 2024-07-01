#include <iostream>
#include <cstdlib> // For system() and rand()
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <vector>
#include <ctime> // For time()
#include <queue>
#include <chrono> // For std::this_thread::sleep_for
#include <thread> // For std::this_thread::sleep_for
#include <algorithm> // For std::sort
#include <filesystem> // For directory and file operations

namespace fs = std::filesystem;
using namespace std;

// Structure to store customer details
struct Customer {
    string name;
    string accountNumber;
    double amount;
    int burstTime = 0;
    int arrivalTime = 0;
    bool isPriority;
    string transactionType;
    int tokenNumber;
    int priority;
};

// Function to clear the console screen
void clearConsole() {
#ifdef _WIN32
    system("cls"); // Clear console on Windows
#else
    system("clear"); // Clear console on Unix-based systems
#endif
}

// Function to display a header
void displayHeader(const string& title) {
    system("clear"); // Clear the console (Linux)
    cout << "\n";
    cout << "===========================================\n";
    cout <<  setw(15) << left << title << "\n";
    cout << "===========================================\n";
    cout << "\n\n";
}

// Function to simulate time delay
void simulateProcessing(int seconds) {
    for (int i = 0; i < seconds; ++i) {
        cout << ".";
        cout.flush();
        this_thread::sleep_for(chrono::seconds(1)); // Simulate time delay
    }
}

// Function to create a new account
void createAccount(const string& accountNumber, const string& accountHolderName, const string& accountType) {
    displayHeader("Creating New Account");
    string command = "mkdir HBL" + accountNumber;
    int result = system(command.c_str());
    if (result != 0) {
        cerr << "Failed to create account " << accountNumber << endl;
    } else {
        cout << "Account " << accountNumber << " created successfully." << endl;

        // Create a file for storing account details
        ofstream file("HBL" + accountNumber + "/details.txt");
        if (file.is_open()) {
            file << "Account Number: " << accountNumber << "\n";
            file << "Account Holder Name: " << accountHolderName << "\n";
            file << "Account Type: " << accountType << "\n";
            file << "Balance: 0.00\n"; // Initial balance set to 0.00
            file.close();
            cout << "Account details saved successfully." << endl;
        } else {
            cerr << "Error creating file for account " << accountNumber << endl;
        }
    }
}

// Function to delete an account
void deleteAccount(const string& accountNumber) {
    displayHeader("Deleting Account");

    string accountDir = "HBL" + accountNumber;

    // Check if the account directory exists
    if (fs::exists(accountDir)) {
        try {
            fs::remove_all(accountDir); // Remove directory and its contents
            cout << "Account " << accountNumber << " deleted successfully." << endl;
        } catch (const fs::filesystem_error& e) {
            cerr << "Error deleting account: " << e.what() << endl;
        }
    } else {
        cerr << "Account " << accountNumber << " does not exist." << endl;
    }
}

// Function to view account details
void viewAccountDetails(const string& accountNumber) {
    displayHeader("Viewing Account Details");

    string filename = "HBL" + accountNumber + "/details.txt";
    ifstream file(filename);

    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            cout << line << endl;
        }
        file.close();
    } else {
        cerr << "Error: Could not open file for account " << accountNumber << endl;
    }
}

// Function to update balance in file
bool updateBalance(const string& accountNumber, double amount, bool isDeposit) {
    string filename = "HBL" + accountNumber + "/details.txt";
    ifstream inFile(filename); // Read mode
    ofstream outFile("temp.txt"); // Temporary file for writing updated content
    stringstream content;
    string line;
    string balanceStr;
    double currentBalance = 0.0;
    bool success = false;

    // Read current balance and other details
    if (inFile.is_open()) {
        while (getline(inFile, line)) {
            if (line.find("Balance: ") != string::npos) {
                balanceStr = line.substr(line.find(": ") + 2);
                currentBalance = stod(balanceStr);
                break;
            } else {
                outFile << line << "\n"; // Copy other details as is
            }
        }
        inFile.close();

        // Check if balance was found
        if (balanceStr.empty()) {
            cerr << "Error: Balance not found in file " << filename << endl;
            return false;
        }
    } else {
        cerr << "Error reading file for account " << accountNumber << endl;
        return false;
    }

    // Update balance based on transaction type
    if (isDeposit) {
        currentBalance += amount;
    } else {
        if (currentBalance >= amount) {
            currentBalance -= amount;
        } else {
            cerr << "Insufficient balance. Transaction not completed." << endl;
            return false;
        }
    }

    // Write updated balance back to file
    outFile << "Balance: " << fixed << setprecision(2) << currentBalance << "\n";
    outFile.close();

    // Replace original file with temporary file
    if (remove(filename.c_str()) == 0) {
        if (rename("temp.txt", filename.c_str()) != 0) {
            cerr << "Error renaming file" << endl;
        } else {
            cout << (isDeposit ? "Deposit" : "Withdrawal") << " of $" << amount << " completed for account " << accountNumber << endl;
            cout << "Current Balance: $" << currentBalance << endl;
            success = true;
        }
    } else {
        cerr << "Error deleting file" << endl;
    }

    return success;
}

// Function to check if account exists
bool doesAccountExist(const string& accountNumber) {
    string filename = "HBL" + accountNumber + "/details.txt";
    ifstream file(filename);
    return file.good();
}

// Function to implement the Token System with selected algorithm
void TokenSystem(vector<Customer>& customers, const string& algorithm) {
    displayHeader("Executing Token System");

    // Sort customers based on arrival time
    sort(customers.begin(), customers.end(), [](const Customer& a, const Customer& b) {
        return a.arrivalTime < b.arrivalTime;
    });

    if (algorithm == "Priority") {
        // Sort customers based on priority
        sort(customers.begin(), customers.end(), [](const Customer& a, const Customer& b) {
            return a.priority < b.priority;
        });
    } else if (algorithm == "SJF") {
        // Sort customers based on burst time (Shortest Job First)
        sort(customers.begin(), customers.end(), [](const Customer& a, const Customer& b) {
            return a.burstTime < b.burstTime;
        });
    } else if (algorithm == "RR") {
        // Implement Round Robin (For simplicity, assuming a quantum of 2 units)
        int quantum = 20;
        queue<Customer> rrQueue;
        for (auto& customer : customers) {
            rrQueue.push(customer);
        }

        int time = 0;
        vector<Customer> tempCustomers;

        while (!rrQueue.empty()) {
            Customer customer = rrQueue.front();
            rrQueue.pop();

            if (customer.burstTime > quantum) {
                time += quantum;
                customer.burstTime -= quantum;
                rrQueue.push(customer);
            } else {
                time += customer.burstTime;
                customer.burstTime = 0;
                customer.arrivalTime = time; // Updating arrival time to reflect completion
            }

            tempCustomers.push_back(customer);
        }

        customers = tempCustomers;
    }

    int time = 0;
    int totalTurnaroundTime = 0;
    int totalWaitingTime = 0;
    vector<string> gantChart;

    cout << "\tToken"
         << "\tName" 
         << "\tAccount Number" 
         << "\t\t\t\tAmount" 
         << "\tBT" 
         << "\tAT" 
         << "\tCT" 
         << "\tTAT" 
         << "\tWT" 
         << "\tP" 
         << "\tTT" << endl;

    cout << "---------------------------------------------------------------------------------------------------------------\n";

    for (auto& customer : customers) {
        int completionTime = (time + customer.burstTime) * rand()%10;
        int turnaroundTime = completionTime - customer.arrivalTime;
        int waitingTime = turnaroundTime - customer.burstTime;
        totalTurnaroundTime += turnaroundTime;
        totalWaitingTime += waitingTime;

        gantChart.push_back(to_string(customer.tokenNumber));

        cout << "\t" << customer.tokenNumber 
             << "\t" << customer.name 
             << "\t" << customer.accountNumber 
             << "\t\t\t\t" << customer.amount 
             << "\t" << customer.burstTime 
             << "\t" << customer.arrivalTime 
             << "\t" << completionTime 
             << "\t" << turnaroundTime 
             << "\t" << waitingTime 
             << "\t" << customer.priority 
             << "\t" << customer.transactionType << endl;

        time = completionTime;
    }

    double avgTurnaroundTime = static_cast<double>(totalTurnaroundTime) / customers.size();
    double avgWaitingTime = static_cast<double>(totalWaitingTime) / customers.size();

    cout << "\nAverage Turnaround Time: " << avgTurnaroundTime << endl;
    cout << "Average Waiting Time: " << avgWaitingTime << endl;

    // Display Gantt Chart
    cout << "\nGantt Chart: ";
    for (const auto& token : gantChart) {
        cout << "[" << token << "] ";
    }
    cout << endl;

    cout << "\nToken System executed successfully using " << algorithm << " algorithm." << endl;
}

int main() {
    srand(static_cast<unsigned>(time(0))); // Initialize random seed

    vector<Customer> customers;
    int customerCount = 0;
    int totalBurstTime = 0;
    int totalArrivalTime = 0;
    string algorithm;

    while (true) {
        clearConsole();
        displayHeader("Bank Management System");
        cout << "1. Create Account\n";
        cout << "2. Delete Account\n";
        cout << "3. View Account Details\n";
        cout << "4. Deposit Money\n";
        cout << "5. Withdraw Money\n";
        cout << "6. Token System\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;

        if (choice == 0) {
            cout << "Exiting program..." << endl;
            break;
        }

        string accountNumber;
        string accountHolderName;
        string accountType;
        double amount;
        bool isPriority;
        string transactionType;

        switch (choice) {
            case 1:
                clearConsole();
                displayHeader("Create Account");
                cout << "Enter account number: ";
                cin >> accountNumber;
                cout << "Enter account holder name: ";
                cin >> accountHolderName;
                cout << "Enter account type: ";
                cin >> accountType;
                createAccount(accountNumber, accountHolderName, accountType);
                break;

            case 2:
                clearConsole();
                displayHeader("Delete Account");
                cout << "Enter account number: ";
                cin >> accountNumber;
                deleteAccount(accountNumber);
                break;

            case 3:
                clearConsole();
                displayHeader("View Account Details");
                cout << "Enter account number: ";
                cin >> accountNumber;
                viewAccountDetails(accountNumber);
                break;

            case 4:
                clearConsole();
                displayHeader("Deposit Money");
                cout << "Enter account number: ";
                cin >> accountNumber;
                if (doesAccountExist(accountNumber)) {
                    cout << "Enter amount to deposit: ";
                    cin >> amount;
                    updateBalance(accountNumber, amount, true);
                } else {
                    cerr << "Error: Account does not exist." << endl;
                }
                break;

            case 5:
                clearConsole();
                displayHeader("Withdraw Money");
                cout << "Enter account number: ";
                cin >> accountNumber;
                if (doesAccountExist(accountNumber)) {
                    cout << "Enter amount to withdraw: ";
                    cin >> amount;
                    updateBalance(accountNumber, amount, false);
                } else {
                    cerr << "Error: Account does not exist." << endl;
                }
                break;

            case 6:
                clearConsole();
                displayHeader("Token System");

                cout << "Enter the number of customers: ";
                cin >> customerCount;

                for (int i = 0; i < customerCount; ++i) {
                    Customer customer;

                    cout << "Enter the details of customer no "<< i + 1<<":\n";
                    cout << "Enter account number: ";
                    cin >> customer.accountNumber;
                    cout << "Enter amount: ";
                    cin >> customer.amount;
                    cout << "Enter priority (1 for high, 2 for medium, 3 for low): ";
                    cin >> customer.priority;
                    cout << "Enter transaction type (deposit/withdraw): ";
                    cin >> customer.transactionType;

                    customer.tokenNumber = i + 1; // Assign token number
                    customer.burstTime += 2;
                    customer.arrivalTime += 1;
                    totalBurstTime += customer.burstTime;
                    totalArrivalTime += customer.arrivalTime;
                    customers.push_back(customer);
                }

                cout << "Select scheduling algorithm (FCFS/Priority/SJF/RR): ";
                cin >> algorithm;

                TokenSystem(customers, algorithm);
                break;

            default:
                cerr << "Invalid choice. Please try again." << endl;
        }

        cout << "\nPress Enter to continue...";
        cin.ignore();
        cin.get();
    }

    return 0;
}
