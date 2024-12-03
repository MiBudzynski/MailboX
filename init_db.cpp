#include <iostream>
#include <sqlite3.h>

using namespace std;

void initializeDatabase() {
    sqlite3 *db;
    char *errMsg = nullptr;

    // Otwórz (lub utwórz) bazę danych
    int rc = sqlite3_open("users.db", &db);
    if (rc != SQLITE_OK) {
        cerr << "Cannot open database: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        exit(1);
    }

    // Tworzenie tabeli użytkowników
    const char *sql = "CREATE TABLE IF NOT EXISTS users ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "login TEXT UNIQUE, "
                      "password TEXT);";

    rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "Failed to create table: " << errMsg << endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        exit(1);
    }

    sqlite3_close(db);
    cout << "Database initialized successfully." << endl;
}

int main() {
    initializeDatabase();
    return 0;
}
