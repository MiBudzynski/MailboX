#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
    struct sockaddr_in server_addr; // Adresy internetowe protokołu

    std::memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(1111);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int my_socket = socket(AF_INET, SOCK_STREAM, 0); // Tworzenie gniazda
    if (my_socket < 0) {
        std::cerr << "Error creating socket\n";
        return 1;
    }

    if (bind(my_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Error binding socket\n";
        close(my_socket);
        return 1;
    }

    if (listen(my_socket, 5) < 0) {
        std::cerr << "Error listening on socket\n";
        close(my_socket);
        return 1;
    }

    int rcv_socket = accept(my_socket, nullptr, nullptr); // Nowe gniazdo do pobierania wiadomości
    if (rcv_socket < 0) {
        std::cerr << "Error accepting connection\n";
        close(my_socket);
        return 1;
    }

    char buff[256];
    char buff_rcv[256];

    while (true) {
        std::memset(buff, 0, 256);      // Zerowanie
        std::memset(buff_rcv, 0, 256);

        std::cin >> buff;
        if (std::strcmp(buff, "exit") == 0) {
            break;
        } else {
            if (write(rcv_socket, buff, std::strlen(buff)) < 0) {
                std::cerr << "Error writing to socket\n";
                break;
            }

            if (read(rcv_socket, buff_rcv, sizeof(buff_rcv)) < 0) {
                std::cerr << "Error reading from socket\n";
                break;
            }

            std::cout << "Received: " << buff_rcv << std::endl;
        }
    }

    close(rcv_socket);
    close(my_socket);

    return 0;
}

