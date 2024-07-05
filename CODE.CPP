#include <iostream>
#include <cstdlib> // For system() and rand()
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <vector>
#include <ctime> // For time()
#include <queue>
#include <chrono>     
#include <thread>     
#include <algorithm> 
#include <filesystem> // For directory and file operations
#include <climits>    // For INT_MAX
#include <numeric>

namespace fs = std::filesystem;
using namespace std;

// Structure to store customer details
struct Customer
{
    string name;
    string accountNumber;
    double amount;
    int burstTime = 0;
    int arrivalTime = 0;
    bool isPriority;
    string transactionType;
    int tokenNumber;
    int priority;
    string transactionStatus; // Added to track transaction status
};

// Function to clear the console screen
void clearConsole()
{
#ifdef _WIN32
    system("cls"); // Clear console on Windows
#else
    system("clear"); // Clear console on Unix-based systems
#endif
}

// Function to display a header
void displayHeader(const string &title)
{
    system("clear"); // Clear the console (Linux)
    cout << "\n";
    cout << "===========================================\n";
    cout << setw(15) << left << title << "\n";
    cout << "===========================================\n";
    cout << "\n\n";
    
}

// Function to simulate time delay
void simulateProcessing(int seconds)
{
    for (int i = 0; i < seconds; ++i)
    {
        cout << ".";
        cout.flush();
        this_thread::sleep_for(chrono::seconds(1)); // Simulate time delay
    }
}

// Function to create a new account
void createAccount(const string &accountNumber, const string &accountHolderName, const string &accountType)
{
    displayHeader("Creating New Account");
    string command = "mkdir HBL" + accountNumber;
    int result = system(command.c_str());
    if (result != 0)
    {
        cout << "Failed to create account " << accountNumber << endl;
    }
    else
    {
        cout << "Account " << accountNumber << " created successfully." << endl;

        // Create a file for storing account details
        ofstream file("HBL" + accountNumber + "/details.txt");
        if (file.is_open())
        {
            file << "Account Number: " << accountNumber << "\n";
            file << "Account Holder Name: " << accountHolderName << "\n";
            file << "Account Type: " << accountType << "\n";
            file << "Balance: 0.00\n"; // Initial balance set to 0.00
            file.close();
            cout << "Account details saved successfully." << endl;
        }
        else
        {
            cerr << "Error creating file for account " << accountNumber << endl;
        }
    }
}

// Function to delete an account
void deleteAccount(const string &accountNumber)
{
    displayHeader("Deleting Account");

    string accountDir = "HBL" + accountNumber;

    // Check if the account directory exists
    if (fs::exists(accountDir))
    {
        try
        {
            fs::remove_all(accountDir); // Remove directory and its contents
            cout << "Account " << accountNumber << " deleted successfully." << endl;
        }
        catch (const fs::filesystem_error &e)
        {
            cerr << "Error deleting account: " << e.what() << endl;
        }
    }
    else
    {
        cerr << "Account " << accountNumber << " does not exist." << endl;
    }
}

// Function to view account details
void viewAccountDetails(const string &accountNumber)
{
    displayHeader("Viewing Account Details");

    string filename = "HBL" + accountNumber + "/details.txt";
    ifstream file(filename);

    if (file.is_open())
    {
        string line;
        while (getline(file, line))
        {
            cout << line << endl;
        }
        file.close();
    }
    else
    {
        cerr << "Error: Could not open file for account " << accountNumber << endl;
    }
}

// Function to update balance in file
bool updateBalance(const string &accountNumber, double amount, bool isDeposit)
{
    string filename = "HBL" + accountNumber + "/details.txt";
    ifstream inFile(filename);    // Read mode
    ofstream outFile("temp.txt"); // Temporary file for writing updated content
    stringstream content;
    string line;
    string balanceStr;
    double currentBalance = 0.0;
    bool success = false;
    cout << endl
         << endl;
    // Read current balance and other details
    if (inFile.is_open())
    {
        while (getline(inFile, line))
        {
            if (line.find("Balance: ") != string::npos)
            {
                balanceStr = line.substr(line.find(": ") + 2);
                currentBalance = stod(balanceStr);
                break;
            }
            else
            {
                outFile << line << "\n"; // Copy other details as is
            }
        }
        inFile.close();

        // Check if balance was found
        if (balanceStr.empty())
        {
            cerr << "Error: Balance not found in file " << filename << endl;
            return false;
        }
    }
    else
    {
        cerr << "Error reading file for account " << accountNumber << endl;
        return false;
    }

    // Update balance based on transaction type
    if (isDeposit)
    {
        currentBalance += amount;
    }
    else
    {
        if (currentBalance >= amount)
        {
            currentBalance -= amount;
        }
        else
        {
            cerr << "Insufficient balance. Transaction not completed." << endl;
            return false;
        }
    }

    // Write updated balance back to file
    outFile << "Balance: " << fixed << setprecision(2) << currentBalance << "\n";
    outFile.close();

    // Replace original file with temporary file
    if (remove(filename.c_str()) == 0)
    {
        if (rename("temp.txt", filename.c_str()) != 0)
        {
            cerr << "Error renaming file" << endl;
        }
        else
        {
            cout << (isDeposit ? "Deposit" : "Withdrawal") << " of $" << amount << " completed for account " << accountNumber << endl;
            cout << "Current Balance: $" << currentBalance << endl;
            success = true;
        }
    }
    else
    {
        cerr << "Error deleting file" << endl;
    }

    return success;
}

// Function to check if account exists
bool doesAccountExist(const string &accountNumber)
{
    string filename = "HBL" + accountNumber + "/details.txt";
    ifstream file(filename);
    return file.good();
}

// Function to implement the Token System with selected algorithm
void TokenSystem(vector<Customer> &customers, const string &algorithm, const vector<int> &arrivalTimes, const vector<int> &burstTimes)
{
    cin.ignore();
    displayHeader("Executing Token System");

    // Assign arrival time and burst time from passed vectors
    for (size_t i = 0; i < customers.size(); ++i)
    {
        customers[i].arrivalTime = arrivalTimes[i];
        customers[i].burstTime = burstTimes[i];
    }

    // Sort customers based on arrival time
    sort(customers.begin(), customers.end(), [](const Customer &a, const Customer &b)
         { return a.arrivalTime < b.arrivalTime; });

    if (algorithm == "1")
    {
        // FCFS Implementation
        int n = customers.size();
        int aw = 0, at = 0;
        vector<int> w(n), t(n);

        w[0] = 0; // First customer has no waiting time

        for (int i = 1; i < n; ++i)
        {
            w[i] = (customers[i - 1].arrivalTime + customers[i - 1].burstTime + w[i - 1]) - customers[i].arrivalTime;
            if (w[i] < 0)
            {
                w[i] = 0; // Waiting time can't be negative
            }
        }

        for (int i = 0; i < n; ++i)
        {
            t[i] = customers[i].burstTime + w[i];
            aw += w[i];
            at += t[i];
        }
        // Perform transaction (deposit/withdrawal)
        for (int i = 0; i < n; ++i)
        {
            bool transactionSuccess = false;
            if (customers[i].transactionType == "Deposit")
            {
                transactionSuccess = updateBalance(customers[i].accountNumber, customers[i].amount, true);
            }
            else if (customers[i].transactionType == "Withdraw")
            {
                transactionSuccess = updateBalance(customers[i].accountNumber, customers[i].amount, false);
            }

            // Update transaction status in the table
            if (transactionSuccess)
            {
                customers[i].transactionStatus = "Success";
            }
            else
            {
                customers[i].transactionStatus = "Failed";
            }
        }

        cout << "\n\tToken Number\tArrival Time\tBurst Time\tWaiting Time\tTurnaround Time\tTransaction Status" << endl;
        for (int i = 0; i < n; ++i)
        {
            cout << "\t" << customers[i].tokenNumber << "\t\t" << customers[i].arrivalTime << "\t\t" << customers[i].burstTime
                 << "\t\t" << w[i] << "\t\t" << t[i] << "\t\t";

            cout << customers[i].transactionStatus << endl;
        }

        cout << "The average turnaround time = " << (float)at / n << endl;
        cout << "The average waiting time = " << (float)aw / n << endl;
    }
    else if (algorithm == "2")
    {
        // SJF Implementation
        int n = customers.size();
        int completed = 0, currentTime = 0;
        vector<int> waitingTime(n, 0), turnaroundTime(n, 0), remainingBurstTime(n);

        // Initialize remaining burst times
        for (int i = 0; i < n; ++i)
        {
            remainingBurstTime[i] = customers[i].burstTime;
        }

        while (completed != n)
        {
            int shortest = -1;
            int minBurst = INT_MAX;

            // Find the customer with the shortest remaining burst time
            for (int i = 0; i < n; ++i)
            {
                if (customers[i].arrivalTime <= currentTime && remainingBurstTime[i] < minBurst && remainingBurstTime[i] > 0)
                {
                    minBurst = remainingBurstTime[i];
                    shortest = i;
                }
            }

            if (shortest == -1)
            {
                currentTime++;
                continue;
            }

            remainingBurstTime[shortest]--;
            currentTime++;

            if (remainingBurstTime[shortest] == 0)
            {
                completed++;
                int finishTime = currentTime;
                waitingTime[shortest] = finishTime - customers[shortest].arrivalTime;

                if (waitingTime[shortest] < 0)
                {
                    waitingTime[shortest] = 0;
                }

                turnaroundTime[shortest] = finishTime - customers[shortest].arrivalTime;

                // Perform transaction (deposit/withdrawal)
                bool transactionSuccess = false;
                if (customers[shortest].transactionType == "Deposit")
                {
                    transactionSuccess = updateBalance(customers[shortest].accountNumber, customers[shortest].amount, true);
                }
                else if (customers[shortest].transactionType == "Withdraw")
                {
                    transactionSuccess = updateBalance(customers[shortest].accountNumber, customers[shortest].amount, false);
                }

                // Update transaction status in the table
                if (transactionSuccess)
                {
                    customers[shortest].transactionStatus = "Success";
                }
                else
                {
                    customers[shortest].transactionStatus = "Failed";
                }
            }
        }

        cout << "\n\tToken Number\tArrival Time\tBurst Time\tWaiting Time\tTurnaround Time\tTransaction Status" << endl;
        for (int i = 0; i < n; ++i)
        {
            cout << "\t" << customers[i].tokenNumber << "\t\t" << customers[i].arrivalTime << "\t\t" << customers[i].burstTime
                 << "\t\t" << waitingTime[i] << "\t\t" << turnaroundTime[i] << "\t\t" << customers[i].transactionStatus << endl;
        }

        cout << "The average turnaround time = " << accumulate(turnaroundTime.begin(), turnaroundTime.end(), 0.0) / n << endl;
        cout << "The average waiting time = " << accumulate(waitingTime.begin(), waitingTime.end(), 0.0) / n << endl;
    }
    else if (algorithm == "3")
    {
        // Priority Scheduling Implementation
        int n = customers.size();
        vector<int> waitingTime(n, 0), turnaroundTime(n, 0);

        sort(customers.begin(), customers.end(), [](const Customer &a, const Customer &b)
             {
                 return a.priority < b.priority; // Sort by priority (lower number means higher priority)
             });

        waitingTime[0] = 0; // First customer has no waiting time

        for (int i = 1; i < n; ++i)
        {
            waitingTime[i] = customers[i - 1].burstTime + waitingTime[i - 1];
        }

        for (int i = 0; i < n; ++i)
        {
            turnaroundTime[i] = customers[i].burstTime + waitingTime[i];
        }

        for (int i = 0; i < n; ++i)
        {
            bool transactionSuccess = false;
            if (customers[i].transactionType == "Deposit")
            {
                transactionSuccess = updateBalance(customers[i].accountNumber, customers[i].amount, true);
            }
            else if (customers[i].transactionType == "Withdraw")
            {
                transactionSuccess = updateBalance(customers[i].accountNumber, customers[i].amount, false);
            }
            if (transactionSuccess)
            {
                customers[i].transactionStatus = "Success";
            }
            else
            {
                customers[i].transactionStatus = "Failed";
            }

        }
     
        cout << "\n\tToken Number\tPriority\tBurst Time\tWaiting Time\tTurnaround Time\tTransaction Status" << endl;
        for (int i = 0; i < n; ++i)
        {
            cout << "\t" << customers[i].tokenNumber << "\t\t" << customers[i].priority << "\t\t" << customers[i].burstTime
                 << "\t\t" << waitingTime[i] << "\t\t" << turnaroundTime[i] << "\t\t";


            cout << customers[i].transactionStatus << endl;
        }

        cout << "The average turnaround time = " << accumulate(turnaroundTime.begin(), turnaroundTime.end(), 0.0) / n << endl;
        cout << "The average waiting time = " << accumulate(waitingTime.begin(), waitingTime.end(), 0.0) / n << endl;
    }
    else if (algorithm == "4") {
    // Round Robin Implementation
    int n = customers.size();
    int completed = 0, currentTime = 0;
    vector<int> waitingTime(n, 0), turnaroundTime(n, 0), remainingBurstTime(n);
    int quantum = 2; // Set time quantum

    // Initialize remaining burst times
    for (int i = 0; i < n; ++i) {
        remainingBurstTime[i] = customers[i].burstTime;
    }

    while (completed != n) {
        bool isAllZero = true;
        for (int i = 0; i < n; ++i) {
            if (remainingBurstTime[i] > 0 && customers[i].arrivalTime <= currentTime) {
                isAllZero = false;
                int timeSlice = min(quantum, remainingBurstTime[i]);
                currentTime += timeSlice;
                remainingBurstTime[i] -= timeSlice;
                if (remainingBurstTime[i] == 0) {
                    completed++;
                    int finishTime = currentTime;
                    waitingTime[i] = finishTime - customers[i].burstTime - customers[i].arrivalTime;

                    if (waitingTime[i] < 0) {
                        waitingTime[i] = 0;
                    }

                    turnaroundTime[i] = customers[i].burstTime + waitingTime[i];
                }
            }
        }
        if (isAllZero && completed != n) {
            currentTime++;
        }
    }
     for (int i = 0; i < n; ++i)
        {
            bool transactionSuccess = false;
            if (customers[i].transactionType == "Deposit")
            {
                transactionSuccess = updateBalance(customers[i].accountNumber, customers[i].amount, true);
            }
            else if (customers[i].transactionType == "Withdraw")
            {
                transactionSuccess = updateBalance(customers[i].accountNumber, customers[i].amount, false);
            }
            if (transactionSuccess)
            {
                customers[i].transactionStatus = "Success";
            }
            else
            {
                customers[i].transactionStatus = "Failed";
            }

        }

    cout << "\n\tToken Number\tArrival Time\tBurst Time\tWaiting Time\tTurnaround Time\tTransaction Status" << endl;
    for (int i = 0; i < n; ++i) {
        cout << "\t" << customers[i].tokenNumber << "\t\t" << customers[i].arrivalTime << "\t\t" << customers[i].burstTime
             << "\t\t" << waitingTime[i] << "\t\t" << turnaroundTime[i] << "\t\t" << customers[i].transactionStatus << endl;
    }

    cout << "The average turnaround time = " << accumulate(turnaroundTime.begin(), turnaroundTime.end(), 0.0) / n << endl;
    cout << "The average waiting time = " << accumulate(waitingTime.begin(), waitingTime.end(), 0.0) / n << endl;

    // Gantt Chart
    cout << "\nGantt Chart:\n";
    for (int i = 0; i < n; ++i) {
        if (waitingTime[i] > 0 && turnaroundTime[i] > 0) {
            int startTime = customers[i].arrivalTime;
            int endTime = startTime + waitingTime[i] + customers[i].burstTime;
            cout << "[" << startTime << ", " << endTime << "] ";
        }
    }
    cout << endl;
}

    else
    {
        cout << "Invalid algorithm selected. Please choose FCFS, SJF, RR or Priority Scheduling." << endl;
    }
}

// Main function
int main()
{
    int choice;
    vector<Customer> customers;

    do
    {
        displayHeader("HBL Banking Management System");
        cout << "1. Create New Account\n";
        cout << "2. Delete Account\n";
        cout << "3. View Account Details\n";
        cout << "4. Perform Transaction\n";
        cout << "5. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
        {
            string accountNumber, accountHolderName, accountType;
            cout << "Enter account number: ";
            cin >> accountNumber;
            cin.ignore(); // Consume newline character
            cout << "Enter account holder name: ";
            getline(cin, accountHolderName);
            cout << "Enter account type: ";
            getline(cin, accountType);
            createAccount(accountNumber, accountHolderName, accountType);
            break;
        }
        case 2:
        {
            string accountNumber;
            cout << "Enter account number: ";
            cin >> accountNumber;
            cin.ignore(); // Consume newline character
            deleteAccount(accountNumber);
            break;
        }
        case 3:
        {
            string accountNumber;
            cout << "Enter account number: ";
            cin >> accountNumber;
            cin.ignore(); // Consume newline character
            viewAccountDetails(accountNumber);
            break;
        }
        case 4:
        {
            int numCustomers;
            cout << "Enter the number of customers: ";
            cin >> numCustomers;
            customers.resize(numCustomers);

            vector<int> arrivalTimes(numCustomers);
            vector<int> burstTimes(numCustomers);

            // Input customer details
            for (int i = 0; i < numCustomers; ++i)
            {
                cout<<endl<< " ============================================ "<<endl;
                cout << "Customer " << i + 1 << " details:" << endl;
                cout << "Enter name: ";
                cin.ignore();
                getline(cin, customers[i].name);
                cout << "Enter account number: ";
                getline(cin, customers[i].accountNumber);
                cout << "Enter transaction type (Deposit/Withdraw): ";
                getline(cin, customers[i].transactionType);
                cout << "Enter amount: ";
                cin >> customers[i].amount;
                cout << "Is it a priority transaction? (1 for yes, 0 for no): ";
                cin >> customers[i].isPriority;
                if (customers[i].isPriority)
                {
                    cout << "Enter priority level (lower number means higher priority): ";
                    cin >> customers[i].priority;
                }
                cout << "Enter arrival time: ";
                cin >> arrivalTimes[i];
                cout << "Enter burst time: ";
                cin >> burstTimes[i];
                customers[i].tokenNumber = i + 1; // Assign token number sequentially
                customers[i].transactionStatus = "Pending";
            }

            string algorithm;
            cout << "Select scheduling algorithm (1. FCFS, 2. SJF, 3. Priority Scheduling 4. Round Robin): ";
            cin >> algorithm;
            
            // Perform transactions for all customers
            TokenSystem(customers, algorithm, arrivalTimes, burstTimes);

            break;
        }
        case 5:
            cout << "Exiting the system. Have a nice day!" << endl;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
            break;
        }
        cin.ignore();
    } while (choice != 5);

    return 0;
}
