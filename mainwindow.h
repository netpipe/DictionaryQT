#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void searchWord();

private:
    QSqlDatabase db;
    QLineEdit *searchInput;
    QTextEdit *resultDisplay;
    QPushButton *searchButton;

    bool connectToDatabase();
};

#endif // MAINWINDOW_H
