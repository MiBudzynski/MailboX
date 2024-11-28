#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>

using namespace std;

int main(int argc, char *argv[])
{
    struct sockaddr_in sa;
    int SocketFD;
    int port = 1100; // atoi(argv[2]);
    cout << "addr: " << argv[1] << endl;
    cout << "port: " << port << endl;
    
    SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (SocketFD == -1) {
        perror("cannot create socket");
        exit(EXIT_FAILURE);
    }

    memset(&sa, 0, sizeof sa);

    string login;
    string password;

    sa.sin_addr.s_addr = inet_addr(argv[1]);
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);

    cout << "Podaj login: " << endl;
    getline(cin, login); // fgets(login, sizeof login, stdin);
    cout << "Podaj hasło: " << endl;
    getline(cin, password); // fgets(password, sizeof password, stdin);
    

    if (connect(SocketFD, (struct sockaddr *)&sa, sizeof sa) == -1) {
        perror("connect failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    else {
        cout << "Connection accepted" << endl;
    }

    char buff[256];
    char buff_rcv[256];

    while (true) {
        memset(buff, 0, sizeof(buff));
        memset(buff_rcv, 0, sizeof(buff_rcv));

        // Wysyłanie loginu
        if (write(SocketFD, login.c_str(), login.size() + 1) <= 0) {
            perror("Error sending login");
            close(SocketFD);
            exit(EXIT_FAILURE);
        }

        // Wysyłanie hasła
        if (write(SocketFD, password.c_str(), password.size() + 1) <= 0) {
            perror("Error sending password");
            close(SocketFD);
            exit(EXIT_FAILURE);
        }
    }

    close(SocketFD);
    return EXIT_SUCCESS;
}
