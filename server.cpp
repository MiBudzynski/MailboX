#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

int main() {
    struct sockaddr_in server_addr; // Adresy internetowe protokołu

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(1100);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int my_socket = socket(AF_INET, SOCK_STREAM, 0); // Tworzenie gniazda
    if (my_socket < 0) {
        cerr << "Error creating socket\n";
        return 1;
    }

    if (bind(my_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Error binding socket\n";
        close(my_socket);
        return 1;
    }

    if (listen(my_socket, 5) < 0) {
        cerr << "Error listening on socket\n";
        close(my_socket);
        return 1;
    }

    int rcv_socket = accept(my_socket, nullptr, nullptr); // Nowe gniazdo do pobierania wiadomości
    if (rcv_socket < 0) {
        cerr << "Error accepting connection\n";
        close(my_socket);
        return 1;
    }

    char buff[256];
    char buff_rcv[256];

    char login[256];
    char password[256];


    while (true) {
        memset(login, 0, 256);      
        memset(password, 0, 256);

        cin >> buff;
        if (strcmp(buff, "exit") == 0) {
            break;
        } else {
            
        // Odbiór loginu
        if (read(rcv_socket, login, sizeof(login)) <= 0) {
            cerr << "Error reading login\n";
            close(rcv_socket);
            close(my_socket);
            return 1;
        }
        cout << "Received login: " << login << std::endl;

        // Odbiór hasła
        if (read(rcv_socket, password, sizeof(password)) <= 0) {
            cerr << "Error reading password\n";
            close(rcv_socket);
            close(my_socket);
            return 1;
        }
        cout << "Received password: " << password << std::endl;
        }
    }

    close(rcv_socket);
    close(my_socket);

    return 0;
}

