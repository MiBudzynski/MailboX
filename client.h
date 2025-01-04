#ifndef CLIENT_H
#define CLIENT_H

#include <string>

bool connect();
bool log(const std::string &username, const std::string &password, const std::string &option);
bool send();

#endif // CLIENT_H