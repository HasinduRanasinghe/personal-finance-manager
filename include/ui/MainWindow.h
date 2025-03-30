#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QDateEdit>
#include <QComboBox>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QDialog>
#include <QChart>
#include <QChartView>
#include <QPieSeries>
#include "../core/ExpenseManager.h"

QT_CHARTS_USE_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    MainWindow(ExpenseManager* expenseManager, QWidget* parent = nullptr);
    ~MainWindow();
    
private slots:
    void addExpense();
    void editExpense();
    void deleteExpense();
    void manageCategories();
    void generateReport();
    void refreshData();
    void filterByMonth();
    
private:
    ExpenseManager* m_expenseManager;
    
    // UI components
    QTableWidget* m_expenseTable;
    QComboBox* m_categoryComboBox;
    QDoubleSpinBox* m_amountSpinBox;
    QLineEdit* m_descriptionEdit;
    QDateEdit* m_dateEdit;
    QPushButton* m_addButton;
    QPushButton* m_editButton;
    QPushButton* m_deleteButton;
    QPushButton* m_generateReportButton;
    QPushButton* m_manageCategoriesButton;
    QComboBox* m_monthComboBox;
    QComboBox* m_yearComboBox;
    QLabel* m_totalExpensesLabel;
    
    void setupUI();
    void updateCategoryComboBox();
    void updateExpenseTable(const std::vector<Expense>& expenses);
    int getSelectedExpenseId() const;
    void showReport(int year, int month);
    
    class CategoryDialog : public QDialog {
    public:
        CategoryDialog(ExpenseManager* manager, QWidget* parent = nullptr);
        
    private:
        ExpenseManager* m_manager;
        QTableWidget* m_categoryTable;
        QLineEdit* m_nameEdit;
        QLineEdit* m_descriptionEdit;
        QPushButton* m_addButton;
        QPushButton* m_updateButton;
        QPushButton* m_deleteButton;
        
        void setupUI();
        void refreshCategories();
        void addCategory();
        void updateCategory();
        void deleteCategory();
        void selectCategory(int row, int column);
    };
};

#endif // MAIN_WINDOW_H