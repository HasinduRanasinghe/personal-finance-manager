#include <QApplication>
#include <QMainWindow>
#include "ui/MainWindow.h"
#include "core/ExpenseManager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Initialize the expense manager with the data file path
    ExpenseManager expenseManager("expenses.json");
    
    // Create and show the main window
    MainWindow mainWindow(&expenseManager);
    mainWindow.show();
    
    return app.exec();
}