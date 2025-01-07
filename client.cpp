#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <pthread.h>
#include <functional>
#include "client.h"

using namespace std;

struct sockaddr_in sa;
int SocketFD;

function<void(const std::string&)> updateGuiCallback;

void * recvThread(void *arg)
{
    char buff_rcv[256];
    memset(buff_rcv, 0, sizeof(buff_rcv));
    int threatSocketFD = *((int *)arg);
    while(true){
        if (read(threatSocketFD, buff_rcv, sizeof buff_rcv ) <= 0) {
            perror("Error receiving response");
            break;
        }
        string message(buff_rcv);
        cout << "message from serrver: " << buff_rcv << endl;
        if (updateGuiCallback) {
            updateGuiCallback(message);
        }
    }
    pthread_exit(NULL);
}

bool connect(){
    memset(&sa, 0, sizeof sa);
    int port = 1100;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = inet_addr("127.0.0.1");

    SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (SocketFD == -1) {
        perror("cannot create socket");
        return false;
    }
    if (connect(SocketFD, (struct sockaddr *)&sa, sizeof sa) == -1) {
        perror("connect failed");
        close(SocketFD);
        return false;
    }

    cout << "Connection accepted" << endl;
    return true;
}

bool log(const string &username, const string &password, const string &option){
    //przesylanie danych logowania
    if (write(SocketFD, option.c_str(), option.length()) <= 0) {
        perror("Error sending login option");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    usleep(100000);
    if (write(SocketFD, username.c_str(), username.length()) <= 0) {
        perror("Error sending login");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    usleep(100000);
    if (write(SocketFD, password.c_str(), password.length()) <= 0) {
        perror("Error sending password");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    //sprawdzanie czy zaakceptowano dane
    char rcv[256];
    memset(rcv, 0, 256);
    if (read(SocketFD, rcv, sizeof rcv ) <= 0) {
        perror("Error receiving response");
    }
    cout << "^^^^^^^^^^^^^^^" << rcv << endl;
    if(strcmp(rcv, "Accept") != 0) return false;

    //tworzenie watku do odbioru nowych wiadomosci
    pthread_t thread_id;
    updateGuiCallback = nullptr;
    if( pthread_create(&thread_id, NULL, recvThread, &SocketFD) != 0 )
        printf("Failed to create thread\n");
    pthread_detach(thread_id);

    return true;
}

bool send(const string &reciver, const string &topic, const string &message){
    //przesylanie danych wiadomosci
    if (write(SocketFD, reciver.c_str(), reciver.length()) <= 0) {
        perror("Error sending reciver");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    usleep(100000);
    if (write(SocketFD, topic.c_str(), topic.length()) <= 0) {
        perror("Error sending topic");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    usleep(100000);
    if (write(SocketFD, message.c_str(), message.length()) <= 0) {
        perror("Error sending message");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    return true;
}
