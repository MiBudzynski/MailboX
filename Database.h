#ifndef DATABASE_H
#define DATABASE_H

#include <string>

int createAccount(const std::string &username, const std::string &password);
bool czyIstnieje(const std::string &username, const std::string &password);
bool czyIstniejeUzytkownik(const std::string &username);
int create();

#endif // DATABASE_H