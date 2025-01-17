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
#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QString>
#include <QMetaObject>
#include <QMessageBox>

using namespace std;

struct sockaddr_in sa;
int SocketFD;

function<void(const std::string&)> updateGuiCallback;

void *recvThread(void *arg) {
    char buff_rcv[256];
    memset(buff_rcv, 0, sizeof(buff_rcv));
    int threadSocketFD = *((int *)arg);
    while (true) {
        if (read(threadSocketFD, buff_rcv, sizeof buff_rcv) <= 0) {
            perror("Error receiving response");
            break;
        }
        string message(buff_rcv);
        cout << "Message from server: " << buff_rcv << endl;
        if (updateGuiCallback) {
            updateGuiCallback(message);
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    memset(&sa, 0, sizeof sa);
    int port = 1100;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = inet_addr("127.0.0.1");

    SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (SocketFD == -1) {
        perror("Cannot create socket");
        return 1;
    }
    if (connect(SocketFD, (struct sockaddr *)&sa, sizeof sa) == -1) {
        perror("Connect failed");
        close(SocketFD);
        return 1;
    }

    cout << "Connection accepted" << endl;

    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, recvThread, &SocketFD) != 0) {
        cerr << "Failed to create thread\n";
        close(SocketFD);
        return 1;
    }
    pthread_detach(thread_id);

    QApplication app(argc, argv);

    QWidget *messageWindow = new QWidget;
    messageWindow->setWindowTitle("SuperEkstraMail - Wiadomości");
    messageWindow->resize(800, 600);

    QVBoxLayout *layout = new QVBoxLayout;

    // Callback do aktualizacji GUI
    updateGuiCallback = [layout](const std::string &message) {
        QMetaObject::invokeMethod(layout, [layout, message]() {
            QPushButton *messageButton = new QPushButton(QString::fromStdString(message));
            QObject::connect(messageButton, &QPushButton::clicked, [message]() {
                QMessageBox::information(nullptr, "Wiadomość", QString::fromStdString(message));
            });
            layout->addWidget(messageButton);
        });
    };

    QPushButton *sendButton = new QPushButton("Napisz wiadomość");
    layout->addWidget(sendButton);
    QObject::connect(sendButton, &QPushButton::clicked, []() {
        QMessageBox::information(nullptr, "Informacja", "Przycisk jeszcze nie obsługiwany!");
    });

    messageWindow->setLayout(layout);

    QObject::connect(&app, &QApplication::aboutToQuit, []() {
        close(SocketFD);
    });

    messageWindow->show();

    return app.exec();
}
