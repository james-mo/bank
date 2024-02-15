#include <string>
#include <sqlite3.h>
#include <iostream>
#include "sql.hpp"
#include "util.hpp"

int exec_sql(std::string path, const char* sql) {
    sqlite3* db;
    if (!fileExists(path)) {
        std::cout << "Database does not exist" << std::endl;
        exit(0);
    }

    int rc = sqlite3_open(path.c_str(), &db);

    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        exit(0);
    } else {
        std::cout << "SQL.cpp : Opened database successfully" << std::endl;
    }
    char* zErrMsg = 0;

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        int res = sqlite3_step(stmt);
        while (res == SQLITE_ROW) {
            // print each column
            for (int i = 0; i < sqlite3_column_count(stmt); i++) {
                printf("%s: %s\n", sqlite3_column_name(stmt, i), sqlite3_column_text(stmt, i));
            }
            res = sqlite3_step(stmt);
        }
    }

    sqlite3_close(db);
    return 0;
}

// console app for sql commands

int main() {
    using std::cout;
    using std::cin;
    using std::string;

    string path = "accounts.db";

    while(not cin.eof()) {
        cout << "Enter SQL command >> ";
        string sql;
        getline(cin, sql);
        exec_sql(path, sql.c_str());
    }
}