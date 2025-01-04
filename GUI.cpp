#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QString>
#include <QTextEdit>
#include <thread>
#include <iostream>
#include "client.h"

using namespace std;

void showMessageInterface(QWidget* mainWindow) {
    QWidget *messageWindow = new QWidget;
    messageWindow->setWindowTitle("SuperEkstraMail - Wiadomości");

    QVBoxLayout *layout = new QVBoxLayout;

    QLabel *statusLabel = new QLabel("Status: Połączono z serwerem");
    QTextEdit *messageBox = new QTextEdit;
    messageBox->setPlaceholderText("Wpisz wiadomość...");
    messageBox->setFixedHeight(100);
    
    QPushButton *sendButton = new QPushButton("Wyślij wiadomość");
    QPushButton *logoutButton = new QPushButton("Wyloguj się");

    layout->addWidget(statusLabel);
    layout->addWidget(messageBox);
    layout->addWidget(sendButton);
    layout->addWidget(logoutButton);
    messageWindow->setLayout(layout);

    // Obsługa przycisku "Wyślij wiadomość"
    QObject::connect(sendButton, &QPushButton::clicked, [&]() {
        string message = messageBox->toPlainText().toStdString();
        if (!message.empty()) {
            if (send()) {
                QMessageBox::information(nullptr, "Sukces", "Wiadomość wysłana!");
                messageBox->clear();
            } else {
                QMessageBox::critical(nullptr, "Błąd", "Nie udało się wysłać wiadomości.");
            }
        } else {
            QMessageBox::warning(nullptr, "Uwaga", "Pole wiadomości jest puste!");
        }
    });

    // Obsługa przycisku "Wyloguj się"
    QObject::connect(logoutButton, &QPushButton::clicked, [&]() {
        messageWindow->close();
        mainWindow->show();
    });

    mainWindow->hide();
    messageWindow->show();
}

void handleLog(QLineEdit* usernameField, QLineEdit* passwordField, QLabel* statusLabel, string option, QWidget* mainWindow) {
    string username = usernameField->text().toStdString();
    string password = passwordField->text().toStdString();

    if (log(username, password, option)) {
        cout << "_____________________ udalo: " << endl;
        statusLabel->setText("Status: Zalogowany");
        QMessageBox::information(nullptr, "Sukces", "Zalogowano pomyślnie!");
        showMessageInterface(mainWindow);
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
    window.resize(1200, 800);

    QFont font;
    font.setPointSize(20);
    QApplication::setFont(font);

    QLabel *statusLabel = new QLabel("Status: Niepołączony");
    QLineEdit *usernameField = new QLineEdit;
    usernameField->setPlaceholderText("Podaj nazwę użytkownika");
    QLineEdit *passwordField = new QLineEdit;
    passwordField->setPlaceholderText("Podaj hasło");
    passwordField->setEchoMode(QLineEdit::Password);

    QPushButton *loginButton = new QPushButton("Zaloguj");
    QPushButton *registerButton = new QPushButton("Zarejestruj się");

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(20);
    layout->addWidget(statusLabel);
    layout->addWidget(usernameField);
    layout->addWidget(passwordField);
    layout->addWidget(loginButton);
    layout->addWidget(registerButton);
    layout->setContentsMargins(20, 20, 20, 20);
    window.setLayout(layout);

    QObject::connect(loginButton, &QPushButton::clicked, [&]() {
        handleLog(usernameField, passwordField, statusLabel, "zaloguj", &window);
    });
    QObject::connect(registerButton, &QPushButton::clicked, [&]() {
        handleLog(usernameField, passwordField, statusLabel, "rejestruj", &window);
    });

    window.show();
    return app.exec();
}
