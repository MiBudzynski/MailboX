#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

using namespace std;

void *socketThread(void *arg) {
    int newSocket = *((int *)arg);
    string messages[] = {"Hello, client!", "How are you?", "Goodbye!"};
    for (const string &msg : messages) {
        if (write(newSocket, msg.c_str(), msg.size() + 1) <= 0) {
            cerr << "Error writing to socket" << endl;
            break;
        }
        sleep(2); // Pauza między wiadomościami
    }
    close(newSocket);
    pthread_exit(NULL);
}

int main() {
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    int port = 1100;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        cerr << "Error creating socket\n";
        return 1;
    }

    if (bind(serverSocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Error binding socket\n";
        close(serverSocket);
        return 1;
    }

    if (listen(serverSocket, 5) < 0) {
        cerr << "Error listening on socket\n";
        close(serverSocket);
        return 1;
    }

    pthread_t thread_id;
    while (true) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket < 0) {
            cerr << "Error accepting connection\n";
            continue;
        }

        if (pthread_create(&thread_id, NULL, socketThread, &clientSocket) != 0) {
            cerr << "Failed to create thread\n";
            close(clientSocket);
        }

        pthread_detach(thread_id);
    }

    close(serverSocket);
    return 0;
}
