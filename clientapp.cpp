#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

void menu() {
    cout << "====== MENU ======" << endl;
    cout << "1. Register" << endl;
    cout << "2. Login" << endl;
    cout << "3. Exit" << endl;
    cout << "==================" << endl;
    cout << "Choose an option: ";
}

bool connectToServer(int &socketFD, const string &serverIP, int port) {
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(serverIP.c_str());

    socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD < 0) {
        cerr << "Error creating socket" << endl;
        return false;
    }

    if (connect(socketFD, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Error connecting to server" << endl;
        close(socketFD);
        return false;
    }

    cout << "Connected to server" << endl;
    return true;
}

bool registerUser(int socketFD) {
    string login, password;

    cout << "Enter login: ";
    cin >> login;
    cout << "Enter password: ";
    cin >> password;

    string message = "REGISTER " + login + " " + password;
    if (write(socketFD, message.c_str(), message.size()) <= 0) {
        cerr << "Error sending registration data" << endl;
        return false;
    }

    char response[256];
    memset(response, 0, sizeof(response));
    if (read(socketFD, response, sizeof(response)) <= 0) {
        cerr << "Error receiving response" << endl;
        return false;
    }

    cout << "Server response: " << response << endl;
    return strcmp(response, "Registration successful") == 0;
}

bool loginUser(int socketFD) {
    string login, password;

    cout << "Enter login: ";
    cin >> login;
    cout << "Enter password: ";
    cin >> password;

    string message = "LOGIN " + login + " " + password;
    if (write(socketFD, message.c_str(), message.size()) <= 0) {
        cerr << "Error sending login data" << endl;
        return false;
    }

    char response[256];
    memset(response, 0, sizeof(response));
    if (read(socketFD, response, sizeof(response)) <= 0) {
        cerr << "Error receiving response" << endl;
        return false;
    }

    cout << "Server response: " << response << endl;
    return strcmp(response, "Login successful") == 0;
}

void chatWithServer(int socketFD) {
    string message;
    char response[256];

    while (true) {
        cout << "Enter message (type 'exit' to quit): ";
        cin >> message;

        if (message == "exit") {
            cout << "Disconnecting..." << endl;
            break;
        }

        if (write(socketFD, message.c_str(), message.size()) <= 0) {
            cerr << "Error sending message" << endl;
            break;
        }

        memset(response, 0, sizeof(response));
        if (read(socketFD, response, sizeof(response)) <= 0) {
            cerr << "Error receiving response" << endl;
            break;
        }

        cout << "Server response: " << response << endl;
    }
}

int main() {
    const string serverIP = "127.0.0.1";
    const int serverPort = 1100;

    int option;

    while (true) {
        menu();
        cin >> option;

        if (option == 3) {
            cout << "Exiting..." << endl;
            break;
        }

        int socketFD;
        if (!connectToServer(socketFD, serverIP, serverPort)) {
            continue;
        }

        switch (option) {
            case 1:
                if (registerUser(socketFD)) {
                    cout << "Registration successful" << endl;
                } else {
                    cout << "Registration failed" << endl;
                }
                break;

            case 2:
                if (loginUser(socketFD)) {
                    cout << "Login successful" << endl;
                    chatWithServer(socketFD);
                } else {
                    cout << "Login failed" << endl;
                }
                break;

            default:
                cout << "Invalid option" << endl;
                break;
        }

        close(socketFD);
    }

    return 0;
}
