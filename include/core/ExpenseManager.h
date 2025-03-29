#ifndef EXPENSE_MANAGER_H
#define EXPENSE_MANAGER_H

#include <string>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include <filesystem>
#include "Expense.h"
#include "Category.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

class ExpenseManager {
public:
    ExpenseManager(const std::string& dataFilePath);
    ~ExpenseManager();
    
    // Expense operations
    bool addExpense(const Expense& expense);
    bool updateExpense(int id, const Expense& expense);
    bool deleteExpense(int id);
    std::vector<Expense> getAllExpenses() const;
    std::vector<Expense> getExpensesByMonth(int year, int month) const;
    std::vector<Expense> getExpensesByCategory(const std::string& category) const;
    
    // Category operations
    bool addCategory(const Category& category);
    bool updateCategory(const std::string& name, const Category& category);
    bool deleteCategory(const std::string& name);
    std::vector<Category> getAllCategories() const;
    
    // Reporting
    std::map<std::string, double> generateCategorySummary(int year, int month) const;
    double getTotalExpenses(int year, int month) const;
    
    // Save and load data
    bool saveData();
    bool loadData();
    
private:
    std::string m_dataFilePath;
    std::vector<Expense> m_expenses;
    std::vector<Category> m_categories;
    int m_nextExpenseId;
    
    void initializeDefaultCategories();
    int getNextExpenseId();
};

#endif // EXPENSE_MANAGER_H