#ifndef EXPENSE_H
#define EXPENSE_H

#include <string>

struct Expense {
    int id;
    double amount;
    std::string description;
    std::string category;
    std::string date;  // Format: YYYY-MM-DD
    
    Expense() : id(0), amount(0.0) {}
    
    Expense(int id, double amount, const std::string& description, 
            const std::string& category, const std::string& date)
        : id(id), amount(amount), description(description), category(category), date(date) {}
};

#endif // EXPENSE_H