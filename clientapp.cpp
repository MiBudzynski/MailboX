#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>

using namespace std;

void clearBuffer(char *buffer, size_t size) {
    memset(buffer, 0, size);
}

void createAccount() {
    char name[256], surname[256], login[256], password[256];
    cout << "Podaj imię: ";
    cin >> name;
    cout << "Podaj nazwisko: ";
    cin >> surname;
    cout << "Podaj login: ";
    cin >> login;
    cout << "Podaj hasło: ";
    cin >> password;

    // Tutaj można dodać zapisywanie danych lokalnie lub wysłanie do serwera rejestracji (jeśli to konieczne).
    cout << "Konto zostało założone. Możesz teraz się zalogować.\n";
}

bool login(int &SocketFD, struct sockaddr_in &sa) {
    char login[256], password[256];
    cout << "Podaj login: ";
    cin >> login;
    cout << "Podaj hasło: ";
    cin >> password;

    // Tworzenie gniazda
    SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (SocketFD == -1) {
        perror("cannot create socket");
        exit(EXIT_FAILURE);
    }

    // Próba połączenia z serwerem
    if (connect(SocketFD, (struct sockaddr *)&sa, sizeof sa) == -1) {
        perror("connect failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    // Wysyłanie danych logowania do serwera
    string loginData = string("LOGIN ") + login + " " + password;
    if (write(SocketFD, loginData.c_str(), loginData.size()) <= 0) {
        perror("Error sending login data");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    char response[256];
    clearBuffer(response, sizeof(response));
    if (read(SocketFD, response, sizeof(response)) <= 0) {
        perror("Error receiving response");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    string serverResponse = response;
    if (serverResponse == "SUCCESS") {
        cout << "Logowanie powiodło się.\n";
        return true;
    } else {
        cout << "Błąd logowania: " << serverResponse << endl;
        close(SocketFD); // Zamknięcie połączenia po nieudanym logowaniu
        return false;
    }
}

int main(int argc, char *argv[]) {
    struct sockaddr_in sa;
    int SocketFD;
    int port = 1100;

    // Konfiguracja serwera
    memset(&sa, 0, sizeof sa);
    sa.sin_addr.s_addr = inet_addr("127.0.0.1");
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);

    bool loggedIn = false;
    while (true) {
        cout << "\nMenu:\n";
        cout << "1. Załóż konto\n";
        cout << "2. Zaloguj się\n";
        cout << "3. Wyjdź\n";
        cout << "Wybierz opcję: ";
        int choice;
        cin >> choice;

        switch (choice) {
            case 1:
                createAccount();
                break;
            case 2:
                loggedIn = login(SocketFD, sa);
                if (loggedIn) {
                    cout << "Przechodzenie do dalszej komunikacji...\n";
                    goto startCommunication;
                }
                break;
            case 3:
                cout << "Zamykanie programu.\n";
                exit(EXIT_SUCCESS);
            default:
                cout << "Nieprawidłowa opcja, spróbuj ponownie.\n";
        }
    }

startCommunication:
    // Dalsza komunikacja po zalogowaniu
    char buff[256];
    char buff_rcv[256];

    while (true) {
        cout << "Enter message (type 'exit' to quit): ";
        cin >> buff;

        if (strcmp(buff, "exit") == 0) {
            cout << "Zamykanie połączenia.\n";
            break;
        }

        if (write(SocketFD, buff, sizeof(buff)) <= 0) {
            perror("Error sending message");
            break;
        }

        clearBuffer(buff_rcv, sizeof(buff_rcv));
        if (read(SocketFD, buff_rcv, sizeof(buff_rcv)) <= 0) {
            perror("Error receiving response");
            break;
        }
        cout << "Server response: " << buff_rcv << endl;
    }

    close(SocketFD);
    return EXIT_SUCCESS;
}
