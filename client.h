#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <functional>

extern std::function<void(const std::string&)> updateGuiCallback;

bool connect();
bool log(const std::string &username, const std::string &password, const std::string &option);
bool send(const string &reciver, const string &topic, const string &message);

#endif // CLIENT_H