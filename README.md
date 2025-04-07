# Personal Finance Manager

A C++ desktop application for tracking personal expenses and generating financial reports.

## Features

- Log expenses with category, amount, description, and date
- Generate monthly expense reports with charts and summaries
- Category management system with customizable expense categories
- View and filter expenses by month and year
- Visualize spending patterns with interactive charts
- Data persistence using JSON file storage

## Technologies Used

- **C++17**: Modern C++ features for robust application architecture
- **Qt 5**: Cross-platform GUI framework for desktop applications
- **nlohmann/json**: JSON parsing library for data storage
- **std::filesystem**: C++17 filesystem operations for file handling
- **Qt Charts**: Data visualization components

## Architecture

The application follows a modular architecture with a clear separation of concerns:

### Core Components

- **Expense**: Data structure for expense entries
- **Category**: Data structure for expense categories
- **ExpenseManager**: Business logic for managing expenses and categories

### UI Components

- **MainWindow**: Main application window with expense table and input form
- **CategoryDialog**: Dialog for managing expense categories
- **ReportDialog**: Dialog for displaying expense reports with charts

![expense_manager](https://github.com/user-attachments/assets/96f4240d-b776-44e8-9dfb-586380a20ddd)

![category_managing](https://github.com/user-attachments/assets/7f009d07-363f-450b-8772-d6c993784abd)

![report_generation](https://github.com/user-attachments/assets/22116301-1eeb-4389-9a9e-8e1a7e51ddcb)


## Installation

### Requirements

- C++17 compatible compiler (GCC 8+, Clang 7+, MSVC 2019+)
- Qt 5.12 or higher
- CMake 3.10 or higher

### Building from Source

1. Clone the repository
```bash
git clone https://github.com/HasinduRanasinghe/personal-finance-manager.git
cd personal-finance-manager
```

2. Create a build directory
```bash
mkdir build
cd build
```

3. Configure with CMake
```bash
cmake ..
```

4. Build the project
```bash
cmake --build .
```

5. Run the application
```bash
./PersonalFinanceManager
```

## Usage Guide

### Adding Expenses

1. Select a category from the dropdown
2. Enter the amount
3. Add a description (optional)
4. Select the date
5. Click "Add Expense"

### Editing Expenses

1. Select an expense from the table
2. The selected expense details will appear in the form
3. Make your changes
4. Click "Edit Selected"

### Filtering Expenses

1. Select the year and month from the dropdowns
2. Click "Apply Filter"

### Generating Reports

1. Select the year and month for the report
2. Click "Generate Report"
3. A new window will open showing a pie chart of expenses by category
4. The report can be exported to CSV using the "Export Report" button

### Managing Categories

1. Click "Manage Categories"
2. Add, edit, or delete expense categories
- Note: Categories in use by expenses cannot be deleted

## Data Storage

The application stores all expense and category data in a JSON file (`expenses.json`) in the application directory. The file is automatically created on first run and updated when changes are made.
