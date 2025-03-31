#include "../../include/ui/MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QHeaderView>
#include <QBarSeries>
#include <QBarSet>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QDateTime>
#include <QFileDialog>
#include <QTextStream>


MainWindow::MainWindow(ExpenseManager* expenseManager, QWidget* parent)
    : QMainWindow(parent), m_expenseManager(expenseManager)
{
    setWindowTitle("Personal Finance Manager");
    setMinimumSize(800, 600);
    
    setupUI();
    updateCategoryComboBox();
    refreshData();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    // Create central widget and main layout
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    // Create filter controls
    QGroupBox* filterGroupBox = new QGroupBox("Filter Expenses");
    QHBoxLayout* filterLayout = new QHBoxLayout(filterGroupBox);
    
    m_yearComboBox = new QComboBox();
    QDateTime currentDate = QDateTime::currentDateTime();
    int currentYear = currentDate.date().year();
    for (int year = currentYear - 5; year <= currentYear + 1; ++year) {
        m_yearComboBox->addItem(QString::number(year), year);
    }
    m_yearComboBox->setCurrentText(QString::number(currentYear));
    
    m_monthComboBox = new QComboBox();
    for (int month = 1; month <= 12; ++month) {
        QDate date(2000, month, 1);
        m_monthComboBox->addItem(date.toString("MMMM"), month);
    }
    m_monthComboBox->setCurrentIndex(currentDate.date().month() - 1);
    
    QPushButton* filterButton = new QPushButton("Apply Filter");
    
    filterLayout->addWidget(new QLabel("Year:"));
    filterLayout->addWidget(m_yearComboBox);
    filterLayout->addWidget(new QLabel("Month:"));
    filterLayout->addWidget(m_monthComboBox);
    filterLayout->addWidget(filterButton);
    filterLayout->addStretch();
    
    m_totalExpensesLabel = new QLabel("Total: $0.00");
    filterLayout->addWidget(m_totalExpensesLabel);
    
    // Create expense table
    m_expenseTable = new QTableWidget();
    m_expenseTable->setColumnCount(5);
    m_expenseTable->setHorizontalHeaderLabels({"ID", "Date", "Category", "Description", "Amount"});
    m_expenseTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_expenseTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_expenseTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_expenseTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    m_expenseTable->verticalHeader()->setVisible(false);
    
    // Create input form
    QGroupBox* inputGroupBox = new QGroupBox("Add New Expense");
    QFormLayout* formLayout = new QFormLayout(inputGroupBox);
    
    m_categoryComboBox = new QComboBox();
    m_amountSpinBox = new QDoubleSpinBox();
    m_amountSpinBox->setRange(0.01, 1000000.00);
    m_amountSpinBox->setValue(0.00);
    m_amountSpinBox->setPrefix("$");
    m_amountSpinBox->setDecimals(2);
    
    m_descriptionEdit = new QLineEdit();
    m_dateEdit = new QDateEdit(QDate::currentDate());
    m_dateEdit->setCalendarPopup(true);
    m_dateEdit->setDisplayFormat("yyyy-MM-dd");
    
    formLayout->addRow("Category:", m_categoryComboBox);
    formLayout->addRow("Amount:", m_amountSpinBox);
    formLayout->addRow("Description:", m_descriptionEdit);
    formLayout->addRow("Date:", m_dateEdit);
    
    // Create buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    m_addButton = new QPushButton("Add Expense");
    m_editButton = new QPushButton("Edit Selected");
    m_deleteButton = new QPushButton("Delete Selected");
    m_generateReportButton = new QPushButton("Generate Report");
    m_manageCategoriesButton = new QPushButton("Manage Categories");
    
    buttonLayout->addWidget(m_addButton);
    buttonLayout->addWidget(m_editButton);
    buttonLayout->addWidget(m_deleteButton);
    buttonLayout->addWidget(m_generateReportButton);
    buttonLayout->addWidget(m_manageCategoriesButton);
    
    // Add all widgets to main layout
    mainLayout->addWidget(filterGroupBox);
    mainLayout->addWidget(m_expenseTable);
    mainLayout->addWidget(inputGroupBox);
    mainLayout->addLayout(buttonLayout);
    
    setCentralWidget(centralWidget);
    
    // Connect signals and slots
    connect(m_addButton, &QPushButton::clicked, this, &MainWindow::addExpense);
    connect(m_editButton, &QPushButton::clicked, this, &MainWindow::editExpense);
    connect(m_deleteButton, &QPushButton::clicked, this, &MainWindow::deleteExpense);
    connect(m_generateReportButton, &QPushButton::clicked, this, &MainWindow::generateReport);
    connect(m_manageCategoriesButton, &QPushButton::clicked, this, &MainWindow::manageCategories);
    connect(filterButton, &QPushButton::clicked, this, &MainWindow::filterByMonth);
    connect(m_expenseTable, &QTableWidget::cellDoubleClicked, this, &MainWindow::editExpense);
}

void MainWindow::updateCategoryComboBox()
{
    m_categoryComboBox->clear();
    
    for (const auto& category : m_expenseManager->getAllCategories()) {
        m_categoryComboBox->addItem(QString::fromStdString(category.name));
    }
}

void MainWindow::updateExpenseTable(const std::vector<Expense>& expenses)
{
    m_expenseTable->setRowCount(0);
    
    for (const auto& expense : expenses) {
        int row = m_expenseTable->rowCount();
        m_expenseTable->insertRow(row);
        
        m_expenseTable->setItem(row, 0, new QTableWidgetItem(QString::number(expense.id)));
        m_expenseTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(expense.date)));
        m_expenseTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(expense.category)));
        m_expenseTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(expense.description)));
        
        QTableWidgetItem* amountItem = new QTableWidgetItem(QString("$%1").arg(expense.amount, 0, 'f', 2));
        amountItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m_expenseTable->setItem(row, 4, amountItem);
    }
    
    // Calculate and display total
    int year = m_yearComboBox->currentData().toInt();
    int month = m_monthComboBox->currentData().toInt();
    double total = m_expenseManager->getTotalExpenses(year, month);
    m_totalExpensesLabel->setText(QString("Total: $%1").arg(total, 0, 'f', 2));
}

int MainWindow::getSelectedExpenseId() const
{
    QList<QTableWidgetItem*> selectedItems = m_expenseTable->selectedItems();
    if (selectedItems.isEmpty()) {
        return -1;
    }
    
    int row = selectedItems.first()->row();
    return m_expenseTable->item(row, 0)->text().toInt();
}

void MainWindow::addExpense()
{
    if (m_categoryComboBox->currentText().isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please select a category.");
        return;
    }
    
    if (m_amountSpinBox->value() <= 0) {
        QMessageBox::warning(this, "Warning", "Please enter a valid amount.");
        return;
    }
    
    Expense expense;
    expense.amount = m_amountSpinBox->value();
    expense.description = m_descriptionEdit->text().toStdString();
    expense.category = m_categoryComboBox->currentText().toStdString();
    expense.date = m_dateEdit->date().toString("yyyy-MM-dd").toStdString();
    
    if (m_expenseManager->addExpense(expense)) {
        refreshData();
        
        // Reset form fields
        m_amountSpinBox->setValue(0.0);
        m_descriptionEdit->clear();
    } else {
        QMessageBox::critical(this, "Error", "Failed to add expense.");
    }
}

void MainWindow::editExpense()
{
    int expenseId = getSelectedExpenseId();
    if (expenseId < 0) {
        QMessageBox::warning(this, "Warning", "Please select an expense to edit.");
        return;
    }
    
    // Find the expense in the list
    std::vector<Expense> expenses = m_expenseManager->getAllExpenses();
    auto it = std::find_if(expenses.begin(), expenses.end(),
                         [expenseId](const Expense& e) { return e.id == expenseId; });
    
    if (it != expenses.end()) {
        // Populate form with expense data
        m_amountSpinBox->setValue(it->amount);
        m_descriptionEdit->setText(QString::fromStdString(it->description));
        m_categoryComboBox->setCurrentText(QString::fromStdString(it->category));
        m_dateEdit->setDate(QDate::fromString(QString::fromStdString(it->date), "yyyy-MM-dd"));
        
        // Ask for confirmation
        if (QMessageBox::question(this, "Edit Expense", 
                                "Update this expense with the new values?",
                                QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            Expense updatedExpense = *it;
            updatedExpense.amount = m_amountSpinBox->value();
            updatedExpense.description = m_descriptionEdit->text().toStdString();
            updatedExpense.category = m_categoryComboBox->currentText().toStdString();
            updatedExpense.date = m_dateEdit->date().toString("yyyy-MM-dd").toStdString();
            
            if (m_expenseManager->updateExpense(expenseId, updatedExpense)) {
                refreshData();
                
                // Reset form fields
                m_amountSpinBox->setValue(0.0);
                m_descriptionEdit->clear();
            } else {
                QMessageBox::critical(this, "Error", "Failed to update expense.");
            }
        }
    }
}

void MainWindow::deleteExpense()
{
    int expenseId = getSelectedExpenseId();
    if (expenseId < 0) {
        QMessageBox::warning(this, "Warning", "Please select an expense to delete.");
        return;
    }
    
    if (QMessageBox::question(this, "Delete Expense", 
                            "Are you sure you want to delete this expense?",
                            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        if (m_expenseManager->deleteExpense(expenseId)) {
            refreshData();
        } else {
            QMessageBox::critical(this, "Error", "Failed to delete expense.");
        }
    }
}

void MainWindow::manageCategories()
{
    CategoryDialog dialog(m_expenseManager, this);
    dialog.exec();
    
    // Update the category combo box after managing categories
    updateCategoryComboBox();
}

void MainWindow::generateReport()
{
    int year = m_yearComboBox->currentData().toInt();
    int month = m_monthComboBox->currentData().toInt();
    
    showReport(year, month);
}

void MainWindow::refreshData()
{
    filterByMonth();
}

void MainWindow::filterByMonth()
{
    int year = m_yearComboBox->currentData().toInt();
    int month = m_monthComboBox->currentData().toInt();
    
    std::vector<Expense> expenses = m_expenseManager->getExpensesByMonth(year, month);
    updateExpenseTable(expenses);
}

void MainWindow::showReport(int year, int month)
{
    QDialog* reportDialog = new QDialog(this);
    reportDialog->setWindowTitle(QString("Expense Report - %1 %2")
                               .arg(m_monthComboBox->currentText())
                               .arg(year));
    reportDialog->setMinimumSize(600, 400);
    
    QVBoxLayout* layout = new QVBoxLayout(reportDialog);
    
    // Create chart
    QChart* chart = new QChart();
    chart->setTitle("Expenses by Category");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    
    // Add pie series
    QPieSeries* series = new QPieSeries();
    
    // Get category summary
    std::map<std::string, double> categorySummary = 
        m_expenseManager->generateCategorySummary(year, month);
    
    // Add slices and calculate total
    double total = 0.0;
    for (const auto& entry : categorySummary) {
        if (entry.second > 0) {
            series->append(QString::fromStdString(entry.first), entry.second);
            total += entry.second;
        }
    }
    
    // Set slice labels to show percentage
    for (QPieSlice* slice : series->slices()) {
        double percentage = (slice->value() / total) * 100.0;
        slice->setLabel(QString("%1: $%2 (%3%)")
                       .arg(slice->label())
                       .arg(slice->value(), 0, 'f', 2)
                       .arg(percentage, 0, 'f', 1));
        slice->setLabelVisible(true);
    }
    
    chart->addSeries(series);
    
    // Create chart view
    QChartView* chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    
    // Create summary table
    QTableWidget* summaryTable = new QTableWidget();
    summaryTable->setColumnCount(2);
    summaryTable->setHorizontalHeaderLabels({"Category", "Amount"});
    summaryTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    summaryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    summaryTable->verticalHeader()->setVisible(false);
    
    int row = 0;
    for (const auto& entry : categorySummary) {
        if (entry.second > 0) {
            summaryTable->insertRow(row);
            summaryTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(entry.first)));
            QTableWidgetItem* amountItem = new QTableWidgetItem(QString("$%1").arg(entry.second, 0, 'f', 2));
            amountItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            summaryTable->setItem(row, 1, amountItem);
            row++;
        }
    }
    
    // Add total row
    summaryTable->insertRow(row);
    QTableWidgetItem* totalLabelItem = new QTableWidgetItem("TOTAL");
    totalLabelItem->setFont(QFont("", -1, QFont::Bold));
    summaryTable->setItem(row, 0, totalLabelItem);
    
    QTableWidgetItem* totalAmountItem = new QTableWidgetItem(QString("$%1").arg(total, 0, 'f', 2));
    totalAmountItem->setFont(QFont("", -1, QFont::Bold));
    totalAmountItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    summaryTable->setItem(row, 1, totalAmountItem);
    
    // Add export button
    QPushButton* exportButton = new QPushButton("Export Report");
    
    connect(exportButton, &QPushButton::clicked, [=]() {
        QString fileName = QFileDialog::getSaveFileName(reportDialog, "Export Report",
                                                       QString(), "CSV Files (*.csv)");
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream stream(&file);
                stream << "Category,Amount\n";
                
                for (const auto& entry : categorySummary) {
                    if (entry.second > 0) {
                        stream << QString::fromStdString(entry.first) << ","
                               << QString::number(entry.second, 'f', 2) << "\n";
                    }
                }
                
                stream << "TOTAL," << QString::number(total, 'f', 2) << "\n";
                file.close();
                
                QMessageBox::information(reportDialog, "Export Successful",
                                       "Report has been exported successfully.");
            } else {
                QMessageBox::critical(reportDialog, "Export Failed",
                                    "Failed to open file for writing.");
            }
        }
    });
    
    // Add widgets to layout
    layout->addWidget(chartView);
    layout->addWidget(summaryTable);
    layout->addWidget(exportButton);
    
    reportDialog->setLayout(layout);
    reportDialog->exec();
}

// CategoryDialog implementation
MainWindow::CategoryDialog::CategoryDialog(ExpenseManager* manager, QWidget* parent)
    : QDialog(parent), m_manager(manager)
{
    setWindowTitle("Manage Categories");
    setMinimumSize(500, 300);
    
    setupUI();
    refreshCategories();
}

void MainWindow::CategoryDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Create category table
    m_categoryTable = new QTableWidget();
    m_categoryTable->setColumnCount(2);
    m_categoryTable->setHorizontalHeaderLabels({"Name", "Description"});
    m_categoryTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_categoryTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_categoryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_categoryTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_categoryTable->verticalHeader()->setVisible(false);
    
    // Create form for adding/editing categories
    QGroupBox* inputGroupBox = new QGroupBox("Add/Edit Category");
    QFormLayout* formLayout = new QFormLayout(inputGroupBox);
    
    m_nameEdit = new QLineEdit();
    m_descriptionEdit = new QLineEdit();
    
    formLayout->addRow("Name:", m_nameEdit);
    formLayout->addRow("Description:", m_descriptionEdit);
    
    // Create buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    m_addButton = new QPushButton("Add");
    m_updateButton = new QPushButton("Update");
    m_deleteButton = new QPushButton("Delete");
    
    buttonLayout->addWidget(m_addButton);
    buttonLayout->addWidget(m_updateButton);
    buttonLayout->addWidget(m_deleteButton);
    
    // Close button
    QPushButton* closeButton = new QPushButton("Close");
    
    // Add all widgets to main layout
    mainLayout->addWidget(m_categoryTable);
    mainLayout->addWidget(inputGroupBox);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(closeButton);
    
    // Connect signals and slots
    connect(m_addButton, &QPushButton::clicked, this, &CategoryDialog::addCategory);
    connect(m_updateButton, &QPushButton::clicked, this, &CategoryDialog::updateCategory);
    connect(m_deleteButton, &QPushButton::clicked, this, &CategoryDialog::deleteCategory);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_categoryTable, &QTableWidget::cellClicked, this, &CategoryDialog::selectCategory);
}

void MainWindow::CategoryDialog::refreshCategories()
{
    m_categoryTable->setRowCount(0);
    
    for (const auto& category : m_manager->getAllCategories()) {
        int row = m_categoryTable->rowCount();
        m_categoryTable->insertRow(row);
        
        m_categoryTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(category.name)));
        m_categoryTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(category.description)));
    }
}

void MainWindow::CategoryDialog::addCategory()
{
    QString name = m_nameEdit->text().trimmed();
    QString description = m_descriptionEdit->text().trimmed();
    
    if (name.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Category name cannot be empty.");
        return;
    }
    
    Category category(name.toStdString(), description.toStdString());
    
    if (m_manager->addCategory(category)) {
        refreshCategories();
        m_nameEdit->clear();
        m_descriptionEdit->clear();
    } else {
        QMessageBox::critical(this, "Error", "Failed to add category. Name may already exist.");
    }
}

void MainWindow::CategoryDialog::updateCategory()
{
    QList<QTableWidgetItem*> selectedItems = m_categoryTable->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please select a category to update.");
        return;
    }
    
    int row = selectedItems.first()->row();
    QString originalName = m_categoryTable->item(row, 0)->text();
    
    QString newName = m_nameEdit->text().trimmed();
    QString description = m_descriptionEdit->text().trimmed();
    
    if (newName.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Category name cannot be empty.");
        return;
    }
    
    Category category(newName.toStdString(), description.toStdString());
    
    if (m_manager->updateCategory(originalName.toStdString(), category)) {
        refreshCategories();
        m_nameEdit->clear();
        m_descriptionEdit->clear();
    } else {
        QMessageBox::critical(this, "Error", "Failed to update category.");
    }
}

void MainWindow::CategoryDialog::deleteCategory()
{
    QList<QTableWidgetItem*> selectedItems = m_categoryTable->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please select a category to delete.");
        return;
    }
    
    int row = selectedItems.first()->row();
    QString name = m_categoryTable->item(row, 0)->text();
    
    if (QMessageBox::question(this, "Delete Category", 
                            "Are you sure you want to delete this category?\n"
                            "This will fail if any expenses are using this category.",
                            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        if (m_manager->deleteCategory(name.toStdString())) {
            refreshCategories();
            m_nameEdit->clear();
            m_descriptionEdit->clear();
        } else {
            QMessageBox::critical(this, "Error", 
                                "Failed to delete category. It may be in use by expenses.");
        }
    }
}

void MainWindow::CategoryDialog::selectCategory(int row, int column)
{
    Q_UNUSED(column);
    
    QString name = m_categoryTable->item(row, 0)->text();
    QString description = m_categoryTable->item(row, 1)->text();
    
    m_nameEdit->setText(name);
    m_descriptionEdit->setText(description);
}