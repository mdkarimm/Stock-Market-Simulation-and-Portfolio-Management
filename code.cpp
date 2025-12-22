//Stock Market Simulation and Portfolio Management System
//Optimized for Windows - Enhanced Version

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <algorithm>

#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
#else
    #include <termios.h>
    #include <unistd.h>
#endif

using namespace std;

// Color codes for Windows console
#ifdef _WIN32
    #define COLOR_DEFAULT 7
    #define COLOR_GREEN 10
    #define COLOR_RED 12
    #define COLOR_YELLOW 14
    #define COLOR_CYAN 11
    #define COLOR_BLUE 9
    #define COLOR_MAGENTA 13
    #define COLOR_WHITE 15
    #define COLOR_GRAY 8
#endif

// Function to set console text color
void setColor(int color) {
    #ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, color);
    #endif
}

// Function to reset color to default
void resetColor() {
    setColor(COLOR_DEFAULT);
}

// Function to print colored text
void printColored(const string& text, int color) {
    setColor(color);
    cout << text;
    resetColor();
}

// Function to clear screen - Windows compatible
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Function to get character without echo - cross-platform
char getChar() {
    #ifdef _WIN32
        return _getch();
    #else
        struct termios oldt, newt;
        char ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    #endif
}

// Function to get current date and time string
string getCurrentDateTime() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    stringstream ss;
    ss << setfill('0') 
       << setw(2) << ltm->tm_mday << "/"
       << setw(2) << (1 + ltm->tm_mon) << "/"
       << (1900 + ltm->tm_year) << " "
       << setw(2) << ltm->tm_hour << ":"
       << setw(2) << ltm->tm_min << ":"
       << setw(2) << ltm->tm_sec;
    return ss.str();
}

// Function to format large numbers with commas
string formatNumber(double value) {
    stringstream ss;
    ss << fixed << setprecision(2) << value;
    string str = ss.str();
    
    // Find decimal point
    size_t decimalPos = str.find('.');
    if (decimalPos == string::npos) decimalPos = str.length();
    
    // Add commas
    int count = 0;
    for (int i = decimalPos - 1; i > 0; i--) {
        count++;
        if (count == 3 && i > 0 && str[i-1] != '-') {
            str.insert(i, ",");
            count = 0;
        }
    }
    return str;
}

// Read stock data from file and store in array
void readDataFromFile(const string& filename, string companies[38][3], int &numCompanies) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Failed to open " << filename << endl;
        cerr << "Please ensure the file exists in the same directory as the executable." << endl;
        return;
    }

    string line;
    int row = 0;
    
    // Skip header line
    getline(file, line);

    while (getline(file, line) && row < 38) {
        stringstream ss(line);
        string cell;
        int col = 0;

        // Read symbol
        getline(ss, companies[row][0], ',');
        
        // Read company name (may contain commas)
        getline(ss, companies[row][1], ',');
        
        // Read stock price
        getline(ss, companies[row][2], ',');

        // Handle multi-part company names
        while (getline(ss, cell, ',') && col < 2) {
            companies[row][1] += "," + cell;
            col++;
        }

        ++row;
    }

    numCompanies = row;
    file.close();
    
    if (numCompanies == 0) {
        cerr << "Warning: No stock data loaded from file." << endl;
    }
}

// Save stock data to file
void saveDataToFile(const string& filename, string companies[38][3], int numCompanies) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Failed to open " << filename << " for writing." << endl;
        return;
    }

    file << "Symbol,Company Name,Stock Price\n";

    for (int row = 0; row < numCompanies; ++row) {
        file << companies[row][0] << "," 
             << companies[row][1] << "," 
             << companies[row][2] << "\n";
    }

    file.close();
}

// Display stock market screen with colors and enhanced formatting
void displayStockMarketScreen(string companies[38][3], double prevPrices[38], double highPrices[38], double lowPrices[38], double percentChanges[38], int numCompanies, const string& topAdvancer, const string& topDecliner) {
    clearScreen();
    
    // Header with colors
    setColor(COLOR_CYAN);
    cout << string(135, '=') << "\n";
    setColor(COLOR_YELLOW);
    cout << "                                     KARACHI STOCK EXCHANGE - LIVE MARKET                                 \n";
    setColor(COLOR_CYAN);
    cout << string(135, '=') << "\n";
    resetColor();
    
    // Display current time
    setColor(COLOR_GRAY);
    cout << " Last Updated: " << getCurrentDateTime() << "\n";
    resetColor();
    
    // Menu options with colors
    setColor(COLOR_WHITE);
    cout << " ";
    printColored("[ENTER]", COLOR_GREEN);
    cout << " Update  ";
    printColored("[P]", COLOR_CYAN);
    cout << " Portfolio  ";
    printColored("[A]", COLOR_GREEN);
    cout << " Buy  ";
    printColored("[R]", COLOR_RED);
    cout << " Sell  ";
    printColored("[M]", COLOR_YELLOW);
    cout << " Deposit  ";
    printColored("[W]", COLOR_YELLOW);
    cout << " Withdraw  ";
    printColored("[E]", COLOR_RED);
    cout << " Exit\n";
    resetColor();
    
    setColor(COLOR_CYAN);
    cout << string(135, '-') << "\n";
    resetColor();
    
    // Table header
    setColor(COLOR_WHITE);
    cout << left << setw(10) << "Symbol" 
         << setw(40) << "Company Name" 
         << setw(15) << "Prev Price" 
         << setw(18) << "Current Price"
         << setw(12) << "Change %" 
         << setw(15) << "High" 
         << setw(15) << "Low" << "\n";
    resetColor();
    
    setColor(COLOR_CYAN);
    cout << string(135, '-') << "\n";
    resetColor();
    
    // Display stocks with color-coded prices
    for (int i = 0; i < numCompanies; ++i) {
        double currPrice = stod(companies[i][2]);
        double change = currPrice - prevPrices[i];
        double changePercent = (prevPrices[i] != 0) ? (change / prevPrices[i]) * 100.0 : 0;
        
        // Symbol and company name
        setColor(COLOR_CYAN);
        cout << left << setw(10) << companies[i][0];
        resetColor();
        cout << setw(40) << companies[i][1];
        
        // Previous price
        cout << setw(15) << fixed << setprecision(2) << prevPrices[i];
        
        // Current price with color
        if (currPrice > prevPrices[i]) {
            setColor(COLOR_GREEN);
            cout << "$" << setw(11) << fixed << setprecision(2) << currPrice << " " << (char)24; // Up arrow
            resetColor();
        } else if (currPrice < prevPrices[i]) {
            setColor(COLOR_RED);
            cout << "$" << setw(11) << fixed << setprecision(2) << currPrice << " " << (char)25; // Down arrow
            resetColor();
        } else {
            setColor(COLOR_GRAY);
            cout << "$" << setw(11) << fixed << setprecision(2) << currPrice << " -";
            resetColor();
        }
        
        // Change percentage with color
        cout << "  ";
        if (changePercent > 0) {
            setColor(COLOR_GREEN);
            cout << "+" << setw(8) << fixed << setprecision(2) << changePercent << "%";
        } else if (changePercent < 0) {
            setColor(COLOR_RED);
            cout << setw(9) << fixed << setprecision(2) << changePercent << "%";
        } else {
            setColor(COLOR_GRAY);
            cout << setw(9) << "0.00%";
        }
        resetColor();
        
        // High and Low prices
        cout << "  ";
        setColor(COLOR_GREEN);
        cout << setw(15) << fixed << setprecision(2) << highPrices[i];
        setColor(COLOR_RED);
        cout << setw(15) << fixed << setprecision(2) << lowPrices[i];
        resetColor();
        
        cout << "\n";
    }
    
    setColor(COLOR_CYAN);
    cout << string(135, '=') << "\n";
    resetColor();
    
    // Display top performers
    cout << " ";
    setColor(COLOR_GREEN);
    cout << "TOP GAINER: ";
    setColor(COLOR_WHITE);
    cout << topAdvancer;
    resetColor();
    cout << "  |  ";
    setColor(COLOR_RED);
    cout << "TOP LOSER: ";
    setColor(COLOR_WHITE);
    cout << topDecliner;
    resetColor();
    cout << "\n";
    
    setColor(COLOR_CYAN);
    cout << string(135, '=') << "\n";
    resetColor();
}

// Update stock prices with random fluctuations
void updatePrices(string companies[38][3], double prevPrices[38], double highPrices[38], double lowPrices[38], double percentChanges[38], int numCompanies) {
    for (int i = 0; i < numCompanies; ++i) {
        double oldPrice = stod(companies[i][2]);
        double maxChange = oldPrice * 0.15; // Max 15% change
        double change = ((rand() % 2001 - 1000) / 1000.0) * maxChange;
        double newPrice = max(0.01, oldPrice + change); // Ensure minimum price of 0.01
        
        prevPrices[i] = oldPrice;
        companies[i][2] = to_string(newPrice);

        // Update high and low prices
        highPrices[i] = max(highPrices[i], newPrice);
        lowPrices[i] = min(lowPrices[i], newPrice);

        // Calculate percentage change
        percentChanges[i] = ((newPrice - oldPrice) / oldPrice) * 100.0;
    }
}

// Find stocks with highest gains and losses
void findTopAdvancerAndDecliner(string companies[38][3], double percentChanges[38], int numCompanies, string &topAdvancer, string &topDecliner) {
    double maxIncrease = -999.0;
    double maxDecrease = 999.0;

    for (int i = 0; i < numCompanies; ++i) {
        if (percentChanges[i] > maxIncrease) {
            maxIncrease = percentChanges[i];
            topAdvancer = companies[i][0];
        }
        if (percentChanges[i] < maxDecrease) {
            maxDecrease = percentChanges[i];
            topDecliner = companies[i][0];
        }
    }
}

// Load portfolio from file or create new one
void loadPortfolio(string portfolioStocks[50][3], int portfolioShares[50], int &numPortfolioStocks, string &userName, double &accountBalance, double prevPrices[38], string companies[38][3], int numCompanies) {
    ifstream file("portfolio.txt");
    
    if (file.is_open()) {
        string line;
        
        // Read first line to get username
        getline(file, line);
        size_t pos = line.find("Portfolio owner: ");
        if (pos != string::npos) {
            userName = line.substr(pos + 17);
        }
        
        // Skip empty line
        getline(file, line);
        
        // Skip header line
        getline(file, line);
        
        // Skip separator line
        getline(file, line);

        numPortfolioStocks = 0;
        while (getline(file, line) && numPortfolioStocks < 50) {
            // Check for Total Gain/Loss line
            if (line.find("Total Gain/Loss:") != string::npos) {
                break;
            }
            
            // Check for empty line
            if (line.empty()) {
                continue;
            }
            
            stringstream ss(line);
            string stock, company, sharesStr;
            
            // Parse the fixed-width format
            ss >> stock;
            if (stock.empty()) continue;
            
            // Read company name (up to shares column)
            string temp;
            company = "";
            while (ss >> temp) {
                // Check if temp is a number (shares)
                bool isNumber = true;
                for (char c : temp) {
                    if (!isdigit(c)) {
                        isNumber = false;
                        break;
                    }
                }
                
                if (isNumber && !company.empty()) {
                    sharesStr = temp;
                    break;
                }
                
                if (!company.empty()) company += " ";
                company += temp;
            }
            
            if (!sharesStr.empty()) {
                portfolioStocks[numPortfolioStocks][0] = stock;
                portfolioStocks[numPortfolioStocks][1] = company;
                portfolioShares[numPortfolioStocks] = stoi(sharesStr);
                ++numPortfolioStocks;
            }
        }
        
        // Read the balance
        while (getline(file, line)) {
            if (line.find("Current Balance:") != string::npos) {
                size_t balPos = line.find(":");
                if (balPos != string::npos) {
                    try {
                        accountBalance = stod(line.substr(balPos + 1));
                    } catch (...) {
                        accountBalance = 10000.0; // Default balance
                    }
                }
                break;
            }
        }
        
        file.close();
        cout << "Portfolio loaded successfully for " << userName << "!\n";
        cout << "Press any key to continue...\n";
        getChar();
    } else {
        // Create new portfolio
        clearScreen();
        cout << "=================================================\n";
        cout << "   Welcome to Stock Market Portfolio Manager   \n";
        cout << "=================================================\n\n";
        cout << "No existing portfolio found. Let's create a new one!\n\n";
        
        cout << "Enter your name: ";
        getline(cin, userName);
        
        while (true) {
            cout << "Enter your initial account balance: $";
            cin >> accountBalance;
            
            if (cin.fail() || accountBalance < 0) {
                cin.clear();
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                cout << "Invalid input. Please enter a positive number.\n";
            } else {
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                break;
            }
        }
        
        // Initialize prevPrices with current prices
        for (int i = 0; i < numCompanies; ++i) {
            prevPrices[i] = stod(companies[i][2]);
        }
        
        cout << "\nPortfolio created successfully!\n";
        cout << "Press any key to continue...\n";
        getChar();
    }
}


// Save portfolio to file with formatted output
void savePortfolio(string portfolioStocks[50][3], int portfolioShares[50], int numPortfolioStocks, const string &userName, double accountBalance, string companies[38][3], double prevPrices[38], double highPrices[38], double lowPrices[38], double percentChanges[38], int numCompanies) {
    ofstream file("portfolio.txt");
    if (!file.is_open()) {
        cerr << "Error: Failed to open portfolio.txt for writing." << endl;
        return;
    }

    file << "Portfolio owner: " << userName << "\n\n";
    file << left << setw(10) << "Stocks" << setw(40) << "Company Name" << setw(10) << "Shares" 
         << setw(10) << "Current" << setw(10) << "Previous" << setw(10) << "Gain/Loss" 
         << setw(10) << "High" << setw(10) << "Low" << "\n";
    file << string(130, '*') << "\n";

    double totalGainLoss = 0.0;
    double totalPortfolioValue = 0.0;

    for (int i = 0; i < numPortfolioStocks; ++i) {
        string stockSymbol = portfolioStocks[i][0];
        string companyName = portfolioStocks[i][1];
        int shares = portfolioShares[i];

        double currPrice = 0.0;
        double prevPrice = 0.0;
        double highPrice = 0.0;
        double lowPrice = 0.0;
        double gainLoss = 0.0;

        for (int j = 0; j < numCompanies; ++j) {
            if (companies[j][0] == stockSymbol) {
                currPrice = stod(companies[j][2]);
                prevPrice = prevPrices[j];
                highPrice = highPrices[j];
                lowPrice = lowPrices[j];
                gainLoss = (currPrice - prevPrice) * shares;
                totalGainLoss += gainLoss;
                totalPortfolioValue += currPrice * shares;
                break;
            }
        }

        file << left << setw(10) << stockSymbol
             << setw(40) << companyName
             << setw(10) << shares
             << setw(10) << fixed << setprecision(2) << currPrice
             << setw(10) << fixed << setprecision(2) << prevPrice
             << setw(10) << fixed << setprecision(2) << gainLoss
             << setw(10) << fixed << setprecision(2) << highPrice
             << setw(10) << fixed << setprecision(2) << lowPrice
             << "\n";
    }

    file << "\nTotal Gain/Loss: " << fixed << setprecision(2) << totalGainLoss << "\n";
    file << "Portfolio Value: " << fixed << setprecision(2) << totalPortfolioValue << "\n";
    file << "Current Balance: " << fixed << setprecision(2) << accountBalance << "\n";
    file << "Total Assets: " << fixed << setprecision(2) << (accountBalance + totalPortfolioValue) << "\n";

    file.close();
}

// Add stock to portfolio with validation
void addStockToPortfolio(string portfolioStocks[50][3], int portfolioShares[50], int &numPortfolioStocks, string companies[38][3], int numCompanies, double &accountBalance) {
    string stockSymbol;
    int shares;

    cout << "\nEnter the stock symbol to add (or 'C' to cancel): ";
    cin >> stockSymbol;
    
    if (stockSymbol == "C" || stockSymbol == "c") {
        cout << "Operation cancelled.\n";
        return;
    }
    
    // Convert to uppercase for consistency
    transform(stockSymbol.begin(), stockSymbol.end(), stockSymbol.begin(), ::toupper);

    cout << "Enter the number of shares: ";
    cin >> shares;
    
    // Validate shares input
    if (cin.fail() || shares <= 0) {
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cout << "Invalid number of shares. Operation cancelled.\n";
        return;
    }

    bool stockFound = false;

    for (int i = 0; i < numCompanies; ++i) {
        if (companies[i][0] == stockSymbol) {
            stockFound = true;
            double stockPrice = stod(companies[i][2]);
            double totalCost = shares * stockPrice;

            cout << "\nStock: " << stockSymbol << " - " << companies[i][1] << "\n";
            cout << "Price per share: $" << fixed << setprecision(2) << stockPrice << "\n";
            cout << "Total cost: $" << fixed << setprecision(2) << totalCost << "\n";
            cout << "Current balance: $" << fixed << setprecision(2) << accountBalance << "\n";

            if (accountBalance >= totalCost) {
                bool alreadyInPortfolio = false;

                for (int j = 0; j < numPortfolioStocks; ++j) {
                    if (portfolioStocks[j][0] == stockSymbol) {
                        portfolioShares[j] += shares;
                        alreadyInPortfolio = true;
                        cout << "\nStock already in portfolio. Added " << shares << " more shares.\n";
                        break;
                    }
                }

                if (!alreadyInPortfolio && numPortfolioStocks < 50) {
                    portfolioStocks[numPortfolioStocks][0] = stockSymbol;
                    portfolioStocks[numPortfolioStocks][1] = companies[i][1];
                    portfolioShares[numPortfolioStocks] = shares;
                    ++numPortfolioStocks;
                    cout << "\nStock added to portfolio successfully!\n";
                } else if (numPortfolioStocks >= 50) {
                    cout << "Portfolio is full. Cannot add more stocks.\n";
                    return;
                }

                accountBalance -= totalCost;
                cout << "New balance: $" << fixed << setprecision(2) << accountBalance << "\n";
            } else {
                cout << "\nInsufficient balance! You need $" << fixed << setprecision(2) << (totalCost - accountBalance) << " more.\n";
            }
            break;
        }
    }

    if (!stockFound) {
        cout << "Stock symbol '" << stockSymbol << "' not found in the market.\n";
    }
    
    cout << "\nPress any key to continue...";
    getChar();
}
// Remove stock from portfolio with validation
void removeStockFromPortfolio(string portfolioStocks[50][3], int portfolioShares[50], int &numPortfolioStocks, string companies[38][3], int numCompanies, double &accountBalance) {
    if (numPortfolioStocks == 0) {
        cout << "\nYour portfolio is empty. Nothing to remove.\n";
        cout << "Press any key to continue...";
        getChar();
        return;
    }
    
    string stockSymbol;
    int sharesToSell;

    cout << "\nEnter the stock symbol to sell (or 'C' to cancel): ";
    cin >> stockSymbol;
    
    if (stockSymbol == "C" || stockSymbol == "c") {
        cout << "Operation cancelled.\n";
        return;
    }
    
    // Convert to uppercase
    transform(stockSymbol.begin(), stockSymbol.end(), stockSymbol.begin(), ::toupper);

    bool stockFound = false;

    for (int i = 0; i < numPortfolioStocks; ++i) {
        if (portfolioStocks[i][0] == stockSymbol) {
            stockFound = true;
            
            // Get current stock price from the market
            double stockPrice = 0.0;
            for (int j = 0; j < numCompanies; ++j) {
                if (companies[j][0] == stockSymbol) {
                    stockPrice = stod(companies[j][2]);
                    break;
                }
            }
            
            cout << "\nYou own " << portfolioShares[i] << " shares of " << stockSymbol << "\n";
            cout << "Current price: $" << fixed << setprecision(2) << stockPrice << " per share\n";
            cout << "Enter number of shares to sell (0 for all): ";
            cin >> sharesToSell;
            
            if (cin.fail() || sharesToSell < 0) {
                cin.clear();
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                cout << "Invalid input. Operation cancelled.\n";
                return;
            }
            
            if (sharesToSell == 0 || sharesToSell >= portfolioShares[i]) {
                sharesToSell = portfolioShares[i];
            }
            
            // Calculate the amount to add to the balance
            double totalValue = stockPrice * sharesToSell;
            accountBalance += totalValue;
            
            cout << "\nSold " << sharesToSell << " shares for $" << fixed << setprecision(2) << totalValue << "\n";
            
            // Update or remove the stock from portfolio
            if (sharesToSell >= portfolioShares[i]) {
                // Remove completely
                for (int j = i; j < numPortfolioStocks - 1; ++j) {
                    portfolioStocks[j][0] = portfolioStocks[j + 1][0];
                    portfolioStocks[j][1] = portfolioStocks[j + 1][1];
                    portfolioShares[j] = portfolioShares[j + 1];
                }
                --numPortfolioStocks;
                cout << "Stock completely removed from portfolio.\n";
            } else {
                // Reduce shares
                portfolioShares[i] -= sharesToSell;
                cout << "Remaining shares: " << portfolioShares[i] << "\n";
            }
            
            cout << "New balance: $" << fixed << setprecision(2) << accountBalance << "\n";
            break;
        }
    }

    if (!stockFound) {
        cout << "Stock symbol '" << stockSymbol << "' not found in your portfolio.\n";
    }
    
    cout << "\nPress any key to continue...";
    getChar();
}
// Display portfolio with colors and detailed information
void displayPortfolio(string portfolioStocks[50][3], int portfolioShares[50], int numPortfolioStocks, const string& userName, double accountBalance, string companies[38][3], double prevPrices[38], double highPrices[38], double lowPrices[38], int numCompanies) {
    clearScreen();
    
    // Header
    setColor(COLOR_CYAN);
    cout << string(145, '=') << "\n";
    setColor(COLOR_YELLOW);
    cout << "                                          PORTFOLIO - " << userName << "                                          \n";
    setColor(COLOR_CYAN);
    cout << string(145, '=') << "\n";
    resetColor();
    
    // Display current time
    setColor(COLOR_GRAY);
    cout << " Last Updated: " << getCurrentDateTime() << "\n";
    resetColor();
    
    // Menu options
    setColor(COLOR_WHITE);
    cout << " ";
    printColored("[ENTER]", COLOR_GREEN);
    cout << " Update  ";
    printColored("[A]", COLOR_GREEN);
    cout << " Buy  ";
    printColored("[R]", COLOR_RED);
    cout << " Sell  ";
    printColored("[M]", COLOR_YELLOW);
    cout << " Deposit  ";
    printColored("[W]", COLOR_YELLOW);
    cout << " Withdraw  ";
    printColored("[E]", COLOR_CYAN);
    cout << " Back\n";
    resetColor();
    
    setColor(COLOR_CYAN);
    cout << string(145, '-') << "\n";
    resetColor();
    
    double totalGainLoss = 0.0;
    double totalPortfolioValue = 0.0;
    double totalInvested = 0.0;
    
    if (numPortfolioStocks == 0) {
        setColor(COLOR_YELLOW);
        cout << "\n  Your portfolio is empty. Press 'A' to start investing!\n\n";
        resetColor();
    } else {
        // Table header
        setColor(COLOR_WHITE);
        cout << left << setw(10) << "Symbol" 
             << setw(35) << "Company Name" 
             << setw(10) << "Shares" 
             << setw(15) << "Avg Price"
             << setw(15) << "Curr Price" 
             << setw(18) << "Market Value"
             << setw(18) << "Gain/Loss"
             << setw(12) << "Return %" << "\n";
        resetColor();
        
        setColor(COLOR_CYAN);
        cout << string(145, '-') << "\n";
        resetColor();

        for (int i = 0; i < numPortfolioStocks; ++i) {
            string stockSymbol = portfolioStocks[i][0];
            string companyName = portfolioStocks[i][1];
            int shares = portfolioShares[i];

            double currPrice = 0.0;
            double avgPrice = 0.0;

            for (int j = 0; j < numCompanies; ++j) {
                if (companies[j][0] == stockSymbol) {
                    currPrice = stod(companies[j][2]);
                    avgPrice = prevPrices[j]; // Using prev price as purchase price
                    break;
                }
            }
            
            double marketValue = currPrice * shares;
            double invested = avgPrice * shares;
            double gainLoss = marketValue - invested;
            double returnPercent = (invested != 0) ? (gainLoss / invested) * 100.0 : 0;
            
            totalGainLoss += gainLoss;
            totalPortfolioValue += marketValue;
            totalInvested += invested;

            // Display stock info
            setColor(COLOR_CYAN);
            cout << left << setw(10) << stockSymbol;
            resetColor();
            cout << setw(35) << companyName
                 << setw(10) << shares;
            
            // Average price
            cout << "$" << setw(14) << fixed << setprecision(2) << avgPrice;
            
            // Current price with color
            if (currPrice > avgPrice) {
                setColor(COLOR_GREEN);
            } else if (currPrice < avgPrice) {
                setColor(COLOR_RED);
            } else {
                setColor(COLOR_GRAY);
            }
            cout << "$" << setw(14) << fixed << setprecision(2) << currPrice;
            resetColor();
            
            // Market value
            cout << "$" << setw(17) << fixed << setprecision(2) << marketValue;
            
            // Gain/Loss with color
            if (gainLoss > 0) {
                setColor(COLOR_GREEN);
                cout << "+$" << setw(16) << fixed << setprecision(2) << gainLoss;
            } else if (gainLoss < 0) {
                setColor(COLOR_RED);
                cout << "-$" << setw(16) << fixed << setprecision(2) << abs(gainLoss);
            } else {
                setColor(COLOR_GRAY);
                cout << "$" << setw(17) << fixed << setprecision(2) << gainLoss;
            }
            resetColor();
            
            // Return percentage with color
            if (returnPercent > 0) {
                setColor(COLOR_GREEN);
                cout << "+" << setw(8) << fixed << setprecision(2) << returnPercent << "%";
            } else if (returnPercent < 0) {
                setColor(COLOR_RED);
                cout << setw(9) << fixed << setprecision(2) << returnPercent << "%";
            } else {
                setColor(COLOR_GRAY);
                cout << setw(9) << "0.00%";
            }
            resetColor();
            
            cout << "\n";
        }
        
        setColor(COLOR_CYAN);
        cout << string(145, '=') << "\n";
        resetColor();
    }

    // Portfolio summary
    cout << "\n";
    setColor(COLOR_WHITE);
    cout << " PORTFOLIO SUMMARY:\n";
    resetColor();
    setColor(COLOR_CYAN);
    cout << " " << string(70, '-') << "\n";
    resetColor();
    
    cout << " Total Invested:      ";
    setColor(COLOR_YELLOW);
    cout << "$" << formatNumber(totalInvested) << "\n";
    resetColor();
    
    cout << " Portfolio Value:     ";
    setColor(COLOR_CYAN);
    cout << "$" << formatNumber(totalPortfolioValue) << "\n";
    resetColor();
    
    cout << " Cash Balance:        ";
    setColor(COLOR_GREEN);
    cout << "$" << formatNumber(accountBalance) << "\n";
    resetColor();
    
    cout << " Total Assets:        ";
    setColor(COLOR_WHITE);
    cout << "$" << formatNumber(accountBalance + totalPortfolioValue) << "\n";
    resetColor();
    
    cout << " Total Gain/Loss:     ";
    if (totalGainLoss > 0) {
        setColor(COLOR_GREEN);
        cout << "+$" << formatNumber(totalGainLoss) << " " << (char)24; // Up arrow
    } else if (totalGainLoss < 0) {
        setColor(COLOR_RED);
        cout << "-$" << formatNumber(abs(totalGainLoss)) << " " << (char)25; // Down arrow
    } else {
        setColor(COLOR_GRAY);
        cout << "$0.00";
    }
    resetColor();
    
    double totalReturnPercent = (totalInvested != 0) ? (totalGainLoss / totalInvested) * 100.0 : 0;
    cout << " (";
    if (totalReturnPercent > 0) {
        setColor(COLOR_GREEN);
        cout << "+" << fixed << setprecision(2) << totalReturnPercent << "%";
    } else if (totalReturnPercent < 0) {
        setColor(COLOR_RED);
        cout << fixed << setprecision(2) << totalReturnPercent << "%";
    } else {
        cout << "0.00%";
    }
    resetColor();
    cout << ")\n";
    
    setColor(COLOR_CYAN);
    cout << " " << string(70, '-') << "\n";
    resetColor();
}


// Add money to account with validation
void addMoneyToAccount(double &accountBalance) {
    double amount;
    cout << "\nCurrent balance: $" << fixed << setprecision(2) << accountBalance << "\n";
    cout << "Enter amount to deposit: $";
    cin >> amount;
    
    if (cin.fail() || amount <= 0) {
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cout << "Invalid amount. Please enter a positive number.\n";
    } else {
        accountBalance += amount;
        cout << "Successfully deposited $" << fixed << setprecision(2) << amount << "\n";
        cout << "New balance: $" << fixed << setprecision(2) << accountBalance << "\n";
    }
    
    cout << "\nPress any key to continue...";
    getChar();
}

// Withdraw money from account with validation
void withdrawMoney(double &accountBalance) {
    double amount;
    cout << "\nCurrent balance: $" << fixed << setprecision(2) << accountBalance << "\n";
    cout << "Enter amount to withdraw: $";
    cin >> amount;
    
    if (cin.fail() || amount <= 0) {
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cout << "Invalid amount. Please enter a positive number.\n";
    } else if (amount > accountBalance) {
        cout << "Insufficient funds! You can withdraw up to $" << fixed << setprecision(2) << accountBalance << "\n";
    } else {
        accountBalance -= amount;
        cout << "Successfully withdrew $" << fixed << setprecision(2) << amount << "\n";
        cout << "New balance: $" << fixed << setprecision(2) << accountBalance << "\n";
    }
    
    cout << "\nPress any key to continue...";
    getChar();
}

// Main function
int main() {
    // Seed random number generator
    srand(static_cast<unsigned int>(time(nullptr)));

    // Declare arrays and variables
    string companies[38][3];
    double prevPrices[38];
    double highPrices[38];
    double lowPrices[38];
    double percentChanges[38];
    int numCompanies = 0;

    string portfolioStocks[50][3];
    int portfolioShares[50];
    int numPortfolioStocks = 0;
    string userName;
    double accountBalance = 0.0;

    // Read stock data from file
    readDataFromFile("companies.txt", companies, numCompanies);
    
    if (numCompanies == 0) {
        cerr << "\nError: No stock data available. Please check companies.txt file.\n";
        cerr << "Press any key to exit...";
        getChar();
        return 1;
    }
      
    // Initialize high and low prices with current prices
    for (int i = 0; i < numCompanies; ++i) {
        highPrices[i] = stod(companies[i][2]);
        lowPrices[i] = stod(companies[i][2]);
        prevPrices[i] = stod(companies[i][2]);
    }
    
    // Load or create portfolio
    loadPortfolio(portfolioStocks, portfolioShares, numPortfolioStocks, userName, accountBalance, prevPrices, companies, numCompanies);

    // Main application loop
    bool running = true;
    while (running) {
        string topAdvancer, topDecliner;
        updatePrices(companies, prevPrices, highPrices, lowPrices, percentChanges, numCompanies);
        findTopAdvancerAndDecliner(companies, percentChanges, numCompanies, topAdvancer, topDecliner);
        displayStockMarketScreen(companies, prevPrices, highPrices, lowPrices, percentChanges, numCompanies, topAdvancer, topDecliner);
        
        // Save portfolio after each update
        savePortfolio(portfolioStocks, portfolioShares, numPortfolioStocks, userName, accountBalance, companies, prevPrices, highPrices, lowPrices, percentChanges, numCompanies);
        
        // Get user input
        char input = getChar();

        switch (input) {
            case 'P':
            case 'p': {
                // Portfolio view loop
                bool inPortfolio = true;
                while (inPortfolio) {
                    displayPortfolio(portfolioStocks, portfolioShares, numPortfolioStocks, userName, accountBalance, companies, prevPrices, highPrices, lowPrices, numCompanies);
                    
                    char portfolioInput = getChar();

                    switch (portfolioInput) {
                        case '\r': // Enter key - update prices
                            updatePrices(companies, prevPrices, highPrices, lowPrices, percentChanges, numCompanies);
                            savePortfolio(portfolioStocks, portfolioShares, numPortfolioStocks, userName, accountBalance, companies, prevPrices, highPrices, lowPrices, percentChanges, numCompanies);
                            break;
                            
                        case 'A':
                        case 'a':
                            addStockToPortfolio(portfolioStocks, portfolioShares, numPortfolioStocks, companies, numCompanies, accountBalance);
                            savePortfolio(portfolioStocks, portfolioShares, numPortfolioStocks, userName, accountBalance, companies, prevPrices, highPrices, lowPrices, percentChanges, numCompanies);
                            break;
                            
                        case 'R':
                        case 'r':
                            removeStockFromPortfolio(portfolioStocks, portfolioShares, numPortfolioStocks, companies, numCompanies, accountBalance);
                            savePortfolio(portfolioStocks, portfolioShares, numPortfolioStocks, userName, accountBalance, companies, prevPrices, highPrices, lowPrices, percentChanges, numCompanies);
                            break;
                            
                        case 'M':
                        case 'm':
                            addMoneyToAccount(accountBalance);
                            savePortfolio(portfolioStocks, portfolioShares, numPortfolioStocks, userName, accountBalance, companies, prevPrices, highPrices, lowPrices, percentChanges, numCompanies);
                            break;
                            
                        case 'W':
                        case 'w':
                            withdrawMoney(accountBalance);
                            savePortfolio(portfolioStocks, portfolioShares, numPortfolioStocks, userName, accountBalance, companies, prevPrices, highPrices, lowPrices, percentChanges, numCompanies);
                            break;
                            
                        case 'E':
                        case 'e':
                            inPortfolio = false;
                            break;
                    }
                }
                break;
            }
            
            case 'A':
            case 'a':
                addStockToPortfolio(portfolioStocks, portfolioShares, numPortfolioStocks, companies, numCompanies, accountBalance);
                savePortfolio(portfolioStocks, portfolioShares, numPortfolioStocks, userName, accountBalance, companies, prevPrices, highPrices, lowPrices, percentChanges, numCompanies);
                break;
                
            case 'R':
            case 'r':
                removeStockFromPortfolio(portfolioStocks, portfolioShares, numPortfolioStocks, companies, numCompanies, accountBalance);
                savePortfolio(portfolioStocks, portfolioShares, numPortfolioStocks, userName, accountBalance, companies, prevPrices, highPrices, lowPrices, percentChanges, numCompanies);
                break;
                
            case 'M':
            case 'm':
                addMoneyToAccount(accountBalance);
                savePortfolio(portfolioStocks, portfolioShares, numPortfolioStocks, userName, accountBalance, companies, prevPrices, highPrices, lowPrices, percentChanges, numCompanies);
                break;
                
            case 'W':
            case 'w':
                withdrawMoney(accountBalance);
                savePortfolio(portfolioStocks, portfolioShares, numPortfolioStocks, userName, accountBalance, companies, prevPrices, highPrices, lowPrices, percentChanges, numCompanies);
                break;
                
            case 'E':
            case 'e':
                running = false;
                break;
                
            case '\r': // Enter key - just refresh
                break;
        }
    }

    // Save final state before exiting
    saveDataToFile("companies.txt", companies, numCompanies);
    savePortfolio(portfolioStocks, portfolioShares, numPortfolioStocks, userName, accountBalance, companies, prevPrices, highPrices, lowPrices, percentChanges, numCompanies);
    
    clearScreen();
    cout << "\n=================================================\n";
    cout << "   Thank you for using Stock Market Manager!   \n";
    cout << "=================================================\n";
    cout << "\nYour portfolio and data have been saved.\n";
    cout << "Final Balance: $" << fixed << setprecision(2) << accountBalance << "\n\n";

    return 0;
}