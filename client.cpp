#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <pthread.h>
#include "client.h"

using namespace std;

struct sockaddr_in sa;
int SocketFD;

void * recvThread(void *arg)
{
    char buff_rcv[256];
    memset(buff_rcv, 0, sizeof(buff_rcv));
    int SocketFD = *((int *)arg);
    while(true){
        if (read(SocketFD, buff_rcv, sizeof buff_rcv ) <= 0) {
            perror("Error receiving response");
            break;
        }
        cout << "Server response: " << buff_rcv << endl;
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
    char rcv[256];
    memset(rcv, 0, 256);
    if (read(SocketFD, rcv, sizeof rcv ) <= 0) {
        perror("Error receiving response");
    }
    cout << "^^^^^^^^^^^^^^^" << rcv << endl;
    if(strcmp(rcv, "Accept") != 0) return false;

    pthread_t thread_id;
    if( pthread_create(&thread_id, NULL, recvThread, &SocketFD) != 0 )
        printf("Failed to create thread\n");
    pthread_detach(thread_id);
    return true;
}

bool send(){
    char buff[256];
    // Pętla do dalszej komunikacji
    while (true) {
        memset(buff, 0, sizeof(buff));
        cout << "Enter message: ";
        cin.getline(buff, sizeof(buff)); 
        if (strcmp(buff, "exit") == 0) {
            break;
        }
        if (write(SocketFD, buff, sizeof buff) <= 0) {
            perror("Error sending message");
            break;
        }
    }

    close(SocketFD);
    return EXIT_SUCCESS;
}