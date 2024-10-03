//Muhammad
//22i-1612_B_Project

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <conio.h>

using namespace std;


//THis will read data from file called Companies.txt. Then store it into array
void readDataFromFile(const string& filename, string companies[38][3], int &numCompanies) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Failed to open the file." << endl;
        return;
    }

    string line;
    int row = 0;
    getline(file, line); 

    while (getline(file, line) && row < 38) {
        stringstream ss(line);
        string cell;
        int col = 0;

        while (getline(ss, cell, ',') && col < 3) {      //skip the commas
            companies[row][col] = cell;
            ++col;
        }

        while (getline(ss, cell, ',')) {
            companies[row][1] += "," + cell;
        }

        ++row;
    }

    numCompanies = row;
    file.close();
}

//Updates the file
void saveDataToFile(const string& filename, string companies[38][3], int numCompanies) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Failed to open the file for writing." << endl;
        return;
    }

    file << "Symbol,Company Name,Stock Price\n";

    for (int row = 0; row < numCompanies; ++row) {
        for (int col = 0; col < 3; ++col) {
            file << companies[row][col];
            if (col < 3 - 1) {
                file << ",";
            }
        }
        file << "\n";
    }

    file.close();
}

void displayStockMarketScreen(string companies[38][3], double prevPrices[38], double highPrices[38], double lowPrices[38], double percentChanges[38], int numCompanies, const string& topAdvancer, const string& topDecliner) {
    cout << "==========================================================================================================================\n";
    cout << "                                                Karachi Stock Market (Live)                         \n";
    cout << "==========================================================================================================================\n";
    cout << " Show updates: Enter     Show Portfolio: P     Add Stock: A     Remove Stock: R     Add money to account: M     Exit: E\n";
    cout << "==========================================================================================================================\n";
    cout << left << setw(10) << "Stocks" << setw(40) << "Company Name" << setw(15) << "Prev price" << setw(15) << "Curr Price" << setw(15) << "High" << setw(15) << "Low" << endl;
    cout << "--------------------------------------------------------------------------------------------------------------------------\n";
    
    for (int i = 0; i < numCompanies; ++i) {
        double currPrice = stod(companies[i][2]);
        cout << left << setw(10) << companies[i][0]
             << setw(40) << companies[i][1]
             << setw(15) << fixed << setprecision(2) << prevPrices[i];
        
        if (currPrice > prevPrices[i]) {
            cout << "\033[32m" << setw(15) << currPrice << "▲\033[0m"; // green up arrow
        } else if (currPrice < prevPrices[i]) {
            cout << "\033[31m" << setw(15) << currPrice << "▼\033[0m"; // red down arrow
        } else {
            cout << setw(15) << currPrice;
        }
        
        cout << setw(15) << fixed << setprecision(2) << highPrices[i]
             << setw(15) << fixed << setprecision(2) << lowPrices[i]
             << endl;
    }
    cout << "\nTop % advancer symbol: " << topAdvancer << endl;
    cout << "Top % decliner symbol: " << topDecliner << endl;
}

void updatePrices(string companies[38][3], double prevPrices[38], double highPrices[38], double lowPrices[38], double percentChanges[38], int numCompanies) {
    for (int i = 0; i < numCompanies; ++i) {
        double oldPrice = stod(companies[i][2]);
        double maxChange = oldPrice * 0.15;
        double change = ((rand() % 2001 - 1000) / 1000.0) * maxChange; // Random change between -maxChange and +maxChange
        double newPrice = oldPrice + change;
        
        if (newPrice < 0) newPrice = 0; // Ensure price does not go negative
        
        prevPrices[i] = oldPrice;
        companies[i][2] = to_string(newPrice);

        // Update high and low prices
        if (newPrice > highPrices[i]) {
            highPrices[i] = newPrice;
        }
        if (newPrice < lowPrices[i]) {
            lowPrices[i] = newPrice;
        }

        // Calculate percentage change
        percentChanges[i] = ((newPrice - oldPrice) / oldPrice) * 100;
    }
}

void findTopAdvancerAndDecliner(string companies[38][3], double percentChanges[38], int numCompanies, string &topAdvancer, string &topDecliner) {
    double maxIncrease = -100.0;
    double maxDecrease = 100.0;
    int topAdvancerIndex = -1;
    int topDeclinerIndex = -1;

    for (int i = 0; i < numCompanies; ++i) {
        if (percentChanges[i] > maxIncrease) {
            maxIncrease = percentChanges[i];
            topAdvancerIndex = i;
        }
        if (percentChanges[i] < maxDecrease) {
            maxDecrease = percentChanges[i];
            topDeclinerIndex = i;
        }
    }

    if (topAdvancerIndex != -1) {
        topAdvancer = companies[topAdvancerIndex][0];
    }
    if (topDeclinerIndex != -1) {
        topDecliner = companies[topDeclinerIndex][0];
    }
}

void loadPortfolio(string portfolioStocks[50][3], int portfolioShares[50], int &numPortfolioStocks, string &userName, double &accountBalance, double prevPrices[38], string companies[38][3], int numCompanies) {
    ifstream file("portfolio.txt");
    if (file.is_open()) {
        file >> userName >> accountBalance;
        file.ignore(); // Ignore newline after balance

        string line;
        numPortfolioStocks = 0;

        // Skip header line
        getline(file, line);

        while (getline(file, line) && numPortfolioStocks < 50) {
            stringstream ss(line);
            string cell;

            getline(ss, portfolioStocks[numPortfolioStocks][0], ','); // Stocks
            getline(ss, portfolioStocks[numPortfolioStocks][1], ','); // Company Name
            getline(ss, cell, ','); // Shares
            portfolioShares[numPortfolioStocks] = stoi(cell);

            // Skip additional columns
            getline(ss, cell, ','); // Curr Price
            getline(ss, cell, ','); // Prev Price
            getline(ss, cell, ','); // High
            getline(ss, cell, ','); // Low
            getline(ss, cell, ','); // Gain/Loss

            ++numPortfolioStocks;
        }
        file.close();
    } else {
        ofstream newFile("portfolio.txt");
        if (newFile.is_open()) {
            cout << "Enter your name: ";
            cin >> userName;
            cout << "Enter your initial account balance: ";
            cin >> accountBalance;
            newFile << userName << " " << accountBalance << "\n";
            newFile.close();

            // Initialize prevPrices with current prices
            for (int i = 0; i < numCompanies; ++i) {
                prevPrices[i] = stod(companies[i][2]);
            }
        } else {
            cerr << "Failed to create the portfolio file." << endl;
        }
    }
}


void savePortfolio(string portfolioStocks[50][3], int portfolioShares[50], int numPortfolioStocks, const string &userName, double accountBalance, string companies[38][3], double prevPrices[38], double highPrices[38], double lowPrices[38], double percentChanges[38], int numCompanies) {
    ofstream file("portfolio.txt");
    if (!file.is_open()) {
        cerr << "Failed to open the file for writing." << endl;
        return;
    }

    file << "Portfolio owner: " << userName << "\n\n";
    file << left << setw(10) << "Stocks" << setw(40) << "Company Name" << setw(10) << "Shares" << setw(10) << "Current" << setw(10) << "Previous" << setw(10) << "Gain/Loss" << setw(10) << "High" << setw(10) << "Low" << "\n";
    file << string(150, '*') << "\n";

    double totalGainLoss = 0.0;

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
    file << "Current Balance: " << fixed << setprecision(2) << accountBalance << "\n";

    file.close();
}

void addStockToPortfolio(string portfolioStocks[50][3], int portfolioShares[50], int &numPortfolioStocks, string companies[38][3], int numCompanies, double &accountBalance) {
    string stockSymbol;
    int shares;

    cout << "Enter the stock symbol to add: ";
    cin >> stockSymbol;
    cout << "Enter the number of shares: ";
    cin >> shares;

    bool stockFound = false;

    for (int i = 0; i < numCompanies; ++i) {
        if (companies[i][0] == stockSymbol) {
            stockFound = true;
            double stockPrice = stod(companies[i][2]);
            double totalCost = shares * stockPrice;

            if (accountBalance >= totalCost) {
                bool alreadyInPortfolio = false;

                for (int j = 0; j < numPortfolioStocks; ++j) {
                    if (portfolioStocks[j][0] == stockSymbol) {
                        portfolioShares[j] += shares;
                        alreadyInPortfolio = true;
                        break;
                    }
                }

                if (!alreadyInPortfolio) {
                    portfolioStocks[numPortfolioStocks][0] = stockSymbol;
                    portfolioStocks[numPortfolioStocks][1] = companies[i][1];
                    portfolioShares[numPortfolioStocks] = shares;
                    ++numPortfolioStocks;
                }

                accountBalance -= totalCost;
                cout << "Stock added to portfolio. Total cost: " << totalCost << endl;
            } else {
                cout << "Insufficient balance to buy the shares." << endl;
            }
            break;
        }
    }

    if (!stockFound) {
        cout << "Stock symbol not found in the market." << endl;
    }
}
void removeStockFromPortfolio(string portfolioStocks[50][3], int portfolioShares[50], int &numPortfolioStocks, string companies[38][3], int numCompanies, double &accountBalance) {
    string stockSymbol;

    cout << "Enter the stock symbol to remove: ";
    cin >> stockSymbol;

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
            
            // Calculate the amount to add to the balance
            double totalValue = stockPrice * portfolioShares[i];
            accountBalance += totalValue;
            cout << "Stock removed from portfolio. Total value added to balance: " << totalValue << endl;

            // Remove the stock from the portfolio
            for (int j = i; j < numPortfolioStocks - 1; ++j) {
                portfolioStocks[j][0] = portfolioStocks[j + 1][0];
                portfolioStocks[j][1] = portfolioStocks[j + 1][1];
                portfolioShares[j] = portfolioShares[j + 1];
            }

            --numPortfolioStocks;
            break;
        }
    }

    if (!stockFound) {
        cout << "Stock symbol not found in your portfolio." << endl;
    }
}
void writeportfolio(string portfolioStocks[50][3], int portfolioShares[50], int numPortfolioStocks, string companies[38][3], double prevPrices[38], double highPrices[38], double lowPrices[38], double percentChanges[38], int numCompanies) {
    ofstream file("portfolio.txt");
    if (file.is_open()) {
        file << "Portfolio owner: UserName" << endl;
        file << left << setw(10) << "Stocks" << setw(40) << "Company Name" << setw(10) << "Shares" << setw(10) << "Current" << setw(10) << "Previous" << setw(10) << "Gain/Loss" << setw(10) << "High" << setw(10) << "Low" << endl;
        file << "-----------------------------------------------------------------------------------------------------" << endl;

        double totalGainLoss = 0.0;

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
                << endl;
        }

        file << "Total Gain/Loss: " << fixed << setprecision(2) << totalGainLoss << endl;
        file << "Current Balance: " << fixed << setprecision(2) << "Balance" << endl;

        file.close();
    } else {
        cout << "Failed to open the file for writing." << endl;
    }
}
void displayPortfolio(string portfolioStocks[50][3], int portfolioShares[50], int numPortfolioStocks, const string& userName, double accountBalance, string companies[38][3], double prevPrices[38], double highPrices[38], double lowPrices[38], int numCompanies) {
    cout << "==========================================================================================================================\n";
    cout << "                                               Portfolio of " << userName << "                                        \n";
    cout << "==========================================================================================================================\n";
    cout << left << setw(10) << "Stocks" << setw(40) << "Company Name" << setw(10) << "Shares" << setw(15) << "Curr Price" << setw(15) << "Prev Price" << setw(15) << "High" << setw(15) << "Low" << setw(15) << "Gain/Loss" << endl;
    cout << "--------------------------------------------------------------------------------------------------------------------------\n";
    
    double totalGainLoss = 0.0;

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
                break;
            }
        }

        cout << left << setw(10) << stockSymbol
             << setw(40) << companyName
             << setw(10) << shares
             << setw(15) << fixed << setprecision(2) << currPrice
             << setw(15) << fixed << setprecision(2) << prevPrice
             << setw(15) << fixed << setprecision(2) << highPrice
             << setw(15) << fixed << setprecision(2) << lowPrice

             << setw(15) << fixed << setprecision(2) << gainLoss
             << endl;
    }

    cout << "\nTotal Gain/Loss: " << fixed << setprecision(2) << totalGainLoss << endl;
    cout << "Current Balance: " << fixed << setprecision(2) << accountBalance << endl;
}


void addMoneyToAccount(double &accountBalance) {
    double amount;
    cout << "Enter amount to add to your account: ";
    cin >> amount;
    accountBalance += amount;
    cout << "Successfully added $" << amount << " to your account. New balance: $" << fixed << setprecision(2) << accountBalance << "\n";
}

void removemoney(double &accountBalance) {
    double amount;
    cout << "Enter amount to remove from your account: ";
    cin >> amount;
    if(amount<=accountBalance){
            accountBalance -= amount;
             cout << "Successfully removed $" << amount << " to your account. New balance: $" << fixed << setprecision(2) << accountBalance << "\n";
    }
    else cout<< "Insuff. Funds" <<endl;
    
}

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));

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
    double accountBalance;

    readDataFromFile("companies.txt", companies, numCompanies);
      
    // Initialize highPrices and lowPrices with current prices
    for (int i = 0; i < numCompanies; ++i) {
        highPrices[i] = stod(companies[i][2]);
        lowPrices[i] = stod(companies[i][2]);
    }
    
    loadPortfolio(portfolioStocks, portfolioShares, numPortfolioStocks, userName, accountBalance, prevPrices, companies, numCompanies);

    while (true) {
        string topAdvancer, topDecliner;
        updatePrices(companies, prevPrices, highPrices, lowPrices, percentChanges, numCompanies);
        findTopAdvancerAndDecliner(companies, percentChanges, numCompanies, topAdvancer, topDecliner);
        displayStockMarketScreen(companies, prevPrices, highPrices, lowPrices, percentChanges, numCompanies, topAdvancer, topDecliner);
        
        savePortfolio(portfolioStocks, portfolioShares, numPortfolioStocks, userName, accountBalance, companies, prevPrices, highPrices, lowPrices, percentChanges, numCompanies);
        char input = _getch();

        if (input == 'P' || input == 'p') {
            while (true) {
                displayPortfolio(portfolioStocks, portfolioShares, numPortfolioStocks, userName, accountBalance, companies, prevPrices, highPrices, lowPrices, numCompanies);
                
                char portfolioInput = _getch();

                if (portfolioInput == '\r') {  // Check for ENTER key press
                    // Update stock prices and portfolio screen
                    updatePrices(companies, prevPrices, highPrices, lowPrices, percentChanges, numCompanies);
                    savePortfolio(portfolioStocks, portfolioShares, numPortfolioStocks, userName, accountBalance, companies, prevPrices, highPrices, lowPrices, percentChanges, numCompanies);
                } else if (portfolioInput == 'E' || portfolioInput == 'e') {  // Exit Portfolio screen
                    break;
                }
                else if (portfolioInput == 'A' || portfolioInput == 'a') {
                     addStockToPortfolio(portfolioStocks, portfolioShares, numPortfolioStocks, companies, numCompanies, accountBalance);
                     savePortfolio(portfolioStocks, portfolioShares, numPortfolioStocks, userName, accountBalance, companies, prevPrices, highPrices, lowPrices, percentChanges, numCompanies);
        } else if (portfolioInput == 'R' || portfolioInput == 'r') {
                     removeStockFromPortfolio(portfolioStocks, portfolioShares, numPortfolioStocks, companies, numCompanies, accountBalance);
                         savePortfolio(portfolioStocks, portfolioShares, numPortfolioStocks, userName, accountBalance, companies, prevPrices, highPrices, lowPrices, percentChanges, numCompanies);
        } else if (portfolioInput == 'M' || portfolioInput == 'm') {
                     addMoneyToAccount(accountBalance);
                     savePortfolio(portfolioStocks, portfolioShares, numPortfolioStocks, userName, accountBalance, companies, prevPrices, highPrices, lowPrices, percentChanges, numCompanies);
        } else if (portfolioInput == 'W' || portfolioInput == 'w') {
                    removemoney(accountBalance);
                    savePortfolio(portfolioStocks, portfolioShares, numPortfolioStocks, userName, accountBalance, companies, prevPrices, highPrices, lowPrices, percentChanges, numCompanies);
        } else if (input == 'E' || input == 'e') {
            break;
        }
            }
        } else if (input == 'A' || input == 'a') {
            addStockToPortfolio(portfolioStocks, portfolioShares, numPortfolioStocks, companies, numCompanies, accountBalance);
            savePortfolio(portfolioStocks, portfolioShares, numPortfolioStocks, userName, accountBalance, companies, prevPrices, highPrices, lowPrices, percentChanges, numCompanies);
        } else if (input == 'R' || input == 'r') {
            removeStockFromPortfolio(portfolioStocks, portfolioShares, numPortfolioStocks, companies, numCompanies, accountBalance);
            savePortfolio(portfolioStocks, portfolioShares, numPortfolioStocks, userName, accountBalance, companies, prevPrices, highPrices, lowPrices, percentChanges, numCompanies);
        } else if (input == 'M' || input == 'm') {
            addMoneyToAccount(accountBalance);
        } else if (input == 'W' || input == 'w') {
            removemoney(accountBalance);
        } else if (input == 'E' || input == 'e') {
            break;
        }
    }

    saveDataToFile("companies.txt", companies, numCompanies);

    return 0;
}