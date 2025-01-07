#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <functional>

using namespace std;

extern function<void(const string&)> updateGuiCallback;

bool connect();
bool log(const string &username, const string &password, const string &option);
bool send(const string &reciver, const string &topic, const string &message);

#endif // CLIENT_H