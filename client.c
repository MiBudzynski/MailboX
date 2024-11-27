#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    struct sockaddr_in sa;
    int SocketFD;
    int port=1100;//atoi(argv[2]);
    printf ("addr: %s\n",argv[1]);
    printf ("port: %i\n",port);
    SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (SocketFD == -1) {
        perror("cannot create socket");
        exit(EXIT_FAILURE);
    }

    memset(&sa, 0, sizeof sa);

    char login[256];
    bzero(login, 256);
    char password[256];
    bzero(password);

    sa.sin_addr.s_addr=inet_addr(argv[1]);
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);

    printf("Podaj login: \n");
    fgets(login, sizeof login, stdin);
    printf("Podaj hasło:\n");
    fgets(password, sizeof password, stdin);

    if (connect(SocketFD, (struct sockaddr *)&sa, sizeof sa) == -1) {
        perror("connect failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Connection accepted \n");
    }
    char buff[256];
    char buff_rcv[256];


    while(1)
    {
        bzero(buff, 256);
        bzero(buff_rcv,256);


        if(read(SocketFD,buff_rcv,sizeof buff_rcv) == 0)
        {
            break;
        }
        else
        {
            printf("recived :%s \n", buff_rcv);
            scanf("%s",buff);
            write(SocketFD,buff,sizeof buff);
        }
    }
    close(SocketFD);
    return EXIT_SUCCESS;
}

