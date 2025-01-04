#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QString>
#include <thread>
#include <iostream>
#include "client.h"

using namespace std;

void handleLog(QLineEdit* usernameField, QLineEdit* passwordField, QLabel* statusLabel, string option) {
    string username = usernameField->text().toStdString();
    string password = passwordField->text().toStdString();

    if (log(username, password, option)) {
        cout << "_____________________ udalo: " << endl;
        statusLabel->setText("Status: Zalogowany");
        QMessageBox::information(nullptr, "Sukces", "Zalogowano pomyślnie!");
    } else {
        cout << "_________________ dupa: " << endl;
        QMessageBox::critical(nullptr, "Błąd", "Niepoprawny login lub hasło.");
    }
}

int main(int argc, char *argv[]) {
    if(!connect()) return 0; 
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("SuperEkstraMail");

    QLabel *statusLabel = new QLabel("Status: Niepołączony");
    QLineEdit *usernameField = new QLineEdit;
    usernameField->setPlaceholderText("Podaj nazwę użytkownika");
    QLineEdit *passwordField = new QLineEdit;
    passwordField->setPlaceholderText("Podaj hasło");
    passwordField->setEchoMode(QLineEdit::Password);

    QPushButton *loginButton = new QPushButton("Zaloguj");
    QPushButton *registerButton = new QPushButton("Zarejestruj się");

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(statusLabel);
    layout->addWidget(usernameField);
    layout->addWidget(passwordField);
    layout->addWidget(loginButton);
    layout->addWidget(registerButton);
    window.setLayout(layout);

    QObject::connect(loginButton, &QPushButton::clicked, [&]() {
        handleLog(usernameField, passwordField, statusLabel, "zaloguj");
    });
    QObject::connect(registerButton, &QPushButton::clicked, [&]() {
        handleLog(usernameField, passwordField, statusLabel, "rejestruj");
    });

    window.show();
    return app.exec();
}
