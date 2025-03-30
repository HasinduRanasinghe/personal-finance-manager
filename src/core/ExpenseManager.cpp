#include "../../include/core/ExpenseManager.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <sstream>

ExpenseManager::ExpenseManager(const std::string& dataFilePath)
    : m_dataFilePath(dataFilePath), m_nextExpenseId(1)
{
    // Create directories if they don't exist
    fs::path filePath(dataFilePath);
    if (!filePath.parent_path().empty() && !fs::exists(filePath.parent_path())) {
        fs::create_directories(filePath.parent_path());
    }
    
    if (fs::exists(filePath)) {
        loadData();
    } else {
        initializeDefaultCategories();
        saveData();
    }
}

ExpenseManager::~ExpenseManager()
{
    saveData();
}

void ExpenseManager::initializeDefaultCategories()
{
    m_categories.push_back(Category("Food", "Groceries, restaurants, etc."));
    m_categories.push_back(Category("Housing", "Rent, mortgage, repairs"));
    m_categories.push_back(Category("Transportation", "Public transit, car expenses"));
    m_categories.push_back(Category("Utilities", "Electricity, water, internet"));
    m_categories.push_back(Category("Entertainment", "Movies, games, hobbies"));
    m_categories.push_back(Category("Health", "Medical expenses, insurance"));
    m_categories.push_back(Category("Personal", "Clothing, grooming"));
    m_categories.push_back(Category("Education", "Books, courses, tuition"));
    m_categories.push_back(Category("Miscellaneous", "Other expenses"));
}

int ExpenseManager::getNextExpenseId()
{
    return m_nextExpenseId++;
}

bool ExpenseManager::addExpense(const Expense& expense)
{
    Expense newExpense = expense;
    newExpense.id = getNextExpenseId();
    m_expenses.push_back(newExpense);
    return saveData();
}

bool ExpenseManager::updateExpense(int id, const Expense& expense)
{
    auto it = std::find_if(m_expenses.begin(), m_expenses.end(), 
                          [id](const Expense& e) { return e.id == id; });
    
    if (it != m_expenses.end()) {
        *it = expense;
        it->id = id;  // Preserve the original ID
        return saveData();
    }
    
    return false;
}

bool ExpenseManager::deleteExpense(int id)
{
    auto it = std::find_if(m_expenses.begin(), m_expenses.end(), 
                          [id](const Expense& e) { return e.id == id; });
    
    if (it != m_expenses.end()) {
        m_expenses.erase(it);
        return saveData();
    }
    
    return false;
}

std::vector<Expense> ExpenseManager::getAllExpenses() const
{
    return m_expenses;
}

std::vector<Expense> ExpenseManager::getExpensesByMonth(int year, int month) const
{
    std::vector<Expense> result;
    
    for (const auto& expense : m_expenses) {
        std::tm tm = {};
        std::istringstream ss(expense.date);
        ss >> std::get_time(&tm, "%Y-%m-%d");
        
        if (tm.tm_year + 1900 == year && tm.tm_mon + 1 == month) {
            result.push_back(expense);
        }
    }
    
    return result;
}

std::vector<Expense> ExpenseManager::getExpensesByCategory(const std::string& category) const
{
    std::vector<Expense> result;
    
    for (const auto& expense : m_expenses) {
        if (expense.category == category) {
            result.push_back(expense);
        }
    }
    
    return result;
}

bool ExpenseManager::addCategory(const Category& category)
{
    auto it = std::find_if(m_categories.begin(), m_categories.end(), 
                          [&category](const Category& c) { return c.name == category.name; });
    
    if (it == m_categories.end()) {
        m_categories.push_back(category);
        return saveData();
    }
    
    return false;
}

bool ExpenseManager::updateCategory(const std::string& name, const Category& category)
{
    auto it = std::find_if(m_categories.begin(), m_categories.end(), 
                          [&name](const Category& c) { return c.name == name; });
    
    if (it != m_categories.end()) {
        *it = category;
        return saveData();
    }
    
    return false;
}

bool ExpenseManager::deleteCategory(const std::string& name)
{
    auto it = std::find_if(m_categories.begin(), m_categories.end(), 
                          [&name](const Category& c) { return c.name == name; });
    
    if (it != m_categories.end()) {
        // Check if any expenses use this category
        bool categoryInUse = std::any_of(m_expenses.begin(), m_expenses.end(),
                                        [&name](const Expense& e) { return e.category == name; });
        
        if (!categoryInUse) {
            m_categories.erase(it);
            return saveData();
        }
    }
    
    return false;
}

std::vector<Category> ExpenseManager::getAllCategories() const
{
    return m_categories;
}

std::map<std::string, double> ExpenseManager::generateCategorySummary(int year, int month) const
{
    std::map<std::string, double> summary;
    
    // Initialize with all categories at 0
    for (const auto& category : m_categories) {
        summary[category.name] = 0.0;
    }
    
    // Sum up expenses for each category
    for (const auto& expense : getExpensesByMonth(year, month)) {
        summary[expense.category] += expense.amount;
    }
    
    return summary;
}

double ExpenseManager::getTotalExpenses(int year, int month) const
{
    double total = 0.0;
    
    for (const auto& expense : getExpensesByMonth(year, month)) {
        total += expense.amount;
    }
    
    return total;
}

bool ExpenseManager::saveData()
{
    try {
        json j;
        
        // Save expenses
        j["expenses"] = json::array();
        for (const auto& expense : m_expenses) {
            j["expenses"].push_back({
                {"id", expense.id},
                {"amount", expense.amount},
                {"description", expense.description},
                {"category", expense.category},
                {"date", expense.date}
            });
        }
        
        // Save categories
        j["categories"] = json::array();
        for (const auto& category : m_categories) {
            j["categories"].push_back({
                {"name", category.name},
                {"description", category.description}
            });
        }
        
        // Save next expense ID
        j["nextExpenseId"] = m_nextExpenseId;
        
        // Write to file
        std::ofstream file(m_dataFilePath);
        if (!file.is_open()) {
            return false;
        }
        
        file << std::setw(4) << j << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving data: " << e.what() << std::endl;
        return false;
    }
}

bool ExpenseManager::loadData()
{
    try {
        std::ifstream file(m_dataFilePath);
        if (!file.is_open()) {
            return false;
        }
        
        json j;
        file >> j;
        
        // Load expenses
        m_expenses.clear();
        for (const auto& expenseJson : j["expenses"]) {
            Expense expense;
            expense.id = expenseJson["id"].get<int>();
            expense.amount = expenseJson["amount"].get<double>();
            expense.description = expenseJson["description"].get<std::string>();
            expense.category = expenseJson["category"].get<std::string>();
            expense.date = expenseJson["date"].get<std::string>();
            m_expenses.push_back(expense);
        }
        
        // Load categories
        m_categories.clear();
        for (const auto& categoryJson : j["categories"]) {
            Category category;
            category.name = categoryJson["name"].get<std::string>();
            category.description = categoryJson["description"].get<std::string>();
            m_categories.push_back(category);
        }
        
        // Load next expense ID
        m_nextExpenseId = j["nextExpenseId"].get<int>();
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading data: " << e.what() << std::endl;
        
        // Initialize with default categories if loading fails
        initializeDefaultCategories();
        return false;
    }
}