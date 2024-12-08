#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h> 
#include <unistd.h>
#include <pthread.h>

using namespace std;

bool czyIstnieje(char login[256], char password[256]) {
    return 1;
}

void createAccaunt(char login[256], char password[256]) {
//ToDo
}

void *socketThread(void *arg)
{
    int newSocket = *((int *)arg);
    char option[256];
    memset(option, 0, sizeof(option));

    // wybor czy zakladanie konta czy logowanie do juz istniejacego
    if (read(newSocket, option, sizeof(option)) <= 0) {
        cerr << "Error reading login option\n";
        close(newSocket);
    }
    cout << "User chose option: " << option << std::endl;

    do{
        char login[256];
        char password[256];
        memset(login, 0, sizeof(login));
        memset(password, 0, sizeof(password));

        // Odbiór loginu
        if (read(newSocket, login, sizeof(login)) <= 0) {
            cerr << "Error reading login\n";
            close(newSocket);
            pthread_exit(NULL);
        }
        cout << "Received login: " << login << endl;

        // Odbiór hasła
        if (read(newSocket, password, sizeof(password)) <= 0) {
            cerr << "Error reading password\n";
            close(newSocket);
            pthread_exit(NULL);
        }
        cout << "Received password: " << password << endl;

        if(strcmp(option, "zaloguj") == 0){
            if(czyIstnieje(login, password) == 1){
                break;
            }
            else{
                write(newSocket, "Wrong username or password\n", 26);
            }
        }else{
            if(czyIstnieje(login, password) == 0){
                createAccaunt(login, password);
                break;
            }
            else{
                write(newSocket, "Username already taken\n", 26);
            }
        }
    }while(true);
    
    //pętla do dalszej komunikacji
    while(true){
        char buff[256];
        memset(buff, 0, sizeof(buff));

        if (read(newSocket, buff, sizeof(buff)) <= 0) {
            cerr << "Client disconnected or error reading\n";
            break;
        }
        cout << "Received message: " << buff << std::endl;

        // Wysyłanie odpowiedzi klientowi
        if (write(newSocket, "Message received\n", 16) <= 0) {
            cerr << "Error writing to client\n";
            break;
        }
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
    cout << "addr: " << INADDR_ANY<< endl;
    cout << "port: " << port << endl;

    int my_socket = socket(AF_INET, SOCK_STREAM, 0); 
    if (my_socket == -1) {
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

    pthread_t thread_id;
    // Pętla do dalszej komunikacji
    while (true) {
        int rcv_socket = accept(my_socket, nullptr, nullptr); 
        if (rcv_socket < 0) {
            cerr << "Error accepting connection\n";
            close(my_socket);
            return 1;
        }

        if( pthread_create(&thread_id, NULL, socketThread, &rcv_socket) != 0 )
           printf("Failed to create thread\n");

        pthread_detach(thread_id);
    }

    close(my_socket);

    return 0;
}

