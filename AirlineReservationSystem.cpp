#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <limits> // For input validation
using namespace std;

// Color codes
const string RED = "\033[31m";
const string GREEN = "\033[32m";
const string YELLOW = "\033[33m";
const string BLUE = "\033[34m";
const string MAGENTA = "\033[35m";
const string CYAN = "\033[36m";
const string BOLD = "\033[1m";
const string RESET = "\033[0m";

// Function to print colored text
void printColored(const string &text, const string &colorCode)
{
    cout << colorCode << text << RESET;
}

// Helper function to safely read an integer with validation
int getInt(const string &prompt, int minVal = numeric_limits<int>::min(), int maxVal = numeric_limits<int>::max())
{
    int value;
    while (true)
    {
        cout << prompt;
        cin >> value;
        if (cin.fail() || value < minVal || value > maxVal)
        {
            printColored("Invalid input. Please enter a valid number.\n", RED);
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        else
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // clear rest of line
            return value;
        }
    }
}

// Helper function to safely read a double with validation
double getDouble(const string &prompt, double minVal = numeric_limits<double>::lowest(), double maxVal = numeric_limits<double>::max())
{
    double value;
    while (true)
    {
        cout << prompt;
        cin >> value;
        if (cin.fail() || value < minVal || value > maxVal)
        {
            printColored("Invalid input. Please enter a valid decimal number.\n", RED);
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        else
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
    }
}

// Flight class
class Flight
{
public:
    string flightNumber;
    string origin;
    string destination;
    string date;      // YYYY-MM-DD
    string time;      // HH:MM
    double price;
    int totalSeats;

    Flight() {}
    Flight(string fn, string org, string dest, string d, string t, double p, int seats)
        : flightNumber(fn), origin(org), destination(dest), date(d), time(t), price(p), totalSeats(seats) {}

    void display() const
    {
        cout << left
             << setw(15) << flightNumber
             << setw(20) << origin
             << setw(20) << destination
             << setw(15) << date
             << setw(10) << time
             << setw(10) << fixed << setprecision(2) << price
             << setw(10) << totalSeats
             << "\n";
    }

    static void printHeader()
    {
        cout << BOLD << CYAN;
        cout << left
             << setw(15) << "Flight No"
             << setw(20) << "Origin"
             << setw(20) << "Destination"
             << setw(15) << "Date"
             << setw(10) << "Time"
             << setw(10) << "Price"
             << setw(10) << "Seats"
             << RESET << "\n";
        cout << string(90, '=') << "\n";
    }

    string toCSV() const
    {
        ostringstream oss;
        oss << flightNumber << "," << origin << "," << destination << "," << date << ","
            << time << "," << price << "," << totalSeats;
        return oss.str();
    }

    static Flight fromCSV(const string &line)
    {
        istringstream iss(line);
        string token;
        vector<string> tokens;
        while (getline(iss, token, ','))
            tokens.push_back(token);
        if (tokens.size() != 7)
            return Flight();
        return Flight(tokens[0], tokens[1], tokens[2], tokens[3], tokens[4],
                      stod(tokens[5]), stoi(tokens[6]));
    }
};

// Booking class
class Booking
{
public:
    string bookingID;
    string passengerUsername;
    string flightNumber;
    int seatNumber;
    bool cancelled;

    Booking() : seatNumber(0), cancelled(false) {}
    Booking(string bID, string pUser, string fNum, int seat)
        : bookingID(bID), passengerUsername(pUser), flightNumber(fNum), seatNumber(seat), cancelled(false) {}

    string toCSV() const
    {
        return bookingID + "," + passengerUsername + "," + flightNumber + "," + to_string(seatNumber) + "," + (cancelled ? "1" : "0");
    }

    static Booking fromCSV(const string &line)
    {
        istringstream iss(line);
        vector<string> tokens;
        string token;
        while (getline(iss, token, ','))
            tokens.push_back(token);
        if (tokens.size() != 5)
            return Booking();
        Booking b;
        b.bookingID = tokens[0];
        b.passengerUsername = tokens[1];
        b.flightNumber = tokens[2];
        b.seatNumber = stoi(tokens[3]);
        b.cancelled = (tokens[4] == "1");
        return b;
    }

    void display() const
    {
        cout << left
             << setw(25) << bookingID
             << setw(20) << flightNumber
             << setw(8) << seatNumber;
        if (cancelled)
            printColored(string(10, ' ') + "Cancelled\n", RED);
        else
            printColored(string(10, ' ') + "Active\n", GREEN);
    }

    static void printHeader()
    {
        cout << BOLD << MAGENTA;
        cout << left
             << setw(25) << "Booking ID"
             << setw(20) << "Flight No"
             << setw(8) << "Seat"
             << setw(10) << "Status"
             << RESET << "\n";
        cout << string(70, '=') << "\n";
    }
};

// User classes
class User
{
protected:
    string username;
    string password;

public:
    User() {}
    User(string u, string p) : username(u), password(p) {}
    virtual bool login(const string &u, const string &p) = 0;
    string getUsername() const { return username; }
    virtual ~User() {}
    string getPassword() const { return password; }
};

class Passenger : public User
{
public:
    Passenger() {}
    Passenger(string u, string p) : User(u, p) {}
    bool login(const string &u, const string &p) override
    {
        return (u == username && p == password);
    }
};

class Admin : public User
{
public:
    Admin() {}
    Admin(string u, string p) : User(u, p) {}
    bool login(const string &u, const string &p) override
    {
        return (u == username && p == password);
    }
};

// AirlineSystem class
class AirlineSystem
{
    vector<Passenger> passengers;
    vector<Admin> admins;
    vector<Flight> flights;
    vector<Booking> bookings;

    const string adminsFile = "admins.txt";
    const string passengersFile = "passengers.txt";
    const string flightsFile = "flights.txt";
    const string bookingsFile = "bookings.txt";

    Passenger *findPassenger(const string &uname)
    {
        for (int i = 0; i < passengers.size(); i++)
        {
            if (passengers[i].getUsername() == uname)
            {
                return &passengers[i];
            }
        }
        return nullptr;
    }


    Admin* findAdmin(const string& uname)
    {
        for (int i = 0; i < admins.size(); i++)
        {
            if (admins[i].getUsername() == uname)
            {
                return &admins[i];
            }
        }
        return nullptr;
    }


    Flight* findFlight(const string& flightNumber)
{
    for (int i = 0; i < flights.size(); i++)
    {
        if (flights[i].flightNumber == flightNumber)
        {
            return &flights[i];
        }
    }
    return nullptr;
}


    void loadAdmins()
    {
        ifstream fin(adminsFile);
        if (!fin.is_open())
        {
            firstTimeAdminSetup();
            return;
        }

        string line;
        bool loadedAny = false;
        while (getline(fin, line))
        {
            istringstream iss(line);
            vector<string> tokens;
            string token;
            while (getline(iss, token, ','))
                tokens.push_back(token);

            if (tokens.size() != 2)
                continue;
            admins.push_back(Admin(tokens[0], tokens[1]));
            loadedAny = true;
        }
        fin.close();

        if (!loadedAny)
        {
            firstTimeAdminSetup();
        }
    }

    void saveAdmins()
{
    ofstream fout(adminsFile);
    for (int i = 0; i < admins.size(); i++)
    {
        fout << admins[i].getUsername() << "," << admins[i].getPassword() << "\n";
    }
    fout.close();
}


    void firstTimeAdminSetup()
    {
        printColored("=== First Time Setup for Admin Account ===\n", YELLOW);
        string uname, pwd1, pwd2;
        cout << "Set Admin Username: ";
        getline(cin, uname);

        while (true)
        {
            cout << "Set Admin Password: ";
            getline(cin, pwd1);
            cout << "Confirm Admin Password: ";
            getline(cin, pwd2);

            if (pwd1 == pwd2)
            {
                break;
            }
            else
            {
                printColored("Passwords do not match. Please try again.\n", RED);
            }
        }
        admins.push_back(Admin(uname, pwd1));
        saveAdmins();
        printColored("Admin account created successfully! Please restart the program to login.\n", GREEN);
        exit(0);
    }

    void loadPassengers()
    {
        ifstream fin(passengersFile);
        if (!fin.is_open())
            return;

        string line;
        while (getline(fin, line))
        {
            istringstream iss(line);
            vector<string> tokens;
            string token;
            while (getline(iss, token, ','))
                tokens.push_back(token);

            if (tokens.size() != 2)
                continue;
            passengers.push_back(Passenger(tokens[0], tokens[1]));
        }
        fin.close();
    }

    void savePassengers()
    {
        ofstream fout(passengersFile);
        for (int i = 0; i < passengers.size(); i++)
        {
            fout << passengers[i].getUsername() << "," << passengers[i].getPassword() << "\n";
        }
        fout.close();
    }


    void loadFlights()
    {
        ifstream fin(flightsFile);
        if (!fin.is_open())
            return;

        string line;
        while (getline(fin, line))
        {
            Flight f = Flight::fromCSV(line);
            if (!f.flightNumber.empty())
            {
                flights.push_back(f);
            }
        }
        fin.close();
    }

    void saveFlights()
    {
        ofstream fout(flightsFile);
        for (int i = 0; i < flights.size(); i++)
        {
            fout << flights[i].toCSV() << "\n";
        }
        fout.close();
    }


    void loadBookings()
    {
        ifstream fin(bookingsFile);
        if (!fin.is_open())
            return;

        string line;
        while (getline(fin, line))
        {
            Booking b = Booking::fromCSV(line);
            if (!b.bookingID.empty())
                bookings.push_back(b);
        }
        fin.close();
    }

    void saveBookings()
    {
        ofstream fout(bookingsFile);
        for (int i = 0; i < bookings.size(); i++)
        {
            fout << bookings[i].toCSV() << "\n";
        }
        fout.close();
    }


public:
    AirlineSystem()
    {
        loadAdmins();
        loadPassengers();
        loadFlights();
        loadBookings();
    }

    ~AirlineSystem()
    {
        saveAdmins();
        savePassengers();
        saveFlights();
        saveBookings();
    }

    void run()
    {
        while (true)
        {
            printColored("Select Role:\n", YELLOW + BOLD);
            printColored("1. Admin\n2. Passenger\n3. Exit\n", YELLOW);
            int roleChoice = getInt("Enter choice: ", 1, 3);

            if (roleChoice == 1)
            {
                adminFlow();
            }
            else if (roleChoice == 2)
            {
                passengerFlow();
            }
            else
            {
                cout << R"(           
                       ______                __   ____           
                      / ____/___  ____  ____/ /  / __ )__  _____ 
                     / / __/ __ \/ __ \/ __  /  / __  / / / / _ \
                    / /_/ / /_/ / /_/ / /_/ /  / /_/ / /_/ /  __/
                    \____/\____/\____/\__,_/  /_____/\__, /\___/ 
                                                    /____/                        
                )" << endl;
                break;
            }
            cout << "\n";
        }
    }

private:
    void adminFlow()
    {
        printColored("\n--- Admin Login ---\n", CYAN + BOLD);
        cout << "Username: ";
        string uname;
        getline(cin >> ws, uname);
        cout << "Password: ";
        string pwd;
        getline(cin >> ws, pwd);

        Admin *admin = findAdmin(uname);
        if (admin && admin->login(uname, pwd))
        {
            printColored("Login successful! Welcome Admin " + uname + "\n", GREEN);
            adminMenu();
        }
        else
        {
            printColored("Login failed! Invalid username or password.\n", RED);
        }
    }

    void adminMenu()
    {
        while (true)
        {
            printColored("\n--- Admin Menu ---\n", MAGENTA + BOLD);
            printColored("1. Add Flight\n", MAGENTA);
            printColored("2. View All Flights\n", MAGENTA);
            printColored("3. Remove Flight\n", MAGENTA);
            printColored("4. Logout\n", MAGENTA);

            int choice = getInt("Enter choice: ", 1, 4);

            if (choice == 1)
            {
                addFlight();
            }
            else if (choice == 2)
            {
                viewFlights();
            }
            else if (choice == 3)
            {
                removeFlight();
            }
            else if (choice == 4)
            {
                printColored("Logging out from Admin account.\n", CYAN);
                break;
            }
        }
    }

    void addFlight()
    {
        cout << "Enter Flight Number: ";
        string fn;
        getline(cin >> ws, fn);

        cout << "Enter Origin: ";
        string org;
        getline(cin >> ws, org);

        cout << "Enter Destination: ";
        string dest;
        getline(cin >> ws, dest);

        cout << "Enter Date (YYYY-MM-DD): ";
        string d;
        getline(cin >> ws, d);

        cout << "Enter Time (HH:MM): ";
        string t;
        getline(cin >> ws, t);

        double p = getDouble("Enter Price: ", 0);

        int seats = getInt("Enter Total Seats: ", 1);

        for (const auto &f : flights)
        {
            if (f.flightNumber == fn)
            {
                printColored("Flight number already exists! Cannot add.\n", RED);
                return;
            }
        }

        flights.push_back(Flight(fn, org, dest, d, t, p, seats));
        saveFlights();
        printColored("Flight added successfully.\n", GREEN);
    }

    void viewFlights()
    {
        if (flights.empty())
        {
            printColored("No flights available.\n", YELLOW);
            return;
        }
        printColored("\nAll Flights:\n", CYAN + BOLD);
        Flight::printHeader();
        for (int i = 0; i < flights.size(); i++)
        {
            flights[i].display();
        }

    }

    void removeFlight()
    {
        if (flights.empty())
        {
            printColored("No flights available to remove.\n", YELLOW);
            return;
        }

        cout << "Enter Flight Number to remove: ";
        string fn;
        getline(cin >> ws, fn);

        for (auto it = flights.begin(); it != flights.end(); ++it)
        {
            if (it->flightNumber == fn)
            {
                flights.erase(it);
                saveFlights();
                printColored("Flight removed successfully.\n", GREEN);
                return;
            }
        }
        printColored("Flight number not found.\n", RED);
    }

    void passengerFlow()
    {
        while (true)
        {
            printColored("\n--- Passenger Menu ---\n", CYAN + BOLD);
            printColored("1. Register\n2. Login\n3. Back to Role Selection\n", CYAN);
            int choice = getInt("Enter choice: ", 1, 4);

            if (choice == 1)
            {
                passengerRegister();
            }
            else if (choice == 2)
            {
                Passenger *passenger = passengerLogin();
                if (passenger != nullptr)
                {
                    passengerMenu(passenger);
                }
            }
            else if (choice == 3)
            {
                break;
            }
            else
            {
                break;
            }
        }
    }

    void passengerRegister()
    {
        cout << "Enter desired username: ";
        string uname;
        getline(cin >> ws, uname);

        if (findPassenger(uname) != nullptr)
        {
            printColored("Username already exists! Please try login or choose another username.\n", RED);
            return;
        }

        cout << "Enter password: ";
        string pwd;
        getline(cin >> ws, pwd);

        passengers.push_back(Passenger(uname, pwd));
        savePassengers();
        printColored("Registration successful! You can now login.\n", GREEN);
    }

    Passenger *passengerLogin()
    {
        cout << "Username: ";
        string uname;
        getline(cin >> ws, uname);
        cout << "Password: ";
        string pwd;
        getline(cin >> ws, pwd);

        Passenger *passenger = findPassenger(uname);
        if (passenger && passenger->login(uname, pwd))
        {
            printColored("Login successful! Welcome Passenger " + uname + "\n", GREEN);
            return passenger;
        }
        else
        {
            printColored("Login failed! Invalid username or password.\n", RED);
            return nullptr;
        }
    }

    string generateBookingID(const string &username, const string &flightNumber)
    {
        static int counter = 0;
        counter++;
        return username + "_" + flightNumber + "_" + to_string(counter);
    }

    bool isSeatAvailable(const string &flightNumber, int seatNumber)
    {
        Flight *f = findFlight(flightNumber);
        if (!f)
            return false;
        if (seatNumber < 1 || seatNumber > f->totalSeats)
            return false;

        for (int i = 0; i < bookings.size(); i++)
        {
            if (!bookings[i].cancelled && bookings[i].flightNumber == flightNumber && bookings[i].seatNumber == seatNumber)
            {
                return false;
            }
        }
        return true;
    }

    void searchFlights()
    {
        cout << "Enter Origin (leave blank for any): ";
        string origin;
        getline(cin >> ws, origin);
        cout << "Enter Destination (leave blank for any): ";
        string dest;
        getline(cin >> ws, dest);
        cout << "Enter Date (YYYY-MM-DD, leave blank for any): ";
        string date;
        getline(cin >> ws, date);

        bool found = false;
        for (int i = 0; i < flights.size(); i++)
        {
            if ((origin.empty() || flights[i].origin == origin) &&
                (dest.empty() || flights[i].destination == dest) &&
                (date.empty() || flights[i].date == date))
            {
                flights[i].display();
                found = true;
            }
        }
        if (!found)
            printColored("No matching flights found.\n", YELLOW);
    }

    void bookTicket(Passenger *passenger)
    {
        cout << "Enter Flight Number to book: ";
        string flightNum;
        getline(cin >> ws, flightNum);
        Flight *f = findFlight(flightNum);
        if (!f)
        {
            printColored("Flight not found.\n", RED);
            return;
        }

        int seatNum = getInt("Enter seat number to book (1 - " + to_string(f->totalSeats) + "): ", 1, f->totalSeats);

        if (!isSeatAvailable(flightNum, seatNum))
        {
            printColored("Seat not available or invalid.\n", RED);
            return;
        }

        string bookingID = generateBookingID(passenger->getUsername(), flightNum);
        Booking newBooking(bookingID, passenger->getUsername(), flightNum, seatNum);
        bookings.push_back(newBooking);
        saveBookings();
        printColored("Booking successful! Your Booking ID is: " + bookingID + "\n", GREEN);
    }

    void viewBookingHistory(Passenger *passenger)
    {
        printColored("\nYour Bookings:\n", CYAN + BOLD);
        Booking::printHeader();
        bool found = false;
        for (const auto &b : bookings)
        {
            if (b.passengerUsername == passenger->getUsername())
            {
                b.display();
                found = true;
            }
        }
        if (!found)
            printColored("No bookings found.\n", YELLOW);
    }

    void cancelBooking(Passenger *passenger)
    {
        cout << "Enter Booking ID to cancel: ";
        string bookingID;
        getline(cin >> ws, bookingID);

        for (auto &b : bookings)
        {
            if (b.bookingID == bookingID && b.passengerUsername == passenger->getUsername())
            {
                if (b.cancelled)
                {
                    printColored("Booking already cancelled.\n", YELLOW);
                    return;
                }
                b.cancelled = true;
                saveBookings();
                printColored("Booking cancelled successfully.\n", GREEN);
                return;
            }
        }
        printColored("Booking ID not found.\n", RED);
    }

    void passengerMenu(Passenger *passenger)
    {
        while (true)
        {
            printColored("\n--- Passenger Menu ---\n", CYAN + BOLD);
            printColored("1. Search Flights\n", CYAN);
            printColored("2. Book Ticket\n", CYAN);
            printColored("3. Cancel Booking\n", CYAN);
            printColored("4. View Booking History\n", CYAN);
            printColored("5. View Flights\n", CYAN);
            printColored("6. Logout\n", CYAN);

            int choice = getInt("Enter choice: ", 1, 6);

            if (choice == 1)
            {
                searchFlights();
            }
            else if (choice == 2)
            {
                bookTicket(passenger);
            }
            else if (choice == 3)
            {
                cancelBooking(passenger);
            }
            else if (choice == 4)
            {
                viewBookingHistory(passenger);
            }
            else if (choice == 5)
            {
                viewFlights();
            }
            else if (choice == 6)
            {
                printColored("Logging out from Passenger account.\n", CYAN);
                break;
            }
        }
    }
};

int main()
{
    system("cls");
        cout << R"(           
             _      _                _____                                _   _                _____             _                 
       /\   (_)    | | (_)          |  __ \                              | | (_)              / ____|           | |                
      /  \   _ _ __| |_ _ __   ___  | |__) |___  ___  ___ _ ____   ____ _| |_ _  ___  _ __   | (___  _   _   ___| |_ ___ _ __ ___  
     / /\ \ | | '__| | | '_ \ / _ \ |  _  // _ \/ __|/ _ \ '__\ \ / / _` | __| |/ _ \| '_ \   \___ \| | | | / __| __/ _ \_'_ ` _ \ 
    / ____ \| | |  | | | | | |  __/ | | \ \  __/\__ \  __/ |   \ V / (_| | |_| | (_) | | | |  ____) | |_| | \__ \ ||  __/ | | | | |
   /_/    \_\_|_|  |_|_|_| |_|\___| |_|  \_\___||___/\___|_|    \_/ \__,_|\__|_|\___/|_| |_| |_____/ \__, | ___/\__\___|  |_|_| |_|
                                                                                                       _/ |                      
                                                                                                     /___/                       
    )" << endl;
    AirlineSystem system;
    system.run();
    return 0;
}