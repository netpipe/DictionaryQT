#include "mainwindow.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QCoreApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    setWindowTitle("Dictionary Search");

    QVBoxLayout *layout = new QVBoxLayout(central);

    QLabel *label = new QLabel("Enter a word:", this);
    layout->addWidget(label);

    searchInput = new QLineEdit(this);
    layout->addWidget(searchInput);
    connect(searchInput, &QLineEdit::returnPressed, this, &MainWindow::searchWord);

    searchButton = new QPushButton("Search", this);
    layout->addWidget(searchButton);

    resultDisplay = new QTextEdit(this);
    resultDisplay->setReadOnly(true);
    layout->addWidget(resultDisplay);

    connect(searchButton, &QPushButton::clicked, this, &MainWindow::searchWord);

    if (!connectToDatabase()) {
        QMessageBox::critical(this, "Error", "Failed to connect to the database.");
    }
}

MainWindow::~MainWindow() {
    if (db.isOpen())
        db.close();
}

bool MainWindow::connectToDatabase() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QCoreApplication::applicationDirPath() +"/Dictionary.db"); // Path to your SQLite DB

    return db.open();
}
void MainWindow::searchWord() {
    QString word = searchInput->text().trimmed();

    if (word.isEmpty()) {
        resultDisplay->setText("Please enter a word.");
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT word, wordtype, definition FROM entries WHERE word = ?");
    query.addBindValue(word);

    if (!query.exec()) {
        resultDisplay->setText("Database query error: " + query.lastError().text());
        return;
    }

    QString resultText;
    while (query.next()) {
        resultText += "Word: " + query.value(0).toString() + "\n";
        resultText += "Type: " + query.value(1).toString() + "\n";
        resultText += "Definition: " + query.value(2).toString() + "\n\n";
    }

    if (!resultText.isEmpty()) {
        resultDisplay->setText(resultText);
        return;
    }

    // Fallback: get close matches using LIKE
    query.prepare("SELECT word, wordtype, definition FROM entries WHERE word LIKE ?");
    query.addBindValue("%" + word + "%");
    if (!query.exec()) {
        resultDisplay->setText("Database query error: " + query.lastError().text());
        return;
    }

    resultText = "No exact match found. Close matches:\n\n";
    bool hasResults = false;
    while (query.next()) {
        hasResults = true;
        resultText += "Word: " + query.value(0).toString() + "\n";
        resultText += "Type: " + query.value(1).toString() + "\n";
        resultText += "Definition: " + query.value(2).toString() + "\n\n";
    }

    if (!hasResults)
        resultText = "No matches found.";

    resultDisplay->setText(resultText);
}
