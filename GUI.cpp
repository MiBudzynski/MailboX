#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QTcpSocket>
#include <QString>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Tworzenie głównego okna
    QWidget window;
    window.setWindowTitle("Klient GUI");

    // Tworzenie elementów GUI
    QLabel *statusLabel = new QLabel("Status: Niepołączony");
    QLineEdit *usernameField = new QLineEdit;
    usernameField->setPlaceholderText("Podaj nazwę użytkownika");
    QLineEdit *passwordField = new QLineEdit;
    passwordField->setPlaceholderText("Podaj hasło");
    passwordField->setEchoMode(QLineEdit::Password);
    QPushButton *loginButton = new QPushButton("Zaloguj");
    QPushButton *registerButton = new QPushButton("Zarejestruj się");

    // Tworzenie połączenia z serwerem
    QTcpSocket *socket = new QTcpSocket;

    // Funkcja do aktualizacji statusu
    auto updateStatus = [&]() {
        if (socket->state() == QTcpSocket::ConnectedState) {
            statusLabel->setText("Status: Połączony");
        } else {
            statusLabel->setText("Status: Niepołączony");
        }
    };

    // Łączenie z serwerem
    socket->connectToHost("127.0.0.1", 1100);
    updateStatus();

    // Obsługa przycisków
    QObject::connect(loginButton, &QPushButton::clicked, [&]() {
        if (socket->state() == QTcpSocket::ConnectedState) {
            QString loginMessage = "zaloguj\n" + usernameField->text() + "\n" + passwordField->text();
            socket->write(loginMessage.toUtf8());
            socket->flush();
        } else {
            QMessageBox::warning(&window, "Błąd", "Nie połączono z serwerem");
        }
    });

    QObject::connect(registerButton, &QPushButton::clicked, [&]() {
        if (socket->state() == QTcpSocket::ConnectedState) {
            QString registerMessage = "zaloz\n" + usernameField->text() + "\n" + passwordField->text();
            socket->write(registerMessage.toUtf8());
            socket->flush();
        } else {
            QMessageBox::warning(&window, "Błąd", "Nie połączono z serwerem");
        }
    });

    // Obsługa odpowiedzi serwera
    QObject::connect(socket, &QTcpSocket::readyRead, [&]() {
        QString response = socket->readAll();
        QMessageBox::information(&window, "Odpowiedź serwera", response);
    });

    // Tworzenie układu i dodanie elementów
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(statusLabel);
    layout->addWidget(usernameField);
    layout->addWidget(passwordField);
    layout->addWidget(loginButton);
    layout->addWidget(registerButton);

    // Ustawienie układu w głównym oknie
    window.setLayout(layout);

    // Wyświetlenie okna
    window.show();

    return app.exec();
}
