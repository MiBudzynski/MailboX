#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QString>
#include <QTextEdit>
#include <QFont>
#include <QFontDatabase>
#include <thread>
#include <iostream>
#include "client.h"

using namespace std;

void showSendInterface(QWidget* window);

void showMessageInterface(QWidget* window) {
    QWidget *messageWindow = new QWidget;
    messageWindow->setWindowTitle("SuperEkstraMail - Wiadomości");
    messageWindow->resize(1920, 1080);

    QPushButton *sendButton = new QPushButton("Napisz wiadomosc");

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(sendButton);
    messageWindow->setLayout(layout);

    QObject::connect(sendButton, &QPushButton::clicked, [messageWindow]() {
        showSendInterface(messageWindow);
    });

    messageWindow->show();
    window->close();
}

void showSendInterface(QWidget* window) {
    QWidget *sendWindow = new QWidget;
    sendWindow->setWindowTitle("SuperEkstraMail - Wysylanie");
    sendWindow->resize(1920, 1080);

    QFont font;
    font.setPointSize(20);
    QApplication::setFont(font);
    QLineEdit *reciverField = new QLineEdit;
    reciverField->setPlaceholderText("Podaj odbiorce");
    QLineEdit *topicField = new QLineEdit;
    topicField->setPlaceholderText("Podaj temat");
    QTextEdit *messageField = new QTextEdit;
    messageField->setPlaceholderText("Napisz wiadomość...");
    messageField->setFixedHeight(400);
    QPushButton *sendButton = new QPushButton("Wyslij");
    QPushButton *backButton = new QPushButton("Wroc do skrzynki odbiorczej");

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(20);
    layout->addWidget(reciverField);
    layout->addWidget(topicField);
    layout->addWidget(messageField);
    layout->addWidget(sendButton);
    layout->addWidget(backButton);
    layout->setContentsMargins(20, 20, 20, 20);
    sendWindow->setLayout(layout);

    QObject::connect(sendButton, &QPushButton::clicked, [sendWindow]() {
        send();
    });

    QObject::connect(backButton, &QPushButton::clicked, [sendWindow]() {
        showMessageInterface(sendWindow);
    });

    sendWindow->show();
    window->close();
}

void handleLog(QLineEdit* usernameField, QLineEdit* passwordField, QLabel* statusLabel, string option, QWidget* window) {
    string username = usernameField->text().toStdString();
    string password = passwordField->text().toStdString();

    if (log(username, password, option)) {
        cout << "_____________________ udalo: " << endl;
        statusLabel->setText("Status: Zalogowany");
        QMessageBox::information(nullptr, "Sukces", "Zalogowano pomyślnie!");
        showMessageInterface(window);
    } else if(option == "zaloguj"){
        cout << "_________________ dupa: " << endl;
        QMessageBox::critical(nullptr, "Błąd", "Niepoprawny login lub hasło.");
    } else{
        cout << "_________________ dupa: " << endl;
        QMessageBox::critical(nullptr, "Błąd", "Nazwa uzytkownika juz zajeta");
    }
}

int main(int argc, char *argv[]) {
    if(!connect()) return 0; 
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("SuperEkstraMail");
    window.resize(1920, 1080);

    int fontId = QFontDatabase::addApplicationFont("Monoton-Regular.ttf");
    QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
    QLabel *titleLabel = new QLabel("Super\nEkstra\nMail");
    QFont titleFont(fontFamily, 100);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);

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
    layout->addWidget(titleLabel);
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
