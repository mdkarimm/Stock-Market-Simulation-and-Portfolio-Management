# Stock Market Simulation and Portfolio Management

## Project Overview

This project is a C++ application that simulates a live stock market environment. It reads stock data from a file, displays real-time fluctuations in stock prices, and allows users to manage a personal portfolio. The goal of this project was to simulate a stock market where users can buy, sell, and track stocks, as well as manage account balances.

## Features

- **Real-time stock price updates:** Prices fluctuate randomly within a predefined limit per session.
- **Portfolio management:** Users can buy/sell stocks and track their profit/loss.
- **Data persistence:** Stock data and user portfolio are saved at the end of each session and reloaded at the start of a new one.
- **Input validation:** Prevents invalid stock symbols, transactions, or balance actions.
- **File I/O operations:** Stock data and portfolio data are read from and written to files to ensure persistence.

## Technologies Used

- **C++** 
  - Arrays, loops, functions, strings, random number generation.
  - File handling (read/write).
  - Data structures (2D arrays for stock data, parallel arrays for portfolios).
