#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sqlite3.h>

using namespace std;

// Funkcja sprawdzająca dane logowania
bool authenticateUser(const string &login, const string &password) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc = sqlite3_open("users.db", &db);

    if (rc != SQLITE_OK) {
        cerr << "Cannot open database: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return false;
    }

    string sql = "SELECT * FROM users WHERE login = ? AND password = ?;";
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        cerr << "Failed to execute query: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return false;
    }

    sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

    bool isAuthenticated = sqlite3_step(stmt) == SQLITE_ROW;
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return isAuthenticated;
}

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        cerr << "Error creating socket\n";
        return 1;
    }

    sockaddr_in server_addr {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(1100);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Error binding socket\n";
        close(server_socket);
        return 1;
    }

    if (listen(server_socket, 5) < 0) {
        cerr << "Error listening on socket\n";
        close(server_socket);
        return 1;
    }

    cout << "Server listening on port 1100...\n";

    while (true) {
        int client_socket = accept(server_socket, nullptr, nullptr);
        if (client_socket < 0) {
            cerr << "Error accepting connection\n";
            continue;
        }

        char login[256], password[256];
        memset(login, 0, sizeof(login));
        memset(password, 0, sizeof(password));

        // Odbieranie loginu
        if (read(client_socket, login, sizeof(login)) <= 0) {
            cerr << "Error reading login\n";
            close(client_socket);
            continue;
        }

        // Odbieranie hasła
        if (read(client_socket, password, sizeof(password)) <= 0) {
            cerr << "Error reading password\n";
            close(client_socket);
            continue;
        }

        // Autoryzacja użytkownika
        if (authenticateUser(login, password)) {
            cout << "User authenticated: " << login << endl;
            write(client_socket, "Login successful", 17);

            char message[256];
            while (true) {
                memset(message, 0, sizeof(message));
                if (read(client_socket, message, sizeof(message)) <= 0) {
                    cerr << "Client disconnected\n";
                    break;
                }
                cout << "Message from " << login << ": " << message << endl;
                write(client_socket, "Message received", 17);
            }
        } else {
            cerr << "Authentication failed for user: " << login << endl;
            write(client_socket, "Login failed", 12);
        }

        close(client_socket);
    }

    close(server_socket);
    return 0;
}
