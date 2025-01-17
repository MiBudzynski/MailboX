#include <iostream>
#include <sqlite3.h>

void checkMessages(const std::string& sender, const std::string& receiver, const std::string& topic) {
    sqlite3 *db;
    if (sqlite3_open("MailBase.db", &db)) {
        return;
    }

    const char *selectSQL = "SELECT Message FROM Messages WHERE Sender = ? and Receiver = ? and Topic = ?;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, selectSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_text(stmt, 1, sender.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, receiver.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, topic.c_str(), -1, SQLITE_STATIC);

    std::string message;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        message = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
    }
    std::cout << "Message: " << message << std::endl;

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

int main() {
    std::string sender, receiver, topic;
    std::cout << "Podaj Sender: ";
    std::getline(std::cin, sender);
    std::cout << "Podaj Receiver: ";
    std::getline(std::cin, receiver);
    std::cout << "Podaj Topic: ";
    std::getline(std::cin, topic);

    checkMessages(sender, receiver, topic);

    return 0;
}
